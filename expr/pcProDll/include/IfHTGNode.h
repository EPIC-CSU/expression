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
//$Id: IfHTGNode.h,v 1.10 1998/03/27 04:26:31 pgrun Exp $
//
// File:  		IfHTGNode.h
// Created:		Wed Jan 29, 97
// Last modified: 	Wed Jan 29, 97
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: include

#ifndef __IFHTGNODE_H__
#define __IFHTGNODE_H__

#include "BaseHTGNode.h"
#include "OpPredicate.h"
#include "IfOp.h"

class IfHTGNode: public BaseHTGNode
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
   BaseHTGNode *_ifHead;
   //
   // Note: IfHead is not always kept uptodate.
   //
   // This is because of an artifact of the trailblazing implementation.
   //
   // For the purpose of trailblazing, the head of the if is considered
   // to be at the same level as that of the parent of the if stmt.
   //
   // It may not always be possible to keep the ifHead upto date.

   // Note: If there is any pass that may invalidate the ifHeads,
   // that pass should be mentioned here.
   //
   // As of now, Loop Unrolling invalidates the ifHead.

   BaseHTGNode *_dummyIfJoin;

   MyLinkedList<BaseHTGNode *> _trueBranch;
   MyLinkedList<BaseHTGNode *> _falseBranch;
   
 public:

	 IfHTGNode();
   
   virtual ~IfHTGNode()
      {
      }
   
   int isIfNode()
      {
	 return _YES_;
      }

   void setIfHead(BaseHTGNode *hNode)
      {
	 _ifHead = hNode;
      }

   BaseHTGNode *getIfHead()
      {
	 return _ifHead;
      }
   
   MyLinkedList<BaseHTGNode *> &getTrueBranch(void)
     {
       return _trueBranch;
     }

   MyLinkedList<BaseHTGNode *> &getFalseBranch(void)
     {
       return _falseBranch;
     }

   void setIfJoin(BaseHTGNode *hNode)
      {
	 _dummyIfJoin = hNode;
      }

   BaseHTGNode *getIfJoin()
      {
	 return _dummyIfJoin;
      }
   
   void appendTrueNode(BaseHTGNode *someNode)
      {
	 _trueBranch.append(someNode);
      }
   
   void appendFalseNode(BaseHTGNode *someNode)
      {
	 _falseBranch.append(someNode);
      }
   
   void insertBefore(BaseHTGNode *thisNode, BaseHTGNode *nextNode);

   MyLinkedListIterator<BaseHTGNode *> *createTrueBranchIterForw()
      {
	 MyLinkedListIterator<BaseHTGNode *> *retPtr;
	 retPtr = _trueBranch.elementsForward();
	 
	 return retPtr;
      }

   MyLinkedListIterator<BaseHTGNode *> *createTrueBranchIterBack()
      {
	 MyLinkedListIterator<BaseHTGNode *> *retPtr;
	 retPtr = _trueBranch.elementsBackward();
	 
	 return retPtr;
      }
   
   MyLinkedListIterator<BaseHTGNode *> *createFalseBranchIterForw()
      {
	 MyLinkedListIterator<BaseHTGNode *> *retPtr;
	 retPtr = _falseBranch.elementsForward();
	 
	 return retPtr;
      }

   MyLinkedListIterator<BaseHTGNode *> *createFalseBranchIterBack()
      {
	 MyLinkedListIterator<BaseHTGNode *> *retPtr;
	 retPtr = _falseBranch.elementsBackward();
	 
	 return retPtr;
      }

   virtual void print(int indent);
   virtual void printC(int indent);

   virtual void trailblaze(void);

   virtual void compute_unifiable_ops(void);

   virtual int find_unifop_destination(BaseOperation *oper,MyLinkedList<BaseOperation *> moves);

   virtual void schedule(void);

   virtual int depends(BaseOperation *,MyLinkedList<BaseOperation *> &moves);

   virtual int contains_unifiable_op(BaseOperation *oper);

   virtual int migrate(BaseOperation *oper,vector<BaseHTGNode *> *path);

   BaseHTGNode *createOrGetFirstTrueNode(int yesOrNo = _YES_);
   BaseHTGNode *createOrGetLastTrueNode(int yesOrNo = _YES_);
   BaseHTGNode *createOrGetFirstFalseNode(int yesOrNo = _YES_);
   BaseHTGNode *createOrGetLastFalseNode(int yesOrNo = _YES_);

   virtual void setInstrID(int &crt,int &opcrt);
   int splitFalse(BaseOperation *oper);
   int splitTrue(BaseOperation *oper);

   virtual void fixIfHead(void);

   virtual void removeUnifiableOps(BaseOperation *oper);

   void appendInstrToTrue(NormalInstr *instr);

   void appendInstrToFalse(NormalInstr *instr);

   virtual int hasLive(BaseOperation *oper);

   int IfHTGNode::hasLiveInTrueBranch(BaseOperation *oper);

   int IfHTGNode::hasLiveInFalseBranch(BaseOperation *oper);

   virtual BaseHTGNode *copyHTGBody();

   virtual void renameDestValues();

   virtual void insertMove(int index, int ssaNum);
   virtual void insertMoves(ValuesSET & moveSet);

   virtual void insertLabel(BaseArgument *newlabel);

   virtual void insertMovesPass(ValuesSET & in, ValuesSET & out);

   virtual void renameSrcValues(ValuesSET & in, ValuesSET & out);

  virtual BasicBlockNode *linkBBs(BasicBlockNode *nextBB, int direction = _FIRST_SUCC_, int arcType = _TREE_ARC_);

 //  virtual void linkBBs(LinkBBList & prevBBs, LinkBBList & thisBBs);

   virtual void setLowestSSAValues();
   
//   virtual int getSuccDirOfExitBB();

   virtual BasicBlockNode *getFirstBB();
   
   virtual BasicBlockNode *getLastBB();

   virtual void mutation(void);
   virtual void InstrSel(void);
 
   virtual void ifConvert(BaseOpProperty* pred, 
						OpList & IselList,
						BaseOperation* ifOp, 
						OpList OperationsList);

   virtual int containsIfNode()
   {
	   return _YES_;
   }

   virtual int containsLoopNode();
   virtual int containsCallNode();

// shouldIfConvertHeur function added

	int shouldIfConvertHeur();

	void removeBranchGOTO(BaseHTGNode* branch);

	void AppendIsels(OpList IselList, BaseOpProperty *predicate, bool sense);

	virtual void createListOfOperations(OpList & oplist);

};
#endif

   
