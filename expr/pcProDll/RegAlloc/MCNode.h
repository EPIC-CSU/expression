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
//: MCNode.h
//
// File:  		MCNode.h
// Created:		Thu Feb 10, 2000
// Author:		Radu Cornea
// Email:		radu@ics.uci.edu, ilp@ics.uci.edu
//
// The MultiChain node in the interference graph
// 


#ifndef __MCNODE_H__
#define __MCNODE_H__

#include "IGNode.h"

class MultiChain;
class DoubleMCNode;
class MetaMCNode;

class MCNode : public IGNode {
	MultiChain *_mc;
	DoubleMCNode *_doubleNode;

public:
	MCNode() { _doubleNode = NULL; }
	MCNode(MultiChain *mc) { _mc = mc; _doubleNode = NULL; }
	~MCNode() {}

	bool isMC() { return true; }

	virtual bool isDoubleMCNode() { return false; }

	virtual bool isMetaMCNode() { return false; }

	DoubleMCNode *getDoubleMCNode() { return _doubleNode; }
	void setDoubleMCNode(DoubleMCNode *node) { _doubleNode = node; }

	virtual MultiChain * getMC() { return _mc; }

	virtual double computeSpillCost();

	void print(ostream& out);
};

#endif	// __MCNODE_H__