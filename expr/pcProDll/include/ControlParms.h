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
// File:  		ControlParms.h
// Created:		Wed Jul 14 99
// Last modified: 	Wed Jul 14 99
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: include

// This file defines the data structure (a table) that holds all
// the control parameters on a per routine basis.
//
// Examples of control parameters are:
//
// PRINT_ILIST_BEFORE_SSA
// PRINT_ILIST_AFTER_SSA_BEFORE_ISEL
// PRINT_ILIST_AFTER_ISEL_BEFORE_TBZ

// Note: The string "EXPRESS_all" is reserved and is assumed to mean
// all routines.

// Note: The columns are for routines and the rows are for the control parameters.
//

#ifndef __CONTROLPARMS_H__
#define __CONTROLPARMS_H__

#include "GlobalIncludes.h"
#include "MyString.h"
#include "STLIncludes.h"
#include "FilePointer.h"

static MyString allRoutines("EXPRESS_all");
static MyString noRoutines("EXPRESS_none");

class ControlParmTable
{
private:
	typedef Vector<int> ControlTableList;

	Map<MyString, ControlTableList> _cntrlTable;

	Map<MyString, int> _colIndex;
	int _maxNumCols;

public:
	ControlParmTable() :_maxNumCols(0)
	{
		_colIndex[allRoutines] = _maxNumCols;
		_maxNumCols++;
	}

	~ControlParmTable(){}

	void addToColIndex(MyString & col);
	void addToColIndex(char *col);

	void addEntry(MyString & row, MyString & col);
	void addEntry(char *row, char *col);

	int hasEntry(MyString & row, MyString & col);
	int hasEntry(char *row, char *col);

	void addEntriesFromFile(char *fileName);
};

#endif