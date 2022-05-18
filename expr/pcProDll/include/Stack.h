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
// $Id: Stack.h,v 1.3 1998/01/22 23:59:42 pgrun Exp $
//
// File:  		Stack.h
// Created:		Fri Mar 21, 97
// Last modified: 	Fri Mar 21, 97
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: include

#ifndef __STACK_H__
#define __STACK_H__

#ifndef WIN32
#include <iostream.h>
#endif

#include "GlobalIncludes.h"
#include "Array.h"

template <class T> class Stack
{
private:
#ifdef LEAKY
 //NOS: used for memory leaks
 char tag[MAX_PATH];
#endif
 
 
protected:
 
 Array<T> _dataStack;
 int _top;
 int _arraySize;
 
public:
 
 Stack()
 {
#ifdef LEAKY
  {
   //NOS
   char t[MAX_PATH];
   GetFileTitle(__FILE__,t,MAX_PATH); 
   sprintf(tag,"%d:%s",__LINE__,t);
  }
#endif
  
  _top = 0;
  _arraySize = 0;
 }
 
 ~Stack()
 {
  _dataStack.removeAllElements();
 }
 
 int isEmpty()
 {
  if (_top == 0)
  {
   return _YES_;
  }
  return _NO_;
 }
 
 void push(const T & obj)
 {
  if (_top == _arraySize)
  {
   _dataStack.appendElement(obj);
   _arraySize++;
  }
  else
  {
   try
   {
    _dataStack.setElementAt(obj, _top);
   }
   catch (Exception *n)
   {
    cout << "Wrong stack management" << (*n)  << endl;
   }
  }
  
  _top++;
 }
 
 T & top()
 {
  if (_top <= 0)
  {
   throw (Exception *)
    new NoSuchElementException("Empty Stack");
  }
  return _dataStack.elementAt(_top - 1);
 }
 
 void pop()
 {
  if (_top <= 0)
  {
   throw (Exception *)
    new NoSuchElementException("Empty Stack");
  }
  
  _top--;
  //
  // Note that we are not explicitly removing the element.
  // It does not matter because when push is called after a pop
  // (i.e. an insert is done at a location where an element
  // exists) the array method, setElementAt(), removes the
  // previous object.
 }
 
 // Use the next function if the size of the stack has grown a lot
 // and then shrunk. This is basically a memory saving device,
 // because I do not explicitely delete the memory space whenever
 // pop is called. Basically, if the user is very pressed for space,
 // he can call reSize whenever he calls pop, or he can determine
 // when to reSize (usually after a large number of pops)
 //
 void reSize()
 {
  _dataStack.setSize(_top);
 }
 
 void reSize(int i)
 {
  if (i < _top)
  {
   cout << "Warning: wrong reSize parameter for Stack" <<
    endl;
   return;
  }
  _dataStack.setSize(i);
 }
 
 
 
};

#endif
