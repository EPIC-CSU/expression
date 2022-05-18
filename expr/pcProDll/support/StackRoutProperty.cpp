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
/*************************************************************
 *file  : StackRoutProperty.cpp                              *
 *authors : Ashok Halambi                                    *
 *last modified :                                            *
 *                                                           *
 *contents: Functions for setting/getting CallStackInfo      *
 *comments:-                                                 *
 *compiled with : MSDEV                                      *
 *for more info :                                            *
 *************************************************************/

#include "stdafx.h"

#include "StackRoutProperty.h"
#include "BaseOperation.h"
#include "NormalInstr.h"
#include "Routine.h"
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

BaseRoutineProperty *StackRoutProperty::copy() const
{
 BaseRoutineProperty* prop;
 prop = new StackRoutProperty(*this);
 return prop;
}

	
void StackRoutProperty::print(ostream& out)
{
	out << "Stack parameters for the Routine" << endl;
}

// calculates the number of registers that can be saved
// based on the start and end addresses. assuming that each register is of size 4 bytes.
//
int StackRoutProperty::getNumSavesFromAddresses(int whichStack)
{
	CallStackInfo & st = GET_STACK(whichStack);

	return ((st._saveRegsEndAddr - st._saveRegsStartAddr)/4 + 1);
}


void noteSPSubValue(StackRoutProperty *prop, BaseOperation *spSubOp)
{
	// get the source 2 of the op. If it is of integer immediate type,
	// store the immediate value in _gccStackInfo.
	//
	BaseArgument *arg;
	if ((arg = spSubOp->ptrToOperand(_SRC_2_)) != NULL)
	{
		if (arg->isIConstArg())
			prop->setStackSize(((IConstArg *)arg)->value(), _GCC_STACK_);
	}
}

// Note: Assume that the proper checking has been done, and that
// the instr really contains the sp sub operation.
//
void noteSPSubValue(StackRoutProperty *prop, BaseInstruction *instr)
{
	BaseOperation *oper = instr->getPtrToRootOperation();

	noteSPSubValue(prop, oper);
}

// if firstOrLast == 0 -> save as first
// else save as last.
//
void noteSaveAddress(StackRoutProperty *prop, BaseOperation *saveOp, int firstOrLast)
{
	// from the saveop, get the offset immediate.
	//
	if ((saveOp == NULL) || (!saveOp->isMemoryOp())) return;

	BaseArgument *arg;
	if ((arg = saveOp->ptrToOperand(_OFFSET_)) != NULL)
	{
		if (!arg->isIConstArg()) return;

		if (firstOrLast == 0)
			prop->setSaveRegsStartAddr(((IConstArg *)arg)->value(), _GCC_STACK_);
		else
			prop->setSaveRegsEndAddr(((IConstArg *)arg)->value(), _GCC_STACK_);
	}
}

// Note: Assume that the proper checking has been done, and that
// the instr really contains the save operation.
//
void noteSaveAddress(StackRoutProperty *prop, BaseInstruction *instr, int firstOrLast)
{
	BaseOperation *oper = instr->getPtrToRootOperation();

	if (!oper->isMemoryOp()) return;

	if (!((MemoryOp *)oper)->isStore()) return;

	noteSaveAddress(prop, oper, firstOrLast);
}

void noteNumSaveInstrs(StackRoutProperty *prop, int numSaves)
{
	prop->setNumSaveInstrs(numSaves, _GCC_STACK_);
}

int getNumGCCSaves(StackRoutProperty *prop)
{
	return (prop->getNumSavesFromAddresses(_GCC_STACK_));
}

int getGCCSaveAddress(StackRoutProperty *prop, int firstOrLast)
{
	if (firstOrLast == 0)
		return prop->getSaveRegsStartAddr(_GCC_STACK_);
	else
		return prop->getSaveRegsEndAddr(_GCC_STACK_);
}
