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
//: RAProperty.h
//
// File:  		RAProperty.h
// Created:		Fri Jan 28, 2000
// Author:		Radu Cornea
// Email:		radu@ics.uci.edu, ilp@ics.uci.edu
//
// RAProperty stores the register number after register allocation
// 

#ifndef __RAPROPERTY_H__
#define __RAPROPERTY_H__

#include "BaseArgProperty.h"

class RAProperty : public BaseArgProperty {
	int _regID;

public:
	RAProperty() :_regID(0) {}
	RAProperty(int regID) : _regID(regID) {}
	virtual ~RAProperty() {}

	BaseArgProperty* copy() { return new RAProperty(*this); }

	int getRegID() { return _regID; }
	void setRegID(int regID) { _regID = regID; }

	int is(PropertyNames name) { if (name==_RA_) return _YES_; return _NO_; }
 
	void print() { cout << _regID; }
	void print(ostream& out) { out << _regID; }

	friend ostream& operator<<(ostream& out, RAProperty& raProp) {
		out << raProp.getRegID();
		return out;
	}
};

#endif // __RAPROPERTY_H__