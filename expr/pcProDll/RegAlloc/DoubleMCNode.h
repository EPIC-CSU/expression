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
//: DoubleMCNode.h
//
// File:  		DoubleMCNode.h
// Created:		Thu Nov 2, 2000
// Author:		Radu Cornea
// Email:		radu@ics.uci.edu, ilp@ics.uci.edu
//
// This special node is used to hold 2 normal nodes that represent fp double values 
// that are to be allocated to adjacent floating point registers
// 


#ifndef __DOUBLEMCNODE_H__
#define __DOUBLEMCNODE_H__

#include "MCNode.h"

class DoubleMCNode : public MCNode {
	MCNode *_firstMCNode;
	MCNode *_secondMCNode;

public:
	DoubleMCNode(MultiChain *firstMC, MultiChain *secondMC) {
		MCNode *temp;
		temp = new MCNode(firstMC);
		setFirstNode(temp);
		temp = new MCNode(secondMC);
		setSecondNode(temp);
	}

	DoubleMCNode(MCNode *firstMCNode, MCNode *secondMCNode) {
		setFirstNode(firstMCNode);
		setSecondNode(secondMCNode);
	}

	~DoubleMCNode() {
		//if (_firstMCNode)
		//	delete _firstMCNode;
		//if (_secondMCNode)
		//	delete _secondMCNode;
	}

	void setFirstNode(MCNode *first) {
		_firstMCNode = first;
		first->setDoubleMCNode(this);
	}
	void setSecondNode(MCNode *second) {
		_secondMCNode = second;
		second->setDoubleMCNode(this);
	}

	MCNode * getFirstNode() { return _firstMCNode; }
	MCNode * getSecondNode() { return _secondMCNode; }

	virtual bool isDoubleMCNode() { return true; }

	MultiChain * getMC() {
		ASSERT_TRACE(0, "Called getMC() in DoubleMCNode");
		return NULL;
	}

	double computeSpillCost() {
		// may have to account for simulaneous double value saves/reloads
		return (_firstMCNode->computeSpillCost() + _secondMCNode->computeSpillCost()) * 10;
	}

	//void setColor(int color) {
		//ASSERT_TRACE(0, "Called setColor() in DoubleMCNode");

	//}

	//int getColor() {
	//	ASSERT_TRACE(0, "Called getColor() in DoubleMCNode");
	//	return -1;
	//}

	void addAdjNode(IGNode *node) {
		ASSERT_TRACE(0, "Called addAdjNode() in DoubleMCNode");
	}

	void removeAdjNode(IGNode *node) {
		ASSERT_TRACE(0, "Called removeAdjNode() in DoubleMCNode");
	}

	void resetAdjList() {
		_firstMCNode->resetAdjList();
		_secondMCNode->resetAdjList();
	}

	void resetMCAdjList() {
		_firstMCNode->resetMCAdjList();
		_secondMCNode->resetMCAdjList();
	}

	MyLinkedList<IGNode *>& getAdjNodeList() {
		ASSERT_TRACE(0, "Called getAdjNodeList() in DoubleMCNode");
		return _adjNodeList;
	}

	int getNoOfAdjNodes() {
		return (_firstMCNode->getNoOfAdjNodes() + _secondMCNode->getNoOfAdjNodes());
	}

	void print(ostream& out);
};

#endif	// __DOUBLEMCNODE_H__