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
// File:  		Optimizations.h
// Created:		Fri Jul 27, 01
// Last modified: 	Fri Jul 27, 01
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: SUPPORT

#ifndef __OPTIMIZATIONS_H__
#define __OPTIMIZATIONS_H__

#include "stdafx.h"

#include "BaseArgument.h"
#include "BaseOperation.h"
#include "BaseInstruction.h"
#include "NormalInstr.h"
#include "BasicBlockNode.h"
#include "Routine.h"
#include "Program.h"
#include "DUProperty.h"
#include "UDProperty.h"
#include "SymbolTable.h"
#include "NewInstrSel.h"

// Speficic to Opt
extern Vector<ISelIROpNode> listOfOptIROps;

// Speficic to Opt
extern Vector<ISelIROpNode>::iterator findOperFromListOfOptIROps(BaseOperation *op);

// Speficic to Opt
extern void addOpsToOptList(BaseInstruction *instr);

// Speficic to Opt
extern void printOptList();

extern BaseArgument *getArgForIdent(int ident);

extern void addArgToIdentList(int ident, BaseArgument *arg, BaseOperation *op);

extern void clearIdentToArgMap();

extern int getIndex(int num, int destOrSrc);

extern int isPresentInISelList(BaseOperation *op);

// Speficic to Opt
extern void setAsDoneOpt(BaseOperation *op);
// Speficic to Opt
extern void setAsDoneOpt(ISelIROpNode & op);
// Speficic to Opt
extern void setAsDoneOpt(OpList & oList);

// Speficic to Opt
extern int doneOpt(BaseOperation *op);

// Speficic to Opt
extern void resetOptFlg();

extern int buildListOfOpsFromDepChains(BaseOperation *op, OpList & treeList, int size);
extern int buildListOfOptOpsSequential(BaseOperation *op, OpList & treeList, int size, int dir = 1); 
//
// dir == 1 : go down the list; dir == 0 : go up the list

extern int buildTreeOfOptOpsFromDepChains(BaseOperation *op, OpList & treeList, int ruleIndex);
extern BaseOperation *buildShellOperation(char *opcode);
extern BaseOperation *buildShellOperation(int opcode);
extern BaseInstruction *buildInstrWithOper(BaseOperation *op);

extern BaseInstruction *getOptInstrCorrespondingToOper(BaseOperation *op);

extern BaseInstruction *getPrevInstr(BaseInstruction *instr, BaseInstruction *start);

// Speficic to Opt
extern BaseInstruction *getOptPrevInstr(BaseInstruction *instr);

extern OpCodeTable globalOpCodeTable;
extern SymbolTable *globalSymbolTable;

extern void markOp(BaseOperation *oper);
extern void deleteUselessIR(BasicBlockNode & bb, Routine *rout, int delEmptyInstrs);
extern void deadCodeElimination(BasicBlockNode *bb, int delEmptyInstrs);

typedef struct ArgTuple
{
	int _genericOrTarget;
	int _opPos;
	int _dstOrSrc;
	int _argNum;
} ArgTuple;

typedef Vector<ArgTuple> ArgTupleList;

typedef Map<int, ArgTupleList> ArgTuplesMap;

#endif
