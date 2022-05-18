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
//$Id: SimpleHTGNode.h,v 1.11 1998/03/27 04:26:36 pgrun Exp $
//
// File:  		SimpleHTGNode.h
// Created:		Wed Jan 29, 97
// Last modified: 	Wed Jan 29, 97
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: include

#ifndef __SIMPLEHTGNODE_H__
#define __SIMPLEHTGNODE_H__

#include "BaseHTGNode.h"
#include "BaseInstruction.h"
#include "LoopHTGNode.h"
//#include "Routine.h"

class Routine;
extern Routine *currRoutine;

class SimpleHTGNode: public BaseHTGNode
{
#ifdef WIN32_GUI//WIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUI
public:
 virtual CRect* Draw(GRAPH_NODE_TYPE nodeType, CDC * pDC);//, int x_pos, int y_pos);
 virtual void DrawInfo(GRAPH_NODE_TYPE nodeType, CDC * pDC, CRect& rect);
 virtual void DOTText(GRAPH_NODE_TYPE nodeType, CString& out);
 virtual GenericNode* FirstNotExpandedNode(GRAPH_NODE_TYPE nodeType);
 virtual MOUSEHITINFO* SimpleHTGNode::getHit(GRAPH_NODE_TYPE nodeType, CPoint & point);
#endif //WIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUI
 
private:
protected:
 BasicBlockNode *_bb;
 
public:
 SimpleHTGNode();
 
 SimpleHTGNode(BasicBlockNode *someBB);
 
 virtual ~SimpleHTGNode()
 {
 }
 
 int isSimpleNode()
 {
	 return _YES_;
 }
 
 virtual BaseHTGNode *copyHTGBody();

 virtual BaseHTGNode *copyHTGBody(int numInstrs);
 virtual void deleteHTGBody(int numInstrs);

 //See baseHTGNode.h for an explanation of the next function.
 //
 virtual BasicBlockNode *linkBBs(BasicBlockNode *nextBB, int direction = _FIRST_SUCC_, int arcType = _TREE_ARC_) 
 {
  // If we recieve a NULL BB, then there is no need to link the successors.
  //
  if (nextBB != NULL)
   nextBB->appendBB(_bb, direction, arcType);
  return _bb;
 } 
 
 virtual BasicBlockNode* getFirstBB()
 {
  return _bb;
 }
 
 virtual BasicBlockNode *getLastBB()
 {
	 return _bb;
 }
 
 virtual void renameDestValues();
 virtual void renameSrcValues(ValuesSET & in, ValuesSET & out);
 
 virtual void insertMove(int index, int ssaNum);
 virtual void insertMoves(ValuesSET & moveSet);
 virtual void insertMovesPass(ValuesSET & in, ValuesSET & out);
 
 virtual void insertLabel(BaseArgument *newlabel);
 
 void setBB(BasicBlockNode *someBB)
 {
	 _bb = someBB;
 }
 
 BasicBlockNode *getBasicBlock()
 {
	 return _bb;
 }
 
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
 
 void appendInstr(NormalInstr *instr,int bbDir);
 
 void removeUnifiableOps(BaseOperation *oper);
 
 virtual int hasLive(BaseOperation *oper);
 
 virtual void setLowestSSAValues();
 
 virtual void mutation(void);
 
 virtual void InstrSel(void);
 
 virtual void ifConvert(BaseOpProperty* pred, 
						OpList & IselList,
						BaseOperation* ifOp, 
						OpList OperationsList);

 virtual void createListOfOperations(OpList & oplist);
};

#endif


