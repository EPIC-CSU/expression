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
// File:  		RenameTable.cc
// Created:		Wed Mar 24, 99
// Last modified: 	Wed Mar 24, 99
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: 

#include "stdafx.h"

#include "RenameTable.h"
#include "SymbolTable.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

extern SymbolTable *globalSymbolTable;

void RenameTable::setLowestValue(BaseArgument *arg, BaseArgProperty *ssa)
{
	int newNum = -1;
	
	int index = getIndex(arg);
	
	int i = getArrayIndex(index);
	
	int ssaNum = getSSANum(ssa);
	
	//		cout << "Printing index, ssaNum : " << index << " " << ssaNum << endl;
	
	if (i < _count) // found the element in the array.
	{
		if (ssaNum < _startNum[i]) // this is the lowest value encountered.
		{
			_startNum[i] = ssaNum;
			_endNewNum[i] = ssaNum;
			//
			// Note: endNewNum[i] is set to ssaNum and not ssaNum + 1.
			// This is basically equivalent to setting all the entries
			// in the array to -1
		}
	}
	else // this is the first time the register argument was encountered.
	{
		_indexTable.push_back(index);
		_startNum.push_back(ssaNum);
		
		_endNewNum.push_back(ssaNum);
		//
		// Note: endNewNum[i] is set to ssaNum and not ssaNum + 1.
		// This is basically equivalent to setting all the entries
		// in the array to -1
		
		int maxOldSSA = globalSymbolTable->getNumVars(index);
		_lastOldNum.push_back(maxOldSSA);
		// 
		// globalSymbolTable->getNumVars returns the number of vars for that
		// perticular register argument. This may be one larger than the max ssa number.
		// If so, this needs to be corrected appropriately.
		
		_count++;
	}
}

void RenameTable::renameValue(BaseArgument *arg, BaseArgProperty *ssa)
{
	int newNum = -1;
	
	int index = getIndex(arg);
	
	int i = getArrayIndex(index);
	
	int ssaNum = getSSANum(ssa);
	
	//		cout << "Printing index, ssaNum : " << index << " " << ssaNum << endl;
	
	if (i < _count) // found the element in the array.
	{
		if (ssaNum < _startNum[i]) // error condition.
		{
			cout << "Panic: Something is wrong with the SSA Numbering" << endl;
			assert(0);
		}
		newNum = ssaNum + 1 + _lastOldNum[i] - _startNum[i];
		if (newNum > _endNewNum[i]) _endNewNum[i] = newNum;
	}
	else // this is the first time the register argument was encountered.
	{
		//Note: this path should not be exectued if setLowestValues was called before
		// this method is executed.
		
		cout << "Warning: Recieved a previously un-encountered register argument in renameValue" << endl;
		
		int maxOldSSA = globalSymbolTable->getNumVars(index);
		_indexTable.push_back(index);
		_startNum.push_back(ssaNum);
		_lastOldNum.push_back(maxOldSSA);
		// 
		// globalSymbolTable->getNumVars returns the number of vars for that
		// perticular register argument. This may be one larger than the max ssa number.
		// If so, this needs to be corrected appropriately.
		
		newNum = maxOldSSA + 1;
		_endNewNum.push_back(newNum);
		_count++;
	}

	((SSAArgProperty*)ssa)->setSSANum(newNum);
	// cout << "Old Index, SSA Num: " << index << " " << ssaNum << endl;
	// cout << "New SSA Num: " << newNum << endl;
}

void RenameTable::resetRenameTable()
{
	_indexTable.clear();
	_startNum.clear();
	_endNewNum.clear();
	_lastOldNum.clear();
	_count = 0;
}

void RenameTable::resetSymbolTable()
{
	if (globalSymbolTable == NULL)
	{
		// error condition.
		//
		cout << "Panic: RenameTable recieved NULL symbol table" << endl;
		assert(0);
	}

	int i;
	for (i = 0; i < _endNewNum.size(); i++)
	{
		globalSymbolTable->setEntry(_indexTable[i], _endNewNum[i], _INVALID_);
	}

}
