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
// File:  		DoubleArgProperty.h
// Created:		Fri, Oct 13, 00
// Last modified: 	Fri, Oct 13, 00
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: ARGUMENT
// Module: PROPERTY

#ifndef __DOUBLEARGPROPERTY_H__
#define __DOUBLEARGPROPERTY_H__

#include "BaseArgProperty.h"

const int DOUBLE_FIRST = 363;
const int DOUBLE_SECOND = 484;

class DoubleArgProperty : public BaseArgProperty
{
 private:
 protected:
   
   BaseArgument *_siblingArg;
   int _type; // is it the first or the second argument of the double
   
 public:
	 DoubleArgProperty() : _siblingArg(NULL), _type(_INVALID_)
	 {
	 }
	 
	 DoubleArgProperty(BaseArgument *arg, int type = _INVALID_) : _siblingArg(arg), _type(type)
	 {
	 }
	 
	 DoubleArgProperty(const DoubleArgProperty & a) : _siblingArg(a._siblingArg), _type(a._type)
	 {
	 }
	 
	 virtual ~DoubleArgProperty()
	 {
	 }
	 
	 BaseArgProperty *copy()
	 {
		 BaseArgProperty *retPtr;
		 
		 retPtr = new DoubleArgProperty(*this);
		 return retPtr;
	 }
	 
	 void setSiblingArg(BaseArgument *arg)
	 {
		 _siblingArg = arg;
	 }
	 
	 BaseArgument *getSiblingArg()
	 {
         return _siblingArg;
	 }

	 void setType(int type) { _type = type; }
	 int getType() { return _type; }

	 int isDouble1Type() 
	 {
		 return ((_type == DOUBLE_FIRST) ? _YES_ : _NO_); 
	 }
	 
	 int isDouble2Type() 
	 { 
		 return ((_type == DOUBLE_SECOND) ? _YES_ : _NO_); 
	 }

	 int isDoubleArgProperty()
	 {
		 return _YES_;
	 }
	 
	 int is(PropertyNames aName)
	 {
		 if (aName==_DOUBLEARG_)
			 return _YES_;
		 else 
			 return _NO_;
	 }

	 int isEq(BaseArgProperty* a) 
	 {
		 if ( a->is(_DOUBLEARG_) ) {
			 return ( ((DoubleArgProperty*)a)->_siblingArg == _siblingArg);
		 }
		 return _NO_;
	 }
	 
	 void print(ostream& out) {};
};

#endif