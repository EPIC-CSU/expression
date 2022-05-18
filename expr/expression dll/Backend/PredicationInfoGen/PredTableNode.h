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
 *file  : PredTableNode.h			                         *
 *authors : Aviral Shrivastava                               *
 *created : July, 30, 2000                                   *
 *last modified :                                            *
 *                                                           *
 *compiled with : g++                                        *
 *for more info : aviral@ics.uci.edu                         *
 *************************************************************/

#ifndef _PRED_TABLE_NODE_H_
#define _PRED_TABLE_NODE_H_

#ifdef WIN32
#pragma warning(disable:4786)
#pragma warning(disable:4503)
#endif

#include "node.h"
#include <ostream.h>

class PredTableNode{
	NodePtr _predicatedOperation; // pointer to predicated operation
	NodePtr _unpredicatedOperation; // pointer to unpredicated operation

public:

	// Every class needs to have the following:
	//
	// 1) Default Constructor
	PredTableNode();
	
	// 2) Default Destructor
	~PredTableNode();
	
	// 3) Copy Constructor
	PredTableNode(const PredTableNode & a);
	
	// 4) operator =
	operator = (const PredTableNode & a);

	// Other optional, but very useful methods :
	//
	// o1) copy() : creates a new copy of this object
	PredTableNode *copy();

	// o2) operator == (checks to see if two objects are equal)

	// o3) operator << (for printing

	// 
	printPredTableNode(ostream & out);


	// 
	printPredTableNodeToFile(FILE *fileptr);


	// Function		: returns the predicated operation
	// Return value : if there exists, predicated operation
	//				  else NULL
	NodePtr predicatedOperation();
	

	// Function		: returns the unpredicated operation
	// Return value : if there exists, unpredicated operation
	//				  else NULL
	NodePtr unPredicatedOperation();
	
	
	// Function		: Adds a predicated operation.
	// Return value : If already a predicated function exists then
	//				  the new node is not added and a 0 is returned
	//				  else returns 1 
	int addPredicatedOperation(NodePtr node);

	
	// function		: Adds an unpredicated operation
	// Return value : If already a unpredicated function exists then
	//				  the new node is not added and a 0 is returned
	//				  else returns 1 	
	int addUnPredicatedOperation(NodePtr node);


	// function		: Prints the predication table node
	// Return Value	: None 	
};

#endif