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
//: IGraph.cpp
//
// File:  		IGraph.cpp
// Created:		Thu Feb 10, 2000
// Author:		Radu Cornea
// Email:		radu@ics.uci.edu, ilp@ics.uci.edu
//
// Data structure for implementing the interference graph
// 

#include "stdafx.h"

#include "IGraph.h"
#include "IGNode.h"
#include "MCNode.h"
#include "MetaMCNode.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

void IGraph::addNode(IGNode *node) {
	_nodeList.add(node); 
}

void IGraph::addEdge(IGNode *node1, IGNode *node2) {

	if ((node1 == NULL) || (node2 == NULL)) // error condition
	{
		// print out a warning for now!
		//
		ASSERT_TRACE(0, "In Interference Graph : Tried to add edge with NULL source/sink nodes");
		return;
	}
	node1->addAdjNode(node2);
	node2->addAdjNode(node1);
}

void IGraph::removeNode(IGNode *node)
{
	MyLinkedListIterator<IGNode *> *adjNodesIter = node->getAdjNodeList().elementsForward();
	while (adjNodesIter->hasMoreElements()) {
		IGNode *adjNode = adjNodesIter->currentElement();
		
		adjNode->removeAdjNode(node);
		
		adjNodesIter->nextElement();
	}
	delete adjNodesIter;
	
	node->getAdjNodeList().removeAll();

	_nodeList.remove(node);
	//delete node;
}

void IGraph::removeEdge(IGNode *node1, IGNode* node2) {
	node1->removeAdjNode(node2);
	node2->removeAdjNode(node1);
}

IGNode * IGraph::extractNode(IGNode *node)
{
	MyLinkedListIterator<IGNode *> *adjNodesIter = node->getAdjNodeList().elementsForward();
	while (adjNodesIter->hasMoreElements()) {
		IGNode *adjNode = adjNodesIter->currentElement();
		
		adjNode->removeAdjNode(node);
		
		adjNodesIter->nextElement();
	}
	delete adjNodesIter;

	_nodeList.remove(node);
	return node;
}

IGNode * IGraph::insertNode(IGNode *node)
{
	MyLinkedListIterator<IGNode *> *adjNodesIter = node->getAdjNodeList().elementsForward();
	while (adjNodesIter->hasMoreElements()) {
		IGNode *adjNode = adjNodesIter->currentElement();
		
		adjNode->addAdjNode(node);
		
		adjNodesIter->nextElement();
	}
	delete adjNodesIter;

	_nodeList.add(node);
	return node;
}

void IGraph::removeAllNodes() {
	
	// delete all nodes
	MyLinkedListIterator<IGNode *> *nodeIter = _nodeList.elementsForward();
	while (nodeIter->hasMoreElements()) {
		IGNode *node = nodeIter->currentElement();
		
		delete node;
		
		nodeIter->nextElement();
	}
	delete nodeIter;
	
	_nodeList.removeAll();
}

MyLinkedList<IGNode *>& IGraph::getNodeList()
{ 
	return _nodeList;
}

MyLinkedList<MultiChain *> * IGraph::getMCList()
{
	MyLinkedList<MultiChain *> *mcList = new MyLinkedList<MultiChain *>();
	
	MyLinkedListIterator<IGNode *> *nIter = _nodeList.elementsForward();
	while (nIter->hasMoreElements()) {
		IGNode *node = nIter->currentElement();
		
		if (node->isMC()) {
			
			if (((MCNode *) node)->isMetaMCNode()) {
				
				MyLinkedListIterator<MultiChain *> *mcListIter = ((MetaMCNode *) node)->getMCList().elementsForward();
				while (mcListIter->hasMoreElements()) {
					MultiChain *mc = mcListIter->currentElement();
					
					mcList->add(mc);
					
					mcListIter->nextElement();
				}
				delete mcListIter;
				
			} else {
				MultiChain *mc = ((MCNode *) node)->getMC();
				mcList->add(mc);
			}
		}

		nIter->nextElement();
	}
	delete nIter;

	return mcList;
}


int IGraph::existEdge(IGNode *node1, IGNode *node2)
{
	return node1->getAdjNodeList().contains(node2);
}

IGraph * IGraph::buildComplement()
{
	/*
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
	*/

	return NULL;
}

void IGraph::print(ostream& out)
{ 
	out << "Graph:" << endl;
	
	printNodes(out);
	
	printEdges(out);
}

void IGraph::printNodes(ostream& out)
{ 
	out << "List of nodes:" << endl;
	
	MyLinkedListIterator<IGNode *> *nodeIter = _nodeList.elementsForward();
	while (nodeIter->hasMoreElements()) {
		IGNode *node = nodeIter->currentElement();
		
		out << *node;
		//out << "  ";
		//node->print(out);
		out << endl;
		
		nodeIter->nextElement();
	}
	delete nodeIter;
}

void IGraph::printEdges(ostream& out)
{ 
	out << "List of edges:" << endl;
	
	MyLinkedListIterator<IGNode *> *nodeIter = _nodeList.elementsForward();
	while (nodeIter->hasMoreElements()) {
		IGNode *node = nodeIter->currentElement();
		
		MyLinkedListIterator<IGNode *> *adjNodeIter = node->getAdjNodeList().elementsForward();
		while (adjNodeIter->hasMoreElements()) {
			IGNode *adjNode = adjNodeIter->currentElement();
			
			//out << *pEdge;
			out << "  ";
			node->print(out);
			cout << " - ";
			adjNode->print(out);
			cout << endl;
			
			adjNodeIter->nextElement();
		}
		delete adjNodeIter;
		
		nodeIter->nextElement();
	}
	delete nodeIter;
}

void IGraph::printMCEdges(ostream& out)
{ 
	out << "List of MC-MC edges:" << endl;
	
	MyLinkedListIterator<IGNode *> *nodeIter = _nodeList.elementsForward();
	while (nodeIter->hasMoreElements()) {
		IGNode *node = nodeIter->currentElement();

		if (node->isMC()) {
		
			MyLinkedListIterator<IGNode *> *adjNodeIter = node->getAdjNodeList().elementsForward();
			while (adjNodeIter->hasMoreElements()) {
				IGNode *adjNode = adjNodeIter->currentElement();
				
				if (adjNode->isMC()) {	
					//out << *pEdge;
					out << "  ";
					node->print(out);
					cout << " - ";
					adjNode->print(out);
					cout << endl;
				}
				
				adjNodeIter->nextElement();
			}
			delete adjNodeIter;
			
		}
		
		nodeIter->nextElement();
	}
	delete nodeIter;
}


void IGraph::printGML(ostream& out)
{
	/*
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
	*/
}
