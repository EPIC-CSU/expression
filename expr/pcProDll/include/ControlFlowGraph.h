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
// $Id: ControlFlowGraph.h,v 1.13 1998/04/11 00:35:38 xji Exp $
//
// File:  		ControlFlowGraph.h
// Created:		Wed Jan 22, 97
// Last modified: 	Wed Jan 22, 97
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: include

#ifndef __CONTROLFLOWGRAPH_H__
#define __CONTROLFLOWGRAPH_H__

#include "GlobalIncludes.h"
#include "BasicBlockNode.h"
#include "LinkedList.h"
#include "Array.h"
#include "LinkedListIterator.h"
#include "STLIncludes.h"


class ControlFlowGraph
{
 private:
 protected:

   LinkedList<BasicBlockNode> *_bbList;
   BasicBlockNode *_startNodePtr;

   Array<BasicBlockNode *> _dfsOrderArray;
   
 public:
   
   ControlFlowGraph() : _dfsOrderArray(0, 1)
      {
	 _bbList = NULL;
	 _startNodePtr = NULL;
      }
   
   // The buildCFG routine creates the actual list of basic blocks.
   // Therefore the destructor of this class should not delete the
   // storage.
   // Instead this class has separate methods for initializing the
   // _bbList pointer and for deleting the memory assigned to it.
   //
   ~ControlFlowGraph()
      {
      }
   
   void setBBList(LinkedList<BasicBlockNode> *someBBList)
      {
	 _bbList = someBBList;
      }
   
   void deleteBBList()
      {
    if (_bbList != NULL)
  	 delete _bbList;
    _bbList = NULL;
	 _startNodePtr = NULL;
      }
    
   void reset()
   {
	   deleteBBList();
	   _dfsOrderArray.removeAllElements();
   }

   void resetDFSNums()
   {
	   if (_bbList == NULL) return;

	   LinkedListIterator<BasicBlockNode> *iter = _bbList->elementsForward();

	   while (iter->hasMoreElements())
	   {
		   (iter->currentElement()).setDFSNum(0);

		   iter->nextElement();
	   }
	   delete iter;
   }

   void appendBBToList(BasicBlockNode *someNode)
   {
	   if ((_bbList != NULL) && (someNode != NULL))
		   _bbList->append(*someNode);
   }

   void appendBBToList(BasicBlockNode & someNode)
   {
	   if (_bbList != NULL)
		   _bbList->append(someNode);
   }

   BasicBlockNode & getFirstBB()
   {
	   return (_bbList->getHead());
   }

   BasicBlockNode & getLastBB()
   {
	   return (_bbList->getTail());
   }

   void setStartNodePtr(BasicBlockNode *startPtr)
      {
	 _startNodePtr = startPtr;
      }
   
   BasicBlockNode *getStartNode()
      {
	 return _startNodePtr;
      }

   // This method is used by the DFS Numbering Algorithm.
   // See BinGraphDFS.h for more info.
   //
   BasicBlockNode *getRootNodePtr()
      {
	 return _startNodePtr;
      }
   
   // This method provides a forward iterator over the list of basic
   // blocks. Note that it is the responsibility of the caller to free
   // up the iterator.
   //
   LinkedListIterator<BasicBlockNode> *createBBIterForw()
      {
	 LinkedListIterator<BasicBlockNode> *retPtr;
	 retPtr = _bbList->elementsForward();
	 return retPtr;
      }

   // This method provides a backward iterator over the list of basic
   // blocks. Note that it is the responsibility of the caller to free
   // up the iterator.
   //
   LinkedListIterator<BasicBlockNode> *createBBIterBack()
      {
	 LinkedListIterator<BasicBlockNode> *retPtr;
	 retPtr = _bbList->elementsBackward();
	 return retPtr;
      }
 
   int numBBs() 
      {
	return _bbList->numItems();
      }

   void orderDFS();

   int numBasicBlocks() 
      {
	 return _bbList->numItems();
      }
   
   ArrayIterator<BasicBlockNode *> *createDFSBBIter()
      {
	 ArrayIterator<BasicBlockNode *> *retPtr;
	 retPtr = _dfsOrderArray.elements();
	 return retPtr;
      }
   
   ArrayIterator<BasicBlockNode *> *createDFSBBIterBack()
      {
	 ArrayIterator<BasicBlockNode *> *retPtr;
	 retPtr = _dfsOrderArray.elementsBackward();
	 return retPtr;
      }
   
   BasicBlockNode *getBBPtr(int dfsNum);

   void genLoopIBSProps();
   
   void shortPrint(ostream & out);
   void print();
   
   void removeBBFromBBList(BasicBlockNode* bb) {
      if ( _bbList )
         _bbList->remove(*bb);
   } 
  
   void removeBBFromDFSList(BasicBlockNode* bb) {
      _dfsOrderArray.removeElement(bb);
   }
   void compact(void);
   void percolateIf(void);

   //Traverse traverses the CFG, and calls for each node the func function.
   int Traverse(int(*func)(BasicBlockNode *,NormalInstr *,BaseOperation *));
   void InstrSel(void);  // Added to traverse CFG while doing Isel

   // Added by SAH on : Thu, Dec 07, 2000
   //
   // A terminal node has either:
   // 1) no successors, or
   // 2) all its successors are thru back arcs.
   //
   void getListOfTerminalNodes(Vector<BasicBlockNode *> & list);
};

#endif
   
