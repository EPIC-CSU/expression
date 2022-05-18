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
// $Id: Loop.h,v 1.8 1998/03/21 01:19:02 ahalambi Exp $
//
// File:  		Loop.h
// Created:		Wed Jan 29, 97
// Last modified: 	Wed Jan 29, 97
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: include

#ifndef __LOOP_H__
#define __LOOP_H__

#include "GlobalIncludes.h"

#include "MyLinkedList.h"
#include "LinkedList.h"

#include "LoopList.h"

#include "PropertyList.h"
#include "BaseLoopProperty.h"

#include "BasicBlockNode.h"
   
#define _REDUCIBLE_ 0
#define _IRREDUCIBLE_ 1
   
class Loop
{
private:
  
#ifdef COUNT_LOOPS
  static int _numLoops;
#endif
   
protected:
  
  int _loopHeadID;
  MyLinkedList<int> _loopTailIDList;
  //
  // Basically, a loop is a strongly connected region. It has a
  // single head (the sink of all back-arcs) and may have many tails
  // (the source of the back-arcs).
  
  MyLinkedList<int> _bbIDList;
  
  LoopList _innerLoopList;
  
  int _type;
  //
  // Reducible or irreducible.
  // If a loop is reducible it has only one tail.
  
  PropertyList<BaseLoopProperty> _loopProps;
  
public:
  Loop();
  ~Loop();
  
#ifdef COUNT_LOOPS
  static int numLoops()
    {
      return _numLoops;
    }
#endif
  
  void setType(int typeNum);
  
  void setHeadID(int headBBID);
  int getHeadID();
  
  /*   void setTailIDList(MyLinkedList<int> *tailIDList)
       {
	_loopTailIDList = tailIDList;
	}
  */ 
  void addTailBBID(int tailID);
  
  int containsBB(int bbID);
  
  int hasTail(BasicBlockNode *bbPtr);
  
  MyLinkedListIterator<int> *createTailListIterForw();
  MyLinkedListIterator<int> *createTailListIterBack();
  
  void addBBToLoop(int bbID);

  /*   
       void setBBIDList(MyLinkedList<int> *bbList)
       {
       _bbIDList = bbList;
       }
       // Note that the owner of the list is now the loop object. So,
       // while this method is not recommended (for other users), if it is
       // being used, make sure that the list is in the heap and that the
       // caller does not delete it.
  */
  /*
    void deleteBBIDList()
    {
    delete _bbIDList;
    }	 
  */
  // This method provides a forward iterator over the list of basic
  // block IDs. Note that it is the responsibility of the caller to free
  // up the iterator.
  //
  MyLinkedListIterator<int> *createBBIDIterForw();
  MyLinkedListIterator<int> *createBBIDIterBack();
  
  int isReducible();
  int isIrreducible();
  
  void addInnerLoop(Loop *newLoop);
  void setInnerLoopNestLevel(int loopDepth);
  
  MyLinkedListIterator<Loop *> *innerLoopIterForw();
  MyLinkedListIterator<Loop *> *innerLoopIterBack();
  
  LoopList & getRefToInnerLoopList();
  
  int hasInnerLoop();
  int hasInnerLoop(BasicBlockNode *aPtr);
  
  void addProperty(BaseLoopProperty & lProp)
    {
      _loopProps.addProperty(lProp);
    }
  
  virtual BaseLoopProperty & getProperty(PropertyNames aName)
    {
	 return _loopProps.getProperty(aName);
    }
  
  virtual BaseLoopProperty *getPtrToProperty(PropertyNames aName)
    {
	 return _loopProps.getPtrToProperty(aName);
    }
  
  void print(int indent = 0);

  void printIBS();
};

#endif



