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
#ifndef _SIMCALL_H_
#define _SIMCALL_H_

#include "ExpressIncludes.h"
#include "ArchSTLIncludes.h"
#include "ArchClassIncludes.h"

class SimCallObject
{
private:
	BaseInstruction *_iPtr;
	BaseOperation *_oPtr;
	
	AddrOperand _dest;

	Routine *_callerRoutine;

public:
	SimCallObject(BaseInstruction *iPtr = NULL, BaseOperation *oPtr = NULL) : _iPtr(iPtr), _oPtr(oPtr), _callerRoutine(NULL){ }
	SimCallObject(const SimCallObject & a) : _iPtr(a._iPtr), _oPtr(a._oPtr), _dest(a._dest), _callerRoutine(a._callerRoutine) { }

	BaseInstruction *ptrToInstruction() { return _iPtr; }
	void setInstruction(BaseInstruction *iPtr) { _iPtr = iPtr; }

	BaseOperation *ptrToOperation() { return _oPtr; }
	void setOperation(BaseOperation *oPtr) { _oPtr = oPtr; }

	AddrOperand & getDest() { return _dest; }
	void setDest(AddrOperand & dst) { _dest = dst; }

	Routine *getCallerRoutine() { return _callerRoutine ; }
	void setCallerRoutine(Routine *rtn) { _callerRoutine = rtn; }

	SimCallObject & operator = (const SimCallObject & a)
	{
		_iPtr = a._iPtr;
		_oPtr = a._oPtr;
		_dest = a._dest;
		_callerRoutine = a._callerRoutine;

		return (*this);
	}
};

typedef struct
{
	AddrOperand addr;
	Val val;
} SysSideEffect;


extern Vector<SimCallObject> simCallStack;
extern void simCallPop();
extern void simCallPush(SimCallObject & a);
extern SimCallObject & simCallTop();
extern bool simCallEmpty();

const int CHAR_POINTER = 333;
const int INT_IMM = 334;
const int VOID_POINTER = 335;

const int STDOUT_HACK = -9980;
const int STDERR_HACK = -9960;

#endif

