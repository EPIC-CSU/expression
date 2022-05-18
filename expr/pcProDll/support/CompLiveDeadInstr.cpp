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

#include "BaseInstrProperty.h"
#include "LiveDeadInstrProperty.h"
#include "LiveDeadBBProperty.h"
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



//adds teh agrument arg into the livedead infor for instr
static void recordInstrUse(SetList<int>*live,BaseArgument* arg){
	int val;
	if (arg && arg->isRegisterArg()){
		val = globalSymbolTable->getID(arg, arg->getPtrToProperty(_SSA_));
		if(!live->contains(val)){
			live->append(val);
		}
	}
}

//adds teh agrument arg into the livedead infor for instr
static void recordInstrDef(SetList<int>*live,BaseArgument* arg){
	int val;
	if (arg && arg->isRegisterArg()){
		val = globalSymbolTable->getID(arg, arg->getPtrToProperty(_SSA_));
		if(live->contains(val)){
			live->remove(val);
		}
	}
}


//computes the live list for one instruction
//instr: ouptut: the instruction
//prev_live_beg: input, teh set of live values at the end of the instruction (at the begining of the previous instr)
//live_beg: output: the set of live values at the begining of the instruciton
//computes the set of live values at the begining of the instruction
void LiveDeadInstr_for_instr(NormalInstr *instr,SetList<int>*prev_live_beg,SetList<int>*&live_beg){
	MyLinkedListIterator<int>* it;
	LiveDeadInstrProperty *prop;
	SetList<int> *live_end;
	ArrayIterator<DataOpSlot>* iter;

	//first delete the old LiveDeadInstrProperty from instr if it has one
	prop=(LiveDeadInstrProperty*)instr->getPropertyList().getPtrToProperty(_LIVEDEADINSTR_);
	if(prop!=NULL){
		instr->getPropertyList().removeProperty(*prop);
	}

	//add a new LIVEDEADINSTR property to instr
	prop=new LiveDeadInstrProperty();
	instr->addProperty(*prop);

	//the live list at the begining of this instruction
	live_beg=prop->getLiveListPtrBeg();
	//the live list at the end of this instruction
	live_end=prop->getLiveListPtrEnd();

	//copy all the elements from prev_live_beg to live_end and live_beg
	it = (MyLinkedListIterator<int>*)(prev_live_beg->elementsForward());
	while ( it->hasMoreElements() ) {
		live_end->append(it->currentElement());
		live_beg->append(it->currentElement());
		it->nextElement();
	}
	delete it;
	

	iter = instr->getDataOpSlotIter();
	//for all data operations
	while ( iter->hasMoreElements() ) {
		BaseOperation* op = iter->currentElement().getPtrToOperation();
		if ( op == NULL ) {
			iter->nextElement();
			continue;
		}
		if ( op->isComputeOp() ) {
            recordInstrUse(live_beg, ((ComputeOp*)op)->ptrToOperand(_SOURCE_1_));  
            recordInstrUse(live_beg, ((ComputeOp*)op)->ptrToOperand(_SOURCE_2_));  
			recordInstrDef(live_beg,((ComputeOp*)op)->ptrToOperand(_DEST_));
		} 
		else if ( op->isMemoryOp() ) {
            recordInstrUse(live_beg, ((MemoryOp*)op)->ptrToSourceOperand());  
            if ( ((MemoryOp*)op)->isStore() )
				recordInstrUse(live_beg, ((MemoryOp*)op)->ptrToDestOperand());  
			else{
				recordInstrDef(live_beg, ((MemoryOp*)op)->ptrToOperand(_DEST_));
			}
            
		} 
		else if ( op->isCallOp() ) {
			if ( ((CallRetOp*)op)->hasOperand(_SOURCE_2_) )
				recordInstrUse(live_beg, op->ptrToOperand(_SOURCE_2_));
			ArgList&  argList = ((CallRetOp*)op)->getParmList();
			ArgListIter argIter;
			for (argIter = argList.begin(); argIter != argList.end(); argIter++)
			{
				recordInstrUse(live_beg, *argIter);
			}
			ArgList& savedRegs = ((CallRetOp*)op)->getSavedRegList();
			for ( argIter = savedRegs.begin(); argIter!=savedRegs.end(); argIter++ ) 
				recordInstrUse(live_beg, *argIter);
		}
		//*/
		
		iter->nextElement();
	}
	delete iter;
	
	// control operation
	ArrayIterator<ControlOpSlot>* cntlIter = instr->getControlOpSlotIter();
	//for each control operation
	while ( cntlIter->hasMoreElements() ) {
		BaseOperation* op = cntlIter->currentElement().getPtrToOperation();
		if ( op && op->isIfOp() ) {
            recordInstrUse(live_beg, ((IfOp*)op)->ptrToCCReg());  
		}
		else if ( op && op->isCallOp() ) {
			if ( ((CallRetOp*)op)->hasOperand(_SOURCE_2_) )
				recordInstrUse(live_beg, op->ptrToOperand(_SOURCE_2_));
			ArgList&  argList = ((CallRetOp*)op)->getParmList();
			ArgListIter argIter;
			for (argIter = argList.begin(); argIter != argList.end(); argIter++)
			{
				recordInstrUse(live_beg, *argIter);
			}
			ArgList& savedRegs = ((CallRetOp*)op)->getSavedRegList();
			for ( argIter = savedRegs.begin(); argIter!=savedRegs.end(); argIter++ ) 
				recordInstrUse(live_beg, *argIter);

			recordInstrDef(live_beg, op->ptrToOperand(_DEST_));
		}
		
		cntlIter->nextElement();
	}
	delete cntlIter;
}

void LiveDeadInstr_for_bb(BasicBlockNode *bb){
	BBBackIterator iter(bb);
	NormalInstr *instr;
	SetList<int>*prev_live_beg,*live_beg;
	//the initial live setinfo is the live set at the end of the basic block
	LiveDeadBBProperty* prop=(LiveDeadBBProperty*)(bb->getPtrToProperty(_LIVEDEAD_));
	prev_live_beg=&(prop->getLiveList());
	//for each instr in the basic block, going backwards
	while ( iter.hasMoreElements() ) {
		//compUseOfNormalInstr(bb, (NormalInstr*)iter.currentElement());
		//compute the livedead info for this instruction 
		instr=(NormalInstr*)iter.currentElement();
		//compute live_beg from live_end and instr
		LiveDeadInstr_for_instr(instr,prev_live_beg,live_beg);
		//the live set at the end of the previous instruction is the live set at the begining of this instruction
		prev_live_beg=live_beg;
		iter.prevElement();
	}
}




//==================================================================
// compLiveDead
//=================================================================

void compLiveDeadInstr(Routine *routine){
	if (routine->isEmptyBodyRoutine()) return;
	
	//for each basic block in the routine
	ArrayIterator<BasicBlockNode*>* bbIter = routine->getCFG().createDFSBBIterBack();
	while ( bbIter->hasMoreElements() ) {
		BasicBlockNode* bb = bbIter->currentElement();
		//compute the livedead info before and fater each instruction in this basic block
		//assuming that the livedead info before and after the basic block is computed already
		LiveDeadInstr_for_bb(bb);
		bbIter->nextElement();
	} 	
	
	delete bbIter;
}

