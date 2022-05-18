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
// File:          CopyProp.cc
// Purpose:       copy propagation 
// Author:        Xiaomei Ji
// 
//=========================================================================

#include "stdafx.h"

#include "RegOrder.h"
#include "BaseArgument.h"
#include "RegisterArg.h"
#include "BaseOperation.h"
#include "ComputeOp.h"
#include "MemoryOp.h"
#include "IfOp.h"
#include "BaseInstruction.h"
#include "NormalInstr.h"
#include "SetList.h"
#include "MyLinkedList.h"
#include "Array.h"
#include "BasicBlockNode.h"
#include "Routine.h"
#include "UDProperty.h"
#include "DUProperty.h"
#include "BBIterator.h"
#include "DataOpSlot.h"
#include "PropertyIncludes.h"
#include "CallOp.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

#define COPYPROP 1

extern void linkUDDU(BaseArgument*,BaseArgument*,BaseOperation*,BaseOperation*);
extern int  SameSymbol(BaseArgument*, BaseArgument*);
extern void MinusKill(SetList<ComputeOp*>&, CompDefBBProperty*, int);
extern void delUseFromDef(MyLinkedList<BaseOperation*>&, BaseOperation*);
extern void setOpAsDel(BaseOperation*);
extern void removeFromGen(SetList<ComputeOp*>&, BaseArgument*, int);
extern BasicBlockNode*	findBBOfInstr(BaseInstruction*, ControlFlowGraph&);

extern Routine*    currentRoutine;
extern OpCodeTable globalOpCodeTable;



//==========================================================================
// OpInInstr
//==========================================================================

static
int
OpInInstr(BaseOperation* op, BaseInstruction* instr) {
#ifdef DEBUG
  assert ( instr->isNormalInstr() ); 
#endif
     if ( op->isDataOp() ) {
         ArrayIterator<DataOpSlot>* opIter = ((NormalInstr*)instr)->getDataOpSlotIter();
         while ( opIter->hasMoreElements() ) {
   	    if ( op==opIter->currentElement().getPtrToOperation()) {
   		   delete opIter;
   		   return _YES_;
   	    }   
   	    opIter->nextElement();
   	 }
              delete opIter;
     } else if ( op->isControlOp() ) {
         ArrayIterator<ControlOpSlot>* opIter = ((NormalInstr*)instr)->getControlOpSlotIter();
         while ( opIter->hasMoreElements() ) {
   		if ( op==opIter->currentElement().getPtrToOperation()) {
   		   delete opIter;
   		   return _YES_;
   		}   
   		opIter->nextElement();
         }
         delete opIter;
     }
  return _NO_;
}


//=========================================================================
// linkAllDefAndUse
//=========================================================================



void print172(void){

}



void
printDefList(MyLinkedList<BaseOperation*>& defList){
	MyLinkedListIterator<BaseOperation*>* iter = defList.elementsForward();
	while ( iter->hasMoreElements() ) {
		BaseOperation* defOp = iter->currentElement();
		defOp->print(0);
		iter->nextElement();
	}
	delete iter;
}

//========================================================================
// linkAllDefAndUse
//
// search for definitions of the variable used in useOp (position) and
// link UDs and DUs accordingly
//========================================================================


void
linkAllDefAndUse(MyLinkedList<BaseOperation*>& defList,
		 BaseOperation* useOp,
		 int 		position )  {

   MyLinkedListIterator<BaseOperation*>* iter = defList.elementsForward();
   while ( iter->hasMoreElements() ) {
	BaseOperation* defOp = iter->currentElement();
	if(defOp->hasOperand(_DEST_, _DEST_LIST_))
   		linkUDDU(defOp->ptrToOperand(_DEST_, _DEST_LIST_), useOp->ptrToOperand(position), 
			defOp, useOp);
	else{

		// debug code inserted by SAH on 01/17/2001
		//
		defOp->hasOperand(_DEST_, _DEST_LIST_);
		defOp->print(0);
		useOp->print(0);
		cout << "Def List:" << endl;
		printDefList(defList);
		abort();
	}
	iter->nextElement();
   }
   delete iter;
}

void 
linkAllDefAndUse(MyLinkedList<BaseOperation*>& defList, BaseOperation *useOp, BaseArgument *useArg)  {

   MyLinkedListIterator<BaseOperation*>* iter = defList.elementsForward();
   while ( iter->hasMoreElements() ) {
	BaseOperation* defOp = iter->currentElement();
	if(defOp->hasOperand(_DEST_, _DEST_LIST_))
   		linkUDDU(defOp->ptrToOperand(_DEST_, _DEST_LIST_), useArg, 
			defOp, useOp);
	else{
		defOp->print(0);
		useOp->print(0);
		cout << "Def List:" << endl;
		printDefList(defList);
		abort();
	}
	iter->nextElement();
   }
   delete iter;
}

//=========================================================================
// copyInArg
// 
// copy progation in argument
//=========================================================================
static void
copyInArg(BaseOperation* copyOp, BaseOperation* useOp, int position) {

	if (position == _DEST_)
	{
		BaseArgument*   copyArg = copyOp->ptrToOperand(_DEST_, _DEST_LIST_);
		BaseArgument* 	arg = useOp->ptrToOperand(position, _DEST_LIST_);
		
		// if this register has been const propagated, need not cp it
		if (    arg==NULL 
			|| ! arg->isRegisterArg() 
			|| arg->getPtrToProperty(_CNSTPROP_) ) 
			return;
		if ( SameSymbol(copyArg, arg) )  
		{ // replace
		  /* do not do this, because the duchain is used when call
		  * copyInOp() in copyPropagation();
		  *
		  // first remove this use arg from its def's duchain
		  ((DUChainProperty*)copyArg->getPtrToProperty(DUCHAIN))->remove(useOp);
			*/
			// then, replace 
			BaseArgument* source = copyOp->ptrToOperand(_SOURCE_1_);
			useOp->addOperand(source, position, _DEST_LIST_);
			delete arg;
			// maintain the argument's property
			// include the du/ud chain and SSA/RegNum property 
			
			
			UDChainProperty* prop=(UDChainProperty*)source->getPtrToProperty(_UDCHAIN_);
			if ( prop )
				linkAllDefAndUse(prop->getDefList(), useOp, position);
			
			
			RegOrderArgProperty* regP = (RegOrderArgProperty*)source->getPtrToProperty(_REGORDER_);
			if ( regP )
				useOp->ptrToOperand(position, _DEST_LIST_)->addProperty(*regP);
			
			
			SSAArgProperty* ssaP = (SSAArgProperty*)source->getPtrToProperty(_SSA_);
			if ( ssaP ) 
				useOp->ptrToOperand(position, _DEST_LIST_)->addProperty(*ssaP);
		}
	}
	else
	{
		BaseArgument*   copyArg = copyOp->ptrToOperand(_DEST_, _DEST_LIST_);
		BaseArgument* 	arg = useOp->ptrToOperand(position, _SRC_LIST_);
		
		// if this register has been const propagated, need not cp it
		if (    arg==NULL 
			|| ! arg->isRegisterArg() 
			|| arg->getPtrToProperty(_CNSTPROP_) ) 
			return;
		if ( SameSymbol(copyArg, arg) )  
		{ // replace
		  /* do not do this, because the duchain is used when call
		  * copyInOp() in copyPropagation();
		  *
		  // first remove this use arg from its def's duchain
		  ((DUChainProperty*)copyArg->getPtrToProperty(DUCHAIN))->remove(useOp);
			*/
			// then, replace 
			BaseArgument* source = copyOp->ptrToOperand(_SOURCE_1_);
			useOp->addOperand(source, position, _SRC_LIST_);
			// maintain the argument's property
			// include the du/ud chain and SSA/RegNum property 
			
			
			UDChainProperty* prop=(UDChainProperty*)source->getPtrToProperty(_UDCHAIN_);
			if ( prop )
				linkAllDefAndUse(prop->getDefList(), useOp, position);
			
			
			RegOrderArgProperty* regP = (RegOrderArgProperty*)source->getPtrToProperty(_REGORDER_);
			if ( regP )
				useOp->ptrToOperand(position, _SRC_LIST_)->addProperty(*regP);
			
			
			SSAArgProperty* ssaP = (SSAArgProperty*)source->getPtrToProperty(_SSA_);
			if ( ssaP ) 
				useOp->ptrToOperand(position, _SRC_LIST_)->addProperty(*ssaP);
		}
	}
}


//=========================================================================
// copyInOp
// 
// copy propagation in operation
//=========================================================================

static
void
copyInOp(BaseOperation* copyOp, BaseOperation* op) {
	copyInArg(copyOp, op, _SOURCE_1_);
	copyInArg(copyOp, op, _SOURCE_2_);
	if ( op->isMemoryOp() && ((MemoryOp*)op)->isStore() )
		copyInArg(copyOp, op, _DEST_);
}



//=========================================================================
// BBOfOp
// return the basic block of an operation
//=========================================================================

static
BasicBlockNode*
BBOfOp(BaseOperation* op, ArrayIterator<BasicBlockNode*>* bbIter) {
   bbIter->reset();
   while ( bbIter->hasMoreElements() ) {
      BasicBlockNode* bb = bbIter->currentElement();
      BBForwIterator  iter(bb);
      while ( iter.hasMoreElements() ) {
         if ( OpInInstr(op, iter.currentElement()) )
		return bb;
	 iter.nextElement();
      }
      bbIter->nextElement();
   }
   return 0;
}


//==========================================================================
// instrOfOp
//
// return the instruction that an operation belongs to
//=========================================================================

static
BaseInstruction*
instrOfOp(BaseOperation* op) {
   MyLinkedListIterator<BaseInstruction*>* iter = currentRoutine->instrListIteratorForw();
   while ( iter->hasMoreElements() ) {
         BaseInstruction* instr = iter->currentElement();
         if ( instr->isNormalInstr() ) {
           if ( op->isDataOp() ) {
   	      ArrayIterator<DataOpSlot>* opIter = ((NormalInstr*)instr)->getDataOpSlotIter();
              while ( opIter->hasMoreElements() ) {
   		if ( op==opIter->currentElement().getPtrToOperation()) {
   		   delete opIter;
   		   return instr;
   		}   
   		opIter->nextElement();
   	      }
              delete opIter;
	   } else if ( op->isControlOp() ) {
   	      ArrayIterator<ControlOpSlot>* opIter = ((NormalInstr*)instr)->getControlOpSlotIter();
              while ( opIter->hasMoreElements() ) {
   		if ( op==opIter->currentElement().getPtrToOperation()) {
   		   delete opIter;
   		   return instr;
   		}   
   		opIter->nextElement();
   	      }
              delete opIter;
           }
         } 
         iter->nextElement();
   }
#ifdef DEBUG
   assert(0>1);
#endif
   delete iter;
   return 0;
}


//=========================================================================
// defBefore
//
// check if the target and source arguments of @copyOp have been
// defined in the @bb before @instr
//=========================================================================

static
int
defBefore(BasicBlockNode*bb, BaseInstruction* instr,ComputeOp* copyOp) {
	 BBForwIterator iter(bb);
	 while ( iter.hasMoreElements() ) {
	   BaseInstruction* prevInstr = iter.currentElement();
	   if ( prevInstr == instr ) {
		return _NO_;
	   }
	      
	   ArrayIterator<DataOpSlot>* dataIter = ((NormalInstr*)prevInstr)->getDataOpSlotIter();
	   while ( dataIter->hasMoreElements() ) {
		BaseOperation* op = dataIter->currentElement().getPtrToOperation();
		if ( op == NULL ) {
			dataIter->nextElement();
			continue;
		}
		// compute operation
		if ( op->isComputeOp() 
		  || op->isMemoryOp() && ((MemoryOp*)op)->isLoad() 
		  || op->isCallOp() && ((CallRetOp*)op)->hasOperand(_DEST_, _DEST_LIST_) ) {
		   BaseArgument* defArg = op->ptrToOperand(_DEST_, _DEST_LIST_);
		   if ( SameSymbol(defArg, copyOp->ptrToOperand(_DEST_, _DEST_LIST_)) 
		     || SameSymbol(defArg, copyOp->ptrToOperand(_SOURCE_1_)) ) {
			delete dataIter;
			return _YES_;
		   }
		}
		dataIter->nextElement();
	   }	
	
	   delete dataIter;
	   iter.nextElement();
	 }
#ifdef _DEBUG
	 assert(0>1);
#endif
	 return _NO_;
}


//=========================================================================
// SatisfyCondOfCP
//
// check if a copy operation satisfy the condition of  copy propagatin
// the condition is:
// for all the use reference use_ref 
// copy operation is in the C_IN[BB], BB is use_ref's basic block
//
// moreover, no definition of lvalue and rvalue of this copy operation
// occur prior to the use_ref within the same BB as the use_ref
//
//=========================================================================
static
int
SatisfyCondOfCP(BasicBlockNode* bbOfCopyOp,
		ComputeOp* copyOp, 
		MyLinkedListIterator<BaseOperation*>* useIter,
		SetList<ComputeOp*>* in) {
   ArrayIterator<BasicBlockNode*>* bbIter = currentRoutine->getCFG().createDFSBBIter();
   while ( useIter->hasMoreElements() ) {
      BaseOperation* useOp = useIter->currentElement();
      BaseInstruction* instr =  instrOfOp(useOp);
      BasicBlockNode* bb = findBBOfInstr(instr, currentRoutine->getCFG());
      // first condition
      if ( bbOfCopyOp != bb && !in[bb->getDFSNum()].contains(copyOp) ) {
	// bbOfCopyOp==bb means source and target of copy operation in same bb
	// means do copy propagation within basic block
	delete bbIter;
	return _NO_;
      }
      // second condition
	  /*
      if ( defBefore(bb, instr, copyOp) ) {
	delete bbIter;
	return _NO_;
      }
	  */
      useIter->nextElement();
   }
   delete bbIter;
   return _YES_;
}


//=========================================================================
// copyPropagation
//=========================================================================

static
void
copyPropagation(BasicBlockNode* bb, ComputeOp* copyOp, SetList<ComputeOp*>* in){
   BaseArgument* dest = copyOp->ptrToOperand(_DEST_, _DEST_LIST_);
   DUChainProperty* prop = (DUChainProperty*)dest->getPtrToProperty(_DUCHAIN_);
   if ( prop ) {
      MyLinkedList<BaseOperation*>& useList = prop->getUseList();
      // check if all of them satisfy the condition of being copy prop
      MyLinkedListIterator<BaseOperation*>* iter = useList.elementsForward();
      if ( SatisfyCondOfCP( bb, copyOp, iter, in ) ) {
	// copy propagation in all used operations
	iter->reset();
	while ( iter->hasMoreElements() ) {
	   copyInOp(copyOp, iter->currentElement());
	   iter->nextElement();
	}
	setOpAsDel(copyOp);
	delete iter;
      }
   } 
}



//=========================================================================
// compGenOfInstr
//
//=========================================================================

static
void
compGenOfInstr(BaseInstruction* instr, SetList<ComputeOp*>& one ) {
      
#ifdef DEBUG
     assert ( instr->isNormalInstr() ); 
#endif

      // data operation
      ArrayIterator<DataOpSlot>* iter = ((NormalInstr*)instr)->getDataOpSlotIter();
      while ( iter->hasMoreElements() ) {
            BaseOperation* op = iter->currentElement().getPtrToOperation();
	    if ( op == NULL ) {
		iter->nextElement();
		continue;
	    }

	    /***  remove, because of the following  case 
	     ***  (1) x = y; .....; y = ....;
	     ***  (2) x = y; .....; x = ....; (can not happen for RSSA)
	     ***/

	    removeFromGen(one, op->ptrToOperand(_DEST_, _DEST_LIST_), COPYPROP);

	    //  add the new copy operation
	    char *name=globalOpCodeTable[((NormalOp*)op)->getOpCodeIndex()].opName();
	    if ((strcmp(name, "IASSIGN")==0) || (strcmp(name, "ASSIGN")==0) || (strcmp(name, "DASSIGN")==0) || (strcmp(name, "FASSIGN")==0)){
                ComputeOp* tmp = (ComputeOp*)op;
		one.append(tmp);
	    }

            iter->nextElement();
      }
      delete iter;
}



//=========================================================================
// compGenOfBB
//
//=========================================================================

static
void
compGenOfBB(BasicBlockNode* bb, SetList<ComputeOp*>& one) {
   BBForwIterator iter(bb);
   while ( iter.hasMoreElements() ) {
      compGenOfInstr(iter.currentElement(), one );
      iter.nextElement();
   }
}


//=========================================================================
// compInOfBB
//=========================================================================

static
void
compInOfBB(ArrayIterator<BasicBlockNode*>*bbIter, SetList<ComputeOp*>*gen, 
	   SetList<ComputeOp*>* in, SetList<ComputeOp*>* out) {
   SetList<ComputeOp*> tmp;
   // initialization
   // the in of first bb is NULL, so across first bb;
   out[1] = gen[1];
   bbIter->nextElement();
   // from 2th bb to last
   while ( bbIter->hasMoreElements() ) {
      BasicBlockNode* bb = bbIter->currentElement();
      int currIndex = bb->getDFSNum();
      SetList<ComputeOp*>& oneIn = in[currIndex];
      BasicBlockPredecessors& preds = bb->getCFPred();
      MyLinkedListIterator<BasicBlockNode*>* predIter = preds.createPredListIterForw();
      int first = _YES_;
      while ( predIter->hasMoreElements() ) {
         BasicBlockNode* pred = predIter->currentElement();
         int index = pred->getDFSNum();
         if ( out[index].isEmpty() == _NO_ ) {
            if ( first )  {
               oneIn = out[index];
               first = _NO_;
 	    }
            else
               oneIn.intersection(out[index]);
         }
         predIter->nextElement();     
      }
      delete predIter;
      // compute out
      out[currIndex] = oneIn;
      MinusKill(out[currIndex], (CompDefBBProperty*)bb->getPtrToProperty(_COMPDEF_) , COPYPROP);
      out[currIndex].set_union(gen[currIndex]);
      // progress
      bbIter->nextElement();
   }
   // reset
   bbIter->reset();
   // iterative 
   int change = 1;
   while ( change ) {
      change = 0;
      bbIter->nextElement(); // across first bb
      while ( bbIter->hasMoreElements() ) {
	 BasicBlockNode* bb = bbIter->currentElement();
	 int index = bb->getDFSNum();
         BasicBlockPredecessors& preds = bb->getCFPred();
         MyLinkedListIterator<BasicBlockNode*>* predIter = preds.createPredListIterForw();
         // here, all have at least one pred
         tmp = out[predIter->currentElement()->getDFSNum()]; 
         predIter->nextElement();
         while ( predIter->hasMoreElements() ) {
            tmp.intersection(out[predIter->currentElement()->getDFSNum()]);
            predIter->nextElement();     
         }
         delete predIter;

         if ( tmp.operator!= (in[index]) ) {
           in[index] = tmp;
           out[index] = tmp;
           MinusKill(out[index], (CompDefBBProperty*)bb->getPtrToProperty(_COMPDEF_), COPYPROP );
           out[index].set_union(gen[index]);
           change = 1; 
	 }  
         bbIter->nextElement();
         tmp.removeAll();
      }
      bbIter->reset();
   }
}



//=========================================================================
// copyProp
//=========================================================================

void
Routine::copyProp() {


	cout << " Begin to compute the copyProp" << endl;


   int numBBs = _cfg.numBBs();
   SetList<ComputeOp*>* gen = new SetList<ComputeOp*> [numBBs+1];
   ArrayIterator<BasicBlockNode*>* bbIter = _cfg.createDFSBBIter();
   while ( bbIter->hasMoreElements() ) {
      BasicBlockNode* bb = bbIter->currentElement();
      compGenOfBB(bb, gen[bb->getDFSNum()] );
      bbIter->nextElement();
   }
   bbIter->reset();

   SetList<ComputeOp*>* in = new SetList<ComputeOp*> [numBBs+1];
   SetList<ComputeOp*>* out = new SetList<ComputeOp*> [numBBs+1];
   compInOfBB(bbIter, gen, in, out);

   delete [] out;

   bbIter->reset();

   // copy propagate
   while ( bbIter->hasMoreElements() ) {
	BasicBlockNode* bb = bbIter->currentElement();
	int  index = bb->getDFSNum();
	MyLinkedListIterator<ComputeOp*>* iter = gen[index].elementsForward();
	while ( iter->hasMoreElements() ) {
	   copyPropagation(bb, iter->currentElement(), in);
	   iter->nextElement();
	}
	delete iter;
	bbIter->nextElement();
   }  

   delete bbIter;
   delete [] in;
   delete [] gen;

	cout << " end to compute the copyProp" << endl;
}
