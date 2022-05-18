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
// File:  		BuildIRSupportFuncs.cc
// Created:		
// Last modified: 
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: INCLUDE

#include "stdafx.h"

#include "GlobalIncludes.h"
#include "BaseArgument.h"
#include "CallOp.h"
#include "expressParser.h"
#include "BuildIR.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

ArgList globalParmList;
extern BaseArgument *buildArg(char *argStr);
extern BaseArgument *buildArg(EXPRESS_ARG_TYPE aT, int num, BaseArgProperty *asrt);
extern BaseArgument *buildArg(EXPRESS_ARG_TYPE aT, char *st, BaseArgProperty *asrt);

int isIGOTO(char *str)
{
	if (strcmp(str, "IGOTO") == 0) return _YES_;
	return _NO_;
}

int addToSymbolicAddressTable(char *str)
{
	return 0;
}

void clearParmList()
{
	globalParmList.clear();
}

void initSpecialParms(CallRetOp & callOp)
{
	BaseArgument *arg;
	if ((arg = globalParmList[0]) == NULL)
	{
		// error condition.
		// the first argument cannot be null;
		//
		cout << "Panic: A Special Call was called without a parameter" << endl;
		assert(0);
	}
	callOp.addOperand(arg, _SOURCE_2_);
	callOp.initParmList(globalParmList);
	clearParmList();
}

void addToParmList(char *arg)
{
	BaseArgument *parmPtr = buildArg(arg);
	globalParmList.push_back(parmPtr);
}

void addToParmList(EXPRESS_ARG_TYPE aT, char *st)
{
   BaseArgument *parmPtr = NULL;

   if (strcmp(st, "$9") != NULL)
   {
	   int tempDebug = 1;
   }

	if (aT != _EXPRESS_REG_STRING_)
	{
		// error condition
		//
		cout << "Warning: addToParmList recieved incompatible type with integer arg" << endl;
	}
	parmPtr = buildArg(aT, st, NULL);

	globalParmList.push_back(parmPtr);
}

void addToParmList(EXPRESS_ARG_TYPE aT, int num)
{
	BaseArgument *parmPtr = NULL;
	
	if (aT != _EXPRESS_REG_INT_)
	{
		// error condition
		//
		cout << "Warning: addToParmList recieved incompatible type with integer arg" << endl;
	}

	if (num == 9)
	{
		int tempDebug = 1;
	}

	parmPtr = buildArg(aT, num, NULL);
	
	globalParmList.push_back(parmPtr);
}

ArgList & getParmList()
{
	return globalParmList;
}

void initParms(CallRetOp & callOp)
{
	callOp.initParmList(globalParmList);
	clearParmList();
}

inline int isSpecialCall(CallRetOp & callOp, int callType)
{
	if (callOp.isSpecialCall(callType)) return _YES_;
	return _NO_;
}

int isDebugPrintCall(CallRetOp & callOp)
{
	return (isSpecialCall(callOp, CALL_DEBUG_PRINT_));
}

int isSpecialCall(CallRetOp & callOp)
{
	if (isSpecialCall(callOp, CALL_DEBUG_PRINT_)) return _YES_;
	// Radu - May 21, 2000
	//if (isSpecialCall(callOp, CALL_SQRT_)) return _YES_;
	//if (isSpecialCall(callOp, CALL_EXP_)) return _YES_;
	
	return _NO_;
}