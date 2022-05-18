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
//
// File:  			PredicationInformation.h
// Created:			August 20, 2000.
// Last modified: 	August 20, 2000.
// Author:			Aviral Shrivastava
// Email:			aviral@ics.uci.edu
//
////////////////////////////////////////////////////////


#ifndef _PREDICATION_INFORMATION_H_
#define _PREDICATION_INFORMATION_H_

#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include <vector>

#ifdef vector
#undef vector
#endif 
#define vector std::vector

#include "MyString.h"
#include "OpCode.h"

typedef vector <MyString> MyStringList;
typedef vector <MyString>::iterator myStringIter;

typedef struct isel_info
{
	MyString _genericIsel;
	MyStringList _targetIselList;
} IselInfo;


typedef struct pred_info
{
	MyString _unpredicatedOperation;
	MyString _predicatedOperation;
} PredInfo;


typedef vector <IselInfo> IselInfoList;
typedef vector <PredInfo> PredInfoList;

typedef vector <IselInfo>::iterator iselIter;
typedef vector <PredInfo>::iterator predIter;


class PredicationInformation 
{
private :
	IselInfoList _iselInfoList;
	PredInfoList _predInfoList;
	
protected :
	
public :

	// 1. Constructor
	PredicationInformation();


	// 2. Destructor
	~PredicationInformation();


	// 3. Add a unpredicated operation 
	void AddUnPredicatedOperation(MyString unpredOp);


	// 4. Add a predicated operation corresponding to a unpredicated operation
	void AddPredicatedOperation(MyString unpredOp, MyString predOp);


	// 5. Add a predicated operation which does not have a predicated counterpart
	void AddPredicatedOperation(MyString predOp);


	// 6. Add a generic isel
	void AddGenericIsel(MyString gIsel);


	// 7. Add a target specific isel corresponding to generic isel
	void AddTargetIsel(MyString tIsel, MyString gIsel);
	
	
	// 8. Tells whether the operation is predicated.
	bool IsPredicatedOperation(MyString Op);


	// 9. Tells whether the operation is predicated.
	bool IsUnPredicatedOperation(MyString Op);


	// 10. Give Predicated counterpart of an operation
	MyString GetPredicatedCounterpart(MyString Op);


	// 11. Reads the predication Information from a file
	void ReadPredicationInformation();


	// 12. Prints the predication Information
	void PrintPredicationInformation();

	
	// 13. Modify the Isel info
	MyString FindPredTargetIsel(MyString tIsel);


	// 14. Get the opcode for the Isel instruction
	int GetIselOpCode(OpCode & opc);

};


#endif