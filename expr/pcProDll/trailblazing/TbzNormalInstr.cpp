/*****************************************************************\
*                                                                *
*  Copyright (C) Regents of University of California, 2003       *
*  This source code is a part of EXPRESSION project and is       *
*  copyrighted by the Regents of the University of California,   *
*  Irvine.                                                       *
*  The authors hereby grant permission to use this description   *
*  and its documentation for educational and non-commercial      *
*  purpose under the BSD license                                 *
*  (http://www.cecs.uci.edu/~express/BSD_License.txt). 	         *
*  The source code comes with no warranty and no author or       *
*  distributor accepts any responsibility for the consequences   *
*  of its use. Permission is granted to distribute this file in  *
*  compiled or executable form under the same conditions that    *
*  apply for source code. Permission is granted	to anyone to     *
*  make or distribute copies of this source code, either as      *
*  received or modified, in any medium, provided that all        *
*  copyright notices, permission and non warranty notices are    *
*  preserved, and that the distributor grants the recipient      *
*  permission for further redistribution as permitted by this    *
*  document. No written agreement, license, or royalty fee is    *
*  required for authorized use of this software.                 *
*                                                                *
*******************************************************************/
// $Id: NormalInstr.cc,v 1.43 1998/04/15 18:02:06 pgrun Exp $
//
// File:  		TbzNormalInstr.cc
// Created:		Wed Nov 20, 96
// Last modified: 	Wed Nov 20, 96
// Author:		G. Grun
// Email:		pgrun@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: INSTRUCTION

#include "stdafx.h"

#include "IburgToIR.h"

#include "Routine.h"
#include "GlobalIncludes.h"

#include "NormalInstr.h"
#include "JumpOp.h"
#include "Instruction.h"
#include "dependency.h"
#include "BasicBlockNode.h"
#include "BaseHTGNode.h"
#include "SimpleHTGNode.h"
#include "IfHTGNode.h"
#include "ComputeOp.h"
#include "PropertyIncludes.h"
#include "UDProperty.h"
#include "DUProperty.h"
#include "BaseInstruction.h"
#include "MemoryOp.h"
#include "Instruction.h"
#include "RenameTable.h"
#include "UDDU.h"
#include "ReservationTables.h"
#include "../DebugSwitches.h"
#include "BaseOperation.h"
#include "BBIterator.h"
#include "ScheduleOpProperty.h"
#include "BaseArgProperty.h"
#include "RegArchModel.h"
#include "RegClassArgProperty.h"

extern int hasScheduleOpProperty(BaseOperation *oper, int flag);
extern int hasScheduleInstrProperty(BaseInstruction *instr, int flag);
extern int getScheduleOpPropertyCode(BaseOperation *oper, int flg);
extern int getScheduleInstrPropertyCode(BaseInstruction *instr, int flg);
extern BaseInstruction *findNextInstr(BasicBlockNode* bb, BaseInstruction* instr);

//this should be used with caution (not tested)
//#define _FAST_TBZ_
#ifdef _FAST_TBZ_
int cannotMoveBeyondNonPipelined(NormalInstr *instr,BaseOperation *oper,MyLinkedList<BaseOperation *> &moves);
#endif

extern int insideTbz;
extern int stalledTbz;
//extern int outOfOrderCompletion;
//extern int missTime;
extern RegArchModel globalRegModel;

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 


int tbzDebug=1;
int debugUnifOps=1;


// By SAH on 02/11/02 : Note to Gaby: Change this to extern int.
//
int LastDependency = 0;

//if 1, pipelined tbz is performed, if 0 non-pipelined
//(default 0, can be changed from the command line arguments)
int pipelinedTbz = 0;
//if 1, stalled tbz is performed (after normal Tbz, stalls or nops are inserted to account for hazards in the pipeline) 
//(default 0, can be changed from the command line arguments)
int stalledTbz = 0;

static NormalInstr *_lastCanMove=NULL;

//the list of migrated operation copies (the new operation copies)
vector<BaseOperation*> migrated_operation_copies;

//the mapping of slots to regsiter classes
map<int,char *> data_regclass_mappings;
map<int,char *> data_regtype_mappings;

//prototypes:
void UpdateLive(BaseOperation *oper,vector<BaseHTGNode *> *path);
void deleteUDDU(BaseOperation *oper);


//the list of elemnts to delete from all the unifiable ops in the trailblazing pass
MyLinkedList<UnifiableOp*> _toDeleteUnifOps; 

//This function returns an array of pointers to the operations preceding
//the flow ops in "this" pointing to "instr"
Array<BaseOpSlot*> & NormalInstr::getReachingFlowOp(BaseInstruction *instr){
	Array<BaseOpSlot*> *ops;
	ArrayIterator<FlowOpSlot> *flowIter;
	ArrayIterator<DataOpSlot> *dataIter;
	ArrayIterator<ControlOpSlot> *controlIter;
	
	flowIter = _flowOpList.elements();
	
	ops=new Array<BaseOpSlot*>(0,1);
	
	while (flowIter->hasMoreElements()){
		if((flowIter->currentElement()).hasOper() && 
		flowIter->currentElement().getPtrToOperation()->isJumpOp() /*&&
		flowIter->currentElement().getPtrToOperation()->getTargetInstr()==instr*/){
		dataIter = _dataOpList.elements();
		while (dataIter->hasMoreElements()){
			if((dataIter->currentElement()).hasOper() && 
				dataIter->currentElement().getPtrToNextSlot()==
				&(flowIter->currentElement())){
				ops->appendElement(&(dataIter->currentElement()));
			}
			dataIter->nextElement();
		}
		delete dataIter;
		
		controlIter = _controlOpList.elements();
		while (controlIter->hasMoreElements()){
			if((controlIter->currentElement()).hasOper() && 
				controlIter->currentElement().getPtrToNextSlot()==
				&(flowIter->currentElement())){
				ops->appendElement(&(controlIter->currentElement()));
			}
			controlIter->nextElement();
		}
		delete controlIter;
		}
		flowIter->nextElement();
	}
	delete flowIter;
	
	return *ops;
}


//the data slot priorities (first are higher priority slots, e.g., AU1, AU2 in ST100)
vector<int> data_slot_priority;

//chooses an available slot for operation o in instruction this, 
//from a list of available slots
BaseOpSlot * NormalInstr::getAvailSlot(BaseOperation *o){
	Vector<int> indexes;
	SlotType optype;

	if(o->isDataOp()){
		optype=DATA_OP_SLOT;
	}
	else if(o->isControlOp()){
		optype=CONTROL_OP_SLOT;
	}
	else
		_ASSERT(0);

	
	if (o == NULL){
		cout << "Expected non null operation" << endl;
		abort();
	}
	
	simpleVLIWInstrTemplate->getSlotListForOper(o, indexes);
	
	
	if(o->isDataOp()){
		/*
		//if o is a data operation
		//for all the slots in the data_slots_priority, in order of the priority
		for(j=data_slot_priority.begin();j!=data_slot_priority.end();j++){
			//for all slots from indexes (choose them in the order of their priority)
			for (int i = 0; i < indexes.size(); i++){
				//if this is the current priority slot selected
				if(indexes[i]==(*j)){
					if(indexes[i]<0) abort();
					//if the current slot has no operation in it
					if(!_dataOpList.elementAt(indexes[i]).hasOper()){
						//if the pipeline is free for this operation in this slot
						if(PipelineFree(this,o,indexes[i],optype))
							//choose this slot 
							return &(_dataOpList.elementAt(indexes[i]));
					}
				}
			}
		}
		*/
		//if o is a data operation
		//for all slots from indexes
		for (int i = 0; i < indexes.size(); i++){
			if(indexes[i]<0) abort();
			//if the current slot has no operation in it
			if(!_dataOpList.elementAt(indexes[i]).hasOper()){
				//if the pipeline is free for this operation in this slot
				if(PipelineFree(this,o,indexes[i],optype))
					//choose this slot 
					return &(_dataOpList.elementAt(indexes[i]));
			}
		}
	}
	else if(o->isControlOp()){
		//if o is a control operation
		//for all slots from indexes
		for (int i = 0; i < indexes.size(); i++){
			//if the current slot has no operation in it
			if(!_controlOpList.elementAt(indexes[i]).hasOper())
				//choose this slot
				return &(_controlOpList.elementAt(indexes[i]));
		}
	}
	else if(o->isControlOp()){
		//if o is a control operation
		//for all slots from indexes
		for (int i = 0; i < indexes.size(); i++){
			//if the current slot has no operation in it
			if(!_controlOpList.elementAt(indexes[i]).hasOper())
				//choose this slot
				return &(_controlOpList.elementAt(indexes[i]));
		}
	}
	else if(o->isFlowOp()){
		for (int i = 0; i < indexes.size(); i++){
			if(!_flowOpList.elementAt(indexes[i]).hasOper())
				return &(_flowOpList.elementAt(indexes[i]));
		}
	}
	return NULL;
}

void moveOperation(NormalInstr *src, BaseOpSlot *o, NormalInstr *dst){
	Array<BaseOpSlot*> *precSlots;
	ArrayIterator<BaseOpSlot*> *iter;
	DataOpSlot *availSlot;
	precSlots=&(dst->getReachingFlowOp(src));
	availSlot=(DataOpSlot*)(dst->getAvailSlot(o->getPtrToOperation()));
	if(availSlot!=NULL){
		availSlot->setOperation(o->getPtrToOperation());
		((DataOpSlot*)o)->resetOperation();
		iter=precSlots->elements();
		while(iter->hasMoreElements()){
			availSlot->setNext((iter->currentElement())->getPtrToNextSlot());
			(iter->currentElement())->setNext(availSlot);
			iter->nextElement();
		}
	}
}

void replicateIList(BaseInstruction *from,BaseInstruction *to,
                    BaseInstruction *&head,BaseInstruction *&tail){
	BaseInstruction *crt,*instr,*prev;
	if(from==to){
		tail=head=NULL;
		return;
	}
	prev=head=from->copy();
	for(crt=from;crt!=to;crt=crt->getFirstSuccOnly()){
		instr=crt->copy();
		prev->setTargetInstr(instr);
		prev=instr;
	}
	tail=prev;
}

void moveConditional(NormalInstr *src, BaseOpSlot *o, NormalInstr *dst){
	if(!(((ControlOpSlot*)o)->getPtrToTrueBranch())->isFlowOpSlot() ||
		!(((ControlOpSlot*)o)->getPtrToFalseBranch())->isFlowOpSlot()){
		return;
		printf("Have to split first");
		assert(0);
	}
	
	Array<BaseOpSlot*> *precSlots;
	ArrayIterator<BaseOpSlot*> *iter;
	ControlOpSlot *availSlot;
	FlowOpSlot*trueSlot,*falseSlot;
	BaseInstruction *succ;
	
	succ=dst->getFirstSuccOnly();
	precSlots=&(dst->getReachingFlowOp(src));
	availSlot=(ControlOpSlot*)(dst->getAvailSlot(o->getPtrToOperation()));
	trueSlot=(FlowOpSlot*)(dst->getAvailSlot((o->getPtrToTrueBranch())->getPtrToOperation()));
	trueSlot->addOperation((o->getPtrToTrueBranch())->getPtrToOperation());
	if(trueSlot!=NULL){
		falseSlot=(FlowOpSlot*)(dst->getAvailSlot((o->getPtrToFalseBranch())->getPtrToOperation()));
		if(availSlot!=NULL && trueSlot!=NULL && falseSlot!=NULL){
			availSlot->addOperation(o->getPtrToOperation());
			falseSlot->addOperation((o->getPtrToFalseBranch())->getPtrToOperation());
			(((FlowOpSlot*)falseSlot)->getPtrToOperation())->setTargetInstr(succ);
			availSlot->setTrueBranch(trueSlot);
			availSlot->setFalseBranch(falseSlot);
			(o->getPtrToTrueBranch())->deleteOperation();
			(o->getPtrToFalseBranch())->deleteOperation();
			o->deleteOperation();
			//TBD: if src has no more ops, remove it
			iter=precSlots->elements();
			while(iter->hasMoreElements()){
				(iter->currentElement())->setNext(availSlot);
				iter->nextElement();
			}
			//duplicate the true branch instrs and link them
			BaseInstruction *head=NULL,*tail=NULL;
			replicateIList(succ,src,head,tail);
			if(head!=NULL){
				tail->setTargetInstr((trueSlot->getPtrToOperation())->getTargetInstr());
				(trueSlot->getPtrToOperation())->setTargetInstr(head);
			}
		}
	}
}

int containsConflictingMove(MyLinkedList<BaseOperation*> &moves,BaseOperation *oper){
	MyLinkedListIterator<BaseOperation *> *iter;
	iter=moves.elementsForward();
	while(iter->hasMoreElements()){
		if(iter->currentElement()->ptrToOperand(_DEST_, _DEST_LIST_)==oper->ptrToOperand(_DEST_, _DEST_LIST_) &&
			iter->currentElement()->ptrToOperand(_SOURCE_1_)!=oper->ptrToOperand(_SOURCE_1_)){
			delete iter;
			return 1;
		}
		iter->nextElement();
	}
	delete iter;
	return 0;
}

int NormalInstr::containsOpNotMove(BaseOperation *op,MyLinkedList<BaseOperation*> &moves){
	ArrayIterator<DataOpSlot> *diter;
	ArrayIterator<ControlOpSlot> *citer;
	int contains=0;
	BaseOperation *oper;
	diter=_dataOpList.elements();
	while(diter->hasMoreElements() && contains!=1){
		if(diter->currentElement().hasOper() && 
			(diter->currentElement()).getPtrToOperation()==op){
			if(diter->currentElement().getPtrToOperation()->isMoveOp() && !containsConflictingMove(moves,diter->currentElement().getPtrToOperation())){
				oper=diter->currentElement().getPtrToOperation();
				moves.append(oper);
			}
			else{
				contains=1;
			}
		}
		diter->nextElement();
	}
	citer=_controlOpList.elements();
	while(citer->hasMoreElements() && contains!=1){
		if(citer->currentElement().hasOper() && 
			(citer->currentElement()).getPtrToOperation()==op)
			contains=1;
		citer->nextElement();
	}
	delete diter;
	delete citer;
	return contains;
}

int NormalInstr::containsOp(BaseOperation *op){
	ArrayIterator<DataOpSlot> *diter;
	ArrayIterator<ControlOpSlot> *citer;
	ArrayIterator<FlowOpSlot> *fiter;
	int contains=0;
	diter=_dataOpList.elements();
	while(diter->hasMoreElements() && contains!=1){
		if(diter->currentElement().hasOper() && 
			(diter->currentElement()).getPtrToOperation()==op){
			contains=1;
		}
		diter->nextElement();
	}
	citer=_controlOpList.elements();
	while(citer->hasMoreElements() && contains!=1){
		if(citer->currentElement().hasOper() && 
			(citer->currentElement()).getPtrToOperation()==op)
			contains=1;
		citer->nextElement();
	}
	fiter=_flowOpList.elements();
	while(fiter->hasMoreElements() && contains!=1){
		if(fiter->currentElement().hasOper() && 
			(fiter->currentElement()).getPtrToOperation()==op)
			contains=1;
		fiter->nextElement();
	}
	delete diter;
	delete citer;
	delete fiter;
	return contains;
}



int NormalInstr::isEmpty(void){

	// Added by SAH on Feb 27, 2001
	//
	// Check for invalid index, opTreeheader type.
	//
	if ((_opTreeHeader.getType() == _INVALID_) || (_opTreeHeader.getIndex() < 0))
		return 1;

	ArrayIterator<DataOpSlot> *diter;
	ArrayIterator<ControlOpSlot> *citer;
	int nr=0;
	diter=_dataOpList.elements();
	while(diter->hasMoreElements()){
		if(diter->currentElement().hasOper()){
			nr++;
		}
		diter->nextElement();
	}
	citer=_controlOpList.elements();
	while(citer->hasMoreElements()){
		if(citer->currentElement().hasOper())
			nr++;
		citer->nextElement();
	}
	
	delete diter;
	delete citer;
	return nr==0;
}


//////////////////////////// Trailblazing related ////////////////////////////////

//chooses an operation to move next, out of a list of unifiable operations
//ops: input, the list of unifiable ops to choose from
//here should come the heuristics to chose in different ways the next operations to move




UnifiableOp *chooseInstrToMoveHeur(MyLinkedList<UnifiableOp*> &ops)
{
	MyLinkedListIterator<UnifiableOp*> *iter;
	UnifiableOp *resultOp;
	iter=ops.elementsForward();
		
	//currently just choose the first operation in the list
	if(iter->hasMoreElements())
		resultOp=iter->currentElement();
	else 
		resultOp=NULL;
    
	delete iter;
	
	return resultOp;
}

// updates the unifiable ops for the memory ops in the basicblock (bb) that follow this instr.
//
void update_unifiable_ops_for_mem_ops(BasicBlockNode *bb, NormalInstr *instr)
{
	ArrayIterator<DataOpSlot> *diter;
	BaseOperation *oper;

	BBBackIterator biter(bb);

	NormalInstr *temp = NULL;
	while (biter.hasMoreElements())
	{
		temp = (NormalInstr *)(biter.currentElement());

		if (temp == instr) break;

		// Now, go thru the list of data ops, search for any memory ops and then update their unifiable ops positions.
		//
		diter = temp->getDataOpSlotIter();

		while (diter->hasMoreElements())
		{
			if ((diter->currentElement()).hasOper())
			{
				oper = (diter->currentElement()).getPtrToOperation();

				if (oper->isMemoryOp())
					oper->compute_unifiable_ops();
			}
			diter->nextElement();
		}
		delete diter;
		biter.prevElement();
	}
}

// updates the unifiable ops for the data ops in the basicblock (bb) that follow this instr.
//
void update_unifiable_ops_for_succ_ops(BasicBlockNode *bb, NormalInstr *instr)
{
	BBBackIterator biter(bb);

	NormalInstr *temp = NULL;
	while (biter.hasMoreElements())
	{
		temp = (NormalInstr *)(biter.currentElement());

		if (temp == instr) break;

		//compute the unifiable ops for all the ops in the instruction temp

		temp->compute_unifiable_ops();

		// Now, go thru the list of data ops, search for any memory ops and then update their unifiable ops positions.
		//
		/*
		diter = temp->getDataOpSlotIter();

		while (diter->hasMoreElements())
		{
			if ((diter->currentElement()).hasOper())
			{
				oper = (diter->currentElement()).getPtrToOperation();

				oper->compute_unifiable_ops();
			}
			diter->nextElement();
		}
		delete diter;
		*/
		biter.prevElement();
	}
}


//updates the unifiable ops for the dependent operations of oper (when oper has been moved)
//oper: input, the operation to update the depends' unifiable ops for
//The new version just looks at the unifiableOps set, instead of looking at all dependencies
//flow, anti, output, and memory
void NormalInstr::updateSetsForDependentsNew(BaseOperation *oper){
	UnifiableOp * elem;
	MyLinkedListIterator<UnifiableOp*> *iter;
	NormalInstr *instr;

	//for all the operation in the unifiable ops of this
	for(iter=this->_unifiableOps.elementsForward();iter->hasMoreElements();iter->nextElement()){
		elem=iter->currentElement();
		//compute the unifiable ops for this operation
		elem->oper->compute_unifiable_ops();
	}
	delete iter;

	
	instr=(NormalInstr*)findNextInstr(this->getBasicBlock(),this);

	if(instr!=NULL){
		//for all the operation in the unifiable ops of this
		for(iter=instr->_unifiableOps.elementsForward();iter->hasMoreElements();iter->nextElement()){
			elem=iter->currentElement();
			//compute the unifiable ops for this operation
			elem->oper->compute_unifiable_ops();
		}
		delete iter;
	}
}



//updates the unifiable ops for the dependent operations of oper (when oper has been moved)
//oper: input, the operation to update the depends' unifiable ops for
void NormalInstr::updateSetsForDependents(BaseOperation *oper){
	BaseArgProperty * prop;
	MyLinkedListIterator<BaseOperation*>*iter;
	BaseArgument *dest;
	if(oper->isComputeOp()){
		//TBD: first delete the occurences of oper in the next instr _unifiableOps
		dest=((ComputeOp*)oper)->ptrToOperand(_DEST_, _DEST_LIST_);
		if(dest==NULL) return;
		prop=dest->getPtrToProperty(_DUCHAIN_);
		if(prop!=NULL){
			iter=((DUChainProperty*)prop)->useListIteratorForw();
			while(iter->hasMoreElements()){
				if(!iter->currentElement()->isPhiOp() && iter->currentElement()!=oper){
					if(iter->currentElement()->getInstr()!=NULL && 
						//and not a loop carried dependency
						iter->currentElement()->getInstr()->idValue()>this->idValue()){
						//recompute unifiable ops for this operation
						iter->currentElement()->compute_unifiable_ops();
					}
				}
				iter->nextElement();
			}
		}
	}
	else if(oper->isMemoryOp()){
		//TBD: first delete the occurences of oper in the next instr _unifiableOps
		prop=((MemoryOp*)oper)->ptrToDestOperand()->getPtrToProperty(_DUCHAIN_);
		if(prop!=NULL){
			iter=((DUChainProperty*)prop)->useListIteratorForw();
			while(iter->hasMoreElements()){
				if(!iter->currentElement()->isPhiOp() && iter->currentElement()!=oper){
					if(iter->currentElement()->getInstr()!=NULL && 
						iter->currentElement()->getInstr()->idValue()>this->idValue()){
						iter->currentElement()->compute_unifiable_ops();
					}
				}
				iter->nextElement();
			}

		}

		// If it is a memory op, then re-compute the unifiable ops of all memory ops that follow this.
		// This is beos, those mem ops may have been stopped from moving becos of disambiguation dependency.
		//
		// Currently, we only recompute the unifiable ops for memory ops in the same basic block.
		//
		/*BasicBlockNode *bb = NULL;
		SimpleHTGNode *nd = NULL;
		if ((nd = dynamic_cast<SimpleHTGNode *>(this->getUp())) != NULL)
		{
			bb = nd->getBasicBlock();
			update_unifiable_ops_for_mem_ops(bb, this);
		}
		*/
	}

	// Note: This is a hack, added by SAH on 02/11/02
	// Ideally, this should be handled by using anti-dependence chains in the program dependency graph.
	// (Currently, the program dependence graph consists only of UD, DU chains that depict true dependences)
	//
	// also, becos these operations may have others ops blocked due to anti-dependencies,
	// recompute the unifiable ops for all ops below this in the basic block.
	//

	BasicBlockNode *bb = NULL;
	SimpleHTGNode *nd = NULL;
	if ((nd = dynamic_cast<SimpleHTGNode *>(this->getUp())) != NULL)
	{
		bb = nd->getBasicBlock();
		update_unifiable_ops_for_succ_ops(bb, this);
	}
}

//returns 1 if the unifiable ops of this instr contains oper
int NormalInstr::contains_unifiable_op(BaseOperation *oper){
	MyLinkedListIterator<UnifiableOp*> *uiter;
	uiter=_unifiableOps.elementsForward();
	while(uiter->hasMoreElements()){
		if(uiter->currentElement()->oper==oper){
			delete uiter;
			return 1;
		}
		uiter->nextElement();
	}
	delete uiter;
	return 0;
}

void NormalInstr::compute_unifiable_ops(void){
	ArrayIterator<DataOpSlot> *iter;
	DataOpSlot *slot;
	BaseOperation *oper;
	BaseInstruction *instr;

	if(debugUnifOps){
		cout << "Computing unif ops for instr " << this->idValue() << endl;
	}
	
	iter=getDataOpSlotIter();
	while(iter->hasMoreElements()){
		slot=&(iter->currentElement());
		if(slot->hasOper()){
			oper=slot->getPtrToOperation();
			instr=oper->getInstr();
			oper->compute_unifiable_ops();
		}
		iter->nextElement();
	}
}

void NormalInstr::removeUnifiableOps(BaseOperation *oper){
	MyLinkedListIterator<UnifiableOp*> *uiter;
	UnifiableOp *elem;
	uiter=_unifiableOps.elementsForward();
	while(uiter->hasMoreElements()){
		elem=uiter->currentElement();
		uiter->nextElement();
		if(elem->oper==oper){
			_unifiableOps.remove(elem);
			_toDeleteUnifOps.append(elem);
		}
	}
	delete uiter;
}


//find a destination for oper inside this instruction
int NormalInstr::find_unifop_destination(BaseOperation *oper,MyLinkedList<BaseOperation *> moves){
	BaseHTGNode *node;
	
	removeUnifiableOps(oper);
	
	//if oper does not depend on this instruction
#ifdef _FAST_TBZ_
	if(!LastDependency && !cannotMoveBeyondNonPipelined(this,oper,moves)){
#else
	if(!LastDependency && !this->cannotMoveBeyond(oper,moves)){
#endif

		node=prevTB(this,oper);
		if(node!=NULL){
			if(node->find_unifop_destination(oper,moves))
				return 1;
		}
		if(this->addUnifiableOp(oper,moves)) return 1;
		else return 0;
	}
	else{
		return 0;
	}
}

//prints the path along which an operation has been moved
//path: input, the path to print
void PrintPath(vector<BaseHTGNode *> *path){
	vector<BaseHTGNode *>::iterator i;
	NormalInstr *instr;
	SimpleHTGNode *simple;
	cout << "Print path: ";
	for(i=path->begin();i!=path->end();i++){
		if((instr=dynamic_cast<NormalInstr*>(*i))!=NULL){
			cout << instr->idValue() << "->";
		}
		else if((simple=dynamic_cast<SimpleHTGNode*>(*i))!=NULL){
			cout << "SimpleHTGNode: " << simple->getBasicBlock()->getDFSNum() << endl;
		}
		else if(dynamic_cast<IfHTGNode*>(*i)!=NULL){
			cout << "IfHTGNode: " << endl;
		}
		else if(dynamic_cast<LoopHTGNode*>(*i)!=NULL){
			cout << "LoopHTGNode: " << endl;
		}
		else abort();
	}
	cout << endl;
}

//this is the newly inserted operation during the last migrate call
BaseOperation *global_newly_migrated_operation;

//schedule this instruction
void NormalInstr::schedule(void){
	UnifiableOp * elem;
	BaseInstruction *instr;
	vector<BaseHTGNode *> path;
	//while there are more operations in the unifiable ops of this instruction
	while(!_unifiableOps.isEmpty())
	{
		//choose the highest priority operation in the unifiable ops
		elem=chooseInstrToMoveHeur(_unifiableOps);

		// Added by SAH on 01/27/01
		//
		// Added code to test if the operation has any schedule op property.
		// If the operation has a NOCODEMOTION flag then do not choose to migrate this op.
		//
		if (hasScheduleOpProperty(elem->oper, _NOCODEMOTION_) == _NO_)
		{
			//find the instruction to which this operation belongs
			instr=elem->oper->getInstr();
			if(instr!=NULL){
				//the path will store the path along which this operation will be moved
				path.clear();
				//migrate will store in migrated_operation_copies the list of operation copies produced
				migrated_operation_copies.clear();
				//trailblaze elem->oper into "this" instruction, starting from instr, and collect the path
				if(instr->migrate(elem->oper,&path)){
					if(tbzDebug){
						cout << "Scheduling instr: " << _uniqueID << endl;
						cout << "Operation: " << elem->oper->idValue(); cout << "\t"; elem->oper->shortPrint(cout); cout << endl;
						PrintPath(&path);
					}
					//extract the operation from the old instruction
					instr->extract(elem->oper);
					//update live-dead information 
					UpdateLive(global_newly_migrated_operation,&path);
					//delete the UDDU from the old operation
					//deleteUDDU(elem->oper);
					//update the unifiable ops for the dependens operations of the one just moved
					//((NormalInstr*)(instr))->updateSetsForDependents(migrated_operation_copies.front());
					((NormalInstr*)(instr))->updateSetsForDependents(migrated_operation_copies.front());
				}
			}
		}
		removeUnifiableOps(elem->oper);
	}
}

void setLastCanMove(NormalInstr*instr){
	_lastCanMove=instr; //this is a global variable with the last instr 
	//containing the oper in the unifiableOps. 
}

NormalInstr *getLastCanMove(void){
	return _lastCanMove;
}

int NormalInstr::migrate(BaseOperation *oper,vector<BaseHTGNode *> *path){
	BaseHTGNode *node;
	path->push_back(this);
	if(!this->contains_unifiable_op(oper)){
		node=prevTB(this,oper,path);
		if(node!=NULL){
			if(node->migrate(oper,path)) 
				return 1;
		}
	}
	else setLastCanMove(this);
	
	//if the instruction is empty, better insert it in parallel with another instruction, and delete this instruction
	//if(this->isEmpty()) return 0;
	//else 
	return this->insert(oper,path);
}

//adds a unifiable op to the list of unifiable ops of this instruction
//oper: input, the operation to add to the list of unifiable ops
//moves: input, remember also the list of moves that this oper has to be moved accross, in case
//it is actually moved here. These move operations (the moves list) are needed to change the 
//variables accesed in oper to the new names dictated by the list of moves
int NormalInstr::addUnifiableOp(BaseOperation *oper,MyLinkedList<BaseOperation *> &moves){
	UnifiableOp *elem=new UnifiableOp(oper,moves);
	_unifiableOps.append(elem);
	return 1;
}

//Delete all the elements in path past the instr
//since the operation trailblazed didn't fit in them
void AdjustPath(vector<BaseHTGNode *> *path,BaseHTGNode *instr){
	vector<BaseHTGNode *>::iterator i;
	for(i=path->begin();i!=path->end();i++){
		if(*i==instr){
			i++;
			while(i!=path->end()){
				i=path->erase(i);
			}
			break;
		}
	}
}


//insert the operation oper into this instruction (this)
//oper: input, the operation to try to insert 
//path: input, output, the path that this operation has been moved along
int NormalInstr::insert(BaseOperation *oper,vector<BaseHTGNode *> *path){

	// Added by SAH on 01/31/01
	//
	// Added code to test if the instruction has any schedule op property.
	// If the instruction has a NOCODEMOTION flag then do not choose to insert into this instr.
	//
	if (hasScheduleInstrProperty(this, _NOCODEMOTION_) == _YES_)
		return 0;

	ArrayIterator<DataOpSlot> *iter;
	DataOpSlot *availSlot;
	MyLinkedListIterator<UnifiableOp*> *uiter;
	MyLinkedList<BaseOperation *> *moves=NULL;
	BaseHTGNode *instr=this;
	BaseOperation *new_op;
	BaseInstruction *ins;
	ins=oper->getInstr();
	if(ins==this) return 0; //ATTENTION!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//find the moves it was moved accross
	uiter=getLastCanMove()->_unifiableOps.elementsForward();
	while(uiter->hasMoreElements()){
		if(uiter->currentElement()->oper==oper){
			moves=&(uiter->currentElement()->moves);
			break;
		}
		uiter->nextElement();
	}
	//insert the operation
	
	//find the available slot in "this" instruction for oper
	availSlot=(DataOpSlot*)this->getAvailSlot(oper);
#ifdef _FAST_TBZ_
	if(availSlot!=NULL && !this->cannotMoveBeyond(oper,*moves)){
#else
	if(availSlot!=NULL){
#endif
		//first adjust the path by deleting the elements following this
		if(path!=NULL) AdjustPath(path,this);
		getLastCanMove()->removeUnifiableOps(oper);
		//add the operation to this slot (w/o creating a new copy)
		availSlot->addOperation(oper);
		//oper1 is the new copy of the operation oper
		BaseOperation *oper1=availSlot->getPtrToOperation();

		global_newly_migrated_operation = oper1;

		//insert the new copy of the operation oper (oper1) into the list of new operation copies
		migrated_operation_copies.push_back(oper1);

		if(moves!=NULL) oper1->renameOperands(*moves);
		//else abort();
		availSlot->getPtrToOperation()->updateUDDU(oper,moves);
		//iter=_flowOpList.elements();
		iter=_dataOpList.elements();
		//insert the newly added operation into the tree of this VLIW instruction

		// added by Radu

		BaseOpSlot& rootSlot = getRootOpSlot();
		availSlot->setNext(&rootSlot);
		setRootOp(DATA_OP, getSlotIndex(availSlot));

		return 1;

		// end added by Radu

		while(iter->hasMoreElements()){
			if(iter->currentElement().hasOper()){
				availSlot->setNext((iter->currentElement()).getPtrToNextSlot());
				(iter->currentElement()).setNext(availSlot);

				// added by Radu
				return 1;
			}
			iter->nextElement();
		}
		return 1;
	}
	else{
		//for now, don't call the the mutation during trailblazing
		return 0;  //ATTENTION!!!!!!!!!!!!!!!!!
		//Try to mutate this operation 
		cout << "\nFrom: \n";fflush(stdout);oper->getInstr()->print(0);
		cout << "\nTo: \n";fflush(stdout);this->print(0);
		int mutated=oper->mutation(new_op);
		if(mutated && new_op!=NULL){	//ADDITION 0706
			availSlot=(DataOpSlot*)this->getAvailSlot(new_op);
			if(availSlot!=NULL){
				getLastCanMove()->removeUnifiableOps(oper);
				DisconnectOperation(new_op, new_op->getInstr());
				ConnectOperation(new_op,availSlot);
				//BaseOperation *oper1=availSlot->getPtrToOperation();
				if(moves!=NULL) new_op->renameOperands(*moves);
				else abort();
				new_op->updateUDDU(oper,moves);
				//iter=_flowOpList.elements();
				iter=_dataOpList.elements();
				while(iter->hasMoreElements()){
					if(iter->currentElement().hasOper()){
						availSlot->setNext((iter->currentElement()).getPtrToNextSlot());
						(iter->currentElement()).setNext(availSlot);

						// added by Radu
						return 1;
					}
					iter->nextElement();
				}
				return 1;
			}
			else UndoLastMutation();
		}
		else if(mutated && new_op==NULL) return 1;
	}
	return 0;
}

// This function was created by SAH on Feb 20th, 2001
//
// Note: Currently this function only works for traversing DataOpSlots.
// One way of making this work for control op slots too, is to iterate
// over the data, control op slot array, and test each slot to see if it
// is the predecessor slot.
//
BaseOpSlot *getPreviousSlot(NormalInstr *instr, BaseOpSlot *slot)
{
	BaseOpSlot & rootSlot = instr->getRootOpSlot();

	if (&rootSlot == slot) return NULL;

	BaseOpSlot *prevSlot = &rootSlot;

	while (prevSlot != NULL)
	{
		if (prevSlot->isDataOpSlot())
		{
			if (prevSlot->getPtrToNextSlot() == slot) return prevSlot;
			prevSlot = prevSlot->getPtrToNextSlot();
		}
		else
		{
			// currently we do not handle the case when it is a contro/flow op slot.
			assert(0);
		}
	}
	return NULL;
}

//extract an operation from an instruction (w/o actually deleting it)
int NormalInstr::extract(BaseOperation *oper, bool keepUDDU){
	BaseOpSlot *slot=oper->getSlot(this);
	ASSERT(slot!=NULL); //should not call extract on an op that's not in a slot
	
	//first, delete all the UDDU chains of this guy
	if (!keepUDDU) 
	{
		deleteUDDU(oper);
	}
	
	if(slot->isDataOpSlot())
	{
		// Added by SAH on 20th Feb, 2001
		//
		// Also, we need to set the next pointers correctly.
		// Get the previous slot, and set its next to the next of this slot.
		//
		BaseOpSlot *prevSlot = getPreviousSlot(this, slot);

		if (prevSlot != NULL)
		{
			prevSlot->setNext(slot->getPtrToNextSlot());
		}
		else
		{
			// added by Radu

			BaseOpSlot& rootSlot = getRootOpSlot();
			if ((& rootSlot) == slot) {
				setRootOp(slot->getPtrToNextSlot());
			}
			
			// end added by Radu
		}

		((DataOpSlot*)(slot))->resetOperation();

		return 1;
	}
	else if(slot->isControlOpSlot())
	{
		((ControlOpSlot*)(slot))->resetOperation();
		return 1;
	}
	else if(slot->isFlowOpSlot())
	{
		((FlowOpSlot*)(slot))->resetOperation();
		return 1;
	}
	else return 0;
}


int NormalInstr::depends(BaseOperation *oper,MyLinkedList<BaseOperation *> &moves){
	//if during pipelined trailblazing mode (both in trailblazing and spread_instrs),
	//or in stalled trailblazing mode (only in spread_instrs)
	if(pipelinedTbz || (stalledTbz && !insideTbz))
		return ::dependsPipelined(oper,this,moves);
	else 
		return ::depends(oper,this,moves);
}

// Added by SAH on 01/31/01
//
// this function should be used in place of depends.
//
int NormalInstr::cannotMoveBeyond(BaseOperation *oper,MyLinkedList<BaseOperation *> &moves){

	// Test if the instruction has any schedule op property.
	// If the instruction has a BARRIER flag then do not choose to insert into this instr
	// and to all instrs above this
	//
	if (hasScheduleInstrProperty(this, _BARRIER_) == _YES_)
	{
		int instrCode, operCode;
		if ((instrCode = getScheduleInstrPropertyCode(this, _BARRIER_)) == _DEFAULT_ALL_) 
			return 1;

		// else the code is specific. check if the operation too has the code.
		//
		if ((operCode = getScheduleOpPropertyCode(oper, _BARRIER_)) != _INVALID_)
			if ((operCode == _DEFAULT_ALL_) || (operCode == instrCode))
				return 1;
	}

	//if during pipelined trailblazing mode (both in trailblazing and spread_instrs),
	//or in stalled trailblazing mode (only in spread_instrs)
	if(pipelinedTbz || (stalledTbz && !insideTbz))
		return ::dependsPipelined(oper,this,moves);
	else 
		return ::depends(oper,this,moves);
}


#ifdef _FAST_TBZ_

int cannotMoveBeyondNonPipelined(NormalInstr *instr,BaseOperation *oper,MyLinkedList<BaseOperation *> &moves){

	// Test if the instruction has any schedule op property.
	// If the instruction has a BARRIER flag then do not choose to insert into this instr
	// and to all instrs above this
	//
	if (hasScheduleInstrProperty(instr, _BARRIER_) == _YES_)
	{
		int instrCode, operCode;
		if ((instrCode = getScheduleInstrPropertyCode(instr, _BARRIER_)) == _DEFAULT_ALL_) 
			return 1;

		// else the code is specific. check if the operation too has the code.
		//
		if ((operCode = getScheduleOpPropertyCode(oper, _BARRIER_)) != _INVALID_)
			if ((operCode == _DEFAULT_ALL_) || (operCode == instrCode))
				return 1;
	}

	//if during pipelined trailblazing mode (both in trailblazing and spread_instrs),
	//or in stalled trailblazing mode (only in spread_instrs)
	return ::depends(oper,instr,moves);
}

#endif

//marks recursive operations that do address computation
void MarkAddressOperation(BaseOperation *op){
	BaseArgument *arg;
	MyLinkedList<BaseOperation*> *reachDefList;
	MyLinkedListIterator<BaseOperation*> *iter;
	BaseOperation * reachDef;
	BaseArgProperty * prop;
	ArgList & a = op->sourceOperandList();
	ArgListIter ai;
	BaseArgProperty *regclassProp;
	RegClassEntryNode2 *cls;

	//Mark this operation, and all it's sources/destinations
	

	//for all the sources of this operation
	for (ai = a.begin(); ai != a.end(); ai++)
	{
		if ((*ai) != NULL)
		{
			//Mark this argument as pointer
			regclassProp = (*ai)->getPtrToProperty(_REGCLASS_);
			if(regclassProp==NULL) continue;
			cls=new RegClassEntryNode2(globalRegModel.getDataTypeIndex("INT", _GENERIC_),
				globalRegModel.getClassTypeIndex("ADDRESS", _GENERIC_));
			((RegClassArgProperty*)regclassProp)->addClass(*cls);

			//this is the current source argument
			arg=(*ai);
			prop=arg->getPtrToProperty(_UDCHAIN_);
			if(prop!=NULL){
				reachDefList=&((UDChainProperty*)prop)->getDefList();
				//for all the operations that define this argument
				for(iter=reachDefList->elementsForward();iter->hasMoreElements();iter->nextElement()){
					//this is the current operation defining this argument
					reachDef=iter->currentElement();
					//if reachDef has not been traversed yet
					//addr=(ArgProperty*)(reachDef->getPtrToProperty(_ADDRESSARG_);
					if(op->idValue()>reachDef->idValue()) 
						MarkAddressOperation(reachDef);
				}
				delete iter;
			}
		}
	}
}


//adds a marker to all the operations that compute addresses (starting from loads/stores)
void AddPointerMarkerForAddressOperations(Routine *curRoutine){
	MyLinkedListIterator<BaseInstruction *> *instrIterator = curRoutine->instrListIteratorForw();
	NormalInstr * curInstr;
	ArrayIterator<DataOpSlot> *dataSlotIterator;
	DataOpSlot *curDataSlot;
	BaseOperation *curDataOp;
	BaseArgument *arg;
	MyLinkedList<BaseOperation*> *reachDefList;
	MyLinkedListIterator<BaseOperation*> *iter;
	BaseOperation * reachDef;
	BaseArgProperty * prop;

	//for all the instructions in the current routine
	for(;instrIterator->hasMoreElements();instrIterator->nextElement()){
		curInstr = (NormalInstr*)(instrIterator->currentElement());
		//for all the data operations in this instruction
		for(dataSlotIterator = curInstr->getDataOpSlotIter();
		dataSlotIterator->hasMoreElements();
		dataSlotIterator->nextElement()){
			curDataSlot = &(dataSlotIterator->currentElement());
			//this is the current operation
			curDataOp = curDataSlot->getPtrToOperation();
			if(curDataOp==NULL) continue;
			if(curDataOp->isMemoryOp()){
				if(((MemoryOp*)curDataOp)->isLoad()){
					ArgList & a = curDataOp->sourceOperandList();
					ArgListIter ai;
					//for all the sources
					for (ai = a.begin(); ai != a.end(); ai++)
					{
						if ((*ai) != NULL)
						{
							//this is the current source argument
							arg=(*ai);
							prop=arg->getPtrToProperty(_UDCHAIN_);
							if(prop!=NULL){
								reachDefList=&((UDChainProperty*)prop)->getDefList();
								//for all the operations that define this argument
								for(iter=reachDefList->elementsForward();iter->hasMoreElements();
									iter->nextElement()){
									//this is the current operation defining this argument
									reachDef=iter->currentElement();
									MarkAddressOperation(reachDef);
								}
								delete iter;
							}
						}
					}
				}
				else if(((MemoryOp*)curDataOp)->isStore()){
					ArgList & a = curDataOp->sourceOperandList();
					//only for the index source argument
					arg=curDataOp->ptrToOperand(_SRC_1_);
					prop=arg->getPtrToProperty(_UDCHAIN_);
					if(prop!=NULL){
						reachDefList=&((UDChainProperty*)prop)->getDefList();
						//for all the operations that define this argument
						for(iter=reachDefList->elementsForward();iter->hasMoreElements();
						iter->nextElement()){
							//this is the current operation defining this argument
							reachDef=iter->currentElement();
							MarkAddressOperation(reachDef);
						}
						delete iter;
					}
				}
				else _ASSERT(0);
			}
		}
	}
}


//adds a marker to all the operations that are in the address computations slots 
void AddPointerMarkerForSlots(Routine *curRoutine){
	MyLinkedListIterator<BaseInstruction *> *instrIterator = curRoutine->instrListIteratorForw();
	NormalInstr * curInstr;
	ArrayIterator<DataOpSlot> *dataSlotIterator;
	DataOpSlot *curDataSlot;
	BaseOperation *curDataOp;
	BaseArgument *arg;
	int i;
	BaseArgProperty *regclassProp;
	RegClassEntryNode2 *cls;

	//for all the instructions in the current routine
	for(;instrIterator->hasMoreElements();instrIterator->nextElement()){
		curInstr = (NormalInstr*)(instrIterator->currentElement());
		//for all the data operations in this instruction
		for(dataSlotIterator = curInstr->getDataOpSlotIter(),i=0;
		dataSlotIterator->hasMoreElements();
		dataSlotIterator->nextElement(),i++){
			curDataSlot = &(dataSlotIterator->currentElement());
			//this is the current operation
			curDataOp = curDataSlot->getPtrToOperation();
			if(curDataOp==NULL) continue;
			ArgList & a = curDataOp->sourceOperandList();
			ArgListIter ai;
			//for all the sources
			for (ai = a.begin(); ai != a.end(); ai++)
			{
				if ((*ai) != NULL)
				{
					//this is the current source argument
					arg=(*ai);
					//based on the slot of this operation and the argument, add to the argument the property for RA
					//if this slot is present in the mapping
					if(data_regclass_mappings.find(i)!=data_regclass_mappings.end()){
						regclassProp = (*ai)->getPtrToProperty(_REGCLASS_);
						if(regclassProp==NULL) continue;
						cls=new RegClassEntryNode2(globalRegModel.getDataTypeIndex(data_regtype_mappings[i], _GENERIC_),
							globalRegModel.getClassTypeIndex(data_regclass_mappings[i], _GENERIC_));
						((RegClassArgProperty*)regclassProp)->addClass(*cls);
					}
				}
			}
			ArgList & b = curDataOp->destOperandList();
			ArgListIter bi;
			//for all the destinations
			for (bi = b.begin(); bi != b.end(); bi++)
			{
				if ((*bi) != NULL)
				{
					//this is the current source argument
					arg=(*bi);
					//based on the slot of this operation and the argument, add to the argument the property for RA
					//if this slot is present in the mapping
					if(data_regclass_mappings.find(i)!=data_regclass_mappings.end()){
						regclassProp = (*bi)->getPtrToProperty(_REGCLASS_);
						if(regclassProp==NULL) continue;
						cls=new RegClassEntryNode2(globalRegModel.getDataTypeIndex(data_regtype_mappings[i], _GENERIC_),
							globalRegModel.getClassTypeIndex(data_regclass_mappings[i], _GENERIC_));
						((RegClassArgProperty*)regclassProp)->addClass(*cls);
					}
				}
			}
		}
	}
}


