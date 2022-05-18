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
//$Id: TrackStack.h,v 1.3 1998/01/22 23:59:46 pgrun Exp $
//
// File:  		TrackStack.h
// Created:		Wed Jan 29, 97
// Last modified: 	Wed Jan 29, 97
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: include

#ifndef __TRACKSTACK_H__
#define __TRACKSTACK_H__

//#include "Routine.h"
#include "GlobalIncludes.h"
#include "MyStack.h"

class Routine;
extern Routine *currRoutine;

class TrackStack
{
 private:
   
   MyStack<Loop *> _loopTracker;
   
 public:
   
   TrackStack()
      {
      }
   
   ~TrackStack()
      {
      }
   
   Loop *getLoop(BasicBlockNode *aPtr)
   {
 		Loop *outerLoop, *retPtr;
	 
 		MyLinkedListIterator<Loop *> *loopIter;
 	 
		int TEMP_DEBUG = 0;

		 if (_loopTracker.isEmpty())
		 {
		    loopIter = (currRoutine->getLoopList()).loopIterForw();
		 }
		 else if ((outerLoop = _loopTracker.top()) != NULL)
		 {
			loopIter = outerLoop->innerLoopIterForw();
		 }
		 else // error condition.
		 {
		 	   return NULL;
		 }
	 
		retPtr = NULL;
	 
		if (loopIter == NULL)
		{
			 // Error condition.
			 cout << "Panic: Could not find a loop DS corresponding to this basic-block:" << endl;
			 cout << "\t" << " (* " << aPtr << " *), DFS = " << aPtr->getDFSNum() << endl;
			assert(0);
		}

		int loopHeadID;
		while (loopIter->hasMoreElements())
		{
			loopHeadID = (loopIter->currentElement())->getHeadID();
			if (loopHeadID == aPtr->getDFSNum())
			{
				 retPtr = loopIter->currentElement();
				break;
			}
			loopIter->nextElement();
		}
	 
		delete loopIter;
	 
		if (retPtr != NULL)
		{
				_loopTracker.push(retPtr);
		}

		if (TEMP_DEBUG)
		{
			cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
			cout << "Found Loop for: (* " << aPtr << " *), DFS = " << loopHeadID << endl;
			cout << "-----------------------------------------------------" << endl;
		}

		return retPtr;
   }	 

   void popLoop()
      {
	 if (!_loopTracker.isEmpty())
	    _loopTracker.pop();
      }
   
};

#endif
