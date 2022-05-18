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
//: IRHelper.cpp
//
// File:  		IRHelper.cpp
// Author:		Radu Cornea
// Email:		radu@ics.uci.edu, ilp@ics.uci.edu
//
// Miscellaneous helper functions
// 

#include "stdafx.h"
#include "IRHelper.h"
#include "SymbolTable.h"
#include "DataOpSlot.h"
#include "ControlOpSlot.h"
#include "MemoryOp.h"
#include "CallOp.h"
#include "ComputeOp.h"
#include "IfOp.h"
#include "NormalInstr.h"
#include "DoubleArgProperty.h"


//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: FAST_MODE

extern SymbolTable *globalSymbolTable;
extern int SameSymbol(BaseArgument* source, BaseArgument* targ);
extern isMove(BaseOperation * oper);

// In case of forwarding, when some reads are done at the end of the cycle,
// after the write
extern int isDelayedRead(BaseArgument *arg, BaseOperation *op);



//========================================================================
// getIDOfArg
//
// return the unique ID associated with arg from the symbol table
//========================================================================
int IRHelper::getIDOfArg(BaseArgument *arg) {
	return globalSymbolTable->getID(arg, arg->getPtrToProperty(_SSA_));
}

// create a new RealRegArg based on its SymTab ID
BaseArgument * IRHelper::createArg(int argID) {
	// create the RealRegArg
	int regIndex = globalSymbolTable->getIndex(argID);
	BaseArgument *regArg = globalRegFileList->getRealReg(regIndex);

	// initialize the SSA property
	int ssaNum = argID - globalSymbolTable->getStartNum(regIndex) + SSA_START_NUM;
	SSAArgProperty prop(ssaNum);
	regArg->addProperty(prop);

	return regArg;
}

BaseArgument * IRHelper::getSiblingArg(BaseArgument *arg) {

	BaseArgument *sibArg = NULL;
	DoubleArgProperty *dblProp = (DoubleArgProperty *) arg->getPtrToProperty(_DOUBLEARG_);

	if (dblProp)
		sibArg = dblProp->getSiblingArg();

	return sibArg;
}

//========================================================================
// getAllDefsInOper
//
// return a list of all variable definitions in the operation
//========================================================================

SetList<BaseArgument *> * IRHelper::getAllDefsInOper(BaseOperation *op) {

	BaseArgument *arg;

	if (op == NULL)
		return NULL;
	
	SetList<BaseArgument *> *defList = new SetList<BaseArgument *>();
	
	ArgList& dList = op->destOperandList();
	ArgListIter dIter;
	for (dIter = dList.begin(); dIter != dList.end(); dIter++) {
		
		arg = *dIter;
		if (arg && arg->isRegisterArg())
			defList->add(arg);
	}
	
	return defList;
}

//========================================================================
// getAllDefsInInstr
//
// return a list of all variable definitions in the instruction
//========================================================================

SetList<BaseArgument *> * IRHelper::getAllDefsInInstr(BaseInstruction *instr) {
	SetList<BaseArgument *> *defList = new SetList<BaseArgument *>();
	BaseArgument * arg;
	
	// data operations
	ArrayIterator<DataOpSlot>* dataOpIter = ((NormalInstr*)instr)->getDataOpSlotIter();
	while (dataOpIter->hasMoreElements()) {
		BaseOperation* op = dataOpIter->currentElement().getPtrToOperation();
		
		if (op == NULL) {
			dataOpIter->nextElement();
			continue;
		}
		
		ArgList& dList = op->destOperandList();
		ArgListIter dIter;
		for (dIter = dList.begin(); dIter != dList.end(); dIter++) {

			arg = *dIter;
			if (arg && arg->isRegisterArg())
				defList->add(arg);
		}
		
		dataOpIter->nextElement();
	}	
	
	delete dataOpIter;
	
	// control operations
	ArrayIterator<ControlOpSlot>* cntlOpIter = ((NormalInstr*)instr)->getControlOpSlotIter();
	while (cntlOpIter->hasMoreElements()) {
		BaseOperation* op = cntlOpIter->currentElement().getPtrToOperation();
		
		if (op == NULL) {
			cntlOpIter->nextElement();
			continue;
		}
		
		ArgList& dList = op->destOperandList();
		ArgListIter dIter;
		for (dIter = dList.begin(); dIter != dList.end(); dIter++) {

			arg = *dIter;
			if (arg && arg->isRegisterArg())
				defList->add(arg);
		}
		
		cntlOpIter->nextElement();
	}
	delete cntlOpIter;
	
	return defList;
}

//========================================================================
// getAllUsesInOper
//
// return a list of all variable references in the operation
//========================================================================

SetList<BaseArgument *> * IRHelper::getAllUsesInOper(BaseOperation *op) {
	
	BaseArgument *arg;

	if (op == NULL)
		return NULL;

	SetList<BaseArgument *> *useList = new SetList<BaseArgument *>();
	
    if (op->isCallOp()) {
        
		// the first source argument is the function "name".
		// Sometimes, this can be a register too.
		//
		arg = op->ptrToOperand(_SRC_1_);
		if (arg && arg->isRegisterArg())
			useList->add(arg);

        ArgList& sList = ((CallRetOp *) op)->getParmList();
        ArgListIter sIter;
        for (sIter = sList.begin(); sIter != sList.end(); sIter++) {
            
            arg = *sIter;
            if (arg && arg->isRegisterArg())
                useList->add(arg);
        }
        
    } else {
        
        ArgList& sList = op->sourceOperandList();
        ArgListIter sIter;
        for (sIter = sList.begin(); sIter != sList.end(); sIter++) {
            
            arg = *sIter;
            if (arg && arg->isRegisterArg())
                useList->add(arg);
        }
    }
        
	return useList;
}


//========================================================================
// getAllUsesInInstr
//
// return a list of all variable references in the instruction
//========================================================================

SetList<BaseArgument *> * IRHelper::getAllUsesInInstr(BaseInstruction *instr) {
	
	SetList<BaseArgument *> *useList = new SetList<BaseArgument *>();
	BaseArgument * arg;

	// data operations
	ArrayIterator<DataOpSlot>* dataOpIter = ((NormalInstr*)instr)->getDataOpSlotIter();
	while (dataOpIter->hasMoreElements()) {
		BaseOperation* op = dataOpIter->currentElement().getPtrToOperation();
		
		if (op == NULL) {
			dataOpIter->nextElement();
			continue;
		}
		
        if (op->isCallOp()) {
            
            ArgList& argList = ((CallRetOp*)op)->getParmList();
            ArgListIter argIter;
            for (argIter = argList.begin(); argIter != argList.end(); argIter++) {
                
                arg = *argIter;
                if (arg && arg->isRegisterArg())
                    useList->add(arg);
                
            }
        }else {
            
            ArgList& sList = op->sourceOperandList();
            ArgListIter sIter;
            for (sIter = sList.begin(); sIter != sList.end(); sIter++) {
                
                arg = *sIter;
                if (arg && arg->isRegisterArg())
                    useList->add(arg);
            }
        }
        
        dataOpIter->nextElement();
    }
	delete dataOpIter;
	
	// control operations
	ArrayIterator<ControlOpSlot>* cntlOpIter = ((NormalInstr*)instr)->getControlOpSlotIter();
	while (cntlOpIter->hasMoreElements()) {
		BaseOperation* op = cntlOpIter->currentElement().getPtrToOperation();
		
		if (op == NULL) {
			cntlOpIter->nextElement();
			continue;
		}

		ArgList& sList = op->sourceOperandList();
		ArgListIter sIter;
		for (sIter = sList.begin(); sIter != sList.end(); sIter++) {

			arg = *sIter;
			if (arg && arg->isRegisterArg())
				useList->add(arg);
		}

		if (op->isCallOp()) {

			ArgList& argList = ((CallRetOp*)op)->getParmList();
			ArgListIter argIter;
			for (argIter = argList.begin(); argIter != argList.end(); argIter++) {
				
				arg = *argIter;
				if (arg && arg->isRegisterArg())
					useList->add(arg);
			}
			
			ArgList& savedRegs = ((CallRetOp*)op)->getSavedRegList();
			for (argIter = savedRegs.begin(); argIter!=savedRegs.end(); argIter++) {
				
				arg = *argIter;
				if (arg && arg->isRegisterArg())
					useList->add(arg);
			}
		}
		
		cntlOpIter->nextElement();
	}
	delete cntlOpIter;
	
	return useList;
}

SetList<BaseArgument *> * IRHelper::getAllDelayedReadsInInstr(BaseInstruction *instr)
{
	SetList<BaseArgument *> *uses = IRHelper::getAllUsesInInstr(instr);
	MyLinkedListIterator<BaseArgument *> *useIter = uses->elementsForward();
	while (useIter->hasMoreElements()) {
		BaseArgument *use = useIter->currentElement();
		useIter->nextElement();

		if (!isDelayedRead(use, IRHelper::getOperationWithArgInInstr(use, (NormalInstr *) instr)))
			uses->remove(use);
	}
	delete useIter;

	return uses;
}

SetList<BaseArgument *> * IRHelper::getAllNotDelayedReadsInInstr(BaseInstruction *instr)
{
	SetList<BaseArgument *> *uses = IRHelper::getAllUsesInInstr(instr);
	MyLinkedListIterator<BaseArgument *> *useIter = uses->elementsForward();
	while (useIter->hasMoreElements()) {
		BaseArgument *use = useIter->currentElement();
		useIter->nextElement();

		if (isDelayedRead(use, IRHelper::getOperationWithArgInInstr(use, (NormalInstr *) instr)))
			uses->remove(use);
	}
	delete useIter;

	return uses;
}

//========================================================================
// getAllMoveOpUsesInInstr
//
// return a list of all variable references in the instruction
//========================================================================

SetList<BaseArgument *> * IRHelper::getAllMoveOpUsesInInstr(BaseInstruction *instr) {
	
	SetList<BaseArgument *> *useList = new SetList<BaseArgument *>();
	BaseArgument * arg;

	// data operations
	ArrayIterator<DataOpSlot>* dataOpIter = ((NormalInstr*)instr)->getDataOpSlotIter();
	while (dataOpIter->hasMoreElements()) {
		BaseOperation* op = dataOpIter->currentElement().getPtrToOperation();
		
		if ((op == NULL) || !isMove(op)) {
			dataOpIter->nextElement();
			continue;
		}
		
		ArgList& sList = op->sourceOperandList();
		ArgListIter sIter;
		for (sIter = sList.begin(); sIter != sList.end(); sIter++) {

			arg = *sIter;
			if (arg && arg->isRegisterArg())
				useList->add(arg);
		}
		
		dataOpIter->nextElement();
	}
	delete dataOpIter;
	
	return useList;
}

//========================================================================
// getDefOfArgInOper
//
// find the definition of arg in the operation (returns NULL if not)
//========================================================================

BaseArgument * IRHelper::getDefOfArgInOper(BaseArgument *arg,
											BaseOperation *op) {
	
	SetList<BaseArgument *> *defList = getAllDefsInOper(op);
	MyLinkedListIterator<BaseArgument *> *defIter = defList->elementsForward();
	
	while (defIter->hasMoreElements()) {
		BaseArgument *def = defIter->currentElement();
		
		if (SameSymbol(arg, def)) {
			delete defIter;
			delete defList;
			
			return def;
		}
		
		defIter->nextElement();
	}
	
	delete defIter;
	delete defList;
	
	return NULL;
}

//========================================================================
// getDefOfArgInInstr
//
// find the definition of arg in the instruction (returns NULL if not)
//========================================================================

BaseArgument * IRHelper::getDefOfArgInInstr(BaseArgument *arg,
											  BaseInstruction * instr) {
	
	SetList<BaseArgument *> *defList = getAllDefsInInstr(instr);
	MyLinkedListIterator<BaseArgument *> *defIter = defList->elementsForward();
	
	while (defIter->hasMoreElements()) {
		BaseArgument *def = defIter->currentElement();
		
		if (SameSymbol(arg, def)) {
			delete defIter;
			delete defList;
			
			return def;
		}
		
		defIter->nextElement();
	}
	
	delete defIter;
	delete defList;
	
	return NULL;
}

BaseArgument * IRHelper::getDefOfArgInOper(int argID,
											  BaseOperation * oper) {
	
	SetList<BaseArgument *> *defList = getAllDefsInOper(oper);
	MyLinkedListIterator<BaseArgument *> *defIter = defList->elementsForward();
	
	while (defIter->hasMoreElements()) {
		BaseArgument *def = defIter->currentElement();
		
		if (getIDOfArg(def) == argID) {
			delete defIter;
			delete defList;
			
			return def;
		}
		
		defIter->nextElement();
	}
	
	delete defIter;
	delete defList;
	
	return NULL;
}

BaseArgument * IRHelper::getDefOfArgInInstr(int argID,
											  BaseInstruction * instr) {
	
	SetList<BaseArgument *> *defList = getAllDefsInInstr(instr);
	MyLinkedListIterator<BaseArgument *> *defIter = defList->elementsForward();
	
	while (defIter->hasMoreElements()) {
		BaseArgument *def = defIter->currentElement();
		
		if (getIDOfArg(def) == argID) {
			delete defIter;
			delete defList;
			
			return def;
		}
		
		defIter->nextElement();
	}
	
	delete defIter;
	delete defList;
	
	return NULL;
}

//========================================================================
// getUsesOfArgInOper
//
// find and return a list of all uses of arg in the operation (or NULL)
//========================================================================

SetList<BaseArgument *> * IRHelper::getUsesOfArgInOper(BaseArgument *arg,
															   BaseOperation *op) {
	
	SetList<BaseArgument *> *useList = getAllUsesInOper(op);
	SetList<BaseArgument *> *argUseList = new SetList<BaseArgument *>();
	
	MyLinkedListIterator<BaseArgument *> *useIter = useList->elementsForward();
	
	while (useIter->hasMoreElements()) {
		BaseArgument *use = useIter->currentElement();
		
		if (SameSymbol(arg, use))
			argUseList->add(use);
		
		useIter->nextElement();
	}
	
	delete useIter;
	delete useList;
	
	return argUseList;
}


//========================================================================
// getUsesOfArgInInstr
//
// find and return a list of all uses of arg in the instruction (or NULL)
//========================================================================

SetList<BaseArgument *> * IRHelper::getUsesOfArgInInstr(BaseArgument *arg,
															   BaseInstruction *instr) {
	
	SetList<BaseArgument *> *useList = getAllUsesInInstr(instr);
	SetList<BaseArgument *> *argUseList = new SetList<BaseArgument *>();
	
	MyLinkedListIterator<BaseArgument *> *useIter = useList->elementsForward();
	
	while (useIter->hasMoreElements()) {
		BaseArgument *use = useIter->currentElement();
		
		if (SameSymbol(arg, use))
			argUseList->add(use);
		
		useIter->nextElement();
	}
	
	delete useIter;
	delete useList;
	
	return argUseList;
}

SetList<BaseArgument *> * IRHelper::getUsesOfArgInOper(int argID,
															   BaseOperation *op) {
	
	SetList<BaseArgument *> *useList = getAllUsesInOper(op);
	SetList<BaseArgument *> *argUseList = new SetList<BaseArgument *>();
	
	MyLinkedListIterator<BaseArgument *> *useIter = useList->elementsForward();
	
	while (useIter->hasMoreElements()) {
		BaseArgument *use = useIter->currentElement();
		
		if (getIDOfArg(use) == argID)
			argUseList->add(use);
		
		useIter->nextElement();
	}
	
	delete useIter;
	delete useList;
	
	return argUseList;
}


SetList<BaseArgument *> * IRHelper::getUsesOfArgInInstr(int argID,
															   BaseInstruction *instr) {
	
	SetList<BaseArgument *> *useList = getAllUsesInInstr(instr);
	SetList<BaseArgument *> *argUseList = new SetList<BaseArgument *>();
	
	MyLinkedListIterator<BaseArgument *> *useIter = useList->elementsForward();
	
	while (useIter->hasMoreElements()) {
		BaseArgument *use = useIter->currentElement();
		
		if (getIDOfArg(use) == argID)
			argUseList->add(use);
		
		useIter->nextElement();
	}
	
	delete useIter;
	delete useList;
	
	return argUseList;
}

// This method tests the equality of arg pointers, not contents
BaseOperation * IRHelper::getOperationWithArgInInstr(BaseArgument *testArg, NormalInstr *instr)
{
	BaseArgument *arg;

	// data operations
	ArrayIterator<DataOpSlot>* dataOpIter = ((NormalInstr*)instr)->getDataOpSlotIter();
	while (dataOpIter->hasMoreElements()) {
		BaseOperation* op = dataOpIter->currentElement().getPtrToOperation();
		
		if (op == NULL) {
			dataOpIter->nextElement();
			continue;
		}
		
        // search for testArg in dest list
        ArgList& dList = op->destOperandList();
        ArgListIter dIter;
        for (dIter = dList.begin(); dIter != dList.end(); dIter++) {
            
            arg = *dIter;
            if (arg == testArg) {
                delete dataOpIter;
                return op;
            }
        }
        
        // search for testArg in src list
        if (op->isCallOp()) {
            
            ArgList& argList = ((CallRetOp*)op)->getParmList();
            ArgListIter argIter;
            for (argIter = argList.begin(); argIter != argList.end(); argIter++) {
                
                arg = *argIter;
                if (arg == testArg) {
                    delete dataOpIter;
                    return op;
                }
            }
        } else {
            ArgList& sList = op->sourceOperandList();
            ArgListIter sIter;
            for (sIter = sList.begin(); sIter != sList.end(); sIter++) {
                
                arg = *sIter;
                if (arg == testArg) {
                    delete dataOpIter;
                    return op;
                }
            }
        }
        
		dataOpIter->nextElement();
	}
	delete dataOpIter;
	
	// control operations
	ArrayIterator<ControlOpSlot>* cntlOpIter = ((NormalInstr*)instr)->getControlOpSlotIter();
	while (cntlOpIter->hasMoreElements()) {
		BaseOperation* op = cntlOpIter->currentElement().getPtrToOperation();

		if (op == NULL) {
			cntlOpIter->nextElement();
			continue;
		}

		// search for testArg in dest list
		ArgList& dList = op->destOperandList();
		ArgListIter dIter;
		for (dIter = dList.begin(); dIter != dList.end(); dIter++) {

			arg = *dIter;
			if (arg == testArg) {
				delete cntlOpIter;
				return op;
			}
		}

		// search for testArg in src list
		ArgList& sList = op->sourceOperandList();
		ArgListIter sIter;
		for (sIter = sList.begin(); sIter != sList.end(); sIter++) {

			arg = *sIter;
			if (arg == testArg) {
				delete cntlOpIter;
				return op;
			}
		}

		if (op->isCallOp()) {

			ArgList& argList = ((CallRetOp*)op)->getParmList();
			ArgListIter argIter;
			for (argIter = argList.begin(); argIter != argList.end(); argIter++) {
				
				arg = *argIter;
				if (arg == testArg) {
					delete cntlOpIter;
					return op;
				}
			}
			
			ArgList& savedRegs = ((CallRetOp*)op)->getSavedRegList();
			for (argIter = savedRegs.begin(); argIter!=savedRegs.end(); argIter++) {
				
				arg = *argIter;
				if (arg == testArg) {
					delete cntlOpIter;
					return op;
				}
			}
		}
		
		cntlOpIter->nextElement();
	}
	delete cntlOpIter;
	
	return NULL;
}

//========================================================================
// ::addArgsToLiveList
//
// add the variables from argList to liveList
//========================================================================

void IRHelper::addArgsToLiveList(MyLinkedList<BaseArgument *> *argList, SetList<int>& liveList)
{
	MyLinkedListIterator<BaseArgument *> *argIter = argList->elementsForward();
	while (argIter->hasMoreElements()) {
		BaseArgument *arg = argIter->currentElement();

		int id = getIDOfArg(arg);
		liveList.add(id);

		argIter->nextElement();
	}
	delete argIter;
}

//========================================================================
// ::delArgsFromLiveList
//
// add the variables from argList to liveList
//========================================================================

void IRHelper::delArgsFromLiveList(MyLinkedList<BaseArgument *> *argList, SetList<int>& liveList)
{
	MyLinkedListIterator<BaseArgument *> *argIter = argList->elementsForward();
	while (argIter->hasMoreElements()) {
		BaseArgument *arg = argIter->currentElement();

		int id = getIDOfArg(arg);
		liveList.remove(id);

		argIter->nextElement();
	}
	delete argIter;
}


void IRHelper::changeRegWithSSA(BaseArgument *toChange, BaseArgument *orig)
{
	// changes "toChange" argument to the symbol values in "orig" argument.
	// Also, modifies the ssa prop of "toChange" to that of "orig" argument.

	((RealRegArg *) toChange)->changeReg(orig);

	BaseArgProperty *prop1, *prop2;
	prop1 = toChange->getPtrToProperty(_SSA_);
	prop2 = orig->getPtrToProperty(_SSA_);
	((SSAArgProperty *) prop1)->setSSANum(((SSAArgProperty *) prop2)->getSSANum());
}