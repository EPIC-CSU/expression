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
//: MyGraph.h
//
// File:  		MyGraph.h
// Created:		Fri Nov 16, 1999
// Author:		Radu Cornea
// Email:		radu@ics.uci.edu, ilp@ics.uci.edu
//
// Implements graph objects
// 

#ifndef __MYGRAPH_H__
#define __MYGRAPH_H__

#include <iostream.h>
#include "MyLinkedList.h"

// forward declaration
template <class Tn, class Te>
class MyGraphEdge;

template <class Tn, class Te>
class MyGraph;

template <class Tn, class Te>
class MyGraphNode {
	friend class MyGraphEdge<Tn, Te>;
	friend class MyGraph<Tn, Te>;
	
	int _id;
	
	// The node's item
	//Tn& _item;
	Tn _item;
	
	// The adjecent edges
	MyLinkedList<MyGraphEdge<Tn, Te> *> _adjEdges;
	
	static int getNewID(int reset = 0)
	{
		static int _count = 0;
		if (reset)
			_count =0;
		else
			_count++;
		
		return _count;
	}
	
	int getID()
	{
		return _id;
	}
	
public:
	MyGraphNode(Tn& item)
		: _adjEdges(), _item(item) { _id = getNewID(); }
	
	~MyGraphNode() {}
	
	void setItem(Tn& item)
	{
		_item = item;
	}

	Tn& getItem()
	{
		return _item;
	}
	
	void addAdjEdge(MyGraphEdge<Tn, Te> *pEdge)
	{ 
		_adjEdges.add(pEdge);
	}
	
	MyLinkedListIterator<MyGraphEdge<Tn, Te> *> * getAdjEdgesIterator()
	{
		return _adjEdges.elementsForward();
	}
	
	int noOfAdjEdges() {
		return _adjEdges.numItems();
	}

	void print(ostream& out)
	{ 
		out << _id << " ( " << _item << " ) " << endl;
		
		/*
		out << "Node: " << _item << " ( id = " << _id << " )" << endl;
		out << "+-List of adjacent edges: ";
		
		MyLinkedListIterator<MyGraphEdge<Tn, Te> *> *edgeIter = _adjEdges.elementsForward();
		while (edgeIter->hasMoreElements()) {
			MyGraphEdge<Tn, Te> *pEdge = edgeIter->currentElement();
			
			out << "(* " << pEdge->getID() << " *) ";
			
			edgeIter->nextElement();
		}
		delete edgeIter;	
		
		out << endl;
		*/
	}
	
	//template<class Tn, class Te>
	friend ostream& operator<<(ostream& out, MyGraphNode& node)
	{
		node.print(out);
		return out;
	}
	
	void printGML(ostream& out)
	{
		out << "node [" << endl;
		out << "	id " << _id << endl;
		out << "	label \"" << _id << "\"" << endl;
		out << "]" << endl;
	}
};

template <class Tn, class Te>
class MyGraphEdge {
private:
	friend class MyGraphNode<Tn, Te>;
	friend class MyGraph<Tn, Te>;
	
	int _id;
	
	// The edge's item
	//Te& _item;
	Te _item;
	
	// The 2 terminal nodes
	MyGraphNode<Tn, Te> *_term1;
	MyGraphNode<Tn, Te> *_term2;
	
	static int getNewID(int reset = 0)
	{
		static int _count = 0;
		if (reset)
			_count = 0;
		else
			_count++;
		
		return _count;
	}
	
	int getID()
	{
		return _id;
	}
	
	
public:
	MyGraphEdge(MyGraphNode<Tn, Te> *pNode1, MyGraphNode<Tn, Te> *pNode2, Te& item)
		: _term1(pNode1), _term2(pNode2), _item(item) { _id = getNewID(); }
	
	~MyGraphEdge() {}
	
	void setItem(Te& item)
	{
		_item = item;
	}
	
	Te& getItem()
	{
		return _item;
	}

	MyGraphNode<Tn, Te> * getFirstNode()
	{
		return _term1;
	}
	
	MyGraphNode<Tn, Te> * getSecondNode()
	{
		return _term2;
	}
	
	void print(ostream& out)
	{
		cout << _term1->getID() << " - " << _term2->getID() << endl;

		/*
		out << "Edge: " << _item << " ( id = " << _id << " )" << endl;
		out << "Terminals: (* " << _term1->getID() << " *), (* " << _term2->getID() << " *)" << endl;
		*/
	}
	
	//template<class Tn, class Te>
	friend ostream& operator<<(ostream& out, MyGraphEdge& edge)
	{
		edge.print(out);
		return out;
	}
	
	void printGML(ostream& out)
	{
		out << "edge [" << endl;
		out << "	source " << _term1->getID() << endl;
		out << "	target " << _term2->getID() << endl;
		out << "	label \"" << _id << "\"" << endl;
		out << "]" << endl;
	}
};

template<class Tn, class Te>
class MyGraph {
private:
	
	MyLinkedList<MyGraphNode<Tn, Te> *> _nodeList;
	MyLinkedList<MyGraphEdge<Tn, Te> *> _edgeList;
	
	MyGraphNode<Tn, Te> * getNode(Tn& item) {
		MyGraphNode<Tn, Te> *pNode = NULL;
		
		MyLinkedListIterator<MyGraphNode<Tn, Te> *> *nodeIter = _nodeList.elementsForward();
		while ((pNode == NULL) && nodeIter->hasMoreElements()) {
			MyGraphNode<Tn, Te> *crtNode = nodeIter->currentElement();
			
			if (crtNode->getItem() == item)
				pNode = crtNode;
			
			nodeIter->nextElement();
		}
		delete nodeIter;
		
		return pNode;
	}
	
	MyGraphEdge<Tn, Te> * getEdge(MyGraphNode<Tn, Te> *node1, MyGraphNode<Tn, Te> *node2) {
		if (node1 == NULL)
			return NULL;
		
		MyLinkedListIterator<MyGraphEdge<Tn, Te> *> *adjEdges = node1->getAdjEdgesIterator();
		while (adjEdges->hasMoreElements()) {
			MyGraphEdge<Tn, Te> *crtEdge = adjEdges->currentElement();
			
			if ((crtEdge->getFirstNode() == node2) ||
				(crtEdge->getSecondNode() == node2)) {
				
				delete adjEdges;
				return crtEdge;
			}
			
			adjEdges->nextElement();
		}
		delete adjEdges;
		
		return NULL;
	}

	MyGraphEdge<Tn, Te> * getEdge(Tn& nodeItem1, Tn& nodeItem2) {
		MyGraphNode<Tn, Te> *node1 = getNode(nodeItem1);
		MyGraphNode<Tn, Te> *node2 = getNode(nodeItem2);

		return getEdge(node1, node2);
	}
	
	
public:
	MyGraph()
		: _nodeList(), _edgeList() { MyGraphNode<Tn, Te>::getNewID(1); MyGraphEdge<Tn, Te>::getNewID(1); }
	
	MyGraph(MyGraph& graph)
	{

		MyGraph();

		MyLinkedListIterator<MyGraphNode<Tn, Te> *> *nodeIter = graph._nodeList.elementsForward();
		while (nodeIter->hasMoreElements()) {
			MyGraphNode<Tn, Te> *pNode = nodeIter->currentElement();
			
			addNode(pNode->getItem());
			
			nodeIter->nextElement();
		}
		delete nodeIter;
		
		MyLinkedListIterator<MyGraphEdge<Tn, Te> *> *edgeIter = graph._edgeList.elementsForward();
		while (edgeIter->hasMoreElements()) {
			MyGraphEdge<Tn, Te> *pEdge = edgeIter->currentElement();
			
			addEdge(pEdge->getFirstNode()->getItem(), pEdge->getSecondNode()->getItem(), pEdge->getItem());
			
			edgeIter->nextElement();
		}
		delete edgeIter;
	}	

	~MyGraph()
	{
		removeAllNodes();
		/*
		// delete all edges
		MyLinkedListIterator<MyGraphEdge<Tn, Te> *> *edgeIter = _edgeList.elementsForward();
		while (edgeIter->hasMoreElements()) {
			MyGraphEdge<Tn, Te> *pEdge = edgeIter->currentElement();
			
			delete pEdge;
			
			edgeIter->nextElement();
		}
		delete edgeIter;
		
		// delete all nodes
		MyLinkedListIterator<MyGraphNode<Tn, Te> *> *nodeIter = _nodeList.elementsForward();
		while (nodeIter->hasMoreElements()) {
			MyGraphNode<Tn, Te> *pNode = nodeIter->currentElement();
			
			delete pNode;
			
			nodeIter->nextElement();
		}
		delete nodeIter;
		*/
	};
	
	void addNode(Tn& item)
	{ 
		MyGraphNode<Tn, Te> *tmp = new MyGraphNode<Tn, Te>(item);
		_nodeList.add(tmp);
	}
	
	void addEdge(Tn& nodeItem1, Tn& nodeItem2, Te& item ) {
		MyGraphNode<Tn, Te> *pNode1 = getNode(nodeItem1);
		MyGraphNode<Tn, Te> *pNode2 = getNode(nodeItem2);
		
		if (pNode1 && pNode2) {
			MyGraphEdge<Tn, Te> *pEdge = getEdge(pNode1, pNode2);

			if (pEdge) {
				pEdge->setItem(item);
			} else {
				pEdge = new MyGraphEdge<Tn, Te>(pNode1, pNode2, item);
			
				_edgeList.add(pEdge);
				pNode1->addAdjEdge(pEdge);
				pNode2->addAdjEdge(pEdge);
			}
		}
	}
	
	/*
	void removeNode(Tn& item)
	{ 
		MyGraphNode<Tn, Te> *tmp = getNode(item);
		_nodeList.remove(tmp);
	}
	*/
	
	void removeEdge(Tn& nodeItem1, Tn& nodeItem2) {
		MyGraphEdge<Tn, Te> *pEdge = getEdge(nodeItem1, nodeItem2);
		
		if (pEdge) {
			pEdge->getFirstNode()->_adjEdges.remove(pEdge);
			pEdge->getSecondNode()->_adjEdges.remove(pEdge);

			_edgeList.remove(pEdge);
			delete pEdge;
		}
	}
	
	void removeNode(Tn& item)
	{
		MyGraphNode<Tn, Te> *pNode = getNode(item);

		MyLinkedListIterator<MyGraphEdge<Tn, Te> *> *edgesIter = pNode->getAdjEdgesIterator();
		while (edgesIter->hasMoreElements()) {
			MyGraphEdge<Tn, Te> *pEdge = edgesIter->currentElement();
			
			MyGraphNode<Tn, Te> *pFirstNode = pEdge->getFirstNode();
			if (pFirstNode == pNode)
				pEdge->getSecondNode()->_adjEdges.remove(pEdge);
			else
				pFirstNode->_adjEdges.remove(pEdge);
			
			_edgeList.remove(pEdge);
			delete pEdge;

			edgesIter->nextElement();
		}
		delete edgesIter;

		_nodeList.remove(pNode);
		delete pNode;
	}

	void removeAllNodes() {
		// delete all edges
		MyLinkedListIterator<MyGraphEdge<Tn, Te> *> *edgeIter = _edgeList.elementsForward();
		while (edgeIter->hasMoreElements()) {
			MyGraphEdge<Tn, Te> *pEdge = edgeIter->currentElement();
			
			delete pEdge;
			
			edgeIter->nextElement();
		}
		delete edgeIter;

		_edgeList.removeAll();
		
		// delete all nodes
		MyLinkedListIterator<MyGraphNode<Tn, Te> *> *nodeIter = _nodeList.elementsForward();
		while (nodeIter->hasMoreElements()) {
			MyGraphNode<Tn, Te> *pNode = nodeIter->currentElement();
			
			delete pNode;
			
			nodeIter->nextElement();
		}
		delete nodeIter;

		_nodeList.removeAll();
	}

	MyLinkedList<Tn> * getNodeItemList() {
		MyLinkedList<Tn> *nodeItems = new MyLinkedList<Tn>();
		
		MyLinkedListIterator<MyGraphNode<Tn, Te> *> *nodeIter = _nodeList.elementsForward();
		while (nodeIter->hasMoreElements()) {
			MyGraphNode<Tn, Te> *pNode = nodeIter->currentElement();
			
			nodeItems->append(pNode->getItem());
			
			nodeIter->nextElement();
		}
		delete nodeIter;
		
		return nodeItems;
	}
	
	MyLinkedList<Tn> * getNeighboursList(Tn& nodeItem) {
		MyGraphNode<Tn, Te> *pNode = getNode(nodeItem);

		MyLinkedListIterator<MyGraphEdge<Tn, Te> *> *edgesIter = pNode->getAdjEdgesIterator();

		MyLinkedList<Tn> *neighboursList = new MyLinkedList<Tn>();
		
		while (edgesIter->hasMoreElements()) {
			MyGraphEdge<Tn, Te> *pEdge = edgesIter->currentElement();
			
			MyGraphNode<Tn, Te> *firstNode = pEdge->getFirstNode();
			if (firstNode == pNode)
				neighboursList->append(pEdge->getSecondNode()->getItem());
			else
				neighboursList->append(firstNode->getItem());
			
			edgesIter->nextElement();
		}
		delete edgesIter;
		
		return neighboursList;
	}

	int noOfNeighbours(Tn& nodeItem)
	{
		MyGraphNode<Tn, Te> *pNode = getNode(nodeItem);

		if (pNode)		
			return pNode->noOfAdjEdges();
		else
			return 0;
	}

	int numItems()
	{
		return _nodeList.numItems();
	}

	int existEdge(Tn& nodeItem1, Tn& nodeItem2) {
		return (getEdge(nodeItem1, nodeItem2) != NULL);
	}
	
	MyGraph<Tn, Te>& buildComplement(Te& edgeItem)
	{
		MyGraph<Tn, Te> *compl = new MyGraph();
		
		MyLinkedListIterator<MyGraphNode<Tn, Te> *> *nodeIter = _nodeList.elementsForward();
		while (nodeIter->hasMoreElements()) {
			MyGraphNode<Tn, Te> *pNode = nodeIter->currentElement();
			
			compl->addNode(pNode->getItem());
			
			nodeIter->nextElement();
		}
		delete nodeIter;
		
		MyLinkedListIterator<MyGraphNode<Tn, Te> *> *nodeIter1 = _nodeList.elementsForward();
		while (nodeIter1->hasMoreElements()) {
			Tn& item1 = nodeIter1->currentElement()->getItem();
			
			MyLinkedListIterator<MyGraphNode<Tn, Te> *> *nodeIter2 = _nodeList.elementsForward();
			while (nodeIter2->hasMoreElements()) {
				Tn& item2 = nodeIter2->currentElement()->getItem();
				
				cout << "Items: " << item1 << " " << item2 << endl;
				
				if ((item1 != item2) && !existEdge(item1, item2) &&
					!existEdge(item2, item1) && !compl->existEdge(item2, item1))
					compl->addEdge(item1, item2, edgeItem);
				
				nodeIter2->nextElement();
			}
			delete nodeIter2;
			
			nodeIter1->nextElement();
		}
		delete nodeIter1;
		
		
		return *compl;
	}
	
	void print(ostream& out)
	{ 
		out << "Graph:" << endl;
		
		printNodes(out);
		
		printEdges(out);
	}
	
	void printNodes(ostream& out)
	{ 
		out << "List of nodes:" << endl;
		
		MyLinkedListIterator<MyGraphNode<Tn, Te> *> *nodeIter = _nodeList.elementsForward();
		while (nodeIter->hasMoreElements()) {
			MyGraphNode<Tn, Te> *pNode = nodeIter->currentElement();
			
			//out << *pNode;
			out << "  ";
			pNode->print(out);
			
			nodeIter->nextElement();
		}
		delete nodeIter;
	}

	void printEdges(ostream& out)
	{ 
		out << "List of edges:" << endl;
		
		MyLinkedListIterator<MyGraphEdge<Tn, Te> *> *edgeIter = _edgeList.elementsForward();
		while (edgeIter->hasMoreElements()) {
			MyGraphEdge<Tn, Te> *pEdge = edgeIter->currentElement();
			
			//out << *pEdge;
			out << "  ";
			pEdge->print(out);
			
			edgeIter->nextElement();
		}
		delete edgeIter;
	}

	//template<class Tn, class Te>
	friend ostream& operator<<(ostream& out, MyGraph& graph)
	{
		graph.print(out);
		return out;
	}
	
	void printGML(ostream& out)
	{
		out << "graph [" << endl;
		
		MyLinkedListIterator<MyGraphNode<Tn, Te> *> *nodeIter = _nodeList.elementsForward();
		while (nodeIter->hasMoreElements()) {
			MyGraphNode<Tn, Te> *pNode = nodeIter->currentElement();
			
			pNode->printGML(out);
			
			nodeIter->nextElement();
		}
		delete nodeIter;
		
		MyLinkedListIterator<MyGraphEdge<Tn, Te> *> *edgeIter = _edgeList.elementsForward();
		while (edgeIter->hasMoreElements()) {
			MyGraphEdge<Tn, Te> *pEdge = edgeIter->currentElement();
			
			pEdge->printGML(out);
			
			edgeIter->nextElement();
		}
		delete edgeIter;
		
		out << "]" << endl;
	}
	
};

#endif	// __MYGRAPH_H__

