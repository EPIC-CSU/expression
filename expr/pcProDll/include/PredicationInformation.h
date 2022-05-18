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


typedef struct isel_info
{
	MyString _genericIsel;
	Vector <MyString> _targetIsel;
} IselInfo;


typedef struct pred_info
{
	MyString _unpredicatedOperation;
	MyString _predicatedOperation;
} PredInfo;


class PredicationInformation 
{
private :
	Vector <IselInfo> _iselInfo;
	Vector <PredInfo> _predInfo;
	
protected :
	
public :

	// 1. Constructor
	PredicationInformation();


	// 2. Destructor
	~PredicationInformation();


	// 3. Add a predicated operation 
	addPredicatedOperation(MyString predOp);


	// 4. Add a Unpredicated operation
	addUnPredicatedOperation(MyString UnpredOp);


	// 5. Add a generic isel
	addGenericIsel(MyString gIsel);


	// 6. Add a target specific isel corresponding to generic isel
	addTargetIsel(MyString tIsel, MyString gIsel);
	
	
	// 7. 
}

typedef Vector<IselInfo>::iterator iselIter;
typedef Vector<PredInfo>::iterator predIter;
