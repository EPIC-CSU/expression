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
// $Id: BaseRoutineProperty.h,v 1.3 1998/01/22 23:58:06 pgrun Exp $
//
// File:  		BaseRoutineProperty.h
// Created:		Wed Nov 05, 97
// Last modified: 	Wed Nov 05, 97
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: ROUTINE, PROPERTY

#ifndef __BASEROUTINEPROPERTY_H__
#define __BASEROUTINEPROPERTY_H__

#include "PropertyIncludes.h"

class BaseRoutineProperty
{
private:
protected:
public:
	//   List of Properties =  { }
	
	BaseRoutineProperty()
	{
	}
	
	BaseRoutineProperty(const BaseRoutineProperty & someRProperty)
	{
	}
	
	virtual ~BaseRoutineProperty()
	{
	}
	
	virtual BaseRoutineProperty *copy() const = 0;
	
	virtual void print(ostream&) = 0;
	
	friend const int operator == (const BaseRoutineProperty & a,
		const BaseRoutineProperty & b)
	{
		return _YES_;
	}
	
	friend ostream & operator << (ostream & out, const BaseRoutineProperty & s)
	{
		return out;
	}
	
	static int isValidProperty(PropertyNames aName)
	{
		switch (aName) {
		case _G_TO_T_REG_MAP_:
			return _YES_;
		case _ROUTPARM_:
			return _YES_;
		case _STACKROUT_:
			return _YES_;
		}
		return _NO_;
	}
	
	virtual int is(PropertyNames aName)
	{
		return _NO_;
	}
	
	virtual int isEq(BaseRoutineProperty* a) {
		return _NO_;
	}
};

#endif


