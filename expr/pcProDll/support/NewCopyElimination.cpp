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
//=================================================================
// copy elimination
//
//======================================================================

#include "stdafx.h"

#include "BranchProperty.h"
#include "Routine.h"
#include "BasicBlockNode.h"
#include "BaseOperation.h"
#include "JumpOp.h"
#include "BaseInstruction.h"
#include "NormalInstr.h"
#include "BaseOpSlot.h"
#include "DataOpSlot.h"
#include "ControlOpSlot.h"
#include "FlowOpSlot.h"
#include "BBIterator.h"
#include "DeleteProperty.h"
#include "DUProperty.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

extern OpList deletedOperList;

extern Vector<int> numDeletedArray;

extern void deleteUselessIR(Routine *rout);

extern void markOp(BaseOperation *oper);

extern int isMove(BaseOperation *oper);
extern int convertReg(int & a, BaseArgument *argPtr);

extern int hasDoubleArgs(BaseOperation *oper);

BaseArgument *getMoveSource(BaseOperation *oper)
{
	// Note: We assume that this is indeed a move operation.
	//
	if (oper->hasOperand(_SOURCE_1_))
		return oper->ptrToOperand(_SOURCE_1_);

	return NULL;
}

BaseArgument *getMoveDest(BaseOperation *oper)
{
	// Note: We assume that this is indeed a move operation.
	//
	if (oper->hasOperand(_DEST_, _DEST_LIST_))
		return oper->ptrToOperand(_DEST_, _DEST_LIST_);

	return NULL;
}

void markIfDeadMove(BaseOperation *oper)
{
	// If it is not a move, do not mark it.
	//
	// If it is a move and has more than 1 source (or more than 1 dest), do not mark it.
	//
	// If it has the same dest and source, mark it.

	// Note: Currently, we do not eliminate moves of doubles 
	// (even if dest and source have the same register pair).

	if (oper == NULL) return;
	
	if (!oper->isComputeOp()) return;

	// check to see if it is a move op.
	//
	if (!isMove(oper)) return;

	if (hasDoubleArgs(oper)) return;

	BaseArgument *srcarg, *destarg;

	srcarg = getMoveSource(oper);
	destarg = getMoveDest(oper);

	// if srcarg is not of register type, do not mark.
	//
	if (!srcarg->isRegisterArg()) return;

	int asrc, bdest;

	convertReg(asrc, srcarg);
	convertReg(bdest, destarg);

	if (asrc == bdest)
		markOp(oper);
}

void markDeadMoveOpsInInstr(BaseInstruction *instr)
{
	// Go thru the data-ops and search for those without any UD properties.
	//
	ArrayIterator<DataOpSlot> *iter = ((NormalInstr *)instr)->getDataOpSlotIter();
	while (iter->hasMoreElements())
	{
		if ((iter->currentElement()).hasOper())
		{
			markIfDeadMove((iter->currentElement()).getPtrToOperation());
		}
		iter->nextElement();
	}
	delete iter;
}

void markDeadMoveOperations(Routine *rout)
{
	// Moves can be marked as dead if the source and the destination are the same.

   MyLinkedListIterator<BaseInstruction*>* iter = rout->instrListIteratorForw();
   while ( iter->hasMoreElements() ) 
   {
      BaseInstruction* instr = iter->currentElement();
	  markDeadMoveOpsInInstr(instr); 
	  iter->nextElement();
   }
   delete iter;

}

void initDMEStatsObject()
{
	// 0 == FLOW OP.
	// 1 == DATA OP.
	// 2 == CONTROL OP.
	// 3 == INSTR.
	// 4 == BB.

	numDeletedArray.clear();

	for (int i = 0; i < 5; i++)
	{
		numDeletedArray.push_back(0);
	}
}

void displayDMEStats()
{
	cout << "---------------------------------------" << endl;
	cout << "-- Num. Move Ops Eliminated: " << numDeletedArray[1] << " ------" << endl;
//	cout << "-- Num. Control Ops Eliminated: " << numDeletedArray[2] << " ---" << endl;
//	cout << "-- Num. Flow Ops Eliminated: " << numDeletedArray[0] << " ------" << endl;
	cout << "-- Num. Instructions Eliminated: " << numDeletedArray[3] << " --" << endl;
//	cout << "-- Num. Basic Blocks Eliminated: " << numDeletedArray[4] << " --" << endl;
	cout << "---------------------------------------" << endl;
}

void
Routine::copyElim() {

	// Copy elimination proceeds in two phases:
	//
	// First, identify moves with same source and dest.
	//
	// Second, delete marked moves, and maintain the cfg structure.

	// First mark the operations to be deleted.
	//
	cout << "Marking moves with identical source and dest ...." << endl;
	markDeadMoveOperations(this);

   // Maintain the DU properties of all affected nodes.
   //
   deletedOperList.clear();

   // Then perform actual deletion of the operations/instructions/basic-blocks.
   //
   cout << "Performing dead move deletion ..." << endl;
   initDMEStatsObject();
   deleteUselessIR(this);
   displayDMEStats();

//   copyProp();
}

