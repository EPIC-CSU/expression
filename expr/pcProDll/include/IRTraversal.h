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
// File:  		IRTraversal.h
// Created:		Fri Sep 03, 99
// Last modified: 	Fri Sep 03, 99
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module:

#ifdef WIN32
#pragma warning(disable:4786)
#endif

#ifndef __IRTRAVERSAL_H__
#define __IRTRAVERSAL_H__

#include "GlobalIncludes.h"
#include "STLIncludes.h"

//#include "Routine.h"
#include "BaseInstruction.h"
#include "BaseOperation.h"

#include "NormalInstr.h"
#include "CallOp.h"

class Routine;

const int WALK_BACKWARDS = -1;
const int WALK_FORWARDS = 1;

const int WALK_DF = -1; // DF = Data Flow order.
const int WALK_CF = 1; // CF = Control Flow order.

const int NORMAL_MODE = -1;
const int FAST_MODE = 1;

const int ALL_OP = -1; // Data, Control, and Flow.
const int DC_OP = 1; // Data, and Control

class WalkInstr
{
private:
	int _walkType; // One of WALK_BACKWARDS, WALK_FORWARDS.

	int _walkEdge; // Currently, only WALK_CF is supported.

	int _mode; // FAST_MODE : no error checking.

	Map<BaseInstruction *, InstrList> _neighborList;

	InstrList _startList;

	Routine *_rout;

	void _addUniq(BaseInstruction *instr, BaseInstruction *addInstr);
	void _delUniq(BaseInstruction *instr, BaseInstruction *addInstr);

	void _setWalkForInstr(BaseInstruction *instr);
	void _resetWalkForInstr(BaseInstruction *instr);

public:
	WalkInstr() : _walkType(0), _walkEdge(WALK_CF), _mode(NORMAL_MODE), _rout(NULL) { }
	
	WalkInstr(int wT, int wE, int md) : _walkType(wT), _walkEdge(wE), _mode(md), _rout(NULL) { }

	~WalkInstr() { _neighborList.clear(); _startList.clear(); }

	void setWalkType(int wT) { _walkType = wT; }

	int getWalkType() { return _walkType; }

	void setWalkEdge(int wE) { _walkEdge = wE; }
	
	int getWalkEdge() { return _walkEdge; }

	void setMode(int md) { _mode = md; }

	int getMode() { return _mode; }

	void initWalk(Routine *rout);

	Routine *getRoutine() { return _rout; }

	InstrList & getStart() { return _startList; }

	InstrList & getNext(BaseInstruction *instr);

	// Note: This reset retains the values of _walkType, _walkEdge, and _mode
	//
	void reset();

	void print(ostream & out);

	// ***********************************
	// Methods to update the Walk Mapping.
	// ***********************************

	// This method adds the instrs in the toList to each instr in the updateList.
	//
	void updateInstrs(InstrList & updateList, InstrList & toList);

	// This method adds a new instr, given its predecessor and successor lists.
	//
	void addNewInstr(BaseInstruction *instr, InstrList & preds, InstrList & succs, int updateStart = 0);

	// This method, given an instr, looks at its successors (thru flow ops)
	// and updates the walker.
	// However, note that the predessors of the instruction are not set.
	//
	// This method is really specific to way PS works.
	//
	void addNewInstr(BaseInstruction *instr);

	// This method, given an instr, looks at its successors (thru flow ops)
	// and updates the walker (removes instr from all its successors list).
	// However, note that the predessors of the instruction are not set.
	//
	void removeInstr(BaseInstruction *instr);

	void addPred(BaseInstruction *instr, BaseInstruction *pred);

	// deletes the edge between instr and delInstr (removes delInstr from the list of neighbours of instr)
	void delEdge(BaseInstruction *instr, BaseInstruction *delInstr);

};

class OpToInstrMap
{
private:
	
	// Note: An operation can belong to only one instruction.

	int _mode;

	int _opType; // Currently, can only specify ALL_OP or DATA_CONTROL_OP

	Routine *_rout;

	Map<BaseOperation *, BaseInstruction *> _opToInstrMap;

public:
	OpToInstrMap() : _mode(NORMAL_MODE), _opType(ALL_OP), _rout(NULL) { }

	OpToInstrMap(int oT, int md) : _opType(oT), _mode(md), _rout(NULL) { }

	~OpToInstrMap() { _opToInstrMap.clear(); }

	void setMode(int md) { _mode = md; }

	int getMode() { return _mode; }

	void setOpType(int oT) { _opType = oT; }

	int getOpType() { return _opType; }

	Routine *getRoutine() { return _rout; }

	void initOpToInstrMap(Routine *rout);

	BaseInstruction *getInstr(BaseOperation *oper);

	// Note: This reset retains the values of _mode and _opType
	//
	void reset();

	void print(ostream & out);

	// ******************************
	// Methods to update the Mapping.
	// ******************************

	void removeOper(BaseOperation *oper);

	// If oper exists, change it. Else, add it.
	//
	void addOper(BaseOperation *oper, BaseInstruction *instr);

	// If oper does not exist, flags an error.
	//
	void changeOper(BaseOperation *oper, BaseInstruction *newInstr);

	void changeOpers(BaseInstruction *oldInstr, BaseInstruction *newInstr);

	void addInstr(BaseInstruction *instr);
};

#endif