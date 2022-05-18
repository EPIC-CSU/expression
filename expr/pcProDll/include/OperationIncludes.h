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
// $Id: OperationIncludes.h,v 1.3 1998/01/22 23:59:21 pgrun Exp $
//
// File:  		OperationIncludes.h
// Created:		Thu Oct 31, 96
// Last modified: 	Thu Oct 31, 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: OPERATION

#ifndef __OPERATIONINCLUDES_H__
#define __OPERATIONINCLUDES_H__

#include "GlobalIncludes.h"
#include "MyString.h"

// #define COUNT_PHIOPS
// #define COUNT_HEADEROPS
// #define COUNT_NORMALOPS

// #define COUNT_DATAOPS
// #define COUNT_CONTROLOPS
// #define COUNT_FLOWOPS

// #define COUNT_COMPUTEOPS
// #define COUNT_MEMORYOPS
// #define COUNT_CALLRETOPS

// #define COUNT_JUMPOPS
// #define COUNT_GOTOOPS

// #define COUNT_IFOPS

const int _C_INT_ = 73;
const int _C_FLOAT_ = 405;
const int _C_DOUBLE_ = 10;
const int _C_2WORD_ = 15;
//
// BTW, these are also the driving directions to Las Vegas, baybeee!!

const int _C_SRC_ = 73;
const int _C_DST_ = 55;

class BaseArgument;

extern void printArgToStringAsC(BaseArgument *arg, MyString & s, int intOrFloatOrDouble, int srcOrDest, int num);



// For now the SRC and DEST have been defined as integers.
// Later enum



// For now the SRC and DEST have been defined as integers.
// Later enum


#endif
