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
#include "stdafx.h"
#include "STLIncludes.h"
#include "CallOp.h"
#include "RoutParmProperty.h"
#include "Routine.h"
#include "Program.h"
#include "MyString.h"
#include "ComputeOp.h"
#include "NormalInstr.h"
#include "JumpOp.h"


/*****************************************************************************
 ********************** IMPORTANT ********************************************
 *
 * The system call portion of call patching has been commented out for ELF compiler
 *
 * Uncomment all comments marked with SYS_CALL when system calls are to be
 * supported by ELF.
 *
 ****************************************************************************/

// SYS_CALL

// #include "../../systemDll/derivedClassDll/SysCallExecSem.h"

// End SYS_CALL

// This function does three things:
//
// 1) It determines the return type of each routine in the program (either int or float)
// 2) It adds the appropriate argument to each RETURN statement in the program.
// 3) it deterimnes the call input parms for each routine in the program.
// 
// The way it works is:
//
// First, for every routine, find the expected register return (either f0 or r4) for each call.
// Maintain a mapping between routines and the register returns.
// Also, maintain the list of input arguments for each routine.
// 
// Second, patch each return in a routine with the register expected as the return (from the mapping).
//
// 

// this function is used by patchCallForDoubles
//
void patchCallInputParmInfo(BaseArgument *arg, Routine *rptr)
{
	RoutParmProperty *parmProp;

	if ((arg == NULL) || (rptr == NULL)) return;

	// check to see if rptr already has the RoutParmProperty with the callParms initialized.
	//
	if ((parmProp = (RoutParmProperty *)(rptr->getPtrToProperty(_ROUTPARM_))) == NULL)
	{
		// first add an empty property, and then get the pointer to it

		RoutParmProperty tempProp;
		rptr->addProperty(tempProp);
		parmProp = (RoutParmProperty *)(rptr->getPtrToProperty(_ROUTPARM_));
	}

	if (arg->getUniqID() != _INVALID_)
		parmProp->addCallParm(arg);

	// Finally, set the status to _INITIALIZED_
	//
	parmProp->setCallParmStatus(_INITIALIZED_);
}

void addCallInputParmInfo(ArgList & parms, Routine *rptr)
{
	RoutParmProperty *parmProp;

	// we know that rptr is not NULL.

	// check to see if rptr already has the RoutParmProperty with the callParms initialized.
	//
	if ((parmProp = (RoutParmProperty *)(rptr->getPtrToProperty(_ROUTPARM_))) != NULL)
	{
		if (parmProp->getCallParmStatus() != _INVALID_) // the property has already been initialized.
		{
			// TBD : We need to check if the arglist (parms) specified here is the same as that in parmProp.
			// If not, it is an error condition, cos it essentially implies that the call has (atleast)
			// 2 different sets of input arguments.

			return; // do nothing.
		}
	}
	else // first add an empty property, and then get the pointer to it
	{
		RoutParmProperty tempProp;
		rptr->addProperty(tempProp);
		parmProp = (RoutParmProperty *)(rptr->getPtrToProperty(_ROUTPARM_));

		// also add sp.
		//
		RealRegArg sp("gsp", 0, globalRegFileList->getIndex("gsp"));
		if (sp.getUniqID() != _INVALID_)
			parmProp->addCallParm(&sp);
	}

	// now, we have parmProp. Go thru the list of parms and add each parm to parmProp.
	//
	ArgListIter i;
	for (i = parms.begin(); i != parms.end(); i++)
	{
		if ((*i)->getUniqID() != _INVALID_)
			parmProp->addCallParm(*i);
	}
	// Finally, set the status to _INITIALIZED_
	//
	parmProp->setCallParmStatus(_INITIALIZED_);
}

// this function is used by patchCallForDoubles
//
void patchCallReturnInfo(BaseArgument *arg, Routine *rptr)
{
	RoutParmProperty *parmProp;

	// we know that rptr is not NULL.

	// check to see if rptr already has the RoutParmProperty with the callParms initialized.
	//
	if ((parmProp = (RoutParmProperty *)(rptr->getPtrToProperty(_ROUTPARM_))) == NULL)
	{
		// first add an empty property, and then get the pointer to it

		RoutParmProperty tempProp;
		rptr->addProperty(tempProp);
		parmProp = (RoutParmProperty *)(rptr->getPtrToProperty(_ROUTPARM_));
	}

	// now, we have parmProp. Add the arg to parmProp.
	//
	if ((arg)->getUniqID() != _INVALID_)
		parmProp->addCallParm(arg);

	// Finally, set the status to _INITIALIZED_
	//
	parmProp->setReturnParmStatus(_INITIALIZED_);
}

void addCallReturnInfo(BaseArgument *arg, Routine *rptr)
{
	RoutParmProperty *parmProp;

	// we know that rptr is not NULL.

	// check to see if rptr already has the RoutParmProperty with the callParms initialized.
	//
	if ((parmProp = (RoutParmProperty *)(rptr->getPtrToProperty(_ROUTPARM_))) != NULL)
	{
		if (parmProp->getReturnParmStatus() != _INVALID_) // the property has already been initialized.
		{
			// TBD : We need to check if the arg specified here is the same as that in parmProp.
			// If not, it is an error condition, cos it essentially implies that the call has (atleast)
			// 2 different return args.

			return; // do nothing.
		}
	}
	else // first add an empty property, and then get the pointer to it
	{
		RoutParmProperty tempProp;
		rptr->addProperty(tempProp);
		parmProp = (RoutParmProperty *)(rptr->getPtrToProperty(_ROUTPARM_));

		// also add sp.
		//
		RealRegArg sp("gsp", 0, globalRegFileList->getIndex("gsp"));
		if (sp.getUniqID() != _INVALID_)
			parmProp->addReturnParm(&sp);

	}

	// now, we have parmProp. Add the arg to parmProp.
	//
	if ((arg)->getUniqID() != _INVALID_)
		parmProp->addReturnParm(arg);

	// Finally, set the status to _INITIALIZED_
	//
	parmProp->setReturnParmStatus(_INITIALIZED_);
}

void inferCallInfo(CallRetOp *callOp)
{
	// We already know that this is a call op.
	//
	BaseArgument *arg;
	Routine *rptr;

	if (callOp->isDynamicAddrCall()) // we do not know which routine is going to be called, so do nothing.
		return;

	arg = callOp->ptrToOperand(_SOURCE_1_);
	
	if ((arg == NULL) || (arg->isLabelArg() != _YES_))
		return;

	if ((rptr = curProgram->getRoutine(arg->lName())) == NULL)
		return;

	// now go thru the list of input parameters and add the input parms and return type info.

	addCallInputParmInfo(callOp->getParmList(), rptr);
	
	if (callOp->hasOperand(_DEST_, _DEST_LIST_))
		addCallReturnInfo(callOp->ptrToOperand(_DEST_, _DEST_LIST_), rptr);
}

void buildCallArgInfo(Routine *rout)
{
	MyLinkedListIterator<BaseInstruction *> *riter;
	
	BaseInstruction *curI;
	BaseOperation *curO;
	
	ArrayIterator<ControlOpSlot> *cIter;
	
	// Now, iterate thru all the instructions in the routine.
	//
	riter = rout->instrListIteratorForw();
	while (riter->hasMoreElements())
	{
		curI = riter->currentElement();
		
		// Now, iterate thru all the control operations in the routine.
		
		cIter = ((NormalInstr *)curI)->getControlOpSlotIter();
		while (cIter->hasMoreElements())
		{
			if ((cIter->currentElement()).hasOper())
			{
				curO = (cIter->currentElement()).getPtrToOperation();
				
				if (curO->isCallOp())
				{
					inferCallInfo((CallRetOp *)curO);
				}
			}
			cIter->nextElement();
		}
		delete cIter;
		
		riter->nextElement();
	}
	delete riter;
}

void patchReturn(CallRetOp *retOp, int retId)
{
	// we know that this is a return op.
	//
	// First, build a real reg arg with retId
	//
	// Next, add it to the return op.

	BaseArgument *retArg = globalRegFileList->getRealReg(retId);

	if (retArg != NULL)
	{
		retOp->addOperand(retArg, _SOURCE_1_);
		delete retArg;
	}
}

void patchReturns(Routine *rout)
{
	MyLinkedListIterator<BaseInstruction *> *riter;
	
	BaseInstruction *curI;
	BaseOperation *curO;
	
	ArrayIterator<ControlOpSlot> *cIter;
	
	// First, check to see if the routine has a return parm (_ROUTPARM_) property
	//
	RoutParmProperty *parmProp;

	int retId = _INVALID_;

	// check to see if rptr already has the RoutParmProperty with the callParms initialized.
	//
	if ((parmProp = (RoutParmProperty *)(rout->getPtrToProperty(_ROUTPARM_))) != NULL)
	{
		if (parmProp->getReturnParmStatus() == _INITIALIZED_) // the property has already been initialized.
		{
			if (!(parmProp->getReturnParms()).empty())
			{
				retId = ((parmProp->getReturnParms()).front())->getIDWithoutSSA();
			}
		}
		// else : error condition. may need to flag this as an error
	}
	// else : error condition. may need to flag this as an error
	
	if (retId == _INVALID_) // either the routine does not have a return or there has been an error
		return;

	// Now, iterate thru all the instructions in the routine.
	//
	riter = rout->instrListIteratorForw();
	while (riter->hasMoreElements())
	{
		curI = riter->currentElement();
		
		// Now, iterate thru all the control operations in the routine.
		
		cIter = ((NormalInstr *)curI)->getControlOpSlotIter();
		while (cIter->hasMoreElements())
		{
			if ((cIter->currentElement()).hasOper())
			{
				curO = (cIter->currentElement()).getPtrToOperation();
				
				if (curO->isRetOp())
				{
					patchReturn((CallRetOp *)curO, retId);
				}
			}
			cIter->nextElement();
		}
		delete cIter;
		
		riter->nextElement();
	}
	delete riter;
}

void setCallParmTypes(MyString & fName);

extern int defines(BaseInstruction *instr, BaseArgument *arg);
extern int uses(BaseInstruction *instr, BaseArgument *arg);

int needsArgC(Routine *rout)
{
	// iterate thru the instrs in the routine.
	// if R4 is used before being defined return _YES_

	int retVal = _NO_;

	RealRegArg r4("gR", 4, globalRegFileList->getIndex("gR"));
	BaseArgument *arg = &r4;

	MyLinkedListIterator<BaseInstruction *> *iter = rout->instrListIteratorForw();

	while (iter->hasMoreElements())
	{
		BaseInstruction *instr = iter->currentElement();

		if (defines(instr, arg))
		{
			retVal = _NO_;
			break;
		}
		if (uses(instr, arg))
		{
			retVal = _YES_;
			break;
		}
		iter->nextElement();
	}
	delete iter;

	return retVal;
}

int needsArgV(Routine *rout)
{
	// iterate thru the instrs in the routine.
	// if R4 is used before being defined return _YES_

	int retVal = _NO_;

	RealRegArg r5("gR", 5, globalRegFileList->getIndex("gR"));
	BaseArgument *arg = &r5;

	MyLinkedListIterator<BaseInstruction *> *iter = rout->instrListIteratorForw();

	while (iter->hasMoreElements())
	{
		BaseInstruction *instr = iter->currentElement();

		if (defines(instr, arg))
		{
			retVal = _NO_;
			break;
		}
		if (uses(instr, arg))
		{
			retVal = _YES_;
			break;
		}
		iter->nextElement();
	}
	delete iter;

	return retVal;
}

void firstPassCallPatch(Program *prog)
{
	if (prog == NULL) return;

	Routine *curR;

	// First, build the call args info.
	//
	// Iterate thru each routine in the program.
	//
	MyLinkedListIterator<Routine *> *piter;
	piter = prog->routineListIteratorForw();

	while (piter->hasMoreElements())
	{
		curR = piter->currentElement();

		buildCallArgInfo(curR);

		piter->nextElement();
	}
	delete piter;

	// Now, it is possible to patch the returns with the appropriate arg.
	//
	piter = prog->routineListIteratorForw();

	while (piter->hasMoreElements())
	{
		curR = piter->currentElement();

		patchReturns(curR);

		piter->nextElement();
	}
	delete piter;

	// The main routine needs to be patched correctly.
	//
	Routine *mainR = prog->getMainRoutine();
	ArgList mainRParmList;
	RealRegArg r4("gR", 4, globalRegFileList->getIndex("gR"));
	RealRegArg r5("gR", 5, globalRegFileList->getIndex("gR"));

	// check to see if the main routine needs r4 and r5 (i.e. needs argc, argv).
	//
	BaseArgument *arg;
	if (needsArgC(mainR))
	{
		arg = &r4;
		mainRParmList.push_back(arg);
	}
	if (needsArgV(mainR))
	{
		arg = &r5;
		mainRParmList.push_back(arg);
	}

	addCallInputParmInfo(mainRParmList, mainR);
	arg = &r4;
	addCallReturnInfo(arg, mainR);

	MyString fName("callParmTypes");
	setCallParmTypes(fName);
}

int ssaize(int regId, int ssaNum = 1)
{
	// need to find the uniqID from globalSymbolTable.
	//
	if ((!doneSSA) || (doneRA)) // error condition.
	{
		cout << "Panic: Tried to ssaize a value either before SSA or after RA" << endl;
		assert(0);
	}	
	return globalSymbolTable->getID(regId, ssaNum);
}

int getCallParmInputSSANum(BaseArgument *arg, Routine *rout)
{
	//the first instruction of rout is the correction instr that contains the ssa nums
	//of the call parms.
	//
	BaseOperation *corrOp = ((NormalInstr *)(((rout->getCFG()).getFirstBB()).getFirstInstr()))->getPtrToRootOperation();

	if (corrOp == NULL) return _INVALID_;
	//go thru the dest list of corrOp and search for this arg.
	//
	int argId = arg->getIDWithoutSSA();

	ArgListIter i;
	ArgList & dList = corrOp->destOperandList();
	for (i = dList.begin(); i != dList.end(); i++)
	{
		if (*i == NULL) continue;

		if (argId == ((*i)->getIDWithoutSSA()))
		{
			return (((SSAArgProperty *)((*i)->getPtrToProperty(_SSA_)))->getSSANum());
		}
	}
	return _INVALID_;
}

void ssaizeInputParms(Routine *rout)
{
	// if the routine has callInput ROUTPARM property, then, iterate over it
	// and change each arg index to its .1 (dot one) SSA version.

	RoutParmProperty *parmProp;

	// The default SSA Num is 1.
	//
	SSAArgProperty ssaP(1);
	// check to see if rout already has the RoutParmProperty with the callParms initialized.
	//
	if ((parmProp = (RoutParmProperty *)(rout->getPtrToProperty(_ROUTPARM_))) != NULL)
	{
		if (parmProp->getCallParmStatus() == _INITIALIZED_) // the property has already been initialized.
		{
			ArgList & inpList = parmProp->getCallParms();
			ArgListIter i;

			for (i = inpList.begin(); i != inpList.end(); i++)
			{
				int ssaNum = getCallParmInputSSANum(*i, rout);

				if (ssaNum == _INVALID_) // error condition ??
					ssaNum = 1;
				
				ssaP.setSSANum(ssaNum);
				(*i)->addProperty(ssaP);
			}
		}
	}
	else // error condition
	{
		cout << "Warning: In Call Patch : ROUTPARM property was not already present in a routine" << endl;
		return;
	}

	// Finally, set the status to _RSSAIZED_
	//
	parmProp->setCallParmStatus(_RSSAIZED_);
}

int ssaize(CallRetOp *retOp)
{
	BaseArgument *arg;

	// we already know that this is a return op.
	//
	// check to see if it has _SOURCE_1_ and then get its uniqId
	//
	if ((arg = retOp->ptrToOperand(_SOURCE_1_)) != NULL)
	{
		return (arg->getUniqID());
	}
	return _INVALID_;
}

void ssaizeReturnParms(Routine *rout)
{
	// go thru the routine.
	// for each return statement, add the return value reg to the ROUTPARM property.
	//
	MyLinkedListIterator<BaseInstruction *> *riter;
	
	BaseInstruction *curI;
	BaseOperation *curO;
	
	ArrayIterator<ControlOpSlot> *cIter;
	
	// First, check to see if the routine has a return parm (_ROUTPARM_) property
	//
	RoutParmProperty *parmProp;

	// check to see if rptr already has the RoutParmProperty with the callParms initialized.
	//
	if ((parmProp = (RoutParmProperty *)(rout->getPtrToProperty(_ROUTPARM_))) == NULL)
	{
		// may need to flag this as an error.
		//
		return;
	}
	if (parmProp->getReturnParmStatus() == _INITIALIZED_) // the property has already been initialized.
	{
		// may need to flag this as an error
		//
		return;
	}

	// first clear the return parm
	//
	ArgListIter tempI;
	for (tempI = (parmProp->getReturnParms()).begin(); tempI != (parmProp->getReturnParms()).end(); tempI++)
	{
		if ((*tempI) != NULL) delete (*tempI);
	}
	(parmProp->getReturnParms()).clear();

	// Now, go iterate over the instructions in the routine, search for return ops, and add them to the returnParms list.
	//
	riter = rout->instrListIteratorForw();

	while (riter->hasMoreElements())
	{
		curI = riter->currentElement();
		
		// Now, iterate thru all the control operations in the routine.
		
		cIter = ((NormalInstr *)curI)->getControlOpSlotIter();
		while (cIter->hasMoreElements())
		{
			if ((cIter->currentElement()).hasOper())
			{
				curO = (cIter->currentElement()).getPtrToOperation();
				
				if (curO->isRetOp())
				{
					(parmProp->getReturnParms()).push_back(curO->ptrToOperand(_SOURCE_1_));
/*
					newId = ssaize((CallRetOp *)curO);

					if (newId != _INVALID_)
						(parmProp->getReturnParms()).push_back(newId);
*/
				}
			}
			cIter->nextElement();
		}
		delete cIter;
		
		riter->nextElement();
	}
	delete riter;
}

void secondPassCallPatch(Program *prog)
{
	if (prog == NULL) return;

	// First, iterate thru each routine in the program.
	//
	MyLinkedListIterator<Routine *> *piter;
	piter = prog->routineListIteratorForw();

	Routine *curR;

	while (piter->hasMoreElements())
	{
		curR = piter->currentElement();

		// First, change each input parm in the ROUTPARM property to its .1 ssa version
		//
		ssaizeInputParms(curR);

		// Next, determine the exit points and change/add the ssa versions of the return args
		//
		ssaizeReturnParms(curR);

		piter->nextElement();
	}
	delete piter;
}

// SYS_CALL

// extern SysCallPtrObjMap systemCalls;

// End SYS_CALL

void patchCallForDoubleArg(CallRetOp *cop, int pos);
extern int consecutiveArgs(BaseArgument *arg1, BaseArgument *arg2);

// Note: We assume that the IR has not been SSAized.
//
void patchCallForDoubleArgBasedOnArgs(CallRetOp *cop)
{
	// iterate over the list of params.
	// Ignore it if it is R4, R5.
	// If after R4 it is R6, then it is a double arg (R6).
	//
	// After that, add double arg to the last arg.

	int count = (cop->getParmList()).size();

	if (count == 0) return;

	ArgList & pList = cop->getParmList();
	
	// the first argument is assumed to be R4.
	//
	BaseArgument *prevArg = pList[0];

	BaseArgument *currArg;
	for (int i = 1; i < count; i++)
	{
		currArg = pList[i];

		if (consecutiveArgs(prevArg, currArg) == _NO_)
		{
			patchCallForDoubleArg(cop, i);
		}
		prevArg = currArg;
	}
}

extern BaseArgument *getNextDoubleArgument(BaseArgument *);
extern void addDoubleProperties(BaseArgument *, BaseArgument *);

// SYS_CALL
/*
void patchSysCallForDoubleArgs(CallRetOp *cop)
{
	char *syscallname = cop->getTargetLabelStr();

	if (syscallname == NULL) return;

	MyString s(syscallname);

	int pos = 0;

	if (systemCalls.find(s) == systemCalls.end()) return;

	// First, check to see if this syscall has a well-defined list of parameters.
	//
	if ((systemCalls[s])->typeOfOperand(pos) == _ANY)
	{
		// this needs to be handled separately.
		// examples of such system/library calls are : printf, fprintf, etc.
		//
		patchCallForDoubleArgBasedOnArgs(cop);
		return;
	}

	int cnt = (cop->getParmList()).size();

	while (pos < cnt)
	{
		if ((systemCalls[s])->typeOfOperand(pos) == _DBL)
		{
			patchCallForDoubleArg(cop, pos);
		}
		pos++;
	}

	// now, see if the return of the call also needs to be patched.
	//
	if ((systemCalls[s])->typeOfOperand(0, _DEST_LIST_) == _DBL)
	{
		BaseArgument *nextArg = getNextDoubleArgument(cop->ptrToOperand(_DST_, _DEST_LIST_));
		cop->addOperand(nextArg, _DOUBLE_DST_BEGIN_, _DEST_LIST_);
		addDoubleProperties(cop->ptrToOperand(_DST_, _DEST_LIST_), cop->ptrToOperand(_DOUBLE_DST_BEGIN_, _DEST_LIST_));
	}
}
*/
// End SYS_CALL

void patchCallForDoubleArg(CallRetOp *cop, int pos)
{
	// go thru the argument list.
	// for the argument in the "pos" position, add the corresponding double argument (at the end).

	// Note: Call Ops do not have any operand in "_SOURCE_2_" position.
	// So, we only need to modify the parmList object.
	//
	ArgList & parmList = cop->getParmList();

	BaseArgument *darg = getNextDoubleArgument(parmList[pos]);
	BaseArgument *tempArg = darg->copy();

	parmList.push_back(darg);

	// SAH : 01/29/01
	//
	// Modified becos we no longer need to add double arg properties for args in calls.
	//
	// addDoubleProperties(parmList[pos], darg);

	// also add this arg to the rout parm property.
	//
	Routine *rptr;

	if (cop->isDynamicAddrCall()) // we do not know which routine is going to be called, so do nothing.
	{
		delete tempArg;		return;
	}

	BaseArgument *arg = cop->ptrToOperand(_SOURCE_1_);
	
	if ((arg == NULL) || (arg->isLabelArg() != _YES_))
	{
		delete tempArg;		return;
	}

	if ((rptr = curProgram->getRoutine(arg->lName())) == NULL)
	{
		delete tempArg;		return;
	}

	// now go thru the list of input parameters and add the input parms and return type info.
	//
	patchCallInputParmInfo(tempArg, rptr);
}

void patchCallForDoubleTypeParms(BaseOperation *op)
{
	// we know that this is a call op.

	CallRetOp *callOp = (CallRetOp *)op;

	if (callOp->isSpecialCall()) // _DEBUG_PRINT is classified as a special call.
		return; // Do nothing for special calls.

	// first, get the call name (e.g. call _foo)
	//
	// then, get the routine corresponding to the call name.
	//
	char *tstr = callOp->getTargetLabelStr();

	if (tstr == NULL) return;

	RoutParmProperty *parmProp;

	Routine *rptr = curProgram->getRoutine(tstr);

	if (rptr == NULL) // this might be a system call routine.
	{
// SYS_CALL
//		patchSysCallForDoubleArgs(callOp);
// End SYS_CALL
		return;
	}
	else
	{
		// get the parm property from this routine.
		//
		if ((parmProp = (RoutParmProperty *)(rptr->getPtrToProperty(_ROUTPARM_))) == NULL)
		{
			// may need to flag this as an error.
			//
			return;
		}
	}

	// first patch the call input parameters.
	//
	Vector<VALUE_TYPE>::iterator i1;
	Vector<VALUE_TYPE> & typeList = parmProp->getCallParmsTypeList();

	int cnt = 0;
	for (i1 = typeList.begin(); i1 != typeList.end(); i1++)
	{
		if ((*i1) == _DBL) // then, this requires two registers to hold the value.
		{
			patchCallForDoubleArg(callOp, cnt);
		}
		cnt++;
	}

	// next, patch the call return parameter.
	//
	if (parmProp->getReturnParmType() == _DBL)
	{
		BaseArgument *nextArg = getNextDoubleArgument(callOp->ptrToOperand(_DST_, _DEST_LIST_));
		callOp->addOperand(nextArg, _DOUBLE_DST_BEGIN_, _DEST_LIST_);
		addDoubleProperties(callOp->ptrToOperand(_DST_, _DEST_LIST_), callOp->ptrToOperand(_DOUBLE_DST_BEGIN_, _DEST_LIST_));
	}
}

void patchRetForDoubleTypeParms(BaseOperation *rop, Routine *rout)
{
	if (rop->hasOperand(_SRC_1_) == _NO_) // error condition??
		return;

	BaseArgument *nextArg = getNextDoubleArgument(rop->ptrToOperand(_SRC_1_));
	rop->addOperand(nextArg, _DOUBLE_SRC_BEGIN_);
	addDoubleProperties(rop->ptrToOperand(_SRC_1_), rop->ptrToOperand(_DOUBLE_SRC_BEGIN_));

	// now also add the double argument to the routine parm property
	//
	patchCallReturnInfo(nextArg, rout);
}

void patchForDoubleTypeParms(BaseInstruction *instr, Routine *rout)
{
	int isMainRoutine = 0;

	// First, check to see if the routine has a return parm (_ROUTPARM_) property
	//
	RoutParmProperty *parmProp;

	// check to see if rptr already has the RoutParmProperty with the callParms initialized.
	//
	if ((parmProp = (RoutParmProperty *)(rout->getPtrToProperty(_ROUTPARM_))) == NULL)
	{
		if (rout->isMainRoutine()) // then, it wont have a parmProp
		{
			isMainRoutine = 1;
		}
		else
		{
			// may need to flag this as an error.
			//
			return;
		}
	}

	int doubleizeRets = 0;
	if (!isMainRoutine)
	{
		if (parmProp->getReturnParmType() == _DBL)
		{
			doubleizeRets = 1;
		}
	}
	
	BaseOperation *curO;
	
	ArrayIterator<ControlOpSlot> *cIter;

	// Now, iterate thru all the control operations in the routine.
	
	cIter = ((NormalInstr *)instr)->getControlOpSlotIter();
	while (cIter->hasMoreElements())
	{
		if ((cIter->currentElement()).hasOper())
		{
			curO = (cIter->currentElement()).getPtrToOperation();

			if (curO->isCallOp())
			{
				patchCallForDoubleTypeParms(curO);
			}
			else if (curO->isRetOp())
			{
				if (doubleizeRets)
					patchRetForDoubleTypeParms(curO, rout);
			}
		}
		cIter->nextElement();
	}
	delete cIter;
}

void patchForDoubleTypeParms(Routine *rout)
{
	int isMainRoutine = 0;

	// First, check to see if the routine has a return parm (_ROUTPARM_) property
	//
	RoutParmProperty *parmProp;

	// check to see if rptr already has the RoutParmProperty with the callParms initialized.
	//
	if ((parmProp = (RoutParmProperty *)(rout->getPtrToProperty(_ROUTPARM_))) == NULL)
	{
		if (rout->isMainRoutine()) // then, it wont have a parmProp
		{
			isMainRoutine = 1;
		}
		else
		{
			// may need to flag this as an error.
			//
			return;
		}
	}

	int doubleizeRets = 0;
	if (!isMainRoutine)
	{
		if (parmProp->getReturnParmType() == _DBL)
		{
			doubleizeRets = 1;
		}
	}

	// go thru the routine.
	//
	MyLinkedListIterator<BaseInstruction *> *riter;
	
	BaseInstruction *curI;
	BaseOperation *curO;
	
	ArrayIterator<ControlOpSlot> *cIter;
	
	// Now, go iterate over the instructions in the routine, 
	// search for call ops, and add them to the returnParms list.
	//
	riter = rout->instrListIteratorForw();

	while (riter->hasMoreElements())
	{
		curI = riter->currentElement();

		// Now, iterate thru all the control operations in the routine.
		
		cIter = ((NormalInstr *)curI)->getControlOpSlotIter();
		while (cIter->hasMoreElements())
		{
			if ((cIter->currentElement()).hasOper())
			{
				curO = (cIter->currentElement()).getPtrToOperation();

				if (curO->isCallOp())
				{
					patchCallForDoubleTypeParms(curO);
				}
				else if (curO->isRetOp())
				{
					if (doubleizeRets)
						patchRetForDoubleTypeParms(curO, rout);
				}
			}
			cIter->nextElement();
		}
		delete cIter;
		riter->nextElement();
	}
	delete riter;
}

void patchForDoubleTypeParms(Program *prog)
{
	if (prog == NULL) return;

	// First, iterate thru each routine in the program.
	//
	MyLinkedListIterator<Routine *> *piter;
	piter = prog->routineListIteratorForw();

	Routine *curR;

	while (piter->hasMoreElements())
	{
		curR = piter->currentElement();

		// go thru each instruction/operation in the routine.
		// for each call operation, patch any double arguments.
		//
		patchForDoubleTypeParms(curR);

		piter->nextElement();
	}
	delete piter;
}

VALUE_TYPE convertStringToValueType(char *str)
{
	if (strcmp(str, "_BOOL") == 0) return _BOOL;
	if (strcmp(str, "_CHAR") == 0) return _CHAR;
	if (strcmp(str, "_UCHAR") == 0) return _UCHAR;
	if (strcmp(str, "_SHORT") == 0) return _SHORT;
	if (strcmp(str, "_USHORT") == 0) return _USHORT;
	if (strcmp(str, "_INT") == 0) return _INT;
	if (strcmp(str, "_UINT") == 0) return _UINT;
	if (strcmp(str, "_FLT") == 0) return _FLT;
	if (strcmp(str, "_DBL") == 0) return _DBL;
	if (strcmp(str, "_STAR") == 0) return _UINT;

	return _ANY;
}

void setCallParmTypes(MyString & fName)
{
	// read a line from the file.
	// the first word is the procedure name,
	// the second word is the return type.
	// the rest of the words are the call parameter types.
	//
	FilePointer fp(fName, "r");
	char *tmptpr;
	char line[81];
	
	try
	{
		while (fgets(line, 80, fp) != NULL)
		{
			tmptpr = strtok(line, " \n");
			//
			// This is the name of the procedure.
			
			Routine *curR = curProgram->getRoutine(tmptpr);
			
			if (curR == NULL) // error condition??
				continue;
			
			// get the routine parameter property.
			//
			RoutParmProperty *parmProp = (RoutParmProperty *)(curR->getPtrToProperty(_ROUTPARM_));
			
			if (parmProp == NULL) // error condition??
				continue;
			
			tmptpr = strtok(NULL, " \n");
			//
			// This is the type of the return parameter.
			//
			parmProp->addReturnParmType(convertStringToValueType(tmptpr));
						
			while ((tmptpr = strtok(NULL, " \n")) != NULL)
			{
				// Now add each call input parameter type

				parmProp->addCallParmType(convertStringToValueType(tmptpr));
			}
		}
	}
	catch (CouldNotOpenFileException e)
	{
		cout << "Panic: Could Not Open File" << endl;
		assert(0);
	}
	fp.close();
}

void addCorrectionOps(Routine *rout)
{
	// First, check to see if the routine has a return parm (_ROUTPARM_) property
	//
	RoutParmProperty *parmProp;

	// check to see if rptr already has the RoutParmProperty with the callParms initialized.
	//
	if ((parmProp = (RoutParmProperty *)(rout->getPtrToProperty(_ROUTPARM_))) == NULL)
	{
		cout << "Note: Routine: " << rout->getName() << " is never called in the program." << endl;

		// may need to flag this as an error.
		//
		// first add an empty property, and then get the pointer to it
		//
		RoutParmProperty tempProp;
		rout->addProperty(tempProp);
		parmProp = (RoutParmProperty *)(rout->getPtrToProperty(_ROUTPARM_));

		// Finally, set the status to _INITIALIZED_
		//
		parmProp->setReturnParmStatus(_INITIALIZED_);

		// also add sp.
		//
		RealRegArg sp("gsp", 0, globalRegFileList->getIndex("gsp"));
		if (sp.getUniqID() != _INVALID_)
			parmProp->addCallParm(&sp);
	}

	// Now create the dummy op and add it to the routine.
	//
	ComputeOp *dummyOp = new ComputeOp();				
	OpCode *opc = new OpCode("NOP");
	dummyOp->initOpCode(*opc);

	NormalInstr *dummyInstr = new NormalInstr();
						
	dummyInstr->initScheduleAndAdd(dummyOp, true);
	BasicBlockNode& firstBB = rout->getCFG().getFirstBB();

	NormalInstr *firstInstr = (NormalInstr*) firstBB.getFirstInstr();
	rout->insertInstrBefore(dummyInstr, firstInstr);

	JumpOp *jOp = new JumpOp(firstInstr);
	BaseOpSlot& newOpSlot = dummyInstr->getFlowOpSlot(numFlowOpsPerInstr - 1);
	((FlowOpSlot *) &newOpSlot)->addOperation(jOp);
	(dummyInstr->getRootOpSlot()).setNext(&newOpSlot);


	// update bb leader
	firstBB.initLeader(dummyInstr);

	int position = _DEST_;
	ArgList & callParms = parmProp->getCallParms();
	
	// for all call parameters
	ArgListIter it;
	for (it = callParms.begin(); it != callParms.end(); it++) 
	{

		BaseArgument *dummyArg = *it;
		dummyOp->addOperand(dummyArg, position, _DEST_LIST_);
		position++;
	}
}

void remCorrectionOps(Routine *rout)
{
	// First, check to see if the routine has a return parm (_ROUTPARM_) property
	//
	RoutParmProperty *parmProp;

	// check to see if rptr already has the RoutParmProperty with the callParms initialized.
	//
	if ((parmProp = (RoutParmProperty *)(rout->getPtrToProperty(_ROUTPARM_))) == NULL)
	{
		// may need to flag this as an error.
		//
		return;
	}

	BasicBlockNode& firstBB = rout->getCFG().getFirstBB();

	NormalInstr *correctionInstr = (NormalInstr *)(firstBB.getFirstInstr());
	NormalInstr *actFirstInstr = (NormalInstr *) correctionInstr->getDefaultTargetInstr();
		
	// update bb leader
	firstBB.initLeader(actFirstInstr);

	rout->removeInstruction(correctionInstr);
	
	delete correctionInstr;	// also delete dummyOp
}
