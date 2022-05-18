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
// $Id: SymbolTable.cc,v 1.4 1998/01/22 23:46:36 pgrun Exp $
//
// File:  		SymbolTable.cc
// Created:		Mon Nov 3, 97
// Last modified: 	Mon Nov 3, 97
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: 

#include "stdafx.h"

#include "SymbolTable.h"
#include "ArrayIterator.h"
#include "BaseArgument.h"
#include "BaseOperation.h"
#include "BaseInstruction.h"
#include "NormalInstr.h"
#include "Routine.h"
#include "DataOpSlot.h"
#include "ControlOpSlot.h"
#include "Program.h"
#include "CallOp.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

Array<STabEntry> SymbolTable::_symTab(0, 1);

SymbolTable *globalSymbolTable;

void STabEntry::setAll(int numVars, int startNum)
{
	_numSSAVars = numVars;
	_startNum = startNum;
}

void STabEntry::setAllMax(int numVars, int startNum)
{
	if ((_numSSAVars == _INVALID_) || (numVars > _numSSAVars))
	{
		_numSSAVars = numVars;
		_startNum = startNum;
	}
}

int STabEntry::nextStartNum()
{
	if ((_startNum == _INVALID_) || (_numSSAVars == _INVALID_))
		return _INVALID_;
	
	return _startNum + _numSSAVars;
}

int STabEntry::contains(int someNum)
{
	if ((someNum >= _startNum) && (someNum < _startNum +
		_numSSAVars))
		return _YES_;
	return _NO_;
}

int operator == (const STabEntry & a, const STabEntry & b)
{
	if ((a._numSSAVars == b._numSSAVars) && (a._startNum == b._startNum))
		return _YES_;
	return _NO_;
}

ostream & operator << (ostream & out, const STabEntry & s)
{
	out << " SSA: " << s._numSSAVars;
	out << " Start: " << s._startNum;
	return out;
}

void STabEntry::print(ostream & out)
{
	out << " Num SSA: " << _numSSAVars;
	out << " Start: " << _startNum;
}


void SymbolTable::setSize (int k)
{
	int count = _symTab.size();
	
	STabEntry tempSymTabEntry;
	
	if (k > count)
	{
		_symTab.setSize(k);
		
		for (int i = count; i < k; i++)
			_symTab.insertElement(tempSymTabEntry);
	}
}

int SymbolTable::getSize(){ return _symTab.size(); }

void SymbolTable::initialize()
{
	// By default, assume that each variable is defined atleast once.
	// This corresponds to the default register file initialization.
	//
	for (int i = 0; i < _symTab.size(); i++)
	{
		_symTab.elementAt(i).setAllMax(1, -1);
	}
}

void SymbolTable::setEntry(int index, int numSSACopies, int startNum)
{
	if (index < _symTab.capacity())
		_symTab.elementAt(index).setAllMax(numSSACopies, startNum);
	else
		cout << "Warning: Invalid index for SymbolTable::setEntry()" << endl;
}

int SymbolTable::getIndex(BaseArgument *ssaReg)
{
	if (ssaReg == NULL) return _INVALID_;
	
	if (ssaReg->isRegisterArg() == _NO_)
	{
		//	       cout << "Warning: For symbol table, argument must be register type." << endl;
		return _INVALID_;
	}
	
	int regNum = ((RegisterArg *) ssaReg)->getVal();
	char *regText = ((RegisterArg *) ssaReg)->getText();
	
	if (globalRegFileList == NULL) 
	{
		cout << "Panic: No global register file list." << endl;
		assert(0);
	}
	return globalRegFileList->getID(regNum, regText);
}

int SymbolTable::getIndex(int regNum, char *regText)
{
	if (globalRegFileList == NULL) 
	{
		cout << "Panic: No global register file list." << endl;
		assert(0);
	}
	
	return globalRegFileList->getID(regNum, regText);
}

int SymbolTable::getStartNum(int index)
{
	try
	{
		return _symTab.elementAt(index).getStartNum();
	}
	catch (Exception *n)
	{
		cout << "Panic: Invalid index" << endl;
#ifdef WIN32
		cout << "Exception: " << *n;
#endif
		assert(0);
	}
	// SAH : Wed Nov 03, 99 : This path should never be taken.
	//
	return -1;
}

int SymbolTable::getNumVars(int index)
{
	try
	{
		return _symTab.elementAt(index).getNumVars();
	}
	catch (Exception *n)
	{
		cout << "Panic: Invalid index" << endl;
#ifdef WIN32
		cout << "Exception: " << *n;
#endif
		assert(0);
	}
	// SAH : Thu Dec 02, 99 : This path should never be taken.
	//
	return -1;
}


int SymbolTable::getID(int regNum, char *regText, int ssaNum)
{
	if (globalRegFileList == NULL) 
	{
		cout << "Panic: No global register file list." << endl;
		assert(0);
	}
	
	int index = globalRegFileList->getID(regNum, regText);
	
	try
	{
		if (ssaNum <= _symTab.elementAt(index).getNumVars())
		{
			return (_symTab.elementAt(index).getStartNum() +
				ssaNum - SSA_START_NUM);
		}
		else
		{
			cout << "Panic: Symbol Table: Invalid SSA Number" << endl;
		}
	}
	catch (Exception *n)
	{
		cout << "Panic: Symbol Table: Invalid index" << endl;
#ifdef WIN32
		cout << "Exception: " << *n;
#endif
		assert(0);
	}
	
	//NOS
	ASSERT_TRACE(0,"Forced EXIT!");
	return 0;
}

int SymbolTable::getID(int idFromGlobalRegFileList, int ssaNum)
{
	// given the idWithoutSSA of the register, and the ssaNumber, get the uniqId
	//
	try
	{
		if (ssaNum <= _symTab.elementAt(idFromGlobalRegFileList).getNumVars())
		{
			return (_symTab.elementAt(idFromGlobalRegFileList).getStartNum() +
				ssaNum - SSA_START_NUM);
		}
		else
		{
			cout << "Panic: Symbol Table: Invalid SSA Number" << endl;
		}
	}
	catch (Exception *n)
	{
		cout << "Panic: Symbol Table: Invalid index" << endl;
#ifdef WIN32
		cout << "Exception: " << *n;
#endif
		assert(0);
	}
	
	//NOS
	ASSERT_TRACE(0,"Forced EXIT!");
	return 0;
}

int SymbolTable::getSSANum(int regNum, char *regText )
{
	if (globalRegFileList == NULL) 
	{
		cout << "Panic: No global register file list." << endl;
		assert(0);
	}
	
	int index = globalRegFileList->getID(regNum, regText);
	
	return   _symTab.elementAt(index).getNumVars();
}


int SymbolTable::getID(BaseArgument *regArg, int ssaNum)
{
	int index = getIndex(regArg);
	
	return (getStartNum(index) + ssaNum - SSA_START_NUM);
}

int SymbolTable::getID(BaseArgument *regArg, BaseArgProperty *ssaProp)
{
	if (ssaProp == NULL)
		return _INVALID_;
	
	if (!ssaProp->isSSAArgProperty())
	{
		cout << "Panic: Non SSA property" << endl;
		assert(0);
	}
	
	int ssaNum = ((SSAArgProperty *) ssaProp)->getSSANum();
	
	return getID(regArg, ssaNum);	 
}

// This function returns the index number given the ID.
// This is useful for performing a reverse mapping from ID to the
// actual register name and number.
//
int SymbolTable::getIndex(int symTabID)
{
	ArrayIterator<STabEntry> *symTabIter;
	symTabIter = _symTab.elements();
	
	int retVal = _INVALID_;
	
	while (symTabIter->hasMoreElements())
	{
		if (symTabIter->currentElement().contains(symTabID))
		{
			retVal = symTabIter->currentIndex();
			break;
		}
		symTabIter->nextElement();
	}
	delete symTabIter;
	
	return retVal;
}

void SymbolTable::setStartNums()
{
	int nextStart;
	
	ArrayIterator<STabEntry> *symTabIter;
	symTabIter = _symTab.elements();
	
	symTabIter->currentElement().setStartNum(START_NUM);
	nextStart = symTabIter->currentElement().nextStartNum();
	
	symTabIter->nextElement();
	
	while (symTabIter->hasMoreElements())
	{
		if (nextStart == _INVALID_)
			break;
		
		symTabIter->currentElement().setStartNum(nextStart);
		nextStart =
			symTabIter->currentElement().nextStartNum();
		
		symTabIter->nextElement();
	}
	
	delete symTabIter;
}

char *SymbolTable::getRegName(int symTabID)
{
	if (globalRegFileList == NULL) 
	{
		cout << "Panic: No global register file list." << endl;
		assert(0);
	}
	const char *rName;
	int rNum, ssaNum;
	
	if (!doneSSA)
	{
		rName = (globalRegFileList->getRegFile(symTabID)).name();
		rNum = symTabID - (globalRegFileList->getRegFile(symTabID)).getID();
	}
	else
	{
		int regIndex = getIndex(symTabID);
		
		rName = (globalRegFileList->getRegFile(regIndex)).name();
		rNum = regIndex - (globalRegFileList->getRegFile(regIndex)).getID();
		ssaNum = symTabID - getStartNum(regIndex) + SSA_START_NUM;
	}
	
	char *retPtr = new char[80];
	strcpy(retPtr, rName);
	
	char buffer[20];
	itoa(rNum, buffer, 10);
	
	strcat(retPtr, buffer);
	
	if (doneSSA)
	{
		strcat(retPtr, ".");
		itoa(ssaNum, buffer, 10);
		strcat(retPtr, buffer);
	}
	
	return retPtr;
}

void SymbolTable::print(ostream & out)
{
	ArrayIterator<STabEntry> *symTabIter;
	symTabIter = _symTab.elements();
	
	while (symTabIter->hasMoreElements())
	{
		out << "Reg Id: " << symTabIter->currentIndex() << " ";
		symTabIter->currentElement().print(out);
		out << endl;
		symTabIter->nextElement();
	}
	delete symTabIter;
}

void SymbolTable::addNewRegs(int index, int numSSAVars)
{
	setEntry(index, numSSAVars, _INVALID_);
}

void SymbolTable::addNewRegs(BaseArgument *arg, int numSSAVars)
{
	int index = getIndex(arg);
	addNewRegs(index, numSSAVars);
}

int SymbolTable::getTotNumVars()
{
	int retVal = 0;
	ArrayIterator<STabEntry> *symTabIter;
	symTabIter = _symTab.elements();
	
	while (symTabIter->hasMoreElements())
	{
		retVal += (symTabIter->currentElement()).getNumVars();
		symTabIter->nextElement();
	}
	delete symTabIter;
	
	return retVal;
}

// destOrSrc == 1 => destination, destOrSrc == 0 => source
void IDToArgsMap::addArg(int index, BaseArgument *arg, int destOrSrc)
{
	if (arg == NULL) return;

	if (destOrSrc == 1)
	{
		((_idToArgsMap[index])._defList).push_back(arg);
	}
	else
	{
		((_idToArgsMap[index])._useList).push_back(arg);
	}
}

void IDToArgsMap::addDefArg(int index, BaseArgument *arg)
{
	if (arg == NULL) return;

	((_idToArgsMap[index])._defList).push_back(arg);
}

void IDToArgsMap::addUseArg(int index, BaseArgument *arg)
{
	if (arg == NULL) return;

	((_idToArgsMap[index])._useList).push_back(arg);
}

// use this function only if the index is to be gotten using the uniqID method of BaseArgument.
// i.e. by using symbol table id as the index
//
void IDToArgsMap::addArg(BaseArgument *arg, int destOrSrc)
{
	if (arg == NULL) return;
	if (!arg->isRegisterArg()) return;

	int index = arg->getUniqID();
	addArg(index, arg, destOrSrc);
}

// use this function only if the index is to be gotten using the uniqID method of BaseArgument.
// i.e. by using symbol table id as the index
//
void IDToArgsMap::addDefArg(BaseArgument *arg)
{
	if (arg == NULL) return;
	if (!arg->isRegisterArg()) return;

	int index = arg->getUniqID();
	addDefArg(index, arg);
}

// use this function only if the index is to be gotten using the uniqID method of BaseArgument.
// i.e. by using symbol table id as the index
//
void IDToArgsMap::addUseArg(BaseArgument *arg)
{
	if (arg == NULL) return;
	if (!arg->isRegisterArg()) return;

	int index = arg->getUniqID();
	addUseArg(index, arg);
}

MultiArgList & IDToArgsMap::getArgs(int index)
{
	return _idToArgsMap[index];
}

ArgList & IDToArgsMap::getDefArgs(int index)
{
	return (_idToArgsMap[index])._defList;
}

ArgList & IDToArgsMap::getUseArgs(int index)
{
	return (_idToArgsMap[index])._useList;
}

// use this function only if the index is to be gotten using the uniqID method of BaseArgument.
// i.e. by using symbol table id as the index
//
MultiArgList & IDToArgsMap::getArgs(BaseArgument *arg)
{
	int index = _INVALID_;
	if ((arg != NULL) && (arg->isRegisterArg()))
		index = arg->getUniqID();
	return getArgs(index);
}

// use this function only if the index is to be gotten using the uniqID method of BaseArgument.
// i.e. by using symbol table id as the index
//
ArgList & IDToArgsMap::getDefArgs(BaseArgument *arg)
{
	int index = _INVALID_;
	if ((arg != NULL) && (arg->isRegisterArg()))
		index = arg->getUniqID();
	return getDefArgs(index);
}

// use this function only if the index is to be gotten using the uniqID method of BaseArgument.
// i.e. by using symbol table id as the index
//
ArgList & IDToArgsMap::getUseArgs(BaseArgument *arg)
{
	int index = _INVALID_;
	if ((arg != NULL) && (arg->isRegisterArg()))
		index = arg->getUniqID();
	return getUseArgs(index);
}

void IDToArgsMap::eraseAll()
{
	_idToArgsMap.erase(_idToArgsMap.begin(), _idToArgsMap.end());
}

void compIDToArgsMap(BaseOperation *oper, IDToArgsMap & idToArgs)
{
	if (oper == NULL) return;

/*
	oper->shortPrint(cout);
	cout << endl;
*/
	// go thru the dest and source lists, add the args.
	//
	ArgList & dL = oper->destOperandList();
	ArgListIter di;

	for (di = dL.begin(); di != dL.end(); di++)
	{
		if (*di == NULL) continue;

		idToArgs.addDefArg(*di);
	}

	ArgList & sL = oper->sourceOperandList();
	ArgListIter si;

	for (si = sL.begin(); si != sL.end(); si++)
	{
		if (*si == NULL) continue;

		idToArgs.addUseArg(*si);
	}

	if (oper->isCallOp())
	{
		ArgList & pL = ((CallRetOp *)oper)->getParmList();
		ArgListIter pi;

		for (pi = pL.begin(); pi != pL.end(); pi++)
		{
			if (*pi == NULL) continue;

			idToArgs.addUseArg(*pi);
		}
	}
}

void compIDToArgsMap(BaseInstruction *instr, IDToArgsMap & idToArgs)
{
	if (instr == NULL) return;

	// for each op in instr, get its operands and add them to the map.
	//
	ArrayIterator<DataOpSlot> *di = ((NormalInstr *)instr)->getDataOpSlotIter();
	while (di->hasMoreElements())
	{
		if ((di->currentElement()).hasOper())
			compIDToArgsMap((di->currentElement()).getPtrToOperation(), idToArgs);
		di->nextElement();
	}
	delete di;

	ArrayIterator<ControlOpSlot> *ci = ((NormalInstr *)instr)->getControlOpSlotIter();
	while (ci->hasMoreElements())
	{
		if ((ci->currentElement()).hasOper())
			compIDToArgsMap((ci->currentElement()).getPtrToOperation(), idToArgs);
		ci->nextElement();
	}
	delete ci;
}

Map<Routine *, IDToArgsMap> globalIDToArgsMap;

// given a routine, removes its previous id to args map and creates a new one.
//
void compIDToArgsMap(Routine *rout)
{
	// iterate over the Routine's IList.
	//
	if (rout == NULL) return;

	MyLinkedListIterator<BaseInstruction *> *iter = rout->instrListIteratorForw();

	IDToArgsMap & tMap = globalIDToArgsMap[rout];
	tMap.eraseAll();

	while (iter->hasMoreElements())
	{
		compIDToArgsMap(iter->currentElement(), tMap);
		iter->nextElement();
	}
	delete iter;
}

void compIDToArgsMap(Program *prog)
{
	// iterate over the program's routine list
	//
	MyLinkedListIterator<Routine *> *iter = prog->routineListIteratorForw();

	while (iter->hasMoreElements())
	{
		compIDToArgsMap(iter->currentElement());
		iter->nextElement();
	}
	delete iter;
}
