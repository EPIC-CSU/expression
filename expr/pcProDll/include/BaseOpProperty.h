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
// $Id: BaseOpProperty.h,v 1.10 1998/02/27 22:54:17 xji Exp $
//
// File:  		BaseOpProperty.h
// Created:		Fri Nov 22, 96
// Last modified: 	Fri Nov 22, 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: OPERATION, PROPERTY

#ifndef __BASEOPPROPERTY_H__
#define __BASEOPPROPERTY_H__

#include "OperationIncludes.h"

class BaseOpProperty
{
 private:
 protected:
 public:
//   List of Properties =  {SSA, PART_OF, DELETE, BRANCH, CLUSTER, PREDICATE, INSTR_TYPE, MEMDEP, SCHEDULEOP};
      
   BaseOpProperty()
      {
      }
   
   BaseOpProperty(const BaseOpProperty & someOpProperty)
      {
      }
   
   virtual ~BaseOpProperty()
      {
      }
      
   virtual BaseOpProperty *copy() const = 0;

   virtual void print(ostream&) = 0;
   
   friend const int operator == (const BaseOpProperty & a,
				 const BaseOpProperty & b)
      {
	 return _YES_;
      }
   
   friend ostream & operator << (ostream & out, const BaseOpProperty & s)
      {
	 return out;
      }
   
   static int isValidProperty(PropertyNames aName)
      {
	      if (aName == _SSA_ || 
           aName == _PART_OF_ || 
           aName == _DELETE_ || 
           aName == _BRANCH_ || 
           aName == _CLUSTER_ ||
           aName == _PREDICATE_ ||
           aName == _INSTR_TYPE_ ||
		   aName == _MEMDEP_ ||
		   aName == _SCHEDULEOP_
           )
	    return _YES_;
	 return _NO_;
      }
   	 
   virtual int is(PropertyNames aName)
      {
	 return _NO_;
      }

   virtual int isEq(BaseOpProperty* a) {
      return _NO_;
   } 
};

#endif


