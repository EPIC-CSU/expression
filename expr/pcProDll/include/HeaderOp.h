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
// $Id: HeaderOp.h,v 1.7 1998/03/21 01:18:48 ahalambi Exp $
//
// File:  		HeaderOp.h
// Created:		Mon Oct 28, 96
// Last modified: 	Thu Oct 31, 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: OPERATION

#ifndef __HEADEROP_H__
#define __HEADEROP_H__

#include "BaseOperation.h"
#include "OpCodeTable.h"

// const NO_OP = 0;
//
// This has already been defined in OpCodeTable.h

#define DATA_OP  1
#define CONTROL_OP  2
#define FLOW_OP  3
#define _INVALID_  -1

class HeaderOp : public BaseOperation 
{
private:
 
#ifdef COUNT_HEADEROPS
 static int _numHeaderOps;	// See comment 1 below.
#endif
 
protected:
 
 int _index;
 int _type;
 
 // Should store location of first operation in the instruction
 // (array index) and the type of the operation (Data/Control/Flow).
 
public:
 HeaderOp(int index = _INVALID_, int type = NO_OP): _index(index),
  _type(type)
 {
#ifdef COUNT_HEADEROPS
	 _numHeaderOps++;	
#endif
 }
 
 virtual ~HeaderOp()
 {
#ifdef COUNT_HEADEROPS
	 _numHeaderOps--;
#endif
 } 
 
#ifdef COUNT_HEADEROPS
 static int numHeaderOps()
 {
	 return _numHeaderOps;
 }
#endif
 
 void initializeID()
 {
 }
 
 void setIndex(int someIndex)
 {
	 _index = someIndex;
 }
 
 void setType(int someType)
 {
	 _type = someType;
 }
 
 int getIndex()
 {
	 if (_index == _INVALID_)
  {
   // throw exception
  }
	 return _index;
 }
 
 int getType()
 {
	 if ((_type < DATA_OP) || (_type > FLOW_OP))
  {
   // throw exception
  }
	 return _type;
 }
 
 BaseOperation *copy();
 
 HeaderOp & operator = (const HeaderOp & b)
 {
	 _type = b._type;
	 _index = b._index;
	 return *this;
 }
 
 int isHeaderOp()
 {
	 return _YES_;
 }
};

// This is not an abstract class. The initializeId method will have to
// be redefined here.  
#endif

// Comment 1:
//
// _numHeaderOps is used to keep a count of the number of HeaderOp objects
// in existence at any point. COUNT_HEADEROPS can be either defined or
// undefined in the file "OperationIncludes.h". The count information
// will be used for debugging (and/or statistical) purposes. It might
// be a good idea to allow the user to specify whether he needs this
// info. 
