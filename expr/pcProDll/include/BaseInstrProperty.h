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
// $Id: BaseInstrProperty.h,v 1.7 1998/02/27 22:54:16 xji Exp $
//
// File:  		BaseInstrProperty.h
// Created:		Fri Nov 15 , 96
// Last modified: 	Fri Nov 15 , 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: INSTRUCTION
// Module: PROPERTY

#ifndef __BASEINSTRPROPERTY_H__
#define __BASEINSTRPROPERTY_H__

#include "PropertyIncludes.h"

class BaseInstrProperty
{
 private:
 protected:
 public:
   BaseInstrProperty()
      {
      }
   
   virtual ~BaseInstrProperty()
      {
      }
  
/* All functions that are declared in the super classes must be first
   declared as virtual functions here.
   Below is an example.
 
   virtual int isSSAInstrProperty()
      {
	 return _NO_;
      }
   
*/   
   virtual BaseInstrProperty *copy() = 0;
   //
   // We have defined the copy method to be a virtual method, and we
   // do not expect to ever create a Base class object explicitely (or
   // to make a copy). Hence, this function has no meaning. By writing
   // it this way, we are explicitely declaring BaseInstrProperty to be
   // an abstract class.
   // All derived classes (which will be created explicitely) should
   // redefine this function.
   
   static int isValidProperty(PropertyNames aName)
      {
         // Instrument properties can only be of certain types. 
         // Return true if the enum aName is one of those types.

	 switch (aName)
	    {
	     case _REGPRESSURE_:
		 case _CODELOC_:
		 case _SCHEDULEINSTR_:
		 case _LIVEDEADINSTR_:
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
   
   friend int operator == (const BaseInstrProperty & a, const
			   BaseInstrProperty & b)
      {
	 return _YES_;
      }
   //
   // As of now, this class has no member data to compare equivalency.

   friend ostream & operator << (ostream & out, const BaseInstrProperty & s)
	 {
	    return out;
	 }
   //
   // Once again, no member data.
   // I am not sure if this function is needed here, because this is
   // an abstract class. 

   virtual int isEq(BaseInstrProperty* a) {
      return _NO_;
   } 
};

#endif
