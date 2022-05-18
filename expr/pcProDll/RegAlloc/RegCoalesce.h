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
//: RegCoalesce.h
//
// File:  		RegCoalesce.h
// Created:		Thu Feb 22, 2001
// Author:		Radu Cornea
// Email:		radu@ics.uci.edu, ilp@ics.uci.edu
//
// Register coalescing (our implementation) tries to assign same physical registers
// to virtual registers used in a copy instruction. Later phases may remove the 
// redundant copy instructions.
// 

#ifndef __REGCOALESCE__
#define __REGCOALESCE__

class BaseOperation;
class BaseInstruction;
class InterferenceGraph;

void regCoalesceOper(BaseOperation *oper, InterferenceGraph& ig);
void regCoalesceInstr(BaseInstruction *instr, InterferenceGraph& ig);
void registerCoalescing(InterferenceGraph& ig);


#endif	// __REGCOALESCE__