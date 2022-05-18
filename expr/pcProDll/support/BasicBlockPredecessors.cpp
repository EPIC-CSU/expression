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
// $Id: BasicBlockPredecessors.cc,v 1.1 1998/03/21 01:16:06 ahalambi Exp $
//
// File:  		BasicBlockPredecessors.cc
// Created:		Wed Mar 11, 98
// Last modified: 	Wed Mar 11, 98
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: SUPPORT

#include "stdafx.h"

#include "BasicBlockPredecessors.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

BasicBlockNode* BasicBlockPredecessors::getFirstPred()
{
   if (_predList == _NULL_)
      return NULL;
   
   return _predList->getHead();
   
   // What happens if the list is NULL ?
   // This means that there is no predecessor to this
   // basic block (i.e. start node). The user is supposed to
   // check that this does not occur. If this does indeed occur,
   // then, the LinkedList class signals an exception.
      
}
//
// Note that there is no order among predecessors. i.e. there is no
// special significane to getFirstPred. (An arbitrary predecessor
// may also be returned).

BasicBlockNode* BasicBlockPredecessors::getLastPred()
{
   if (_predList == _NULL_)
      return NULL;
   
   return _predList->getTail();
}
//
// Similar to getFirstPred.
   
void BasicBlockPredecessors::addPred(BasicBlockNode *somePred)
{
   if (_predList == _NULL_)
      {
	 _predList = new MyLinkedList<BasicBlockNode *>();
      }
   _predList->append(somePred);
}

void BasicBlockPredecessors::addUniqPred(BasicBlockNode *somePred)
{
   if (_predList == _NULL_)
      {
	 _predList = new MyLinkedList<BasicBlockNode *>();
      }
	 _predList->add(somePred);
}


void BasicBlockPredecessors::removePred(BasicBlockNode *somePred)
{
   if ( _predList ) 
      _predList->remove(somePred);
}


// The next two methods are the most useful methods of this class.
//
MyLinkedListIterator<BasicBlockNode *>* BasicBlockPredecessors::createPredListIterForw()
{
   if (_predList == _NULL_) return NULL;
   
   MyLinkedListIterator<BasicBlockNode *> *retPtr;
   retPtr = _predList->elementsForward();
   return retPtr;
}

MyLinkedListIterator<BasicBlockNode *>* BasicBlockPredecessors::createPredListIterBack()
{
   if (_predList == _NULL_) return NULL;
   
   MyLinkedListIterator<BasicBlockNode *> *retPtr;
   retPtr = _predList->elementsBackward();
   return retPtr;
}

ostream& operator << (ostream & out, const BasicBlockPredecessors & s)
{
   MyLinkedListIterator<BasicBlockNode *> *predIter;

   // cout has a problem.
   // so, put an endl at regular intervals

   int i = 0;
   if (s._predList != NULL)
      {
	 predIter = (s._predList)->elementsForward();
	 while (predIter->hasMoreElements())
	    {
	       out << "\tP: " << predIter->currentElement();
	       predIter->nextElement();

		   // This is to solve the buffer overflow problem.
		   i++;
		   if (i %5 == 0) out << endl;
	    }
	 delete predIter;
	 out << endl;
      }
   else
      {
	 out << "BB has no predecessors" << endl;
      }
	 return out;
}
