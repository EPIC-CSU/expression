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
// $Id: ReverseMap.h,v 1.1 1998/03/27 00:15:02 ahalambi Exp $
//
// File:  		ReverseMap.h
// Created:		Mon Feb 23, 98
// Last modified: 	Mon Feb 23, 98
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module:

#ifndef __REVERSEMAP_H__
#define __REVERSEMAP_H__

#include "GlobalIncludes.h"

typedef BaseHTGNode *HTGNodePtr;
typedef BasicBlockNode *BBNodePtr;

class ReverseMap
{
 private:

   Array<HTGNodePtr> _bbToHTGMap;
   
 public:
   ReverseMap()
      {
      }
   
   ~ReverseMap()
      {
      }
   
   HTGNodePtr up(BBNodePtr a)
      {
	 try
	    {
	       return (_bbToHTGMap.elementAt(a->getDFSNum()));
	    }
	 catch (Exception *e)
	    {
	       cout << "Panic: Reverse map for BasicBlockNode does not exist" << endl;
	       assert(0);
	    }
      }
   
   void setReverseForBBNode(BBNodePtr a)
      {
	 try
	    {
	       _bbToHTGMap.setElementAt(a->getDFSNum());
	    }
	 catch (Exception *e)
	    {
	       cout << "Panic: Could not set reverse pointer for BasicBlockNode" << endl;
	       assert(0);
	    }
   }
};

#endif