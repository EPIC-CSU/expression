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
// $Id: ControlFlowGraph.cc,v 1.8 1998/02/23 19:32:18 pgrun Exp $
//
// File:  		ControlFlowGraph.cc
// Created:		Wed Nov 12, 97
// Last modified: 	Wed Nov 12, 97
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module:
   
#include "stdafx.h"

#include "ControlFlowGraph.h"
#include "BBIterator.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

void ControlFlowGraph::orderDFS()
{
   // assumes that dfs numbering has already been done.
      
   if (_bbList == _NULL_)
      {
	 cout << "Panic: Basic Block not initialized yet." << endl;
	 assert(0);
      }
   
   _dfsOrderArray.setSize(_bbList->numItems());
   
   LinkedListIterator<BasicBlockNode> *bbLIter;
   bbLIter = _bbList->elementsForward();
   
   BasicBlockNode *orderPtr;
   
   while (bbLIter->hasMoreElements())
      {
	 orderPtr = & (bbLIter->currentElement());
	 
	 try
	    {
	       _dfsOrderArray.setElementAt(orderPtr, (orderPtr->getDFSNum() - 1));
	    }
	 catch (Exception *e)
	    {
	       cout << "Panic: Encountered invalid DFS Number" << endl;
#ifdef WIN32
		   cout << "Exception: " << *e;
#endif
	    }
	 
	 bbLIter->nextElement();
      }
   
   delete bbLIter;
}

BasicBlockNode* ControlFlowGraph::getBBPtr(int dfsNum)
{
   try
      {
	 return (_dfsOrderArray.elementAt(dfsNum - 1));
      }
   catch (Exception *e)
      {
	 cout << "Warning: Wrong DFS Number specified" << endl;
#ifdef WIN32
		   cout << "Exception: " << *e;
#endif
	 return NULL;
      }
}

void ControlFlowGraph::shortPrint(ostream & out)
{
   LinkedListIterator<BasicBlockNode> *bbIter;
   bbIter = _bbList->elementsForward();
   
   int i = 0;
   
   while (bbIter->hasMoreElements())
      {
//	 out << "BB: " << i;
	 i++;
//	 out << " Loc: " << &(bbIter->currentElement());
//	 out << " DFS: " << (bbIter->currentElement()).getDFSNum();
//	 out << endl;
	 
	 bbIter->nextElement();
      }
   
   delete bbIter;
}


void ControlFlowGraph::print()
{
 LinkedListIterator<BasicBlockNode> *bbIter;
 bbIter = _bbList->elementsForward(); 
 int i = 0; 

 while (bbIter->hasMoreElements())
 {
	 cout << "Basic Block: " << i << " Loc: " << &(bbIter->currentElement()) << " ";
	 bbIter->currentElement().print();
	 cout << endl;

	 i++;  	 
	 bbIter->nextElement();	 
 }
 
 delete bbIter;
}

// Instruction selection by traversing the CFG
void ControlFlowGraph::InstrSel()
{
	LinkedListIterator<BasicBlockNode> *bbIter;
	bbIter = _bbList->elementsForward(); 
	
	while (bbIter->hasMoreElements())
	{
		BBForwIterator iter(&(bbIter->currentElement()));
		while(iter.hasMoreElements()){
			(iter.currentElement())->InstrSel();
			iter.nextElement();
		}
		bbIter->nextElement();
	}
}


   
void ControlFlowGraph::compact(void)
{
   LinkedListIterator<BasicBlockNode> *bbIter;
   bbIter = _bbList->elementsForward();
   
   while (bbIter->hasMoreElements())
      {
	 (bbIter->currentElement()).compactBBHeur();
	 
	 bbIter->nextElement();
	 
      }
   
   delete bbIter;
}

   
void ControlFlowGraph::percolateIf(void)
{
   LinkedListIterator<BasicBlockNode> *bbIter;
   bbIter = _bbList->elementsForward();
   
   while (bbIter->hasMoreElements())
      {
	 (bbIter->currentElement()).percolateIfBBHeur();
	 
	 bbIter->nextElement();
	 
      }
   
   delete bbIter;
}

/////////////////////////////////////////////////////////////////////////////
//used by Traverse, to avoid infinite loops in the CFG
set<BasicBlockNode*> traversal_bb_list;


int TraverseBB(int(*func)(BasicBlockNode*,NormalInstr *instr,BaseOperation *oper),BasicBlockNode*bb){
	NormalInstr *instr;
	BaseOperation *oper;
	//if already traversed, return 0
	if(traversal_bb_list.find(bb)!=traversal_bb_list.end())
		return 0;
	traversal_bb_list.insert(bb);
	
	//for each instruction in this basic block
	for(BBForwIterator i(bb);i.hasMoreElements();i.nextElement()){
		instr=dynamic_cast<NormalInstr*>(i.currentElement());
		_ASSERT(instr);
		//for(oper=instr->getPtrToRootOperation();oper
		//apply the function func to each operation in the bb

		//aded by Radu
		ASSERT_TRACE(0, "TraverseBB called, oper not initialized");
		if((*func)(bb,instr,oper)){
			return 1;
		}
	}
	
	if(TraverseBB(func,bb->getFirstSucc())) return 1;
	if(TraverseBB(func,bb->getNextSucc())) return 1;

	return 0;
}

//Traverse traverses the CFG, and calls for each node the func function.
//if func returns 1 it stops and returns
//if func returns 0 it continues calling for the rest of the CFG
//func is a function pointer with BasicBlock as argument and returning an int
int ControlFlowGraph::Traverse(int(*func)(BasicBlockNode*,NormalInstr *instr,BaseOperation *oper)){
	//initialize the list of BBs
	traversal_bb_list.clear();

	return TraverseBB(func,&getFirstBB());
}
/////////////////////////////////////////////////////////////////////////////

// Added by SAH on : Thu, Dec 07, 2000
//
// A terminal node has either:
// 1) no successors, or
// 2) all its successors are thru back arcs.
//
void ControlFlowGraph::getListOfTerminalNodes(Vector<BasicBlockNode *> & list)
{
	BasicBlockNode *currBB;
	BasicBlockEdge *currEdge;

   // iterate over the list of basic blocks.
   //
   ArrayIterator<BasicBlockNode *> *iter = createDFSBBIter();

   while (iter->hasMoreElements())
   {
	   currBB = iter->currentElement();

	   currEdge = currBB->getFirstEdge();
	   if ((currEdge != NULL) && (!currEdge->isBackArc()))
	   {
		   iter->nextElement();
		   continue;
	   }

	   currEdge = currBB->getNextEdge();
	   if ((currEdge != NULL) && (!currEdge->isBackArc()))
	   {
		   iter->nextElement();
		   continue;
	   }

	   // if reached here, it has no forward successors. add it.
	   //
	   list.push_back(currBB);

	   iter->nextElement();
   }
   delete iter;
}
