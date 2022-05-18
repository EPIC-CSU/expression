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
//===========================================================================
// File:          CopyElim.cc
// Purpose:       copy elimination 
// Author:        Xiaomei Ji
// 
//=========================================================================

//$Id
#ifndef WIN32
#include <stl.h>
#endif
#include <assert.h>
#include "UDProperty.h"
#include "DUProperty.h"
#include "BaseInstruction.h"
#include "MyLinkedList.h"
#include "BasicBlockNode.h"
#include "NormalInstr.h"
#include "BaseArgument.h"
#include "Array.h"
#include "ComputeOp.h"
#include "MemoryOp.h"
#include "Routine.h"
#include "BaseInstruction.h"
#include "BBIterator.h"
#include "RealRegArg.h"
#include "PropertyList.h"
#include "BasicBlockNode.h"
#include "RegisterFileList.h"
#include "BaseHTGNode.h"
#include "LoopHTGNode.h"
#include "IfHTGNode.h"
#include "SimpleHTGNode.h"
#include "CallHTGNode.h"
//#include "ComplexHTGNode.h"
#include "DeleteProperty.h"
#include "OpList.h"

#define UNROLLING_TIMES 3


extern RegisterFileList* globalRegFileList;
extern int 		 registerCounter;

extern int  SameSymbol(BaseArgument*, BaseArgument*);
extern void linkUDDU(BaseArgument*,BaseArgument*,BaseOperation*,BaseOperation*);


//=========================================================================
// isIMov
//=========================================================================

static
int
isIMov(BaseOperation* op) {
    char* name=globalOpCodeTable[((NormalOp*)op)->getOpCodeIndex()].opName();
    if ( !strcmp( name, "IASSIGN" ) )
	return 1;
    return 0;
}



//=========================================================================
// satisfyCond
//=========================================================================

static
int
satisfyCond(BaseArgument* useArg) {
   if ( (((UDChainProperty*)useArg->getPtrToProperty(UDCHAIN)))->getDefList()
							.numItems()==1 ) {
	return _YES_;
   }
   return _NO_;
}


static
void
recordAllUses(ComputeOp* moveOp, OperationListList& oneChain, 
	      OperationList& chain, int& validMoveChain){
      if ( validMoveChain == _NO_ )
	return;
      // assume the moveOp is a = b;
      // then, need to record all the uses of a
      BaseArgument* dest = moveOp->ptrToOperand(_DEST_, _DEST_LIST_);
      DUChainProperty* duChain=(DUChainProperty*)dest->getPtrToProperty(DUCHAIN);
      if ( duChain ) {
	MyLinkedList<BaseOperation*>& useList = duChain->getUseList();
	MyLinkedListIterator<BaseOperation*>* iter = useList.elementsForward();
	while ( iter->hasMoreElements() ) {
	   BaseOperation* useOp = iter->currentElement();
	   if ( isIMov(useOp) ) {
	 	OperationList chain;
	        chain.append(useOp);
		oneChain.append(chain);
		recordAllUses((ComputeOp*)useOp,oneChain, chain,validMoveChain);
		iter->nextElement();
		continue;
	   }
	   BaseArgument* useArg = useOp->ptrToOperand(_SOURCE_1_);
	   if ( SameSymbol(dest, useArg) ) {
	      // check whether this use has unique  definition, or
	      // has one within loop and the other outside loop,
	      // but the one outside loop "a = x+y",
	      // if later, can be changed to " b = x+y", 	
	      // must satisfy the following constraints:
	      // 1. this outside definition is the predecessor of loop header,
	      // 2. no b's def and use from the definition "a=x+y" to a's use
	      // 3. the definition only  has one use
	      if ( satisfyCond(useArg) ) {
		chain.append(useOp);
	      } else  {
		validMoveChain = 0;
		break;
	      }
	   } 
	   if ( useOp->isComputeOp() )
		useArg = useOp->ptrToOperand(_SOURCE_2_);
	   if ( useOp->isMemoryOp() && ((MemoryOp*)useOp)->isStore() )
		useArg = useOp->ptrToOperand(_DEST_, _DEST_LIST_);
	   if ( useArg && SameSymbol(dest, useArg) ) {
	      if ( satisfyCond(useArg) ) {
		chain.append(useOp);
	      } else {
		validMoveChain = 0;
		break;
	      }
	   }

	   iter->nextElement();
	}
	delete iter;
      }
}



static
int
uniqueUD(ComputeOp* moveOp) {
   BaseArgument* useArg = moveOp->ptrToOperand(_SOURCE_1_);
   UDChainProperty* udProp=(UDChainProperty*)useArg->getPtrToProperty(UDCHAIN);
   if ( udProp ) {
	MyLinkedList<BaseOperation*>& defList = udProp->getDefList();
	if ( defList.numItems() == 1 ) {
	   BaseArgument* defArg = defList.getHead()->ptrToOperand(_DEST_, _DEST_LIST_);
	   DUChainProperty* duProp=(DUChainProperty*)defArg->getPtrToProperty(DUCHAIN);
	   if ( duProp->getUseList().numItems() == 1 )
		return _YES_;
	}	
   }
   return _NO_;
}


static
int 
copyIsInChain(ComputeOp* moveOp, MyLinkedList<OperationListList>& copyChains) {
   MyLinkedListIterator<OperationListList>* iter = copyChains.elementsForward();
   while ( iter->hasMoreElements() ) {
      OperationListList& oneChain = iter->currentElement();
      MyLinkedListIterator<OperationList>* listIter=oneChain.elementsForward();
      while (  listIter->hasMoreElements() ) {
	OperationList& oneOpList =  listIter->currentElement();
        if ( moveOp == oneOpList.getHead() ) {
	   delete listIter;
	   delete iter;
	   return _YES_;
	}
	listIter->nextElement();
      }
      delete listIter;
      iter->nextElement();
   }
   delete iter;
   return _NO_;
}



//=========================================================================
// createChainOfMov
//=========================================================================

static
void
createChainOfMov(ComputeOp* moveOp, MyLinkedList<OperationListList>& copyChains) {
   if ( !copyIsInChain(moveOp, copyChains) ) {
      // constrains: this moveOp has one def, and the def has one use -- moveOp
      if ( !uniqueUD(moveOp) ) 
	return;
      int 	validMoveChain = 1;
      OperationListList 	oneChain;
      OperationList chain;
      BaseOperation* tmp = moveOp;
      chain.append(tmp);
      oneChain.append(chain);
      recordAllUses(moveOp, oneChain, chain, validMoveChain);
      if ( validMoveChain )
         copyChains.append(oneChain);
   }
}


//=========================================================================
// createChainInInstr
//=========================================================================

static
void
createChainInInstr(BaseInstruction* instr, MyLinkedList<OperationListList>& copyChains) {
      // data operation
      ArrayIterator<DataOpSlot>*iter=((NormalInstr*)instr)->getDataOpSlotIter();
      while ( iter->hasMoreElements() ) {
        BaseOperation* op = iter->currentElement().getPtrToOperation();
	if ( op == NULL ) {
		iter->nextElement();
		continue;
	}
        if ( isIMov(op) )  {
		createChainOfMov((ComputeOp*)op, copyChains);
	}
        iter->nextElement();
      }
      delete iter;
}

//=========================================================================
// createChainInBB
//=========================================================================

static
void
createChainInBB(BasicBlockNode* bb, MyLinkedList<OperationListList>& copyChains) {
	BBForwIterator bbIter(bb);
	while ( bbIter.hasMoreElements() ) {
	   createChainInInstr(bbIter.currentElement(), copyChains);
	   bbIter.nextElement();
	}
}


//=========================================================================
// createChainInLoop
//=========================================================================
static
void
createChainInLoop(LoopHTGNode* loop, MyLinkedList<OperationListList>& copyChains) {
   MyLinkedListIterator<BaseHTGNode*>* iter = loop->createLoopIterForw();
   while ( iter->hasMoreElements() ) {
	BaseHTGNode* inner = iter->currentElement();
	if (inner->isComplexNode() || inner->isCallNode() || inner->isIfNode()) 
		;
	else if ( inner->isSimpleNode() ) {
	  createChainInBB(((SimpleHTGNode*)inner)->getBasicBlock(), copyChains);
	} else { // LoopNode again
	  createChainInLoop((LoopHTGNode*)inner, copyChains);	
	}
	iter->nextElement();
   }
   delete iter;
}



static void
replaceArg(BaseOperation* defOp, BaseOperation* op, int index,
		   BaseArgument* newReg ) 
{
	BaseArgument* useArg;
	if (index = _DEST_LIST_)
		useArg = op->ptrToOperand(index, _DEST_LIST_);
	else
		useArg = op->ptrToOperand(index, _SRC_LIST_);
	
	if ( useArg == NULL )
		return;
	
	BaseArgument* defArg = defOp->ptrToOperand(_DEST_, _DEST_LIST_);
	if ( SameSymbol(defArg, useArg) ) 
	{
		// replace useArg with new Reg 
		delete useArg;
		if (index == _DEST_)
		{
			op->addOperand(newReg, index, _DEST_LIST_);
			useArg = op->ptrToOperand(index, _DEST_LIST_);
		}
		else
		{
			op->addOperand(newReg, index, _SRC_LIST_);
			useArg = op->ptrToOperand(index, _SRC_LIST_);
		}
		
		// ssaProp and udProp
		useArg->addProperty(*(newReg->getPtrToProperty(SSA)));
		linkUDDU(defArg, useArg, defOp, op);
	}
}


//=========================================================================
// copyInOneChain
//=========================================================================
static
void
copyInOneChain(OperationListList& chain) {
  // copy propagation
  // choose a new virtual register
  BaseArgument* newReg=globalRegFileList->getRealReg(registerCounter++);
  SSAArgProperty ssaProp(1);
  newReg->addProperty(ssaProp);
  MyLinkedListIterator<OperationList>* iter = chain.elementsForward();
  DeleteProperty delProp;
  
  BaseArgument* sourceOfMove = iter->currentElement().getHead()->
						ptrToOperand(_SOURCE_1_);
  UDChainProperty* udProp = (UDChainProperty*)sourceOfMove->
						getPtrToProperty(UDCHAIN);
  BaseArgument* defArg;
  BaseOperation* defOp;
  if ( udProp ) {
   defOp = udProp->getDefList().getHead();
   delete defOp->ptrToOperand(_DEST_, _DEST_LIST_);
   // replace defArg with newReg
   defOp->addOperand(newReg, _DEST_, _DEST_LIST_);
   defArg = defOp->ptrToOperand(_DEST_, _DEST_LIST_);
   // copy ssa property
   defArg->addProperty(*(newReg->getPtrToProperty(SSA)));
  }
  while ( iter->hasMoreElements() ) {
     OperationList&  oneOpList = iter->currentElement();
     MyLinkedListIterator<BaseOperation*>* opIter = oneOpList.elementsForward();
     BaseOperation* moveOp = opIter->currentElement();
     BaseArgument*  destOfMove = moveOp->ptrToOperand(_DEST_, _DEST_LIST_);
     // set as deleted
     moveOp->addProperty(delProp);
     opIter->nextElement();
     while ( opIter->hasMoreElements() ) {
	BaseOperation* op = opIter->currentElement();
	replaceArg(defOp, op, _SOURCE_1_, newReg); 
	if ( op->isComputeOp() )
	   replaceArg(defOp, op, _SOURCE_2_, newReg);	
	if ( op->isMemoryOp() && ((MemoryOp*)op)->isStore() )
	   replaceArg(defOp, op, _DEST_, newReg);
	opIter->nextElement();
     }
     delete opIter;
     iter->nextElement();
  }
  delete iter;
}


static
void
unroll(LoopHTGNode* loop) {
}



static
int 
cycleOfMov(OperationListList& oneChain) {
      MyLinkedListIterator<OperationList>* listIter=oneChain.elementsBackward();
      MyLinkedListIterator<OperationList>* iter = oneChain.elementsBackward();

      int index = 0;
      while (  listIter->hasMoreElements() ) {
	MyLinkedList<BaseOperation*>& oneOpList =  listIter->currentElement();
	// the head is the move op
	BaseOperation* moveOp = oneOpList.getHead();
	BaseArgument*  destArg = moveOp->ptrToOperand(_DEST_, _DEST_LIST_);
	BaseArgument*  sourceArg = moveOp->ptrToOperand(_SOURCE_1_);
	MyLinkedList<BaseArgument*> argList;
 	argList.append(destArg);
	argList.append(sourceArg);
	// find for cycle "a = b", "b = c" ..... " = x" "x = a"
	// find in others for source Arg
	int i = 0;
	while ( iter->hasMoreElements() ) {
	   if ( i != index )
		iter->nextElement();	
	}
	iter->nextElement(); // find it from next element
	while ( iter->hasMoreElements() ) {
	   moveOp = iter->currentElement().getHead();
	   if ( SameSymbol(moveOp->ptrToOperand(_DEST_, _DEST_LIST_), sourceArg) ) {
		sourceArg = moveOp->ptrToOperand(_SOURCE_1_);
		if ( argList.contains(sourceArg) ) { //  cycle
		   delete listIter;
		   delete iter;
		   return _YES_;
		} 
		argList.append(sourceArg);
	   } 
	   iter->nextElement();
	}
	iter->reset();

	listIter->nextElement();
	index ++;
      }
      delete listIter;
      return _NO_;
}


//=========================================================================
// copyElimInLoop
//=========================================================================

static
void
copyElimInLoop(LoopHTGNode* loop) {
	for ( int i=0; i<UNROLLING_TIMES; i++ ) {
		unroll(loop);
	}
	MyLinkedList<OperationListList> 	copyChains;
	createChainInLoop(loop, copyChains);
	// check each chain, do copy propagation
	// notes: 
	// 1. choose a new register
	// 2. do not consider cycle
	MyLinkedListIterator<OperationListList>* iter=copyChains.elementsForward();
	while ( iter->hasMoreElements() ) {
	   OperationListList& oneChain = iter->currentElement();
	   if ( !cycleOfMov(oneChain) ) {
		copyInOneChain(oneChain); 
	   }
	   iter->nextElement();
	}
	delete iter;
}



//=========================================================================
// hasMoveInInstr
//
// check whether an instruction has move op or not
//=========================================================================

static
int
hasMoveInInstr(BaseInstruction* instr) {

      // data operation
      ArrayIterator<DataOpSlot>*iter=((NormalInstr*)instr)->getDataOpSlotIter();
      while ( iter->hasMoreElements() ) {
        BaseOperation* op = iter->currentElement().getPtrToOperation();
	if ( op == NULL ) {
		iter->nextElement();
		continue;
	}
        if ( isIMov(op) )  {
		delete iter;
		return _YES_;
	}
        iter->nextElement();
      }
      delete iter;
      return _NO_;
}


//=========================================================================
// hasMoveInBB
//
// check wheter a basic block has move op or not
//=========================================================================
static
int
hasMoveInBB(BasicBlockNode* bb) {
	BBForwIterator bbIter(bb);
	while ( bbIter.hasMoreElements() ) {
	   if ( hasMoveInInstr(bbIter.currentElement()) ) 
		return _YES_;
	   bbIter.nextElement();
	}
	return _NO_;
}


//=========================================================================
// hasMoveInLoop
//
// check whether a loop has move op or not
//=========================================================================

static
int
hasMoveInLoop(LoopHTGNode* node) {
   MyLinkedListIterator<BaseHTGNode*>* iter = node->createLoopIterForw();
   while ( iter->hasMoreElements() ) {
	BaseHTGNode* inner = iter->currentElement();
	if (inner->isComplexNode() || inner->isCallNode() || inner->isIfNode()) 
		;
	else if ( inner->isSimpleNode() ) {
		if ( hasMoveInBB(((SimpleHTGNode*)inner)->getBasicBlock()) ) {
			delete iter;
			return _YES_;
		}
	} else { // LoopNode again
		if ( hasMoveInLoop((LoopHTGNode*)inner) )
			copyElimInLoop((LoopHTGNode*)inner);	
	}
	iter->nextElement();
   }
   delete iter;
   return _NO_;
}



//=========================================================================
// travBranch
//=========================================================================

static void traverseNode(BaseHTGNode*);

static
void
travBranch(MyLinkedListIterator<BaseHTGNode*>* iter) {
   while ( iter->hasMoreElements() ) {
	traverseNode(iter->currentElement());
	iter->nextElement();
   }
   delete iter;
}


//=========================================================================
// traverseNode
//=========================================================================

static
void
traverseNode(BaseHTGNode* node) {
    if ( node->isSimpleNode() || node->isComplexNode() || node->isCallNode() )
		;
    else if ( node->isLoopNode() ) {
	  if ( hasMoveInLoop((LoopHTGNode*)node) ) {
		copyElimInLoop((LoopHTGNode*)node);
	  }
    } else { // ifNode
	travBranch(((IfHTGNode*)node)->createTrueBranchIterForw());
	travBranch(((IfHTGNode*)node)->createFalseBranchIterForw());
      }
}


//=========================================================================
// copyElim
//=========================================================================

void
Routine::copyElim() {

#ifdef DEBUG
   printf(" begin to copy elimination \n");
#endif

   BaseHTGNode* node = this->getHTG()->getHead();
   do {
      traverseNode(node);
      node = node->getNext();
   } while ( node );

#ifdef DEBUG
   printf(" end to copy elimination \n");
#endif

}
