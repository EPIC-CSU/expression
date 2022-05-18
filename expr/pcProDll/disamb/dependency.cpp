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
#include "stdafx.h"

#include "Routine.h"
#include "BasicBlockNode.h"
#include "NormalInstr.h"
#include "PropertyIncludes.h"
#include "UDProperty.h"
#include "SimpleHTGNode.h"
#include "MemoryOp.h"
#include "IburgToIR.h"
#include "CallOp.h"
#include "Instruction.h"
#include "BasicBlockPredecessors.h"

#include "GlobalIncludes.h"
#include "FilePointer.h"
#include "NormalInstr.h"
#include "OpCodeTable.h"
#include "Routine.h"
#include "Program.h"



#ifndef __UNIX__
#include <process.h>
#else
#define __declspec(dllexport)
#define __stdcall
#define _ASSERT assert
#include <stdio.h>
#endif

#ifndef __UNIX__
 	#include "crtdbg.h"
#else
	#define _ASSERT assert
#endif
//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

int debugDep=0;

//declarations
int ComputeNrDepCycles(BaseOperation *oper,int unit,SlotType optype,NormalInstr *instr);
//extern int PIPELENGTH;
__declspec(dllimport) int PIPELENGTH;
int isCCArg(BaseArgument *arg);
//this is to inform find_unifiable_ops_dest to stop at the next instruction
extern int LastDependency;


int definesArg(BaseArgument * arg,BaseInstruction *instr,MyLinkedList<BaseOperation*> &moves){
  MyLinkedList<BaseOperation*> *reachDefList;
  MyLinkedListIterator<BaseOperation*> *iter;
  BaseOperation * reachDef;
  BaseArgProperty * prop;
  if(arg==NULL) return 0;
  prop=arg->getPtrToProperty(_UDCHAIN_);
  if(prop==NULL)
    return 0;
  reachDefList=&((UDChainProperty*)prop)->getDefList();
  iter=reachDefList->elementsForward();
  while(iter->hasMoreElements()){
    reachDef=iter->currentElement();
    if(instr->isNormalInstr() && ((NormalInstr*)instr)->containsOpNotMove(reachDef,moves)) 
      return 1;
    iter->nextElement();
  }
  delete iter;
  //delete reachDefList;
  return 0;
}

int usesArg(BaseArgument * arg,BaseInstruction *instr,MyLinkedList<BaseOperation*> &moves){
	ArrayIterator<DataOpSlot> *iter;
	DataOpSlot *slot;
	BaseOperation *op;

	if(arg==NULL) return 0;
	if(!instr->isNormalInstr()) return 0;

	iter=((NormalInstr*)instr)->getDataOpSlotIter();

	while(iter->hasMoreElements()){
		slot=&(iter->currentElement());
		if(slot->hasOper()){
			op=slot->getPtrToOperation();
/*
			if(equalRegs(arg,op->ptrToOperand(_SOURCE_1_))){
				delete iter;
				return 1;
			}
			else if(equalRegs(arg,op->ptrToOperand(_SOURCE_2_))){
				delete iter;
				return 1;
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
				{
					if (equalRegs(arg, (*ai)))
					{
						delete iter;
						return 1;
					}
				}
			}
			//
			// end change for MULTIPLE_DESTS_AND_SOURCES

		}
		iter->nextElement();
	}

	delete iter;
	return 0;
}

//returns 1 if the instr contains a source as a CC argument
int hasCCDestArg(BaseInstruction *instr){
	ArrayIterator<DataOpSlot> *iterD;
	ArrayIterator<ControlOpSlot> *iterC;
	DataOpSlot *slotD;
	ControlOpSlot *slotC;
	BaseOperation *op;

	if(!instr->isNormalInstr()) return 0;

	iterD=((NormalInstr*)instr)->getDataOpSlotIter();

	while(iterD->hasMoreElements()){
		slotD=&(iterD->currentElement());
		if(slotD->hasOper()){
			op=slotD->getPtrToOperation();
			ArgList & a = op->destOperandList();
			ArgListIter ai;
			for (ai = a.begin(); ai != a.end(); ai++)
			{
				if ((*ai) != NULL)
				{
					if (isCCArg((*ai)))
					{
						delete iterD;
						return 1;
					}
				}
			}
		}
		iterD->nextElement();
	}

	delete iterD;

	iterC=((NormalInstr*)instr)->getControlOpSlotIter();

	while(iterC->hasMoreElements()){
		slotC=&(iterC->currentElement());
		if(slotC->hasOper()){
			op=slotC->getPtrToOperation();
			ArgList & a = op->destOperandList();
			ArgListIter ai;
			for (ai = a.begin(); ai != a.end(); ai++)
			{
				if ((*ai) != NULL)
				{
					if (isCCArg((*ai)))
					{
						delete iterC;
						return 1;
					}
				}
			}
		}
		iterC->nextElement();
	}

	delete iterC;

	return 0;
}

//returns 1 if the instr contains a source as a CC argument
int hasCCSourceArg(BaseInstruction *instr){
	ArrayIterator<DataOpSlot> *iterD;
	ArrayIterator<ControlOpSlot> *iterC;
	DataOpSlot *slotD;
	ControlOpSlot *slotC;
	BaseOperation *op;

	if(!instr->isNormalInstr()) return 0;

	iterD=((NormalInstr*)instr)->getDataOpSlotIter();

	while(iterD->hasMoreElements()){
		slotD=&(iterD->currentElement());
		if(slotD->hasOper()){
			op=slotD->getPtrToOperation();
			ArgList & a = op->sourceOperandList();
			ArgListIter ai;
			for (ai = a.begin(); ai != a.end(); ai++)
			{
				if ((*ai) != NULL)
				{
					if (isCCArg((*ai)))
					{
						delete iterD;
						return 1;
					}
				}
			}
		}
		iterD->nextElement();
	}

	delete iterD;

	iterC=((NormalInstr*)instr)->getControlOpSlotIter();

	while(iterC->hasMoreElements()){
		slotC=&(iterC->currentElement());
		if(slotC->hasOper()){
			op=slotC->getPtrToOperation();
			ArgList & a = op->sourceOperandList();
			ArgListIter ai;
			for (ai = a.begin(); ai != a.end(); ai++)
			{
				if ((*ai) != NULL)
				{
					if (isCCArg((*ai)))
					{
						delete iterC;
						return 1;
					}
				}
			}
		}
		iterC->nextElement();
	}

	delete iterC;

	return 0;
}
int outputDependency(BaseArgument * arg,BaseInstruction *instr,MyLinkedList<BaseOperation*> &moves){
	ArrayIterator<DataOpSlot> *aiter;
	DataOpSlot *aslot;
	BaseOperation *op;

	if(arg==NULL) return 0;
	if(!instr->isNormalInstr()) return 0;

	aiter=((NormalInstr*)instr)->getDataOpSlotIter();

	while(aiter->hasMoreElements()){
		aslot=&(aiter->currentElement());
		if(aslot->hasOper()){
			op=aslot->getPtrToOperation();
	
			// Changed by SAH : Wed, 16th August, 2000
			//
			// MULTIPLE_DESTS_AND_SOURCES
			//
			ArgList & a = op->destOperandList();
			ArgListIter ai;
			for (ai = a.begin(); ai != a.end(); ai++)
			{
				if ((*ai) != NULL)
				{
					if (equalRegs(arg, (*ai)))
					{
						delete aiter;
						return 1;
					}
				}
			}
			//
			// end change for MULTIPLE_DESTS_AND_SOURCES

		}
		aiter->nextElement();
	}
	delete aiter;

	ArrayIterator<ControlOpSlot> *citer;
	ControlOpSlot *cslot;

	citer=((NormalInstr*)instr)->getControlOpSlotIter();

	while(citer->hasMoreElements()){
		cslot=&(citer->currentElement());
		if(cslot->hasOper()){
			op=cslot->getPtrToOperation();
	
			// Changed by SAH : Wed, 16th August, 2000
			//
			// MULTIPLE_DESTS_AND_SOURCES
			//
			ArgList & a = op->destOperandList();
			ArgListIter ai;
			for (ai = a.begin(); ai != a.end(); ai++)
			{
				if ((*ai) != NULL)
				{
					if (equalRegs(arg, (*ai)))
					{
						delete citer;
						return 1;
					}
				}
			}
			//
			// end change for MULTIPLE_DESTS_AND_SOURCES

		}
		citer->nextElement();
	}
	delete citer;


	return 0;
}



int mem_depends(BaseOperation *op,BaseInstruction *instr){
  ArrayIterator<DataOpSlot> *iter;
  BaseOperation *op2;
  disambType d;
  int DEP=0;
  if(instr->isNormalInstr() && op->isMemoryOp()){
    iter=((NormalInstr*)instr)->getDataOpSlotIter();
    while(iter->hasMoreElements() && !DEP){
      if(iter->currentElement().hasOper()){
		  op2=iter->currentElement().getPtrToOperation();
		  if(op2->isMemoryOp()){
			  if(((MemoryOp*)op)->isStore() || ((MemoryOp*)op2)->isStore()){
				d=disamb(op,(iter->currentElement().getPtrToOperation()));
				switch(d){
				case EQUAL:
				case OVERLAPING:
				case DONTKNOW:
					DEP=1;
					break;
				case DISJOINT:
					break;
				}
			  }
		  }
      }
      iter->nextElement();
    }
  }
  delete iter;
  return DEP;
}

//returns 1 if the oper when moved past instr would generate a register
//pressure larger then the number of available registers 
int surpassesRegisterCount(BaseOperation *oper,BaseInstruction *instr){
/*
	BaseArgument *arg;
	if(oper->hasOperand(_DEST_,_DEST_LIST_)){
		arg=oper->ptrToOperand(_DEST_,_DEST_LIST_);
		//if the number of live variables of the same register class as arg is already equal to the 
		//available number of register of that class at the begining of instr, return 1
		//if(count_live_vars(arg_type,instr)>no_available_regs(arg_type)) return 1;
		if(isCCArg(arg) && hasCCArg(instr)){
			return 1;
		}
	}
*/
	ArgList & a = oper->sourceOperandList();
	ArgListIter ai;
	//for all the sources
	for (ai = a.begin(); ai != a.end(); ai++){
		if ((*ai) != NULL){
			if (isCCArg(*ai) && hasCCDestArg(instr)){
				//if the source is a cc, and instr contains a cc
				return 1;
			}
		}
	}
	ArgList & b = oper->destOperandList();
	//for all the dests
	for (ai = b.begin(); ai != b.end(); ai++){
		if ((*ai) != NULL){
			if (isCCArg(*ai) && (hasCCDestArg(instr) || hasCCSourceArg(instr))){
				//if the dest is a cc and instr contains a cc
				return 1;
			}
		}
	}
	//otherwise return 0;
	return 0;
}


int depends(BaseOperation *oper,BaseInstruction *instr,MyLinkedList<BaseOperation*> &moves){

/*		
	if(definesArg(oper->ptrToOperand(_SOURCE_1_),instr,moves)) return 1;
	if(definesArg(oper->ptrToOperand(_SOURCE_2_),instr,moves)) return 1;
	if(oper->hasOperand(_DEST_, _DEST_LIST_) && usesArg(oper->ptrToOperand(_DEST_, _DEST_LIST_),instr,moves)) 
		return 1;
*/

	// Changed by SAH : Wed, 16th August, 2000
	//
	// MULTIPLE_DESTS_AND_SOURCES
	//

	// First, check for true dependency (RAW).
	//
	ArgList & a = oper->sourceOperandList();
	ArgListIter ai;
	for (ai = a.begin(); ai != a.end(); ai++)
	{
		if ((*ai) != NULL)
		{
			if (definesArg((*ai), instr, moves)) return 1;
		}
	}

	// Next, check for anti dependency (WAR)
	//
	ArgList & b = oper->destOperandList();
	for (ai = b.begin(); ai != b.end(); ai++)
	{
		if ((*ai) != NULL)
		{
			if (usesArg((*ai), instr, moves)) return 1;
		}
	}
	
	// Finally, check for output dependency (WAW)
	//
	ArgList & c = oper->destOperandList();
	for (ai = c.begin(); ai != c.end(); ai++)
	{
		if ((*ai) != NULL)
		{
			if (outputDependency((*ai), instr, moves)) return 1;
		}
	}

	//
	// end change for MULTIPLE_DESTS_AND_SOURCES
	
	// Also, if the operation is a memory op, need to perform disambiguation check.
	//
	if(oper->isMemoryOp() && mem_depends(oper,instr)) return 1;
	if(surpassesRegisterCount(oper,instr)) return 1;
	return 0;
}


int debugPrint=0;
extern Program *curProgram;

//Determinmes if oper can or cannot be inserted in instr
//due to data dependencies (possibly from previous instrs in the pipe). 
int dependsPipelined(BaseOperation *oper,BaseInstruction *instr,MyLinkedList<BaseOperation*> &moves){
	BaseHTGNode *node;
	const int PIPESIZE=PIPELENGTH; //used to be 5
	int i=0,nr,saveI;
	NormalInstr *prev;
	Vector<int> indexes;
	BasicBlockNode *bb,*prevBB;
	SlotType optype;

	//find a unit supporting this opcode, since the ComputeNrDepCycles need a
	//valid unit as argument
	simpleVLIWInstrTemplate->getSlotListForOper(oper, indexes);

	if(oper->isDataOp()){
		optype=DATA_OP_SLOT;
	}
	else if(oper->isControlOp()){
		optype=CONTROL_OP_SLOT;
	}
	else
		_ASSERT(0);

	_ASSERT(indexes.size());

	/*if(debugPrint){
		curProgram->printHTG();
	}*/


	//see if there is any following instruction to 'instr' causing a dependency for 'oper'
	/*for(node=instr->getNext(),i=1;i<PIPESIZE && node!=NULL && 
			(next=dynamic_cast<NormalInstr*>(node))!=NULL;node=node->getNext(),i++){
		if(depends(oper,next,moves)){
			//if(i==0) return 1; //faster (no need for ComputeNrCyclesDep)
			
			nr=ComputeNrDepCycles(oper,indexes[0],optype,next);
			//if the nr of cycles we need (nr) is greater than the nr of cycles we have (i), there is a data hazard
			if(abs(nr)>i) return 1;
		}
	}*/

	//see if there is any previous instruction to 'instr' (including 'instr') causing a dependency for 'oper'
	for(node=instr,i=0;i<PIPESIZE && node!=NULL && 
			(prev=dynamic_cast<NormalInstr*>(node))!=NULL;node=node->getPrev(),i++){
		if(depends(oper,prev,moves)){
			//if(i==0) return 1; //faster (no need for ComputeNrCyclesDep)
			
			nr=ComputeNrDepCycles(oper,indexes[0],optype,prev);
			if(nr==0 && i==0){
				//set this to inform the find_unifiable_ops_dest to stop at the next instruction
				LastDependency=1;
			}
			if(debugDep){
				cout << "Operation " << oper->idValue() << " (into " << instr->idValue() << ") depends on Instruction " << prev->idValue() << " with latency " << nr ;
			}
			//if the nr of cycles we need (nr) is greater than the nr of cycles we have (i), there is a data hazard
			if(nr>i){
				if(debugDep){
					cout << " and latency is NOT satisfied" << endl;
				}
				return 1;
			}
			if(debugDep){
				cout << " and latency is satisfied" << endl;
			}
		}
	}

	

	//if reached the first instr in this basic block, go to the previous basic blocks
	if(i<PIPESIZE){
		//see if there is any previous basic block causing dependency for 'oper'

		//save i for later restarting
		saveI=i;

		bb=instr->getBasicBlock();
		
		//find the list of predecessors of bb
		BasicBlockPredecessors *predecessors=&(bb->getCFPred());
		MyLinkedListIterator<BasicBlockNode *> *iter;

		iter=predecessors->createPredListIterForw();
		
		if(iter!=NULL){
			//for all previous bbs of bb
			while(iter->hasMoreElements()){
				i=saveI;
				prevBB=iter->currentElement();
				//for the previous instructions 'node' starting from the last instr in prevBB
				for(node=prevBB->getLastInstr();i<PIPESIZE && node!=NULL && 
					(prev=dynamic_cast<NormalInstr*>(node))!=NULL;node=node->getPrev(),i++){
					if(depends(oper,prev,moves)){
						nr=ComputeNrDepCycles(oper,indexes[0],optype,prev);
						//if the nr of cycles we need (nr) is greater than the nr of cycles we have (i), there is a data hazard
						if(nr>i){
							delete iter;
							return 1;
						}
					}
				}
				iter->nextElement();
			}
			delete iter;
		}
	}
	return 0;
}
