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
// File:  		AddCallNameProperty.cpp
// Created:		Mon Jan 24, 00
// Last modified: 	Mon Jan 24, 00
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: SUPPORT

#include "stdafx.h"
#include "CallNameArgProperty.h"
#include "Program.h"
#include "Routine.h"
#include "BaseInstruction.h"
#include "NormalInstr.h"
#include "BaseOperation.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

void addCallNameProperty(BaseArgument *arg, Routine *rout)
{
	CallNameArgProperty cnp(rout);
	arg->addProperty(cnp);
}

void addCallNameProperty(BaseArgument *arg, int sysCallIndex)
{
	CallNameArgProperty cnp(sysCallIndex);
	arg->addProperty(cnp);
}

extern int getSystemCallNameAsInt(char *name);

void addCallNameProperty(BaseOperation *op, Program *prog)
{
	if (op == NULL) return;

	BaseArgument *arg;
	Routine *rptr;
	int sysI;

	// Now, the destination argument cannot be a label.

	if (op->hasOperand(_SOURCE_1_))
	{
		arg = op->ptrToOperand(_SOURCE_1_);

		if ((arg != NULL) && (arg->isLabelArg() == _YES_))
		{
			if ((rptr = prog->getRoutine(arg->lName())) != NULL)
			{
				addCallNameProperty(arg, rptr);
			}
			else if ((sysI = getSystemCallNameAsInt(arg->lName())) != _INVALID_)
			{
				addCallNameProperty(arg, sysI);
			}
		}
	}
	if (op->hasOperand(_SOURCE_2_))
	{
		arg = op->ptrToOperand(_SOURCE_2_);

		if ((arg != NULL) && (arg->isLabelArg() == _YES_))
		{
			if ((rptr = prog->getRoutine(arg->lName())) != NULL)
			{
				addCallNameProperty(arg, rptr);
			}
			else if ((sysI = getSystemCallNameAsInt(arg->lName())) != _INVALID_)
			{
				addCallNameProperty(arg, sysI);
			}
		}
	}
	// Currently, we only handle _SOURCE_1_, _SOURCE_2_.
	// This should be enuff to handle most (if not all) operations
}

void addCallNameProperty(Program *prog)
{
	if (prog == NULL) return;

	// First, iterate thru each routine in the program.
	//
	MyLinkedListIterator<Routine *> *piter;
	piter = prog->routineListIteratorForw();

	MyLinkedListIterator<BaseInstruction *> *riter;
	Routine *curR;

	BaseInstruction *curI;
	BaseOperation *curO;

	ArrayIterator<DataOpSlot> *dIter;
	ArrayIterator<ControlOpSlot> *cIter;

	while (piter->hasMoreElements())
	{
		curR = piter->currentElement();

		// Now, iterate thru all the instructions in the routine.
		//
		riter = curR->instrListIteratorForw();
		while (riter->hasMoreElements())
		{
			curI = riter->currentElement();

			// Now, iterate thru all the operations in the routine.
			//
			dIter = ((NormalInstr *)curI)->getDataOpSlotIter();

			while (dIter->hasMoreElements())
			{
				if ((dIter->currentElement()).hasOper())
				{
					curO = (dIter->currentElement()).getPtrToOperation();

					addCallNameProperty(curO, prog);
				}
				dIter->nextElement();
			}
			delete dIter;

			cIter = ((NormalInstr *)curI)->getControlOpSlotIter();
			while (cIter->hasMoreElements())
			{
				if ((cIter->currentElement()).hasOper())
				{
					curO = (cIter->currentElement()).getPtrToOperation();

					addCallNameProperty(curO, prog);
				}
				cIter->nextElement();
			}
			delete cIter;

			riter->nextElement();
			// We do not do this for flow ops.
		}
		delete riter;

		piter->nextElement();
	}
	delete piter;
}