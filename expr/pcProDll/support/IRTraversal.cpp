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
// File:  		IRTraversal.cpp
// Created:		Fri Sep 03, 99
// Last modified: 	Fri Sep 03, 99
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: SUPPORT

#include "stdafx.h"

#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include "IRTraversal.h"
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


void WalkInstr::_addUniq(BaseInstruction *instr, BaseInstruction *addInstr)
{
	if (_mode == FAST_MODE)
		_neighborList[instr].push_back(addInstr);
	else
	{
		if ((instr == NULL) || (addInstr == NULL)) return;

		InstrList & temp = _neighborList[instr];
		if (std::find(temp.begin(), temp.end(), addInstr) == temp.end())
			_neighborList[instr].push_back(addInstr);
	}
}

void WalkInstr::_delUniq(BaseInstruction *instr, BaseInstruction *delInstr)
{
	if ((instr == NULL) || (delInstr == NULL)) return;

	InstrList & temp = _neighborList[instr];
	InstrListIter it = std::find(temp.begin(), temp.end(), delInstr);
	if (it != temp.end())
		_neighborList[instr].erase(it);

	if (_walkType == WALK_BACKWARDS)
	{
		// also need to update the predecessors of instrs to be the predecessors of delInstr.
		//
		InstrList & prevList = _neighborList[delInstr];
		InstrListIter pit;
		for (pit = prevList.begin(); pit != prevList.end(); pit++)
		{
			_addUniq(instr, *pit);
		}
	}
}

void WalkInstr::_setWalkForInstr(BaseInstruction *instr)
{
	if (instr == NULL) return;

	BaseInstruction *nextInstr = instr->getFirstSucc();

	while (nextInstr != NULL)
	{
		// First set the _neighborList correctly.
		//
		if (_walkType == WALK_BACKWARDS)
		{
			_addUniq(nextInstr, instr);
		}
		else // if _walkType == WALK_FORWARDS
		{
			// do nothing.
		}
		nextInstr = instr->getNextSucc();
	}
	instr->deleteSuccIter();
}

void WalkInstr::_resetWalkForInstr(BaseInstruction *instr)
{
	if (instr == NULL) return;

	BaseInstruction *nextInstr = instr->getFirstSucc();

	while (nextInstr != NULL)
	{
		// First set the _neighborList correctly.
		//
		if (_walkType == WALK_BACKWARDS)
		{
			_delUniq(nextInstr, instr);
		}
		else // if _walkType == WALK_FORWARDS
		{
			// do nothing.
		}
		nextInstr = instr->getNextSucc();
	}
	instr->deleteSuccIter();
}

void WalkInstr::initWalk(Routine *rout)
{
	Deque<BaseInstruction *> activeList;

	Map<BaseInstruction *, int> doneList;

	if (_mode != FAST_MODE) // then do all the error checking.
	{
		if (_walkType != WALK_BACKWARDS)
		{
			// Only WALK_BACKWARD is handled as of now.
			//
			cout << "Warning: Only WALK_BACKWARDS is supported" << endl;
			return;
		}

		if (_walkEdge != WALK_CF)
		{
			// Only WALK_CF is handled as of now.
			//
			cout << "Warning: Only WALK_CF is supported" << endl;
			return;
		}

		if (rout == NULL) return;
	}

	_rout = rout;

	BaseInstruction *thisInstr, *nextInstr;

	if (_walkEdge == WALK_CF)
	{
		// We are always guaranteed that the routine has only 1 first instr.
		//
		thisInstr = rout->getFirstInstr();

		if (thisInstr == NULL) return;

		activeList.push_back(thisInstr);
		
		while (!activeList.empty())
		{
			thisInstr = activeList.front();
			activeList.pop_front();
			
			_setWalkForInstr(thisInstr);

			nextInstr = thisInstr->getFirstSucc();
			while (nextInstr != NULL)
			{
				// Then, add nextInstr to active list.
				//
				if (doneList.find(nextInstr) == doneList.end())
					activeList.push_back(nextInstr);
				
				nextInstr = thisInstr->getNextSucc();
			}
			thisInstr->deleteSuccIter();

			// Finally, set thisInstr to done.
			//
			doneList[thisInstr] = 1;
		}

		// Now, set the proper start instructions.
		//
		// if (WALK_BACKWARDS)
		// Go thru the entire _neighbor list and add those that do not have any neighbors.
		//
		// if (WALK_FORWARDS)
		// get the first instr from the routine.

		if ((_walkType == WALK_BACKWARDS) && (_walkEdge == WALK_CF))
		{
			Map<BaseInstruction *, InstrList>::iterator i;

			for (i = _neighborList.begin(); i != _neighborList.end(); i++)
			{
				if (((*i).second).empty())
					_startList.push_back((*i).first);
			}
		}
		else // if (_walkType == WALK_FORWARDS)
		{
			// do not handle this as of now.
		}
	}
}

InstrList & WalkInstr::getNext(BaseInstruction *instr)
{
	if (_mode == FAST_MODE)
		return _neighborList[instr];

	if (instr == NULL)
	{
		// error condition.
		//
		cout << "Panic: Tried to getNext for a NULL instruction" << endl;
		assert(0);
	}

	Map<BaseInstruction *, InstrList>::iterator i;

	if ((i = _neighborList.find(instr)) != _neighborList.end())
		return i->second;

	// error condition
	//
	cout << "Panic: Tried to getNext for an invalid/first/last instruction" << endl;
	//assert(0);

	static InstrList a;
	return a;
}

void WalkInstr::reset()
{
	_rout = NULL;

	_neighborList.clear();
	_startList.clear();
}

void WalkInstr::print(ostream & out)
{
	if (_rout == NULL) return;

	out << "*************************************************" << endl;
	out << "**** Walking Instructions for Routine: " << _rout->getName() << " ***" << endl;
	out << endl;

	Map<BaseInstruction *, InstrList>::iterator i;
	InstrListIter j;

	for (i = _neighborList.begin(); i != _neighborList.end(); i++)
	{
		out << "(* " << i->first << " *) ";
		(i->first)->shortPrint(out);
		out << endl;
		
		for (j = (i->second).begin(); j != (i->second).end(); j++)
		{
			out << "\t" << " (* " << (*j) << " *) ";
			(*j)->shortPrint(out);
			out << endl;
		}
	}

	out << "**** End walking instructions *******************" << endl;
	out << "*************************************************" << endl;
}

void WalkInstr::updateInstrs(InstrList & updateList, InstrList & toList)
{
	for (InstrListIter i = updateList.begin(); i != updateList.end(); i++)
	{
		if ((*i) != NULL)
		{
			for (InstrListIter j = toList.begin(); j != toList.end(); j++)
			{
				_addUniq(*i, *j);
			}
		}
	}
}

void WalkInstr::addNewInstr(BaseInstruction *instr, InstrList & preds, InstrList & succs, int updateStart)
{
	if (_mode != FAST_MODE) // then do all the error checking.
	{
		if (instr == NULL)
		{
			// Warning.
			//
			cout << "Warning: WalkInstr::addNewInstr recieved NULL instr" << endl;
			return;
		}
		if (!(_neighborList[instr]).empty())
		{
			// Warning
			//
			cout << "Warning: Tried to add an already existing instruction as newInstr to WalkInstr" << endl;
			return;
		}
	}

	InstrList temp;
	temp.push_back(instr);
	if (_walkType == WALK_BACKWARDS)
	{
		updateInstrs(temp, preds);
		updateInstrs(succs, temp);
		
		if (updateStart)
		{
			// Add this instr to start if it has no succs.
			//
			if (succs.empty())
				_startList.push_back(instr);
		}
	}
	else // if (_walkType == WALK_FORWARDS)
	{
		updateInstrs(temp, succs);
		updateInstrs(preds, temp);

		if (updateStart)
		{
			// Add this instr to start if it has no preds.
			//
			if (preds.empty())
				_startList.push_back(instr);
		}
	}
}

void WalkInstr::addNewInstr(BaseInstruction *instr)
{
	if (_mode != FAST_MODE) // then do all the error checking.
	{
		if (instr == NULL)
		{
			// Warning.
			//
			cout << "Warning: WalkInstr::addNewInstr recieved NULL instr" << endl;
			return;
		}
		if (!(_neighborList[instr]).empty())
		{
			// Warning
			//
			cout << "Warning: Tried to add an already existing instruction as newInstr to WalkInstr" << endl;
			return;
		}
	}
	_setWalkForInstr(instr);
}

void WalkInstr::removeInstr(BaseInstruction *instr)
{
	if (_mode != FAST_MODE) // then do all the error checking.
	{
		if (instr == NULL)
		{
			// Warning.
			//
			cout << "Warning: WalkInstr::addNewInstr recieved NULL instr" << endl;
			return;
		}
	}
	_resetWalkForInstr(instr);
}

void WalkInstr::addPred(BaseInstruction *instr, BaseInstruction *pred)
{
	if (_mode != FAST_MODE) // then do all the error checking.
	{
		if (instr == NULL)
		{
			// Warning.
			//
			cout << "Warning: WalkInstr::addNewInstr recieved NULL instr" << endl;
			return;
		}
	}

	InstrList temp1, temp2;
	temp1.push_back(instr);
	temp2.push_back(pred);

	if (_walkType == WALK_BACKWARDS)
		updateInstrs(temp1, temp2);
	else // if (_walkType == WALK_FORWARDS)
		updateInstrs(temp2, temp1);
}

void WalkInstr::delEdge(BaseInstruction *instr, BaseInstruction *delInstr)
{
	// just delete the edge between instr and delInstrs.
	//
	// previously was: _delUniq(instr, delInstr);
	//
	if ((instr == NULL) || (delInstr == NULL)) return;

	InstrList & temp = _neighborList[instr];
	InstrListIter it = std::find(temp.begin(), temp.end(), delInstr);
	if (it != temp.end())
		_neighborList[instr].erase(it);
}

void OpToInstrMap::initOpToInstrMap(Routine *rout)
{
	Deque<BaseInstruction *> activeList;
	Map<BaseInstruction *, int> doneList;

	if (_mode != FAST_MODE) // then do all the error checking.
	{
		if (rout == NULL) return;
	}
	
	_rout = rout;
		
	BaseInstruction *thisInstr, *nextInstr;

	// We are always guaranteed that the routine has only 1 first instr.
	//
	thisInstr = rout->getFirstInstr();
	
	if (thisInstr == NULL) return;
	
	activeList.push_back(thisInstr);
		
	while (!activeList.empty())
	{
		thisInstr = activeList.front();
		activeList.pop_front();
		
		addInstr(thisInstr);
		
		nextInstr = thisInstr->getFirstSucc();
		while (nextInstr != NULL)
		{
			// add nextInstr to active list.
			//
			if (doneList.find(nextInstr) == doneList.end())
				activeList.push_back(nextInstr);
			
			nextInstr = thisInstr->getNextSucc();
		}
		thisInstr->deleteSuccIter();
		
		// Finally, set thisInstr to done.
		//
		doneList[thisInstr] = 1;
	}
}

BaseInstruction* OpToInstrMap::getInstr(BaseOperation *oper)
{
	if (_mode == FAST_MODE)
		return _opToInstrMap[oper];

	if (oper == NULL)
	{
		// error condition.
		//
		cout << "Panic: Tried to getInstr for a NULL operation" << endl;
		assert(0);
	}

	Map<BaseOperation *, BaseInstruction *>::iterator i;

	if ((i = _opToInstrMap.find(oper)) != _opToInstrMap.end())
		return i->second;

	// error condition
	//
	cout << "Panic: Tried to getInstr for an invalid operation" << endl;
	assert(0);

	return NULL;
}

void OpToInstrMap::reset()
{
	_rout = NULL;

	_opToInstrMap.clear();
}

void OpToInstrMap::print(ostream & out)
{
	if (_rout == NULL) return;

	out << "*************************************************" << endl;
	out << "**** Printing Op to Instr Mapping for Routine: " << _rout->getName() << " ***" << endl;
	out << endl;

	Map<BaseOperation *, BaseInstruction *>::iterator i;

	for (i = _opToInstrMap.begin(); i != _opToInstrMap.end(); i++)
	{
		out << "(* " << i->first << " *) ";
		(i->first)->shortPrint(out);
		out << endl;
		out << "\t" << " (* " << i->second << " *) ";
		(i->second)->shortPrint(out);
		out << endl;
	}

	out << "**** End printing Op to Instr Mapping ***********" << endl;
	out << "*************************************************" << endl;
}

void OpToInstrMap::removeOper(BaseOperation *oper)
{
	if (_mode == FAST_MODE)
		_opToInstrMap.erase(oper);

	if (oper == NULL)
	{
		// error condition.
		//
		cout << "Panic: Tried to erase a NULL operation in OpToInstrMap" << endl;
		assert(0);
	}

	Map<BaseOperation *, BaseInstruction *>::iterator i;

	if ((i = _opToInstrMap.find(oper)) != _opToInstrMap.end())
		_opToInstrMap.erase(i);
}

void OpToInstrMap::addOper(BaseOperation *oper, BaseInstruction *instr)
{
	if (_mode == FAST_MODE)
		_opToInstrMap[oper] = instr;

	if (oper == NULL)
	{
		// error condition.
		//
		cout << "Panic: Tried to add a NULL operation in OpToInstrMap" << endl;
		assert(0);
	}

	_opToInstrMap[oper] = instr;
}

void OpToInstrMap::changeOper(BaseOperation *oper, BaseInstruction *instr)
{
	if (oper == NULL)
	{
		// error condition.
		//
		cout << "Panic: Tried to add a NULL operation in OpToInstrMap" << endl;
		assert(0);
	}

	Map<BaseOperation *, BaseInstruction *>::iterator i;

	if ((i = _opToInstrMap.find(oper)) != _opToInstrMap.end())
		(*i).second = instr;
	else
	{
		// error condition.
		//
		cout << "Panic: Tried to change a non-existant operation in OpToInstrMap" << endl;
		assert(0);
	}
}

void OpToInstrMap::changeOpers(BaseInstruction *oldInstr, BaseInstruction *newInstr)
{
	Map<BaseOperation *, BaseInstruction *>::iterator i;

	for (i = _opToInstrMap.begin(); i != _opToInstrMap.end(); i++)
	{
		if (oldInstr == (*i).second)
			(*i).second = newInstr;
	}
}

void OpToInstrMap::addInstr(BaseInstruction *instr)
{
	if (instr == NULL) return;

	// Check dataSlots, controlSlots etc. for operations and add them to the map.
	//
	// Note: Currently, we do not check to see if the operations already are mapped
	// to other instrs. In case they are mapped to a diff. instr a warning should be thrown.

	if ((_opType == DC_OP) || (_opType == ALL_OP))
	{
		ArrayIterator<DataOpSlot> *dIter = ((NormalInstr *)instr)->getDataOpSlotIter();

		while (dIter->hasMoreElements())
		{
			if ((dIter->currentElement()).hasOper())
				addOper((dIter->currentElement()).getPtrToOperation(), instr);

			dIter->nextElement();
		}
		delete dIter;
	}

	if ((_opType == DC_OP) || (_opType == ALL_OP))
	{
		ArrayIterator<ControlOpSlot> *cIter = ((NormalInstr *)instr)->getControlOpSlotIter();

		while (cIter->hasMoreElements())
		{
			if ((cIter->currentElement()).hasOper())
				addOper((cIter->currentElement()).getPtrToOperation(), instr);

			cIter->nextElement();
		}
		delete cIter;
	}

	if (_opType == ALL_OP)
	{
		ArrayIterator<FlowOpSlot> *fIter = ((NormalInstr *)instr)->getFlowOpSlotIter();

		while (fIter->hasMoreElements())
		{
			if ((fIter->currentElement()).hasOper())
				addOper((fIter->currentElement()).getPtrToOperation(), instr);

			fIter->nextElement();
		}
		delete fIter;
	}
}

