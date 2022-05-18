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
//===========================================================================
// File:          CompLiveDead.cc
// Purpose:       compute live variables of basic block 
// Author:        Xiaomei Ji
// 
//=========================================================================

// $Id
#include "stdafx.h"

#include "BaseBBProperty.h"
#include "LiveDeadBBProperty.h"
#include "LiveDeadInstrProperty.h"
#include "BaseArgument.h"
#include "BasicBlockNode.h"
#include "MyLinkedList.h"
#include "Array.h"
#include "PropertyList.h"
#include "BaseInstruction.h"
#include "NormalInstr.h"
#include "MemoryOp.h"
#include "ComputeOp.h"
#include "IfOp.h"
#include "DataOpSlot.h"
#include "ControlOpSlot.h"
#include "Routine.h"
#include "BBIterator.h"
#include "SymbolTable.h"
#include "CallOp.h"
#include "SimpleHTGNode.h"
#include "UDDU.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

extern SymbolTable* globalSymbolTable;
extern void compLiveDeadInstr(Routine *routine);
int usesArg(BaseArgument * arg,BaseInstruction *instr,MyLinkedList<BaseOperation*> &moves);

// ** IMPORTANT **
// SAH, 06/25/01
// We record the list of live variables only at the exit of each basic block.
// To determine the list of live variables at any instruction, we need to perform additional
// computation starting from the live list at the exit of the block.

//==================================================================
// recordUse
//
//==================================================================

static
void 
recordUse(BasicBlockNode* bb, 
		  NormalInstr* instr,
		  BaseArgument* arg, 
		  SetList<int>& use) {
	if ( arg && arg->isRegisterArg() ) {
		int val = arg->getUniqID();
		//
		// was previously : val = globalSymbolTable->getID(arg, arg->getPtrToProperty(_SSA_));

		BBForwIterator iter(bb);
		while ( iter.hasMoreElements() ) {
			BaseInstruction* prevInstr = iter.currentElement();
			if ( prevInstr == instr ) {
				break;
			}
			
			ArrayIterator<DataOpSlot>* dataIter = ((NormalInstr*)prevInstr)->getDataOpSlotIter();
			while ( dataIter->hasMoreElements() ) {
				BaseOperation* op = dataIter->currentElement().getPtrToOperation();
				if ( op == NULL ) {
					dataIter->nextElement();
					continue;
				}
/*
				// compute operation
				if ( op->isComputeOp() 
					|| op->isMemoryOp() && ((MemoryOp*)op)->isLoad() 
					|| op->isCallOp() && ((CallRetOp*)op)->hasOperand(_DEST_, _DEST_LIST_) ) {
					BaseArgument* defArg = op->ptrToOperand(_DEST_, _DEST_LIST_);
					if ( globalSymbolTable->getID(defArg, defArg->getPtrToProperty(_SSA_)) == val )   {
						delete dataIter;
						return;
					}
				}
*/

				// Changed by SAH : Wed, 16th August, 2000
				//
				// MULTIPLE_DESTS_AND_SOURCES
				//
				ArgList & a = op->destOperandList();
				ArgListIter ai;
				for (ai = a.begin(); ai != a.end(); ai++)
				{
					if ((*ai) != NULL)
						if ((*ai)->isRegisterArg())
							if (((RegisterArg *)(*ai))->getUniqID() == val )   {
								delete dataIter;
								return;
							}
				}
				//
				// end change for MULTIPLE_DESTS_AND_SOURCES
				
				dataIter->nextElement();
			}	
			
			delete dataIter;
			
			ArrayIterator<ControlOpSlot>* cntlIter = ((NormalInstr*)prevInstr)->getControlOpSlotIter();
			while ( cntlIter->hasMoreElements() ) {
				BaseOperation* op = cntlIter->currentElement().getPtrToOperation();
				if ( op == NULL ) {
					cntlIter->nextElement();
					continue;
				}
/*
				if ( op->isCallOp() && ((CallRetOp*)op)->hasOperand(_DEST_, _DEST_LIST_) ) { 
					BaseArgument* defArg = op->ptrToOperand(_DEST_, _DEST_LIST_);
					if ( globalSymbolTable->getID(defArg, defArg->getPtrToProperty(_SSA_)) == val )   {
						delete cntlIter;
						return;
					}
				}
*/

				// Changed by SAH : Wed, 16th August, 2000
				//
				// MULTIPLE_DESTS_AND_SOURCES
				//
				ArgList & a = op->destOperandList();
				ArgListIter ai;
				for (ai = a.begin(); ai != a.end(); ai++)
				{
					if ((*ai) != NULL)
						if ((*ai)->isRegisterArg())
							if (((RegisterArg *)(*ai))->getUniqID() == val )   {
								delete cntlIter;
								return;
							}
				}
				//
				// end change for MULTIPLE_DESTS_AND_SOURCES
				
				cntlIter->nextElement();
			}
			delete cntlIter;
			
			
			
			iter.nextElement();
		} 
		use.add(val);
	}
}



//==================================================================
// compUseOfNormalInstr
//==================================================================

static
void
compUseOfNormalInstr(BasicBlockNode* bb, NormalInstr* instr, SetList<int>& use){
	// data operation
	ArrayIterator<DataOpSlot>* iter = instr->getDataOpSlotIter();
	while ( iter->hasMoreElements() ) {
		BaseOperation* op = iter->currentElement().getPtrToOperation();
		if ( op == NULL ) {
			iter->nextElement();
			continue;
		}
		/*
		if ( op->isComputeOp() ) {
		recordUse(bb, instr, ((ComputeOp*)op)->ptrToOperand(_SOURCE_1_), use);  
		recordUse(bb, instr, ((ComputeOp*)op)->ptrToOperand(_SOURCE_2_), use);  
		} else if ( op->isMemoryOp() ) {
		recordUse(bb, instr, ((MemoryOp*)op)->ptrToSourceOperand(),  use);  
		if ( ((MemoryOp*)op)->isStore() )
		recordUse(bb, instr, ((MemoryOp*)op)->ptrToDestOperand(), use);  
		
		  } else if ( op->isCallOp() ) {
		  if ( ((CallRetOp*)op)->hasOperand(_SOURCE_2_) )
		  recordUse(bb, instr, op->ptrToOperand(_SOURCE_2_), use);
		  ArgList&  argList = ((CallRetOp*)op)->getParmList();
		  ArgListIter argIter;
		  for (argIter = argList.begin(); argIter != argList.end(); argIter++)
		  {
		  recordUse(bb, instr, *argIter, use);
		  }
		  ArgList& savedRegs = ((CallRetOp*)op)->getSavedRegList();
		  for ( argIter = savedRegs.begin(); argIter!=savedRegs.end(); argIter++ ) 
		  recordUse(bb, instr, *argIter, use);
		  }
		*/
		// Changed by SAH : Wed, 16th August, 2000
		//
		// MULTIPLE_DESTS_AND_SOURCES
		//
		ArgList & a = op->sourceOperandList();
		ArgListIter ai;
		for (ai = a.begin(); ai != a.end(); ai++)
		{
			if ((*ai) != NULL)
				recordUse(bb, instr, *ai, use);
		}
		//
		// end change for MULTIPLE_DESTS_AND_SOURCES
		
		iter->nextElement();
	}
	delete iter;
	
	// control operation
	ArrayIterator<ControlOpSlot>* cntlIter = instr->getControlOpSlotIter();
	while ( cntlIter->hasMoreElements() ) {
		BaseOperation* op = cntlIter->currentElement().getPtrToOperation();
		
		if (op == NULL)
		{
			cntlIter->nextElement();
			continue;
		}
		/*
		if ( op && op->isIfOp() ) {
		recordUse(bb, instr, ((IfOp*)op)->ptrToCCReg(),  use);  
		}
		else if ( op && op->isCallOp() ) {
		if ( ((CallRetOp*)op)->hasOperand(_SOURCE_2_) )
		recordUse(bb, instr, op->ptrToOperand(_SOURCE_2_), use);
		ArgList&  argList = ((CallRetOp*)op)->getParmList();
		ArgListIter argIter;
		for (argIter = argList.begin(); argIter != argList.end(); argIter++)
		{
		recordUse(bb, instr, *argIter, use);
		}
		ArgList& savedRegs = ((CallRetOp*)op)->getSavedRegList();
		for ( argIter = savedRegs.begin(); argIter!=savedRegs.end(); argIter++ ) 
		recordUse(bb, instr, *argIter, use);
		}
		*/		

		// Changed by SAH : Wed, 16th August, 2000
		//
		// MULTIPLE_DESTS_AND_SOURCES
		//
		ArgList & a = op->sourceOperandList();
		ArgListIter ai;
		for (ai = a.begin(); ai != a.end(); ai++)
		{
			if ((*ai) != NULL)
				recordUse(bb, instr, *ai, use);
		}
		//
		// end change for MULTIPLE_DESTS_AND_SOURCES
		
		if (op->isCallOp())
		{
			ArgList& argList = ((CallRetOp*)op)->getParmList();
			ArgListIter argIter;
			for (argIter = argList.begin(); argIter != argList.end(); argIter++) {
				recordUse(bb, instr, *argIter, use);
			}
			
			ArgList& savedRegs = ((CallRetOp*)op)->getSavedRegList();
			for ( argIter = savedRegs.begin(); argIter!=savedRegs.end(); argIter++ ) 
				recordUse(bb, instr, *argIter, use);
		}
		cntlIter->nextElement();
	}
	delete cntlIter;
}



//==================================================================
// compUseOfBB
//==================================================================

static
void
compUseOfBB(BasicBlockNode* bb, SetList<int>& use) {
	BBForwIterator iter(bb);
	while ( iter.hasMoreElements() ) {
		compUseOfNormalInstr(bb, (NormalInstr*)iter.currentElement(), use);
		iter.nextElement();
	}
}


//==================================================================
// outMinusDef
//
// can minus directly
//=================================================================

static
void
outMinusDef(SetList<int>& dest, MyLinkedList<BaseArgument*>& source) {
	MyLinkedListIterator<BaseArgument*>* iter = source.elementsForward();
	while ( iter->hasMoreElements() ) {
		
		// Added by SAH, Wed, Jan 20, 99
		//
		// Code to take care of the condition when iter->currentElement() == NULL
		//
		if (iter->currentElement() != NULL)
		{
			int val = ((RegisterArg *)(iter->currentElement()))->getUniqID();
			//
			// was previously : val = globalSymbolTable->getID(iter->currentElement(), iter->currentElement()->getPtrToProperty(_SSA_));
			dest.remove(val);
		}
		iter->nextElement();
	}
	delete iter;
}



//==================================================================
// compIn
//
//=================================================================

static
void
compIn(ArrayIterator<BasicBlockNode*>* bbIter, SetList<int>* use,
       SetList<int>* in, SetList<int> * out ) { 
	
	SetList<int> tmp;
	
	int change = 1;
	while ( change ) {
		change = 0;
		bbIter->reset();
		while ( bbIter->hasMoreElements() ) {
			BasicBlockNode* bb = bbIter->currentElement();
			int index = bb->getDFSNum();
			// out = union of succ(in);
			if ( bb->getFirstSucc() != _NULL_ ) { 
				out[index] = in[bb->getFirstSucc()->getDFSNum()];
				if ( bb->getNextSucc() != _NULL_ ) {
					out[index].set_union(in[bb->getNextSucc()->getDFSNum()]);
				}
			}
			tmp = out[index];
			if ( CompDefBBProperty* prop = (CompDefBBProperty*)bb->getPtrToProperty(_COMPDEF_) ) {
				outMinusDef(tmp, prop->getDefList());
			}
			tmp.set_union(use[index]);
			if ( tmp.operator!= (in[index]) ) {
				change = 1;
				in[index] = tmp;
			}
			tmp.removeAll();
			bbIter->nextElement();
		} 
	}
}



//==================================================================
// compLiveVar
//=================================================================

static
void
compLiveVar(ArrayIterator<BasicBlockNode*>* bbIter, SetList<int>* out){
	// set live variable into basic block's property list
	while ( bbIter->hasMoreElements() ) {
		BasicBlockNode* bb = bbIter->currentElement();
		LiveDeadBBProperty tmpProp;
		bb->addProperty(tmpProp);
        LiveDeadBBProperty& prop = (LiveDeadBBProperty&)bb->getPropList().getTail();
        MyLinkedListIterator<int>* iter = out[bb->getDFSNum()].elementsForward();
        while ( iter->hasMoreElements() ) {
			prop.appendLiveVar(iter->currentElement() );
			iter->nextElement();
		}
        delete iter;
		bbIter->nextElement();
	}
}

void printSet(SetList<int>& list)
{
	MyLinkedListIterator<int> *iter = list.elementsForward();
	while (iter->hasMoreElements()) {
		int elem = iter->currentElement();

		cout << elem << " ";

		iter->nextElement();
	}
	delete iter;
}

// ** IMPORTANT **
//
// compLiveDead records the 
// variables that are live at the exit of the basic block
//


//==================================================================
// compLiveDead
//=================================================================

void
Routine::compLiveDead() {
#ifdef DEBUG
	printf(" Begin to compute the live dead set\n");
#endif
	
	if (isEmptyBodyRoutine()) return;
	
	int numBBs = _cfg.numBBs();
	SetList<int>* use= new SetList<int>[numBBs+1];
	
	ArrayIterator<BasicBlockNode*>* bbIter = _cfg.createDFSBBIterBack();
	while ( bbIter->hasMoreElements() ) {
		BasicBlockNode* bb = bbIter->currentElement();
		compUseOfBB(bb, use[bb->getDFSNum()]);
		bbIter->nextElement();
	} 	
	
	bbIter->reset();
	
   	SetList<int>* in = new SetList<int>[numBBs+1];
   	SetList<int>* out = new SetList<int>[numBBs+1];
	compIn(bbIter, use, in, out);

	delete [] use;
   	delete [] in;
	
	bbIter->reset();
	compLiveVar(bbIter, out);
	
	delete bbIter;
   	delete [] out;

	//compute live dead instr information for this routine
	compLiveDeadInstr(this);
	
#ifdef DEBUG
	printf(" end of computing the live dead set \n");
#endif
}


int CountUses(BaseArgument *dst){
	MyLinkedList<BaseOperation*> *useList;
	
	useList=GetUseList(dst);
	if(useList==NULL) return 0;
	int nr=useList->numItems();
	return nr;
}

//add an arg to the live set of a bb (if it is not there already) 
void AddLiveToBB(BaseArgument *arg,BasicBlockNode *bb){
	int val = ((RegisterArg *)arg)->getUniqID();
	//
	// was previously : val = globalSymbolTable->getID(arg, arg->getPtrToProperty(_SSA_));
	LiveDeadBBProperty* prop=(LiveDeadBBProperty*)bb->getPtrToProperty(_LIVEDEAD_);
	if(prop){
		SetList<int>& liveSet = prop->getLiveList();
		if(!liveSet.contains(val))
			liveSet.append(val);
	}
}

void AddLiveBeg(NormalInstr *instr,BaseArgument *arg){
	LiveDeadInstrProperty *prop;
	int val;
	SetList<int> *live;
	//find the LIVEDEADINSTR property of instr
	prop=(LiveDeadInstrProperty*)instr->getPropertyList().getPtrToProperty(_LIVEDEADINSTR_);
	if(prop!=NULL){
		live=prop->getLiveListPtrBeg();
		//add it to the begining live list
		if (arg && arg->isRegisterArg()){
			val = globalSymbolTable->getID(arg, arg->getPtrToProperty(_SSA_));
			if(!live->contains(val)){
				live->append(val);
			}
		}
	}
	else{
		_ASSERT(0);
	}
}

void AddLiveEnd(NormalInstr *instr,BaseArgument *arg){
	LiveDeadInstrProperty *prop;
	int val;
	SetList<int> *live;
	//find the LIVEDEADINSTR property of instr
	prop=(LiveDeadInstrProperty*)instr->getPropertyList().getPtrToProperty(_LIVEDEADINSTR_);
	if(prop!=NULL){
		//add it to the begining live list
		live=prop->getLiveListPtrEnd();
		//add it to the begining live list
		if (arg && arg->isRegisterArg()){
			val = globalSymbolTable->getID(arg, arg->getPtrToProperty(_SSA_));
			if(!live->contains(val)){
				live->append(val);
			}
		}
	}
	else{
		_ASSERT(0);
	}
}

void RemoveLiveBeg(NormalInstr *instr,BaseArgument *arg){
	LiveDeadInstrProperty *prop;
	int val;
	SetList<int> *live;
	//find the LIVEDEADINSTR property of instr
	prop=(LiveDeadInstrProperty*)instr->getPropertyList().getPtrToProperty(_LIVEDEADINSTR_);
	if(prop!=NULL){
		//remove it from the begining live list
		live=prop->getLiveListPtrBeg();
		//add it to the begining live list
		if (arg && arg->isRegisterArg()){
			val = globalSymbolTable->getID(arg, arg->getPtrToProperty(_SSA_));
			if(live->contains(val)){
				live->remove(val);
			}
		}
	}
	else{
//		_ASSERT(0);
	}
}

void RemoveLiveEnd(NormalInstr *instr,BaseArgument *arg){
	LiveDeadInstrProperty *prop;
	int val;
	SetList<int> *live;
	//find the LIVEDEADINSTR property of instr
	prop=(LiveDeadInstrProperty*)instr->getPropertyList().getPtrToProperty(_LIVEDEADINSTR_);
	if(prop!=NULL){
		//remove it from the begining live list
		live=prop->getLiveListPtrEnd();
		//add it to the begining live list
		if (arg && arg->isRegisterArg()){
			val = globalSymbolTable->getID(arg, arg->getPtrToProperty(_SSA_));
			if(live->contains(val)){
				live->remove(val);
			}
		}
	}
	else{
//		_ASSERT(0);
	}
}


int ContainsLiveBeg(NormalInstr *instr,BaseArgument *arg){
	LiveDeadInstrProperty *prop;
	int val;
	SetList<int> *live;
	//find the LIVEDEADINSTR property of instr
	prop=(LiveDeadInstrProperty*)instr->getPropertyList().getPtrToProperty(_LIVEDEADINSTR_);
	if(prop!=NULL){
		live=prop->getLiveListPtrEnd();
		if (arg && arg->isRegisterArg()){
			val = globalSymbolTable->getID(arg, arg->getPtrToProperty(_SSA_));
			return live->contains(val);
		}
	}
	else{
		_ASSERT(0);
	}
	return 0;
}

int ContainsLiveEnd(NormalInstr *instr,BaseArgument *arg){
	LiveDeadInstrProperty *prop;
	int val;
	SetList<int> *live;
	//find the LIVEDEADINSTR property of instr
	prop=(LiveDeadInstrProperty*)instr->getPropertyList().getPtrToProperty(_LIVEDEADINSTR_);
	if(prop!=NULL){
		live=prop->getLiveListPtrEnd();
		if (arg && arg->isRegisterArg()){
			val = globalSymbolTable->getID(arg, arg->getPtrToProperty(_SSA_));
			return live->contains(val);
		}
	}
	else{
		_ASSERT(0);
	}
	return 0;
}

void UpdateDest(BaseArgument *arg,vector<BaseHTGNode *> *path){
	vector<BaseHTGNode *>::iterator i;
	BasicBlockNode *bb;
	SimpleHTGNode *simple;
	int first=1;
	int first_instr=1;
	MyLinkedList<BaseOperation*> moves;
	NormalInstr *instr;
	
	if(CountUses(arg)>0){
		//add dest in all the lives of bbs, except the first one 
		//(the oper is moving updwards)
		for(i=path->begin();i!=path->end();){
			simple=dynamic_cast<SimpleHTGNode *>(*i);
			instr=dynamic_cast<NormalInstr*>(*i);
			i++;
			if(simple!=NULL){
				bb=simple->getBasicBlock();
				if(!first){
					AddLiveToBB(arg,bb);
				}
				else first=0;
			}
			else if(instr!=NULL){
				//for each instruction traversed
				if(i!=path->end()){
					//if this is not the destination instruction
					//add arg to the live set for the begining of this instruction
					AddLiveBeg(instr,arg);
				}
				if(!first_instr){
					//if this is not the source instruction
					//add arg to the live set for the end of this instruction
					AddLiveEnd(instr,arg);
				}
				else
					first_instr=0;
			}
		}
	}
}

void UpdateSrc(BaseArgument *arg,vector<BaseHTGNode *> *path){
	vector<BaseHTGNode *>::iterator i;
	BasicBlockNode *bb;
	SimpleHTGNode *simple;
	NormalInstr *instr;
	int first=1;
	int first_instr=1;
	MyLinkedList<BaseOperation*> moves;
	//for each HTG node traversed during trailblazing
	for(i=path->begin();i!=path->end();){
		simple=dynamic_cast<SimpleHTGNode *>(*i);
		instr=dynamic_cast<NormalInstr*>(*i);
		i++;
		if(simple!=NULL){
			//for each basic block traversed
			bb=simple->getBasicBlock();
			if(!first){
				//if wasn't live in the first bb (the source bb), need to delete 
				// from next bb's only as long as there are no other uses in those bbs
			}
			else{
				first=0;
				//if live in bb (i.e. at the end of bb), then nothing needed
				//since it's gonna be live higher too
				if(bb->isLive(arg)) break;
			}
		}
		else if(instr!=NULL){
			//for each instruction traversed
			if(first_instr){
				first_instr=0;
				//this is the source instruction, where the operation is moved out of
				//if arg is live at the end of this guy, do not delete it on the path
				if(ContainsLiveEnd(instr,arg)){
					//done with this arg
					return;
				}
				else{
					//if it is not live at the end of the source instruction
					//remove arg from this instr beg live set and the following ones (as in previous ones:-))
					RemoveLiveBeg(instr,arg);
				}
			}	
			else{
				//these are the instructions on the path
				if(usesArg(arg,instr,moves)){
					//if the instr uses the arg argument
					//delete arg only from LiveSetEnd of instr
					RemoveLiveEnd(instr,arg);
					//and stop for this arg
					return;
				}
				else{
					//if the instr does not use the arg argument (it shouldn't define it, since that would be a 
					//dependence)
					//remove the arg from both the beg and end live lists of instr
					RemoveLiveEnd(instr,arg);
					if(i!=path->end()){
						//if this is not the destination instruction, remove arg from the live set at the begining
						RemoveLiveBeg(instr,arg);
					}
				}
			}
		}
	}
}


//Update Live information for oper, moving along the path path. 
void UpdateLive(BaseOperation *oper,vector<BaseHTGNode *> *path){
	if (oper == NULL) return;

	/*
	if(oper->hasOperand(_DEST_, _DEST_LIST_))
	UpdateDest(oper->ptrToOperand(_DEST_, _DEST_LIST_),path);
	if(oper->hasOperand(_SOURCE_1_))
	UpdateSrc(oper->ptrToOperand(_SOURCE_1_),path);
	if(oper->hasOperand(_SOURCE_2_))
	UpdateSrc(oper->ptrToOperand(_SOURCE_2_),path);
	*/
	
	// Changed by SAH : Wed, 16th August, 2000
	//
	// MULTIPLE_DESTS_AND_SOURCES
	//
	ArgList & a = oper->destOperandList();
	ArgListIter ai;
	for (ai = a.begin(); ai != a.end(); ai++)
	{
		if ((*ai) != NULL)
			UpdateDest(*ai, path);
	}
	ArgList & b = oper->sourceOperandList();
	for (ai = b.begin(); ai != b.end(); ai++)
	{
		if ((*ai) != NULL)
			UpdateSrc(*ai, path);
	}
	//
	// end change for MULTIPLE_DESTS_AND_SOURCES
				
}
