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
/*************************************************************
 *file  : PredTable.h					                     *
 *authors : Aviral Shrivastava                               *
 *created : July, 30, 2000                                   *
 *last modified :                                            *
 *                                                           *
 *compiled with : g++                                        *
 *for more info : aviral@ics.uci.edu                         *
 *************************************************************/

#ifndef _PRED_TABLE_H_
#define _PRED_TABLE_H_

#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include <string.h>
#include <iostream.h>
#include <vector>

#include "MyString.h"
#include "node.h"
#include "PredTableNode.h"

#ifdef vector
#undef vector
#endif 
#define vector std::vector


#ifdef WIN32
#pragma warning(disable:4786)
#endif

#ifdef __UNIX__
	#include <stdio.h>
	#include <ctype.h>
#endif

typedef vector<PredTableNode> PredNodeList;
typedef vector<PredTableNode>::iterator PredNodeListIter;

class PredTable{
	vector <PredTableNode> _predTable;

public :

	// Default constructor
	PredTable();

	// Default destructor
	~PredTable();
	
	// Copy Constructor
	PredTable(const PredTable & a);

	// operator =
	operator = (const PredTable & a);

	// Build the predication table
	void buildPredTable(NodePtr);

	// Print the predication table
	void printPredTable(ostream & out);

	// Print the predication table in a file
	void printPredTable(ostream & out, FILE *);

	// Add an unpredicated operation
	void addUnPredicatedOperation(NodePtr);

	// Add a predicated operation
	void addPredicatedOperation(NodePtr);

	// Predicated counterpart
	MyString predicatedCounterpart(MyString);

};


int isUnPredicatedOperation(NodePtr operationNode);
int isPredicatedOperation(NodePtr operationNode);

#endif