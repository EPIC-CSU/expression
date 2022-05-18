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
// $Id: PropertyIncludes.h,v 1.9 1998/02/27 22:54:30 xji Exp $
//
// File:  		PropertyIncludes.h
// Created:		Thu Dec 05, 96
// Last modified: 	Thu Dec 05, 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: PROPERTY

#ifndef __PROPERTYINCLUDES_H__
#define __PROPERTYINCLUDES_H__

#include "GlobalIncludes.h"
#include "STLIncludes.h"

enum PropertyNames 
{
	_SSA_, _PREDID_, _PRED_, _COMPDEF_, _LIVEDEAD_, _LIVEDEADINSTR_, 
	_UDCHAIN_, _DUCHAIN_, _MULTICHAIN_, _PART_OF_, _IBS_, 
	_CNSTPROP_, _DELETE_, _BRANCH_, _EXECNUM_, 
	_EXECPOSSIBILITY_, _REGPRESSURE_, _REGORDER_, 
	_LOADSTORE_, _CLUSTER_, _PREDICATE_, _INSTR_TYPE_, _CODELOC_,
	_CALLNAME_, _HLINFO_, _RA_, _G_TO_T_REG_MAP_,
	_ROUTPARM_, _MEMDEP_, _DOUBLEARG_, _SCHEDULEOP_, _SCHEDULEINSTR_,
	_REGCLASS_, _STACKROUT_
};

typedef Vector<PropertyNames> PropNameList;
typedef Vector<PropertyNames>::iterator PropNameListIter;

#endif
