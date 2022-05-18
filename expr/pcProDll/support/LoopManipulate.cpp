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
//
// File:  		LoopManipulate.cc
// Created:		Wed Nov 20, 96
// Last modified: 	Wed Nov 20, 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: LOOP

#include "stdafx.h"

#include "Routine.h"
#include "BaseHTGNode.h"
#include "LoopHTGNode.h"
#include "BasicBlockIncludes.h"
#include "GotoOp.h"
#include "SymbolTable.h"
#include "RenameTable.h"
#include "BinGraphDFS.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#endif
//END MFC DEBUG NEW: 

extern SymbolTable *globalSymbolTable;
extern RenameTable globalRenameTable;

extern void maintainUD(BaseHTGNode *loop);
extern void maintainDoubles(BaseHTGNode *loop);

const int _NOT_LAST_ = 0;
const int _LAST_ = 1;

extern Routine *currRoutine;

BasicBlockNode *addAndLink(BaseHTGNode *someNode, BaseHTGNode *copyNode, BasicBlockNode *lastBB, BasicBlockNode *succBB, int i = _NOT_LAST_)
{		
	BasicBlockNode *copyLastBB;
	BaseHTGNode *tempPtr;

	copyLastBB = copyNode->linkBBs(succBB, _ALL_SUCC_, _TREE_ARC_);
	copyLastBB->appendBB(lastBB, _SECOND_SUCC_, _TREE_ARC_);
	
	// Note: if succBB is NULL (i.e. the loop is the last node), then
	// the forward branch of the first if will point to NULL.
	//
	lastBB->setNextBB(succBB, _FIRST_SUCC_, _FORWARD_ARC_);
	
	if (i != _LAST_)
	{
		copyNode->setLowestSSAValues();
		copyNode->renameDestValues();
	}

	tempPtr = ((LoopHTGNode*)copyNode)->getLoopHead();
	//
	// Now, we need to add the internal nodes of the newly created HTG node to the
	// original loop.
	
	//******************************IMPORTANT*****************************
	// Note: Altho the new nodes are added after the old nodes in the loop,
	// for all practical purposes, the original nodes are considered as the
	// new nodes. (i.e we rename dests of someNode, etc).
	//******************************IMPORTANT*****************************
	
	while (tempPtr != NULL)
	{
		// Oct 27, 99 : SAH. Changed from appendOnly to appendNode
		//
		((LoopHTGNode *) someNode)->appendNode(tempPtr);
		tempPtr = tempPtr->getNext();
	}
		
	if (i != _LAST_)
	{
		globalRenameTable.resetSymbolTable();
		globalRenameTable.resetRenameTable();
	}

	// finally return the new lastBB.
	//
	return (copyNode->getLastBB());
}

void removeLoopHeaderLabel(BaseHTGNode *someNode)
{
	// remove the label from the first instruction in the htg node.
	// 
	NormalInstr *instr = (NormalInstr *)(someNode->getFirstBB())->getFirstInstr();

	if (instr->hasLabel())
		instr->deleteLabel();
}

void unrollSimple(BaseHTGNode *someNode, int numTimes)
{
	Vector<BaseHTGNode *> loopUnrollCopies;
	Vector<BasicBlockNode *> ifBBs;
	//
	// ifBBs tracks the BBs whose if conditions need to be
	// changed. (e.g. LT becomes GE)
	
	Vector<BasicBlockNode *>::iterator iter;
	
	// assume that all checking has been done before.
	
	BaseHTGNode *copyNode;
	BasicBlockNode *lastBB, *succBB;
	
	lastBB = someNode->getLastBB();
	ifBBs.push_back(lastBB);
	succBB = lastBB->getNextSucc();
	
	// First, unroll the loop numTimes - 1 times.
	// Then, rename the dest values.
	// Then unroll once more.
	// Finally, rename the sources and maintain the ud chains.
	//
	for (int i = 0; i < numTimes; i++)
	{
		copyNode = someNode->copyHTGBody();
		removeLoopHeaderLabel(copyNode);
		loopUnrollCopies.push_back(copyNode);
	}
	
	someNode->setLowestSSAValues();
	someNode->renameDestValues();
	globalRenameTable.resetSymbolTable();
	globalRenameTable.resetRenameTable();
	
	for (int j = 0; j < numTimes - 1; j++)
	{
		lastBB = addAndLink(someNode, loopUnrollCopies[j], lastBB, succBB);
		ifBBs.push_back(lastBB);
	}
	
	//
	// Note: only the partially unrolled loop needs to be renamed (for dest args).
	// Thus this pass is done before last new nodes are added to the original loop.
	//
	
	// After renaming the destination values, make sure that the symbol table is properly
	// updated to account for the newly created values.
	//
	globalRenameTable.resetSymbolTable();
	globalSymbolTable->setStartNums();
	
	lastBB = addAndLink(someNode, loopUnrollCopies[numTimes - 1], lastBB, succBB, _LAST_);
	//
	// Do not push this lastBB into the ifBBs list, because we do not need to convert
	// this if.
	
	// The next step is to insert moves where necessary.
	// This is needed due to RSSA.
	// The only two cases where a move needs to be inserted are:
	// 1) in an if structure
	// 2) in a loop structure.
	//
	// Currently, the first in set to the loop (i.e. the unrolled loop) is empty.
	// However, for it to be truly correct, it should be passed the set of definitions
	// that reach the first basic_block of the loop.
	
	ValuesSET in1, out1;
	
	someNode->insertMovesPass(in1, out1);
	
	ValuesSET in2, out2;
	
	// Now we rename all the source values.
	
	// The first pass starts with an empty in set.
	//
	// Note: The in, out sets generated by the previous pass (insertMoves) could be used instead.
	// This would avoid recomputation of the same in/out sets.
	//
	someNode->renameSrcValues(in2, out2);
	
	// The second pass gives the out generated by the first pass as input to the second pass.
	//
	someNode->renameSrcValues(out2, in2);
	
	// Call maintainUD function on someNode.
	//
	maintainUD(someNode);
	
	// We also need to maintain the double properties.
	// These get mangled during the copying phase, so use this phase to correctly align them.
	// Note: This phase is needed only if the patchForDoubles phase has been invoked from NoGUIMain
	//
	maintainDoubles(someNode);
	//
	// also, the symbol table may need to be updated again.
	//
	globalSymbolTable->setStartNums();
	
	// Finally, negate all the ifs.
	for (iter = ifBBs.begin(); iter != ifBBs.end(); iter++)
	{
		(*iter)->negateIf();
	}
	// Also, the basicblock pointed to by the last node (i.e. the backward arc from
	// the new last BB) may need to be properly set.
	//
	// Now, if the loop had an unconditional goto as the last op, then we need to
	// change the fall thru branch to point to the first instr of the loop.
	
	// This could happen for instance if the loop has been shifted.
	
	if (((LoopHTGNode *)someNode)->loopType() == _LOOP_TYPE_2)
	{
		(someNode->getFirstBB())->appendBB(lastBB, _SECOND_SUCC_, _BACK_ARC_);
	}
	else
	{
		(someNode->getFirstBB())->appendBB(lastBB, _FIRST_SUCC_, _BACK_ARC_);
	}
	
	// Now, reset the control flow graph.
	//
	// 1) Re-compute the control-flow predecessors.
	// 2) Re-do DFS numbering.
	
	// First, we recompute the CFPreds.
	//
	currRoutine->buildCFPreds();
	
	// Also, since we added new basic blocks we need to do a dfs ordering of the bbs.
	//	
	BinGraphDFS<ControlFlowGraph, BasicBlockNode, BasicBlockEdge> basicBlockDFS;
	currRoutine->getCFG().resetDFSNums();
	basicBlockDFS.doDFS(currRoutine->getCFG());   
	currRoutine->getCFG().orderDFS();
}

void unroll(BaseHTGNode *someLoop, int numTimes)
{
   if (someLoop == NULL) return;
   if (!someLoop->isLoopNode()) 
      {
	 // Error. 
	 cout << "Panic: Recieved non-loop node for unrolling";
	 exit(1);
      }

   /*
   if (((LoopHTGNode *)someLoop)->getPtrToLoopDS()->hasInnerLoop())
      {
	 cout << "Unrolling is not supported for loops with inner loops" << endl;
	 return;
      }
   */


   if (numTimes <= 0)
      {
	 cout << "Loop unrolling recieved invalid unroll count: " << numTimes << endl;
	 exit(1);
      }

   unrollSimple(someLoop, numTimes);
}

extern int insertBBBetween(BasicBlockNode *prevNode, BasicBlockNode *nextNode, BasicBlockNode *newNode);

// Note: Important assumption : prologueNode is a simple node.
//
void insertAndLinkPrologue(BaseHTGNode *loopNode, BaseHTGNode *prologueNode)
{
	// note: we assume that the prologue node is a simple node.
	// In the case of adding complex prologue nodes, 
	// the internal nodes themselves should be linked first.

	// first, find the position where the prologue node should be added.
	//
	// then, add the instructions to the instrlist.
	// finally, add the htg node in the appropriate place.
	//

	// finding the position to insert the prologue node.
	//
	BaseHTGNode *parentNode = loopNode->getUp();
	BaseHTGNode *prevNode = loopNode->getPrev();

	if (prevNode == NULL) // error??
	{
		if (parentNode != NULL) // this is indeed an error.
		{
			cout << "Panic: Could not add the prologue node during loop shifting (1)" << endl;
			assert(0);
		}
		else
		{ 
			// need to insert the prologue node as the first node in the routine.
			//
			(currRoutine->getHTG())->prepend(prologueNode);
			prologueNode->setNext(loopNode);
		}
	}
	else
	{

		if (parentNode == NULL)
			(currRoutine->getHTG())->insertAfter(prologueNode, prevNode);
		else
			parentNode->insertBefore(prologueNode, loopNode);

		prevNode->setNext(prologueNode);
		prologueNode->setNext(loopNode);
	}

	BasicBlockNode *prevNodeLastBB = prevNode->getLastBB();
	BasicBlockNode *nextNodeFirstBB = loopNode->getFirstBB();

	if (insertBBBetween(prevNodeLastBB, nextNodeFirstBB, prologueNode->getFirstBB()) == _NO_)
	{
		cout << " Panic: Could not add the prologue node during loop shifting (2)" << endl;
		assert(0);
	}
	//
	// this works becos prologue node is a simple node. See comments above.
}

extern void removeLabel(NormalInstr *instr);

// Added by SAH 
// Note: This version of loop shifting shifts instructions across the loop iteration.
//
void shiftInstrs(BaseHTGNode *someNode, int numInstrs)
{
	// Loop shifting occurs as follows:
	//
	// Assume a piece of loop code:
	//
	// L1 
	// Instr 1
	// ...
	// Instr m
	// ...
	// Instr n
	// IF cond GOTO L1
	//
	//
	// After shifting m times it becomes:
	//
	// Instr 1
	// ...
	// Instr m
	// L1
	// Instr m+1
	// ...
	// Instr n
	// IF !cond GOTO L2
	// Instr 1
	// ...
	// Instr m
	// GOTO L1
	// L2

   // assume that all checking has been done before.

	BaseHTGNode *headNode = (dynamic_cast<LoopHTGNode*>(someNode))->getLoopHead();

	// As of now, we only shift the loop if the head node is a simple node.
	//
	if (!headNode->isSimpleNode())
	{
		cout << "Warning: Did not shift loop cos the head node was not a simple node" << endl;
		return;
	}

	// first create new htgnodes with the first n operations
	//
	BaseHTGNode *prologueNode = headNode->copyHTGBody(numInstrs);
	BaseHTGNode *shiftedNode = headNode->copyHTGBody(numInstrs);

	// remove labels in the prologue node, if any.
	//
	removeLabel((NormalInstr *)((prologueNode->getFirstBB())->getFirstInstr()));

	// now, we add the shifted node into the loop node.
	// Note: This makes the loop unstructured.

	BasicBlockNode *copyLastBB = shiftedNode->getLastBB();
	BasicBlockNode *lastBB = someNode->getLastBB();
	BasicBlockNode *succBB = lastBB->getNextSucc();
	
	// Note that we are not linking the internal BBs of the copyNode becos we are assuming tha
	// that the copyNode is a Simple HTG Node.
	// In case we allow shifting of non Simple Nodes, we need to add code that links all 
	// the BBs of the copied node.
	
	if (copyLastBB != NULL)
		copyLastBB->appendBB(lastBB, _FIRST_SUCC_, _TREE_ARC_);

/*	
	if (succBB != NULL)
	{
		succBB->appendBB(copyLastBB, _SECOND_SUCC_, _TREE_ARC_);
		
		// originally if the last BB contained an if stmt, then we make the last bb point to succ BB
		// However, if the last BB was an unconditional goto to the head of the loop, then it is changed
		// to point to copylastBB.
		//
		if (((NormalInstr *)copyLastBB->getLastInstr())->hasIfOp())
			succBB->appendBB(lastBB, _SECOND_SUCC_, _FORWARD_ARC_);
		else
			copyLastBB->appendBB(lastBB, _SECOND_SUCC_, _FORWARD_ARC_);
	}
*/

   //
   // Now, we need to add the internal nodes of the newly created HTG node to the
   // original loop.
   
   BaseHTGNode *tempPtr = shiftedNode;
   while (tempPtr != NULL)
   {
	   ((LoopHTGNode *) someNode)->appendNode(tempPtr);
	   tempPtr = tempPtr->getNext();
   }

   // We then delete the shifted instructions (starting from the loop head).
   // These instructions have already been shifted to the loop tail.
   //
   headNode->deleteHTGBody(numInstrs);

	// next insert the prologue node into the htgnode before the loop node.
	//
	insertAndLinkPrologue(someNode, prologueNode);

	// point the successor of the shifted node to the headNode.
	//
	BasicBlockNode *firstBB = headNode->getFirstBB();
	if (firstBB == NULL) /* signal error */ { assert(0); }

	firstBB->appendBB(copyLastBB, _FIRST_SUCC_, _BACK_ARC_);

	// now, remove the label from the loop head.
	// The original label is no longer needed.
	//
	removeLabel((NormalInstr *)(firstBB->getFirstInstr()));

   // After renaming the destination values, make sure that the symbol table is properly
   // updated to account for the newly created values.
   //
   globalRenameTable.resetSymbolTable();
   globalSymbolTable->setStartNums();

   // Now, reset the control flow graph.
   //
   // 1) Re-compute the control-flow predecessors.
   // 2) Re-do DFS numbering.

   // First, we recompute the CFPreds.
   //
   currRoutine->buildCFPreds();

   //
   // also, the symbol table may need to be updated again.
   //
   globalSymbolTable->setStartNums();
   
   // Also, since we added new basic blocks we need to do a dfs ordering of the bbs.
   //	
   BinGraphDFS<ControlFlowGraph, BasicBlockNode, BasicBlockEdge> basicBlockDFS;
   currRoutine->getCFG().resetDFSNums();
   basicBlockDFS.doDFS(currRoutine->getCFG());   
   currRoutine->getCFG().orderDFS();

   // Call maintainUD function on someNode.
   //
   // maintainUD(someNode);
}

extern char *getNewLabel();

int shiftSimpleOnce(BaseHTGNode *someNode)
{
   // assume that all checking has been done before.

	BaseHTGNode *headNode = (dynamic_cast<LoopHTGNode*>(someNode))->getLoopHead();
	// Check to see if the head node is a simple node.
	//
	// Also, check to see that the bb inside the simple node does not contain an if oper.
	//
	// We need to do this becos if heads are migrated up one level in the HTG hierarchy
	// and thus may become the head of a loop Node.
	//
	// Another check that is performed is to see if the loop has more than one HTG node.
	// If it doesn't we currently do not perform shifting.
	//
	// ######################################
	//    Note: The above assumption is a
	//	  very important restriction.
	// ######################################

	// ************************************************
	// TODO: Implement the second part of the checking.
	// ************************************************
	//
	if (!headNode->isSimpleNode())
	{
		cout << "Warning: Currently, shifting is not supported if the loop head node is not a simple node" << endl;
		return _NO_;
	}

	// Check that the size of the loop is more than one htg node.
	//
	if ((((LoopHTGNode *)someNode)->getLoopList()).numItems() <= 1)
	{
		cout << "Warning: Currently, shifting does not occur if the body of the loop contains only one HTGNode" << endl;
		return _NO_;
	}

   BaseHTGNode *copyNode = headNode->copyHTGBody();
   BasicBlockNode *copyLastBB = copyNode->getLastBB();
   BasicBlockNode *lastBB = someNode->getLastBB();
   BasicBlockNode *succBB = lastBB->getNextSucc();
   
   // Note that we are not linking the internal BBs of the copyNode becos we are assuming tha
   // that the copyNode is a Simple HTG Node.
   // In case we allow shifting of non Simple Nodes, we need to add code that links all 
   // the BBs of the copied node.

	if (copyLastBB != NULL)
		   copyLastBB->appendBB(lastBB, _FIRST_SUCC_, _TREE_ARC_);
   
   if (succBB != NULL)
      {
	 succBB->appendBB(copyLastBB, _SECOND_SUCC_, _TREE_ARC_);

	 // originally if the last BB contained an if stmt, then we make the last bb point to succ BB
	 // However, if the last BB was an unconditional goto to the head of the loop, then it is changed
	 // to point to copylastBB.
	 //
	 if (((NormalInstr *)copyLastBB->getLastInstr())->hasIfOp())
		succBB->appendBB(lastBB, _SECOND_SUCC_, _FORWARD_ARC_);
	 else
		copyLastBB->appendBB(lastBB, _SECOND_SUCC_, _FORWARD_ARC_);
      }

   //
   // Also, the basicblock pointed to by copyLastBB (i.e. the backward arc from
   // the new last BB) may need to be properly set.

   // Note: we do not need to rename any dest values. (Altho, this would mean that some variables
   // would have two definitions (one inside the loop and the other outside the loop)).
   //

   //
   // Now, we need to add the internal nodes of the newly created HTG node to the
   // original loop.
   
   BaseHTGNode *tempPtr = copyNode;
   while (tempPtr != NULL)
   {
	   ((LoopHTGNode *) someNode)->appendNode(tempPtr);
	   tempPtr = tempPtr->getNext();
   }

   // Note: We do not need to insert any new moves (because we are only shifting simple htg nodes).
   // Note: We also do not need to rename any source values (because no dest values have been changed
   // and no new defs are being created)

   // Now,we need to remove the loop head node and place it outside the loop.
   // After that, we need to add an (unconditional) jump statement that jumps to the new head of the loop.

   BaseHTGNode *newLoopHead = headNode->getNext();

   if (newLoopHead == NULL) // something's wrong
   {
	   cout << "Encountered NULL New Loop Head while shifting" << endl;
	   assert(0);
   }

   BaseHTGNode *tempNode = someNode->getPrev();
   if ((tempNode != NULL) && (tempNode->getNext() == someNode))
	   tempNode->setNext(headNode);

   tempNode = someNode->getUp();
   if (tempNode != NULL)
	   tempNode->insertBefore(headNode, someNode);
   else // head node should be inserted to the top level of the HTG hierarchy.
   {
	   MyLinkedList<BaseHTGNode *> *topPtr = currRoutine->getHTG();

	   if (topPtr == NULL) // WTF ??? If this is true, then something is seriously wrong. 
	   {
		   cout << "Error: No top-level HTG found in the routine" << endl;
		   assert(0);
	   }

	   tempNode = someNode->getPrev();

	   // Now, if tempNode == NULL, then the loop was the first node in the routine.
	   //
	   if (tempNode != NULL)
	   {
		   topPtr->insertBefore(headNode, someNode);
	   }
	   else
	   {
		   topPtr->prepend(headNode);
	   }
   }

   (dynamic_cast<LoopHTGNode *>(someNode))->setLoopHead(newLoopHead);
   headNode->setNext(someNode);
   
   // Now, remove the previous head node from the loop list inside the loop HTGNode.

   ((LoopHTGNode *)someNode)->getLoopList().removeFirst();

   BaseArgument *newLabel;
   char *labelStr = getNewLabel();
   newLabel = new LabelArg(labelStr);

   newLoopHead->insertLabel(newLabel);

   BasicBlockNode *newLoopHeadBB = newLoopHead->getFirstBB();
	BaseOperation *newgoto;
	newgoto = new GotoOp();
	((GotoOp*)newgoto)->addLabelArg(newLabel);
	((GotoOp*)newgoto)->setTargetInstr(newLoopHeadBB->getFirstInstr());

   copyLastBB->insertAsLastOp(newgoto);
   newLoopHeadBB->appendBB(copyLastBB, _FIRST_SUCC_, _BACK_ARC_);

   // Call maintainUD function on someNode.
   //
   maintainUD(someNode);

   return _YES_;

}

void shiftSimple(BaseHTGNode *someNode, int numTimes)
{
	for (int i = 0; i < numTimes; i++)
	{
		if (shiftSimpleOnce(someNode) == _NO_)
			return;
	}
}

void shift(BaseHTGNode *someLoop, int numTimes)
{
   if (someLoop == NULL) return;
   if (!someLoop->isLoopNode()) 
      {
	 // Error. 
	 cout << "Panic: Recieved non-loop node for shifting";
	 exit(1);
      }

   /*
   if (((LoopHTGNode *)someLoop)->getPtrToLoopDS()->hasInnerLoop())
      {
	 cout << "Shifting is not supported for loops with inner loops" << endl;
	 return;
      }
   */

   if (numTimes <= 0)
      {
	 cout << "Loop shifting recieved invalid shift count" << endl;
	 exit(1);
      }
   
   shiftInstrs(someLoop, numTimes);
}


