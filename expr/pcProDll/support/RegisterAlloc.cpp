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
// File:	RegisterAlloc.cc
// Purpose:     register Allocation
// 		assume that all are general purpose registers
// Creat: 	Jan. 27, 98
// Authur:	Xiaomei Ji
//==========================================================================


#include "BaseInstruction.h"
#include "BaseOperation.h"
#include "ComputeOp.h"
#include "MemoryOp.h"
#include "JumpOp.h"
#include "BaseArgument.h"
#include "RealRegArg.h"
#include "RegPressure.h"
#include "RegOrder.h"
#include "MyLinkedList.h"
#include "Routine.h"
#include "BasicBlockNode.h"
#include "SetList.h"
#include "DataOpSlot.h"
#include "Array.h"
#include "DUProperty.h"
#include "UDProperty.h"
#include "ClusterProperty.h"
#include "BBIterator.h"


#define MAX_NUM_OF_REG 32
#define UNALLOCATED -1
#define HASALLOCATED -1

// RegAllocStatus is used to present:
// which argument resides on which register 
// RegAllocStatus[i] stores the argument index that was allocated into Ri
static int RegAllocStatus[MAX_NUM_OF_REG];

extern Routine* 	currentRoutine;
extern SymbolTable* 	globalSymbolTable;


extern BasicBlockNode* findBBOfInstr(BaseInstruction*,ControlFlowGraph&);
extern int		SameSymbol(BaseArgument*, BaseArgument*);



//=======================================================================
// removeDef
//
// remove one definition from the live variable set
//========================================================================

static
void
removeDef(SetList<int>& liveSet, NormalInstr* instr) {
   ArrayIterator<DataOpSlot>* iter = instr->getDataOpSlotIter();
   while ( iter->hasMoreElements() ) {
 	BaseOperation* op = iter->currentElement().getPtrToOperation();
	// compute operation
	if ( op == NULL ) {
		iter->nextElement();
		continue;
	}
	if ( op->isComputeOp() ||
	     op->isMemoryOp() && ((MemoryOp*)op)->isLoad() ) {
	     BaseArgument* arg =op->ptrToOperand(_DEST_, _DEST_LIST_); 
	     liveSet.remove(globalSymbolTable->getID(arg, arg->getPtrToProperty(_SSA_)));
	}
 	iter->nextElement();
   }	
   delete iter;
}


//=======================================================================
// addUse
//=======================================================================
static
void
addUse(SetList<int>& liveSet, NormalInstr* instr) {
      ArrayIterator<DataOpSlot>* iter = instr->getDataOpSlotIter();
      while ( iter->hasMoreElements() ) {
         BaseOperation* op = iter->currentElement().getPtrToOperation();
	 if ( op == NULL ) {
		iter->nextElement();
		continue;
	 }
         if ( op->isComputeOp() ) {
	    BaseArgument* arg = op->ptrToOperand(_SOURCE_1_);
	    int index=globalSymbolTable->getID(arg, arg->getPtrToProperty(_SSA_));
	    liveSet.add(index);
	    arg = op->ptrToOperand(_SOURCE_2_);
	    if ( arg && arg->isRegisterArg() ) {
	       index = globalSymbolTable->getID(arg, arg->getPtrToProperty(_SSA_));
	       liveSet.add(index);
	    }
         } else if ( op->isMemoryOp() ) {
	    BaseArgument* arg = op->ptrToOperand(_SOURCE_1_);
	    int index=globalSymbolTable->getID(arg, arg->getPtrToProperty(_SSA_));
	    liveSet.add(index);
            if ( ((MemoryOp*)op)->isStore() ) {
	       arg = op->ptrToOperand(_DEST_, _DEST_LIST_);
	       index=globalSymbolTable->getID(arg, arg->getPtrToProperty(_SSA_));
	       liveSet.add(index);
	    }
         }
         iter->nextElement();
      }
      delete iter;

      // control operation
      ArrayIterator<ControlOpSlot>* cntlIter = ((NormalInstr*)instr)->getControlOpSlotIter();
      while ( cntlIter->hasMoreElements() ) {
         BaseOperation* op = cntlIter->currentElement().getPtrToOperation();
         if ( op && op->isIfOp() ) {
	    BaseArgument* arg = op->ptrToOperand(_SOURCE_1_);
	    int index=globalSymbolTable->getID(arg, arg->getPtrToProperty(_SSA_));
	    liveSet.add(index);
         }
         cntlIter->nextElement();
      }
      delete cntlIter;
}


//=======================================================================
// compLiveVarOfInstr
//
// compute the live variables after an instruction
// the live variables of the exit of one instruction  can be caculated
// by the current basic block's out live variables
//=============================================================================

void
compLiveVarOfInstr(SetList<int>& liveSet, BaseInstruction* currInstr, BasicBlockNode* bb = 0) {
	// After scheduling, cfg is still valid
       if ( bb == 0 ) // find the bb that this instruction is in
		bb = findBBOfInstr(currInstr, currentRoutine->getCFG());
	// get out live vars of this bb
	LiveDeadBBProperty* prop = (LiveDeadBBProperty*)bb->getPtrToProperty(_LIVEDEAD_);
	if ( prop )  {
		liveSet = prop->getLiveList();
	}
	BBBackIterator iter(bb);
	while ( iter.hasMoreElements() ) {
	   BaseInstruction* nextInstr = iter.currentElement();
	   if ( nextInstr == currInstr )
		break;
	   // liveSet.minus(dest).add(source1 and source2)
	   removeDef(liveSet, (NormalInstr*)nextInstr);
	   addUse(liveSet, (NormalInstr*)nextInstr);
	   iter.prevElement();
	}

}


//==========================================================================
// compUnAllocVar
//
// compute unallocated live variable numbers of an instruction
//==========================================================================

static
int
compUnAllocVar(SetList<int>& liveSet){
    int unAllocNum = 0;
    MyLinkedListIterator<int>* iter = liveSet.elementsForward();
    while ( iter->hasMoreElements() ) {
	int var = iter->currentElement();
        int i = 0;
	for ( ; i<MAX_NUM_OF_REG; i++ )  {
		if ( var == RegAllocStatus[i] )
			break;
	}
        if ( i>=MAX_NUM_OF_REG) 
		unAllocNum++;
	iter->nextElement();
    }  
    delete iter;
    return unAllocNum;
}

//========================================================================
// compRegPressure
//
// compute register pressure as the property of instruction
// formular: regPressure = unallocated live var num (over) avail reg num
//========================================================================

void
compRegPressure(BaseInstruction* instr, BasicBlockNode* bb=0) {
	// compute the un - allocated - variables
	// first, the live variables of this instr
	SetList<int> liveVar;
	if ( bb == 0 ) // find the bb that this instruction is in
		bb = findBBOfInstr(instr, currentRoutine->getCFG());
	compLiveVarOfInstr(liveVar, instr, bb);
	// then, check in web list, calculate unallocated live vars num
	int unAllocVars = compUnAllocVar(liveVar);

	int availRegs = MAX_NUM_OF_REG - ( liveVar.numItems() - unAllocVars );
        RegPressProperty prop(unAllocVars, availRegs);
	instr->addProperty(prop);
}


static void propDef(BaseArgument*, int, MyLinkedList<BaseArgument*>&);

//==================================================================
// propUse
//
//=================================================================

static
void
propUse(BaseArgument* arg, int order, MyLinkedList<BaseArgument*>& visit) {
   if ( visit.contains(arg) ) 
	return;
   visit.append(arg);
   // propagate to its def
   UDChainProperty* prop=(UDChainProperty*)arg->getPtrToProperty(_UDCHAIN_);
   if ( prop ) {
	RegOrderArgProperty tmp(order);
	MyLinkedListIterator<BaseOperation*>* defIter = prop->defListIteratorForw();
	while ( defIter->hasMoreElements() ) {
	   BaseOperation* defOp = defIter->currentElement();
   	   BaseArgument* def = defOp->ptrToOperand(_DEST_, _DEST_LIST_);
	   if ( visit.contains(def) )  {
#ifdef DEBUG
	      RegOrderArgProperty* regProp=(RegOrderArgProperty*)def->getPtrToProperty(REGORDER);
	      assert(regProp->getOrder() == order);
#endif
	   } else {
       	      def->addProperty(tmp);
	      propDef(def, order, visit);
	   }
	   defIter->nextElement();
	   
	}
   }
}




//==================================================================
// propToUseArg
//==================================================================

static
void
propToUseArg(BaseArgument* useArg, int order, 
	     MyLinkedList<BaseArgument*>& visit) {
   if ( visit.contains(useArg) ) {
#ifdef DEBUG
	RegOrderArgProperty* prop=(RegOrderArgProperty*)useArg->getPtrToProperty(REGORDER);
	assert(prop->getOrder() == order);
#endif
   } else {
	RegOrderArgProperty tmp(order);
        useArg->addProperty(tmp);
	propUse(useArg, order, visit);
   }
}


//==================================================================
// propDef
//
//==================================================================

static
void
propDef(BaseArgument* arg,int order,MyLinkedList<BaseArgument*>& visit){
   if ( visit.contains(arg) )
	return;
   visit.append(arg);
   // prop to its all uses
   DUChainProperty* prop=(DUChainProperty*)arg->getPtrToProperty(_DUCHAIN_);
   if ( prop ) {
      RegOrderArgProperty tmp(order);
      MyLinkedListIterator<BaseOperation*>* iter=prop->useListIteratorForw();
      while ( iter->hasMoreElements() ) {
	BaseOperation* op = iter->currentElement();
#ifdef DEBUG
	assert ( op->isNormalOp() ); 
#endif
	BaseArgument* source = op->ptrToOperand(_SOURCE_1_);
	if ( SameSymbol(arg, source) )  {
	   propToUseArg(source, order, visit);
	}
	source = op->ptrToOperand(_SOURCE_2_);
	if ( source && SameSymbol(arg, source) ) {
	   propToUseArg(source, order, visit);
	}
	iter->nextElement();
      }
      delete iter;
   }
}


//========================================================================
// regAllocForArg
//
//========================================================================

static
int
regAllocForArg(BaseArgument* arg,int* status,BaseInstruction*instr){
   // has allocated
   if ( arg==NULL || !arg->isRegisterArg() ||  arg->getPtrToProperty(_REGORDER_) )
	return HASALLOCATED;
   // allocate one
   // if have available register
   int i = 0;
   for ( ; i<MAX_NUM_OF_REG; i++ ) {
	if ( RegAllocStatus[i]==UNALLOCATED && status[i]==UNALLOCATED )
		break;
   }
   if ( i<MAX_NUM_OF_REG )  { // find available
	RegOrderArgProperty tmp(i);
        arg->addProperty(tmp);
	if ( status ) 
		status[i] = globalSymbolTable->getID(arg, arg->getPtrToProperty(_SSA_));
	else
		RegAllocStatus[i] = globalSymbolTable->getID(arg, arg->getPtrToProperty(_SSA_));
	return i;
   } else {
	// replace one in RegAllocStatus
	SetList<int> liveVar;
	compLiveVarOfInstr(liveVar, instr);
        int i = 0;
	for ( ; i<MAX_NUM_OF_REG; i++ ) {
	   int var =  RegAllocStatus[i];
           if ( var == UNALLOCATED ) 
		continue;
	   if ( ! liveVar.contains(var) ) {
		if ( status && status[i] == UNALLOCATED ) {
		   status[i] = globalSymbolTable->getID(arg, arg->getPtrToProperty(_SSA_));
		   RegOrderArgProperty tmp(i);
		   arg->addProperty(tmp);
		   return i;
		}
		if ( ! status ) {
		   RegAllocStatus[i] = globalSymbolTable->getID(arg, arg->getPtrToProperty(_SSA_));
		   RegOrderArgProperty tmp(i);
		   arg->addProperty(tmp);
		   return i;
		}
	   }
	}
#ifdef DEBUG
	assert( 0>1 );
#endif
	return -1;
   }
}


//=========================================================================
// regAllocForInstr
//
//========================================================================

static
void
regAllocForInstr(BaseInstruction* instr) {
   int sourceStatus[MAX_NUM_OF_REG];
   for ( int i=0; i<MAX_NUM_OF_REG; i++ ) {
	sourceStatus[i] = UNALLOCATED;
   }
#ifdef DEBUG
   assert ( instr->isNormalInstr() ); 
#endif
   // first, alloc all source operands in all operation
   int targStatus[MAX_NUM_OF_REG];
   for (int i=0; i<MAX_NUM_OF_REG; i++ ) {
	targStatus[i] = UNALLOCATED;
   }
   ArrayIterator<DataOpSlot>*iter=((NormalInstr*)instr)->getDataOpSlotIter();
   while ( iter->hasMoreElements() ) {
	 BaseOperation* op = iter->currentElement().getPtrToOperation();
	 if ( op != NULL && op->getPtrToProperty(_DELETE_) == NULL ) {
	    BaseArgument* arg = op->ptrToOperand(_SOURCE_1_);
	    int order = regAllocForArg(arg,sourceStatus,instr); 
	    if ( order != HASALLOCATED ) {
		MyLinkedList<BaseArgument*> visit;
		propUse(arg, order, visit);
	    }
	    arg = op->ptrToOperand(_SOURCE_2_);
	    order = regAllocForArg(arg,sourceStatus,instr); 
	    if ( order != HASALLOCATED ) {
		MyLinkedList<BaseArgument*> visit;
		propUse(arg, order, visit);
	    }
	 }
	 iter->nextElement();
   }
   // union sourceStatus with RegAllocStatus
   for (int i=0; i<MAX_NUM_OF_REG; i++ ) {
	if ( sourceStatus[i] != UNALLOCATED )
		RegAllocStatus[i] = sourceStatus[i];	
   }
   iter->reset();
   while ( iter->hasMoreElements() ) {
	BaseOperation* op = iter->currentElement().getPtrToOperation();
	if ( op != NULL && op->getPtrToProperty(_DELETE_) == NULL ) {
	    BaseArgument* arg = op->ptrToOperand(_DEST_, _DEST_LIST_);
	    int order = regAllocForArg(arg, targStatus, instr); 
	    if ( order != HASALLOCATED ) {
		MyLinkedList<BaseArgument*> visit;
		propDef(arg, order, visit );
	    }
	 }
	 iter->nextElement();
   }
   // union targStatus with RegAllocStatus
   for (int i=0; i<MAX_NUM_OF_REG; i++ ) {
	if ( targStatus[i] != UNALLOCATED )
		RegAllocStatus[i] = targStatus[i];	
   }
   delete iter;
}


//===========================================================================
// registerAlloc
//
//========================================================================== 

void
Routine::RegAlloc() {
#ifdef DEBUG
   printf( " now, into register allocation \n");
#endif
   for ( int i=0; i<MAX_NUM_OF_REG; i++ ) 
	RegAllocStatus[i] = UNALLOCATED;
   MyLinkedListIterator<BaseInstruction*>* iter = instrListIteratorForw();
   while ( iter->hasMoreElements() ) {
	compRegPressure(iter->currentElement());
#ifdef DEBUG
	   printf(" RA  for instr = 0x%x\n", iter->currentElement());
#endif 
	regAllocForInstr(iter->currentElement());
	iter->nextElement();
   }
   delete iter;

#ifdef  DEBUG
   printf( " done RA \n");
#endif
}
