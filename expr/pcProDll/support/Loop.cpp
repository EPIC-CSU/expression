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
// $Id: Loop.cc,v 1.4 1998/03/21 01:16:32 ahalambi Exp $
//
// File:  		Loop.cc
// Created:		Mon Feb 03, 97
// Last modified: 	Mon Feb 03, 97
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: INCLUDE LOOP

#include "stdafx.h"

#include "Loop.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

#ifdef COUNT_LOOPS
   int Loop::_numLoops = 0;	
#endif

Loop::Loop()
{
  
  /*	 _loopTailIDList = new MyLinkedList<int>();
	 _bbIDList = new MyLinkedList<int>(); */
  
#ifdef COUNT_LOOPS
  _numLoops++;
#endif
}

Loop::~Loop()
{
  /*	 if (_loopTailIDList != NULL)
	 delete _loopTailIDList;
	 if (_bbIDList != NULL)
	 delete _bbIDList;
  */
#ifdef COUNT_LOOPS
  _numLoops--;
#endif
}
   
void Loop::setType(int typeNum)
{
  // Check if it is a valid type.
  //
  if ((typeNum == _REDUCIBLE_) || (typeNum == _IRREDUCIBLE_))
    {
      _type = typeNum;
    }
  // Maybe throw an exception here.
}

void Loop::setHeadID(int headBBID)
{
  _loopHeadID = headBBID;
}

int Loop::getHeadID()
{
  return _loopHeadID;
}

void Loop::addTailBBID(int tailID)
{
  _loopTailIDList.append(tailID);
}

int Loop::containsBB(int bbID)
{
  if (bbID == _loopHeadID) return _YES_;
  MyLinkedListIterator<int> *bbIter;
  bbIter = _bbIDList.elementsForward();
  int retVal = _NO_;
  while (bbIter->hasMoreElements())
    {
      if (bbIter->currentElement() == bbID)
	{
	  retVal = _YES_;
	  break;
	}
      bbIter->nextElement();
    }
  delete bbIter;
	 return retVal;
}
   
int Loop::hasTail(BasicBlockNode *bbPtr)
{
  if (bbPtr == NULL) return _NO_;
  int bbID = bbPtr->getDFSNum();
  
  MyLinkedListIterator<int> *tailIter;
  tailIter = _loopTailIDList.elementsForward();
  int retVal = _NO_;
  while (tailIter->hasMoreElements())
    {
      if (tailIter->currentElement() == bbID)
	{
	  retVal = _YES_;
	  break;
	}
      tailIter->nextElement();
    }
  delete tailIter;
  return retVal;
}

MyLinkedListIterator<int>* Loop::createTailListIterForw()
{
  MyLinkedListIterator<int> *retPtr;
  retPtr = _loopTailIDList.elementsForward();
  return retPtr;
}

MyLinkedListIterator<int>* Loop::createTailListIterBack()
{
  MyLinkedListIterator<int> *retPtr;
  retPtr = _loopTailIDList.elementsBackward();
  return retPtr;
}

void Loop::addBBToLoop(int bbID)
{
  _bbIDList.append(bbID);
}

// This method provides a forward iterator over the list of basic
// block IDs. Note that it is the responsibility of the caller to free
// up the iterator.
//
MyLinkedListIterator<int>* Loop::createBBIDIterForw()
{
  MyLinkedListIterator<int> *retPtr;
  retPtr = _bbIDList.elementsForward();
  return retPtr;
}

// This method provides a backward iterator over the list of basic
// block IDs. Note that it is the responsibility of the caller to free
// up the iterator.
//
MyLinkedListIterator<int>* Loop::createBBIDIterBack()
{
  MyLinkedListIterator<int> *retPtr;
  retPtr = _bbIDList.elementsBackward();
  return retPtr;
}

int Loop::isReducible()
{
  if (_type == _REDUCIBLE_) return _YES_;
  return _NO_;
}

int Loop::isIrreducible()
{
  if (_type == _IRREDUCIBLE_) return _YES_;
  return _NO_;
}
// As of now, these are the only uses of the _type data variable. If
// the number of types increases a new method will be needed to
// determine the type of the loop.

void Loop::addInnerLoop(Loop *newLoop)
{
  //
  // Note that this method does not account for the fact that
  // the depth of the input loop may be the same as that of the
  // loop it is being attached to. This method assumes that
  // the user knows the correct place to attach the loop.
  //
  
  _innerLoopList.addLoop(newLoop);
}

void Loop::setInnerLoopNestLevel(int loopDepth)
{
  // Note that I do not expect this method to be used under
  // normal circumstances. The loop depth is automatically set
  // whenever an innerloop is added (see LoopList.h).
  
  _innerLoopList.setLevelOfNesting(loopDepth);
}

MyLinkedListIterator<Loop *>* Loop::innerLoopIterForw()
{
  return (_innerLoopList.loopIterForw());
}

MyLinkedListIterator<Loop *>* Loop::innerLoopIterBack()
{
  return (_innerLoopList.loopIterBack());
}

LoopList & Loop::getRefToInnerLoopList()
{
  return _innerLoopList;
}

int Loop::hasInnerLoop()
{
  return (!_innerLoopList.isEmpty());
}

int Loop::hasInnerLoop(BasicBlockNode *aPtr)
{
  if (aPtr == NULL) return _NO_;
  if (_innerLoopList.isEmpty()) return _NO_;
  int bbID = aPtr->getDFSNum();
  
  int retVal = _NO_;
  MyLinkedListIterator<Loop *> *lIter;
  lIter = _innerLoopList.loopIterForw();
  Loop *curInnerLoop;
  
  while (lIter->hasMoreElements())
    {
      curInnerLoop = lIter->currentElement();
      if (curInnerLoop == NULL) break;
      if (curInnerLoop->containsBB(bbID))
	{
	  retVal = _YES_;
	  break;
	}
      lIter->nextElement();
    }
  delete lIter;
  return retVal;
}

void Loop::print(int indent)
{
	char a[20];
	for (int i = 0; i < indent; i++){
		a[i] = ' ';
	}
	a[indent] = '\0';

  cout << endl;
  cout << a << "Loop Head Id: " << _loopHeadID << endl << a << "Loop Tail Id: " ;
  
  /*	 if (_loopTailIDList == NULL)
	 return; */
  MyLinkedListIterator<int> *tailIter = _loopTailIDList.elementsForward(); 
  while (tailIter->hasMoreElements())
    {
      cout << " " << tailIter->currentElement() << " ";
      tailIter->nextElement();
    }
  
  /* 	 if (_bbIDList == NULL)
	 return; */
  MyLinkedListIterator<int> *bbIter = _bbIDList.elementsForward();
  cout << endl << a << "Loop Body: " ;
  while (bbIter->hasMoreElements())
    {
      cout << " " << bbIter->currentElement() << " ";
	  cout.flush();
      bbIter->nextElement();
    }
  cout << endl;
  _innerLoopList.print(indent + 2);
  
  delete tailIter; 
  delete bbIter;
}

void Loop::printIBS()
{
	BaseLoopProperty *prop = getPtrToProperty(_IBS_);

	if (prop != NULL)
	{
		prop->print(cout);
	}
}
