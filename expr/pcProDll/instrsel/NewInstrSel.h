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
// File:  		NewInstrSel.h
// Created:		Fri Jul 27, 01
// Last modified: 	Fri Jul 27, 01
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: SUPPORT

#ifndef __NEWINSTRSEL_H__
#define __NEWINSTRSEL_H__

#include "stdafx.h"

#include "BaseArgument.h"
#include "BaseOperation.h"
#include "BaseInstruction.h"
#include "NormalInstr.h"
#include "BasicBlockNode.h"
#include "Routine.h"
#include "Program.h"
#include "DUProperty.h"
#include "SymbolTable.h"
#include "ISelParse.h"

class ISelIROpNode
{
private:
	BaseOperation *_op;
	BaseInstruction *_instr;
	int _flg;

public:
	ISelIROpNode() : _op(NULL), _instr(NULL), _flg(_INVALID_) { }
	ISelIROpNode(BaseOperation *op, BaseInstruction *instr, int flg = _INVALID_) : _op(op), _instr(instr), _flg(flg) { }

	~ISelIROpNode() {}

	BaseOperation *getOp() { return _op; }
	void setOp(BaseOperation *op) { _op = op; }

	BaseInstruction *getInstr() { return _instr; }
	void setInstr(BaseInstruction *instr) { _instr = instr; }

	int getFlg() { return _flg; }
	void setFlg(int flg) { _flg = flg; }

	int operator < (const ISelIROpNode & b)
	{
		if (_op < b._op) return _YES_;
		return _NO_;
	}

	int operator == (const ISelIROpNode & b)
	{
		if (_op == b._op) return _YES_;
		return _NO_;
	}

	int operator == (const BaseOperation *op)
	{
		if (_op == op) return _YES_;
		return _NO_;
	}

	ISelIROpNode & operator = (const ISelIROpNode & b)
	{
		_op = b._op;
		_instr = b._instr;
		_flg = b._flg;

		return *this;
	}
};

extern Vector<ISelIROpNode> listOfISelIROps;

typedef struct
{
	BaseArgument *_arg;
	BaseOperation *_op;
} ArgAndOper;

extern Map<int, ArgAndOper> identToArgMap;

extern Vector<ISelIROpNode>::iterator findOperFromListOfISelIROps(BaseOperation *op);

extern void addOpsToISelList(BaseInstruction *instr);

extern void printISelList();

extern BaseArgument *getArgForIdent(int ident);
extern BaseArgument *getArgForIdent(ArgIdent & ident);

extern void addArgToIdentList(int ident, BaseArgument *arg, BaseOperation *op);

extern void clearIdentToArgMap();

extern int getIndex(int num, int destOrSrc);

extern void setAsDoneInstrSel(BaseOperation *op);
extern void setAsDoneInstrSel(ISelIROpNode & op);
extern void setAsDoneInstrSel(OpList & oList);

extern int doneInstrSel(BaseOperation *op);

extern void resetISelFlg();

extern int buildListOfOpsFromDepChains(BaseOperation *op, OpList & treeList, int size);
extern int buildListOfISelOpsSequential(BaseOperation *op, OpList & treeList, int size);

extern BaseOperation *buildShellOperation(char *opcode);
extern BaseOperation *buildShellOperation(int opcode);
extern BaseInstruction *buildInstrWithOper(BaseOperation *op);

extern BaseInstruction *getISelInstrCorrespondingToOper(BaseOperation *op);

extern BaseInstruction *getPrevInstr(BaseInstruction *instr, BaseInstruction *start);

extern BaseInstruction *getISelPrevInstr(BaseInstruction *instr);

extern OpCodeTable globalOpCodeTable;
extern SymbolTable *globalSymbolTable;

extern void markOp(BaseOperation *oper);
extern void deleteUselessIR(BasicBlockNode & bb, Routine *rout, int delEmptyInstrs = 1);

extern BaseArgument *createTempArg(BaseArgument *actArg = NULL);

#endif
