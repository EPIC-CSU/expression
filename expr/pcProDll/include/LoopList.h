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
// $Id: LoopList.h,v 1.5 1998/03/21 01:19:04 ahalambi Exp $
//
// File:  		LoopList.h
// Created:		Mon Feb 03, 97
// Last modified: 	Mon Jan 03, 97
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: include

#ifndef __LOOPLIST_H__
#define __LOOPLIST_H__

#include "GlobalIncludes.h"
#include "MyLinkedList.h"

class Loop;

class LoopList
{

 private:
 protected:
   int _loopDepth;
   MyLinkedList<Loop *> *_listOfLoops;
 
 public:
   LoopList();
   ~LoopList();
   
   void setLevelOfNesting(int nestLevel)
      {
	 _loopDepth = nestLevel;
      }
   
   int getLevelOfNesting()
      {
	 return _loopDepth;
      }

   int isEmpty();
   
   void addListOfLoops(MyLinkedList<Loop *> *someLoopList)
      {
	 _listOfLoops = someLoopList;
      }
   
   void addLoop(Loop *someLoop);
   
   void deleteListOfLoops()
      {
	 delete _listOfLoops;
	 _listOfLoops = NULL;
      }
   
   MyLinkedListIterator<Loop *> *loopIterForw();
   MyLinkedListIterator<Loop *> *loopIterBack();
   
   void print(int indent = 0);
   
};

#endif   

