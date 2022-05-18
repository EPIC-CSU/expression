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
// $Id: GlobalIncludes.h,v 1.8 1998/03/27 00:15:05 ahalambi Exp $
//
// File:  		GlobalIncludes.h
// Created:		Mon Oct 28, 96
// Last modified: 	Mon Oct 28, 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: SUPPORT DS

//
// This file defines some useful (and very much used) values.
// It is intended that this file be included by almost all other
// files. 
// 10/25/96 : This file contains some preliminary #defines, and
// #includes. This file should be modified (only if necessary) to add
// something that is going to be useful to all other modules.
//

#ifndef __GLOBALINCLUDES_H__
#define __GLOBALINCLUDES_H__

#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include <iostream.h>
#include <assert.h>

#ifndef WIN32
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stl.h>
#endif

const int _TRUE_ = 1;
const int _FALSE_ = 0;

const int _YES_ = 1;
const int _NO_ = 0;

const int _INVALID_ = -1;

//
// All other functions/methods should use the above definitions to
// perform comparisions. Eg.
// if (someObject.hasData() == _YES_) .....
//

#define _NULL_ 0
//
// Use _NULL_ to test whether a pointer is a null pointer (or to
// assign a pointer to NULL). 
// i.e. use "if (ptr == _NULL_) ....."
// instead of "if (ptr == NULL) ...."
// This is because ANSI C defines NULL as either (void*)0 or 0.
// Now, if NULL is defined as (void*)0, then type conversions will
// have to be made, when assigning a pointer to NULL.
//

typedef int IDType;
//
// 10/28/96 :
// Most likely, IDType will become a class definition later. This
// representation has several shortcomings:
// No functions which can manipulate IDs cleverly,
// Also, this representation is nothing but a counter. It is not
// possible to give the same ID to objects of different classes (like
// instructions, operations).

// #define FULL_DIAGNOSTICS

#endif





