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
// $Id: Routine.cc,v 1.25 1998/04/14 18:29:07 ahalambi Exp $
//
// File:  		Routine.cc
// Created:		Wed Nov 20, 96
// Last modified: 	Wed Nov 20, 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: INSTRUCTION

#include "stdafx.h"

#include "Routine.h"

#include "ComputeOp.h"
#include "PhiOp.h"
#include "IfOp.h"
#include "UDProperty.h"
#include "IBSLoopProperty.h"
#include "SimpleHTGNode.h"
#include "IfHTGNode.h"

#include "MutPass.h"

#include "IRTraversal.h"
#include "symtabs.h"
#include "Program.h"
#include "BaseOperation.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

const int INIT_NUM_GOTOS = 10;

#ifdef COUNT_ROUTINES
int Routine::_numRoutines = 0;	
#endif

extern Program *curProgram;
extern Routine *currRoutine;

extern void printIndent(FilePointer & fp, int indent);

Routine::~Routine()
{
	free(_name);
	_cfg.deleteBBList();
	_loops.deleteListOfLoops();
	
	_htg.removeAll();
	
	MyLinkedListIterator<BaseInstruction *> *instrIter = _instrList.elementsForward();
	while (instrIter->hasMoreElements()) {
		BaseInstruction *instr = instrIter->currentElement();

		delete instr;

		instrIter->nextElement();
	}
	delete instrIter;
	
	_instrList.removeAll();
	
	_prologue.clear();
	_epilogue.clear();
	
	if (iWalker) delete iWalker;
	if (oMapper) delete oMapper;
	
#ifdef COUNT_ROUTINES
	_numRoutines--;
#endif
}

extern void unroll(BaseHTGNode*, int);
extern void shift(BaseHTGNode*, int);

void Routine::buildInstrFlow()
{
	MyLinkedListIterator<BaseInstruction *> *instrIter;
	
	// The first pass in building up the instruction list is
	// to set up an array which holds pointers to instructions
	// which have labels.
	// This array will be used later to dereference labels
	// specified in "control" operations.
	
	// Begin first pass
	
	instrIter = (*this).instrListIteratorForw();
	
	Array<BaseInstruction *> labelArray(INIT_NUM_GOTOS, 1);
	
	while (instrIter->hasMoreElements())
	{
		if ((instrIter->currentElement())->hasLabel())
		{
			labelArray.insertElement(instrIter->currentElement());
		}
		instrIter->nextElement();
	}
	
	delete instrIter;
	
	// End first pass
	
	ArrayIterator<BaseInstruction *> *labelArrayIter;
	
	labelArrayIter = labelArray.elements();
	
	instrIter = (*this).instrListIteratorForw();
	
	BaseInstruction *currInstr, *nextInstr;
	
	char *labelStr;
	
	while (instrIter->hasMoreElements())
	{
		currInstr = instrIter->currentElement();
		
		if (currInstr->getRootOpType() == DATA_OP)
		{
			try
			{
				instrIter->nextElement();
				if  (!instrIter->hasMoreElements()) break;
				
				nextInstr = instrIter->currentElement();
				
				(currInstr->getRootOpSlot().getPtrToNextSlot())->getPtrToOperation()->setTargetInstr(nextInstr);
			}
			catch (Exception *n)
			{
				//NOS
				Exception* dummy=n;
				
				// do nothing. We have reached the end of the instr list.
				
				break;
			}
		}
		else
		{
			if (currInstr->getRootOpType() == CONTROL_OP){
				try{
					instrIter->nextElement();
					if (instrIter->hasMoreElements())
						nextInstr = instrIter->currentElement();
					else
						nextInstr = NULL;
					
					(currInstr->getRootOpSlot().getPtrToFalseBranch())->getPtrToOperation()->setTargetInstr(nextInstr);
					
					// Now, we need to set the true branch.
					//
					if (!(currInstr->getPtrToRootOperation())->isCallRetOp())
					{
						// if it is a call/ret op, there is a separate pass wherein we assign the correct flow op target.
						//
						// That pass is "initCallPass(Program *prog)" in InitCall.cpp
						//
						// else, look at the label and determine the correct instruction to point to.
						
						labelStr =(char *) (currInstr->getPtrToRootOperation())->getTargetLabelStr();
						
						nextInstr = NULL;
						while (labelArrayIter->hasMoreElements())
						{
							//				 cout << "Lab: " << labelArrayIter->currentElement()->getPtrToLabelArg()->lName();
							//				 cout << "    Instr: " << labelArrayIter->currentElement();
							//				 cout << endl;
							
							if (((labelArrayIter->currentElement())->getPtrToLabelArg())->isEqual(labelStr))
							{
								nextInstr = labelArrayIter->currentElement();
								break;
							}
							labelArrayIter->nextElement();
						}
						labelArrayIter->reset();
						
						if (nextInstr != NULL)
						{
							(currInstr->getRootOpSlot().getPtrToTrueBranch())->getPtrToOperation()->setTargetInstr(nextInstr);
						}
						else
						{
							// Something's gone wrong. There is an operation with a wrong label.
							
							assert(0);
						}
					}//if
				}//try
				catch (Exception *n)
				{
					// do nothing. We have reached the end of the instr list.
					
					cout << "Panic: Wrong Label encountered. " << endl;
					assert(0);
#ifdef WIN32
					cout << "Exception: " << *n;
#endif
					
					break;
				}
			}//if
			else{ // if (currInstr->getRootOpType() == FLOW_OP)
				instrIter->nextElement();
				try{
					labelStr = (char *)(currInstr->getPtrToRootOperation())->getTargetLabelStr();
					
					nextInstr = NULL;
					while (labelArrayIter->hasMoreElements()){
						//				 cout << "Lab: " << labelArrayIter->currentElement()->getPtrToLabelArg()->lName();
						// 				 cout << "    Instr: " << labelArrayIter->currentElement();
						//				 cout << endl;
						
						if (((labelArrayIter->currentElement())->getPtrToLabelArg())->isEqual(labelStr)){
							nextInstr = labelArrayIter->currentElement();
							break;
						}
						labelArrayIter->nextElement();
					}
					labelArrayIter->reset();
					
					
					if (nextInstr != NULL){
						(currInstr->getRootOpSlot()).getPtrToOperation()->setTargetInstr(nextInstr);
					}
					else{
						// Something's gone wrong. There is an operation with a wrong label.
						
						cout << "Panic: Wrong Label encountered. " << endl;
						assert(0);
					}
				}
				catch (Exception *n){
					//NOS
					Exception* dummy=n;
					// do nothing. We have reached the end of the instr list.
					
					break;
				}
			}//else
		 }//else
      }//while
      delete labelArrayIter;
      delete instrIter;
}

WalkInstr wI(WALK_BACKWARDS, WALK_CF, NORMAL_MODE);

void setMultiplePred(Routine *rout)
{
	wI.initWalk(rout);
}

int hasMultiplePred(BaseInstruction *instr)
{
	if ((wI.getNext(instr)).size() > 1)
		return _YES_;
	return _NO_;
}

void resetMultiplePred()
{
	wI.reset();
}

int isBBLeaderType(BaseInstruction *currInstr, BaseInstruction *prevInstr)
{
	if ((currInstr == NULL) || (prevInstr == NULL))
		return _NO_;

	// rule b and c. See below for explanation.
	//
	if ((currInstr->hasLabel() == _YES_)||
		(prevInstr->getRootOpType()==CONTROL_OP)) 
	{
		return _YES_;
	}

	// rule d.

	if (hasMultiplePred(currInstr)) return _YES_;

	return _NO_;
}

InstrList leaderList;

int isALeaderInstr(BaseInstruction *instr)
{
	InstrListIter i;

	for (i = leaderList.begin(); i != leaderList.end(); i++)
	{
		if (instr == *i)
			return _YES_;
	}
	return _NO_;
}

// This method contains an implementation of the algorithm for grouping
// a list of instructions into basic blocks. The algorithm is given
// below: 
//
// Algorithm: Partition into basic blocks.
// Input: A sequence of N-addr statements (within a routine).
// Output: A list of basic blocks with each N-addr statement in
// exactly one block.
//
// Method:
//
// 1. Determine the set of leaders (the first statements of basic
//    blocks) according to the following rules:
//
//    a) The first statement is a leader.
//    b) Any statement that is the target of a conditional or
//       unconditional goto is a leader.
//    c) Any statement that immediately follows a goto or conditional
//       goto statement is a leader.
//
// Addition by SAH on Mon, Dec 06, 99
//
//    d) Any statement that has more than one predecessor is a leader.
//
// 2. For each leader, its basic block consists of the leader and all
//    statements upto but not including the next header, or the end of
//    the list of N-addr statements.
//
// Note that this algorithm is taken from the Dragon Book.
//
//
void Routine::buildCFG()
{
	//
	// This method looks at the instruction_list in the routine and
	// builds the CFG structure (i.e. it fills up the _cfg data
	// member).
	// The _cfg data member contains a pointer to list_of_basicblocks. 
	// This method first creates a list of basic block and then uses
	// the methods provided by the ControlFlowGraph class to initialize
	// the pointer.
	//
	
	cout << endl << "#++++++++++++++++++++++++++++++++#" << endl;
	cout << endl << "Building Control Flow Graph (CFG)." << endl;

  //**************************************
  // DEBUG_CODE
  //**************************************

  int temp;
  if (strcmp(_name, "_locate") == 0)
  {
	  temp = 1;
  }

  //**************************************
  // END DEBUG_CODE
  //**************************************

  int isCall = 0;
	BasicBlockNode tempBB;
	
	LinkedList<BasicBlockNode> *bbList;
	
	bbList = new LinkedList<BasicBlockNode>();
	
	if (_instrList.isEmpty())
		return;

 // Try to make this re-entrant. SAH Oct 28, 99
 //
 _cfg.deleteBBList();

	resetMultiplePred();

	setMultiplePred(this);

	MyLinkedListIterator<BaseInstruction *> *instrIter;
	instrIter = (*this).instrListIteratorForw();
	//
	// This creates an iterator over the list of instructions in the
	// routine. 
	
	// Make the first statement a leader (See Comment Above - 1a)
	// i.e. create a new BBNode, add it to the list, and then fill in
	// its leader.
	
	tempBB.initLeader(instrIter->currentElement());
	//
	// Filling in tempBB with the address of the first instruction in
	// the routine.
	
	bbList->append(tempBB);
	//
	// Note that this basicblock is the start block. The way this
	// algorithm is implemented, this block is the head of the list and
	// therefore the start block can be easily determined. In case
	// there is a change in the implementation, then a pointer to the
	// start block will have to be stored (and the cfg structure
	// initialized using it).
	// Also, note that this is the place to mark the basic block as a
	// _START_ type node.
	
	BaseInstruction *prevInstr = instrIter->currentElement();
	instrIter->nextElement();
	
	int done = 0;
	while (instrIter->hasMoreElements())
	{
		// Make any target statement a leader (See Comment Above -
		// 1b) 
		//
  // instrIter->currentElement()->print();

		// This code was changed by SAH on Mon, Dec 06, 99
		//
		// Previous code is commented below
		//
//		if (((instrIter->currentElement())->hasLabel() == _YES_)||
//			(prevInstr->getRootOpType()==CONTROL_OP)) 

		if (isBBLeaderType(instrIter->currentElement(), prevInstr))
		{
			tempBB.initLeader(instrIter->currentElement());
			bbList->append(tempBB);

			// Add this instr to the leaderList
			//
			leaderList.push_back(instrIter->currentElement());
		}
		prevInstr = instrIter->currentElement();
		instrIter->nextElement();
		
		// Note the next section of code has been commented out entirely.
		// Previously, the 1b comment and 1c comment were coded as separate cases.
		// The two cases have now been combined into one in the above section.
		// The next section is left as it is (and will be removed as soon as I
		// have finished testing this thoroughly).
		
		/*********************************************************************/
		// Note: Please contact Ashok before making any changes to this code.
		/*********************************************************************/
		
		/*
		if (!instrIter->hasMoreElements()) break;
		
		  // Make any statement immediately following a goto, a leader.
		  // (See Comment Above - 1c)
		  //
		  // There are a few complications:
		  //
		  // Taking care of successive gotos.
		  // What if the instruction is a goto and also a target.
		  //
		  // To solve the second problem: the algorithm should take a
		  // look at each statement only once (i.e. at most only one
		  // BBNode is created per statement). This means that each
		  // iteration of the outer loop should advance the instruction
		  // iterator.
		  // However, this could cause the first complication to be
		  // resolved incorrectly. So, the algorithm searches for
		  // successive gotos and creates basic blocks for them. (This
		  // implies that 1c of the algorithm has been changed a bit.)
		  //
		  while ((instrIter->currentElement())->getRootOpType() == CONTROL_OP)
		  {
		  // Have to be careful about the situation when a goto
		  // is the last statement in a routine.
		  //
		  instrIter->nextElement();
		  
			if (instrIter->hasMoreElements())
			{
			tempBB.initLeader(instrIter->currentElement());
			bbList->append(tempBB);
			}
			else
			{
			
			  // reached end of routine. Break from the loop.
			  done = 1;
			  break;
			  }
			  }
			  if (!done)
			  instrIter->nextElement();
		*/
	}
	
	delete instrIter;
	//
	// Freeing up the memory taken by the iterator object.
	
	// We now have to identify the trailers.
	// To do that, we go thru the list in the reverse direction, and
	// fill in the trailers of the basic blocks one by one.
	
	instrIter = (*this).instrListIteratorBack();
	
	if (!instrIter->hasMoreElements())
	{
		// This means that the routine has no instructions.
		cout << "Panic: Routine has no instructions." << endl;
		assert(0);
	}
	
	LinkedListIterator<BasicBlockNode> *bbIter;
	bbIter = bbList->elementsForward();
	
	BaseInstruction *hdr;
	
	while (bbIter->hasMoreElements())
	{
		hdr = (bbIter->currentElement()).getFirstInstr();
		
		// Now, keep walking down the instrList until we hit upon a header instruction
		// Since the header instruction belongs to another basicblock, our trailer is
		// the previous instruction.
		//
		done = 0;
		while (!done)
		{
			if (hdr == NULL)
			{
				// error condition.
				//
				cout << "Panic: During BuildCFG, recieved an un-recognizable Control Flow structure" << endl;
				assert(0);
			}
			prevInstr = hdr;
			
			hdr = prevInstr->getFirstSucc();
			if (hdr == NULL) // i.e. prevInstr is the last instr
			{ 
				(bbIter->currentElement()).initTrailer(prevInstr);
				done = 1;
			}
			else if (prevInstr->getNextSucc() != NULL) // then prevInstr is the trailer.
			{
				(bbIter->currentElement()).initTrailer(prevInstr);
				done = 1;
			}
			else if (isALeaderInstr(hdr)) // then prevInstr is the trailer
			{
				(bbIter->currentElement()).initTrailer(prevInstr);
				done = 1;
			}
		}

		bbIter->nextElement();
	}

	delete bbIter;
	leaderList.clear();


/*************************************************************************
	(bbIter->currentElement()).initTrailer(instrIter->currentElement());
	//
	// The end of the program is always a trailer for the last basic
	// block.
	
	(bbIter->currentElement()).setToLastBB();
	
	while ((bbIter->hasMoreElements()) && (instrIter->hasMoreElements()))
	{
		BasicBlockNode* newTempBB = &bbIter->currentElement();
		try
		{
			bbIter->nextElement();
			
			// Position instrIter to point to the instruction which is the
			// leader of the temp Basic Block.
			//
			while (instrIter->currentElement() != newTempBB->getFirstInstr())
			{
				instrIter->nextElement();
			}
			//
			// Note that we are guaranteed that there will be an
			// instruction which corresponds to the leader (i.e. we do
			// not need to insert the instrIter.hasMoreElements()
			// condition here).
			
			if (!instrIter->hasMoreElements()) break;
			
			instrIter->nextElement();
			
			if (!bbIter->hasMoreElements()) break;
			
			(bbIter->currentElement()).initTrailer(instrIter->currentElement());
		}
		catch (Exception *s)
		{
			//NOS
			Exception* dummy=s;
			// no more basic blocks.
			
			break;
		}
	}
	
	delete bbIter;
	delete instrIter;
**********************************************/
	
	// Now, we need to set the edges of the basic blocks.
	
	bbIter = bbList->elementsForward();
	
	BasicBlockEdge tempEdge;
	
	LinkedListIterator<BasicBlockNode> *targetBBIter;
	targetBBIter = bbList->elementsForward();
	
	BaseInstruction *lastInstrPtr, *nextInstrPtr;
	
	while (bbIter->hasMoreElements())
	{
		lastInstrPtr = (bbIter->currentElement()).getLastInstr();
		
		if (lastInstrPtr->getRootOpType() == DATA_OP)
		{
			bbIter->currentElement().setToSimpleBB();
			
			nextInstrPtr = (lastInstrPtr->getRootOpSlot().getPtrToNextSlot())->getPtrToOperation()->getTargetInstr();
			
			if (nextInstrPtr == NULL)
			{
				bbIter->nextElement();
				continue;
			}
			
			targetBBIter->reset();
			while (targetBBIter->hasMoreElements())
			{
				if (targetBBIter->currentElement().getFirstInstr() == nextInstrPtr)
				{
					tempEdge.setNext(&(targetBBIter->currentElement()));
					break;
				}
				targetBBIter->nextElement();
			}
			
			bbIter->currentElement().setFirstEdge(tempEdge);
		}
		else
			if (lastInstrPtr->getRootOpType() == CONTROL_OP)
			{
				if ((lastInstrPtr->getPtrToRootOperation())->isCallOp())
				{
					isCall = 1;
					bbIter->currentElement().setToSimpleBB();
					nextInstrPtr = (lastInstrPtr->getRootOpSlot().getPtrToFalseBranch())->getPtrToOperation()->getTargetInstr();
				}
				else
				{
					isCall = 0;
					bbIter->currentElement().setToComplexBB();
					nextInstrPtr = (lastInstrPtr->getRootOpSlot().getPtrToTrueBranch())->getPtrToOperation()->getTargetInstr();
				}			 
				if (nextInstrPtr == NULL) 
				{
					if (!isCall)
					{
						// the true branch must point to a valid instruction.
					
//						cout << "Warning: The true branch of a Control Op points to nowhere" << endl;
					}

					bbIter->nextElement();
					continue;
				}
			

				targetBBIter->reset();
				while (targetBBIter->hasMoreElements())
				{
					if (targetBBIter->currentElement().getFirstInstr() == nextInstrPtr)
					{
						tempEdge.setNext(&(targetBBIter->currentElement()));
						break;
					}
					targetBBIter->nextElement();
				}
				
				bbIter->currentElement().setFirstEdge(tempEdge);
				
				if (!isCall)
				{
					nextInstrPtr = (lastInstrPtr->getRootOpSlot().getPtrToFalseBranch())->getPtrToOperation()->getTargetInstr();
					
					if (nextInstrPtr == NULL)
					{
						bbIter->nextElement();
						continue;
					}
					
					targetBBIter->reset();
					while (targetBBIter->hasMoreElements())
					{
						if (targetBBIter->currentElement().getFirstInstr() == nextInstrPtr)
						{
							tempEdge.setNext(&(targetBBIter->currentElement()));
							break;
						}
						targetBBIter->nextElement();
					}
					
					bbIter->currentElement().setNextEdge(tempEdge);
				}
			}
			else // if (lastInstrPtr->getRootOpType() == FLOW_OP)
			{
				bbIter->currentElement().setToSimpleBB();
				
				nextInstrPtr = (lastInstrPtr->getRootOpSlot()).getPtrToOperation()->getTargetInstr();
				
				targetBBIter->reset();
				while (targetBBIter->hasMoreElements())
				{
					if (targetBBIter->currentElement().getFirstInstr() == nextInstrPtr)
					{
						tempEdge.setNext(&(targetBBIter->currentElement()));
						break;
					}
					targetBBIter->nextElement();
				}
				
				bbIter->currentElement().setFirstEdge(tempEdge);
			}
			bbIter->nextElement();
	}

	delete bbIter;
	delete targetBBIter;

	resetMultiplePred();

	bbList->getHead().setToFirstBB();
	
	// Now, we need to initialize the _cfg data member.
	//
	_cfg.setBBList(bbList);
	_cfg.setStartNodePtr(&(bbList->getHead()));
	
	// Now, we need to do a DFS Numbering of the Nodes in the Basic
	// Block Graph.
	
	BinGraphDFS<ControlFlowGraph, BasicBlockNode, BasicBlockEdge> basicBlockDFS;
	basicBlockDFS.doDFS(_cfg);
	
//	_cfg.shortPrint(cout);
//	_cfg.print();

	_cfg.orderDFS();

	cout << endl << "Performing DFS ordering on CFG." << endl;
	cout << endl << "Done Building CFG." << endl;
	cout << endl << "#++++++++++++++++++++++++++++++++#" << endl;
}

void Routine::buildCFPreds()
{
 LinkedListIterator<BasicBlockNode> *bbIter;
 
 BasicBlockNode *currBBPtr, *succBBPtr;
 
 if (isEmptyBodyRoutine())
	 return;

 bbIter = _cfg.createBBIterForw();
 
 // first delete all the predecessor lists.
 //
 while (bbIter->hasMoreElements())
 {
	 currBBPtr = &(bbIter->currentElement());
	 (currBBPtr->getCFPred()).deletePredList();

	 bbIter->nextElement();
 }
 delete bbIter;

 bbIter = _cfg.createBBIterForw();
 
 while (bbIter->hasMoreElements())
 {
	 currBBPtr = &(bbIter->currentElement());
	 
	 if (currBBPtr->isLastBB())
  {
   // the last basic block has no successors. (which
   // implies that no other basic block can have this
   // block as its predecessor)
  }
	 else
  {
   if ((succBBPtr = currBBPtr->getFirstSucc()) != NULL)
		  succBBPtr->addUniqCFPred(currBBPtr);
   if ((succBBPtr = currBBPtr->getNextSucc()) != NULL)
		  succBBPtr->addUniqCFPred(currBBPtr);
  }
	 bbIter->nextElement();
 }
 
 delete bbIter;
}

int getSrcNum(BaseOperation *anOper)
{
 if (anOper == _NULL_) return _INVALID_;
 
 if (!anOper->isComputeOp()) return _INVALID_;
 
 if (anOper->ptrToOperand(_SOURCE_1_)->isImmediateArg()) 
  return _SOURCE_1_;
 
 char *argName = ((RegisterArg* )(anOper->ptrToOperand(_DEST_, _DEST_LIST_)))->getText();
 
 char *srcName = ((RegisterArg*)(anOper->ptrToOperand(_SOURCE_1_)))->getText();
 
 if (strcmp(argName, srcName) == 0) return _SOURCE_1_;
 
 if (anOper->hasOperand(_SOURCE_2_))
	 return _SOURCE_2_;

 return _INVALID_;
}

// Note that this function returns the Argument that is the first one in the list of definitions.
//
BaseArgument *getDefiningArg(BaseArgument *aPtr, int position)
{
	if (aPtr == NULL) return NULL;
	
	MyLinkedList<BaseOperation *> & defList = ((UDChainProperty*)aPtr->getPtrToProperty(_UDCHAIN_))->getDefList();
	
	if (defList.isEmpty()) return NULL;
	
	if (position == _DEST_)
		return (defList.getHead()->ptrToOperand(position, _DEST_LIST_));
	else
		return (defList.getHead()->ptrToOperand(position, _SRC_LIST_));
}

// This function looks at the first element in the Def list.
// 
BaseOperation *getDefiningOper(BaseOperation *aPtr, int position)
{
	if (aPtr == NULL) return NULL;
	
	MyLinkedList<BaseOperation *> & defList = ((UDChainProperty*)aPtr->ptrToOperand(position, _SRC_LIST_)->getPtrToProperty(_UDCHAIN_))->getDefList();

	// Memory Leak
	if (position == _DEST_)
		defList = ((UDChainProperty*)aPtr->ptrToOperand(position, _DEST_LIST_)->getPtrToProperty(_UDCHAIN_))->getDefList();
	
	if (defList.isEmpty()) return NULL;
	
	return (defList.getHead());
}

extern int isImmediateLikeArg(BaseArgument *arg);

// Note:
// the getDefiningArg and getDefiningOper methods have to be changed to account for the fact that 
// the definitions are no longer unique, but actually form a list.
//
void Routine::genIBSForLoop(Loop *aLoop)
{
	IBSLoopProperty lProp;
	
	if (aLoop == _NULL_) return;
	
	if (aLoop->isIrreducible())
	{
		// cannot tackle this situation as of now.
		
		return;
	}
	
	MyLinkedListIterator<int> *tailIter;
	tailIter = aLoop->createTailListIterForw();
	
	// Currently I am assuming that the loop must be regular, which
	// implies that it has only one tail BasicBlock.
	
	int bbID = tailIter->currentElement();
	
	BasicBlockNode *tailPtr = _cfg.getBBPtr(bbID);
	
	if (tailPtr == _NULL_) 
	{
		// Error condition.
		
		cout << "Panic: Something wrong with Basic Block ID" <<
			endl;
		return;
	}
	
	BaseInstruction *backEdge = tailPtr->getLastInstr();
	BaseOperation *ifOper, *indexOper;//, *boundsOper, *stepOper;
	BaseArgument *startBoundsArg, *endBoundsArg;
	BaseArgument *stepArg, *tempArg; 
	
	MyLinkedList<BaseArgument *> *indexList;
	indexList = new MyLinkedList<BaseArgument *>();
	
	if (((ifOper = backEdge->getPtrToRootOperation()) != _NULL_) &&
		(ifOper->isIfOp()))
	{
		indexOper = getDefiningOper(ifOper, _SOURCE_1_);
		indexOper = getDefiningOper(indexOper, _SOURCE_1_);
		//
		// We assume that UD has already been done.
		
/*
		indexOper->shortPrint(cout);
		cout << endl;
*/

		if (indexOper == _NULL_) 
		{
			// error condition.
			cout << "Panic: DEF was null" << endl;
			assert(0);
		}
		
		tempArg = indexOper->ptrToOperand(_SOURCE_2_);
		
		if (isImmediateLikeArg(tempArg))
		{
			// This is definitely the end bounds ptr.
			
			endBoundsArg = tempArg;
		}
		else
		{
			// Now, we cannot be sure if this is the end bounds
			// ptr.
			
			// Currently, we assume that it is the end bounds ptr.
			
			endBoundsArg = getDefiningArg(tempArg, _DEST_);
		}
		
		indexOper = getDefiningOper(indexOper, _SOURCE_1_);
		//
		// This indexOper now contains the definition of the index
		// variable.
		
		tempArg = indexOper->ptrToOperand(_SOURCE_2_); 
		//
		// This should be the step argument.
		
		if (isImmediateLikeArg(tempArg))
		{
			// This is definitely the step argument.
			
			stepArg = tempArg;
		}
		else
		{
			// Now we cannot be sure that this is the step
			// argument.
			
			// Currently we assume that this must be the step
			// argument. 
			//
			// This should be modified s.t the actual step
			// argument is a phi argument. 
			
			stepArg = getDefiningArg(tempArg, _DEST_);
		}
				
		int indexOperandNum;
		
		while (indexOper != _NULL_)
		{
/*
			indexOper->shortPrint(cout);
			cout << endl;
*/

			tempArg = indexOper->ptrToOperand(_DEST_, _DEST_LIST_);
			
			indexList->append(tempArg);
			
			if (indexOper->hasOperand(_SOURCE_1_))
			{
				tempArg = indexOper->ptrToOperand(_SOURCE_1_);

				if (isImmediateLikeArg(tempArg))
				{
					// this is the start bounds.
					//
					startBoundsArg = tempArg;
					break;
				}
			}
			if ((indexOperandNum = getSrcNum(indexOper)) ==
				_INVALID_)
				break;
			
			indexOper = getDefiningOper(indexOper, indexOperandNum);
		}
				
		// We now add the property to the loop.
		
		lProp.setIndex(indexList);
		lProp.setStartBounds(startBoundsArg);
		lProp.setEndBounds(endBoundsArg);
		lProp.setStep(stepArg);
		
		aLoop->addProperty(lProp);
	}
}

void Routine::genIBSForLoopList(LoopList & aList)
{
 MyLinkedListIterator<Loop *> *loopIter;
 loopIter = aList.loopIterForw();
 
 if (loopIter == NULL) return;

 Loop *curLoop;
 
 while (loopIter->hasMoreElements())
 {
	 curLoop = loopIter->currentElement();
	 
	 if (curLoop == NULL) 
  {
   // Error condition.
   //
   cout << "Panic: Encountered Null Loop" << endl;
   continue;
  }
	 
	 genIBSForLoop(curLoop);

	 // Debug Code. Added by SAh on 25th, Sept, 00.
	 //
	 cout << endl << "Printing IBS for Loop" << endl;
	 curLoop->print();
	 curLoop->printIBS();

	 genIBSForLoopList(curLoop->getRefToInnerLoopList());
	 
	 loopIter->nextElement();
 }
 
 delete loopIter;
}      

void Routine::genIBS()
{
 genIBSForLoopList(_loops);
}

BaseInstruction *Routine::getFirstInstr()
{
	if (_instrList.isEmpty())
		return NULL;
	return (_instrList.getHead());
}

BaseInstruction *Routine::getLastInstr()
{
	if (_instrList.isEmpty())
		return NULL;
	return (_instrList.getTail());
}

int Routine::isEmptyBodyRoutine()
{
	if (_instrList.isEmpty())
		return _YES_;
	return _NO_;
}

Vector<BasicBlockNode *> visitedVector;

void addToVisitedVector(BasicBlockNode *nd)
{
	visitedVector.push_back(nd);
}

int hasBeenVisited(BasicBlockNode *nd)
{
	Vector<BasicBlockNode *>::iterator iter1 = visitedVector.begin();
	Vector<BasicBlockNode *>::iterator iter2 = visitedVector.end();

	while (iter1 != iter2)
	{
		if (nd == *iter1) return _YES_;
		iter1++;
	}
	return _NO_;
}

typedef BasicBlockNode *BasicBlockNodePtr;

// Note:
// Whenever this function is called for a routine,
// it should appear like this:
//
// <LOOP OVER ROUTINE BASICBLOCKS>
//		if (!hasBeenCalled(bbnode))
//			... isUnstructredIf(bbnode) ...
//      .........
//
int isUnstructuredIf(BasicBlockNode *nd, BasicBlockNodePtr & nBB)
{
	if (hasBeenVisited(nd))
	{
		// We are trying to determine if an already visited if-block is 
		// unstructured or not. This could be an error.
		//
		// Conservatively return unstructured.

		return _YES_;
	}

	// First of all add to visited vector
	//
	addToVisitedVector(nd);

	BasicBlockNode *trueBB, *falseBB, *tempBB, *joinBB, *nextBB;

	nextBB = NULL;
	if (nd == NULL) 
      {
		nBB = NULL;
		return _NO_;
      }

	tempBB = nd;
   trueBB = nd->getFirstSucc();
   falseBB = nd->getNextSucc();

   int ifHeadDFSNum = nd->getDFSNum();

   // For a structured if stmt, the if head should dominate all the other nodes.
   // So, we need to check for two conditions
   //
   // 1) The DFS num of the join node is greater than the DFS num of the if head.
   // 2) If the IF stmt contains a loop exit node, then the
   //    DFS num of the loop head must be greater than the DFS num of the if head.
   //
   // Note: These two conditions are necessary. I haven't really checked to see if
   // they are sufficient.

   while ((falseBB != NULL) && (falseBB->getDFSNum() > tempBB->getDFSNum()))
   {
	   tempBB = falseBB;

	   if (falseBB->isIfHeadType())
	   {
		   if (isUnstructuredIf(falseBB, nextBB)) return _YES_;
		   falseBB = nextBB;
	   }
	   else if (falseBB->isLoopTailType())
	   { 
		   // Then check to see if the DFS num of loop head is greater than
		   // DFS num of the if head.

		   BasicBlockNode *loopHead = falseBB->getFirstSucc();

		   if ((loopHead != NULL) && (loopHead->getDFSNum() > ifHeadDFSNum))
		   {
			   // everything checks out OK.

			   falseBB = falseBB->getNextSucc();
		   }
		   else
		   {
			   // This could be an unstructured routine.

			   nBB = NULL;
			   return _YES_;
		   }
	   }
	   else if (falseBB->isCallRetType())
	   {
		   // Then the first succ is set to point to the fall thru
		   // branch of the call.
		   //
		   // Note: A CALL instr may have 2 flow ops (i.e. 2 target instrs),
		   // but the basic block that incorporates the call has only 1 succ.
		   //
		   falseBB = falseBB->getFirstSucc();
	   }
	   else // if (falseBB is normal type)
	   {
		   falseBB = falseBB->getFirstSucc();
	   }
   }

   joinBB = falseBB;

   if (joinBB == NULL)
   {
	   nBB = NULL;
	   return _YES_;
   }

   while ((trueBB != NULL) && (trueBB != joinBB))
   {
	   if (trueBB->isIfHeadType())
	   {
		   if (isUnstructuredIf(trueBB, nextBB)) return _YES_;
		   trueBB = nextBB;
	   }
	   else if (trueBB->isLoopTailType())
	   { 
		   // Then check to see if the DFS num of loop head is greater than
		   // DFS num of the if head.

		   BasicBlockNode *loopHead = trueBB->getFirstSucc();

		   if ((loopHead != NULL) && (loopHead->getDFSNum() > ifHeadDFSNum))
		   {
			   // everything checks out OK.

			   trueBB = trueBB->getNextSucc();
		   }
		   else
		   {
			   // This could be an unstructured routine.

			   nBB = NULL;
			   return _YES_;
		   }
	   }
	   else if (trueBB->isCallRetType())
	   { 
		   trueBB = trueBB->getFallThruNode();
	   }
	   else // if (falseBB is normal type)
	   {
		   trueBB = trueBB->getFallThruNode();
	   }
   }

   if (trueBB == NULL)
   {
	   nBB = NULL;
	   return _YES_;
   }

   nBB = joinBB;
   return _NO_;
}

int Routine::isUnstructured(int recompute)
{
	if (!recompute)
	{
		if (_isUnstructured)
			return _YES_;
		else
			return _NO_;
	}

// Note: A routine may be considered unstructured if it has IRREDUCIBLE loops.
// However, we can encapsulate such loops into an HTG node, and hence
// we don't consider such loops as unstructured in our context.
// So, we basically need to check if all the IF stmts in the routine are structured.

	BasicBlockNode *tempNode, *nextBB;
	tempNode = _cfg.getStartNode();

	LinkedListIterator<BasicBlockNode> *iter;
	iter= _cfg.createBBIterForw();

	nextBB = NULL;
	while (iter->hasMoreElements())
	{
		tempNode = &(iter->currentElement());

		if (tempNode->isIfHeadType())
		{
			if (!hasBeenVisited(tempNode))
			{
				if (isUnstructuredIf(tempNode, nextBB))
				{
					_isUnstructured = 1;
					break;
				}
			}
		}
		iter->nextElement();
	}
	delete iter;

	if (_isUnstructured)
		return _YES_;

	return _NO_;
}


void Routine::setTargetMask()
{
 
 LinkedListIterator<BasicBlockNode> *BBIter= _cfg.createBBIterForw();
 while( BBIter->hasMoreElements()) {
  BasicBlockNode &curBB = BBIter->currentElement();
  BBIter->nextElement();
  int BBid = curBB.getDFSNum();
  BaseInstruction *lastBBInstr = curBB.getLastInstr();
  if (lastBBInstr->isNormalInstr())
   ((NormalInstr *)lastBBInstr)->setTargetMask(BBid);
 }
}

inline void unrollLoopSupportFunc(BaseHTGNode *node)
{
	// We assume that node has been checked to see if it is a loop node.

	int unrollTimes = 0;

	// Ask the user if he wants this loop to be unrolled.
//	cout << "How many times should I unroll this loop? " << endl;
//	cin >> unrollTimes;

	unrollTimes = 2;
	if (unrollTimes)
	{
		unroll(node, unrollTimes);
		//   currHTGNode->trailblaze();
		cout << "\t\t ** Done unrolling the loop **" << endl;
//		unrollTimes = 0;
	}
}

inline void shiftLoopSupportFunc(BaseHTGNode *node)
{
	// We assume that node has been checked to see if it is a loop node.

	int shiftTimes = 0;

	// Ask the user if he wants this loop to be unrolled.
//	cout << "How many times should I unroll this loop? " << endl;
//	cin >> unrollTimes;

	shiftTimes = 2;
	if (shiftTimes)
	{
		shift(node, shiftTimes);
		//   currHTGNode->trailblaze();
		cout << "\t\t ** Done shifting the loop **" << endl;
		shiftTimes = 0;
	}
}

void unrollSupportFunc(	HTGLinkedListIter *iter, int innerLoopOnly)
{
	if (iter == NULL) return;

	BaseHTGNode *currHTGNode;
	while (iter->hasMoreElements())
	{
		currHTGNode = iter->currentElement();
		if (currHTGNode->isLoopNode())
		{
			if ((innerLoopOnly) && (((LoopHTGNode *)currHTGNode)->hasInnerLoop()))
				{
					HTGLinkedListIter *innerIter = ((LoopHTGNode *)currHTGNode)->createLoopIterForw();
					unrollSupportFunc(innerIter, innerLoopOnly);
					delete innerIter;
				}
			else
				unrollLoopSupportFunc(currHTGNode);
		}
		else if(currHTGNode->isIfNode())
		{
			HTGLinkedListIter *ifIter = ((IfHTGNode *)currHTGNode)->createTrueBranchIterForw();
			unrollSupportFunc(ifIter, innerLoopOnly);
			delete ifIter;
			ifIter = ((IfHTGNode *)currHTGNode)->createFalseBranchIterForw();
			unrollSupportFunc(ifIter, innerLoopOnly);
			delete ifIter;
		}
		iter->nextElement();
	}
}

void shiftSupportFunc(HTGLinkedListIter *iter, int innerLoopOnly)
{
	if (iter == NULL) return;

	BaseHTGNode *currHTGNode;
	while (iter->hasMoreElements())
	{
		currHTGNode = iter->currentElement();
		if (currHTGNode->isLoopNode())
		{
			if ((innerLoopOnly) && (((LoopHTGNode *)currHTGNode)->hasInnerLoop()))
				{
					HTGLinkedListIter *innerIter = ((LoopHTGNode *)currHTGNode)->createLoopIterForw();
					shiftSupportFunc(innerIter, innerLoopOnly);
					delete innerIter;
				}
			else
				shiftLoopSupportFunc(currHTGNode);
		}
		else if(currHTGNode->isIfNode())
		{
			HTGLinkedListIter *ifIter = ((IfHTGNode *)currHTGNode)->createTrueBranchIterForw();
			shiftSupportFunc(ifIter, innerLoopOnly);
			delete ifIter;
			ifIter = ((IfHTGNode *)currHTGNode)->createFalseBranchIterForw();
			shiftSupportFunc(ifIter, innerLoopOnly);
			delete ifIter;
		}
		iter->nextElement();
	}
}

void Routine::testUnroll(int innerLoopOnly)
{	
	currRoutine = this;

	cout << endl << "+*****************+ Start test unroll +***************************+" << endl;
	
	HTGLinkedListIter *iter;
	iter = _htg.elementsForward();
	while (iter->hasMoreElements())
	{
		unrollSupportFunc(iter, innerLoopOnly);
	}

	cout << endl << "+*****************+ Finished test unroll +***************************+" << endl;
}

void Routine::testShift(int innerLoopOnly)
{
	cout << endl << "+*****************+ Start test shift +***************************+" << endl;
	
	HTGLinkedListIter *iter;
	iter = _htg.elementsForward();
	while (iter->hasMoreElements())
	{
		shiftSupportFunc(iter, innerLoopOnly);
	}

	cout << endl << "+*****************+ Finished test shift +***************************+" << endl;
}

void Routine::mutation(void){
	cout <<"Applying mutations..." << endl;

	SetGenericMutations(1);
 
	MyLinkedListIterator<BaseHTGNode *> *iter;
	
	iter=_htg.elementsForward();
	while(iter->hasMoreElements()){
		iter->currentElement()->mutation();
		iter->nextElement();
	}
	delete iter;

	globalSymbolTable->setStartNums();

	SetGenericMutations(0);
 
	cout <<"Done applying mutations." << endl;
}

void Routine::InstrSel(void){
	const int htgtraversal=0;  
	// in general, we will traverse the CFG instead of HTG for instruction selection
	// We can add a global flag to set htgtraversal later if htgtraversal is at all needed
	cout << endl << "Applying instruction selection..." << endl;

	SetInstructionSelection(1);
 
	if(htgtraversal){
		//htg traversal
		MyLinkedListIterator<BaseHTGNode *> *iter;		
		iter=_htg.elementsForward();
		while(iter->hasMoreElements()){
			iter->currentElement()->InstrSel();
			iter->nextElement();
		}
		delete iter;
	}
	else{
		//cfg traversal
		 _cfg.InstrSel();

	}

	globalSymbolTable->setStartNums();

	SetInstructionSelection(0);
 
	cout << endl << "Done applying instruction selection." << endl;
}



void Routine::setInstrID(void){
	
	if (isEmptyBodyRoutine()) return;
	
	MyLinkedListIterator<BaseHTGNode *>*iter;
	LinkedListIterator<BasicBlockNode> *citer;
	BaseHTGNode * node;
	
	int crt=0,opcrt=0;
	printf("\nSetting instrs ID...\n");
	if(!isUnstructured(0)){
		iter=_htg.elementsForward();
		while(iter->hasMoreElements()){
			node=iter->currentElement();
			node->setInstrID(crt,opcrt);
			iter->nextElement();
		}
		delete iter;
	}
	else{
		citer= _cfg.createBBIterForw();
		
		while (citer->hasMoreElements())
		{
			(citer->currentElement()).setInstrID(crt,opcrt);
			citer->nextElement();
		}
		delete citer;
	}
	printf("Done setting instrs ID.\n");
}

void Routine::printHTG(void){
	cout << "Printing HTG for Routine: ";
 	 if (_name != NULL) cout << _name;
	 cout << endl;

	 if (isEmptyBodyRoutine()) return;

	 //setInstrID();

 MyLinkedListIterator<BaseHTGNode *>*iter;
 iter=_htg.elementsForward();
 while(iter->hasMoreElements()){
  iter->currentElement()->print(0);
  iter->nextElement();
 }
 delete iter;
}

void Routine::printC(void){
  cout << "Printing C for Routine: " << endl;
  if (_name != NULL) 
    cout << "void " << _name << "() {" << endl;
  createListOfOperands();
  
  MyLinkedListIterator<BaseHTGNode *>*iter;
  iter=_htg.elementsForward();
  while(iter->hasMoreElements()){
    iter->currentElement()->printC(0);
    iter->nextElement();
  }
  cout << "}" << endl;
} // end of fcn printC

// list of operators/variables so that they can be used by printC 
void Routine :: createListOfOperands()
{
  Symtab *operandSymTable = new Symtab();
  MyLinkedListIterator<BaseInstruction *> *instrIter;
  
  instrIter = _instrList.elementsForward();
  
  while (instrIter->hasMoreElements())
    {
      (instrIter->currentElement())->createListOfOperands(operandSymTable);
      instrIter->nextElement();
    }
  delete instrIter;
  operandSymTable->printVariables();
  delete operandSymTable;
} // end of fcn createListOfOperands

void Routine::printReturnType(FilePointer & fp)
{
	if (isMainRoutine())
		fprintf(fp, "void");
	else
	{
		// Commented by SAH on Thu, Aug 31, 2000
		//
		// fprintf(fp, "Val");

		fprintf(fp, "void");
	}
}

void Routine::printFormalParmList(FilePointer & fp)
{
	// TBD.

	if (isMainRoutine())
	{
		fprintf(fp, "(int argc, char **argv)");
	}
	else
		fprintf(fp, "(void)");
}

int getNumRegs()
{
	if (!doneSSA || doneRA)
	{
		return (RegisterFileList::totNumRegs());
	}
	else
	{
		return (globalSymbolTable->getTotNumVars());
	}
}

void Routine::irToC(FilePointer & fp)
{
	if (_name == NULL) return;

	fprintf(fp, "\n/* IR to C for Routine : %s */\n", _name+1);
	printReturnType(fp);
	fprintf(fp, " ");
	fprintf(fp, "%s", _name+1);
	printFormalParmList(fp);
	fprintf(fp, "\n{\n");

	printIndent(fp, 2);
	fprintf(fp, "int temp1; /* Intermediate used for address calculation */\n\n");
	printIndent(fp, 2);
	fprintf(fp, "Val temp2, temp3; /* Intermediates used to store values */\n\n");
	printIndent(fp, 2);
	fprintf(fp, "Val rtemp1, rtemp2, rtemp3; /* Intermediates used to store reg values */\n\n");

	// if it is the main routine, initialize the registers to all zeros.
	//
	// also, call initAll(); (initAll is used to set the system calls table)
	//
	// then, initialize the commandline args.
	//
	if (isMainRoutine())
	{
		int numRegs = getNumRegs();

/*
		printIndent(fp, 2);
		fprintf(fp, "Val zeroVal(0);\n");
		printIndent(fp, 2);
		fprintf(fp, "for (int i = 0; i < (%d * 4); i++) {\n", numRegs);
		printIndent(fp, 3);
		fprintf(fp, "RF.push_back(zeroVal);\n");
		printIndent(fp, 2);
		fprintf(fp, "}\n\n");
*/
		printIndent(fp, 2);
		fprintf(fp, "initRegFile(%d);\n\n", numRegs);
		printIndent(fp, 2);
		fprintf(fp, "setCommandLineArgs(argc, argv);\n\n");
		printIndent(fp, 2);
		fprintf(fp, "initAll();\n\n");
	}

	// now, perform CFG serialization.
	//
	serializeCFGPhase1(this);

	// next, print the serialized CFG.
	//
	irToCPhase2(fp, 2);

/*
	// Now iterate over the instruction list, and print out the individual instrs/opers.

	MyLinkedListIterator<BaseInstruction *> *instrIter;

	instrIter = _instrList.elementsForward();

	while (instrIter->hasMoreElements())
	{
		(instrIter->currentElement())->irToC(fp, 2);
		instrIter->nextElement();
	}

	delete instrIter;
*/
	// finally, print the cycle count if the routine is the main routine.
	//
	if (isMainRoutine())
	{
		printIndent(fp, 2);
		fprintf(fp, "printf(\"\\n  Cycle count = %%d  \\n\", cycle_count);\n");
	}
	
	fprintf(fp, "\n}\n");
	fprintf(fp, "/* End IR to C for Routine : %s */\n", _name+1);

}

void Routine::fixIfHead(void){
 ::fixIfHead(&_htg);
}

void Routine::deleteUDDU(void){
	NormalInstr *instr;
	MyLinkedListIterator<BaseInstruction *> *iter;
	iter=_instrList.elementsForward();
	while(iter->hasMoreElements()){
		if(iter->currentElement()->isNormalInstr()){
			instr=(NormalInstr*)(iter->currentElement());
			BasicBlockNode*bb=instr->getBasicBlock();
			BaseBBProperty *bbp;
			if(bb!=NULL && (bbp=bb->getPtrToProperty(_COMPDEF_))!=NULL)
				bb->removeProperty(*bbp);
			Array<DataOpSlot>* slots=instr->getDataOpList();
			ArrayIterator<DataOpSlot> *slotiter;
			slotiter=slots->elements();
			while(slotiter->hasMoreElements()){
				if(slotiter->currentElement().hasOper()){
					if(slotiter->currentElement().getPtrToOperation()->isComputeOp()){
						ComputeOp* op=(ComputeOp*)(slotiter->currentElement().getPtrToOperation());
						BaseArgProperty *prop;
						BaseArgument *arg;
						arg=op->ptrToOperand(_SOURCE_1_);
						if(arg!=NULL && (prop=arg->getPtrToProperty(_UDCHAIN_))!=NULL){
							arg->removeProperty(prop);
						}
						if(arg!=NULL && (prop=arg->getPtrToProperty(_DUCHAIN_))!=NULL){
							arg->removeProperty(prop);
						}
						arg=op->ptrToOperand(_SOURCE_2_);
						if(arg!=NULL && (prop=arg->getPtrToProperty(_UDCHAIN_))!=NULL){
							arg->removeProperty(prop);
						}
						if(arg!=NULL && (prop=arg->getPtrToProperty(_DUCHAIN_))!=NULL){
							arg->removeProperty(prop);
						}
						arg=op->ptrToOperand(_DEST_, _DEST_LIST_);
						if(arg!=NULL && (prop=arg->getPtrToProperty(_UDCHAIN_))!=NULL){
							arg->removeProperty(prop);
						}
						if(arg!=NULL && (prop=arg->getPtrToProperty(_DUCHAIN_))!=NULL){
							arg->removeProperty(prop);
						}
					}
				}
				slotiter->nextElement();
			}
			delete slotiter;
		}
		iter->nextElement();
	}
	delete iter;
}

void Routine::initEpilogue(OpList & a)
{
	OpListIter i;
	BaseOperation *tempOp;

	for (i = a.begin(); i != a.end(); i++)
	{
	   if ((*i) != NULL)
	   {
		   tempOp = (*i)->copy();
		   _epilogue.push_back(tempOp);
	   }
	}
}

void Routine::addEpilogueOp(BaseOperation *a)
{
	if (a == NULL) return;
	_epilogue.push_back(a->copy());
}

void Routine::initPrologue(OpList & a)
{
	OpListIter i;
	BaseOperation *tempOp;

	for (i = a.begin(); i != a.end(); i++)
	{
	   if ((*i) != NULL)
	   {
		   tempOp = (*i)->copy();
		   _prologue.push_back(tempOp);
	   }
	}
}

void Routine::addPrologueOp(BaseOperation *a)
{
	if (a == NULL) return;
	BaseOperation *temp = a->copy();
	_prologue.push_back(temp);
}

void Routine::getSavedRegList(ArgList & aList)
{
	OpListIter i;
	MemoryOp *memOp;
	for (i = _prologue.begin(); i != _prologue.end(); i++)
	{
		if (!(*i)->isMemoryOp()) continue;
		memOp = (MemoryOp *)(*i);
		if (!memOp->isStore()) continue;

		// Now, we know that the operation in the prologue is a store op.

		aList.push_back(memOp->ptrToOperand(_SOURCE_1_));
	}
}

void Routine::printCFG()
{
 cout << "Printing CFG for Routine: ";
 if (_name != NULL) cout << _name;
 cout << endl;
 
 _cfg.print();
 cout << endl;
}

void Routine::getFileName(MyString & str)
{
	MyString s(_name);

	curProgram->getFileName(s, str);
}

void Routine::rebuildCFG()
{
	// First delete all elements in the CFG DS and then call build CFG.

	_cfg.deleteBBList();

	buildCFG();
}

void Routine::rebuildHTG()
{
	// First delete all elements in the HTG DS
	//
	// The, delete and rebuild the looplist DS
	//
	// Finally, call build HTG.

	_htg.removeAll();
	_loops.deleteListOfLoops();
	buildLoopList();

	buildHTG();
}

// Currently, we go : Routine -> BasicBlock -> Instruction -> Operation -> Argument.
// Note that HTGNode is not present in the recursion.
//
void Routine::recursiveRemoveProperty(PropertyNames name)
{
	_propList.removeProperty(name);

	LinkedListIterator<BasicBlockNode> *iter;
	iter= _cfg.createBBIterForw();

	while (iter->hasMoreElements())
	{
		(iter->currentElement()).recursiveRemoveProperty(name);
		iter->nextElement();
	}
	delete iter;
}

int Routine::countOperations()
{
	MyLinkedListIterator<BaseInstruction *> *instrIter;
	instrIter = _instrList.elementsForward();

	int retcnt = 0;

	while (instrIter->hasMoreElements())
	{
		retcnt += ((NormalInstr *)(instrIter->currentElement()))->countDataOps();
		retcnt += ((NormalInstr *)(instrIter->currentElement()))->countControlOps();

		instrIter->nextElement();
	}
	delete instrIter;

	return retcnt;
}

