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
// File:  		CodeLocProperty.h
// Created:		Mon Jan 17 ,00
// Last modified: 	Mon Jan 17 ,00
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: INSTRUCTION
// Module: PROPERTY

#ifndef __CODELOCPROPERTY_H__
#define __CODELOCPROPERTY_H__

#include "BaseInstrProperty.h"

class CodeLocProperty : public BaseInstrProperty
{
private:
	int _loc;
public:
	CodeLocProperty() : _loc(0) { }
	CodeLocProperty(int loc) : _loc(loc) { }
	CodeLocProperty(const CodeLocProperty & a) : _loc(a._loc) { }
	virtual ~CodeLocProperty() { }

	virtual int is(PropertyNames aName)
	{
		return ((aName == _CODELOC_) ? 1 : 0);
	}

	virtual BaseInstrProperty *copy() 
	{
		BaseInstrProperty *retPtr;
		retPtr = new CodeLocProperty(*this);

		return retPtr;
	}

	void setLoc(int loc) { _loc = loc; }
	int getLoc() { return _loc; }

	virtual void print(ostream & out)
	{
		out << "Code Loc: " << _loc << endl;
	}

	virtual int isEq(BaseInstrProperty *a)
	{
		if (a == NULL) return _NO_;
		if (!(a->is(_CODELOC_))) return _NO_;

		if (((CodeLocProperty *)a)->getLoc() == _loc) return _YES_;

		return _NO_;
	}
};

#endif
