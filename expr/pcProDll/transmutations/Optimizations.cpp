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
// Optimizations.cpp
//
// File:  		Optimizations.cpp
// Created:		Fri Jul 27, 2001
// Author:		Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
// 

#include "stdafx.h"

#include "GlobalIncludes.h"
#include "BaseOperation.h"
#include "BaseInstruction.h"
#include "NormalInstr.h"
#include "BasicBlockNode.h"
#include "Routine.h"
#include "Program.h"
#include "Optimizations.h"
#include "BBIterator.h"
#include "ISelParse.h"
#include "IConstArg.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

int debugOptimizations = 0;

extern int matchOp(BaseOperation *op, ISelOpListType *trule, int opNum);
extern void translateOp(BaseOperation *op, ISelOpListType *trule);
extern void translateArgs(BaseOperation *op, ISelArgListType *iselArgs, int dstOrSrc, int newOp);
extern void markToBeDeleted(OpList & gList, OpList & tList);
extern void deleteUselessOps(BasicBlockNode *bb);

extern BasicBlockNode *currBB;
extern Routine *currRoutine;

extern void changeIfOp(BaseInstruction *instr, BaseOperation *oldIfOp, BaseOperation *newIfOp);
extern void resetUDDUChains(OpList & gList, OpList & tList, int ruleIndex);

// if scheduleWhere == 1, schedule at the end of the list, else at the beginning.
//
void insertRHSOpsIntoIR(OpList & gList, OpList & tList, int ruleIndex, int scheduleIt, int scheduleWhere)
{
	BaseInstruction *lastInstr;

	if (scheduleWhere == 1)
		lastInstr = getOptInstrCorrespondingToOper(gList.back());
	else
		lastInstr = getOptInstrCorrespondingToOper(gList.front());

	if (lastInstr == NULL) // something's wrong.
	{
		cout << "NULL instruction encountered during Instruction Selection" << endl;
		assert(0);
	}

	int size = tList.size();

	if ((gList.back())->isControlOp()) // the last generic operation was an if op.
	{
		if ((tList.back())->isControlOp()) // the last target operation too is an if op.
		{
			// make an in place conversion of the instruction.
			//
			changeIfOp(lastInstr, gList.back(), tList.back());
			size--;
		}
		else
		{
			// do not handle this as of now.
			assert(0);
		}
		lastInstr = getPrevInstr(lastInstr, currBB->getFirstInstr());
	}

	// reset the UD and DU chains to the new ops.
	//
	resetUDDUChains(gList, tList, ruleIndex);

	if (scheduleIt == _YES_)
	{
		// delete the markedOps.
		//
		deadCodeElimination(currBB, 0);

		currBB->print(3);
		cout << endl;

		if (size == 1)
		{
			// initSchedule and add the target op into the last instr.
			//
			((NormalInstr *)lastInstr)->initScheduleAndAdd(tList[0], true); 
			//
			// the second parameter is the noCopy parameter
			// Setting it to true means that the add function will not make a copy
			// of the operation. 
			// If the operation were to be copied, the resetUDDU chains function
			// performed earlier will be invalidated.

			if (debugOptimizations)
			{
				cout << endl;
				currBB->print(3);
				cout << endl;
				cout << endl;
			}

			return;
		}
	}

	int changeTrailer = 0;
	if (lastInstr == currBB->getLastInstr())
		changeTrailer = 1;

	for (int i = 0; i < size; i++)
	{
		BaseInstruction *currInstr = buildInstrWithOper(tList[i]);
		((NormalInstr *)currInstr)->appendInstr(lastInstr, _FALSE_BRANCH_, 1);
		lastInstr = currInstr;
	}
	if (changeTrailer)
		currBB->initTrailer(lastInstr);

	if (debugOptimizations)
	{
		cout << endl;
		currBB->print(3);
		cout << endl;
		cout << endl;
	}
}

int translateLHSToRHS(BaseOperation *op, int ruleIndex)
{
	if (numOps(lhsRules[ruleIndex]) != 1) // not possible to use this rule to translate.
		return _NO_;

	if (numOps(rhsRules[ruleIndex]) == 1) // this case is easy.
	{
		if (matchOp(op, lhsRules[ruleIndex], 0) == _NO_) // 0 => first operation in list of ops.
			return _NO_;

		// found a rule that matches the operation. perform the translation.
		// TBD
		//
		translateOp(op, rhsRules[ruleIndex]);
	}
	else
	{
		// The target rule contains more ops than the generic rule.
		// => Need to create new ops.

		// TBD
		assert(0);
	}

	return _YES_;
}

// for each operation, performs a 1 - 1 mapping between generic and target.
// returns _YES_ if successful.
//
int doOptFor1Op(ISelIROpNode & opNode, int ruleIndex)
{
	if (debugOptimizations)
	{
		cout << endl;
		cout << "LHS: ";
		(opNode.getOp())->shortPrint(cout);
	}

	// see if the generic op can be translated to a target op.
	//
	clearIdentToArgMap();
	if (translateLHSToRHS(opNode.getOp(), ruleIndex) == _YES_)
	{
		setAsDoneOpt(opNode);

		if (debugOptimizations)
		{
			cout << "\tRHS: ";
			(opNode.getOp())->shortPrint(cout);
			cout << endl;
		}

		return _YES_;
	}
	if (debugOptimizations)
		cout << "\t No Simple RHS." << endl;

	return _NO_;
}

void translateLHSToRHS(OpList & gList, OpList & tList, ISelOpListType *trule)
{
	// At this point we know that the rule has been matched.
	// Create a new list of ops using trule.

	BaseOperation *op;
	// iterate over trule.
	ISelOpListType::iterator tri;
	for (tri = trule->begin(); tri != trule->end(); tri++)
	{
		// for each individual iselop, create a new IR op.
		//
		op = buildShellOperation((*tri)->getOpcode());
		translateArgs(op, (*tri)->getDstList(), _DEST_LIST_, 1); // 1 => op is a newly created operation
		translateArgs(op, (*tri)->getSrcList(), _SRC_LIST_, 1); // 1 => op is a newly created operation

		tList.push_back(op);
	}
}

int matchLHSOps(OpList & gList, int ruleIndex)
{
	// Now, gtreeList contains the list of operations. See if this matches the rule.
	// If yes, then replace the operations.
	//
	OpListIter oiter;
	int opNum = 0;
	for (oiter = gList.begin(); oiter != gList.end(); oiter++)
	{
		if (matchOp(*oiter, lhsRules[ruleIndex], opNum) == _NO_)
			return _NO_;
		opNum++;
	}
	return _YES_;
}

// create tree of size "size" and match with rules.
// if a rule matches, perform the translation.
//
// If scheduleIt == 1, then try to fit the target ops into the existing IR.
//
int doOptForMultiOps(ISelIROpNode & opNode, int ruleIndex, int scheduleIt = 0)
{
	clearIdentToArgMap();

	if (debugOptimizations)
	{
		cout << endl;
		cout << "LHS: ";
		(opNode.getOp())->shortPrint(cout);
		cout << endl;
	}

	// Note: We are only interested in ops that reside in a basic-block.
	// So, the entire tree has to reside in a basic-block.

	int size = numOps(lhsRules[ruleIndex]);

	// build the tree with the DU chain from this op to other ops.
	//
	OpList gtreeList;

	int foundSeqMatch = 0, foundTreeMatch = 0;

	if (buildListOfOptOpsSequential(opNode.getOp(), gtreeList, size, 0) == _YES_) // see if this matches
	{
		if (matchLHSOps(gtreeList, ruleIndex) == _YES_)
			foundSeqMatch = 1;
	}

	if (!foundSeqMatch)
	{
		gtreeList.clear();
		clearIdentToArgMap();

		if (buildTreeOfOptOpsFromDepChains(opNode.getOp(), gtreeList, ruleIndex) == _YES_) // built list of ops of size "size"
		{
			if (matchLHSOps(gtreeList, ruleIndex) == _YES_)
				foundTreeMatch = 1;
		}
	}

	if ((foundSeqMatch == 0) && (foundTreeMatch == 0)) // no match
		return _NO_;

	// the rule has been matched. now, perform the translation.
	//
	// for each op in the target rule, create a new op.
	//
	OpList ttreeList;
	translateLHSToRHS(gtreeList, ttreeList, rhsRules[ruleIndex]);

	setAsDoneOpt(gtreeList);


	// Now, ttreeList contains all the target ops.
	// Need to insert them into the basic-block.
	//
	// insert them after the last instruction in the gtreeList.
	//
	if (foundSeqMatch)
		insertRHSOpsIntoIR(gtreeList, ttreeList, ruleIndex, scheduleIt, 1);
	else // if (foundTreeMatch)
		insertRHSOpsIntoIR(gtreeList, ttreeList, ruleIndex, scheduleIt, 1);



	if (debugOptimizations)
	{
		cout << endl << "LHS Ops: " << endl;
/*		
		for (OpListIter i1 = gtreeList.begin(); i1 != gtreeList.end(); i1++)
		{
			(*i1)->print(2);
			cout << endl;
		}
*/
		cout << endl << "RHS Ops: " << endl;
				
		for (OpListIter i2 = ttreeList.begin(); i2 != ttreeList.end(); i2++)
		{
			(*i2)->print(2);
			cout << endl;
		}
	}


	return _YES_;
}

int num1OpTranslations = 0;
int numMultiOpTranslations = 0;

void doOptmizations()
{
	// iterate over listOfISelIROps
	//
	Vector<ISelIROpNode>::iterator iter;

	for (iter = listOfOptIROps.begin(); iter != listOfOptIROps.end(); iter++)
	{
		if (iter->getFlg() == _YES_) // already been Optimized. Do nothing.
			continue;

		// find the list of rules for this op.
		// the order of the rules dictates the priority of that rule.
		//
		// first, get the opcode.
		//
		BaseOperation *op = iter->getOp();
		OpCode & opcode = op->getOpCode();

		// next, query the opcode to rule-list map and get the list of rules.
		//
		ArrayOfIntType & ruleNumList = getListOfRuleNums(opcode, 1);

		// iterate over ruleNumList.
		for (ArrayOfIntType::iterator i = ruleNumList.begin(); i != ruleNumList.end(); i++)
		{
			// if the size of generic rule and size of target rule both equal 1,
			// then we can do a simple in place translation.
			//
			if ((numOps(lhsRules[*i]) == 1) && (numOps(rhsRules[*i]) == 1))
			{
				if (doOptFor1Op(*iter, *i) == _YES_) // was successful in translating op
				{
					num1OpTranslations++;
					break;
				}
			}
			else
			{
				if (doOptForMultiOps(*iter, *i, 1) == _YES_) // was successful in translating op and possibly other ops.
				{
					numMultiOpTranslations++;
					break;
				}
			}
		}
	}
}

void doOptmizations(BasicBlockNode *nd)
{
	// iterate over the instrs.
	//
	// add all the ops in the instrs to listOfISelIROps
	//
	
	currBB = nd;

	int done = 0;

	int prevNumTranslations = num1OpTranslations + numMultiOpTranslations;

	while (!done)
	{
		listOfOptIROps.clear();

		BBForwIterator iter(nd);
		BaseInstruction *instr;

		while (iter.hasMoreElements())
		{
			instr = iter.currentElement();

			// build the operation, instruction data-structure.
			//
			addOpsToOptList(instr);

			iter.nextElement();
		}

		if (debugOptimizations)
			printOptList();

		// next, do Optimizations on the list of ops.
		//
		doOptmizations();

		deadCodeElimination(nd, 1);
		done = 1;

		if ((num1OpTranslations + numMultiOpTranslations) == prevNumTranslations) // i.e. no more new tranlsations
			done = 1;
		else
			prevNumTranslations = num1OpTranslations + numMultiOpTranslations;
	}
}

void doOptimizations(Routine *rout)
{
	// perform optimizations on a basic-block by basic-block basis.
	//
	if (rout == NULL) return;

	currRoutine = rout;

	ArrayIterator<BasicBlockNode *> *iter = (rout->getCFG()).createDFSBBIter();

	while (iter->hasMoreElements())
	{
		doOptmizations(iter->currentElement());

		if (debugOptimizations)
		{
			cout << endl;
			(iter->currentElement())->print(2);
			cout << endl;
		}

		iter->nextElement();
	}
	delete iter;

	listOfOptIROps.clear();
	globalSymbolTable->setStartNums();
}

extern Program *curProgram;

void doOptimizations()
{
	cout << "Performing TransMutation Optimizations." << endl;
	// iterate over routine list
	//
	MyLinkedListIterator<Routine *> *iter = curProgram->routineListIteratorForw();

	while (iter->hasMoreElements())
	{
		doOptimizations(iter->currentElement());
		iter->nextElement();
	}
	delete iter;

	cout << "Done performing Instruction Selection." << endl;
}
