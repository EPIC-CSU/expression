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
//: IGNode.h
//
// File:  		IGNode.h
// Created:		Fri Nov 05, 99
// Author:		Radu Cornea
// Email:		radu@ics.uci.edu, ilp@ics.uci.edu
//
// An IGNode is a node of the Interference Graph. It is also the base class
// for MultiChain class

#ifndef __IGNODE_H__
#define __IGNODE_H__

#include <iostream.h>
#include "MyLinkedList.h"

#define UNCOLORED -1

class IGNode {
protected:
	MyLinkedList<IGNode *> _adjNodeList;
	int _color;

public:
	IGNode(int color = UNCOLORED) { _color = color; }

	~IGNode() {}

	virtual bool isMC() { return false; }

	virtual bool isReg() { return false; }

	void setColor(int color) {
		_color = color;
	}

	int getColor() {
		return _color;
	}

	virtual void addAdjNode(IGNode *node) { _adjNodeList.add(node); }

	virtual void removeAdjNode(IGNode *node) { _adjNodeList.remove(node); }

	virtual void resetAdjList() { _adjNodeList.removeAll(); }
	virtual void resetMCAdjList();

	virtual MyLinkedList<IGNode *>& getAdjNodeList() { return _adjNodeList; }

	virtual int getNoOfAdjNodes() { return _adjNodeList.numItems(); }

	virtual void print(ostream& out) { cout << _color; }
	void print() { print(cout); }

	friend ostream& operator<<(ostream& out, IGNode& n) {
		n.print(out);
		out << " (" << n.getNoOfAdjNodes() << " neighbours)";
		return out;
	}

	friend ostream& operator<<(ostream& out, IGNode* p) {
		p->print(out);
		out << " (" << p->getNoOfAdjNodes() << " neighbours)";
		return out;
	}
};

#endif	// __IGNODE_H__