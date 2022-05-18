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
// File:  		MemDepProperty.h
// Created:		Fri, Sep 22, 00
// Last modified: 	Fri, Sep 22, 00
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: ARGUMENT
// Module: PROPERTY

#ifndef __MEMDEPPROPERTY_H__
#define __MEMDEPPROPERTY_H__

#include "BaseOpProperty.h"
#include "BaseOperation.h"

class MemDepProperty : public BaseOpProperty
{
 private:
 protected:
	OpList _depList;

	inline void _hasOp(BaseOperation *op)
	{
		OpListIter oi;
		
		for (oi = _depList.begin(); oi != _depList.end(); oi++)
		{
			if (*oi == op) return _YES_;
		}
		return _NO_;
	}
	
      
 public:
   MemDepProperty()
      {
      }
   
   MemDepProperty(OpList & ol) : _depList(ol)
      {
/*
	   OpListIter oi;
	   for (oi = ol.begin(); oi != ol.end(); oi++)
	   {
		   _depList.push_back(*oi);
	   }
*/
      }

   MemDepProperty(const MemDepProperty & b) : _depList(b._depList)
      {
      }

   virtual ~MemDepProperty()
      {
      }
   
   BaseArgProperty *copy()
   {
	   BaseArgProperty *retPtr = new MemDepProperty(*this);
	   return retptr;
   }

   OpList & getDepList() {return _depList;}

   void appendDep(BaseOperation *op)
   {
	   if ((op == NULL) || (_hasOp(op) == _YES_)) return;

	   _depList.push_back(op);
   }
  
   void clearAll()
   {
	   _depList.clear();
   }

   void removeDep(BaseOperation *op)
   {
	   if (op == NULL) return;

		OpListIter oi;
		
		for (oi = _depList.begin(); oi != _depList.end(); oi++)
		{
			if (*oi == op)
			{
				_depList.erase(oi);
				return;
			}
		}
   }

   int isMemDepProperty()
      {
	 return _YES_;
      }

   int is(PropertyNames aName)
      {
 	if (aName==_MEMDEP_)
	 return _YES_;
	else 
	 return _NO_;
      }

   void print(ostream& out) {};
};

#endif
