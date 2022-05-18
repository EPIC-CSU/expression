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
// $Id: BasicBlockPredecessors.h,v 1.6 1998/03/21 01:18:29 ahalambi Exp $
//
// File:  		BasicBlockPredecessors.h
// Created:		Thu Jan 23, 97
// Last modified: 	Thu Jan 23, 97
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: include

#ifndef __BASICBLOCKPREDECESSORS_H__
#define __BASICBLOCKPREDECESSORS_H__

#include "GlobalIncludes.h"
#include "MyLinkedList.h"

class BasicBlockNode;

class BasicBlockPredecessors
{
 private:
 protected:
   
   MyLinkedList<BasicBlockNode *> *_predList;
   
 public:
   BasicBlockPredecessors()
      {
	 _predList = NULL;
      }
   
   ~BasicBlockPredecessors()
      {
      }
      
   // This method will be called by a method in the ControlFlowGraph class. 
   // That method will first create the list of predecessors for a
   // particular basic block and then use this method to initialize
   // the _dataFlowPredecessors data of the BasicBlockNode class.
   //
   void setPredList(MyLinkedList<BasicBlockNode *> *somePredList)
      {
	 _predList = somePredList;
      }
   
   void deletePredList()
      {
	 delete _predList;
	 _predList = NULL;
      }
   
   BasicBlockNode *getFirstPred();
   BasicBlockNode *getLastPred();

   void addPred(BasicBlockNode *somePred);
   void addUniqPred(BasicBlockNode *somePred);

   void removePred(BasicBlockNode *somePred);

   int numItems() { return ((_predList != NULL) ? _predList->numItems() : -1); }

   // The next two methods are the most useful methods of this class.
   //
   MyLinkedListIterator<BasicBlockNode *> *createPredListIterForw();
   MyLinkedListIterator<BasicBlockNode *> *createPredListIterBack();
   
   friend ostream & operator << (ostream & out, const BasicBlockPredecessors & s);

};

#endif
