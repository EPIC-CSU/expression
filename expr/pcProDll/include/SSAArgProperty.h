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
// $Id: SSAArgProperty.h,v 1.6 1998/01/22 23:59:38 pgrun Exp $
//
// File:  		SSAArgProperty.h
// Created:		Fri Nov 15 , 96
// Last modified: 	Fri Nov 15 , 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: ARGUMENT
// Module: PROPERTY

#ifndef __SSAARGPROPERTY_H__
#define __SSAARGPROPERTY_H__

#include "BaseArgProperty.h"

class SSAArgProperty : public BaseArgProperty
{
private:
protected:
 
 int _ssaCopyNum;
 
public:
 SSAArgProperty()
 {
  _ssaCopyNum = -1;
 }
 
 SSAArgProperty(int someNum) : _ssaCopyNum(someNum)
 {
 }
 
 virtual ~SSAArgProperty(){ }
 
 void setSSANum(int id)
 {
  _ssaCopyNum = id;
 }
 
 int getSSANum()
 {
  return _ssaCopyNum;
 }
 
 
 BaseArgProperty *copy();

 //
 // copy is a duplication method. It just makes another copy of the
 // existing SSAArgProperty object.
 
 void assignCopyNum(const char *regName)
 {
	 // Given the name of the register, come up with the copy
	 // number and assign it to _ssaCopyNum.
	 // Here's how I plan to do it
  // When the source program is being parsed, we create a table
  // which stores each variable. This table allows us to store
  // a property object with each table entry. When we are
  // computing SSA numbers, we store the latest copy number
  // associated with each variable as a property of the
  // corresponding table entry. Then, all this function has to
  // do is perform a table lookup, and increment the number
  // stored in the property object.
  // One point to note
  // The annotation to the table need not be in the form of
  // property objects; they can be just integers.
  // Right now, the variables table will only allow integers to
  // be annotated. Later on, change it if needed.
  // (This is a short-cut to save memory space; if we really
  // want to make this code "experimentable", we should make
  // the annotations property objects.)
  
 }
 
 const int ssaCopyNum()
 {
  return _ssaCopyNum;
 }
 
 int isSSAArgProperty()
 {
	 return _YES_;
 }
 
 int is(PropertyNames aName)
 {
 	if (aName==_SSA_)
   return _YES_;
  else 
   return _NO_;
 }
 
 void print(ostream& out)
 {
  out << " SSA: " << _ssaCopyNum << " ";
 }
 
 friend int operator == (const SSAArgProperty & a,
  const SSAArgProperty & b)
 {
	 return _YES_;
 }
 
 friend ostream& operator << (ostream & out, const SSAArgProperty & s) 
 {
  out << " SSA: " << s._ssaCopyNum;
	 return out;
 }
 //
 // Rewrite the operators == and << later.
 
};

#endif

