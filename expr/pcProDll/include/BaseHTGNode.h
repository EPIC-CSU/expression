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
//$Id: BaseHTGNode.h,v 1.16 1998/03/27 04:26:25 pgrun Exp $
//
// File:  		BaseHTGNode.h
// Created:		Wed Jan 29, 97
// Last modified: 	Wed Jan 29, 97
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: include

#ifndef __BASEHTGNODE_H__
#define __BASEHTGNODE_H__

#ifndef __UNIX__
	#include "..\StdAfx.h"
#else
	#ifndef ASSERT_TRACE
		#define ASSERT_TRACE(c,e) if (!(c)) { cerr << endl << "WOA, NELLIE! " << __FILE__ << "(" << __LINE__ << "): " << e << endl; assert(0); }
	#endif
#endif

#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include <map>
#include "ValuesSET.h"
#include "GlobalIncludes.h"
#include "BaseOperation.h"
#include "MyLinkedList.h"
#include "LinkedList.h"
#include "BaseOperation.h"
#include "BasicBlockIncludes.h"


#include "GenericNode.h"
//#include <vector>

class BasicBlockNode;

class BaseHTGNode : public GenericNode
{
#ifdef WIN32_GUI//WIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUI
public:
 virtual CRect* Draw(GRAPH_NODE_TYPE nodeType, CDC * pDC);//, int x_pos, int y_pos);
 virtual void DrawInfo(GRAPH_NODE_TYPE nodeType, CDC * pDC, CRect& rect);
 virtual void DOTText(GRAPH_NODE_TYPE nodeType, CString& out);
 virtual GenericNode* FirstNotExpandedNode(GRAPH_NODE_TYPE nodeType);
#endif //WIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUI

private:
 
protected:
 BaseHTGNode *_next;
 
public:
 //NOS
 virtual void renameDestValues() 
 {
  ASSERT_TRACE(0,"Don't call us. We'll call you!");
 };
 virtual void setLowestSSAValues() 
 {
  ASSERT_TRACE(0,"Don't call us. We'll call you!");
 };

 virtual void renameSrcValues(ValuesSET & in, ValuesSET & out) 
 {
  ASSERT_TRACE(0,"Don't call us. We'll call you!");
 };

 virtual void insertMove(int index, int ssaNum) 
 {
  ASSERT_TRACE(0,"Don't call us. We'll call you!");
 };

 virtual void insertMoves(ValuesSET & moveSet) 
 {
  ASSERT_TRACE(0,"Don't call us. We'll call you!");
 };

 virtual void insertMovesPass(ValuesSET & in, ValuesSET & out) 
 {
  ASSERT_TRACE(0,"Don't call us. We'll call you!");
 };

 virtual BasicBlockNode* getFirstBB()
 {
  ASSERT_TRACE(0,"Don't call us. We'll call you!");
  return NULL;
 };
 //NOS
 
 BaseHTGNode();
 
 virtual ~BaseHTGNode()
 {
 }
 
 virtual BaseHTGNode *copyHTGBody()
 {
	 cout << "Error: Base class virtual function should never be called" << endl;
  
	 exit(1);
		return NULL;
 }
 
 virtual BaseHTGNode *copyHTGBody(int numInstrs)
 {
	 cout << "Error: Base class virtual function should never be called" << endl;
  
	 exit(1);
		return NULL;
 }

 virtual void deleteHTGBody(int numInstrs)
 {
	 cout << "Error: Base class virtual function should never be called" << endl;
  
	 exit(1);
 }

 virtual int isSimpleNode()
 {
	 return _NO_;
 }
 
 virtual int isComplexNode()
 {
	 return _NO_;
 }
 
 virtual int isLoopNode()
 {
	 return _NO_;
 }
 
 virtual int isIfNode()
 {
	 return _NO_;
 }
 
 virtual int isNormalInstr()
 {
	 return _NO_;
 }
 
 virtual int isCallNode()
 {
	 return _NO_;
 }
 
 virtual BaseHTGNode *getPrev();
 
 virtual BaseHTGNode *getUp();
 
 virtual BaseHTGNode *getNext()
 {
	 return _next;
 }
 
 // This function returns a pointer to the "last" basic-block in a list of connected BBs.
 // Example: If this function is called within a Loop Node: It first connects all the internal BBs,
 // and then connects the first basicblock to the prevBB (if direction = _FIRST_SUCC_, to its true branch).
 // It also sets the arc Type of the connecting edge to that specified by arcType
 // It then returns a pointer to the last basicblock (i.e. the exit) of the loop.   
 //
 virtual BasicBlockNode *linkBBs(BasicBlockNode *prevBB, int direction = _FIRST_SUCC_, int arcType = _TREE_ARC_)
 {
	 cout << "Panic: This base class function (linkBBs) should never be called" << endl;
	 assert(0);
	 return NULL;
 }
 
 virtual int getSuccDirOfExitBB()
 {
	 // This function makes sense for "compound" htg nodes. For all other htg nodes return default value.
  
  return _FIRST_SUCC_;
 }
 
 virtual BasicBlockNode *getLastBB()
 {
	 cout << "Panic: This base class function (getLastBB) should never be called" << endl;
	 assert(0);
	 return NULL;
 } 
 
 virtual void insertLabel(BaseArgument *newlabel)
 {
	 cout << "Panic: This base class function (insertLabel) should never be called" << endl;
	 assert(0);
 } 

 virtual void insertBefore(BaseHTGNode *thisnode, BaseHTGNode *nextNode)
 {
	 cout << "Panic: This base class function (insertBefore) should never be called" << endl;
	 assert(0);
 } 

 virtual void setNext(BaseHTGNode *someNode)
 {
	 _next = someNode;
 }
 
 virtual BasicBlockNode *getBasicBlock()
 {
	 return NULL;
 }
 
 virtual void print(int indent){
  
 }
 
 virtual void printC(int indent){
  
 }
 
 virtual void trailblaze(void){
 }
 
 virtual void compute_unifiable_ops(void){
 }
 
 virtual int find_unifop_destination(BaseOperation *oper,MyLinkedList<BaseOperation *> moves){
  return -1;
 }
 
 virtual void schedule(void){
 }
 
 virtual int depends(BaseOperation *,MyLinkedList<BaseOperation *> &moves){
  return -1;
 }
 
 virtual void setInstrID(int &crt,int &opcrt){
 }
 
 virtual int contains_unifiable_op(BaseOperation *oper){
  return -1;
 }
 
 virtual int migrate(BaseOperation *oper,vector<BaseHTGNode *> *path){
  return -1;
 }
 
 virtual void fixIfHead(void){
 }
 
 virtual void removeUnifiableOps(BaseOperation *oper){
 }
 virtual int hasLive(BaseOperation *oper){
  return 0;
 }
 
 virtual void mutation(void){
 }

 virtual void InstrSel(void){
 }

 virtual int containsIfNode(void){
	 return _NO_;
 }

 virtual int containsLoopNode(void){
	 return _NO_;
 }

 virtual int containsCallNode(void){
	 return _NO_;
 }

 virtual void ifConvert(BaseOpProperty* pred, 
						OpList & IselList,
						BaseOperation* ifOp, 
						OpList OperationsList);

 virtual void createListOfOperations(OpList & oplist)
 {}

};


void printIndent(int indent);
void printIndent(FilePointer & fp, int indent);
void compute_unifiable_ops(MyLinkedList<BaseHTGNode*> *list);
void schedule_instructions(MyLinkedList<BaseHTGNode*> *list);
int contains_unifiable_op(MyLinkedList<BaseHTGNode*> *list,BaseOperation *oper);
BaseHTGNode * prevTB(BaseHTGNode *node,BaseOperation *oper,vector<BaseHTGNode *> *path=NULL);
void fixIfHead(MyLinkedList<BaseHTGNode*> *list);

int setNotGoUp(int);
int getNotGoUp(void);

typedef MyLinkedList<BaseHTGNode *> HTGLinkedList;
typedef MyLinkedListIterator<BaseHTGNode *> HTGLinkedListIter;

#endif


