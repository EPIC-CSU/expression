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
// File:  		ControlParms.cpp
// Created:		Wed July 14, 99
// Last modified: 	
// Author:		Prabhat Kumar Mishra
// Email:		pmishra@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: SUPPORT


#include "stdafx.h"
#include "ControlParms.h"

ControlParmTable globalControlParms;

void ControlParmTable::addToColIndex(MyString & col)
{
    Map<MyString, int>::iterator i;

	// Check if 'col' is already present else add it.
	if((i = _colIndex.find(col)) == _colIndex.end())
	{
		_colIndex[col] = _maxNumCols;
		_maxNumCols++;
	}
}

void ControlParmTable::addToColIndex(char *col)
{
	MyString a(col);
	addToColIndex(a);
}

// This function adds an entry in the ControlParmTable. 
// It takes row and col as input strings.
// 
void ControlParmTable::addEntry(MyString & row, MyString & col)
{
    Map<MyString, int>::iterator i;

	int num = -1;

	// Check if 'col' is already present else add it.
	if((i = _colIndex.find(col)) != _colIndex.end())
	{
		num = (*i).second;
	}
	else
	{
		num = _maxNumCols;
		_colIndex[col] = _maxNumCols;
		_maxNumCols++;
	}

    // Add the entry
    (_cntrlTable[row]).push_back(num);
}


void ControlParmTable::addEntry(char *row, char *col)
{
	MyString a(row);
	MyString b(col);

	addEntry(a,b);
}

// This function checks whether there is an entry specified by given 
// row and col in ControlParmTable.
//
int ControlParmTable::hasEntry(MyString & row, MyString & col)
{
	Map<MyString, int>::iterator i;
	int colPos;

	// Get the integer corresponding to "col" if exists.
    if((i = _colIndex.find(col)) != _colIndex.end())
	{
		colPos = (*i).second;
	}
	else
	{
		// error condition.
		//
		cout << "Warning: Got an un-expected Column Index in ControlParmsTable" << endl;
		return _NO_;
	}

	// Get the row vector if exists.

	Map<MyString, ControlTableList>::iterator j;

	if((j = _cntrlTable.find(row)) == _cntrlTable.end())
	{
		// error condition.
		//
		cout << "Warning: Got an un-expected Row index in ControlParmsTable" << endl;
		return _NO_;
	}


	// Iterate over the row and check whether 'colPos' exists
	// Existance of EXPRESS_all entry covers all.

	vector<int>::iterator itr;

    for (itr = ((*j).second).begin(); itr != ((*j).second).end(); itr++)
	{
		if ((*itr == 0) || (*itr == colPos))
			return _YES_;
	}

	return _NO_;
}


int ControlParmTable::hasEntry(char *row, char *col)
{
	MyString a(row);
	MyString b(col);

	return (hasEntry(a, b));
}



// This function initializes ControlParmTable from a given file.
//
void ControlParmTable::addEntriesFromFile(char *fileName)
{
    FilePointer cpfPtr;
	char line[1024];
	char *row;
	char *col;
	char seps[] = " ,\t\n";

    try 
    {
	    cpfPtr.open(fileName, "r");
    }
    catch (CouldNotOpenFileException e)
    {
	    cout << e;
	    assert(0);
    }

    // Read from the file add entry.
    while (!feof(cpfPtr.file_ptr()))
	{
		fgets(line, 80, cpfPtr);

		row = strtok(line, seps);
		if (row == NULL) continue;

		col = strtok(NULL, seps);
		if (col == NULL)
		{
			// This means that a row was defined without any columns attached.
			
			MyString a(row);
			addEntry(a, noRoutines);
			continue;
		}

		while (col != NULL)
		{
			// TBD: We currently do not handle duplicate copies of columns;
			//
			addEntry(row, col);
			col = strtok(NULL, seps);
		}

	}

}


