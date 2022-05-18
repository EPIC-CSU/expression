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
//$Id: LoopHTGNode.h,v 1.11 1998/03/27 04:26:33 pgrun Exp $
//
// File:  		LoopHTGNode.h
// Created:		Wed Jan 29, 97
// Last modified: 	Wed Jan 29, 97
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: include

#ifndef __LOOPHTGNODE_H__
#define __LOOPHTGNODE_H__

#include "BaseHTGNode.h"
#include "MyLinkedList.h"
#include "Loop.h"
#include "BasicBlockIncludes.h"


#define _LOOP_TYPE_0 0  // IRREGULAR LOOP
#define _LOOP_TYPE_1 1  // REGULAR LOOP with conditional back arc.
#define _LOOP_TYPE_2 2  // REGULAR LOOP with unconditional back arc.

class LoopHTGNode: public BaseHTGNode
{
#ifdef WIN32_GUI//WIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUI
public:
 virtual CRect* Draw(GRAPH_NODE_TYPE nodeType, CDC * pDC);//, int x_pos, int y_pos);
 virtual void DrawInfo(GRAPH_NODE_TYPE nodeType, CDC * pDC, CRect& rect);
 virtual void DOTText(GRAPH_NODE_TYPE nodeType, CString& out);
 virtual GenericNode* FirstNotExpandedNode(GRAPH_NODE_TYPE nodeType);
 virtual MOUSEHITINFO* getHit(GRAPH_NODE_TYPE nodeType, CPoint & point);
#endif //WIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUI
 
private:
protected:
 MyLinkedList<BaseHTGNode *> _loopList;
 BaseHTGNode _dummyLoopHead;
 
 Loop *_ptrToLoopDS;
 
 MyLinkedList<BaseHTGNode *> _exitList;
 //
 // Note: Exit list contains list of break nodes too.

 MyLinkedList<BaseHTGNode *> _breakList;

public:
 LoopHTGNode();
 
 virtual ~LoopHTGNode()
 {
 }
 
 void setNewLoopHead(BaseHTGNode *hNode)
 {
	 _dummyLoopHead.setNext(hNode);
	 _loopList.append(hNode);
 }
 
 void setLoopHead(BaseHTGNode *hNode)
 {
	 _dummyLoopHead.setNext(hNode);
 }

 BaseHTGNode *getLoopHead()
 {
	 return _dummyLoopHead.getNext();
      }
   
   BaseHTGNode *getExitNode()
   {
	   if (_exitList.isEmpty())
		return _loopList.getTail();
	   else
	   {
		   cout << "Panic: The list of exit nodes (for the loop) was not empty" << endl;
		   cout << " Most likely, the function getExitNode returned a wrong node" << endl;
		   assert(0);
	   }
	   return NULL;
   }

   void appendExitNode(BaseHTGNode *node)
   {
	   _exitList.append(node);
   }

   void appendBreakNode(BaseHTGNode *node)
   {
	   _breakList.append(node);
   }

   // Note: ExitList contains break nodes too.
   //
   MyLinkedList<BaseHTGNode *> & getExitList() { return _exitList; }

   MyLinkedList<BaseHTGNode *> & getBreakList() { return _breakList; }

   void insertBefore(BaseHTGNode *thisNode, BaseHTGNode *nextNode);

   virtual BaseHTGNode *copyHTGBody();
   
   virtual BasicBlockNode *linkBBs(BasicBlockNode *nextBB, int direction = _FIRST_SUCC_, int arcType = _TREE_ARC_);

//   virtual void linkBBs(LinkBBList & prevBBs, LinkBBList & thisBBs);

   virtual BasicBlockNode *getFirstBB();

   virtual BasicBlockNode *getLastBB();

   virtual void renameDestValues();

   virtual void renameSrcValues(ValuesSET & in, ValuesSET & out);

   virtual void insertMove(int index, int ssaNum);
   virtual void insertMoves(ValuesSET & moveSet);
   virtual void insertMovesPass(ValuesSET & in, ValuesSET & out);
 
   virtual void insertLabel(BaseArgument *newlabel);

   virtual int getSuccDirOfExitBB()
      {
	 // Currently the successor basic block for the loop htg node will always be the _NEXT_SUCC_ of the last basic block
	 // of the loop body (i.e the bb which contains the back arc.). If the loop HTG structure is changed so that there
	 // is a new (dummy) basic block that serves as the exit, then a flag can be set and a different value returned for
	 // this function.
  
  return _SECOND_SUCC_;
 }
 
 MyLinkedList<BaseHTGNode *> &getLoopList(void)
 {
  return _loopList;
 }
 
 void appendNode(BaseHTGNode *someNode)
 {
	 (_loopList.getTail())->setNext(someNode);
	 _loopList.append(someNode);
      }
   
   void appendOnly(BaseHTGNode *someNode)
   {
	   _loopList.append(someNode);
   }

   void setPtrToLoopDS(Loop *thisLoop)
      {
	 _ptrToLoopDS = thisLoop;
 }
 
 Loop *getPtrToLoopDS()
 {
	 if (_ptrToLoopDS == NULL) 
  {
   // Error.
   cout << "Panic: Loop Data Structure not initialized properly" << endl;
   exit(1);
  }
	 return _ptrToLoopDS;
 }
 
 int hasInnerLoop()
 {
	 if (_ptrToLoopDS == NULL) 
  {
   // Error.
   cout << "Panic: Loop Data Structure not initialized properly" << endl;
   exit(1);
  }
	 return _ptrToLoopDS->hasInnerLoop();
 }

 MyLinkedListIterator<BaseHTGNode *> *createLoopIterForw()
 {
	 MyLinkedListIterator<BaseHTGNode *> *retPtr;
	 retPtr = _loopList.elementsForward();
	 
	 return retPtr;
 }
 
 MyLinkedListIterator<BaseHTGNode *> *createLoopIterBack()
 {
	 MyLinkedListIterator<BaseHTGNode *> *retPtr;
	 retPtr = _loopList.elementsBackward();
	 
	 return retPtr;
 }
 
 int isLoopNode()
 {
	 return _YES_;
      }

 virtual int containsLoopNode()
 {
	 return _YES_;
 }

 virtual int containsIfNode();

 virtual int containsCallNode();

   virtual void print(int indent);
   virtual void printC(int indent);

   virtual void trailblaze(void);

   virtual void compute_unifiable_ops(void);

   virtual int find_unifop_destination(BaseOperation *oper,MyLinkedList<BaseOperation *> moves);

   virtual void schedule(void);

   virtual int depends(BaseOperation *,MyLinkedList<BaseOperation *> &moves);

   virtual void setInstrID(int &crt,int &opcrt);

   virtual int contains_unifiable_op(BaseOperation *oper);

   virtual int migrate(BaseOperation *oper,vector<BaseHTGNode *> *path);

   virtual void fixIfHead(void);

   virtual int hasLive(BaseOperation *oper);

   virtual void setLowestSSAValues();

   virtual void mutation(void);
   
   virtual void InstrSel(void);

   virtual void ifConvert(BaseOpProperty* pred, 
						OpList & IselList,
						BaseOperation* ifOp, 
						OpList OperationsList);

   virtual void createListOfOperations(OpList &oplist);

   virtual int loopType(void);
};

#endif

