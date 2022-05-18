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
// File:  		OpCodeFileDS.h
// Created:		Wed, Oct 25, 00
// Last modified: 	Wed, Oct 25, 00
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: ARCHITECTURE, include

#ifndef __OPCODEFILEDS_H__
#define __OPCODEFILEDS_H__

#include "GlobalIncludes.h"
#include "ArchitectureIncludes.h"
#include "STLIncludes.h"
#include "MyString.h"
#include "BaseOperation.h"

typedef int OperandNum;
typedef int OperandType;

typedef map<OperandNum, OperandType> OperandTypeList;

typedef struct
{
	OperandTypeList dstList;
	OperandTypeList srcList;
} DstAndSrcDataTypes;

typedef int OpcodeIndex;

typedef map<OpcodeIndex, DstAndSrcDataTypes> OperandTypesForOperations;

extern OperandTypesForOperations globalOperandTypesForOperations;

// This is the Name to Define table
typedef map<MyString, int> Name2DefineList;
typedef Name2DefineList::iterator N2DIter;

extern int getDataTypeForArgInOper(int, int, int);

extern void initName2DefineList();
extern void readOpCodeFile(MyString & s);

#endif
