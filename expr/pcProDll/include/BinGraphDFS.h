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
// $Id: BinGraphDFS.h,v 1.4 1998/03/21 01:18:31 ahalambi Exp $
//
// File:  		BinGraphDFS.h
// Created:		Fri Mar 21, 97
// Last modified: 	Fri Mar 21, 97
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: include

// Depth First Search Numbering of a Graph where each node has at max
// 2 children .
// Assume that each node in the graph has initial DFS number = 0
// In a Binary Graph, there are four possibilities for each arc:
//    1) tree arc
//    2) back arc
//    3) forward arc
//    4) cross arc

#ifndef __BINGRAPHDFS_H__
#define __BINGRAPHDFS_H__

#include "GlobalIncludes.h"
#include "MyStack.h"
#include "MyStackIterator.h"

const int NOT_VISITED = 0;
const int LEFT_VISITED = 1;
const int RIGHT_VISITED = 2;

template <class Node> class DFSStackEntry
{
#ifdef LEAKY
 //NOS: used for memory leaks
 char tag[MAX_PATH];
#endif
 
 public:
  Node *_nPtr;
  int _visitNum;
  
  DFSStackEntry()
  {
#ifdef LEAKY
   {
    //NOS
    char t[MAX_PATH];
    GetFileTitle(__FILE__,t,MAX_PATH); 
    sprintf(tag,"%d:%s",__LINE__,t);
   }
#endif
   
   _nPtr = NULL;
   _visitNum = 0;
  }
  
  ~DFSStackEntry()
  {
   _nPtr = NULL;
  }
  
  friend int operator == (const DFSStackEntry & a, const DFSStackEntry & b)
  {
   if ((a._nPtr == b._nPtr) && (a._visitNum == b._visitNum))
    return _YES_;
   return _NO_;
  }
  
  friend ostream & operator << (ostream & out, const DFSStackEntry & a)
  {
   out << "VisitNum :" << a._visitNum << endl;
   return out;
  }
};


template <class Graph, class Node, class Edge> class BinGraphDFS
{
#ifdef LEAKY
 //NOS: used for memory leaks
 char tag[MAX_PATH];
#endif
 
protected:
 
 MyStack<DFSStackEntry<Node> *> _currPath;
 
public:
 BinGraphDFS()
 {
#ifdef LEAKY
  {
   //NOS
   char t[MAX_PATH];
   GetFileTitle(__FILE__,t,MAX_PATH); 
   sprintf(tag,"%d:%s",__LINE__,t);
  }
#endif
  
 }
 
 ~BinGraphDFS()
 {
 }
 
 void doDFS(Graph & graph)
 {
  int dfsNum = 0;
  
  Node *nextNodePtr;
  Edge *currEdgePtr;
  
  if ((nextNodePtr = graph.getRootNodePtr()) == NULL)
   return;
  
  DFSStackEntry<Node> *tempEntryPtr = new DFSStackEntry<Node>;
  tempEntryPtr->_nPtr = nextNodePtr;
  tempEntryPtr->_visitNum = NOT_VISITED;
  
  _currPath.push(tempEntryPtr);
  
  nextNodePtr->setDFSNum(++dfsNum);
  
  DFSStackEntry<Node> currEntry;
  
  int currNodeDFSNum, nextNodeDFSNum, backArc;
  
  MyStackIterator<DFSStackEntry<Node> *> *currPathIter;
  
  /*	 currPathIter = new MyStackIterator<DFSStackEntry<Node> *>(_currPath); */
  
  while (! _currPath.isEmpty())
  {
   
   currEntry._visitNum = (_currPath.top())->_visitNum;
   currEntry._nPtr = (_currPath.top())->_nPtr;
   
   if (currEntry._visitNum == RIGHT_VISITED)
    _currPath.pop();
   else
   {
    if (currEntry._visitNum == LEFT_VISITED)
    {
     (_currPath.top())->_visitNum = RIGHT_VISITED;
     
     currEdgePtr = (currEntry._nPtr)->getNextEdge();
     
     nextNodePtr = (currEntry._nPtr)->getNextSucc();
    }
    else   // if (currEntry._visitNum == NOT_VISITED)
    {
     (_currPath.top())->_visitNum = LEFT_VISITED;
     
     currEdgePtr = (currEntry._nPtr)->getFirstEdge();
     
     nextNodePtr = (currEntry._nPtr)->getFirstSucc();
    }
    
    if (nextNodePtr != NULL)
    {
     currNodeDFSNum =
      (currEntry._nPtr)->getDFSNum();
     nextNodeDFSNum =
      nextNodePtr->getDFSNum();
     
     if (nextNodeDFSNum == 0)
     {
      // This is a TREE ARC condition.
      
      currEdgePtr->setToTreeArc();
      
      tempEntryPtr = new DFSStackEntry<Node>;
      tempEntryPtr->_nPtr = nextNodePtr;
      tempEntryPtr->_visitNum = NOT_VISITED;
      
      _currPath.push(tempEntryPtr);
      
      nextNodePtr->setDFSNum(++dfsNum);
     }
     else
     {
      if (nextNodeDFSNum > currNodeDFSNum)
       
      {
       // This is a FORWARD ARC condition
       
       currEdgePtr->setToForwardArc();
      }
      else if (nextNodeDFSNum == currNodeDFSNum)
      {
       // This is a BACK ARC condition
       
       currEdgePtr->setToBackArc();
      }
      else  // if (nextNodeDFSNum < currNodeDFSNum)
      {
       // This is an either BACK ARC
       // or CROSS ARC condition.
       
       backArc = 0;
       
       currPathIter = new MyStackIterator<DFSStackEntry<Node> *>(_currPath);
       while (currPathIter->hasMoreElements())
       {
//        cout << (currPathIter->currentElement()->_nPtr) << endl;
        currPathIter->nextElement();
       }
       currPathIter->reset();
       
       while
        (currPathIter->hasMoreElements())
       {
        if
         ((currPathIter->currentElement())->_nPtr->getDFSNum() == nextNodeDFSNum)
        {
         // This is a BACK ARC
         // condition
         
         currEdgePtr->setToBackArc();
         
         backArc = 1;
         break;
        }
        currPathIter->nextElement();
       }
       if (!backArc)
       {
        // This is a CROSS ARC condition
        
        currEdgePtr->setToCrossArc();
       }
       /*				       currPathIter->reset(); */
       delete currPathIter;
      }
     }
    }
   }
      }
      /*	 delete currPathIter; */
}

};

#endif



