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
// $Id: PredidArgProperty.h,v 1.5 1998/01/22 23:59:25 pgrun Exp $
//
// File:  		PredidArgProperty.h
// Created:		Fri Nov 15 , 96
// Last modified: 	Fri Nov 15 , 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: ARGUMENT
// Module: PROPERTY

#ifndef __PREDIDARGPROPERTY_H__
#define __PREDIDARGPROPERTY_H__

#include "BaseArgProperty.h"

class PredidArgProperty : public BaseArgProperty
{
 private:
 protected:
   
   int _predID;
   
 public:
   PredidArgProperty()
      {
	 _predID = 0;
      }
   
   PredidArgProperty(int id)
      {
	 _predID = id;
      }

   virtual ~PredidArgProperty()
      {
      }
   
   BaseArgProperty *copy();

   void setPredID(int dfsNum)
   {
   _predID = dfsNum;
   }

   int getPredID()
      {
         return _predID;
      }
   
   int isPredidArgProperty()
      {
	 return _YES_;
      }

   int is(PropertyNames aName)
      {
 	if (aName==_PREDID_)
	 return _YES_;
	else 
	 return _NO_;
      }

   void print(ostream& out) {};
};

#endif
