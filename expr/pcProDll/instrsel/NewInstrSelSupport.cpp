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
// File:  		NewInstrSelSupport.cpp
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
#include "ISelParse.h"
#include "OpCodeTable.h"
#include "FilePointer.h"
#include "DataopSlot.h"
#include "ControlOpSlot.h"
#include "IConstArg.h"


//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

/***************************************************************************************/
/***************************************************************************************/
// The following section of the file is to provide support 
// to NewInstrSel.cpp and ISelParse.y functions.
/***************************************************************************************/
/***************************************************************************************/

// global variables declaration.
//
ISelRuleType genericRules;
ISelRuleType targetRules;

OpcToRuleNoType ISelOpcToRuleNo;

extern OpCodeTable globalOpCodeTable;

// returns the number of ops that compose the rule.
// a rule could be a single op mapping (such as iadd -> add) or multiple ops.
//
int numOps(ISelOpListType *rule)
{
	if (rule == NULL) return _INVALID_;

	return rule->size();
}

// returns the opcode of the n'th operation in the rule.
// (A rule is a list of ops).
//
// By default, returns the opcode of the first operation in the rule.
//
int getOpcode(ISelOpListType *rule, int opPos)
{
	if (rule == NULL) return _INVALID_;

	if (rule->size() < opPos) return NULL; // error condition.

	return ((*rule)[opPos]->getOpcode());
}


// returns the destination list of the n'th operation in the rule.
// (A rule is a list of ops).
//
// By default, returns the destination list of the first operation in the rule.
//
ISelArgListType *destList(ISelOpListType *rule, int opPos)
{
	if (rule == NULL) return NULL;

	if (rule->size() < opPos) return NULL; // error condition.

	return ((*rule)[opPos]->getDstList());
}

// returns the source list of the n'th operation in the rule.
// (A rule is a list of ops).
//
// By default, returns the source list of the first operation in the rule.
//
ISelArgListType *sourceList(ISelOpListType *rule, int opPos)
{
	if (rule == NULL) return NULL;

	if (rule->size() < opPos) return NULL; // error condition.

	return ((*rule)[opPos]->getSrcList());
}

// if type == 0, use ISelOpcToRuleNo, else use OptOpcToRuleNo
//
ArrayOfIntType & getListOfRuleNums(OpCode & opc, int type)
{
	int opindex = opc.index();

	return ((type == 0) ? ISelOpcToRuleNo[opindex] : OptOpcToRuleNo[opindex]);
}

void ISelOp::addOpcode(char *opcode)
{
     int opcodeIndex;
	 opcodeIndex = globalOpCodeTable.getIndex(opcode);
	 _opcode = opcodeIndex;
	 return;
}
    
// check if intVal satisfies the condition imposed by oprnd.
//
int satisfiesImmCond(int intVal, Oprnd & oprnd)
{
	int oprtr = oprnd.getOperator();
	int val = oprnd.getVal();

	if (oprtr == _INVALID_) return _YES_;

	if ((oprtr == _EQ_OPERATOR_) && (intVal == val)) return _YES_;
	if ((oprtr == _LT_OPERATOR_) && (intVal < val)) return _YES_;

	return _NO_;
}

int satisfiesImmCond(double fltVal, Oprnd & oprnd)
{
	// Not handled as of now.
	//
	return _YES_;
}

extern int getOpcodeIndex(char *opcode);

// if type == 0, use ISelOpcToRuleNo, else use OptOpcToRuleNo
//
int existsCurrentOp(int currOp, int type)
{
	OpcToRuleNoType::iterator i;
	
	if (type == 0)
	{
		if( (i = ISelOpcToRuleNo.find(currOp)) != ISelOpcToRuleNo.end() )
		{
			return 1;
		}
	}
	else
	{
		if( (i = OptOpcToRuleNo.find(currOp)) != OptOpcToRuleNo.end() )
		{
			return 1;
		}
	}
	return 0;
}


// if type == 0, use ISelOpcToRuleNo, else use OptOpcToRuleNo
//
ArrayOfIntType &getRuleNumList(int currOp, int type)
{
     OpcToRuleNoType::iterator i;

	 if (type == 0)
		 i = ISelOpcToRuleNo.find(currOp);
	 else
		 i = OptOpcToRuleNo.find(currOp);

	 return (*i).second;
}

void initInstrSel()
{
    try{
           FilePointer fp("ISelMapping.txt", "r");
	       ISelrestart(fp.file_ptr());
	       ISelparse();
	}
    catch (CouldNotOpenFileException e)
    {
      cout << "Panic: Could Not Open File" << endl;
      assert(0);
    }
}

// if there exists a rule with size greater than "size" for op, returns the size of that rule.
// else returns _INVALID_.
// the size of a rule is the number of operations in that rule.
//
// if type == 0, use ISelOpcToRuleNo, else use OptOpcToRuleNo
//
int sizeOfMinSizeRule(BaseOperation *op, int size, int type)
{
	int retVal = _INVALID_;

	int setSize = 0;
	int temp;

	// get the opcode from the op.
	//
	int opIndex = (op->getOpCode()).index();

	// next get the set of all rules that are indexed by this op.
	//
	ArrayOfIntType & rList = getRuleNumList(opIndex, type);

	// find the smallest rule greater than size "size".
	//
	for (ArrayOfIntType::iterator i = rList.begin(); i != rList.end(); i++)
	{
		// TBD.
		// Based on type, either use genericRules or lhsRules
		//
		assert(0);

		if ((temp = numOps(genericRules[*i])) > size)
		{
			if (setSize == 0)
			{
				setSize = 1;
				retVal = temp;
			}
			else
			{
				retVal = ((temp < retVal) ? temp : retVal);
			}
		}
	}

	return retVal;
}

Vector<ISelIROpNode>::iterator findOperFromListOfISelIROps(BaseOperation *op)
{
	Vector<ISelIROpNode>::iterator retVal;

	for (retVal = listOfISelIROps.begin(); retVal != listOfISelIROps.end(); retVal++)
	{
		if (*retVal == op)
			return retVal;
	}
	return retVal;
}

// return _YES_ if op matches the operation in rule[pos].
//
// the operations match if:
//
// 1) The opcodes match.
//
int matchesOpInRule(BaseOperation *op, int pos, ISelOpListType *rule)
{
	// get the opcode from the op.
	//
	int opIndex = (op->getOpCode()).index();

	if (opIndex == getOpcode(rule, pos))
		return _YES_;

	return _NO_;
}

// currently, we assume that the last operation in the rule is the root.
//
int getRootTreePos(ISelOpListType *rule)
{
	if (rule == NULL) return _INVALID_;

	return (rule->size() - 1);
}

// if arg's identifier number exists in any definition of the ops in rule.
// return the position of that op.
// else return _INVALID_
// 
int hasDefInRuleTree(ISelArg & arg, ISelOpListType *rule)
{
	int retPos = 0;
	ISelArgListType::iterator dI1, dI2;

	int argIdent = arg.getIdent();

	// iterate over the operations in rule.
	ISelOpListType::iterator i;
	for (i = rule->begin(); i != rule->end(); i++, retPos++)
	{
		ISelOp *currOp = *i;

		// Now, go thru the currOp's destination list and see if any dest's identifier matches argIdent.
		//
		dI1 = dI2 = NULL;
		if (currOp->getDstList() != NULL)
		{
			dI1 = (currOp->getDstList())->begin();
			dI2 = (currOp->getDstList())->end();
		}
		while (dI1 != dI2)
		{
			if (dI1->getIdent() == argIdent)
				return retPos;

			dI1++;
		}
	}
	return _INVALID_;
}

/***************************************************************************************/
/***************************************************************************************/
// The following section of the file is to provide support to NewInstrSel.cpp functions.
/***************************************************************************************/
/***************************************************************************************/

Vector<BaseInstruction *> doneInstrSelVect;
Vector<ISelIROpNode> listOfISelIROps;

Map<int, ArgAndOper> identToArgMap; // this stores the argument corresponding to each identifier. 
// e.g. DEST[1] REG (N). then N would be the first element 
// and the argument corresponding to DEST[1] would be the second element of the map.

BaseArgument *getArgForIdent(int ident)
{
	Map<int, ArgAndOper>::iterator i;
	if ((i = identToArgMap.find(ident)) != identToArgMap.end())
		return (i->second)._arg;

	return NULL;
}

BaseArgument *getArgForIdent(ArgIdent & ident)
{
	// two cases:
	//
	// 1) ident->getIDType() == _ID_SINGLE_
	// 2) ident->getIDType() == _ID_EXPRESSION_
	//
	if (ident.getIDType() == _ID_SINGLE_)
	{
		int id = ident.getIdent();
		Map<int, ArgAndOper>::iterator i;
		if ((i = identToArgMap.find(id)) != identToArgMap.end())
			return (i->second)._arg;
	}
	else if (ident.getIDType() == _ID_EXPRESSION_)
	{
		// currently, the only types of expressions allowed are:
		//
		// arg1(imm) + arg2(imm)
		// arg1(imm) - arg2(imm)
		//
		int id1, id2;
		id1 = ident.getIdent1();
		id2 = ident.getIdent2();

		Map<int, ArgAndOper>::iterator i;
		if ((i = identToArgMap.find(id1)) == identToArgMap.end()) // error condition ?
			return NULL;
		BaseArgument *arg1 = (i->second)._arg;

		if ((i = identToArgMap.find(id2)) == identToArgMap.end()) // error condition ?
			return NULL;
		BaseArgument *arg2 = (i->second)._arg;

		if ((arg1 == NULL) || (arg2 == NULL)) // error condition ?
			return NULL;

		if ((!arg1->isIConstArg()) || (!arg2->isIConstArg())) // error condition ?
			return NULL;

		BaseArgument *newArg = arg1->copy();

		if (ident.getOperator() == _ADD_OPERATOR_)
			((IConstArg *)newArg)->changeValue(((IConstArg *)arg1)->value() + ((IConstArg *)arg2)->value());
		else if (ident.getOperator() == _SUB_OPERATOR_)
			((IConstArg *)newArg)->changeValue(((IConstArg *)arg1)->value() - ((IConstArg *)arg2)->value());

		return newArg;
	}
	return NULL;
}

void addArgToIdentList(int ident, BaseArgument *arg, BaseOperation *op)
{
	ArgAndOper aop;
	aop._op = op;
	aop._arg = arg->copy();

	identToArgMap[ident] = aop;
}

void clearIdentToArgMap()
{
	// delete all the args.
	//
	Map<int, ArgAndOper>::iterator i;

	for (i = identToArgMap.begin(); i != identToArgMap.end(); i++)
	{
		if ((i->second)._arg != NULL)
			delete (i->second)._arg;
	}
	identToArgMap.clear();
}

inline int getIndex(int num, int destOrSrc)
{
	if (destOrSrc == _DEST_LIST_)
		return (_DST_ + num - 1);
	else // if (destOrSrc == _SRC_LIST_)
		return (_SRC_1_ + num - 1);
}

void setAsDoneInstrSel(BaseOperation *op)
{
	Vector<ISelIROpNode>::iterator iter;

	for (iter = listOfISelIROps.begin(); iter != listOfISelIROps.end(); iter++)
	{
		if (*iter == op)
		{
			iter->setFlg(_YES_);
			return;
		}
	}

	// error condition.
	//
	assert(0);
}

void setAsDoneInstrSel(ISelIROpNode & op)
{
	op.setFlg(_YES_);
}

void setAsDoneInstrSel(OpList & oList)
{
	// iterate over list of ops.
	//
	OpListIter i;
	for (i = oList.begin(); i != oList.end(); i++)
	{
		setAsDoneInstrSel(*i);
	}
}

int doneInstrSel(BaseOperation *op)
{
	Vector<ISelIROpNode>::iterator iter;

	for (iter = listOfISelIROps.begin(); iter != listOfISelIROps.end(); iter++)
	{
		if (*iter == op)
		{
			if (iter->getFlg() == _YES_)
				return _YES_;
			break;
		}
	}
	return _NO_;
}

void resetISelFlg()
{
	Vector<ISelIROpNode>::iterator iter;

	for (iter = listOfISelIROps.begin(); iter != listOfISelIROps.end(); iter++)
		iter->setFlg(_INVALID_);
}

// return _YES_ if operation is present in listOfISelIROps
// else return _NO_

int isPresentInISelList(BaseOperation *op)
{
	// iterate over listOfISelIROps.
	//
	Vector<ISelIROpNode>::iterator iter;

	for (iter = listOfISelIROps.begin(); iter != listOfISelIROps.end(); iter++)
	{
		if (*iter == op)
			return _YES_;
	}
	return _NO_;
}

BaseInstruction *getISelInstrCorrespondingToOper(BaseOperation *op)
{
	// iterate over listOfISelIROps.
	//
	Vector<ISelIROpNode>::iterator iter;

	for (iter = listOfISelIROps.begin(); iter != listOfISelIROps.end(); iter++)
	{
		if (*iter == op)
			return iter->getInstr();
	}
	return NULL;
}

void addOpsToISelList(BaseInstruction *instr)
{
	ISelIROpNode temp;
	temp.setInstr(instr);

	// iterate over the instruction and add each op to listOfISelIROps
	//
	ArrayIterator<DataOpSlot> *diter;
	diter = ((NormalInstr *)instr)->getDataOpSlotIter();

	while (diter->hasMoreElements())
	{
		if ((diter->currentElement()).hasOper())
		{
			temp.setOp((diter->currentElement()).getPtrToOperation());
			listOfISelIROps.push_back(temp);
		}
		diter->nextElement();
	}
	delete diter;

	ArrayIterator<ControlOpSlot> *citer;
	citer = ((NormalInstr *)instr)->getControlOpSlotIter();

	while (citer->hasMoreElements())
	{
		if ((citer->currentElement()).hasOper())
		{
			temp.setOp((citer->currentElement()).getPtrToOperation());
			listOfISelIROps.push_back(temp);
		}
		citer->nextElement();
	}
	delete citer;
}

void printISelList()
{
	cout << "Printing ISelList: " << endl;

	// iterate over listOfISelIROps.
	//
	Vector<ISelIROpNode>::iterator iter;

	for (iter = listOfISelIROps.begin(); iter != listOfISelIROps.end(); iter++)
	{
		(iter->getOp())->shortPrint(cout);
		cout << endl;
	}
}

BaseOperation *getFirstOpInBBFromDUProp(DUChainProperty *prop)
{
	BaseOperation *retVal = NULL;
	MyLinkedListIterator<BaseOperation *> *iter = prop->useListIteratorForw();

	while (iter->hasMoreElements())
	{
		if (isPresentInISelList(iter->currentElement())) // isPresentInISelList is the same as asking if the op is present in the bb.
		{
			retVal = iter->currentElement();
			break;
		}
		iter->nextElement();
	}
	delete iter;
	return retVal;
}

// Use this function to build a list of ops (belonging to the bb)
// based on the dependence chain (i.e op1.def -> op2.src, op2.def -> op3.src, ...)
//
int buildListOfOpsFromDepChains(BaseOperation *op, OpList & treeList, int size)
{
	BaseOperation *currOp = op;
	treeList.push_back(currOp);

	BaseOperation *depOp;
	BaseArgProperty *prop;
	for (int i = 0; i < size - 1; i++)
	{
		// as of now we only look at the first dest position in currOp.
		//
		BaseArgument *dArg = currOp->ptrToOperand(_DST_, _DEST_LIST_);

		if (dArg == NULL) // NULL destination.
		{
			// cannot build the list of ops, return _NO_
			//
			return _NO_;
		}
		if ((prop = dArg->getPtrToProperty(_DUCHAIN_)) != NULL)
		{
			// right now, we get the first op in use list that is also present in the basic-blcok.
			// later on change this to search thru all ops in the use list that belong to the bb.
			//
			// also, later on, change this to ensure that the op that is selected has not already been converted to target.
			//
			depOp = getFirstOpInBBFromDUProp((DUChainProperty *)prop);

			if (depOp != NULL)
				currOp = depOp;
			else
			{
				// could not find an op.
				return _NO_;
			}
			treeList.push_back(currOp);
		}
		else // error.
		{
			cout << "Found a dest without a DUChain property (operation: " << currOp->idValue() << ")"<< endl;
			assert(0);
		}
	}
	return _YES_;
}

// use this function to build a list of ops (belonging to the bb)
// in sequential order.
// i.e location(op1) = N, location(op2) = N+1, ...
//
int buildListOfISelOpsSequential(BaseOperation *op, OpList & seqList, int size)
{
	BaseOperation *currOp = op;

	Vector<ISelIROpNode>::iterator i = findOperFromListOfISelIROps(currOp);

	if (i == listOfISelIROps.end()) return _NO_;

	seqList.push_back(currOp);
	i++;

	int count = 0;
	for (i; (i != listOfISelIROps.end() && count < size - 1); i++, count++)
	{
		seqList.push_back((*i).getOp());
	}

	if (count < size - 1)
		return _NO_;
	return _YES_;
}

BaseOperation *buildShellOperation(int opcode)
{
	return (buildShellOperation((globalOpCodeTable[opcode]).opName()));
}

BaseInstruction *getISelPrevInstr(BaseInstruction *instr)
{
	BaseInstruction *start = (listOfISelIROps[0]).getInstr();

	return getPrevInstr(instr, start);
}
