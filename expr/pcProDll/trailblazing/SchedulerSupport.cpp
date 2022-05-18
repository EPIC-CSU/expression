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
// File:  		SchedulerSupport.cpp
// Created:		Mon Nov 1, 99
// Last modified: 	Mon Nov 1, 99
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: SUPPORT

#include "stdafx.h"

#include "BaseArgument.h"
#include "BaseOperation.h"
#include "BaseInstruction.h"
#include "NormalInstr.h"
#include "CallOp.h"
#include "DUProperty.h"
#include "UDProperty.h"
#include "Routine.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

typedef Deque<BaseInstruction *> InstrQ;
typedef InstrQ::iterator InstrQIter;

extern void addUniqArgToArgList(ArgList & aList, BaseArgument *arg);

// if uniq == 1, add a def only if it is not already present in aList.
//
void addDefsToList(BaseOperation *oper, ArgList & aList, int uniq = 0)
{
	if (oper == NULL) return;

	// go thru the dest list of oper and add each reg def to aList.
	//
	ArgList & dL = oper->destOperandList();
	
	for (ArgListIter i = dL.begin(); i != dL.end(); i++)
	{
		if ((*i) == NULL) continue;
		if (!(*i)->isRegisterArg()) continue;

		if (uniq == 0)
			aList.push_back(*i);
		else
			addUniqArgToArgList(aList, *i);
	}
}

// if uniq == 1, add a use only if it is not already present in aList.
//
void addUsesToList(BaseOperation *oper, ArgList & aList, int uniq = 0)
{
	if (oper == NULL) return;

	// go thru the source list of oper and add each reg source to aList.
	//
	ArgList & sL = oper->sourceOperandList();
	
	for (ArgListIter i = sL.begin(); i != sL.end(); i++)
	{
		if ((*i) == NULL) continue;
		if (!(*i)->isRegisterArg()) continue;

		if (uniq == 0)
			aList.push_back(*i);
		else
			addUniqArgToArgList(aList, *i);
	}

	// if the operation is a call ret op, need to also add each of its parameters to aList.
	//
	if (oper->isCallOp())
	{
		ArgList & pL = ((CallRetOp *)oper)->getParmList();
		
		for (ArgListIter pI = pL.begin(); pI != pL.end(); pI++)
		{
			if ((*pI) == NULL) continue;
			if (!(*pI)->isRegisterArg()) continue;

		if (uniq == 0)
			aList.push_back(*pI);
		else
			addUniqArgToArgList(aList, *pI);
		}
	}
}

// if uniq == 1, add a def only if it is not already present in aList.
//
void listOfDefs(BaseInstruction *instr, ArgList & aList, int uniq = 0)
{
	if (instr == NULL) return;

	// go thru all the data and control ops.
	//
	ArrayIterator<DataOpSlot> *dIter;
	dIter = ((NormalInstr *)instr)->getDataOpSlotIter();

	while (dIter->hasMoreElements())
	{
		if ((dIter->currentElement()).hasOper())
		{
			addDefsToList((dIter->currentElement()).getPtrToOperation(), aList, uniq);
		}
		dIter->nextElement();
	}
	delete dIter;

	ArrayIterator<ControlOpSlot> *cIter;
	cIter = ((NormalInstr *)instr)->getControlOpSlotIter();

	while (cIter->hasMoreElements())
	{
		if ((cIter->currentElement()).hasOper())
		{
			addDefsToList(cIter->currentElement().getPtrToOperation(), aList, uniq);
		}
		cIter->nextElement();
	}
	delete cIter;
}

// if uniq == 1, add a use only if it is not already present in aList.
//
void listOfUses(BaseInstruction *instr, ArgList & aList, int uniq = 0)
{
	if (instr == NULL) return;

	// go thru all the data and control ops.
	//
	if (instr == NULL) return;

	// go thru all the data and control ops.
	//
	ArrayIterator<DataOpSlot> *dIter;
	dIter = ((NormalInstr *)instr)->getDataOpSlotIter();

	while (dIter->hasMoreElements())
	{
		if ((dIter->currentElement()).hasOper())
		{
			addUsesToList(dIter->currentElement().getPtrToOperation(), aList, uniq);
		}
		dIter->nextElement();
	}
	delete dIter;

	ArrayIterator<ControlOpSlot> *cIter;
	cIter = ((NormalInstr *)instr)->getControlOpSlotIter();

	while (cIter->hasMoreElements())
	{
		if ((cIter->currentElement()).hasOper())
		{
			addUsesToList(cIter->currentElement().getPtrToOperation(), aList, uniq);
		}
		cIter->nextElement();
	}
	delete cIter;
}

// Note: If this is called after RA with uniq == 1, 
// returns the list of registers defined by this routine.
//
void listOfDefs(Routine *rout, ArgList & aList, int uniq = 0)
{
	MyLinkedListIterator<BaseInstruction *> *iter = rout->instrListIteratorForw();

	while (iter->hasMoreElements())
	{
		listOfDefs(iter->currentElement(), aList, uniq);

		iter->nextElement();
	}
	delete iter;
}

// Note: If this is called after RA with uniq == 1, 
// returns the list of registers used within this routine.
//
void listOfUses(Routine *rout, ArgList & aList, int uniq = 0)
{
	MyLinkedListIterator<BaseInstruction *> *iter = rout->instrListIteratorForw();

	while (iter->hasMoreElements())
	{
		listOfUses(iter->currentElement(), aList, uniq);

		iter->nextElement();
	}
	delete iter;
}

int regionContainsOperFromList(MyLinkedList<BaseOperation *> & list, InstrQ *region)
{
	// iterate over the region.
	// for each instr, see if it contains an oper that is present in list.
	//
	InstrQIter i;

	for (i = region->begin(); i != region->end(); i++)
	{
		// for each instr
		// go thru all the data and control ops.
		//
		ArrayIterator<DataOpSlot> *dIter;
		dIter = ((NormalInstr *)(*i))->getDataOpSlotIter();

		while (dIter->hasMoreElements())
		{
			if ((dIter->currentElement()).hasOper())
			{
				if (list.contains((dIter->currentElement()).getPtrToOperation()))
					return _YES_;
			}
			dIter->nextElement();
		}
		delete dIter;

		ArrayIterator<ControlOpSlot> *cIter;
		cIter = ((NormalInstr *)(*i))->getControlOpSlotIter();

		while (cIter->hasMoreElements())
		{
			if ((cIter->currentElement()).hasOper())
			{
				if (list.contains((cIter->currentElement()).getPtrToOperation()))
					return _YES_;
			}
			cIter->nextElement();
		}
		delete cIter;
	}
	return _NO_;
}

// Return _YES_ if the instruction defines a variable that is used by some instruction in region.
//
int instrDefinesVarUsedInRegion(BaseInstruction *instr, InstrQ *region)
{
	ArgList aL;

	listOfDefs(instr, aL, 0);

	// Now, aL contains list of defs in instr.

	// for each def, if it has a def-use list, see if the def-use list contains an instr from region.
	//
	ArgListIter i;
	BaseArgProperty *prop;
	for (i = aL.begin(); i != aL.end(); i++)
	{
		if (*i == NULL) continue;
		if ((prop = (*i)->getPtrToProperty(_DUCHAIN_)) != NULL)
		{
			if (regionContainsOperFromList(((DUChainProperty *)prop)->getUseList(), region) == _YES_)
				return _YES_;
		}
	}	
	return _NO_;
}

// Return _YES_ if the instruction uses a variable that is defined by some instruction in region.
//
int instrUsesVarDefinedInRegion(BaseInstruction *instr, InstrQ *region)
{
	ArgList aL;

	listOfUses(instr, aL, 0);

	// Now, aL contains list of uses in instr.

	// for each use, if it has a use-def list, see if the use-def list contains an instr from region.
	//
	ArgListIter i;
	BaseArgProperty *prop;
	for (i = aL.begin(); i != aL.end(); i++)
	{
		if (*i == NULL) continue;
		if ((prop = (*i)->getPtrToProperty(_UDCHAIN_)) != NULL)
		{
			if (regionContainsOperFromList(((UDChainProperty *)prop)->getDefList(), region) == _YES_)
				return _YES_;
		}
	}
	return _NO_;
}

// it is possible to move curr region past next region if
// there are no dependencies between curr region and next region.
// So, check if any def originates in curr region and has a use in next region.
//
int checkIfPossibleToMovePast(InstrQ *currRegion, InstrQ *nextRegion)
{
	if ((currRegion == NULL) || (nextRegion == NULL)) return _NO_;

	InstrQIter currI;

	for (currI = currRegion->begin(); currI != currRegion->end(); currI++)
	{
		if (instrDefinesVarUsedInRegion(*currI, nextRegion))
			return _NO_;
	}
	return _YES_;
}

// it is possible to move curr region before prev region if
// there are no dependencies between curr region and next region.
// So, check if any use originates in curr region and has a def in next region.
//
int checkIfPossibleToMoveBefore(InstrQ *currRegion, InstrQ *prevRegion)
{
	if ((currRegion == NULL) || (prevRegion == NULL)) return _NO_;

	InstrQIter currI;

	for (currI = currRegion->begin(); currI != currRegion->end(); currI++)
	{
		if (instrUsesVarDefinedInRegion(*currI, prevRegion))
			return _NO_;
	}
	return _YES_;
}

// this function is present in IRSupportFuncs.cpp (in Support directory).
//
extern void reorderRegions(BasicBlockNode *bb, BaseInstruction *start1, BaseInstruction *end1, BaseInstruction *start2, BaseInstruction *end2);

// returns 1 if was able to successfully move currRegion past nextRegion.
//
// if moveBeforeOrPastControl = 1, then neglect the control ops.
//
int movePast(BasicBlockNode *bb, InstrQ *currRegion, InstrQ *nextRegion, int moveBeforeOrPastControl)
{
	if ((bb == NULL) || (currRegion == NULL) || (nextRegion == NULL)) return _NO_;

	BaseInstruction *firstCurrInstr = currRegion->front();
	BaseInstruction *lastCurrInstr = currRegion->back();

	BaseInstruction *firstNextInstr = nextRegion->front();
	BaseInstruction *lastNextInstr = NULL; // the lastNextInstr is the last instr that does not contain a control op.
	
	InstrQIter i = nextRegion->end();
	while (i != nextRegion->begin())
	{
		i--;
		if (*i == NULL) continue; // this is an error check. should never occur

		if (((NormalInstr *)(*i))->countControlOps() == 0)
		{
			lastNextInstr = *i;
			break;
		}
		else if (moveBeforeOrPastControl == 0) // return NO.
			return _NO_;
	}
	
	// if lastNextInstr is still NULL, could not find any non-control op instr in nextRegion. do not move.
	//
	if (lastNextInstr == NULL)
		return _NO_;

	reorderRegions(bb, firstCurrInstr, lastCurrInstr, firstNextInstr, lastNextInstr);

	return _YES_;
}

// Note: moveBefore(currRegion, prevRegion) is the same as movePast(prevRegion, currRegion).
//
// if moveBeforeOrPastControl = 1, then neglect the control ops.
//
int moveBefore(BasicBlockNode *bb, InstrQ *currRegion, InstrQ *prevRegion, int moveBeforeOrPastControl)
{
	if ((bb == NULL) || (currRegion == NULL) || (prevRegion == NULL)) return _NO_;

	BaseInstruction *firstCurrInstr = currRegion->front();
	BaseInstruction *lastCurrInstr = NULL; // the lastCurrInstr is the last instr that does not contain a control op.

	BaseInstruction *firstPrevInstr = prevRegion->front();
	BaseInstruction *lastPrevInstr = prevRegion->back(); 
	
	InstrQIter i = currRegion->end();
	while (i != currRegion->begin())
	{
		i--;
		if (*i == NULL) continue; // this is an error check. should never occur

		if (((NormalInstr *)(*i))->countControlOps() == 0)
		{
			lastCurrInstr = *i;
			break;
		}
		else if (moveBeforeOrPastControl == 0) // return NO.
			return _NO_;
	}
	
	// if lastCurrInstr is still NULL, could not find any non-control op instr in currRegion. do not move.
	//
	if (lastCurrInstr == NULL)
		return _NO_;

	reorderRegions(bb, firstPrevInstr, lastPrevInstr, firstCurrInstr, lastCurrInstr);

	return _YES_;
}

