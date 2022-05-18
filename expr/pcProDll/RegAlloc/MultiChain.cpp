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
//: MultiChain.cpp
//
// File:  		MultiChain.cpp
// Created:		Fri Nov 05, 1999
// Author:		Radu Cornea
// Email:		radu@ics.uci.edu, ilp@ics.uci.edu
//
// MultiChain implements the multichain associated to each variable (argument).
// A multichain consists of a set of defs and the set of uses associated to each def.
// 

#include "stdafx.h"

#include <ostream.h>
#include "MultiChain.h"
#include "MultiChainProperty.h"
#include "DoubleArgProperty.h"
#include "STLIncludes.h"
#include "BaseOperation.h"
#include "BaseInstruction.h"
#include "SymbolTable.h"
#include "DataOpSlot.h"
#include "NormalInstr.h"
#include "MemoryOp.h"
#include "CallOp.h"
#include "ComputeOp.h"
#include "IfOp.h"
#include "DUProperty.h"
#include "Routine.h"
#include "IGNode.h"
#include "IRHelper.h"
#include "MCNode.h"
#include "DoubleMCNode.h"
#include "RoutParmProperty.h"
#include "RegArchModel.h"

#include <float.h>

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

static int debugRA = 0;

// In case of forwarding, when some reads are done at the end of the cycle,
// after the write
extern bool allowDelayedReads;
extern int isDelayedRead(BaseArgument *arg, BaseOperation *op);

extern Routine *currRoutine;
extern SymbolTable* globalSymbolTable;
extern NormalInstr *dummyInstr;
extern RegArchModel globalRegModel;

//===========================================================================
// SameSymbol
//
// test if the two symbols given as arguments are the same
//===========================================================================
extern int SameSymbol(BaseArgument* source, BaseArgument* targ);

int nestingDepth(NormalInstr *instr) {
	BaseHTGNode *node = instr;
	int depth = 0;

	while ((node = node->getUp()) != NULL)
		if (node->isLoopNode())
			depth++;

	return depth;
}

MultiChain::MultiChain(int ID) : _regClass()
{ 
	_spilled = 0;
	_defsNo = 0;
	_usesNo = 0; 
	_symIndex = globalSymbolTable->getIndex(ID);
	_ssaNum = ID - globalSymbolTable->getStartNum(_symIndex) + SSA_START_NUM;
	_siblingMC = NULL;
}

MultiChain::MultiChain(SetList<BaseOperation *>& uses,
					   SetList<BaseOperation *>& defs,
					   int ID) : _regClass() {
	_useList.removeAll();
	_defList.removeAll();
	
	_spilled = 0;
	_symIndex = globalSymbolTable->getIndex(ID);
	_ssaNum = ID - globalSymbolTable->getStartNum(_symIndex) + SSA_START_NUM;
	_siblingMC = NULL;

	MyLinkedListIterator<BaseOperation*>* iter = uses.elementsForward();
	while ( iter->hasMoreElements() ) {
		this->addUse((BaseInstruction *) iter->currentElement()->getInstr());
		iter->nextElement();
	}
	delete iter;

	iter = defs.elementsForward();
	while ( iter->hasMoreElements() ) {
		this->addDef((BaseInstruction *) iter->currentElement()->getInstr());
		iter->nextElement();
	}
	delete iter;
}

MultiChain::MultiChain(SetList<BaseInstruction *>& uses,
					   SetList<BaseInstruction *>& defs,
					   int ID) : _regClass() {
	_useList.removeAll();
	_defList.removeAll();

	_spilled = 0;
	_symIndex = globalSymbolTable->getIndex(ID);
	_ssaNum = ID - globalSymbolTable->getStartNum(_symIndex) + SSA_START_NUM;
	_siblingMC = NULL;

	MyLinkedListIterator<BaseInstruction*>* iter = uses.elementsForward();
	while ( iter->hasMoreElements() ) {
		this->addUse(iter->currentElement());
		iter->nextElement();
	}
	delete iter;

	iter = defs.elementsForward();
	while ( iter->hasMoreElements() ) {
		this->addDef(iter->currentElement());
		iter->nextElement();
	}
	delete iter;
}

MultiChain::MultiChain(MultiChain& chain) : _regClass() {
	MultiChain(chain.getUseList(), chain.getDefList(), chain.getID());
	//_spilled = 0;
}

MultiChain::~MultiChain()
{
	//removeUseDefList();
}

/*
void MultiChain::removeUseDefList()
{
	_useList.removeAll();
	_defList.removeAll();
}
*/

void MultiChain::setID(int ID) {
	_symIndex = globalSymbolTable->getIndex(ID);
	_ssaNum = ID - globalSymbolTable->getStartNum(_symIndex) + SSA_START_NUM;
}

int MultiChain::getID() {
	return (globalSymbolTable->getStartNum(_symIndex) + _ssaNum - SSA_START_NUM);
}

int MultiChain::isCallAddress() {
	return false; // FIXME!!
}

int MultiChain::isCallParamArg() {
	// an arg is call param if it is defined in dummy instruction, inserted at 
	// the beginning of each routine

	int symId = getID();

	// First, check to see if the routine has a return parm (_ROUTPARM_) property
	//
	RoutParmProperty *parmProp;

	// check to see if rptr already has the RoutParmProperty with the callParms initialized.
	//
	if ((parmProp = (RoutParmProperty *)(currRoutine->getPtrToProperty(_ROUTPARM_))) != NULL)
	{
		if (parmProp->hasCallParm(symId) == _YES_) return true;
	}

	// Commented out by SAH on Jan 12th, 2001. The piece of code above now
	// does the same work.
	//
	/*
	if (dummyInstr && _defList.contains(dummyInstr))
		return true;
	*/

	// an argument can also be a call param if it is used in a call operation
	MyLinkedListIterator<BaseInstruction *> *useInstrIter = useListIteratorForw();
	while (useInstrIter->hasMoreElements()) {
		NormalInstr *useInstr = (NormalInstr *) useInstrIter->currentElement();

		// check if the use is within a call op
		BaseOperation *op = useInstr->hasCallRetOp();
		if (op && ((CallRetOp *) op)->isCallOp()) {

			// check all params in param arg list
			ArgList& argList = ((CallRetOp *) op)->getParmList();
			ArgListIter argIter;
			for (argIter = argList.begin(); argIter != argList.end(); argIter++) {
				BaseArgument *arg = *argIter;
				
				// check the param of the call op
				int paramArgID = IRHelper::getIDOfArg(arg);
				
				if (paramArgID == getID()) {
					delete useInstrIter;
					return true;
				}
			}
		}

		useInstrIter->nextElement();
	}
	delete useInstrIter;

	return false;
}

int MultiChain::isCallReturnArg() {
	BaseArgument *arg;

	// an argument is a call return arg if it is defined in a call operation
	MyLinkedListIterator<BaseInstruction *> *defInstrIter = defListIteratorForw();
	while (defInstrIter->hasMoreElements()) {
		NormalInstr *defInstr = (NormalInstr *) defInstrIter->currentElement();

		// check if the def is within a call op
		BaseOperation *op = defInstr->hasCallRetOp();
		if (op && ((CallRetOp *) op)->isCallOp()) {

			// check the destination of the call op
			/*
			int callRetArgID = IRHelper::getIDOfArg(((CallRetOp *) op)->ptrToOperand(_DEST_, _DEST_LIST_));

			if (callRetArgID == getID()) {
				delete defInstrIter;
				return true;
			}*/

			// check the destination of the call op
			ArgList& dList = op->destOperandList();
			ArgListIter dIter;
			for (dIter = dList.begin(); dIter != dList.end(); dIter++) {

				arg = *dIter;
				if (arg) {
					
					int callRetArgID = IRHelper::getIDOfArg(arg);

					if (callRetArgID == getID()) {
						delete defInstrIter;
						return true;
					}
				}
			}
		}

		defInstrIter->nextElement();
	}
	delete defInstrIter;

	// an argument can also be a call return arg if it is used in a ret operation
	MyLinkedListIterator<BaseInstruction *> *useInstrIter = useListIteratorForw();
	while (useInstrIter->hasMoreElements()) {
		NormalInstr *useInstr = (NormalInstr *) useInstrIter->currentElement();

		// check if the use is within a ret op
		BaseOperation *op = useInstr->hasCallRetOp();
		if (op && ((CallRetOp *) op)->isRetOp()) {

			/*
			//check the first argument of ret op
			BaseArgument *retArg = ((CallRetOp *) op)->ptrToOperand(_SOURCE_1_, _SRC_LIST_);
			if (retArg) {
				int retArgID = IRHelper::getIDOfArg(retArg);
				
				if (retArgID == getID()) {
					delete useInstrIter;
					return true;
				}
			}

			//check the second argument of ret op
			retArg = ((CallRetOp *) op)->ptrToOperand(_SOURCE_2_, _SRC_LIST_);
			if (retArg) {
				int retArgID = IRHelper::getIDOfArg(retArg);
				
				if (retArgID == getID()) {
					delete useInstrIter;
					return true;
				}
			}*/

			//check the arguments of ret op
			ArgList& sList = op->sourceOperandList();
			ArgListIter sIter;
			for (sIter = sList.begin(); sIter != sList.end(); sIter++) {

				arg = *sIter;
				if (arg) {
					
					int callRetArgID = IRHelper::getIDOfArg(arg);

					if (callRetArgID == getID()) {
						delete useInstrIter;
						return true;
					}
				}
			}
		}

		useInstrIter->nextElement();
	}
	delete useInstrIter;

	return false;
}

int MultiChain::isStackPointerArg() {
	return false;	// This is handled by the general case; all SPs are allocated to the target SP
}

extern isCompareOp(int opCode);

// Added by SAH on 05/25/01. Witnessed by RC and MNM on the same day.
//
int checkCompareOpsWithSameDef(NormalInstr *defInstr, int id)
{
	BaseOperation *oper;

	ArgListIter i1, i2;

	// iterate over all the data ops of defInstr.
	//
	ArrayIterator<DataOpSlot> *iter = defInstr->getDataOpSlotIter();

	while (iter->hasMoreElements())
	{
		if ((iter->currentElement()).hasOper())
		{
			oper = (iter->currentElement()).getPtrToOperation();

			// now we have the operation.
			// check if it is a cmp op, and also if it contains dest with id.
			//
			if (isCompareOp((oper->getOpCode()).index()))
			{
				// go thru its destination list.
				//
				i1 = (oper->destOperandList()).begin();
				i2 = (oper->destOperandList()).end();

				while (i1 != i2)
				{
					if (*i1)
					{
						if ((IRHelper::getIDOfArg(*i1)) == id)
						{
							delete iter;
							return _YES_;
						}
					}
					i1++;
				}
			}
		}
			iter->nextElement();
	}
	delete iter;

	return _NO_;
}

int MultiChain::isConditionCodeArg() {
	// an argument is a condition code arg if it is used in an if operation
	MyLinkedListIterator<BaseInstruction *> *useInstrIter = useListIteratorForw();
	while (useInstrIter->hasMoreElements()) {
		NormalInstr *useInstr = (NormalInstr *) useInstrIter->currentElement();

		// check if the use is within an if op
		BaseOperation *op = useInstr->hasIfOp();
		if (op) {
			int ccArgID = IRHelper::getIDOfArg(((IfOp *) op)->ptrToCCReg());

			if (ccArgID == getID()) {
				delete useInstrIter;
				return true;
			}
		}

		useInstrIter->nextElement();
	}
	delete useInstrIter;

	// Added by SAH on 05/25/01. Witnessed by RC and MNM on the same day.
	//
	// an argument is a condition code arg if it is defined by a COMPARE operation.
	//
	MyLinkedListIterator<BaseInstruction *> *defInstrIter = defListIteratorForw();
	while (defInstrIter->hasMoreElements()) {
		NormalInstr *defInstr = (NormalInstr *) defInstrIter->currentElement();

		// for each operation in the defInstr, check :
		// 1) if it is an ELF target compare op.
		// 2) if it contains the id of this multichain.
		//
		if (checkCompareOpsWithSameDef(defInstr, getID()) == _YES_)
			return true;

		defInstrIter->nextElement();
	}
	delete defInstrIter;

	return false;
}

int MultiChain::isDoubleFloatArg() {
	
	// an argument is a double float arg if it is part of a double pair
	MCNode *mcNode = currRoutine->getIG().getMCNode(this);
	if (mcNode->getDoubleMCNode() != NULL) {
		return true;
	}

	return false;
}

char * MultiChain::getName()
{
	return globalSymbolTable->getRegName(getID());
}

int MultiChain::isEq(MultiChain *mChain) {
	
	return (this == mChain);

	/*
	//compare the def lists
	if ( _defList.numItems() == mChain->getDefList().numItems() ) {
		MyLinkedListIterator<BaseInstruction *>* iter = mChain->defListIteratorForw();
		while ( iter->hasMoreElements() ) {
			if ( _defList.contains(iter->currentElement()) == _NO_ )
				return _NO_;
			iter->nextElement();
		}
		delete iter;
	} else 
		return _NO_;
	
	//compare the use lists
	if ( _useList.numItems() == mChain->getUseList().numItems() ) {
		MyLinkedListIterator<BaseInstruction*>* iter = mChain->useListIteratorForw();
		while ( iter->hasMoreElements() ) {
			if ( _useList.contains(iter->currentElement()) == _NO_ )
				return _NO_;
			iter->nextElement();
		}
		delete iter;
	} else 
		return _NO_;
	
	return _YES_;
	*/
} 

int MultiChain::noOfDefs()
{
	return _defsNo;
}

int MultiChain::noOfUses()
{
	return _usesNo;
}

void MultiChain::computeSpillCost()
{
	_spillCost = 0;

	MyLinkedListIterator<BaseInstruction *>* defIter = defListIteratorForw();
	while (defIter->hasMoreElements()) {
		NormalInstr *instr = (NormalInstr *) defIter->currentElement();

		_spillCost += 1 * pow(10, nestingDepth(instr));

		//cout << "   Depth = " << nestingDepth(instr) << " ";
		//instr->shortPrint(cout);

		defIter->nextElement();
	}
	delete defIter;

	MyLinkedListIterator<BaseInstruction *>* useIter = useListIteratorForw();
	while (useIter->hasMoreElements()) {
		NormalInstr *instr = (NormalInstr *) useIter->currentElement();

		// if there is only one use ant that is a delayed read,
		// don't allow spilling
		if (allowDelayedReads) {

			int argID = getID();
			SetList<BaseArgument *> *useArgs = IRHelper::getUsesOfArgInInstr(argID, instr);
			MyLinkedListIterator<BaseArgument *> *useArgIter = useArgs->elementsForward();
			while (useArgIter->hasMoreElements()) {
				BaseArgument *useArg = useArgIter->currentElement();

				if ((noOfUses() == 1) && isDelayedRead(useArg, IRHelper::getOperationWithArgInInstr(useArg, (NormalInstr *) instr))) {
					_spillCost = DBL_MAX / 1000;

					delete useArgIter;
					delete useArgs;
					break;
				}

				useArgIter->nextElement();
			}
			delete useArgIter;
			delete useArgs;
		}

		_spillCost += 1 * pow(10, nestingDepth(instr));

		//cout << "   Depth = " << nestingDepth(instr) << " ";
		//instr->shortPrint(cout);

		useIter->nextElement();
	}
	delete useIter;
}

double MultiChain::getSpillCost()
{
	if ((noOfDefs() == 0) || (noOfUses() == 0))
		return DBL_MAX / 1000;

	// we do not want to spill a variable more than once
	if (isSpilled())
		return DBL_MAX / 1000;

	computeSpillCost();

	return _spillCost;
}

SetList<BaseArgument *> * MultiChain::getAllArgs()
{
	SetList<BaseArgument *> *args = new SetList<BaseArgument *>();
	int id = getID();

	MyLinkedListIterator<BaseInstruction *> *defIter = _defList.elementsForward();
	while (defIter->hasMoreElements()) {
		BaseInstruction *defInstr = defIter->currentElement();

		BaseArgument *defArg = IRHelper::getDefOfArgInInstr(id, defInstr);

		if (defArg)		// in case of dummyInstr, it may be NULL
			args->add(defArg);

		defIter->nextElement();
	}
	delete defIter;
	
	MyLinkedListIterator<BaseInstruction *> *useIter = _useList.elementsForward();
	while (useIter->hasMoreElements()) {
		BaseInstruction *useInstr = useIter->currentElement();

		SetList<BaseArgument *> *useArg = IRHelper::getUsesOfArgInInstr(id, useInstr);
		args->set_union(*useArg);
		delete useArg;

		useIter->nextElement();
	}
	delete useIter;

	return args;
}

void MultiChain::unionWith(MultiChain& prop) 
{
	addDefList(prop.getDefList());
	addUseList(prop.getUseList());
}

// recomputes the register class for the current multichain
Vector<RegClassEntryNode2>& MultiChain::computeRegClass()
{
	// fill argList with all arguments (uses and defs) from the multichain
	SetList<BaseArgument *> *args = getAllArgs();
	ASSERT_TRACE(!args->isEmpty(), "The multichain has no arguments");

	// get list of register classes for the generic register arguments
	_regClass.clear();
	globalRegModel.createListOfClassTypesForArgs(args, _regClass, _GENERIC_);
	delete args;

	return _regClass;
}

// if regClass is already set for the current multichain, return it
// otherwise recompute it
Vector<RegClassEntryNode2>& MultiChain::getRegClass()
{
	if (!_regClass.empty())
		return _regClass;
	else
		return computeRegClass();
}

void MultiChain::print(ostream& out) {
	
	int ID = getID();
	char *regName = globalSymbolTable->getRegName(ID);

	out << "{ MC-" << ID << "(" << regName << ") ";
	if (_spilled)
		cout << "spilled " << _spilled << " times ";
	cout << "}";

	delete regName;
	
	/*
	out << endl;
	out << "--------------------------------------------" << endl;
	out << "MULTICHAIN OF " << globalSymbolTable->getRegName(_ID);
	out << " (id=" << _ID << ")" << endl;
	out << endl;
	out << "THE DEF LIST: ";
	out << "(" << _defList.numItems() << " items)" << endl;
	
	  MyLinkedListIterator<BaseInstruction *> *iter = defListIteratorForw();
	  while ( iter->hasMoreElements() ) {
	  //out << "(* " << iter->currentElement() << " *)";
	  iter->currentElement()->shortPrint(out);
	  //out << endl;
	  iter->nextElement();
	  }
	  
		out << "THE USE LIST: ";
		out << "(" << _useList.numItems() << " items)" << endl;
		
		  iter = useListIteratorForw();
		  while ( iter->hasMoreElements() ) {
		  //out << "(* " << iter->currentElement() << " *)";
		  iter->currentElement()->shortPrint(out);
		  //out << endl;
		  iter->nextElement();
		  }
		  delete iter;
		  
			out << "++++++++++++++++++++++++++++++++++++++++++++" << endl;
	*/	
}


MultiChain * MultiChain::getMCOfArgID(BaseArgument *defArg, Routine *routine)
{
	InterferenceGraph& ig = routine->getIG();
	int ID = IRHelper::getIDOfArg(defArg);

	MultiChain *pMC = ig.getMC(ID);

	if ( pMC == NULL) { // ID does not exist already in ig
		pMC = new MultiChain(ID);
		ig.setMC(ID, pMC);
		// add the multichain to the interference graph
		ig.addMultiChain(pMC);
	}
	
	// test for possible double argument
	BaseArgument *sibArg = IRHelper::getSiblingArg(defArg);
	if (sibArg) {
		
		int sibID = IRHelper::getIDOfArg(sibArg);
		
		MultiChain *sibMC = ig.getMC(sibID);
		
		if ( sibMC == NULL) { // sibID does not exist already in ig
			sibMC = new MultiChain(sibID);
			ig.setMC(sibID, sibMC);
			// add the multichain to the interference graph
			ig.addMultiChain(sibMC);
		}
		
		// check if double node not already there
		if (ig.getMCNode(pMC)->getDoubleMCNode() == NULL) {
			
			if (ig.getMCNode(sibMC)->getDoubleMCNode() != NULL) {
				cout << "MULTICHAIN: " << *sibMC << "is present in two pairs:" << endl;
				cout << "   Pair 1: ";
				ig.getMCNode(sibMC)->getDoubleMCNode()->print(cout);
				cout << endl;
				cout << "   Pair 2: Double :" << endl << "[" << endl << *pMC << endl << *sibMC << endl << "]" << endl;
				ASSERT_TRACE(0, "The node is part of two different double pairs");
			}
			
			pMC->setSiblingMC(sibMC);
			sibMC->setSiblingMC(pMC);
				
			if (ID < sibID)
				ig.makeDoubleMCNode(pMC, sibMC);
			else
				ig.makeDoubleMCNode(sibMC, pMC);
		} else {
			if (ig.getMCNode(pMC)->getDoubleMCNode() != ig.getMCNode(sibMC)->getDoubleMCNode()) {

				
				cout << "MULTICHAIN: " << *pMC << "is present in two pairs:" << endl;
				cout << "   Pair 1: ";
				ig.getMCNode(pMC)->getDoubleMCNode()->print(cout);
				cout << endl;
				cout << "   Pair 2: Double :" << endl << "[" << endl << *pMC << endl << *sibMC << endl << "]" << endl;

				ASSERT_TRACE(0, "There is already a different double pair in the IG");
			}
		}
	}
	
	return pMC;
}

void MultiChain::compMultiChainsOfInstr(BaseInstruction *instr, Routine *routine) {
	
	if (debugRA) {
	//	cout << "computing multichain of instruction: " << endl;
	//	instr->shortPrint(cout);
	}
	
	MultiChain *pMC;

	MyLinkedList<BaseArgument *> *defArgList = IRHelper::getAllDefsInInstr(instr);
	MyLinkedListIterator<BaseArgument *> *defArgIter = defArgList->elementsForward();
	
	while (defArgIter->hasMoreElements()) {
		BaseArgument *defArg = defArgIter->currentElement();
		
		//updateMCOfArg(defArg, instr, routine);
		pMC = getMCOfArgID(defArg, routine);

		pMC->addDef(instr);
	
		MultiChainProperty tmp(pMC);
		defArg->addProperty(tmp);

// NEW VERSION : Works with RegArchModel.cpp
// #if 0
		// compute the register class property for defArg
		BaseOperation * defOp = IRHelper::getOperationWithArgInInstr(defArg, (NormalInstr *) instr);
		globalRegModel.setRegClassPropertyForArg(defArg, defOp);
// #endif		
		defArgIter->nextElement();
	}
	
	delete defArgIter;
	delete defArgList;

	MyLinkedList<BaseArgument *> *useArgList = IRHelper::getAllUsesInInstr(instr);
	MyLinkedListIterator<BaseArgument *> *useArgIter = useArgList->elementsForward();
	
	while (useArgIter->hasMoreElements()) {
		BaseArgument *useArg = useArgIter->currentElement();
		
		//updateMCOfArg(useArg, instr, routine);
		pMC = getMCOfArgID(useArg, routine);
		
		pMC->addUse(instr);
	
		MultiChainProperty tmp(pMC);
		useArg->addProperty(tmp);

// NEW VERSION : Works with RegArchModel.cpp
// #if 0
		// compute the register class property for useArg
		BaseOperation * useOp = IRHelper::getOperationWithArgInInstr(useArg, (NormalInstr *) instr);
		globalRegModel.setRegClassPropertyForArg(useArg, useOp);
// #endif		
		useArgIter->nextElement();
	}
	
	delete useArgIter;
	delete useArgList;
}

//========================================================================
// Routine::compMultiChains
//
// compute the multichains of variables inside a routine
//========================================================================

void Routine::compMultiChains() {

	if (debugRA)
		cout << "Begin to compute the multichains of variables" << endl;


	if (isEmptyBodyRoutine()) return;

	MyLinkedListIterator<BaseInstruction *> *instrIter = _instrList.elementsForward();
	while (instrIter->hasMoreElements()) {
		BaseInstruction *instr = instrIter->currentElement();

		MultiChain::compMultiChainsOfInstr(instr, this);

		instrIter->nextElement();
	}

	delete instrIter;

	if (debugRA)
		cout << "End of computing multichains" << endl;
}



