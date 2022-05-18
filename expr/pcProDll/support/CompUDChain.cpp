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
// File:          CompUDChain.cc
// Purpose:       compute ud chain and du chain
// Create:        Oct. 27, 97
// Author:        Xiaomei Ji
// 
// assumption:    only compute RealRegArg's ud and du
//=========================================================================

//$Id
#include "stdafx.h"

#include <iostream.h>

#ifndef WIN32
#include <stl.h>
#endif

#include <assert.h>
#include "UDProperty.h"
#include "DUProperty.h"
#include "BaseInstruction.h"
#include "MyLinkedList.h"
#include "BasicBlockNode.h"
#include "NormalInstr.h"
#include "BaseArgument.h"
#include "Array.h"
#include "DataOpSlot.h"
#include "ControlOpSlot.h"
#include "IfOp.h"
#include "ComputeOp.h"
#include "MemoryOp.h"
#include "Routine.h"
#include "BaseInstruction.h"
#include "BBIterator.h"
#include "RealRegArg.h"
#include "PropertyList.h"
#include "SymbolTable.h"
#include "BasicBlockNode.h"
#include "CallOp.h"


//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 


#define FINDDEFWITHINBB 1
#define FINDDEFINREACHDEF 0

// In case of forwarding, when some reads are done at the end of the cycle,
// after the write
extern bool allowDelayedReads;
extern int isDelayedRead(BaseArgument *arg, BaseOperation *op);

extern SymbolTable* globalSymbolTable;

// Some comments on the next functions were added by Radu, Sep 29, 1999


//===========================================================================
// SameSymbol
//
// test if the two symbols given as arguments are the same
//===========================================================================

int
SameSymbol(BaseArgument* source, BaseArgument* targ) {
	// Added by SAH on Wednesday, Jan 20, 99
	//
	// The condition when source or targ == NULL was not taken care of.
	// I have modified this to return _NO_ when any of the arguments is NULL
	
	if ((source == NULL) || (targ == NULL)) return _NO_;
	
	if (((RegisterArg *)source)->getUniqID() == ((RegisterArg *)targ)->getUniqID())
		return _YES_;
	//
	// was previously :
	// if ( globalSymbolTable->getID(source, source->getPtrToProperty(_SSA_)) 
	//  	== globalSymbolTable->getID(targ, targ->getPtrToProperty(_SSA_)) ) 
	//	  return _YES_;
	return _NO_;
}



//===========================================================================
// linkUDDU
//
// add useSt operation to def's DU-chain
// add defSt operation to use's UD-chain
//===========================================================================

void
linkUDDU(BaseArgument* def, BaseArgument* use, 
         BaseOperation* defSt, BaseOperation* useSt) {
	if(def==NULL){
		defSt->print(0);
		useSt->print(0);
		cout << endl;
		abort();
	}
	DUChainProperty* duProp=(DUChainProperty*)def->getPtrToProperty(_DUCHAIN_);
	if ( duProp ) 
		duProp->addUse(useSt);
	else {
		DUChainProperty tmpProp;
		tmpProp.appendUse(useSt);
		def->addProperty(tmpProp);
	}
	UDChainProperty* udProp = (UDChainProperty*)use->getPtrToProperty(_UDCHAIN_);
	if ( udProp ) {
		udProp->addDef(defSt);
	} else {
		UDChainProperty tmpProp;
		tmpProp.appendDef(defSt);
		use->addProperty(tmpProp);
	}
	/*
	cout << endl;
	cout << " Def : ";
	defSt->shortPrint(cout);
	cout << endl;
	cout << "Use : ";
	useSt->shortPrint(cout);
	cout << endl;
	*/
}

//===========================================================================
// FindDefInOps
//
// look for definitions of 'arg' in the list of operations 'in' 
// if found, link ud and du chains between def and use of arg
//===========================================================================

static
int 
FindDefInOps(BaseArgument* arg, SetList<BaseOperation*>&in, BaseOperation* use,
			 int range ){
	MyLinkedListIterator<BaseOperation*>* iter = in.elementsForward();
	while ( iter->hasMoreElements() ) {
		BaseOperation* def = iter->currentElement();
		
		/*
		BaseArgument* defArg = def->ptrToOperand(_DEST_, _DEST_LIST_);
		if ( SameSymbol(defArg, arg) ) {
		// link ud and du
		linkUDDU(defArg, arg, def, use);
		if ( range == FINDDEFWITHINBB )  {
		delete iter;
		return  _YES_;
		}
		}
		*/
		
		// Changed by SAH : Wed, 16th August, 2000
		//
		// MULTIPLE_DESTS_AND_SOURCES
		//
		ArgList & a = def->destOperandList();
		ArgListIter ai;
		for (ai = a.begin(); ai != a.end(); ai++)
		{
			if ((*ai) != NULL)
			{
				if (SameSymbol((*ai), arg))
				{
					linkUDDU((*ai), arg, def, use);
					
					// Note: Because of RSSA, there can be only one definition of the symbol in the basic block.
					// So, we don't need to go thru the entire list of destinations.
					//
					if (range == FINDDEFWITHINBB)
					{
						delete iter;
						return _YES_;
					}
				}
			}
		}
		
		iter->nextElement();
	}
	delete iter;
	return  _NO_;
} 




//===========================================================================
// FindDefInPrevInstr
//
// look for all the definitions of 'arg' in all the previous instructions 
// (data and control slots)
// if found, link ud and du chains between any pair of def-use of arg
//===========================================================================

static
int
FindDefInPrevInstr(BaseArgument* arg,BaseOperation* useOp,BBBackIterator iter){
	BaseInstruction* instr = iter.currentElement();
	if ( iter.hasMoreElements() )
		iter.prevElement();
	while ( iter.hasMoreElements() ) {
		BaseInstruction* prevInstr = iter.currentElement();
		
		ArrayIterator<DataOpSlot>* opSlotIter = ((NormalInstr*)prevInstr)->getDataOpSlotIter();
		while ( opSlotIter->hasMoreElements() ) {
			BaseOperation* op = opSlotIter->currentElement().getPtrToOperation();
			if ( op == NULL ) {
				opSlotIter->nextElement();
				continue;
			}
			// because of RSSA, no same arg definition in same bb 
			// compute operation &&  memory operation
			/*
			if ( op->isComputeOp()  || 
			op->isMemoryOp() && ((MemoryOp*)op)->isLoad()
			|| op->isCallOp() && ((CallRetOp*)op)->hasOperand(_DEST_, _DEST_LIST_) ) {
			BaseArgument* defArg = op->ptrToOperand(_DEST_, _DEST_LIST_);
			if ( SameSymbol(arg, defArg) ) {
			linkUDDU(defArg, arg, op, useOp);
			delete opSlotIter;
			return _YES_;
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
				{
					if (SameSymbol(arg, (*ai)))
					{
						linkUDDU(*ai, arg, op, useOp);
						delete opSlotIter;
						return _YES_;
					}
				}
				
			}
			
			opSlotIter->nextElement();
		}	
		
		delete opSlotIter;
		
		ArrayIterator<ControlOpSlot>* cntlIter = ((NormalInstr*)prevInstr)->getControlOpSlotIter();
		while ( cntlIter->hasMoreElements() ) {
			BaseOperation* op = cntlIter->currentElement().getPtrToOperation();
			if ( op == NULL ) {
				cntlIter->nextElement();
				continue;
			}
			/*
			if ( op->isCallRetOp() && ((CallRetOp*)op)->hasOperand(_DEST_, _DEST_LIST_) ) { 
			BaseArgument* defArg = op->ptrToOperand(_DEST_, _DEST_LIST_);
			if ( SameSymbol(arg, defArg) ) {
			linkUDDU(defArg, arg, op, useOp);
			delete cntlIter;
			return _YES_;
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
				{
					if (SameSymbol(arg, (*ai)))
					{
						linkUDDU(*ai, arg, op, useOp);
						delete opSlotIter;
						return _YES_;
					}
				}
			}
			
			cntlIter->nextElement();
		}
		delete cntlIter;
		
		
		
		iter.prevElement();
	}
	return _NO_;
}


//===========================================================================
// FindDefInCrtInstr
//
// look for all the definitions of 'arg' in the current instruction
// (data and control slots)
// if found, link ud and du chains between any pair of def-use of arg
//===========================================================================

// the body of this function was copied and modified from FindDefInPrevInstr()

static
int
FindDefInCrtInstr(BaseArgument* arg,BaseOperation* useOp,BBBackIterator iter){
	BaseInstruction* crtInstr = iter.currentElement();
	
	ArrayIterator<DataOpSlot>* opSlotIter = ((NormalInstr*)crtInstr)->getDataOpSlotIter();
	while ( opSlotIter->hasMoreElements() ) {
		BaseOperation* op = opSlotIter->currentElement().getPtrToOperation();
		if ( op == NULL ) {
			opSlotIter->nextElement();
			continue;
		}
		// because of RSSA, no same arg definition in same bb 
		// compute operation &&  memory operation
		/*
		if ( op->isComputeOp()  || 
		op->isMemoryOp() && ((MemoryOp*)op)->isLoad()
		|| op->isCallOp() && ((CallRetOp*)op)->hasOperand(_DEST_, _DEST_LIST_) ) {
		BaseArgument* defArg = op->ptrToOperand(_DEST_, _DEST_LIST_);
		if ( SameSymbol(arg, defArg) ) {
		linkUDDU(defArg, arg, op, useOp);
		delete opSlotIter;
		return _YES_;
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
			{
				if (SameSymbol(arg, (*ai)))
				{
					linkUDDU(*ai, arg, op, useOp);
					delete opSlotIter;
					return _YES_;
				}
			}
			
		}
		
		opSlotIter->nextElement();
	}	
	
	delete opSlotIter;
	
	ArrayIterator<ControlOpSlot>* cntlIter = ((NormalInstr*)crtInstr)->getControlOpSlotIter();
	while ( cntlIter->hasMoreElements() ) {
		BaseOperation* op = cntlIter->currentElement().getPtrToOperation();
		if ( op == NULL ) {
			cntlIter->nextElement();
			continue;
		}
		/*
		if ( op->isCallRetOp() && ((CallRetOp*)op)->hasOperand(_DEST_, _DEST_LIST_) ) { 
		BaseArgument* defArg = op->ptrToOperand(_DEST_, _DEST_LIST_);
		if ( SameSymbol(arg, defArg) ) {
		linkUDDU(defArg, arg, op, useOp);
		delete cntlIter;
		return _YES_;
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
			{
				if (SameSymbol(arg, (*ai)))
				{
					linkUDDU(*ai, arg, op, useOp);
					delete opSlotIter;
					return _YES_;
				}
			}
		}
		
		cntlIter->nextElement();
	}
	delete cntlIter;
	
	return _NO_;
}



//===========================================================================
// CompUDOfArg
// 
// compute ud chain of arg (serch in the previous instructions; if not found, 
// search in the current one).
//===========================================================================

static
void
compUDOfArg(BaseArgument* arg, SetList<BaseOperation*>& in,
			BaseOperation* op, BBBackIterator iter) {
	
	// assume real register argument
	if ( arg == NULL || arg->isRealRegArg() == _NO_ )
		return;
	
	// added by Radu, 02/05/2002, to support correct UDDU analysis in the
	// presence of delayed reads
	if (allowDelayedReads && isDelayedRead(arg, op)) {
		int found = FindDefInCrtInstr(arg, op, iter);
		ASSERT_TRACE(found, "Definition for a delayed read argument is not in the same instruction");
		return;
	}

	if ( FindDefInPrevInstr(arg, op, iter) ) 
		return;
	FindDefInOps(arg, in, op, FINDDEFINREACHDEF);
	
}

//=========================================================================
// compUDOfInstr
//
// compute ud chain of all used arguments in one instruction
//
// to used variables in normal instruction
// find the definition in the basic block's gen set and in set
//
// to used variables ( that is the arglist ) in phi instruction
// find the def only in bb's reaching definition
//
//=========================================================================

static
void 
compUDOfInstr(BBBackIterator BBIter, SetList<BaseOperation*>& in) {
	
	BaseInstruction* instr = BBIter.currentElement();
	// data operation
	ArrayIterator<DataOpSlot>* iter = ((NormalInstr*)instr)->getDataOpSlotIter();
	while ( iter->hasMoreElements() ) {
		BaseOperation* op = iter->currentElement().getPtrToOperation();
		
		if ( op == NULL ) {
			iter->nextElement();
			continue;
		}
		
		/*
		if ( op->isComputeOp() ) {
		compUDOfArg(((ComputeOp*)op)->ptrToOperand(_SOURCE_1_),in,op,BBIter); 
		compUDOfArg(((ComputeOp*)op)->ptrToOperand(_SOURCE_2_),in,op,BBIter); 
		} else if ( op->isMemoryOp() ) {
		compUDOfArg(((MemoryOp*)op)->ptrToSourceOperand(),  in,  op, BBIter); 
		if ( ((MemoryOp*)op)->isStore() )
		compUDOfArg(((MemoryOp*)op)->ptrToDestOperand(),  in,  op,BBIter);
		}
		// Note: The below piece of code is vestegial. call ops are control ops.
		else if ( op->isCallOp() )  {
		if (((CallRetOp*)op)->hasOperand(_SOURCE_1_))
		compUDOfArg(op->ptrToOperand(_SOURCE_1_), in, op, BBIter);
		
		  if ( ((CallRetOp*)op)->hasOperand(_SOURCE_2_) )
		  compUDOfArg(op->ptrToOperand(_SOURCE_2_), in, op, BBIter);
		  ArgList&  argList = ((CallRetOp*)op)->getParmList();
		  ArgListIter argIter;
		  for (argIter = argList.begin(); argIter != argList.end(); argIter++)
		  {
		  compUDOfArg(*argIter, in, op, BBIter);
		  }
		  ArgList& savedRegs = ((CallRetOp*)op)->getSavedRegList();
		  for ( argIter = savedRegs.begin(); argIter!=savedRegs.end(); argIter++ ) 
		  compUDOfArg(*argIter, in, op, BBIter);
		  
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
				compUDOfArg((*ai), in, op, BBIter);
			}
		}
		
		
		iter->nextElement();
	}
	delete iter;
	
	// control operation
	ArrayIterator<ControlOpSlot>* cntlIter = ((NormalInstr*)instr)->getControlOpSlotIter();
	while ( cntlIter->hasMoreElements() ) {
		BaseOperation* op = cntlIter->currentElement().getPtrToOperation();
		
		if ( op == NULL ) {
			cntlIter->nextElement();
			continue;
		}
		/*
		if ( op && op->isIfOp() ) {
		compUDOfArg(((IfOp*)op)->ptrToCCReg(),  in,  op, BBIter);  
		}
		else if ( op && op->isCallOp() )  {
		// Added by SAH on Tue Dec 14, 99
		//
		if (((CallRetOp*)op)->hasOperand(_SOURCE_1_))
		compUDOfArg(op->ptrToOperand(_SOURCE_1_), in, op, BBIter);
		
		  if ( ((CallRetOp*)op)->hasOperand(_SOURCE_2_) )
		  compUDOfArg(op->ptrToOperand(_SOURCE_2_), in, op, BBIter);
		  ArgList&  argList = ((CallRetOp*)op)->getParmList();
		  ArgListIter argIter;
		  for (argIter = argList.begin(); argIter != argList.end(); argIter++)
		  {
		  compUDOfArg(*argIter, in, op, BBIter);
		  }
		  ArgList& savedRegs = ((CallRetOp*)op)->getSavedRegList();
		  for ( argIter = savedRegs.begin(); argIter!=savedRegs.end(); argIter++ ) 
		  compUDOfArg(*argIter, in, op, BBIter);
		  
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
				compUDOfArg((*ai), in, op, BBIter);
			}
		}
		
		if (op->isCallOp())
		{
			ArgListIter argIter;
			ArgList & parmRegs = ((CallRetOp*)op)->getParmList();
			for ( argIter = parmRegs.begin(); argIter!=parmRegs.end(); argIter++ ) 
				compUDOfArg(*argIter, in, op, BBIter);

			ArgList& savedRegs = ((CallRetOp*)op)->getSavedRegList();
			for ( argIter = savedRegs.begin(); argIter!=savedRegs.end(); argIter++ ) 
				compUDOfArg(*argIter, in, op, BBIter);
		}
		
		cntlIter->nextElement();
	}
	delete cntlIter;
}


//==========================================================================
// compUDOfBB
// 
// compute ud of all used arguments in one basic block
//
// argument: bb: basic block
//           iter: instruction iterator
//===========================================================================

static
void 
compUDOfBB(BasicBlockNode* bb, SetList<BaseOperation*>& in ) {
	
	BBBackIterator iter(bb);
	while ( iter.hasMoreElements() ) {
		compUDOfInstr(iter, in);
		iter.prevElement();
	}
	
}



//========================================================================
// UnionOfPred
//========================================================================

static
void
UnionOfPred(SetList<BaseOperation*>& in, BasicBlockNode* bb,
			SetList<BaseOperation*>* out  ) {
	BasicBlockPredecessors& preds = bb->getCFPred();
	MyLinkedListIterator<BasicBlockNode*>* predIter = preds.createPredListIterForw();
	int first = _YES_;
	while ( predIter->hasMoreElements() ) {
		BasicBlockNode* pred = predIter->currentElement();
		int index = pred->getDFSNum();
		if ( out[index].isEmpty() == _NO_ ) {
            if ( first ) {
                in = out[index];
                first = _NO_;
			}
            else
				in.set_union(out[index]);
		}
		predIter->nextElement();     
	}
	delete predIter;
}



//========================================================================
// MinusKill
//========================================================================

// TBD : SAH : 17th August, 2000
//
static
void
MinusKill(SetList<BaseOperation*>& out, SetList<BaseOperation*>& gen) {
	//MyLinkedList<BaseOperation*> removeSet;
	MyLinkedListIterator<BaseOperation*>* iter = out.elementsForward();
	MyLinkedListIterator<BaseOperation*>* genIter = gen.elementsForward();
	while ( iter->hasMoreElements() ) {
		BaseOperation* op = iter->currentElement();
		iter->nextElement();
		// check if this op defined the same symbol as the definition in gen
		while ( genIter->hasMoreElements() ) {
			BaseOperation* opInGen = genIter->currentElement();
			if ( op != opInGen && 
				SameSymbol(op->ptrToOperand(_DEST_, _DEST_LIST_),
				opInGen->ptrToOperand(_DEST_, _DEST_LIST_) ) )  {
				out.remove(op);
				//removeSet.append(op);
				break;
			}
			genIter->nextElement();
		}
		genIter->reset();
	}
	//out.minus(removeSet);
	delete iter;
	delete genIter;
}


//========================================================================
// compIn
//========================================================================

static
void
compIn(ArrayIterator<BasicBlockNode*>* bbIter, 
	   SetList<BaseOperation*>* gen,
	   SetList<BaseOperation*>* in,
	   SetList<BaseOperation*>* out) {
	
	while ( bbIter->hasMoreElements() ) {
		int index = bbIter->currentElement()->getDFSNum();
		out[index] = gen[index];
		bbIter->nextElement();
	}
	bbIter->reset();
	
	//out[1] = gen[1];
	
	int change = _TRUE_;
	while ( change ) {
		change = _FALSE_;
		if ( bbIter->hasMoreElements() ) 
			bbIter->nextElement();   // across the first bb
		while (bbIter->hasMoreElements() ) {
			BasicBlockNode* bb = bbIter->currentElement();
			int index = bb->getDFSNum();
			// in[] = union of (pred's out[])
			UnionOfPred(in[index], bb, out);
			SetList<BaseOperation*> oldOut = out[index];
			//out[index] = gen + ( in - kill )
			out[index] = in[index];
			MinusKill(out[index], gen[index]);
			out[index].set_union(gen[index]);
			if ( oldOut.operator!=(out[index]) ) 
				change = _TRUE_;
			bbIter->nextElement();
		}
		bbIter->reset();
	}
}


//========================================================================
// compGenOfInstr
//========================================================================

static
void
compGenOfInstr(BaseInstruction* instr, SetList<BaseOperation*>& gen) {
	ArrayIterator<DataOpSlot>* iter = ((NormalInstr*)instr)->getDataOpSlotIter();
	while ( iter->hasMoreElements() ) {
		BaseOperation* op = iter->currentElement().getPtrToOperation();
		if ( op == NULL ) {
			iter->nextElement();
			continue;
		}
		// because of RSSA, no same arg definition in same bb 
		/*
		// compute operation
		if ( op->isComputeOp() )  
		gen.append(op); 
		// memory operation
		else if (op->isMemoryOp() && ((MemoryOp*)op)->isLoad())
		gen.append(op);
		else if ( op->isCallOp() && ((CallRetOp*)op)->hasOperand(_DEST_, _DEST_LIST_) )
		gen.append(op);
		*/
		// Changed by SAH : Wed, 16th August, 2000
		//
		// MULTIPLE_DESTS_AND_SOURCES
		//
		ArgList & a = op->destOperandList();
		if (a.size() != 0)
			gen.append(op);
		
		iter->nextElement();
	}
	delete iter;
	
	
	ArrayIterator<ControlOpSlot>* cntlIter = ((NormalInstr*)instr)->getControlOpSlotIter();
	while ( cntlIter->hasMoreElements() ) {
		BaseOperation* op = cntlIter->currentElement().getPtrToOperation();
		if ( op == NULL ) {
			cntlIter->nextElement();
			continue;
		}
		/*
		if ( op->isCallOp() && ((CallRetOp*)op)->hasOperand(_DEST_, _DEST_LIST_) ) 
		gen.append(op);
		*/
		
		// Changed by SAH : Wed, 16th August, 2000
		//
		// MULTIPLE_DESTS_AND_SOURCES
		//
		ArgList & a = op->destOperandList();
		if (a.size() != 0)
			gen.append(op);
		
		cntlIter->nextElement();
	}
	delete cntlIter;
	
}


//=======================================================================
// compGenOfBB
//=======================================================================

static
void
compGenOfBB(BasicBlockNode* bb, SetList<BaseOperation*>& gen) {
	BBBackIterator bbIter(bb);
	int TEMP_DEBUG = 0;
	while ( bbIter.hasMoreElements() ) {
		compGenOfInstr(bbIter.currentElement(), gen);
		bbIter.prevElement();
		
		if (TEMP_DEBUG)
		{
			(bbIter.currentElement())->shortPrint(cout);
		}
	}
}


//========================================================================
// Routine::compUDChain
//
// compute the ud chain and du chain of a routine
//========================================================================

void
Routine::compUDChain() {
	
#ifdef DEBUG
	printf("begin to compute the ud chain and du chain of variables \n");
#endif
	
	if (isEmptyBodyRoutine()) return;
	
	int numBBs = _cfg.numBBs();
	SetList<BaseOperation*>*  gen = new SetList<BaseOperation*>[numBBs+1];
	ArrayIterator<BasicBlockNode*>* bbIter = _cfg.createDFSBBIter();
	while ( bbIter->hasMoreElements() ) {
		BasicBlockNode* bb = bbIter->currentElement();
		compGenOfBB(bb, gen[bb->getDFSNum()]);
		bbIter->nextElement();
	}
	bbIter->reset();
	
	SetList<BaseOperation*>* in = new SetList<BaseOperation*>[numBBs+1];
	SetList<BaseOperation*>* out = new SetList<BaseOperation*>[numBBs+1];
	compIn(bbIter, gen, in, out);
	bbIter->reset();
	delete [] gen;
	delete [] out;
	
	while ( bbIter->hasMoreElements() ) {
		BasicBlockNode* bb = bbIter->currentElement();
		compUDOfBB(bb, in[bb->getDFSNum()]);
		bbIter->nextElement();
	}
	
	delete bbIter;
	delete [] in;
	
#ifdef DEBUG
	printf("end of computing ud chain and duchain \n");
#endif
}
