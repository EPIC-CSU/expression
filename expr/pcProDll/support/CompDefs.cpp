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
// File:	CompDefs.cc
// Purpose:     compute the defined variables of a basic block,
//		record them into this basic block's property
//              pls. note that all variables in the file refer to Register Arg
// Creat: 	Oct. 23, 97
// Authur:	Xiaomei Ji
//==========================================================================

#include "stdafx.h"

#include <iostream.h>

#include "MyLinkedListIterator.h"
#include "BaseInstruction.h"
#include "BasicBlockNode.h"
#include "BaseBBProperty.h"
#include "CompDefBBProperty.h"
#include "NormalInstr.h"
#include "MemoryOp.h"
#include "ComputeOp.h"
#include "BaseOperation.h"
#include "DataOpSlot.h"
#include "Array.h"
#include "Routine.h"
#include "BBIterator.h"
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

Routine*	currentRoutine;

//========================================================================
// compDefsOfInstr
//
// compute the definition variables in an instruction
// only three kinds of opertations :
// computeOp, phiOp, MemoryOp's ldOp;
// have the definition side-effect
//
// argument: 	instr:the instruction 
//		prop: the property of a basic block to which the instr belongs
//=======================================================================

static
void
compDefsOfInstr(BaseInstruction* instr, CompDefBBProperty& prop) {
#ifdef DEBUG
	printf(" begin to compute the def var of instr 0x%x\n", instr->idValue());
	instr->print(cout);
#endif
	ArrayIterator<DataOpSlot>* iter = ((NormalInstr*)instr)->getDataOpSlotIter();
	while ( iter->hasMoreElements() ) {
		BaseOperation* op = iter->currentElement().getPtrToOperation();
		if ( op == NULL ) {
			iter->nextElement();
			continue;
		}
/*
		// compute operation
		if ( op->isComputeOp() ) 
			prop.appendDefinition(((ComputeOp*)op)->ptrToOperand(_DEST_, _DEST_LIST_));
		// memory operation
		else if ( op->isMemoryOp() && ((MemoryOp*)op)->isLoad() )
			prop.appendDefinition(((MemoryOp*)op)->ptrToDestOperand());
		else if ( op->isCallOp() && ((CallRetOp*)op)->hasOperand(_DEST_, _DEST_LIST_) ) 
			prop.appendDefinition(((CallRetOp*)op)->ptrToOperand(_DEST_, _DEST_LIST_));
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
				prop.appendDefinition(*ai);
		}
		//
		// end change for MULTIPLE_DESTS_AND_SOURCES

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
			prop.appendDefinition(((CallRetOp*)op)->ptrToOperand(_DEST_, _DEST_LIST_));
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
				prop.appendDefinition(*ai);
		}
		//
		// end change for MULTIPLE_DESTS_AND_SOURCES

		cntlIter->nextElement();
	}
	delete cntlIter;

}


//=========================================================================
// compDefsOfBB
//
// compute the defined variables of a basic block
// 
// Argument: basic block
// 
// now, two arguments, should be changed later
//========================================================================

static
void
compDefsOfBB(BasicBlockNode& bb) {
#ifdef DEBUG
 //	printf(" begin to compute the def set of basic block %d\n", bb.getDFSNum());
#endif
	// by the following method, even if a basic block has not def property
	// it will has the perproty object, 
	// so, it incurs redundancy of property,
	// but reduce cost of copy operation.
	// or, can first scan instructions in a bb, 
	// then, make decision whether a bb should has def property
	// this can be done after the iteration of instr in bb become efficient 
 
	CompDefBBProperty	tmpProp;
	bb.addProperty(tmpProp);
	CompDefBBProperty& prop = (CompDefBBProperty&)bb.getPropList().getTail();
 
	int TEMP_DEBUG = 0;
 BasicBlockNode *tbb = &bb;
 BBForwIterator bbIter(tbb);

 BaseInstruction *temp;
 while ( bbIter.hasMoreElements() ) {
		temp = bbIter.currentElement();

		if (TEMP_DEBUG)
		{
			temp->shortPrint(cout);
		}
		compDefsOfInstr(temp, prop);
		bbIter.nextElement();
 }
}


//=========================================================================
// Routine::compDefs
//
// compute the defined variables of a routine
//=========================================================================

void 
Routine::compDefs() {
#ifdef DEBUG
	printf(" Begin to compute the def set\n");
#endif
 
 currentRoutine = this;
 
 if (isEmptyBodyRoutine()) return;

	LinkedListIterator<BasicBlockNode>* bbIter=_cfg.createBBIterForw();
	while ( bbIter->hasMoreElements() ) {
  //		cout << "***** Executing CompDefs for BasicBlock: " << &(bbIter->currentElement()) << endl;
		compDefsOfBB(bbIter->currentElement());
		bbIter->nextElement();
	} 	
 
	delete bbIter;
 
 
#ifdef DEBUG
	printf(" end of computing the def set \n");
#endif
}
