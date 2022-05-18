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
//: RegNode.h
//
// File:  		RegNode.h
// Created:		Fri Feb 07, 1999
// Author:		Radu Cornea
// Email:		radu@ics.uci.edu, ilp@ics.uci.edu
//
// A RegNode represent a real register in the Interference Graph.
// 

#ifndef __REGNODE_H__
#define __REGNODE_H__

#include "IGNode.h"

class RegNode : public IGNode {
	//char *_rfName;
	int _rfID;
	int _regNum;

public:
	RegNode(int rfID, int regNum) : _rfID(rfID), _regNum(regNum) {}
	~RegNode() {}

	bool isReg() { return true; }

	int getRFID() { return _rfID; }

	int getNum() { return _regNum; }

	int getIndex() { return _rfID + _regNum; }

	void print(ostream& out);
};

#endif	// __REGNODE_H__