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
// $Id$
//
// File:  		IRSupportFuncs.cpp
// Created:		Thu Dec 24, 98
// Last modified: 	Thu Dec 24, 98
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: INCLUDE

#include "stdafx.h"

#include "GlobalIncludes.h"

#include "IfOp.h"
#include "JumpOp.h"
#include "GotoOp.h"
#include "CallOp.h"
#include "MemoryOp.h"
#include "ComputeOp.h"
#include "NormalInstr.h"
#include "BasicBlockNode.h"
#include "BBIterator.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

int _LAST_FLOW_POS_;

NormalInstr *buildMIPSInstr(BaseOperation *op = NULL)
{
	NormalInstr *retPtr;
	retPtr = new NormalInstr();

	int index;

	JumpOp tempJumpOp;

	if (op == NULL)
	{
		retPtr->initAddFlowOp(&(tempJumpOp), _LAST_FLOW_POS_);
		return retPtr;
	}
	retPtr->initScheduleAndAdd(op);

	if (op->isDataOp())
	{   
	   index = retPtr->addFlowOpToList(&(tempJumpOp), _LAST_FLOW_POS_);

	   (retPtr->getRootOpSlot()).setNext(&(retPtr->getFlowOpSlot(index)));
	}
	else if (op->isControlOp())
	{
	   index = retPtr->addFlowOpToList(&(tempJumpOp));
	   (retPtr->getRootOpSlot()).setTrueBranch(&(retPtr->getFlowOpSlot(index)));
	   
	   index = retPtr->addFlowOpToList(&(tempJumpOp), _LAST_FLOW_POS_);
	   (retPtr->getRootOpSlot()).setFalseBranch(&(retPtr->getFlowOpSlot(index)));
	}
	else // if (op->isFlowOp())
	{
	}

   return retPtr;
}

void insertInstructionIntoIR(BaseInstruction *newInstr, BaseInstruction *prevInstr, BasicBlockNode *thisBB,  int nextInstrDir)
{
	int isLast = 0;

	if (newInstr == NULL)
	{
		// error condition.
		cout << "Panic: Cannot insert NULL instruction into the IR" << endl;
		return;
	}

	if (prevInstr == NULL)
	{
		// error condition.
		cout << "Panic: Cannot insert instruction into the IR with previous instr = NULL" << endl;
		return;
	}

	if (thisBB->hasInstr(prevInstr) == _NO_)
	{
		// error condition.
		cout << "Panic: The basic block does not contain the previous instruction" << endl;
		return;
	}

	if (prevInstr == thisBB->getLastInstr())
	{
		isLast = 1;
		if ((thisBB->getFirstEdge() != NULL) && (thisBB->getNextEdge() != NULL))
		{
			// i.e. the basic block has more than 1 successor.
			cout << "Warning: The case of inserting an instruction after an IF stmt is not handled" << endl;
			return;
		}
		((NormalInstr *)newInstr)->appendInstr(prevInstr, nextInstrDir, 1);
		thisBB->initTrailer(newInstr);
	}
	else // not the last instr of a basic block. can just append it to the IList.
	{
		((NormalInstr *)newInstr)->appendInstr(prevInstr, nextInstrDir, 1);
	}
}

BaseInstruction *findPrevInstr(BasicBlockNode* bb, BaseInstruction* instr) 
{
	BaseInstruction* prev = NULL;
	BBForwIterator instrIter(bb);
	while ( instrIter.hasMoreElements() ) 
	{
		if ( instrIter.currentElement() == instr ) 
            return prev;
		
		prev = instrIter.currentElement();
		instrIter.nextElement();
	}
	
	//NOS
	ASSERT_TRACE(0,"Forced EXIT!");
	return NULL;
}

BaseInstruction *findNextInstr(BasicBlockNode* bb, BaseInstruction* instr) 
{
	BaseInstruction* next = NULL;
	BBBackIterator instrIter(bb);
	while ( instrIter.hasMoreElements() ) 
	{
		if ( instrIter.currentElement() == instr ) 
            return next;
		
		next = instrIter.currentElement();
		instrIter.prevElement();
	}
	
	//NOS
	ASSERT_TRACE(0,"Forced EXIT!");
	return NULL;
}

void setTargetOfNormalInstr(NormalInstr* predInstr, BaseInstruction* instr, BaseInstruction* target) 
{
	int find = 0;
	ArrayIterator<FlowOpSlot>* flowIter = predInstr->getFlowOpSlotIter();
	while ( flowIter->hasMoreElements() ) 
	{
		BaseOperation* flowOp = flowIter->currentElement().getPtrToOperation();
		// it is possible that two flows point to the same instr
		if ( flowOp && ((JumpOp*)flowOp)->getTargetInstr() == instr ) 
		{
			((JumpOp*)flowOp)->setTargetInstr(target);
			predInstr->removeSuccID(instr->idValue());
			find = 1;
		}
		flowIter->nextElement();
	}
	delete flowIter;
	// maintain _listOfSuccessorIDs
	if ( find && target )
		predInstr->addSuccID(target->idValue());
}

void setPredOfNormalInstr(BaseInstruction* nextInstr, BaseInstruction* instr, NormalInstr * pred) 
{
	int find = 0;
	ArrayIterator<FlowOpSlot>* flowIter = pred->getFlowOpSlotIter();
	while ( flowIter->hasMoreElements() ) 
	{
		BaseOperation* flowOp = flowIter->currentElement().getPtrToOperation();

		// we are guaranteed that the pred instr has only one flow op.
		//
		if ( flowOp )
		{
			BaseInstruction *temp = ((JumpOp*)flowOp)->getTargetInstr();

			((JumpOp*)flowOp)->setTargetInstr(nextInstr);
			pred->removeSuccID(temp->idValue());
			find = 1;
		}
		flowIter->nextElement();
	}
	delete flowIter;
	// maintain _listOfSuccessorIDs
	if ( find && nextInstr )
		pred->addSuccID(nextInstr->idValue());
}

// Use the next two functions to essentially de-link an instruction (or instructions) from the IR.

// Before:
// (I_pred[1..n]) -> instr -> target
//
// After:
// (I_pred[1..n]) -> target
//
// if changeLeaderTrailer == 1, then modify the bb's headers and trailers (if necessary).
//
void setTargetInstrOfPred(BasicBlockNode* bb, BaseInstruction* instr, BaseInstruction* target, int changeLeaderTrailer) 
{
	if (instr != bb->getFirstInstr())
	{
		// prev instr in this bb's target points to this instr's target
		BaseInstruction* prevInstr = findPrevInstr(bb, instr);
		setTargetOfNormalInstr((NormalInstr*)prevInstr,instr, target); 

		if (changeLeaderTrailer)
			if ( bb->getLastInstr() == instr )
				bb->initTrailer(prevInstr);
	}
	else
	{
		// bb's pred's flow op whose target is this instr
		// points to the instr's target
		MyLinkedListIterator<BasicBlockNode*>* predIter = bb->getCFPred().createPredListIterForw();
		if ( predIter != _NULL_ ) 
		{
			while ( predIter->hasMoreElements() ) 
			{
				BasicBlockNode* pred = predIter->currentElement();
				BaseInstruction* predInstr = pred->getLastInstr();

				if (predInstr==NULL) // i.e the previous bb was a dummy bb
				{
					// do nothing.
				}
				else
				{
					setTargetOfNormalInstr((NormalInstr*)predInstr, instr, target);
				}
				predIter->nextElement();
			}
			delete predIter; 
		}

		if (changeLeaderTrailer)
			bb->initLeader(target);
	}
}

// Before:
// pred -> instr -> (I_target[1..n])
//
// After:
// pred -> (I_target[1..n])
//
// if changeLeaderTrailer == 1, then modify the bb's headers and trailers (if necessary).
//
void setPredInstrOfTarget(BasicBlockNode *bb, BaseInstruction *instr, BaseInstruction *pred, int changeLeaderTrailer)
{
	if (instr != bb->getLastInstr())
	{
		// next instr is in this bb.
		BaseInstruction *nextInstr = findNextInstr(bb, instr);
		setPredOfNormalInstr(nextInstr, instr, (NormalInstr *)pred);

		if (changeLeaderTrailer)
			if (bb->getFirstInstr() == instr)
				bb->initLeader(nextInstr);
	}
	else
	{
		BasicBlockNode *target;
		BaseInstruction *nextInstr;

		target = bb->getFirstSucc();
		if (target != NULL)
		{
			nextInstr = target->getFirstInstr();
			setPredOfNormalInstr(nextInstr, instr, (NormalInstr *)pred);
		}

		target = bb->getNextSucc();
		if (target != NULL)
		{
			nextInstr = target->getFirstInstr();
			setPredOfNormalInstr(nextInstr, instr, (NormalInstr *)pred);
		}

		if (changeLeaderTrailer)
			bb->initTrailer(pred);
	}
}

// This function reorders the regions within a basic-block.
//
// let R1 = {start1, end1} and R2 = {start2, end2} be two non-overlapping regions in bb. ... R1 ... R2 ...
// then after this function, the order becomes : ... R2 ... R1 ...
//
// Note: This also takes care of boundary condtions such as R1 = basicblock begin or R2 = basicblock end.
//
void reorderRegions(BasicBlockNode *bb, BaseInstruction *start1, BaseInstruction *end1, BaseInstruction *start2, BaseInstruction *end2)
{
	// The bb looks like this:
	//
	//
	//  T1 -> start1 -> end1 -> T2 -> T3 -> start2 -> end2 -> T4
	//
	// need to make it into:
	//
	//  T1 -> start2 -> end2 -> T2 -> T3 -> start1 -> end1 -> T4
	//
	// Note: T2, T3 are guaranteed to be within the basic block.
	// T1, T4 are not.

	BaseInstruction *t2, *t3;

	// Store t2 and t3.
	//
	t2 = findNextInstr(bb, end1);
	t3 = findPrevInstr(bb, start2);

	// setTargetInstrOfPred finds the predecessor of start1 and makes it point to start2.
	//
	setTargetInstrOfPred(bb, start1, start2, 0); // do not modify the leader and trailer of bb.
	//
	// now it looks like this:
	//
	//  T1 -> start2 -> end2 -> T4     start1 -> end1 -> T2 -> T3

	setPredInstrOfTarget(bb, end2, end1, 0); // do not modify the leader and trailer of bb.
	//
	// now it looks like this:
	//
	//  T1 -> start2 -> end2      start1 -> end1 -> T4    T2 -> T3

	// finally, need to make end2 point to T2 and T3 point to start1
	//
	// however, if the regions were T1 -> start1 -> end1 -> start2 -> end2
	// then make end2 point to start1 and that's it.
	if ((t2 != start2) && (t3 != end1)) // Note: The second comparision is unnecessary.
	{
		end2->setNextInstr(t2);
		t3->setNextInstr(start1);
	}
	else
		end2->setNextInstr(start1);

	// make sure that the bb's leaders and trailers are properly initialized.
	//
	if (start1 == bb->getFirstInstr())
	{
		bb->initLeader(start2);
	}
	if (end2 == bb->getLastInstr())
	{
		bb->initTrailer(end1);
	}
}
