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
// File:  		rISASchedulerSupport.cpp
// Created:		Mon Nov 1, 99
// Last modified: 	Mon Nov 1, 99
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: SUPPORT

#include "stdafx.h"

#include "ScheduleOpProperty.h"
#include "Routine.h"
#include "BaseOperation.h"
#include "Program.h"
#include "ThumbPass.h"
#include "BBIterator.h"
#include "STLIncludes.h"
#include "BaseInstruction.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

int debugRISAScheduler = 0;

extern void setBarrier(BaseInstruction *instr, int code = _DEFAULT_ALL_);
extern void setNoCodeMotion(BaseInstruction *instr, int code = _DEFAULT_ALL_);
extern void setScheduleOpProperty(BaseOperation *oper, int flg, int code);

extern rISABlockList rISABlocks;
extern int doThumb;

/****************************************************************************/
// The next set of functions is used by SetScheduleProperty functions
// to curtail the motion of operations.
// This allows the scheduler to schedule ops while still respecting the 
// rISA block boundaries.
/****************************************************************************/
//
void setNoRISACodeMotionForOps(BaseInstruction *instr)
{
	// find the data-op and set it for no code motion.
	//
	ArrayIterator<DataOpSlot> *iter = ((NormalInstr *)instr)->getDataOpSlotIter();
	while (iter->hasMoreElements())
	{
		if (iter->currentElement().hasOper())
		{
			BaseOperation *oper = (iter->currentElement()).getPtrToOperation();
			setScheduleOpProperty(oper, _NOCODEMOTION_, _DEFAULT_ALL_);
		}
		iter->nextElement();
	}
	delete iter;
}

// Assume that the rISA blocks have already been determined.
//
// Set barriers at the top and bottom of each blocks such that 
// no operation can move out of, or into the blocks.
//
void setRISABarriers(Routine *rout)
{
	if (!doThumb) return;

	rISABlockListIter i;

	for (i = rISABlocks.begin(); i != rISABlocks.end(); i++)
	{
		// for each rISA block, query if it was deemed profitable.
		//
		// Then, set barriers on its first and last instruction.
		//
		if ((*i).profitability > 0)
		{
			setBarrier((*i).startInstr);
			setBarrier((*i).endInstr);
		}
		//
		//
		// Also, ensure that the first and the last instrs themselves are not moved.
		//
		if ((*i).profitability > 0)
		{
			setNoCodeMotion((*i).startInstr);
			setNoCodeMotion((*i).endInstr);

			setNoRISACodeMotionForOps((*i).startInstr);
			setNoRISACodeMotionForOps((*i).endInstr);
		}

	}
}

int isAFirstRISAInstr(BaseInstruction *instr, BasicBlockNode *bb)
{
	// go thru the rISABlocks DS (created by rISAPhase1).
	//
	rISABlockListIter i1;

	for (i1 = rISABlocks.begin(); i1 != rISABlocks.end(); i1++)
	{
		if ((i1->bb == bb) && (i1->startInstr == instr)) // found a match.
			return _YES_;
	}
	return _NO_;
}

int isALastRISAInstr(BaseInstruction *instr, BasicBlockNode *bb)
{
	// go thru the rISABlocks DS (created by rISAPhase1).
	//
	rISABlockListIter i1;

	for (i1 = rISABlocks.begin(); i1 != rISABlocks.end(); i1++)
	{
		if ((i1->bb == bb) && (i1->endInstr == instr)) // found a match.
			return _YES_;
	}
	return _NO_;
}

extern BaseInstruction *findPrevInstr(BasicBlockNode *bb, BaseInstruction *instr);

Mode getRISAOrNonRISATypeForInstr(BaseInstruction *instr, BasicBlockNode *bb, Mode currType)
{
	if (currType == _NORMAL_)
	{
		if (isAFirstRISAInstr(instr, bb)) 
			return _THUMB_;
		else 
			return _NORMAL_;
	}
	else // if (currType == _THUMB_)
	{
		// get the predecessor instruction.
		//
		BaseInstruction *prevInstr = findPrevInstr(bb, instr);
		if (isALastRISAInstr(prevInstr, bb)) 
			return _NORMAL_;
		else 
			return _THUMB_;
	}
}

/****************************************************************************/

/****************************************************************************/
// The next set of functions is used to re-order instrs within a basic-block
// such that rISA instrs are grouped together.
/****************************************************************************/

const int _NO_DIR_ = 110901;
const int _UP_DIR_ = 100901;
const int _DOWN_DIR_ = 120901;

const int MAX_VAL = 10000;

typedef Deque<BaseInstruction *> InstrQ;
typedef InstrQ::iterator InstrQIter;

typedef Vector<InstrQ *> ListOfInstrLists;
typedef ListOfInstrLists::iterator ListOfInstrListsIter;

ListOfInstrLists listOfRegions, failedToRescheduleRegions;

// This region is not re-schedule-able.
// So, store it in the failedToRescheduleRegions list.
//
// Note, we do not store the iterator, but the region itself.
// This is becos, the iterator pointer may change over time (as the listOfRegions is rebuilt)
// but the region itself does not change.
//
void addToFailedRegionsList(ListOfInstrListsIter currRegionIter)
{
	failedToRescheduleRegions.push_back(*currRegionIter);
}

inline int failedToReschedule(InstrQ *currRegion)
{
	if (std::find(failedToRescheduleRegions.begin(), 
			 failedToRescheduleRegions.end(), currRegion) != failedToRescheduleRegions.end())
		return _YES_;
	return _NO_;
}

void clearListOfRegions()
{
	ListOfInstrListsIter i;

	for (i = listOfRegions.begin(); i != listOfRegions.end(); i++)
		delete (*i);

	listOfRegions.clear();
}

InstrQ *getUpNeighbor(InstrQ *currRegion)
{
	InstrQ *prevRegion = NULL;
	ListOfInstrListsIter i;

	for (i = listOfRegions.begin(); i != listOfRegions.end(); i++)
	{
		if ((*i) == currRegion)
			break;
		prevRegion = currRegion;
	}
	if (i != listOfRegions.end())
		return prevRegion;
	else
		return NULL;
}

InstrQ *getDownNeighbor(InstrQ *currRegion)
{
	InstrQ *nextRegion = NULL;
	ListOfInstrListsIter i;

	i = listOfRegions.end();
	i--;
	for (i; i != listOfRegions.begin(); i--)
	{
		if ((*i) == currRegion)
			break;
		nextRegion = currRegion;
	}
	if (currRegion == (*i)) return nextRegion;
	else
		return NULL;
}

ListOfInstrListsIter getHighestPriorityRegionWithPreferredDir(int & prefDir)
{
	// if the list has only 2 regions, then there is no point.
	//
	if (listOfRegions.size() <= 2)
		return listOfRegions.end();

	// if the list has only 2 valid regions, then there is no point.
	//
	int count = 0;
	for (ListOfInstrListsIter j = listOfRegions.begin(); j != listOfRegions.end(); j++)
	{
		if (!failedToReschedule(*j))
			count++;
	}
	if (count <= 2) 
		return listOfRegions.end();

	// return the region with the least number of instrs.
	//
	ListOfInstrListsIter i, minI;
	InstrQ *currRegion = *(listOfRegions.begin());
	int minSize = MAX_VAL;
	minI = listOfRegions.end();

	for (i = listOfRegions.begin(); i != listOfRegions.end(); i++)
	{
		currRegion = *i;

		if (currRegion->size() < minSize)
		{
			if (!failedToReschedule(currRegion))
			{
				minSize = currRegion->size();
				minI = i;
			}
		}		
	}
	// now, minI contains the location of the region with minimum size.
	//
	if (minI == listOfRegions.end()) // error condition ??
	{
	}
	else if (minI == listOfRegions.begin())
	{
		// the first block is the minimum size region.
		//
		prefDir = _DOWN_DIR_;
	}
	else
	{
		// need to determine the direction of preference.
		//
		ListOfInstrListsIter prevI, nextI;
		prevI = minI - 1;
		nextI = minI + 1;

		if (nextI == listOfRegions.end())
		{
			// the last block is the minimum size region.
			//
			prefDir = _UP_DIR_;
		}
		else
		{
			// try to build large blocks. So, try to move the region into a larger sized block.
			// This is just a heuristic and should be experimented with.
			//
			if ((*prevI)->size() < (*nextI)->size())
				prefDir = _DOWN_DIR_;
			else
				prefDir = _UP_DIR_;
		}
	}
	return minI;
}

// This function merges the regions in listOfRegions.
//
// if dir == _UP_DIR_ merges mergeFrom at the bottom of mergeInto.
//
// else if dir == _DOWN_DIR_ merges mergeFrom at the top of mergeInto.
//
void mergeRegions(InstrQ *mergeInto, InstrQ *mergeFrom, int dir)
{
	if ((mergeInto == NULL) || (mergeFrom == NULL)) return;

	// first add the instr ptrs from mergeFrom to mergeInto.
	//
	// then, remove mergeFrom from listOfRegions
	//

	// Store the location of mergeFrom.
	//

	ListOfInstrListsIter loc;
	if ((loc = std::find(listOfRegions.begin(), listOfRegions.end(), mergeFrom)) == NULL)
	{
		// error condition (??).
		return;
	}

	InstrQIter i1;

	if (dir == _UP_DIR_)
	{
		for (i1= mergeFrom->begin(); i1 != mergeFrom->end(); i1++)
		{
			mergeInto->push_back(*i1);
		}
	}
	else // if (dir == _DOWN_DIR_)
	{
		i1 = mergeFrom->end(); i1 --;

		for (i1; i1 != mergeFrom->begin(); i1--)
		{
			mergeInto->push_front(*i1);
		}
		mergeInto->push_front(*i1);
	}

	delete mergeFrom;

	listOfRegions.erase(loc);
}

void scheduleForRISAPhase1(BasicBlockNode *bb)
{
	// Build regions of similar type instructions.

	// get the list of rISA blocks in this basic block node.
	//
	BBForwIterator iter(bb);
	BaseInstruction *currInstr;

	InstrQ *currRegion;

	Mode currRegionType = _NORMAL_, instrType;
	currInstr = iter.currentElement();
	currRegion = new InstrQ;
	currRegionType = getRISAOrNonRISATypeForInstr(currInstr, bb, currRegionType);
	currRegion->push_back(currInstr);
	iter.nextElement();

	while (iter.hasMoreElements())
	{
		currInstr = iter.currentElement();

		instrType = getRISAOrNonRISATypeForInstr(currInstr, bb, currRegionType);

		if (instrType != currRegionType)
		{
			// add currRegion to the list of Regions
			//
			listOfRegions.push_back(currRegion);

			// create new region.
			//
			currRegion = new InstrQ;
			
			currRegionType = instrType;
		}
		// else if (instrType == currRegionType) do nothing.
		// and in either case add the current instr to currRegion
		
		currRegion->push_back(currInstr);

		iter.nextElement();
	}
	
	// Add the last region to listOfRegions.
	//
	listOfRegions.push_back(currRegion);
}

extern int checkIfPossibleToMovePast(InstrQ *currRegion, InstrQ *nextRegion);
extern int checkIfPossibleToMoveBefore(InstrQ *currRegion, InstrQ *prevRegion);

// if negateDir is 1 then the actual preferred dir is !prefDir
//
int isPossibleToMove(ListOfInstrListsIter currRegionIter, int prefDir, int negateDir)
{
	InstrQ *currRegion, *prevRegion, *nextRegion;
	currRegion = *currRegionIter;
	prevRegion = nextRegion = NULL;

	if (((prefDir == _UP_DIR_) && (!negateDir)) || ((prefDir == _DOWN_DIR_) && (negateDir)))
	{
		if (currRegionIter != listOfRegions.begin())
			prevRegion = *(currRegionIter - 1);
	}
	else if (((prefDir == _DOWN_DIR_) && (!negateDir)) || ((prefDir == _UP_DIR_) && (negateDir)))
	{
		if ((currRegionIter + 1) != listOfRegions.end())
			nextRegion = *(currRegionIter + 1);
	}

	if ((prevRegion == NULL) && (nextRegion != NULL))
		return checkIfPossibleToMovePast(currRegion, nextRegion);
	else if ((prevRegion != NULL) && (nextRegion == NULL))
		return checkIfPossibleToMoveBefore(currRegion, prevRegion);
	// else both prevRegion and nextRegion are NULL.

	return _NO_;
}

extern int movePast(BasicBlockNode *bb, InstrQ *currRegion, InstrQ *nextRegion, int moveBeforeOrPastControl = 0);
extern int moveBefore(BasicBlockNode *bb, InstrQ *currRegion, InstrQ *prevRegion, int moveBeforeOrPastControl = 0);

// if negateDir is 1 then the actual preferred dir is !prefDir
//
int moveRegion(BasicBlockNode *bb, ListOfInstrListsIter currRegionIter, int prefDir, int negateDir)
{
	// need to re-order the instructions in the basic-block bb.
	//
	InstrQ *currRegion, *prevRegion, *nextRegion;
	currRegion = *currRegionIter;
	prevRegion = nextRegion = NULL;

	if (((prefDir == _UP_DIR_) && (!negateDir)) || ((prefDir == _DOWN_DIR_) && (negateDir)))
	{
		prevRegion = *(currRegionIter - 1);
	}
	else if (((prefDir == _DOWN_DIR_) && (!negateDir)) || ((prefDir == _UP_DIR_) && (negateDir)))
	{
		nextRegion = *(currRegionIter + 1);
	}

	if (prevRegion == NULL)
		return movePast(bb, currRegion, nextRegion);
	else
		return moveBefore(bb, currRegion, prevRegion);

}

void mergeRegions(ListOfInstrListsIter currRegionIter, int prefDir, int negateDir)
{
	InstrQ *currRegion, *prevRegion, *nextRegion, *tempRegion;
	currRegion = *currRegionIter;
	prevRegion = nextRegion = tempRegion = NULL;

	if (((prefDir == _UP_DIR_) && (!negateDir)) || ((prefDir == _DOWN_DIR_) && (negateDir)))
	{
		prevRegion = *(currRegionIter - 1);
		if (currRegion != listOfRegions.back()) // i.e. nextRegion exists.
			nextRegion = *(currRegionIter + 1);
		if (prevRegion != listOfRegions.front()) // i.e. prevRegion is not the first region.
			tempRegion = *(currRegionIter - 2);

		if (tempRegion != NULL)
			mergeRegions(tempRegion, currRegion, _UP_DIR_);
		if (nextRegion != NULL)
			mergeRegions(nextRegion, prevRegion, _DOWN_DIR_);
	}
	else if (((prefDir == _DOWN_DIR_) && (!negateDir)) || ((prefDir == _UP_DIR_) && (negateDir)))
	{
		nextRegion = *(currRegionIter + 1);
		if (currRegion != listOfRegions.front()) // i.e. prevRegion exists.
			prevRegion = *(currRegionIter - 1);
		if (nextRegion != listOfRegions.back()) // i.e. nextRegion is not the last region.
			tempRegion = *(currRegionIter + 2);

		if (tempRegion != NULL)
			mergeRegions(tempRegion, currRegion, _DOWN_DIR_);
		if (nextRegion != NULL)
			mergeRegions(prevRegion, nextRegion, _UP_DIR_);
	}
}

void scheduleForRISAPhase2(BasicBlockNode *bb)
{
	// iterate over the list of regions.
	//
	ListOfInstrListsIter currRegionIter;

	// first get the most important region to move.
	// also get the preferred direction to move the region.
	//
	int prefDir;
	currRegionIter = getHighestPriorityRegionWithPreferredDir(prefDir);

	int succeededInMoving = _NO_;

	while (currRegionIter != listOfRegions.end())
	{
		if (isPossibleToMove(currRegionIter, prefDir, 0)) // prefDir, 0 => use prefDir
		{
			// while it may be possible to move a region, it might still not be moved.
			// this could be if the regions contain control ops.
			//
			if (moveRegion(bb, currRegionIter, prefDir, 0) == _NO_) // could not move this region.
				addToFailedRegionsList(currRegionIter);
			else
				mergeRegions(currRegionIter, prefDir, 0);
		}
		else if (isPossibleToMove(currRegionIter, prefDir, 1)) // prefDir, 1 => use !prefDir
		{
			// while it may be possible to move a region, it might still not be moved.
			// this could be if the regions contain control ops.
			//
			if (moveRegion(bb, currRegionIter, prefDir, 1) == _NO_) // could not move this region.
				addToFailedRegionsList(currRegionIter);
			else
				mergeRegions(currRegionIter, prefDir, 1);
		}
		else
		{
			// failed to move this region.
			// so, put this region in the failed to schedule regions list.
			//
			addToFailedRegionsList(currRegionIter);
		}

		if (debugRISAScheduler)
		{
			cout << endl << "**********************" << endl;
			bb->print(4);
			cout << endl << "**********************" << endl;
		}

		currRegionIter = getHighestPriorityRegionWithPreferredDir(prefDir);
	}
}

void scheduleForRISA(BasicBlockNode *bb)
{
	if (bb->isEmpty()) return;

	// First phase: Identify loner instructions 
	// (i.e instrs of a particular type residing between blocks of different types)
	// (e.g. a nonRISA instr between 2 rISA blocks)
	//
	// Second phase: Move the loner instrs to regions of similar identity (if possible).
	// Note: The move can be up or down.
	// The move is valid only if it does not break any dependencies.

	// First phase:
	//
	scheduleForRISAPhase1(bb);

	if (!(listOfRegions.size() > 1)) // only one region present in bb. do nothing.
		return;

	// Second phase:
	//
	scheduleForRISAPhase2(bb);

	clearListOfRegions();
}

void scheduleForRISA(Routine *rout)
{
	// iterate over the basic-blocks of the routine.
	//
	LinkedListIterator<BasicBlockNode> *iter = (rout->getCFG()).createBBIterForw();

	while (iter->hasMoreElements())
	{
		scheduleForRISA(&(iter->currentElement()));
		iter->nextElement();
	}
	delete iter;
}
