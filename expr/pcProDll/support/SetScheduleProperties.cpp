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
// File:  		SetScheduleProperties.cpp
// Created:		Mon Nov 1, 99
// Last modified: 	Mon Nov 1, 99
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: SUPPORT

#include "stdafx.h"

#include "ScheduleOpProperty.h"
#include "Routine.h"
#include "BaseOperation.h"
#include "Program.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

const int _CC_BARRIER_ = 560;
const int _FP_BARRIER_ = 670;
const int _SP_BARRIER_ = 780;
const int _HILO_BARRIER_ = 890;

void setScheduleOpProperty(BaseOperation *oper, int flg, int code)
{
	if (oper == NULL) return;

	ScheduleOpProperty *prop = (ScheduleOpProperty *)(oper->getPtrToProperty(_SCHEDULEOP_));

	if (prop == NULL)
	{
		ScheduleOpProperty sop(flg, code);
		oper->addProperty(sop);
	}
	else // a property already exists. replace it with this one.
	{
		prop->setCode(code);
		prop->setScheduleFlag(flg);
	}
}

int getScheduleOpPropertyCode(BaseOperation *oper, int flg)
{
	if (oper == NULL) return _INVALID_;

	ScheduleOpProperty *prop = (ScheduleOpProperty *)(oper->getPtrToProperty(_SCHEDULEOP_));

	if (prop != NULL)
		return (prop->getCode());

	return _INVALID_;
}

int hasScheduleOpProperty(BaseOperation *oper, int flg)
{
	if (oper == NULL) return _NO_;

	ScheduleOpProperty *prop = (ScheduleOpProperty *)(oper->getPtrToProperty(_SCHEDULEOP_));

	if (prop != NULL)
	{
		if (prop->getScheduleFlag() == flg) 
			return _YES_;
	}
	return _NO_;
}

void setScheduleInstrProperty(BaseInstruction *instr, int flg, int code)
{
	ScheduleInstrProperty sop(flg, code);
	instr->addProperty(sop);
}

int getScheduleInstrPropertyCode(BaseInstruction *instr, int flg)
{
	if (instr == NULL) return _INVALID_;

	ScheduleInstrProperty *prop = (ScheduleInstrProperty *)(instr->getPtrToProperty(_SCHEDULEINSTR_));

	if (prop != NULL)
		return (prop->getCode());

	return _INVALID_;
}

int hasScheduleInstrProperty(BaseInstruction *instr, int flg)
{
	if (instr == NULL) return _NO_;

	ScheduleInstrProperty *prop = (ScheduleInstrProperty *)(instr->getPtrToProperty(_SCHEDULEINSTR_));

	if (prop != NULL)
	{
		if (prop->getScheduleFlag() == flg) 
			return _YES_;
	}
	return _NO_;
}

extern int isGenericArchDefinedArg(BaseArgument *arg);
extern int isCCArg(BaseArgument *arg);
extern int isFPArg(BaseArgument *arg);
extern int isSPArg(BaseArgument *arg);

extern int isHILOArg(BaseArgument *arg);

void setNoCodeMotion(BaseOperation *oper, int code = _DEFAULT_ALL_)
{
	if (oper->isCallRetOp())
	{
		setScheduleOpProperty(oper, _NOCODEMOTION_, code);
		return;
	}

	// Go thru the destination list: if it has 
	// R0, R4, R5, R6, R7, R8, R9, R10, sp0 or f0 then set the property
	//
	// Also, set No code motion for cc0
	//
	// Added by SAH on 04/27/01
	//
	// For MIPS based m/cs hilo too is a restricted resource. So, do not 
	// move ops that define hilo registers.

	ArgList & dList = oper->destOperandList();
	ArgListIter i2 = dList.end();

	for (ArgListIter i1 = dList.begin(); i1 != i2; i1++)
	{
		if (*i1)
		{
/*
			if (isGenericArchDefinedArg(*i1))
			{
				setScheduleOpProperty(oper, _NOCODEMOTION_);
				break;
			}
*/
/*
			if (isFPArg(*i1))
			{
				setScheduleOpProperty(oper, _NOCODEMOTION_, _FP_BARRIER_);
				break;
			}
*/
/*
			if (isSPArg(*i1))
			{
				setScheduleOpProperty(oper, _NOCODEMOTION_, code);
				break;
			}
*/
/*
			if (isCCArg(*i1))
			{
				setScheduleOpProperty(oper, _NOCODEMOTION_, _CC_BARRIER_);
				break;
			}
*/
/*
			if (isHILOArg(*i1))
			{
				setScheduleOpProperty(oper, _NOCODEMOTION_, _HILO_BARRIER_);
				break;
			}
*/
		}
	}
}

void setBarrier(BaseOperation *oper, int code = _DEFAULT_ALL_)
{
	// Go thru the destination list: if it has 
	// cc0 then set the property
	ArgList & dList = oper->destOperandList();
	ArgListIter i2 = dList.end();

	for (ArgListIter i1 = dList.begin(); i1 != i2; i1++)
	{
		if (*i1)
		{
/*
			if (isCCArg(*i1))
			{
				setScheduleOpProperty(oper, _BARRIER_, _CC_BARRIER_);
				break;
			}
			if (isFPArg(*i1))
			{
				setScheduleOpProperty(oper, _BARRIER_, _FP_BARRIER_);
			}
			if (isSPArg(*i1))
			{
				setScheduleOpProperty(oper, _BARRIER_, _SP_BARRIER_);
			}
			if (isHILOArg(*i1))
			{
				setScheduleOpProperty(oper, _BARRIER_, _SP_BARRIER_);
			}
*/
		}
	}
}

void setNoCodeMotion(BaseInstruction *instr, int code = _DEFAULT_ALL_)
{
	if (instr != NULL)
		setScheduleInstrProperty(instr, _NOCODEMOTION_, code);
}

void setBarrier(BaseInstruction *instr, int code = _DEFAULT_ALL_)
{
	if (instr != NULL)
		setScheduleInstrProperty(instr, _BARRIER_, code);
}

void setNoCodeMotionOps(BaseInstruction *instr)
{
	// see setNoCodeMotionOps(Routine *rout) for list of ops that are not to be moved during scheduling

	if (instr == NULL) return;

	ArrayIterator<DataOpSlot> *diter = ((NormalInstr *)instr)->getDataOpSlotIter();
	while (diter->hasMoreElements())
	{
		if ((diter->currentElement()).hasOper())
			setNoCodeMotion((diter->currentElement()).getPtrToOperation());
		diter->nextElement();
	}
	delete diter;

	ArrayIterator<ControlOpSlot> *citer = ((NormalInstr *)instr)->getControlOpSlotIter();
	while (citer->hasMoreElements())
	{
		if ((citer->currentElement()).hasOper())
			setNoCodeMotion((citer->currentElement()).getPtrToOperation());
		citer->nextElement();
	}
	delete citer;
}

void setNoCodeMotionOps(Routine *rout)
{
	// iterate over the instr list.
	//
	// currently, we set all ops that define the call parms
	// such as R0, R4, R5, R6, R7, R8, R9, R10, sp0 or f0 to NoCodeMotion.
	// (Note that this includes calls)
	// Also set return ops to NoCodeMotion.

	MyLinkedListIterator<BaseInstruction *> *iter = rout->instrListIteratorForw();

	while (iter->hasMoreElements())
	{
		setNoCodeMotionOps(iter->currentElement());
		iter->nextElement();
	}
	delete iter;
}

void setNoCodeMotionInstrs(Routine *rout)
{
	// The first instr is assumed to be a dummy instr that only contains a NOP.
	// This first instr will be removed in a subsequent pass.
	// So, mark this instr as not to be scheduled into.
	//
	//
	BaseInstruction *firstInstr = ((rout->getCFG()).getFirstBB()).getFirstInstr();
	setNoCodeMotion(firstInstr);
}

void setBarrierOps(BaseInstruction *instr)
{
	// see setNoCodeMotionOps(Routine *rout) for list of ops that are not to be moved during scheduling

	if (instr == NULL) return;

	ArrayIterator<DataOpSlot> *diter = ((NormalInstr *)instr)->getDataOpSlotIter();
	while (diter->hasMoreElements())
	{
		if ((diter->currentElement()).hasOper())
			setBarrier((diter->currentElement()).getPtrToOperation());
		diter->nextElement();
	}
	delete diter;

	ArrayIterator<ControlOpSlot> *citer = ((NormalInstr *)instr)->getControlOpSlotIter();
	while (citer->hasMoreElements())
	{
		if ((citer->currentElement()).hasOper())
			setBarrier((citer->currentElement()).getPtrToOperation());
		citer->nextElement();
	}
	delete citer;
}

void setBarrierOps(Routine *rout)
{
	// Barrier ops do not allow ops with the same barrier flag to pass them (or be in parallel with them).
	//
	// Instrs/Ops that use cc act as barriers to other ops that define cc.
	//
	MyLinkedListIterator<BaseInstruction *> *iter = rout->instrListIteratorForw();

	while (iter->hasMoreElements())
	{
		setBarrierOps(iter->currentElement());
		iter->nextElement();
	}
	delete iter;
}

void setBarrierInstrs(Routine *rout)
{
	// All CALL and RET ops are barrier ops.
	// As of now, We cannot set barriers on ops, but on the instrs that contain the ops.
	// So, we set instrs containing calls and rets as barrier instrs
	//
	//
	MyLinkedListIterator<BaseInstruction *> *iter = rout->instrListIteratorForw();

	NormalInstr *currInstr;
	while (iter->hasMoreElements())
	{
		currInstr = (NormalInstr *)(iter->currentElement());
		// An if instr serves as a barrier to all ops that define cc0.
		//
		if (currInstr->hasIfOp())
			setBarrier(currInstr, _CC_BARRIER_);

		// This overides the previous barrier (in the case that an instr has both if and call ops).
		//
		if (currInstr->hasCallRetOp())
			setBarrier(currInstr);

		iter->nextElement();
	}
	delete iter;
}

// This function is present in rISASchedulerSupport.cpp
//
extern void setRISABarriers(Routine *rout);

void setScheduleProperties(Routine *rout)
{
	// First set all the barrier ops.
	//
	setBarrierOps(rout);

	// Then, set all no code motion ops.
	//
	setNoCodeMotionOps(rout);

	// Next, set all the barrier instructions
	// These are typically CALL and RETURN instrs.
	//
	setBarrierInstrs(rout);

	// Then, set all no code motion instrs
	//
	setNoCodeMotionInstrs(rout);

	// For rISA code, set rISA barriers.
	//
	// A rISA barrier is like a CALL/RET barrier. 
	// It does not allow any operation below it to move above it (or vice-versa).
	// So, all we have to do is set the barrier on the first and last instr of the rISA block.
	//
	setRISABarriers(rout);
}

extern Program *curProgram;

void setScheduleProperties()
{
	// iterate over routine list
	//
	MyLinkedListIterator<Routine *> *iter = curProgram->routineListIteratorForw();

	while (iter->hasMoreElements())
	{
		setScheduleProperties(iter->currentElement());
		iter->nextElement();
	}
	delete iter;
}
