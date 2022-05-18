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
// File:  		RegClassArgProperty.h
// Created:		Fri Aug 03 ,01
// Last modified: 	Fri Aug 03 ,01
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: INSTRUCTION
// Module: PROPERTY

#ifndef __REGCLASSARGPROPERTY_H__
#define __REGCLASSARGPROPERTY_H__

#include "BaseArgProperty.h"
#include "STLIncludes.h"
#include "RegArchModel.h"

class RegClassArgProperty : public BaseArgProperty
{
private:
	Vector<RegClassEntryNode2> _regClasses;

public:
	RegClassArgProperty() { }
	RegClassArgProperty(const RegClassArgProperty & b) : _regClasses(b._regClasses) {}
	virtual ~RegClassArgProperty() { }

	void addClass(RegClassEntryNode2 & cls)
	{
		_regClasses.push_back(cls);
	}

	Vector<RegClassEntryNode2> & getClasses()
	{
		return _regClasses;
	}

	BaseArgProperty *copy()
	{
		BaseArgProperty *retPtr = new RegClassArgProperty(*this);
		return retPtr;
	}
	
	int isRegClassArgProperty()
	{
		return _YES_;
	}
	
	int is(PropertyNames aName)
	{
		if (aName==_REGCLASS_)
			return _YES_;
		else 
			return _NO_;
	}
	
	void print(ostream& out)
	{
		out << " RegClasses: ";
		// TBD.
	}
};

#endif
