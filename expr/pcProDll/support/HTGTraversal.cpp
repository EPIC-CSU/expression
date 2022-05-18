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
// File:  		HTGTraversal.cpp
// Created:		Wed Mar 11, 98
// Last modified: 	Wed Mar 11, 98
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: SUPPORT

#include "stdafx.h"

#include "HTGTraversal.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 


void HTGTraversalFuncPtrs::traverseIfHTGNode(BaseHTGNode *node)
{
	(*_ifHTGTraversal)(node, *this);
}

void HTGTraversalFuncPtrs::traverseLoopHTGNode(BaseHTGNode *node)
{
	(*_loopHTGTraversal)(node, *this);
}

void HTGTraversalFuncPtrs::traverseSimpleHTGNode(BaseHTGNode *node)
{
	(*_simpleHTGTraversal)(node);
}

void HTGTraversalFuncPtrs::traverseHTGNode(BaseHTGNode* node)
{
		if ( node->isComplexNode() || node->isCallNode() )
		{
			// as of now, do nothing.
		}
		else if ( node->isIfNode() ) 
		{
			traverseIfHTGNode(node);
		}
		else if ( node->isLoopNode() )
		{
			traverseLoopHTGNode(node);
		}
		else if ( node->isSimpleNode() )
		{
			traverseSimpleHTGNode(node);
		}
}

void HTGTraversalFuncPtrs::traverseHTGNodes(MyLinkedList<BaseHTGNode *> & list)
{
	MyLinkedListIterator<BaseHTGNode *> *iter;
	iter = list.elementsForward();

	while (iter->hasMoreElements())
	{
		traverseHTGNode(iter->currentElement());

		iter->nextElement();
	}
	delete iter;
}

void IfHTGTraversalFunc::operator ()(BaseHTGNode *node, HTGTraversalFuncPtrs & funcs)
{
	// we expect node to be of If HTG node type.

	IfHTGNode *ifNode = (IfHTGNode *)node;

	// first process the if head.
	//
	BaseHTGNode* ifHead = ((IfHTGNode*)node)->getIfHead();

	if (ifHead != NULL)
		funcs.traverseHTGNode(ifHead);

	// Next process the if true branch
	//
	funcs.traverseHTGNodes(ifNode->getTrueBranch());

	// Finally process the if false branch
	//
	funcs.traverseHTGNodes(ifNode->getFalseBranch());
}

void LoopHTGTraversalFunc::operator ()(BaseHTGNode *node, HTGTraversalFuncPtrs & funcs)
{
	// traverse the loop list.
	//
	LoopHTGNode *loopNode = (LoopHTGNode *)node;

	funcs.traverseHTGNodes(loopNode->getLoopList());
}

IfHTGTraversalFunc globalIfHTGTraversalFunc;
LoopHTGTraversalFunc globalLoopHTGTraversalFunc;
SimpleHTGTraversalFunc globalSimpleHTGTraversalFunc;

HTGTraversalFuncPtrs globalHTGTraversalFuncPtrs;