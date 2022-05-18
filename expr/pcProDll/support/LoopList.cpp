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
#include "stdafx.h"

#include "LoopList.h"
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

LoopList::LoopList()
{
  _listOfLoops = _NULL_;
}

LoopList::~LoopList()
{
}

int LoopList::isEmpty()
{
  if (_listOfLoops == NULL) return _YES_;
  return _NO_;
}

void LoopList::addLoop(Loop *someLoop)
{
  if (_listOfLoops == _NULL_)
    {
      _listOfLoops = new MyLinkedList<Loop *>();
    }
  
  _listOfLoops->append(someLoop);
  //
  // The list should be a possesive list (i.e. it should not be
  // a copy list). In that case the pointer would have to be
  // passed to append (instead of the object itself).
  
  //	 int innerLoopDepth = _loopDepth + 1;
  
  //	 (someLoop->_innerLoopList).setLevelOfNesting(innerLoopDepth);
  
  // Note that we are also setting the nest level of the inner
	 // loop when we add this loop.
  
  // Imp -
  //       I have commented out the portion that sets the nest level of
  //       the inner loop.
  
}

MyLinkedListIterator<Loop *>* LoopList::loopIterForw()
{
  MyLinkedListIterator<Loop *> *retPtr;
  if (_listOfLoops==_NULL_)
    return _NULL_;
  retPtr = _listOfLoops->elementsForward();
  return retPtr;
}

MyLinkedListIterator<Loop *>* LoopList::loopIterBack()
{
  MyLinkedListIterator<Loop *> *retPtr;
  if (_listOfLoops==_NULL_)
    return _NULL_;
  retPtr = _listOfLoops->elementsBackward();
  return retPtr;
}

void LoopList::print(int indent)
{
	char a[20];
	for (int i = 0; i < indent; i++){
		a[i] = ' ';
	}
	a[indent] = '\0';


   if (_listOfLoops != _NULL_)
   {
	cout << a << " ***** Printing Loop List for Loop Depth : " << _loopDepth << " *********";

	MyLinkedListIterator<Loop *> *iterPtr;
	iterPtr = _listOfLoops->elementsForward();
	while (iterPtr->hasMoreElements())
	{
		(iterPtr->currentElement())->print(indent + 2);
		iterPtr->nextElement();
    }
	delete iterPtr;
	
	cout << a << " **** End Loop Depth : " << _loopDepth << " ******* " << endl;
   }
}

