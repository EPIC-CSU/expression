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
// File:  		PatchForDoubles.cpp
// Created:		Mon Oct 09, 00
// Last modified: 	Mon Oct 09, 00
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: OPERATION

#include "stdafx.h"

#include "Program.h"
#include "Routine.h"
#include "DoubleArgProperty.h"
#include "OpCodeFileDS.h"
#include "BasicBlockNode.h"
#include "BBIterator.h"
#include "SSAArgProperty.h"
#include "HTGTraversal.h"


//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW:

extern int doneSSA;

extern Routine *currRoutine;

int hasDoubleProp(BaseArgument *arg);

int hasDoubleArgs(BaseOperation *oper)
{
	// iterate over src and dst list.
	// if any arg has a doublearg property, return _YES_

	ArgList & saL = oper->sourceOperandList();
	ArgListIter i = saL.begin();
	for (i; i != saL.end(); i++)
	{
		if (*i == NULL) continue;

		if (hasDoubleProp(*i)) return _YES_;
	}

	ArgList & daL = oper->destOperandList();
	i = daL.begin();
	for (i; i != daL.end(); i++)
	{
		if (*i == NULL) continue;

		if (hasDoubleProp(*i)) return _YES_;
	}

	return _NO_;
}

BaseArgument *getNextDoubleArgument(BaseArgument *arg)
{
	BaseArgument *retVal = NULL;

	if (!arg->isRegisterArg()) return retVal;

	retVal = arg->copy();

	// Note: For now, we just add "1" to the old symval to get the new symval.
	//
	int newSymVal = ((RegisterArg *)retVal)->getVal() + 1;

	((RegisterArg *)retVal)->setVal(newSymVal);

	return retVal;
}

// arg1 does not have a SSA number. give it the same number that was assigned to arg2.
//
void doubleSSAize(BaseArgument *arg1, BaseArgument *arg2)
{
	BaseArgProperty *ssaProp = arg2->getPtrToProperty(_SSA_);

	if (ssaProp == NULL) // error condition
	{
		cout << "Error: After Done SSA, recieved a register argument with NULL SSA Property" << endl;
		assert(0);
	}

	arg1->addProperty(*ssaProp);
	//
	// Also, update the symbol table to reflect the fact that we just created a new variable with a SSA Number.
	//
	globalSymbolTable->addNewRegs(arg1, ((SSAArgProperty *)ssaProp)->getSSANum());
}

// Note: returns true if the argument in (pos, srcOrDst) is of type double
//
// Whether an arg is a double (or not) may depend on various factors such as:
// opcode, value of the arg, position of the arg, etc.
//
int isDoubleArg(BaseOperation *oper, int pos, int srcOrDst)
{
	int opIndex = (oper->getOpCode()).index();

	if (getDataTypeForArgInOper(opIndex, pos, srcOrDst) == _DOUBLE_)
		return _YES_;

	return _NO_;
}

int hasDoubleProp(BaseArgument *arg)
{
	if (arg->getPtrToProperty(_DOUBLEARG_) != NULL)
		return _YES_;
	return _NO_;
}

void remDoubleProperty(BaseArgument *arg)
{
	arg->removeProperty(arg->getPtrToProperty(_DOUBLEARG_));
}

void addDoubleProperties(BaseArgument *arg1, BaseArgument *arg2)
{
	if ((arg1 == NULL) || (arg2 == NULL)) return;

	if (hasDoubleProp(arg1)) 
		remDoubleProperty(arg1);

	if (hasDoubleProp(arg2)) 
		remDoubleProperty(arg2);

	DoubleArgProperty p1(arg2, DOUBLE_FIRST); // add this to arg1
	DoubleArgProperty p2(arg1, DOUBLE_SECOND); // add this to arg2

	arg1->addProperty(p1);
	arg2->addProperty(p2);
}

void patchForDoubles(BaseOperation *oper)
{
	// For each argument, determine if it is a double.
	// Then, add another argument corresponding to the second part of the double.
	//

	BaseArgument *nextArg;

	ArgList & dList = oper->destOperandList();
	ArgListIter iter;
	int i = 0;
	int j = 0;

	for (iter = dList.begin(); iter != dList.end(); iter++, i++)
	{
		if ((*iter) != NULL)
		{
			if (isDoubleArg(oper, i, _DEST_LIST_))
			{
				// create a new double argument.
				// add it to the oper
				// add properties to both iter and the newly added arg.

				nextArg = getNextDoubleArgument(*iter);
				oper->addOperand(nextArg, _DOUBLE_DST_BEGIN_+j, _DEST_LIST_);
				addDoubleProperties(oper->ptrToOperand(i, _DEST_LIST_), oper->ptrToOperand(_DOUBLE_DST_BEGIN_+j, _DEST_LIST_));

				// if SSA has been done, SSAize it.
				//
				if (doneSSA)
					doubleSSAize(oper->ptrToOperand(_DOUBLE_DST_BEGIN_+j, _DEST_LIST_), oper->ptrToOperand(i, _DEST_LIST_));

				j++;

				// need to delete nextArg.
				//
				delete nextArg;
				nextArg = NULL;

				// now, the iterator could have become invalid. So, start it from the beginning.
				// and go to the previous starting point
				// 
				iter = dList.begin();
				for (int k = 0; k < i; k++)
					iter++;
			}
		}
	}

	ArgList & sList = oper->sourceOperandList();
	i = 0;
	j = 0;
	
	for (iter = sList.begin(); iter != sList.end(); iter++, i++)
	{
		if ((*iter) != NULL)
		{
			if (!(*iter)->isRegisterArg()) continue;

			if (isDoubleArg(oper, i, _SRC_LIST_))
			{
				// create a new double argument.
				// add it to the oper
				// add properties to both iter and the newly added arg.

				nextArg = getNextDoubleArgument(*iter);
				oper->addOperand(nextArg, _DOUBLE_SRC_BEGIN_+j, _SRC_LIST_);
				addDoubleProperties(oper->ptrToOperand(i, _SRC_LIST_), oper->ptrToOperand(_DOUBLE_SRC_BEGIN_+j, _SRC_LIST_));

				// if SSA has been done, SSAize it.
				//
				if (doneSSA)
					doubleSSAize(oper->ptrToOperand(_DOUBLE_SRC_BEGIN_+j, _SRC_LIST_), oper->ptrToOperand(i, _SRC_LIST_));

				j++;

				// need to delete nextArg.
				//
				delete nextArg;
				nextArg = NULL;

				// now, the iterator could have become invalid. So, start it from the beginning
				// and go to the previous starting point
				// 
				iter = sList.begin();
				for (int k = 0; k < i; k++)
					iter++;
			}
		}
	}
/*
	oper->shortPrint(cout);
	cout << endl;
*/
}

void patchForDoubles(BaseInstruction *instr)
{
	if (!instr->isNormalInstr()) // error condition ??
		return;

	NormalInstr *n = (NormalInstr *)instr;

	// Now, patch doubles in the DataOpSlot Array.
	//
	ArrayIterator<DataOpSlot> *dIter;
	dIter = n->getDataOpSlotIter();
	
	while (dIter->hasMoreElements())
	{
		if ((dIter->currentElement()).hasOper())
		{
			patchForDoubles((dIter->currentElement()).getPtrToOperation());
		}
		dIter->nextElement();
	}
	delete dIter;
}

void patchForDoubles(Routine *rout)
{
	MyLinkedListIterator<BaseInstruction *> *iter;
	iter = rout->instrListIteratorForw();

	while (iter->hasMoreElements())
	{
		patchForDoubles(iter->currentElement());
		iter->nextElement();
	}
	delete iter;
}

extern void patchForDoubleTypeParms(Program *prog);

void patchForDoubles(Program *prog)
{
	MyLinkedListIterator<Routine *> *rIter;
	rIter = prog->routineListIteratorForw();

	while (rIter->hasMoreElements())
	{
		patchForDoubles(rIter->currentElement());
		rIter->nextElement();
	}
	delete rIter;

	patchForDoubleTypeParms(prog);
}

extern void patchForDoubleTypeParms(BaseInstruction *instr, Routine *rout);

// adds the double argument at the first available position.
// if dstOrSrc = _DEST_LIST_ it starts at _DOUBLE_DST_BEGIN_
// and if dstOrSrc = _SRC_LIST_ it starts at _DOUBLE_SRC_BEGIN_
//
int addDoubleArgAtAvailPos(BaseOperation *op, BaseArgument *arg, int dstOrSrc)
{
	int startPos, endPos, currPos;

	if ((op == NULL) || (arg == NULL)) return _INVALID_;

	if (dstOrSrc == _DEST_LIST_)
	{
		startPos = _DOUBLE_DST_BEGIN_;
		endPos = _DOUBLE_DST_END_;
	}
	else // if (dstOrSrc == _SRC_LIST_)
	{
		startPos = _DOUBLE_SRC_BEGIN_;
		endPos = _DOUBLE_SRC_END_;
	}
	currPos = startPos;

	while (currPos != endPos+1)
	{
		if (!op->hasOperand(currPos, dstOrSrc))
		{
			op->addOperand(arg, currPos, dstOrSrc);
			break;
		}
		currPos++;
	}
	return currPos;
}

// Use this function when creating a new op, based on an old op.
//
// Op is the new op, arg is the old arg (and is possibly a double arg).
// If it is a double arg and is of double1 type
// get its corresponding double arg (darg) and its location
// Add darg to op and add properties between the arg at pos and darg.
// Note: we don't add properties between arg and darg
//
// Note: If newOp == 0, then it means that we are only changing the normal arguments in op.
// i.e. op is an old op, and its double position args remain the same.
// just need to update the pointers.
//
void checkAndAddNewDoubleArg(BaseOperation *op, BaseArgument *arg, int pos, int dstOrSrc, int newOp)
{
	if (!hasDoubleProp(arg)) return;

	DoubleArgProperty *prop = (DoubleArgProperty *)(arg->getPtrToProperty(_DOUBLEARG_));

	if (!prop->isDouble1Type()) return;

	BaseArgument *sarg = prop->getSiblingArg();

	if (newOp)
	{
		int dpos = addDoubleArgAtAvailPos(op, sarg, dstOrSrc);
		addDoubleProperties(op->ptrToOperand(pos, dstOrSrc), op->ptrToOperand(dpos, dstOrSrc));
	}
	else
	{
		addDoubleProperties(op->ptrToOperand(pos, dstOrSrc), sarg);
	}
}

void maintainDoubles(BasicBlockNode *node)
{
	BBForwIterator iter(node);

	while (iter.hasMoreElements())
	{
		// first call the patchForDoubles routine
		//
		patchForDoubles(iter.currentElement());

		// next call the call patch routine
		//
		patchForDoubleTypeParms(iter.currentElement(), currRoutine);
		iter.nextElement();
	}
}

void maintainDoubles(BaseHTGNode *loop)
{
	if (!loop->isLoopNode())
	{
		// error condition.
		cout << "Panic: Recieved non loop node in maintainUD" << endl;
		assert(0);
	}

	TraverseForDoubles doubleTrouble;

	// traverse the loop and maintain the double properties at each instruction/operation.
	//
	globalHTGTraversalFuncPtrs.addIfHTGTraversal(&globalIfHTGTraversalFunc);
	globalHTGTraversalFuncPtrs.addLoopHTGTraversal(&globalLoopHTGTraversalFunc);
	globalHTGTraversalFuncPtrs.addSimpleHTGTraversal(&doubleTrouble);

	globalHTGTraversalFuncPtrs.traverseHTGNode(loop);
}

