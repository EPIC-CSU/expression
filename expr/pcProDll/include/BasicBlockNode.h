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
// $Id: BasicBlockNode.h,v 1.21 1998/03/27 04:26:28 pgrun Exp $
//
// File:  		BasicBlockNode.h
// Created:		Mon Nov 11, 96
// Last modified: 	Mon Nov 11, 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: include

#ifndef __BASICBLOCKNODE_H__
#define __BASICBLOCKNODE_H__

#include "GlobalIncludes.h"
#include "BasicBlockIncludes.h"
#include "BaseInstruction.h"
#include "BasicBlockEdge.h"
#include "BasicBlockPredecessors.h"

#include "MyLinkedList.h"

#include "PropertyIncludes.h"
#include "BaseBBProperty.h"
#include "CompDefBBProperty.h"
#include "LiveDeadBBProperty.h"
#include "PropertyList.h"
#include "Flags.h"

#ifdef WIN32_GUI//WIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUI
#include "GenericNode.h"
#endif //WIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUI

// #define COUNT_BASICBLOCKS

const int _ALLINSTRS_ = -1;

class BasicBlockNode : public BaseHTGNode
{
#ifdef WIN32_GUI//WIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUI
public:
 void DisplayProperties(BasePropPage* dialog);
 virtual CRect* Draw(GRAPH_NODE_TYPE nodeType, CDC * pDC);//, int x_pos, int y_pos);
 virtual void DrawInfo(GRAPH_NODE_TYPE nodeType, CDC * pDC, CRect& rect);
 virtual void DOTText(GRAPH_NODE_TYPE nodeType, CString& out);
 virtual MOUSEHITINFO* getHit(GRAPH_NODE_TYPE nodeType, CPoint & point);
 virtual GenericNode* FirstNotExpandedNode(GRAPH_NODE_TYPE nodeType);
 virtual void ToggleUDDU(GRAPH_NODE_TYPE nodeType);
 virtual void ToggleUD(GRAPH_NODE_TYPE nodeType);
 virtual void ToggleDU(GRAPH_NODE_TYPE nodeType);
#endif //WIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUI

private:
 
#ifdef COUNT_BASICBLOCKS
 static int _numBasicBlocks;	
#endif
 
protected:
 
/*   IDType _leaderID;
 IDType _trailerID; */
 //
 // This is not really needed. Actually, the best representation of
 // a basic block is to have a list of instructionIDs (so that when
 // the instructions are moved, the pointers are not left dangling).
 
 BaseInstruction *_leader;
 BaseInstruction *_trailer;
 
 int _dfsNum;
 
 int _bbType;
 //
 // Each basicblock can be one of four types:
 // 1. FIRST : it has no incoming edges.
 // 2. LAST  : it has no outgoing edges.
 // 3. SIMPLE : it has only 1 outgoing edge (and multiple incoming
 //                                          edges)
 // 4. COMPLEX : it has multiple outgoing edges.
 
 BasicBlockEdge *_firstSucc;
 BasicBlockEdge *_nextSucc;
 //
 // A basic block can have a maximum of 2 exits. It is easier to
 // maintain two separate BasicBlockEdge structures, than to
 // maintain a list of BasicBlockEdges.
 
 // Need to store the label of the leader (if it exists).
 // Also, need to know if the label is a forward branch label or
 // backward branch label (or both).
 
 BasicBlockPredecessors _controlFlowPredecessors;
 //
 // A list of pointers that point to predecessor basic blocks.
 
 // property list
 PropertyList<BaseBBProperty> _propList;
 
 void _justSetNextBBOnly(BasicBlockNode *nextBB, int nextBBDir, int arcType);
 void _setNextBBInstrs(BasicBlockNode *nextBB, int nextBBDir, int arcType);

public:
 BasicBlockNode();
 BasicBlockNode(const BasicBlockNode & a);
 
 virtual ~BasicBlockNode()
 {
#ifdef COUNT_BASICBLOCKS
	 _numBasicBlocks--;
#endif
	 //delete _propList;
	 
	 if (_firstSucc != NULL)
		 delete _firstSucc;
	 if (_nextSucc != NULL)
		 delete _nextSucc;
 }
 
#ifdef COUNT_BASICBLOCKS
 static int numBasicBlocks()
 {
	 return _numBasicBlocks;
 }
#endif
 
 BasicBlockNode *copy() const;
 
 BasicBlockNode *copyBody(int numInstrs = _ALLINSTRS_);
 void deleteBody(int nInstrs = _ALLINSTRS_);

 void initLeader(BaseInstruction *someLeader)
 {
	 _leader = someLeader;
 }
 
 void initTrailer(BaseInstruction *someTrailer)
 {
	 _trailer = someTrailer;
 }
 
 BaseInstruction *getFirstInstr() const
 {
	 return _leader;
 }
 
 BaseInstruction *getLastInstr() const
 {
	 return _trailer;
 }
 
 void setDFSNum(int dfsNum)
 {
	 _dfsNum = dfsNum;
 }
 
 int getDFSNum() const
 {
	 return _dfsNum;
 }
 
 void setToFirstBB()
 {
	 _bbType = _FIRST_BB_;
 }
 
 void setToLastBB()
 {
	 _bbType = _LAST_BB_;
 }
 
 void setToSimpleBB()
 {
	 _bbType = _SIMPLE_BB_;
 }
 
 void setToComplexBB()
 {
	 _bbType = _COMPLEX_BB_;
 }
 
 int isFirstBB()
 {
	 if (_bbType == _FIRST_BB_)
   return _YES_;
	 return _NO_;
 }
 
 int isLastBB()
 {
	 if (_bbType == _LAST_BB_)
   return _YES_;
	 return _NO_;
 }
 
 int isSimpleBB()
 {
	 if (_bbType == _SIMPLE_BB_)
   return _YES_;
	 return _NO_;
 }
 
 int isComplexBB()
 {
	 if (_bbType == _COMPLEX_BB_)
   return _YES_;
	 return _NO_;
 }
 
 int isEmpty()
 {
	 if ((_leader == NULL) || (_trailer == NULL))
		 return _YES_;
	 return _NO_;
 }

 void setFirstEdge(BasicBlockEdge *someEdge)
 {
	 _firstSucc = someEdge->copy();
 }
 
 void setFirstEdge(BasicBlockEdge & someEdge)
 {
	 _firstSucc = someEdge.copy();
 }
 
 void setNextEdge(BasicBlockEdge *someEdge)
 {
	 _nextSucc = someEdge->copy();
 }
	
 void setNextEdge(BasicBlockEdge & someEdge)
 {
	 _nextSucc = someEdge.copy();
 }
	
#ifdef WIN32
 __declspec( dllexport ) 
#endif
  BasicBlockEdge  *getFirstEdge();
 
#ifdef WIN32
 __declspec( dllexport ) 
#endif
  BasicBlockEdge  *getNextEdge();
 
 // The next two functions are provided so that the user can quickly
 // access the successor basicblocks (instead of having to call the
 // methods associated with the BasicBlockEdge class)
 //
 BasicBlockNode *getFirstSucc();
 BasicBlockNode *getNextSucc();
 
 // Returns the number of instrs in the basic block.
 // Note: This may not be the same as the number of operations in the basic block.
 //
 int numInstrs();

 void addCFPredecessors(MyLinkedList<BasicBlockNode *> *listOfPreds)
 {
	 _controlFlowPredecessors.setPredList(listOfPreds);
 }
 
 void addCFPred(BasicBlockNode *somePred)
 {
  _controlFlowPredecessors.addPred(somePred);
 }
 
 void addUniqCFPred(BasicBlockNode *somePred)
 {
  _controlFlowPredecessors.addUniqPred(somePred);
 }
 
 void removeCFPred(BasicBlockNode *somePred)
 {
  _controlFlowPredecessors.removePred(somePred);
 }
 
 void deleteCFPredecessors()
 {
	 _controlFlowPredecessors.deletePredList();
 }
 
 //
 // Note that the above two methods (add and delete CFPredecessors)
 // need not have been written. They only call the methods of
 // _controlFlowPredecessors object. But, the next method gives us a
 // direct handle to _controlFlowPredecessors (which can be used to
 // add and delete CFPredecessors). I have provided them because it
 // seems better to do them directly instead of having something
 // that looks like (for example)
 //
 // (((someRoutine.getCFG()).getLastBasicBlock()).getCFPred()).setPredList(...)
 //
 // Using the above functions, it would be
 //
 // (((someRoutine.getCFG()).getLastBasicBlock()).addCFPredecessors(...)
 //
 
 BasicBlockPredecessors & getCFPred()
 {
	 return _controlFlowPredecessors;
 }
 
 int isLoopHeadType();
 int isIfHeadType();
 
 int isCallRetType();
 int isLoopTailType();

 friend const int operator == (const BasicBlockNode & a, const BasicBlockNode & b)
 {
  return (  a.getFirstInstr() == b.getFirstInstr() 
   && a.getLastInstr() == b.getLastInstr()
   && a.getDFSNum() == b.getDFSNum() );
 }
 
 void print();
 
 friend ostream & operator << (ostream & out, const BasicBlockNode & s)
 {
	 return out;
 }
 //
 // Rewrite the operators == and << later.
 
	
   // argument should be const, should change addProperty's argument to const
   void addProperty(BaseBBProperty& prop) {_propList.addProperty(prop);}
   PropertyList<BaseBBProperty>& getPropList() { return _propList; }
   BaseBBProperty& getProperty(PropertyNames name) {
	return _propList.getProperty(name);
   }
   BaseBBProperty* getPtrToProperty(PropertyNames name) {
	return _propList.getPtrToProperty(name);
   }
   void  printProperty(PropertyNames name) {
	BaseBBProperty* prop = getPtrToProperty(name);
        if ( prop )
	   prop->print(); 
   }
   
   void removeProperty(BaseBBProperty& prop) {
      _propList.removeProperty(prop);
   }

   void recursiveRemoveProperty(PropertyNames name);

   int isEq(BasicBlockNode* a);

   void delFirstSucc() {
      delete _firstSucc;
      _firstSucc = NULL;
   }

   void delNextSucc() {
      delete _nextSucc;
      _nextSucc = NULL;
   }

   void compactBBHeur(void);
   void percolateIfBBHeur(void);

   int hasBackArcTo(BasicBlockNode * bb);
   
   int isSimpleType(void);

   void compute_unifiable_ops(void);

   int depends(BaseOperation *,MyLinkedList<BaseOperation *> &moves);
   int cannotMoveBeyond(BaseOperation *,MyLinkedList<BaseOperation *> &moves);

   void print(int indent);
   void printC(int indent);

   void irToC(FilePointer & fp, int indent);

   void setInstrID(int &crt,int &opcrt);

   void schedule(void);

   int contains_unifiable_op(BaseOperation *oper);

   int hasInstr(BaseInstruction *instr);

   void appendInstr(NormalInstr *instr,int bbDir);
   void addInstruction(BaseInstruction *instrToBeAdded, BaseInstruction *beforeOrAfterThisInstr, bool before);


   void appendBB(BasicBlockNode *prevBB, int nextBBDir, int arcType = _TREE_ARC_);

   // This function deletes the arc, and also sets the flow-ops in the last instr of the basic-block 
   // to NULL.
   //
   void deleteArc(int nextBBDir, int arcType = _TREE_ARC_);

   void setNextBB(BasicBlockNode *nextBB, int nextBBDir, int arcType = _TREE_ARC_);
   BasicBlockNode *getSuccBB(int nextBBDirection);

   int isLive(BaseArgument *arg);

   int countLive(void);
   
   void negateIf();

   void insertAsLastOp(BaseOperation *op);
   Flags flags;
   void markBlock(bool val);
   virtual void ifConvert(BaseOpProperty* pred, OpList &IselList, BaseOperation* ifOp, OpList OperationsList);
   
   virtual void createListOfOperations(OpList & oplist);
   void printASMByCFG(ostream &);
   void printASMByCFGForThumb(ostream &);
	  
   // Return the label (if it exists) from the leader instr, else return NULL.
   //
   BaseArgument *getLabel();
	  
   // Return NULL if the trailer instruction is not a goto.
   //
   BaseOperation *hasGoto();
   BasicBlockNode *getFallThruNode();
   BasicBlockNode *getTrueNode();

   // Return NULL if the trailer instruction is not an if.
   //
   BaseOperation *hasIf();

};

BaseInstruction *getPrev(BaseInstruction *instr,BaseInstruction *start);

// Note: The next function is defined in BasicBlockNode.cpp
// I have not declared it here becos it needs Routine (which I don't want to include in this file).
//
// BasicBlockNode *insertDummyBBBetween(BasicBlockNode *currBB, BasicBlockNode *succBB, Routine *curR);

#endif


