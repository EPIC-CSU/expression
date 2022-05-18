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
// File:  		NewInstrSel.cpp
// Created:		Fri Jul 27, 01
// Last modified: 	Fri Jul 27, 01
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
#include "BasicBlockNode.h"
#include "Routine.h"
#include "Program.h"
#include "NewInstrSel.h"
#include "BBIterator.h"
#include "ISelParse.h"
#include "IConstArg.h"
#include "FConstArg.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

int debugNewInstrSel = 0;

extern ISelRuleType genericRules;
extern ISelRuleType targetRules;

extern int equalRegs(BaseArgument *reg1, BaseArgument *reg2);

BasicBlockNode *currBB;

int matchArgs(BaseArgument *arg, BaseArgument *identArg)
{
	if ((arg == NULL) || (identArg == NULL)) return _NO_;

	if ((arg->isRegisterArg()) && (identArg->isRegisterArg()))
	{
		return (equalRegs(arg, identArg));
	}
	else if ((arg->isImmediateArg()) && (identArg->isImmediateArg()))
	{
		return (arg->isEq(identArg));
	}
	else if ((arg->isLabelArg()) && (identArg->isLabelArg()))
	{
		return (arg->isEq(identArg));
	}
	return _NO_;
}

extern int satisfiesImmCond(int intVal, Oprnd & oprnd);
extern int satisfiesImmCond(double fltVal, Oprnd & oprnd);

// assumes that the arg is an immediate argument.
//
// iselarg contains the oprnd field that contains the value of the immediate and the operator.
//
// return _YES_ if arg satisfies the <operator immediate> expression.
//
int satisfiesImmCond(BaseArgument *arg, ISelArg & iselarg)
{
	int intVal;
	double fltVal;

	Oprnd & oprnd = iselarg.getOperand();

	if (arg->isIConstArg())
	{
		intVal = ((IConstArg *)arg)->value();
		return satisfiesImmCond(intVal, oprnd);
	}
	else if (arg->isFConstArg())
	{
		fltVal = ((FConstArg *)arg)->value();
		return satisfiesImmCond(fltVal, oprnd);
	}
	return _NO_;
}

int matchArgs(BaseArgument *arg, ISelArg & iselarg)
{
	// iselarg contains info on whether the arg is expected to be reg or imm.
	// if it is an imm it could also contain the value (max/min) expected.
	//
	// right now, we only check if it is of type reg or imm.
	//
	if ((iselarg.getOperand()).isRegType())
		return (arg->isRegisterArg());
	else if ((iselarg.getOperand()).isImmType())
		return (satisfiesImmCond(arg, iselarg));
	else if ((iselarg.getOperand()).isLabType())
		return (arg->isLabelArg());

	return _NO_;
}

// returns _YES_ if any arg in identToArgMap matches "arg"
//
// if identType == -1 only compares those args whose identNumber is < _DONT_CARE_IDENT_
// if identType == 0, tries to match with all args
// if identType == 1, only compares those args whose identNumber is > _DONT_CARE_IDENT_
//
int matchArgs(BaseArgument *arg, int identType)
{
	Map<int, ArgAndOper>::iterator i;

	if (identType == 0)
	{
		for (i = identToArgMap.begin(); i != identToArgMap.end(); i++)
		{
			if (matchArgs(arg, (i->second)._arg) == _YES_)
				return _YES_;
		}
	}
	else if (identType == -1)
	{
		for (i = identToArgMap.begin(); i != identToArgMap.end(); i++)
		{
			if (i->first < _DONT_CARE_IDENT_)
			{
				if (matchArgs(arg, (i->second)._arg) == _YES_)
					return _YES_;
			}
		}
	}
	else if (identType == 1)
	{
		for (i = identToArgMap.begin(); i != identToArgMap.end(); i++)
		{
			if (i->first > _DONT_CARE_IDENT_)
			{
				if (matchArgs(arg, (i->second)._arg) == _YES_)
					return _YES_;
			}
		}
	}
	return _NO_;
}

int matchArgs(BaseOperation *op, ISelOp *iselOp, int dstOrSrc)
{
	ArgList & aList = op->operandList(dstOrSrc);
	ISelArgListType *iselArgs = iselOp->getList(dstOrSrc);

	// Note: matchArgs can be used to match dest or source list.
	// For certain ops the list might be empty.
	// In that case return _YES_ if the dest/source list of op is also empty
	//
	if (iselArgs == NULL)
	{
		if (aList.size() == 0)
			return _YES_;
		else
			return _NO_;
	}

	// iterate over the arglist and the iselArgs list.
	//
	ISelArgListType::iterator iselIter;

	for (iselIter = iselArgs->begin(); iselIter !=  iselArgs->end(); iselIter++)
	{
		// get the index number from iselIter and also get the corresponding arg.
		//
		int argNum = getIndex(iselIter->getArgNum(), dstOrSrc);
		int argIdent = iselIter->getIdent();

		if (aList[argNum] == NULL) return _NO_;

		// each iselarg has an identifier number. 
		// if that identifier number has been used before, it means that this arg must be the same 
		// as the arg that first used this identifier number.
		//
		// check for this condition.
		//
		// Note:
		//	CASE A: 	An identifier number 0 means don't care:
		//				i.e. no need to match args.
		//
		//      An identifier number other than 0 has two cases:
		//  CASE B:		An identifier number > 0
		//					if the argument corresponding to that number already exists,
		//				    make sure that this argument matches that previous argument
		//
		//	CASE C:		An identifier number < 0
		//					if the argument corresponding to that (negative) number already exists
		//					make sure that this argument matches that previous argument
		//                  else
		//				    make sure that this argument does not match any of 
		//					the previous negative numbered arguments
		//
		
		if (argIdent == _DONT_CARE_IDENT_) // matches CASE A.
			continue;

		BaseArgument *identArg = NULL;
		if ((identArg = getArgForIdent(iselIter->getIdent())) != NULL) // matches CASE B, CASE C when match.
		{
			// found a valid argument.
			//
			if (matchArgs(aList[argNum], identArg) == _NO_) return _NO_;
		}
		else
		{
			// this is an arg corresponding to a new identifier.
			//
			if (matchArgs(aList[argNum], *iselIter) == _NO_)
				return _NO_;
			else
			{
				if (argIdent < _DONT_CARE_IDENT_) // matches CASE C: unique number
				{
					if (matchArgs(aList[argNum], -1) == _YES_) // returns _YES_ if match with any negative arg in list.
						return _NO_;
				}
				// found a unique match. add it to the ident list.
				//
				addArgToIdentList(iselIter->getIdent(), aList[argNum], op);
			}
		}
	}
	return _YES_;
}

int matchOpcode(OpCode & opc, int iselopc)
{
	return ((opc.index() == iselopc) ? _YES_ : _NO_);
}

void translateOpCode(BaseOperation *op, int opcode)
{
	(op->getOpCode()).init(opcode);
}

BaseArgument *createNewArg(ISelArg & nArg)
{
	BaseArgument *retVal = NULL;

	// two possibilities
	//
	// 1) Need to create new Immediate Arg.
	// 2) need to create temp var arg (i.e. Register Arg).
	
	if ((nArg.getOperand()).isImmType()) 
	{
		// currently we assume that it is an integer immediate op.
		//
		retVal = new IConstArg((nArg.getOperand()).getVal());
	}
	else if ((nArg.getOperand()).isRegType()) 
	{
		retVal = createTempArg();
	}
	return retVal;
}

extern int hasDoubleProp(BaseArgument *arg);

// if the arg is not of double type, delete it.
// else, 
// if it is of double1 type, delete it and its sibling arg.
// if it is of double2 type, do not delete it.
// 
void checkAndDeleteArg(BaseArgument **argPtrPtr)
{
	if (hasDoubleProp(*argPtrPtr))
	{
		// TBD. See comment above.
		//
		assert(0);
	}
	delete *argPtrPtr;
	*argPtrPtr = NULL;
}

extern void checkAndAddNewDoubleArg(BaseOperation *op, BaseArgument *arg, int pos, int dstOrSrc, int newOp);

// if newOp == 1, op is a newly created op
// else translateArgs does an in-place translation
// 
void translateArgs(BaseOperation *op, ISelArgListType *iselArgs, int dstOrSrc, int newOp)
{
	// Note: translateArgs can be used to match dest or source list.
	// For certain ops the list might be empty.
	// In that case return
	//
	if (iselArgs == NULL) return;

	// iterate over iselArgs. if it has an identifier number, and it exists, use the argument pointed
	// to by the identifier.
	//
	ISelArgListType::iterator iselIter;

	for (iselIter = iselArgs->begin(); iselIter !=  iselArgs->end(); iselIter++)
	{
		// get the index number from iselIter and also get the corresponding arg.
		//
		int argNum = getIndex(iselIter->getArgNum(), dstOrSrc);

		BaseArgument *arg = NULL;
		if ((arg = getArgForIdent(iselIter->getArgIdent())) != NULL)
		{
			op->addOperand(arg, argNum, dstOrSrc);

			// handle the case when the arg has a double operand.
			//
			checkAndAddNewDoubleArg(op, arg, argNum, dstOrSrc, newOp);
		}
		else // arg == NULL => need to create a new arg (temp arg).
		{
			arg = createNewArg(*iselIter);

			if (arg != NULL) // add this to the identToArgMap.
				addArgToIdentList(iselIter->getIdent(), arg, op);

			op->addOperand(arg, argNum, dstOrSrc);
			delete arg;
		}
	}

	// if this is not a new op (i.e. in place translation)
	// make sure that we delete all args that are not indicated by the iselArgs list.
	if (!newOp)
	{
		ArgListIter ai;
		ArgList & l = op->operandList(dstOrSrc);
		int pos = 0;
		for (ai = l.begin(); ai != l.end(); ai++, pos++)
		{
			if (*ai == NULL)
				continue;

			// check to see if the position is present in iseArgs.
			//
			int isPresent = 0;
			for (iselIter = iselArgs->begin(); iselIter != iselArgs->end(); iselIter++)
			{
				int argNum = getIndex(iselIter->getArgNum(), dstOrSrc);

				if (pos == argNum)
				{
					isPresent = 1;
					break;
				}
			}

			if (!isPresent)
				checkAndDeleteArg(ai); // if it is of double1 type, delete both *ai, and its sibling arg.
		}
	}
}

void translateCallOp(BaseOperation *op, ISelOpListType *trule)
{
	translateOpCode(op, getOpcode(trule));
}

void translateOp(BaseOperation *op, ISelOpListType *trule)
{
	// Note: This is an in-place translation. (i.e the op is changed. No new op is created)
	//

	// SAH on 01/30/02
	//
	// special case call ops for now. 
	// Ideally, it should be  handled by using constructs to specify variable number of parms.
	//
	if (op->isCallOp())
	{
		translateCallOp(op, trule);
		return;
	}

	translateOpCode(op, getOpcode(trule));

	translateArgs(op, destList(trule), _DEST_LIST_, 0);

	translateArgs(op, sourceList(trule), _SRC_LIST_, 0);
}

int matchCallOp(BaseOperation *op, ISelOp *iselop)
{
	if (!matchOpcode(op->getOpCode(), iselop->getOpcode()))
		return _NO_;

	return _YES_;
}

int matchOp(BaseOperation *op, ISelOp *iselop)
{
	if ((op == NULL) || (iselop == NULL)) return _NO_;

	// SAH on 01/30/02
	//
	// special case call ops for now. 
	// Ideally, it should be  handled by using constructs to specify variable number of parms.
	//
	if (op->isCallOp())
	{
		return matchCallOp(op, iselop);
	}

	if (!matchOpcode(op->getOpCode(), iselop->getOpcode()))
		return _NO_;
	if (!matchArgs(op, iselop, _DEST_LIST_))
		return _NO_;
	if (!matchArgs(op, iselop, _SRC_LIST_))
		return _NO_;

	return _YES_;
}

int matchOp(BaseOperation *op, ISelOpListType *trule, int opNum)
{
	return matchOp(op, (*trule)[opNum]);
}

int translateGenericToTarget(BaseOperation *op, int ruleIndex)
{
	if (numOps(genericRules[ruleIndex]) != 1) // not possible to use this rule to translate.
		return _NO_;

	if (numOps(targetRules[ruleIndex]) == 1) // this case is easy.
	{
		if (matchOp(op, genericRules[ruleIndex], 0) == _NO_) // 0 => first operation in list of ops.
			return _NO_;

		// found a rule that matches the operation. perform the translation.
		// TBD
		//
		translateOp(op, targetRules[ruleIndex]);
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
int doInstrSelFor1Op(ISelIROpNode & opNode, int ruleIndex)
{
	if (debugNewInstrSel)
	{
		cout << endl;
		cout << "Generic: ";
		(opNode.getOp())->shortPrint(cout);
	}

	// see if the generic op can be translated to a target op.
	//
	clearIdentToArgMap();
	if (translateGenericToTarget(opNode.getOp(), ruleIndex) == _YES_)
	{
		setAsDoneInstrSel(opNode);

		if (debugNewInstrSel)
		{
			cout << "\tTarget: ";
			(opNode.getOp())->shortPrint(cout);
			cout << endl;
		}

		return _YES_;
	}
	if (debugNewInstrSel)
		cout << "\t No Simple Target." << endl;

	return _NO_;
}

void translateGenericToTarget(OpList & gList, OpList & tList, ISelOpListType *trule)
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

void changeIfOp(BaseInstruction *instr, BaseOperation *oldIfOp, BaseOperation *newIfOp)
{
	BaseOpSlot *slot = ((NormalInstr *)instr)->getPtrToSlot(oldIfOp);

	if (slot == NULL) // something's wrong.
	{
		cout << "Could not find if op in an instruction that was supposed to contain it" << endl;
		assert(0);
	}

	slot->addOperation(newIfOp);
}

void insertTargetOpsIntoIR(OpList & gList, OpList & tList)
{
	BaseInstruction *lastInstr = getISelInstrCorrespondingToOper(gList.back());

	if (lastInstr == NULL) // something's wrong.
	{
		cout << "NULL instruction encountered during Instruction Selection" << endl;
		assert(0);
	}

	int size = tList.size();

	if ((gList.back())->isControlOp()) // the last generic operation was an if op.
	{
		if (debugNewInstrSel)
		{
			cout << "Generic: " << endl;
			(gList.back())->shortPrint(cout);
			cout << endl;
			cout << "Target: " << endl;
			(tList.back())->shortPrint(cout);
			cout << endl;
		}
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
}

void markToBeDeleted(OpList & gList, OpList & tList)
{
	int size = gList.size();

	if ((gList.back())->isControlOp()) // the last generic operation was an if op.
	{
		if ((tList.back())->isControlOp()) // the last target operation too is an if op.
			size--;
	}
	for (int i = 0; i < size; i++)
	{
		markOp(gList[i]);
	}
}

int matchGenericOps(OpList & gList, int ruleIndex)
{
	// Now, gtreeList contains the list of operations. See if this matches the rule.
	// If yes, then replace the operations.
	//
	OpListIter oiter;
	int opNum = 0;
	for (oiter = gList.begin(); oiter != gList.end(); oiter++)
	{
		if (matchOp(*oiter, genericRules[ruleIndex], opNum) == _NO_)
			return _NO_;
		opNum++;
	}
	return _YES_;
}

// create tree of size "size" and match with rules.
// if a rule matches, perform the translation.
//
int doInstrSelForMultiOps(ISelIROpNode & opNode, int ruleIndex)
{
	clearIdentToArgMap();

	// Note: We are only interested in ops that reside in a basic-block.
	// So, the entire tree has to reside in a basic-block.

	int size = numOps(genericRules[ruleIndex]);

	// build the tree with the DU chain from this op to other ops.
	//
	OpList gtreeList;

	int foundMatch = 0;

	if (buildListOfISelOpsSequential(opNode.getOp(), gtreeList, size) == _YES_) // see if this matches
	{
		if (matchGenericOps(gtreeList, ruleIndex) == _YES_)
			foundMatch = 1;
	}

	if (!foundMatch)
	{
		gtreeList.clear();
		clearIdentToArgMap();

		if (buildListOfOpsFromDepChains(opNode.getOp(), gtreeList, size) == _NO_) // could not build list of ops of size "size"
			return _NO_;

		if (matchGenericOps(gtreeList, ruleIndex) == _NO_)
			return _NO_;
	}

	// the rule has been matched. now, perform the translation.
	//
	// for each op in the target rule, create a new op.
	//
	OpList ttreeList;
	translateGenericToTarget(gtreeList, ttreeList, targetRules[ruleIndex]);

	// Now, ttreeList contains all the target ops.
	// Need to insert them into the basic-block.
	//
	// insert them after the last instruction in the gtreeList.
	//
	insertTargetOpsIntoIR(gtreeList, ttreeList);

	setAsDoneInstrSel(gtreeList);

	// mark the (now useless) generic ops as to be deleted.
	//
	markToBeDeleted(gtreeList, ttreeList);

	if (debugNewInstrSel)
	{
		cout << endl << "Generic Ops: " << endl;
		
		for (OpListIter i1 = gtreeList.begin(); i1 != gtreeList.end(); i1++)
		{
			(*i1)->print(2);
			cout << endl;
		}
		cout << endl << "Target Ops: " << endl;
		
		for (OpListIter i2 = ttreeList.begin(); i2 != ttreeList.end(); i2++)
		{
			(*i2)->print(2);
			cout << endl;
		}
	}

	return _YES_;
}

void doNewInstrSel()
{
	// iterate over listOfISelIROps
	//
	Vector<ISelIROpNode>::iterator iter;

	for (iter = listOfISelIROps.begin(); iter != listOfISelIROps.end(); iter++)
	{
		if (iter->getFlg() == _YES_) // already been InstrSel'd. Do nothing.
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
		ArrayOfIntType & ruleNumList = getListOfRuleNums(opcode, 0);

		// iterate over ruleNumList.
		for (ArrayOfIntType::iterator i = ruleNumList.begin(); i != ruleNumList.end(); i++)
		{
			// if the size of generic rule and size of target rule both equal 1,
			// then we can do a simple in place translation.
			//
			// Note: It is necessary to distinguish the two cases cos IFs and CALLs are rather
			// difficult to handle, and an in-place conversion simplifies the associated duties.
			//
			if (op->isCallOp())
			{
				if ((numOps(genericRules[*i]) == 1) && (numOps(targetRules[*i]) == 1))
				{
						if (doInstrSelFor1Op(*iter, *i) == _YES_) // was successful in translating op
							break;				
				}
			}
			else
			{
				if (doInstrSelForMultiOps(*iter, *i) == _YES_) // was successful in translating op and possibly other ops.
					break;
			}
		}
	}
}

void newInstrSel(BasicBlockNode *nd)
{
	// iterate over the instrs.
	//
	// add all the ops in the instrs to listOfISelIROps
	//
	
	currBB = nd;

	listOfISelIROps.clear();

	BBForwIterator iter(nd);
	BaseInstruction *instr;

	while (iter.hasMoreElements())
	{
		instr = iter.currentElement();

		// build the operation, instruction data-structure.
		//
		addOpsToISelList(instr);

		iter.nextElement();
	}

	if (debugNewInstrSel)
		printISelList();

	// next, do Instr Sel on the list of ops.
	//
	doNewInstrSel();
}

void newInstrSel(Routine *rout)
{
	// perform instruction selection on a basic-block by basic-block basis.
	//
	if (rout == NULL) return;

	ArrayIterator<BasicBlockNode *> *iter = (rout->getCFG()).createDFSBBIter();

	while (iter->hasMoreElements())
	{
		newInstrSel(iter->currentElement());
		deleteUselessIR(*(iter->currentElement()), rout);

		iter->nextElement();
	}
	delete iter;

	listOfISelIROps.clear();
	globalSymbolTable->setStartNums();
}

extern Program *curProgram;

void newInstrSel()
{
	cout << "Performing Instruction Selection." << endl;
	// iterate over routine list
	//
	MyLinkedListIterator<Routine *> *iter = curProgram->routineListIteratorForw();

	while (iter->hasMoreElements())
	{
		newInstrSel(iter->currentElement());
		iter->nextElement();
	}
	delete iter;

	cout << "Done performing Instruction Selection." << endl;
}

