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
// $Id: BuildHTG.cc,v 1.13 1998/04/17 19:12:00 pgrun Exp $
//
// File:  		BuildHTG.cc
// Created:		Thu Jan 02, 97
// Last modified: 	Thu Jan 02, 97
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: INCLUDE

#include "stdafx.h"

#include "BaseArgument.h"
#include "BaseHTGNode.h"
#include "GlobalIncludes.h"
#include "BasicBlockNode.h"
#include "SimpleHTGNode.h"
#include "LoopHTGNode.h"
#include "IfHTGNode.h"
#include "Routine.h"
#include "TrackStack.h"
#include "ArchClassIncludes.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

extern Routine *currRoutine;

typedef BaseHTGNode* BaseHTGNodePtr;


Vector<int> basicBlockDFSNumArray;

TrackStack trackStack;

BasicBlockNode *buildCompoundHTG(BasicBlockNode *aPtr, BaseHTGNodePtr & currPtr);
BasicBlockNode *buildLoopHTG(BasicBlockNode *aPtr, BaseHTGNodePtr & currPtr);
BasicBlockNode *buildBreakHTG(BasicBlockNode *aPtr, BaseHTGNodePtr & currPtr);
BasicBlockNode *buildIfHTG(BasicBlockNode *aPtr, BaseHTGNodePtr & currPtr);
BasicBlockNode *buildSimpleHTG(BasicBlockNode *aPtr, BaseHTGNodePtr & currPtr);
BasicBlockNode *buildHTGEx(BasicBlockNode *aPtr, BaseHTGNodePtr & currPtr);

void Routine::buildHTG()
{
//  cout << endl << "Building HTG..." << endl;
  currRoutine = this;
  currRoutine->_htg.removeAll();

   BasicBlockNode *startPtr;
  BaseHTGNode *htgNode;
  
  //**************************************
  // DEBUG_CODE
  //**************************************

  int temp;
  if (strcmp(_name, "_inbuffer") == 0)
  {
	  temp = 1;
  }

  //**************************************
  // END DEBUG_CODE
  //**************************************

   if (isEmptyBodyRoutine()) return;

   if (isUnstructured())
   {
	   // Do not build the HTG for this routine.
	   //
	   // Note: This solution is pessimistic.
	   //
	   // It should be possible to identify the region of unstructuredness
	   // and build HTG around it.

	   cout << "#================================================================#" << endl;
	   cout << "    Routine " << _name << " is an unstructured routine." << endl;
	   cout << "    Skipping build HTG for this routine" << endl;
	   cout << "#================================================================#" << endl;

	   return;
   }

   startPtr = _cfg.getStartNode();
   int totNumBBs = _cfg.numBasicBlocks();
   
   if (startPtr == NULL)
      {
	 cout << "Panic: Build HTG called on empty graph." <<endl;
	 assert(0);
      }

   basicBlockDFSNumArray.clear();
   for (int i = 0; i < totNumBBs; i++)
      {
	 basicBlockDFSNumArray.push_back(0);
      }
   
   BasicBlockNode *nextBB;
   BaseHTGNode *tempNodePtr,*nextNodePtr;
   nextBB = buildHTGEx(startPtr, htgNode);

   nextNodePtr=htgNode;

   while (nextBB != NULL)
      {
	 tempNodePtr = nextNodePtr;
	 nextBB = buildHTGEx(nextBB, nextNodePtr);
	 tempNodePtr->setNext(nextNodePtr);
      }
  BaseHTGNode *n=htgNode;
  while(n!=NULL){
    _htg.append(n);
    n=n->getNext();
  }

//  fixIfHead();
//  cout << endl << "Done building HTG." << endl;
}

BasicBlockNode *buildCompoundHTG(BasicBlockNode *aPtr, BaseHTGNodePtr & currPtr)
{
   if (aPtr == NULL)
      {
	 cout << "Warning: Tried to build Compound HTG Node on a null basic block";
	 cout << endl;
	 
	 return NULL;
      }
/*   	 
   if (aPtr->isSimpleType()) 
      {
	 // Note that we do not call the isSimpleBB method.
	 // This is because we are not interested in knowing whether
	 // the basic block is simple or not.
         // We are interested in knowing whether the Basic Block contains
	 // only simple instructions or if it contains some complex
	 // instructions (like call, if head, loop head, if join, ...)

	 return buildSimpleHTG(aPtr, currPtr);
      }
*/
   if (aPtr->isLoopHeadType())
      {
	 return buildLoopHTG(aPtr, currPtr);
      }
   else if (aPtr->isIfHeadType())
      {
	 return buildIfHTG(aPtr, currPtr);
      }
   else
      {
	 return NULL;
      }
}

BasicBlockNode *buildSimpleHTG(BasicBlockNode *aPtr, BaseHTGNodePtr & currPtr)
{
   if (aPtr == NULL) return NULL;

//   cout << " Building SimpleHTGNode for BB : " << aPtr << endl;
   
   currPtr = new SimpleHTGNode(aPtr);
 
   basicBlockDFSNumArray[aPtr->getDFSNum() - 1] = 1;
   
   if(aPtr->getFirstEdge()!=NULL && !(aPtr->getFirstEdge())->isBackArc())
     return (aPtr->getFirstSucc());
   else if(aPtr->getNextEdge()!=NULL && !(aPtr->getNextEdge())->isBackArc())
     return (aPtr->getNextSucc());
   else 
	   return NULL;
}

int isAfterLoop(BasicBlockNode *aptr, BaseArgument *larg)
{
	int retVal = _NO_;
	BaseInstruction *currInstr;
	int compare = 0;
	
	if (aptr == NULL) return _NO_;
	BaseInstruction *fInstr = aptr->getFirstInstr();
	
	MyLinkedListIterator<BaseInstruction *> *instrIter;
	instrIter = currRoutine->instrListIteratorBack();
	
	while (instrIter->hasMoreElements())
	{
		currInstr = instrIter->currentElement();
		instrIter->nextElement();
		if (currInstr == fInstr)
		{
			compare = 1;
			break;
		}
	}
	if (compare == 1)
	{
		currInstr = instrIter->currentElement();
		if (currInstr->getOpWithArg(larg, CONTROL_OP) != NULL)
			retVal = _YES_;
		if (currInstr->getOpWithArg(larg, FLOW_OP) != NULL)
			retVal = _YES_;
	}	
	delete instrIter;
	
	return retVal;
}

int isBreak(BasicBlockNode *aptr, BaseArgument *larg)
{
	// First check to see if the BB ptr is an If type.
	// The check to see if the instr 
	// prior to the if target is the tail of the loop.

	if (aptr->isIfHeadType())
	{
		if (isAfterLoop(aptr->getFirstSucc(), larg)) return _YES_;
	}
	return _NO_;
}

BasicBlockNode *buildBreakHTG(BasicBlockNode *aPtr, BaseHTGNodePtr & currPtr)
{
	if (aPtr == NULL) 
	{
		cout << "Warning: Trying to build Break HTG Node on NULL BBlock" << endl;
		return NULL;
	}
	buildSimpleHTG(aPtr, currPtr);

	// Currently, we expect a break stmt to be an if stmt: i.e. IF cc goto L5
	// So, the bb should have two exits. 
	//
	if (aPtr->getNextEdge() == NULL)
	{
		// error condition.
		//
		cout << "Panic: Expected a bb with an if stmt (while building a break HTG node). Got something else." << endl;
		assert(0);
	}
	return (aPtr->getNextSucc());
}

BasicBlockNode *buildLoopHTG(BasicBlockNode *aPtr, BaseHTGNodePtr & currPtr)
{
	if (aPtr == NULL) 
	{
		cout << "Warning: Trying to build LoopHTG Node on NULL BBlock" << endl;
		return NULL;
	}

//	cout << " Building LoopHTGNode for BB : " << aPtr << endl;
   
	currPtr = new LoopHTGNode();
   
	BaseHTGNode *tempNodePtr, *nextNodePtr;
   
	Loop *correspLoop;
	BasicBlockNode *nextBB;
   
	if ((correspLoop = trackStack.getLoop(aPtr)) == NULL)
		{
			// error condition.
			cout << "Panic: Could not find corresponding loop structure";
			cout << endl;
			assert(0);
		}

   ((LoopHTGNode *)currPtr)->setPtrToLoopDS(correspLoop);
   
   // Now get the label for the loop.
   //
   BaseInstruction *firstInstr = aPtr->getFirstInstr();
   BaseArgument *labelArg;
   if (firstInstr->hasLabel())
   {
	   labelArg = firstInstr->getPtrToLabelArg();
   }
   else
   {
	   // error condition.
	   cout << "Panic: Found a loop without a label at the first instruction." << endl;
	   assert(0);
   }
   if (correspLoop->hasInnerLoop(aPtr))
      {
	 		nextBB = buildLoopHTG(aPtr, nextNodePtr);
      }
   else if (aPtr->isIfHeadType())
      {
	 		nextBB = buildIfHTG(aPtr, nextNodePtr);
      }
   else 
      {
	 		nextBB = buildSimpleHTG(aPtr, nextNodePtr);
      }
   
   ((LoopHTGNode *)currPtr)->setNewLoopHead(nextNodePtr);
   if (correspLoop->hasTail(aPtr))
      {
	   // Finished building the loop. Pop the loop from the track stack and return the next BB.

	   trackStack.popLoop();
	   
//	   cout << "Finished building loop for bb : " << aPtr << " Next bb : " << nextBB << endl;

	   return nextBB;
      }
    
   tempNodePtr = nextNodePtr;
   while ((nextBB != NULL) && (!correspLoop->hasTail(nextBB)))
      {
		// Check to see if the nextBB is a break stmt
		//
		if (isBreak(nextBB, labelArg))
		{
			// do break stuff.
			nextBB = buildBreakHTG(nextBB, nextNodePtr);
			((LoopHTGNode *)currPtr)->appendBreakNode(nextNodePtr);
		}
		else
			nextBB = buildHTGEx(nextBB, nextNodePtr);

		tempNodePtr->setNext(nextNodePtr);
		((LoopHTGNode *)currPtr)->appendNode(nextNodePtr);
		tempNodePtr = nextNodePtr;
      }
   if (nextBB != NULL)
      {
	 nextBB = buildSimpleHTG(nextBB, nextNodePtr);
	 //
	 // Note - Currently, I am assuming that the tail of the loop is not
	 // of type IFHTG Node nor is it the target of another jump.

	 tempNodePtr->setNext(nextNodePtr);
	 ((LoopHTGNode *)currPtr)->appendNode(nextNodePtr);
      }

   // Finally pop the loop out of the track stack.
   trackStack.popLoop();

//   cout << "Finished building loop for bb : " << aPtr << " Next bb : " << nextBB << endl;
   return nextBB;
}

BasicBlockNode *buildIfHTG(BasicBlockNode *aPtr, BaseHTGNodePtr & currPtr)
{
   if (aPtr == NULL) 
      {
	 cout << "Warning: Trying to build IfHTG Node on NULL BBlock" << endl;
	 return NULL;
      }
   
//   cout << " Building IfHTGNode for BB : " << aPtr << endl;

   BaseHTGNode *tempNodePtr, *nextNodePtr, *ifHeadPtr;
   
   BasicBlockNode *nextBB,*trueBB,*falseBB,*tempBB,*joinBB;

   nextBB = buildSimpleHTG(aPtr, nextNodePtr);
   ifHeadPtr = nextNodePtr;
   
   currPtr = new IfHTGNode();
   
   ((IfHTGNode *)currPtr)->setIfHead(ifHeadPtr);
   
   tempBB = aPtr;
   falseBB = aPtr->getNextSucc();
   trueBB = aPtr->getFirstSucc();

   while (falseBB!=NULL && falseBB->getDFSNum() > tempBB->getDFSNum()){
     tempNodePtr = nextNodePtr;
     tempBB = falseBB;
     falseBB = buildHTGEx(falseBB, nextNodePtr);
     tempNodePtr->setNext(nextNodePtr);
     ((IfHTGNode *)currPtr)->appendFalseNode(nextNodePtr);
   }
   if(falseBB!=NULL && falseBB->getDFSNum() < tempBB->getDFSNum()){
     joinBB=falseBB;
   }
   else
   {
	   // This means that we have a non-structured piece of code in our hands.
	   assert(0);
   }
   nextNodePtr = ifHeadPtr;
   while (trueBB != NULL && trueBB != joinBB)
      {
	 tempNodePtr = nextNodePtr;
	 trueBB = buildHTGEx(trueBB, nextNodePtr);
	 tempNodePtr->setNext(nextNodePtr);
	 ((IfHTGNode *)currPtr)->appendTrueNode(nextNodePtr);
      }
   if (trueBB == joinBB)
      {
	 //joinBB = buildSimpleHTG(joinBB, nextNodePtr);
	 //tempNodePtr->setNext(nextNodePtr);

//NOS	THIS IS A TRAP!! 
//((IfHTGNode *)currPtr)->setIfJoin(new SimpleHTGNode(new BasicBlockNode()));

	 /*joinBB = buildSimpleHTG(joinBB, nextNodePtr);
	 ((IfHTGNode *)currPtr)->setIfJoin(nextNodePtr);*/
      }
   return trueBB;
}

BasicBlockNode *buildHTGEx(BasicBlockNode *aPtr, BaseHTGNodePtr & currPtr)
{
  if(aPtr->isSimpleType()) 
    return buildSimpleHTG(aPtr,currPtr);
  else
    return buildCompoundHTG(aPtr,currPtr);
}

void printIndent(int indent){
  cout << endl;
  for(int i=0;i<indent;i++)
    cout << "   " ;
  cout << flush;
}

void printIndent(FilePointer & fp, int indent)
{
	for (int i = 0; i < indent; i++)
	{
		fprintf(fp, "   ");
	}
	fflush(fp);
}
