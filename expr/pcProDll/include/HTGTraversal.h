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
//$Id: LoopHTGNode.h,v 1.11 1998/03/27 04:26:33 pgrun Exp $
//
// File:  		LoopHTGNode.h
// Created:		Wed Jan 29, 97
// Last modified: 	Wed Jan 29, 97
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: include

#ifndef __HTGTRAVERSAL_H__
#define __HTGTRAVERSAL_H__

#include "MyLinkedList.h"
#include "MyLinkedListIterator.h"

#include "BaseHTGNode.h"
#include "SimpleHTGNode.h"
#include "IfHTGNode.h"
#include "LoopHTGNode.h"

class BaseHTGTraversalFunc;

class HTGTraversalFuncPtrs
{
private:
	BaseHTGTraversalFunc *_simpleHTGTraversal;
	BaseHTGTraversalFunc *_ifHTGTraversal;
	BaseHTGTraversalFunc *_loopHTGTraversal;

public:
	HTGTraversalFuncPtrs() : _simpleHTGTraversal(NULL), _ifHTGTraversal(NULL), _loopHTGTraversal(NULL) {}
	HTGTraversalFuncPtrs(const HTGTraversalFuncPtrs & b) : _simpleHTGTraversal(b._simpleHTGTraversal), _ifHTGTraversal(b._ifHTGTraversal), _loopHTGTraversal(b._loopHTGTraversal) {}
	~HTGTraversalFuncPtrs() { /* should we delete the traversal func pointers here?? */ }

	void addSimpleHTGTraversal(BaseHTGTraversalFunc *func) { _simpleHTGTraversal = func; }
	void addIfHTGTraversal(BaseHTGTraversalFunc *func) { _ifHTGTraversal = func; }
	void addLoopHTGTraversal(BaseHTGTraversalFunc *func) { _loopHTGTraversal = func; }

	void traverseSimpleHTGNode(BaseHTGNode *node);
	void traverseIfHTGNode(BaseHTGNode *node);
	void traverseLoopHTGNode(BaseHTGNode *node);
	void traverseHTGNode(BaseHTGNode *node);
	void traverseHTGNodes(MyLinkedList<BaseHTGNode *> & list);
};

class BaseHTGTraversalFunc
{
public:
	BaseHTGTraversalFunc(){}
	~BaseHTGTraversalFunc(){}

	virtual void operator ()(BaseHTGNode *node) { assert(0); }
	virtual void operator ()(BaseHTGNode *node, HTGTraversalFuncPtrs & funcs) { assert(0); }
};

class SimpleHTGTraversalFunc : public BaseHTGTraversalFunc // this is a generic Simple HTG Traversal function.
{
	// do nothing here. The user has to derive another traversal function from this class.
	virtual void operator ()(BaseHTGNode *node) {  }
};

class IfHTGTraversalFunc : public BaseHTGTraversalFunc // this should be used by most traversal algorithms for If HTG nodes
{
public:
	IfHTGTraversalFunc(){}
	~IfHTGTraversalFunc(){}

	virtual void operator ()(BaseHTGNode *node, HTGTraversalFuncPtrs & funcs);
};

class LoopHTGTraversalFunc : public BaseHTGTraversalFunc // this should be used by most traversal algorithms for If HTG nodes
{
public:
	LoopHTGTraversalFunc(){}
	~LoopHTGTraversalFunc(){}

	virtual void operator ()(BaseHTGNode *node, HTGTraversalFuncPtrs & funcs);
};

extern void maintainDoubles(BasicBlockNode *bb);

// This SimpleHTGTraversal is used by the maintainDouble routine (for Loop unrolling/shifting)
//
class TraverseForDoubles : public BaseHTGTraversalFunc
{
public:
	virtual void operator ()(BaseHTGNode *node)
	{
		// we expect node to be of SimpleHTGNode type
		//
		maintainDoubles(((SimpleHTGNode *)node)->getBasicBlock());
	}
};

extern IfHTGTraversalFunc globalIfHTGTraversalFunc;
extern LoopHTGTraversalFunc globalLoopHTGTraversalFunc;
extern SimpleHTGTraversalFunc globalSimpleHTGTraversalFunc;

extern HTGTraversalFuncPtrs globalHTGTraversalFuncPtrs;

#endif