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
// File:  		SaveRestoreRegs.cpp
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
#include "CallOp.h"
#include "NormalInstr.h"
#include "Routine.h"
#include "StackRoutProperty.h"
#include "BasicBlockNode.h"
#include "BBIterator.h"
#include "IConstArg.h"
#include "RealRegArg.h"
#include "RegArchModel.h"
#include "RAProperty.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

extern void markOpsInInstr(BaseInstruction *instr);
extern void deleteUselessIR(Routine *rout);

void markSaveInstrsAndNoteAddresses(Routine *rout, StackRoutProperty *prop);
void markReloadInstrs(Routine *rout, StackRoutProperty *prop);

BaseInstruction *getPtrToSPSubInstr(Routine *rout);
extern void noteSPSubValue(StackRoutProperty *prop, BaseInstruction *spSubInstr);
extern void noteSaveAddress(StackRoutProperty *prop, BaseInstruction *saveInstr, int firstOrLast);
BaseInstruction *getPtrToSPAddInstr(Routine *rout);
extern void noteNumSaveInstrs(StackRoutProperty *prop, int numSaves);
extern int getNumGCCSaves(StackRoutProperty *prop);
extern int getGCCSaveAddress(StackRoutProperty *prop, int firstOrLast);

int isMIPSspSubInstr(BaseInstruction *instr);
int isMIPSspAddInstr(BaseInstruction *instr);
int isMIPSspAssignInstr(BaseInstruction *instr, int dstOrSrc);

extern void listOfDefs(Routine *rout, ArgList & listofDefRegs, int uniq);
void addSaveInstrs(Routine *rout, StackRoutProperty *prop, ArgList & listofDefRegs, int sp0Or1);
void addRestoreInstrs(Routine *rout, StackRoutProperty *prop, ArgList & listofDefRegs, int sp0Or1);

BaseArgument *getTargetSPArg(int sp0Or1);
extern BaseArgument *createTempArg(BaseArgument *arg);

extern void insertInstrBetween(NormalInstr *newInstr, NormalInstr *prevInstr, NormalInstr *nextInstr);

// If we are using the GCC Express front-end, then this function can be used to remove the
// register saves and restores performed by GCC.
//
// Note: This function should be called only after the point where simulate: noSSA takes place.
// Also, this function should be called before code-motion as it assumes that the save/restore operations
// occur in a particular order. Finally, if this is called after SSA, then a lot of book-keeping functions
// have to be performed (E.g. maintaining correct SSA number of the NOP instr's SP variable.
// 
// This function, removes the operations/instructions physically from the IR and stores all the relevant information,
// such as number of variables that GCC saved/restored, the sp relative addresses that were used for the register saves, etc.
// This information is stored as a Routine property.
//
void removeGCCSaveRestores(Routine *rout)
{
	// Saves:
	// The first (non-dummy) instruction of each procedure contains a ISUB sp operation.
	// Then, the next few IVSTORE sp operations are register save operations.
	// After the IVSTORE operations is an ASSIGN fp sp operation
	// 
	// Restores:
	// Restore ops are in reverse order of saves.
	// The last instruction of each procedure contains the IGOTO operation (RETURN).
	// The instruction before is an IADD sp operation.
	// The instructions before that are the IVLOAD sp operations
	// Preceeding those operations is the ASSIGN sp fp operation.

	// Remove the ISUB, IVSTORE, ASSIGN, IADD, IVLOAD operations mentioned above.

	// Create the Routine Stack Property.
	//
	StackRoutProperty *prop;
	if ((prop = (StackRoutProperty *)(rout->getPtrToProperty(_STACKROUT_))) == NULL)
	{
		StackRoutProperty tempProp;
		rout->addProperty(tempProp);
		prop = (StackRoutProperty *)(rout->getPtrToProperty(_STACKROUT_));
	}

	// Next, remove the SAVE operations.
	//
	BaseInstruction *spSubInstr = getPtrToSPSubInstr(rout);
	noteSPSubValue(prop, spSubInstr);

	// Note: SAH on 03/25/02
	//
	// As of now, we do not delete the sp sub and add instrs, because these have to be used as they are later on.
	//
	// i.e. we do maintain the original values of sp, becos that is used to access stack based variables (such as local arrays, etc).
	//
	// Currently, if we need to extend the stack size (becos our reg alloc allocated more regs than gcc did), then we use 
	// a companion stack (accessed using sp1).

//	markOpsInInstr(spSubInstr); // So that we can later DCE it.

	markSaveInstrsAndNoteAddresses(rout, prop);

	// Finally, remove the RESTORE operations.
	//
	BaseInstruction *spAddInstr = getPtrToSPAddInstr(rout);

//	markOpsInInstr(spAddInstr); // So that we can later DCE it.

	markReloadInstrs(rout, prop);

	// finally call DCE on the routine.
	//
	deleteUselessIR(rout);
}

// Use this function after RegAlloc, to save the registers at the beginning and to restore the registers at the end of each procedure.
//
// Note: In order to be "optimal" in terms of number of saves/restores needed,
// we need to do the following things:
// Inter-procedural effects aware RA,
// and save/restore only those regs that are live across any call of this routine.
//
// One thing to do would be for RA, to have a set of arguments designated as "local" registers
// based on the front-end knowledge, and allocate them to a set of registers that will never 
// be saved/restored.
//
// However, we currently follow the naive approach of saving all registers that are defined in this routine.
//
void addSavesRestores(Routine *rout)
{
	// Usually, if the routine is a main routine,
	// we need to save/restore fp, return address register.
	//
	// However, we don't do this as of now.
	//
	if (rout->isMainRoutine()) // don't do anything.
		return;
	
	// Get the list of registers defined in this routine.
	//
	ArgList listOfDefRegs;
	listOfDefs(rout, listOfDefRegs, 1); // 1 -> get only the list of unique defs

	StackRoutProperty *prop;
	if ((prop = (StackRoutProperty *)(rout->getPtrToProperty(_STACKROUT_))) == NULL)
		return;

	int numPrevSaves = getNumGCCSaves(prop);
	int numCurrSaves = listOfDefRegs.size();

	if (numCurrSaves <= numPrevSaves) // can easily add the saves and restores to the current routine.
	{
		// Add the save instructions.
		//
		addSaveInstrs(rout, prop, listOfDefRegs, 0); // use sp0

		// Add the restore instructions.
		//
		addRestoreInstrs(rout, prop, listOfDefRegs, 0); // use sp0
	}
	else
	{
		// Need to add saves/restores using sp1 and also the sub, add sp1 instrs.
		// TBD.
		assert(0);
	}
}

void markSaveInstrsAndNoteAddresses(Routine *rout, StackRoutProperty *prop)
{
	BaseInstruction *instr;
	int numSaves = 0;

	// Go thru the first BB in searching for the first save instruction.
	//
	BBForwIterator iter(&((rout->getCFG()).getFirstBB()));
	while (iter.hasMoreElements())
	{
		instr = iter.currentElement();

		if (isMIPSspSubInstr(instr)) break;

		if (isMIPSspAssignInstr(instr, 0)) // we have reach the end of the block wherein we could have found the SPSub instr
		{
			cout << "Could not find SP initialization instruction in Routine : ";
			rout->printName(cout);
			cout << endl;

			return;
		}
		iter.nextElement();
	}

	iter.nextElement();
	instr = iter.currentElement();

	// instr now contains the first save instr.
	//
	noteSaveAddress(prop, instr, 0); // 0 -> is the first save address

	BaseInstruction *lastInstr = instr;
	int foundLastSave = 0;
	while (iter.hasMoreElements())
	{
		instr = iter.currentElement();

		if (isMIPSspAssignInstr(instr, 0)) // we have reach the end of the block of save instrs
		{
			foundLastSave = 1;
			break;
		}

		numSaves++;
		markOpsInInstr(instr);

		iter.nextElement();
		lastInstr = instr;
	}

	if (foundLastSave)
	{
		noteSaveAddress(prop, lastInstr, 1); // 1 -> is the last save address
		noteNumSaveInstrs(prop, numSaves);
	}
}

void markReloadInstrs(Routine *rout, StackRoutProperty *prop)
{
	BaseInstruction *instr;

	// Go thru the last BB in searching for the first save instruction.
	//
	BBBackIterator iter(&((rout->getCFG()).getLastBB()));
	while (iter.hasMoreElements())
	{
		instr = iter.currentElement();

		if (isMIPSspAddInstr(instr)) break;

		if (isMIPSspAssignInstr(instr, 1)) // we have reach the end of the block wherein we could have found the SPSub instr
		{
			cout << "Could not find SP initialization instruction in Routine : ";
			rout->printName(cout);
			cout << endl;

			return;
		}
		iter.prevElement();
	}

	iter.prevElement();
	instr = iter.currentElement();

	// instr now contains the last reload instr.
	//
	while (iter.hasMoreElements())
	{
		instr = iter.currentElement();

		if (isMIPSspAssignInstr(instr, 1)) // we have reach the end of the block of save instrs
			break;

		markOpsInInstr(instr);

		iter.prevElement();
	}
}


extern int isOpCode(int opCode, char *str);
extern int isSPArg(BaseArgument *arg);

int isMIPSspSubInstr(BaseInstruction *instr)
{
	BaseOperation *op = ((NormalInstr *)instr)->getPtrToRootOperation();

	if (!isOpCode((op->getOpCode()).index(), "ISUB")) return _NO_;

	// Now, check to see if the Destination is SP.
	//
	if (!isSPArg(op->ptrToOperand(_DEST_, _DEST_LIST_)))
		return _NO_;

	return _YES_;
}

int isMIPSspAddInstr(BaseInstruction *instr)
{
	BaseOperation *op = ((NormalInstr *)instr)->getPtrToRootOperation();

	if (!isOpCode((op->getOpCode()).index(), "IADD")) return _NO_;

	// Now, check to see if the Destination is SP.
	if (!isSPArg(op->ptrToOperand(_DEST_, _DEST_LIST_)))
		return _NO_;

	return _YES_;
}

int isMIPSspAssignInstr(BaseInstruction *instr, int srcOrDst)
{
	BaseOperation *op = ((NormalInstr *)instr)->getPtrToRootOperation();

	if (!isOpCode((op->getOpCode()).index(), "IASSIGN")) return _NO_;

	// Now, check to see if the Destination/Source is SP.
	//
	if (srcOrDst == 0) // source
	{
		if (!isSPArg(op->ptrToOperand(_SRC_1_, _SRC_LIST_)))
			return _NO_;
	}
	else
	{
		if (!isSPArg(op->ptrToOperand(_DST_, _DEST_LIST_)))
			return _NO_;
	}

	return _YES_;
}

BaseInstruction *getPtrToSPSubInstr(Routine *rout)
{
	BaseInstruction *instr;

	// Go thru the first BB in searching for the instruction.
	//
	BBForwIterator iter(&((rout->getCFG()).getFirstBB()));
	while (iter.hasMoreElements())
	{
		instr = iter.currentElement();

		if (isMIPSspSubInstr(instr)) return instr;

		if (isMIPSspAssignInstr(instr, 0)) // we have reach the end of the block wherein we could have found the SPSub instr
		{
			cout << "Could not find SP addition instruction in Routine : ";
			rout->printName(cout);
			cout << endl;

			return NULL;
		}
		iter.nextElement();
	}
	return instr;
}

BaseInstruction *getPtrToSPAddInstr(Routine *rout)
{
	BaseInstruction *instr;

	// Go thru the last BB in searching for the instruction.
	//
	BBBackIterator iter(&((rout->getCFG()).getLastBB()));
	while (iter.hasMoreElements())
	{
		instr = iter.currentElement();

		if (isMIPSspAddInstr(instr)) return instr;

		if (isMIPSspAssignInstr(instr, 1)) // we have reach the end of the block wherein we could have found the SPSub instr
		{
			cout << "Could not find SP addition instruction in Routine : ";
			rout->printName(cout);
			cout << endl;

			return NULL;
		}
		iter.prevElement();
	}
	return instr;
}

extern BaseOperation *createStoreOp(BaseArgument *storeSource, BaseArgument *indexArg, BaseArgument *offset, BaseArgument *sibArg);
extern BaseOperation *createLoadOp(BaseArgument *loadDest, BaseArgument *indexArg, BaseArgument *offset, BaseArgument *sibArg);

void addSaveInstrs(Routine *rout, StackRoutProperty *prop, ArgList & listOfDefRegs, int sp0Or1)
{
	// for each def add a store instruction, starting with the saveStartAddress.
	ArgListIter ai;

	BaseArgument *indexArg = getTargetSPArg(sp0Or1);

	BaseInstruction *instr;

	// Go thru the first BB in searching for the instruction.
	//
	BasicBlockNode *saveBB = &((rout->getCFG()).getFirstBB());
	BBForwIterator iter(saveBB);
	while (iter.hasMoreElements())
	{
		instr = iter.currentElement();

		if (isMIPSspSubInstr(instr))
		{
			iter.nextElement();
			break;
		}
		iter.nextElement();
	}

	BaseInstruction *nextInstr = iter.currentElement();
	//
	// Now, instr points to the place where we can start adding SAVE instrs, and nextInstrs marks the instruction that
	// immediately follows the block of save instrs.

	int currOffset = getGCCSaveAddress(prop, 0); // get the starting address 
	for (ai = listOfDefRegs.begin(); ai != listOfDefRegs.end(); ai++, currOffset+=4)
	{
		IConstArg offset(currOffset);
		BaseOperation *op = createStoreOp(*ai, indexArg, &offset, NULL);

		NormalInstr *saveInstr = new NormalInstr();
		saveInstr->initScheduleAndAdd(op, true);
		saveInstr->setID(0);

		insertInstrBetween(saveInstr, (NormalInstr *)instr, (NormalInstr *)nextInstr);
		instr = saveInstr;
	}
	delete indexArg;
}

void addRestoreInstrs(Routine *rout, StackRoutProperty *prop, ArgList & listOfDefRegs, int sp0Or1)
{
	// for each def add a load instruction, starting with the saveStartAddress.
	ArgListIter ai;

	BaseArgument *indexArg = getTargetSPArg(sp0Or1);

	BaseInstruction *instr;

	// Go thru the last BB in searching for the instruction.
	//
	BasicBlockNode *saveBB = &((rout->getCFG()).getLastBB());
	BBBackIterator iter(saveBB);
	while (iter.hasMoreElements())
	{
		instr = iter.currentElement();

		if (isMIPSspAddInstr(instr))
		{
			iter.prevElement();
			break;
		}
		iter.prevElement();
	}

	BaseInstruction *prevInstr = iter.currentElement();
	//
	// Now, prevInstr points to the place where we can start adding RESTORE instrs, and instr marks the instruction that
	// immediately follows the block of restore instrs.

	int currOffset = getGCCSaveAddress(prop, 0); // get the starting address 
	for (ai = listOfDefRegs.begin(); ai != listOfDefRegs.end(); ai++, currOffset+=4)
	{
		IConstArg offset(currOffset);
		BaseOperation *op = createLoadOp(*ai, indexArg, &offset, NULL);

		NormalInstr *restoreInstr = new NormalInstr();
		restoreInstr->initScheduleAndAdd(op, true);
		restoreInstr->setID(0);

		insertInstrBetween(restoreInstr, (NormalInstr *)prevInstr, (NormalInstr *)instr);
		instr = restoreInstr;
	}
	delete indexArg;
}

extern RegArchModel globalRegModel;
extern void setSPTypeInRegClassEntryNode2(RegClassEntryNode2 & rcen);
extern int convertRegNumToRegIndex(int rfName, int regNum);

// Note: We assume that Reg Alloc has been done.
// This implies that the argument that is created will be a temp arg with RA property.
// This RA property will indicate that the register is sp.
//
BaseArgument *getTargetSPArg(int sp0Or1) // if sp0Or1 = 0, then we need to get the default target SP register arg.
{
	if (!doneRA)
	{
		cout << "Panic: Tried to getTargtSPArg before Reg Alloc" << endl;
		assert(0);
	}

	// First, get the target equivalent of the generic sp.
	//
	RegClassEntryNode2 rcen;
	Vector<RegNumList> rList;
	globalRegModel.getAllRegsToMapping(_GENERIC_, rcen, _TARGET_, rList);

	// Now, rList contains the list of all regs that can be mapped to sp.
	//
	// if sp0Or1 == 0, then use the first element in rList.

	BaseArgument *retPtr;
	retPtr = createTempArg(NULL);
	int spIndex;

	if (sp0Or1 == 0)
	{
		int rfName = (rList.front())._rfName;
		int regNum = ((rList.front())._rList).front();

		spIndex = convertRegNumToRegIndex(rfName, regNum);
	}
	else
	{
		// TBD.
		assert(0);
	}

	// create RA property with spIndex.
	//
	RAProperty rap(spIndex);
	retPtr->addProperty(rap);

	return retPtr;
}