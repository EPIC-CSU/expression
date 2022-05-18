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
//: MultiChain.h
//
// File:  		MultiChain.h
// Created:		Fri Nov 05, 99
// Author:		Radu Cornea
// Email:		radu@ics.uci.edu, ilp@ics.uci.edu
//
// MultiChain implements the multichain associated to each variable (argument).
// A multichain consists of a set of defs and the set of uses associated to each def.
// 

#ifndef __MULTICHAIN_H__
#define __MULTICHAIN_H__

#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include "SetList.h"
#include "IGNode.h"
#include "RegArchModel.h"
//#include "Routine.h"

//class SymbolTable;
//extern SymbolTable* globalSymbolTable;

class BaseArgument;
class BaseOperation;
class BaseInstruction;
class Routine;
class NormalInstr;

//template <class T>
//class SetList;

class MultiChain {
	//int _ID;
	int _symIndex;
	int _ssaNum;
	//int _regIndex;
	SetList<BaseInstruction*> _useList;
	SetList<BaseInstruction*> _defList;
	int _spilled;
	double _spillCost;
	int _defsNo, _usesNo;
	MultiChain *_siblingMC;
	Vector<RegClassEntryNode2> _regClass;

public:
	MultiChain(int ID = 0);
	MultiChain(SetList<BaseOperation *>& uses,
			   SetList<BaseOperation *>& defs,
			   int ID = 0);
	MultiChain(SetList<BaseInstruction *>& uses,
			   SetList<BaseInstruction *>& defs,
			   int ID = 0);
	MultiChain(MultiChain& chain);
	
	~MultiChain();

	//void removeUseDefList();

	void setID(int ID);

	int getID();

	void setSiblingMC(MultiChain *mc) { _siblingMC = mc; }

	MultiChain * getSiblingMC() { return _siblingMC; }
	
	int getRegIndex() {
		//return _regIndex;
		return _symIndex;
	}

	int getSSANum() {
		//return _regIndex;
		return _ssaNum;
	}

	int isCallAddress();
	int isCallParamArg();
	int isCallReturnArg();
	int isStackPointerArg();
	int isConditionCodeArg();
	int isDoubleFloatArg();

	void setSpilled() { _spilled++; }

	void resetSpilled() { _spilled = 0; }

	int isSpilled() { return _spilled; }

	char * getName();

	int isEq(MultiChain* mChain);

	bool isMC() { return true; }

	int noOfDefs();
	int noOfUses();

	void computeSpillCost();

	double getSpillCost();

	//defList operations
	void addDef(BaseInstruction *def) { _defList.add(def); _defsNo++; }
	void addDefList(MyLinkedList<BaseInstruction *>& defs)
	{
		_defList.set_union((SetList<BaseInstruction *>&) defs);
		_defsNo += defs.numItems();
	}
	bool containsDef(BaseInstruction *def) { return _defList.contains(def); }
	SetList<BaseInstruction *>& getDefList() { return _defList; }
	void removeDef(BaseInstruction *def) { _defList.remove(def); _defsNo--; }
	//void addDefAndLinks(BaseOperation* def);

	//useList operations
	void addUse(BaseInstruction *use) { _useList.add(use); _usesNo++; }
	void addUseList(MyLinkedList<BaseInstruction *>& uses)
	{
		_useList.set_union((SetList<BaseInstruction *>&) uses);
		_usesNo += uses.numItems();
	}
	bool containsUse(BaseInstruction *use) { return _useList.contains(use); }
	SetList<BaseInstruction *>& getUseList() { return _useList; }
	void removeUse(BaseInstruction *use) { _useList.remove(use); _usesNo--; }
	//void addUseAndLinks(BaseOperation* use);

	//list iterators
	MyLinkedListIterator<BaseInstruction *>* useListIteratorForw() {
		return _useList.elementsForward();
	}
 
	MyLinkedListIterator<BaseInstruction *>* useListIteratorBack() {
		return _useList.elementsBackward();
	}

	MyLinkedListIterator<BaseInstruction *>* defListIteratorForw() {
		return _defList.elementsForward();
	}
 
	MyLinkedListIterator<BaseInstruction *>* defListIteratorBack() {
		return _defList.elementsBackward();
	}

	SetList<BaseArgument *> * getAllArgs();

	void unionWith(MultiChain& prop);

	Vector<RegClassEntryNode2>& computeRegClass();

	Vector<RegClassEntryNode2>& getRegClass();

	void print(ostream& out);
	void print() { print(cout); }

	friend ostream& operator<<(ostream& out, MultiChain& mc) {
		mc.print(out);
		return out;
	}

	friend ostream& operator<<(ostream& out, MultiChain* p) {
		p->print(out);
		return out;
	}
	
	static MultiChain * MultiChain::getMCOfArgID(BaseArgument *defArg, Routine *routine);
	static void compMultiChainsOfInstr(BaseInstruction *instr, Routine *routine);
};

#endif	// __MULTICHAIN_H__