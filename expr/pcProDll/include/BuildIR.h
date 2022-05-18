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
// File:  		BuildIR.h
// Created:		Tue Jul 06, 99
// Last modified: 	Tue Jul 06 , 99
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   

#ifndef __BUILDIR_H__
#define __BUILDIR_H__


#ifndef WIN32
#include <ctype.h>
#endif

#include "GlobalIncludes.h"

#include "BaseArgument.h"

#include "IfOp.h"
#include "JumpOp.h"
#include "GotoOp.h"
#include "CallOp.h"
#include "MemoryOp.h"
#include "ComputeOp.h"

#include "RealRegArg.h"
#include "IConstArg.h"
#include "FConstArg.h"

#include "NormalInstr.h"
#include "OpCode.h"

#include "RegisterFileList.h"

const int SIZE_OF_CHAR = 1;

const int MAX_NUM_OF_ARGS = 3;
//
// This should be made user settable (or it should be read from a file).

const int FIRST_POS = 0;
const int SECOND_POS = 1;
const int THIRD_POS = 2;

extern int numFlowOpsPerInstr;

const int _MEMORY_OP_POS_ = 3;
const int _IF_OP_POS_ = 4;

extern RegisterFileList *globalRegFileList;

extern ArgList globalParmList;

extern int addToSymbolicAddressTable(char *str);
extern void initSpecialParms(CallRetOp & callOp);
extern void addToParmList(char *arg);
extern void clearParmList();
extern ArgList & getParmList();
extern void initParms(CallRetOp & callOp);
extern int isSpecialCall(CallRetOp & callOp);
extern int isIGOTO(char *str);

#endif
