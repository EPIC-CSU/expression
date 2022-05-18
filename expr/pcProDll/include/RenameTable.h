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
// File:  		RenameTable.h
// Created:		Fri July 17, 98
// Last modified: 	Fri July 17, 98
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: ARGUMENT

#ifndef __RENAMETABLE_H__
#define __RENAMETABLE_H__

#include "GlobalIncludes.h"
#include "RegisterArg.h"
#include "SSAArgProperty.h"
#include "BaseArgument.h"
#include "BaseArgProperty.h"
#include "SymbolTable.h"
#include "RegisterFileList.h"

extern RegisterFileList *globalRegFileList;
extern SymbolTable *globalSymbolTable;

class RenameTable
{
protected:
	Vector<int> _indexTable;
	// this table stores the index of the base argument.
	// every register argument has a unique id. So, this can be used to index
	// into the other three tables described below.

	Vector<int> _startNum;
	// this table stores the ssa number of the first register argument (of the
	// corresponding index) encountered during the renaming phase.

	Vector<int> _endNewNum;
	// this table stores the max. ssa number of the register arg (of the corresponding
	// index).

	Vector<int> _lastOldNum;
	// This stores the previous highest ssa number of the register arg (of the
	// corresponding index).

	// new_number = this_ssa_number + 1 + _lastOldNum[i] - _startNum[i]

	int _count;

	inline int getIndex(BaseArgument *arg)
	{
		int regNum = ((RegisterArg *)arg)->getVal();
		char *regText = ((RegisterArg *)arg)->getText();

		if (globalRegFileList == NULL) 
	    {
	       cout << "Panic: No global register file list." << endl;
	       assert(0);
	    }
	 
		int index = globalRegFileList->getID(regNum, regText);
		return index;
	}

	inline int getSSANum(BaseArgProperty *ssa)
	{
		int ssaNum = ((SSAArgProperty*)ssa)->getSSANum();
		return ssaNum;
	}

	inline int getArrayIndex(int index)
	{
		int i;
		for (i = 0; i < _count; i++)
		{
			 if (_indexTable[i] == index)
				 break;
		}
		return i;
	}

public:
	RenameTable() : _count(0){}
	~RenameTable(){}

	void init(){}
/*
	void setLowestSSAValues(BaseHTGNode *someNode)
	{
		// given a region (like a loop or an if), try to find the lowest ssaNumber
		// for each index in that region. This serves as the base value for renaming.
		// i.e. the lowest ssaNumber gets assigned to the previous highest ssaNumber + 1

		someNode->setLowestSSAValues();
	}
*/
	void setLowestValue(BaseArgument *arg, BaseArgProperty *ssa);

	void renameValue(BaseArgument *arg, BaseArgProperty *ssa);

	void resetRenameTable();
	void resetSymbolTable();
};
#endif

