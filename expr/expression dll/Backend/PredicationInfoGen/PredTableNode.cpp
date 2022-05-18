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

#ifdef __UNIX__
	#include <stdio.h>
	#define _ASSERT assert
#else
	#include "crtdbg.h"
#endif

#include "PredTableNode.h"
#include <vector>

// Every class needs to have the following:
//
// 1) Default constructor
// 2) Default Destructor
// 3) Copy Constructor
// 4) operator =

// Other optional, but very useful methods :
//
// o1) copy() : creates a new copy of this object
// o2) operator == (checks to see if two objects are equal)
// o3) operator << (for printing)


// Default Constructor
PredTableNode::PredTableNode() : _predicatedOperation(NULL), _unpredicatedOperation(NULL) {}


// Default Destructor
PredTableNode::~PredTableNode()
{
	// Note: We do not delete the pointers (_predicatedOperation & _unpredicatedOperation) becos the objects they point to are not owned by this class.
}


// Copy Constructor
PredTableNode::PredTableNode(const PredTableNode & a) : _predicatedOperation(a._predicatedOperation), _unpredicatedOperation(a._unpredicatedOperation) {}


// operator =
PredTableNode::operator = (const PredTableNode & a)
{
	_predicatedOperation = a._predicatedOperation;
	_unpredicatedOperation = a._unpredicatedOperation;
}


// copy() : creates a new copy of this object
PredTableNode * PredTableNode :: copy()
{
	PredTableNode *retPtr = new PredTableNode(*this);
	return retPtr;
}



PredTableNode::printPredTableNode(ostream & out)
{
	if (_predicatedOperation)
		out << "Predicated Operation Name : " << (_predicatedOperation)->Name() << endl; 
	if (_unpredicatedOperation)
		out << "UnPredicated Operation Name : " << (_unpredicatedOperation)->Name() << endl; 
}


// Print the predication information in the predication information file
PredTableNode::printPredTableNodeToFile(FILE *predInfoFilePtr)
{
	if (_unpredicatedOperation != NULL)
		fprintf(predInfoFilePtr, "unpredOp = %s", _unpredicatedOperation->Name()); 
	
	if (_predicatedOperation != NULL)
		fprintf(predInfoFilePtr, "\tpredOp = %s", _predicatedOperation->Name());

	fprintf(predInfoFilePtr, "\n");
}


// Function		: returns the predicated operation
// Return value : if there exists, predicated operation
//				  else NULL
NodePtr PredTableNode::predicatedOperation()
{ return  _predicatedOperation; }


// Function		: returns the unpredicated operation
// Return value : if there exists, unpredicated operation
//				  else NULL
NodePtr PredTableNode::unPredicatedOperation()
{ return  _unpredicatedOperation; }


// Function		: Adds a predicated operation.
// Return value : If already a predicated function exists then
//				  the new node is not added and a 0 is returned
//				  else returns 1 
int PredTableNode::addPredicatedOperation(NodePtr node)
{
	if (_predicatedOperation != NULL)
	{ return 0; }
	else
	{ _predicatedOperation = node; return 1; }
}


// function		: Adds an unpredicated operation
// Return value : If already a unpredicated function exists then
//				  the new node is not added and a 0 is returned
//				  else returns 1 	
int PredTableNode::addUnPredicatedOperation(NodePtr node)
{
	if (_unpredicatedOperation != NULL)
	{ return 0; }
	else
	{ _unpredicatedOperation = node; return 1; }
}
