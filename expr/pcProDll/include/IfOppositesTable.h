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
// File:  		IfOppositesTable.h
// Created:		Tue Jul 14, 98
// Last modified: 	Tue Jul 14, 98
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: ARCHITECTURE, INCLUDE

#ifndef __IFOPPOSITESTABLE_H__
#define __IFOPPOSITESTABLE_H__

#include "GlobalIncludes.h"
#include "OpCodeTable.h"
#include "Instruction.h"

extern OpCodeTable globalOpCodeTable;

class IfOppositesTable
{
private:
protected:
	static Vector<int> _op1Table;
	static Vector<int> _op2Table;
	int _count;

public:
	IfOppositesTable(): _count(0){}
	~IfOppositesTable(){}

	void addEntry(int op1, int op2)
	{
		_op1Table[_count] = op1;
		_op2Table[_count] = op2;
		_count++;
	}

	void addEntry(char *op1, char *op2)
	{
		_op1Table[_count] = globalOpCodeTable.getIndex(op1);
		_op2Table[_count] = globalOpCodeTable.getIndex(op2);
		_count++;
	}

	int getOppositeIndex(int index1)
	{
		int bankNum = _INVALID_;
		int countNum = _INVALID_;

		for (int i = 0; i < _count; i++)
		{
			if (_op1Table[i] == index1)
			{
				return _op2Table[i];
			}
			else if (_op2Table[i] == index1)
			{
				return _op1Table[i];
			}
		}

		// Error: could not find the index.
		cout << "Warning: Could not find opposite of : " << index1 << endl;
		return _INVALID_;
	}

	int getOppositeIndex(char *opName)
	{
		return getOppositeIndex(globalOpCodeTable.getIndex(opName));
	}
};

#endif