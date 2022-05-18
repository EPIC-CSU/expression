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
// File:  		InitCall.cpp
// Created:		Tue Dec 02, 98
// Last modified: 	Tue Dec 02, 98
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
// 

// This file contains the routines needed to initialize
// the routine pointers in each call operation.
// Note: Each call (which has a label as its call address) has a pointer to a routine.
// This pointer to routine is used by other passes (such as the simulator).
//
// This routine pointer could have been set during the time we parse the original .s file
// That way, we could have avoided having to go thru each instruction and searching for
// call ops. (In that case, each routine must be declared before it is used.)

// However, I think that when we have multiple files (and extern functions), 
// the above mentioned approach wont work.

// Note: We could have created a global list of CallOps (to which each call op is added
// at the moment of its creation). However, I chose this way, because it is not too expensive,
// and seemed to have a better separation of concerns. 

#include "stdafx.h"

#include "GlobalIncludes.h"
#include "STLIncludes.h"

#include "BaseOpSlot.h"
#include "CallOp.h"
#include "Program.h"
#include "Routine.h"
#include "MyLinkedList.h"
#include "MyLinkedListIterator.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

void initRoutineTable(Program *prog, RoutineTable & table)
{
	MyLinkedListIterator<Routine *> *iter;
	iter = prog->routineListIteratorForw();

	Routine *curR;
	char *rName;

	while (iter->hasMoreElements())
	{
		curR = iter->currentElement();
		if ((rName = curR->getName()) != NULL)
		{
			table[MyString(rName)] = curR;
		}
		iter->nextElement();
	}
	delete iter;
}

void checkInstrAndSetCallOpList(BaseInstruction *instr, OpList & callList, OpSlotList & slotList)
{
	if (instr == NULL) return;
	if (!instr->isNormalInstr())
	{
		cout << "Warning: Encountered a non Normal Instruction during initCallPass" << endl;
		return;
	}
	NormalInstr *nInstr = ((NormalInstr *)instr);

	// Depending on the particular target architecture and compiler implementation,
	// the call op may be either a data op or a control op.

	ArrayIterator<DataOpSlot> *diter;
	diter = nInstr->getDataOpSlotIter();

	BaseOperation *curOp;
	while (diter->hasMoreElements())
	{
		// Now check each dataop slot to see if it contains a call op.
		if ((diter->currentElement()).hasOper())
		{
			curOp = (diter->currentElement()).getPtrToOperation();
			if ((curOp != NULL) && (curOp->isCallOp()))
			{
				callList.push_back(curOp);
				slotList.push_back(&(diter->currentElement()));
			}
		}
		diter->nextElement();
	}
	delete diter;

	ArrayIterator<ControlOpSlot> *citer;
	citer = nInstr->getControlOpSlotIter();

	while (citer->hasMoreElements())
	{
		// Now check each controlop slot to see if it contains a call op.
		if ((citer->currentElement()).hasOper())
		{
			curOp = (citer->currentElement()).getPtrToOperation();
			if ((curOp != NULL) && (curOp->isCallOp()))
			{
				callList.push_back(curOp);
				slotList.push_back(&(citer->currentElement()));
			}
		}
		citer->nextElement();
	}
	delete citer;
}

// If you just need to set the CFG (call) graph correctly, then make setSavedRegs = 0
//
void setCalls(Routine *curR, RoutineTable & table, int setSavedRegs = 1)
{
	if (curR == NULL) return;
	
	MyLinkedListIterator<BaseInstruction *> *iter;
	iter = curR->instrListIteratorForw();

	OpList callList;
	OpSlotList slotList;

	OpListIter i;
	OpSlotListIter j;

	BaseOpSlot *fslot;

	RoutineTableIter tableIter;

	BaseArgument *arg;
	ArgList sList;
	while (iter->hasMoreElements())
	{
		// Check each instruction for call ops and
		// set the routine pointer, saved reg list of those ops.
		//
		checkInstrAndSetCallOpList(iter->currentElement(), callList, slotList);

		// For each call op in the list, set the routine pointer, set the branch instruction, saved reg list.
		//
		for (i = callList.begin(), j = slotList.begin(); i != callList.end(); i++, j++)
		{
			arg = (*i)->ptrToOperand(_SOURCE_1_);
			if ((arg != NULL) && (arg->isLabelArg()))
			{
				// First make a check to see if the label is really present in the table.
				//
				for (tableIter = table.begin(); tableIter != table.end(); tableIter++)
				{
					if ((*tableIter).first == arg->lName()) break;
				}
				if (tableIter != table.end())
				{
					// first set the routine pointer
					//
					((CallRetOp *)(*i))->setRoutine((*tableIter).second);
					
					// then set the flow op (of the true branch) to the next instruction.
					//
					if ((fslot = (*j)->getPtrToTrueBranch()) != NULL)
					{
						if (fslot->hasOper()) // i.e the flow op slot has already been filled with an operation)
						{
							(fslot->getPtrToOperation())->setTargetInstr(((*tableIter).second)->getFirstInstr());
						}
						else
						{
							// currently do nothing.
							cout << "Warning: The true branch of the call op does not have any flow op" << endl;
						}
					}
					else
					{
						// error condition. we expect that the true branch of a call op leads to a flow slot.
						cout << "panic: A call op should have a true branch." << endl;
						assert(0);
					}

					// next, set the list of saved regs
					//
					((*tableIter).second)->getSavedRegList(sList);
					((CallRetOp *)(*i))->initSavedRegList(sList);
					sList.clear();

					// Finally, indicate that this routine is "reachable".
					// i.e. there exists a path from the main routine to this routine.
					//
					((*tableIter).second)->setIsUsedInProgram();
				}
			}
		}
		iter->nextElement();
		callList.clear();
		slotList.clear();
	}
	delete iter;
}

// Sets the flow pointers from Call Ops to the corresponding Routine's first instruction.
void setCallPointers(Program *prog)
{
	if (prog == NULL) return;

	RoutineTable routineTable;

	initRoutineTable(prog, routineTable);

	MyLinkedListIterator<Routine *> *iter;
	iter = prog->routineListIteratorForw();

	while (iter->hasMoreElements())
	{
		setCalls(iter->currentElement(), routineTable, 0);
		iter->nextElement();
	}
	delete iter;

	// finally, set the main program to "IsUsed" status.
	//
	(prog->getMainRoutine())->setIsUsedInProgram();
}

void initCallPass(Program *prog)
{
	if (prog == NULL) return;

	RoutineTable routineTable;

	initRoutineTable(prog, routineTable);

	MyLinkedListIterator<Routine *> *iter;
	iter = prog->routineListIteratorForw();

	while (iter->hasMoreElements())
	{
		setCalls(iter->currentElement(), routineTable);
		iter->nextElement();
	}
	delete iter;

	// finally, set the main program to "IsUsed" status.
	//
	(prog->getMainRoutine())->setIsUsedInProgram();
}