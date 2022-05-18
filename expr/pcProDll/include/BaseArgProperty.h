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
// $Id: BaseArgProperty.h,v 1.8 1998/03/21 01:18:21 ahalambi Exp $
//
// File:  		BaseArgProperty.h
// Created:		Fri Nov 15 , 96
// Last modified: 	Fri Nov 15 , 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: ARGUMENT
// Module: PROPERTY

#ifndef __BASEARGPROPERTY_H__
#define __BASEARGPROPERTY_H__

#include "PropertyIncludes.h"

class BaseArgProperty
{
private:
protected:
public:
	BaseArgProperty()
	{
	}
	
	virtual ~BaseArgProperty()
	{
	}
	
	virtual int isSSAArgProperty()
	{
		return _NO_;
	}
	
	virtual int isPredProperty()
	{
		return _NO_;
	}
	
	virtual int isPredidArgProperty()
	{
		return _NO_;
	}
	
	virtual int isDoubleArgProperty()
	{
		return _NO_;
	}

	virtual int isRegClassArgProperty()
	{
		return _NO_;
	}

	virtual BaseArgProperty *copy() = 0;
	//
	// We have defined the copy method to be a virtual method, and we
	// do not expect to ever create a Base class object explicitely (or
	// to make a copy). Hence, this function has no meaning. By writing
	// it this way, we are explicitely declaring BaseArgProperty to be
	// an abstract class.
	// All derived classes (which will be created explicitely) should
	// redefine this function.
	
	
	static int isValidProperty(PropertyNames aName)
	{
		// Argument properties can only be of certain types. 
		// Return true if the enum aName is one of those types.
		
		switch (aName)
		{
		case _SSA_:
		case _PRED_:
		case _PREDID_:
		case _UDCHAIN_:
		case _DUCHAIN_:
		case _MULTICHAIN_:
		case _CNSTPROP_:
		case _REGORDER_:
		case _CALLNAME_:
		case _RA_:
		case _HLINFO_:
		case _G_TO_T_REG_MAP_:
		case _DOUBLEARG_:
		case _REGCLASS_:
				break;
		default:
			return _NO_;
		}
		return _YES_;
	}
	
	virtual int is(PropertyNames aName)
	{
		return _NO_;
	}
	
	virtual void print(ostream&) = 0;
	
	friend int operator == (const BaseArgProperty & a, const
		BaseArgProperty & b)
	{
		return _YES_;
	}
	//
	// As of now, this class has no member data to compare equivalency.
	
	friend ostream & operator << (ostream & out, const BaseArgProperty & s)
	{
		return out;
	}
	//
	// Once again, no member data.
	// I am not sure if this function is needed here, because this is
	// an abstract class. 
	
	virtual int isEq(BaseArgProperty* a) {
		return _NO_;
	} 
	
};

#endif
