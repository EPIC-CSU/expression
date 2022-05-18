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
//: IGraph.h
//
// File:  		IGraph.h
// Created:		Thu Feb 10, 2000
// Author:		Radu Cornea
// Email:		radu@ics.uci.edu, ilp@ics.uci.edu
//
// Data structure for implementing the interference graph
// 

#ifndef __IGGRAPH_H__
#define __IGGRAPH_H__

#include <iostream.h>
#include "MyLinkedList.h"

class IGNode;
class MultiChain;

class IGraph {
public:
	MyLinkedList<IGNode *> _nodeList;
	
public:
	IGraph() {}
	
	// does not make a copy of the nodes (IGNode)
	//IGraph(IGraph& graph) : _nodeList(*(graph.getNodeList())) {}

	// delete the graph (nodes and edges), but not the elements inside nodes
	~IGraph() { removeAllNodes(); }
	
	void addNode(IGNode *node);
	
	void addEdge(IGNode *node1, IGNode *node2);
	
	// remove the node and all adjacent edges
	void removeNode(IGNode *node);

	void removeEdge(IGNode *node1, IGNode* node2);

	// extract a node along with all its adjacent edges
	IGNode * extractNode(IGNode *node);

	IGNode * insertNode(IGNode *node);

	void removeAllNodes();

	MyLinkedList<IGNode *>& getNodeList();
	MyLinkedList<MultiChain *> * getMCList();

	int numItems() { return _nodeList.numItems(); }

	int existEdge(IGNode *node1, IGNode *node2);

	IGraph * buildComplement();
	
	void print(ostream& out);

	void print() { print(cout); }
		
	void printNodes(ostream& out);
	
	void printEdges(ostream& out);

	void printMCEdges(ostream& out);
	
	friend ostream& operator<<(ostream& out, IGraph& graph)
	{
		graph.print(out);
		return out;
	}
	
	void printGML(ostream& out);
};

#endif	// __IGGRAPH_H__

