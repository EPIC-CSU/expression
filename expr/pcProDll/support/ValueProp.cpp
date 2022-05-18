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
// File:          ValueProp.cc
// Purpose:       simple cse 
// Author:        Xiaomei Ji
// 
// here, the cse only refers to the simple tripe, no associativity and
// distributivity is considered
//
// questions: here, one more temp variable is introduced because of cse,
// 	      to this variable,  its _SSA_ property is 1,
//	      but how to add it into symbol table,
//	      so, the behind phases can take this arg into consideration 
//=========================================================================

#include "stdafx.h"

#include "Routine.h"
#include "BaseArgument.h"
#include "BaseInstruction.h"
#include "BaseOperation.h"
#include "ComputeOp.h"
#include "MemoryOp.h"
#include "IfOp.h"
#include "DataOpSlot.h"
#include "MyLinkedList.h"
#include "PropertyIncludes.h"
#include "UDProperty.h"
#include "DUProperty.h"
#include "ComputeOpCodeStruct.h"
#include "SetList.h"
#include "BasicBlockNode.h"
#include "BBIterator.h"
#include "CnstProperty.h"
#include "ImmediateArg.h"
#include "IConstArg.h"
#include "FConstArg.h"
#include "RegisterFileList.h"
#include "DeleteProperty.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

#define MAX_NUM_OF_OPERATION 20   // change according to opCodeFile
#define VALUEPROP 0
#define COPYPROP 1


int registerCounter = 35; // pseudo-reg auto generator


extern  RegisterFileList* globalRegFileList;
extern  struct ComputeOpCodeStruct ComputeOpCodeTable[]; // CnstProp.cc

extern int  findOpIndex(char* );
extern void linkUDDU(BaseArgument*,BaseArgument*,BaseOperation*,BaseOperation*);
extern int  SameSymbol(BaseArgument*, BaseArgument*);


static MyLinkedList<ComputeOp*> visit;


extern Routine*	currentRoutine; 


//==========================================================================
// commutativity
//==========================================================================

static
int
commutativity(char* name) {
   int index = findOpIndex(name);
   if ( index == _INVALID_ )
	return _NO_;
   return  (ComputeOpCodeTable[index].commutative);
}



//==========================================================================
// compCnstPropAndImm
//
// compare whether the const value in one arg's const property
// is equal to the immediate value in another arg or not
//==========================================================================
static
int
compCnstPropAndImm(CnstProperty* prop, BaseArgument* arg) {
   // Float ??????????
   if ( arg->isIConstArg() && prop->getType() == ICONSTTYPE )
      return ( prop->getIConst() != ((IConstArg*)arg)->value() );

   return 1;
}



//==========================================================================
// compareArg
//
// compare if two args are equal or not
// 0 indicate equal
//==========================================================================

static
int
compareArg(BaseArgument* arg1, BaseArgument* arg2) {
   if ( arg1 == NULL  || arg2 == NULL ) {
	if ( arg2 == arg1 )
		return 0;
	return 1;
   }
   CnstProperty* prop;
   if ( arg1->isRegisterArg() ) {
      if ( arg2->isRegisterArg() ) 
         return  ( !SameSymbol(arg1, arg2) );
      else {
         if ( prop = (CnstProperty*)arg1->getPtrToProperty(_CNSTPROP_) ) {
            return compCnstPropAndImm(prop, arg2);
         }
         return 1;
      }
   } else {  // Imm
      if ( arg2->isRegisterArg() ) {
         if ( prop = (CnstProperty*)arg2->getPtrToProperty(_CNSTPROP_) ) {
            return compCnstPropAndImm(prop, arg1);
         }
         return 1;
      } else {
         // no isEq in FConstArg ??????
         return (!((ImmediateArg*)arg1)->isEq(arg2));  
      }
   }
}


//=====================================================================
// compare
// 
// compare if two compute expressions are equal or not
// 0 indicate equal
//=====================================================================
static
int
compare(ComputeOp* targ, ComputeOp* source) {
   // same op code
   if ( targ->getOpCodeIndex() != source->getOpCodeIndex() )
      return 1; // not equal

   int commute = commutativity(globalOpCodeTable[targ->getOpCodeIndex()].opName());
   BaseArgument* tar_1 = targ->ptrToOperand(_SOURCE_1_);
   BaseArgument* sour_1 = source->ptrToOperand(_SOURCE_1_);
   if ( !compareArg(tar_1, sour_1)  &&
        !compareArg(targ->ptrToOperand(_SOURCE_2_), 
		    source->ptrToOperand(_SOURCE_2_) ) )
	return 0;
   if ( commute ) { // commutativity
      if ( !compareArg(tar_1, source->ptrToOperand(_SOURCE_2_))  &&
           !compareArg(sour_1, targ->ptrToOperand(_SOURCE_2_)) )
		return 0;
   }

   return 1;
}



//=====================================================================
// getType
// 
// get the type of register argument
//=====================================================================
static
int
getType(BaseArgument* arg) {
#ifdef DEBUG
   assert(arg->isRealRegArg() );
#endif
   return (*globalRegFileList)[((RealRegArg*)arg)->regFileIndex()].valType();
}



//=====================================================================
// copyToArg
//=====================================================================

static void
copyToArg(BaseOperation* op,int position,BaseArgument* old,BaseOperation* source){
	BaseArgument* arg;
	
	if (position == _DEST_)
	{
		arg = op->ptrToOperand(position, _DEST_LIST_);
		if ( arg && SameSymbol(arg, old) ) 
		{
			delete arg;
			BaseArgument* value = source->ptrToOperand(_DEST_, _DEST_LIST_);
			op->addOperand(value, position, _DEST_LIST_);
			linkUDDU(value, op->ptrToOperand(position, _DEST_LIST_), source, op);
		}
	}
	else
	{
		arg = op->ptrToOperand(position, _SRC_LIST_);
		if ( arg && SameSymbol(arg, old) ) 
		{
			delete arg;
			BaseArgument* value = source->ptrToOperand(_DEST_, _DEST_LIST_);
			op->addOperand(value, position, _SRC_LIST_);
			linkUDDU(value, op->ptrToOperand(position, _SRC_LIST_), source, op);
		}
	}
	
}


//=====================================================================
//  copyProp
//=====================================================================
static
void
copyProp(BaseArgument* old,  ComputeOp* source) {
   DUChainProperty* prop=(DUChainProperty*)old->getPtrToProperty(_DUCHAIN_); 
   if ( prop ) {
      MyLinkedListIterator<BaseOperation*>* iter=prop->useListIteratorForw();
      while ( iter->hasMoreElements() ) {
         BaseOperation* op = iter->currentElement();
	 copyToArg(op, _SOURCE_1_, old, source);
	 copyToArg(op, _SOURCE_2_, old, source);
         if ( op->isMemoryOp() && ((MemoryOp*)op)->isStore() ) {
	      copyToArg(op, _DEST_, old, source);
         }
         iter->nextElement();
      }
      delete iter;
   }
}


//=====================================================================
// changeSource
//=====================================================================
static
void
changeSource(ComputeOp* source, BaseArgument* arg) {
   
   // for example: t0=y+z; if (...) t1=y+z; else t2=y+z;
   // when handle t1, the t0 and t1 statement have been changed
   // when handle t2, need not change t0 again
   if ( visit.contains(source) == _NO_ ) {
      visit.append(source);
      BaseArgument* destOfS = source->ptrToOperand(_DEST_, _DEST_LIST_);
      source->addOperand(arg, _DEST_, _DEST_LIST_);
      // add _SSA_ property
      BaseArgument* newDest = source->ptrToOperand(_DEST_, _DEST_LIST_);
      if ( BaseArgProperty* prop = arg->getPtrToProperty(_SSA_) )
         newDest->addProperty(*prop);

      // insert a new instr below source 
      // or not insert, do copy propagation, 
      // change all the usage of destOfS to new register;
      copyProp(destOfS, source);
      delete destOfS;
   }
}


//==================================================================
// removeFromDefProp
// 
// remove definition from bb's def property
// without reverse link from argument to op,
// from op to instr, from instr to bb,
// this process seems very time-consuming.
// we can do this process in the dead code elimination phase
// but doing here makes modular clearly
//==================================================================

static
void
removeFromDefProp(BaseArgument* arg) {
	LinkedListIterator<BasicBlockNode>* bbIter=currentRoutine->getCFG().createBBIterForw();
	while ( bbIter->hasMoreElements() ) {
		CompDefBBProperty* prop = (CompDefBBProperty*)bbIter->currentElement().getPtrToProperty(_COMPDEF_);
                if ( prop ) {
                   if ( prop->getDefList().remove(arg) ) {
                      if ( prop->getDefList().isEmpty() ) 
                         bbIter->currentElement().removeProperty(*prop);
                      break;
                   }
                }
		bbIter->nextElement();
	} 	
	delete bbIter;
}


void delUseFromDef(MyLinkedList<BaseOperation*>&, BaseOperation*);

//=====================================================================
// setCompOpAsDel
//
// note: this operation is a definition
//=====================================================================

void
setOpAsDel(BaseOperation* op){
	// first: delete the rvalue from it definition's duchain
	UDChainProperty* udProp = (UDChainProperty*)op->ptrToOperand(_SOURCE_1_)->getPtrToProperty(_UDCHAIN_);
	if ( udProp ) {
	   delUseFromDef(udProp->getDefList(), op); 
	}

	BaseArgument* source = op->ptrToOperand(_SOURCE_2_);
	if ( source ) {
	   udProp = (UDChainProperty*)source->getPtrToProperty(_UDCHAIN_);
	   if ( udProp ) {
	      delUseFromDef(udProp->getDefList(), op); 
	   }
	}
	
	// then: set this copy op as deleted
	DeleteProperty tmp;
	op->addProperty(tmp);

	// remove this op from bb's def property
        removeFromDefProp(op->ptrToOperand(_DEST_, _DEST_LIST_));
}



//=====================================================================
// delUseFromDef
//
// delete one use from all its definition's du chain
//=====================================================================

void
delUseFromDef(MyLinkedList<BaseOperation*>& defList, 
	      BaseOperation* useOp) {
  MyLinkedListIterator<BaseOperation*>* iter=defList.elementsForward();
  while ( iter->hasMoreElements() ) {
      BaseOperation* defOp = iter->currentElement();
      if ( defOp == useOp ) {
         iter->nextElement();
	 continue;
      }
      BaseArgument* def = defOp->ptrToOperand(_DEST_, _DEST_LIST_);
      ((DUChainProperty*)def->getPtrToProperty(_DUCHAIN_))->remove(useOp);
      if ( ((DUChainProperty*)def->getPtrToProperty(_DUCHAIN_))->getUseList().isEmpty() ) {
	// delete this definition
	setOpAsDel(defOp);
      }
      iter->nextElement();
  }
  delete iter;
}



//=====================================================================
// changeTarget
// 
// change the @targ operation
// @targ: the operation that contains cse that will be replaced
//=====================================================================

static
void
changeTarget(ComputeOp* targ, BaseArgument* argument) {
   // first, delete the ud / du relative to targ' two source
   BaseArgument* tag_1 = targ->ptrToOperand(_SOURCE_1_);
   UDChainProperty* prop = (UDChainProperty*)tag_1->getPtrToProperty(_UDCHAIN_);
   if ( prop ) 
      delUseFromDef(prop->getDefList(), targ);
   delete tag_1;

   BaseArgument* tag_2 = targ->ptrToOperand(_SOURCE_2_);
   if ( tag_2 ) {
   	prop = (UDChainProperty*)tag_2->getPtrToProperty(_UDCHAIN_);
   	if ( prop ) 
		delUseFromDef(prop->getDefList(), targ);
        targ->clearOperand(_SOURCE_2_);
   }

   // then, change targ to ASSIGN,
   int type = getType(targ->ptrToOperand(_DEST_, _DEST_LIST_));
   if ( type == _INT_ ) { // IASSIGN
      targ->initOpCode(globalOpCodeTable.getIndex("IASSIGN"));  
   } else { // FASSIGN
      targ->initOpCode(globalOpCodeTable.getIndex("FASSIGN"));  
   }
   targ->addOperand(argument, _SOURCE_1_);
   // add _SSA_ property
   BaseArgument* sourOfT = targ->ptrToOperand(_SOURCE_1_);
   if ( BaseArgProperty* prop = argument->getPtrToProperty(_SSA_) )
      sourOfT->addProperty(*prop);
}



//=====================================================================
// propagationInBB
//
// @source: the source cse
// @targ:   the cse that need to be replaced 
//=====================================================================

static
void
propagationInBB(ComputeOp* source, ComputeOp* targ) {
   BaseArgument* destOfS = source->ptrToOperand(_DEST_, _DEST_LIST_);
   changeTarget(targ, destOfS);
   BaseArgument* sourOfT = targ->ptrToOperand(_SOURCE_1_);
   // last, link the new ud / du relation
   linkUDDU(destOfS, sourOfT, source, targ);
}



//=====================================================================
// valuePropInBB
//
// value propagation within bb
//=====================================================================

static
int
valuePropInBB(ComputeOp* op,SetList<ComputeOp*>& opList) {
   MyLinkedListIterator<ComputeOp*>* opIter = opList.elementsForward();
   while ( opIter->hasMoreElements() ) {
      ComputeOp* source = opIter->currentElement();
      if ( !compare(op, source) ) {
           propagationInBB(source, op);
           delete opIter;
           return _YES_;
      }
      opIter->nextElement();
   }
   delete opIter;
   return _NO_; 
}



//=========================================================================
// globValueProp
//
// global simple cse
//=========================================================================
static
void
globValueProp(ComputeOp* op, SetList<ComputeOp*>& opList, BasicBlockNode* bb) {

   MyLinkedListIterator<ComputeOp*>* opIter = opList.elementsForward();

   MyLinkedList<ComputeOp*> cseSet;

   // first find all cses of op
   while ( opIter->hasMoreElements() ) {
      ComputeOp* source = opIter->currentElement();
      if ( !compare(op, source) ) {
           cseSet.append(source);
      }
      opIter->nextElement();
   }
   delete opIter;

   int numItems = cseSet.numItems();
   if ( numItems == 0 ) 
      return;
   else if ( numItems == 1 )  {
      propagationInBB(cseSet.getHead(), op);
   } else {
      BaseArgument* regArg =  globalRegFileList->getRealReg(registerCounter++);
      SSAArgProperty ssaProp(1);
      regArg->addProperty(ssaProp);
      // change target
      changeTarget(op, regArg);
      // change source and copy propagation
      opIter = cseSet.elementsForward();
      while ( opIter->hasMoreElements() ) {
         ComputeOp* sourceOp = opIter->currentElement();
	 changeSource(sourceOp, regArg);
         linkUDDU(sourceOp->ptrToOperand(_DEST_, _DEST_LIST_), op->ptrToOperand(_SOURCE_1_),                   sourceOp, op);
         opIter->nextElement();
      }
      delete opIter;
   } 
}




//=====================================================================
// removeFromGen
// 
// see MinusKill for the explanation of tag
//======================================================================

void
removeFromGen(SetList<ComputeOp*>& gen, BaseArgument* arg, int tag) {
#ifdef DEBUG
   assert(arg->isRegisterArg());
#endif
   //MyLinkedList<ComputeOp*> removeSet;
   MyLinkedListIterator<ComputeOp*>* genIter = gen.elementsForward();
   while ( genIter->hasMoreElements() ) {
      ComputeOp* op = genIter->currentElement();
      genIter->nextElement();
      BaseArgument* sour_1 = op->ptrToOperand(_SOURCE_1_); 
      BaseArgument* sour_2 = op->ptrToOperand(_SOURCE_2_);
      BaseArgument* targ = op->ptrToOperand(_DEST_, _DEST_LIST_);
      if ( tag == VALUEPROP 
	&& ( sour_1->isRegisterArg() && SameSymbol(arg, sour_1)
          || sour_2 && sour_2->isRegisterArg() && SameSymbol(arg, sour_2) ) 
	|| tag == COPYPROP
	&& ( sour_1->isRegisterArg() && SameSymbol(arg, sour_1)
	  || SameSymbol(arg, targ) ) ) 
        gen.remove(op);
	//removeSet.append(op);
   }
   //gen.minus(removeSet);
   delete genIter;
}


//=========================================================================
// CSEInInstr
//=========================================================================

static
void
CSEInInstr(BaseInstruction* instr,SetList<ComputeOp*>& exprSet,BasicBlockNode* tag){

#ifdef DEBUG
      assert ( instr->isNormalInstr() ); 
#endif
	ArrayIterator<DataOpSlot>* iter = ((NormalInstr*)instr)->getDataOpSlotIter();
	while ( iter->hasMoreElements() ) {
 	   BaseOperation* op = iter->currentElement().getPtrToOperation();
	   if ( op == NULL ) {
		iter->nextElement();
		continue;
	   }
           if ( op->isComputeOp() ) {
	     if ( op->ptrToOperand(_SOURCE_2_) ) {
		// compute operation
                if ( tag == 0 ) {
		   if ( valuePropInBB((ComputeOp*)op, exprSet) == _NO_ ) {
		      ComputeOp* compOp = (ComputeOp*)op;
                      exprSet.append(compOp);
		   }
         	   // remove expr from exprSet -- genSet
         	   // for example:  ... = a+b;  a = ...;  ( move "a+b" )
                   removeFromGen(exprSet,op->ptrToOperand(_DEST_, _DEST_LIST_),VALUEPROP);
                } else {
		   globValueProp((ComputeOp*)op, exprSet, tag);
                }
	     }
	   }
           else if ( op->isMemoryOp() 
                  && ((MemoryOp*)op)->isLoad()
                  && tag == 0 ) { // remove
             removeFromGen(exprSet, ((MemoryOp*)op)->ptrToDestOperand(), VALUEPROP);
           }
	   iter->nextElement();
	}	
	delete iter;
}


//=========================================================================
// CSE
//
// @parameters
// tag = 0: cse in bb, 
// and generate the gen-expr set of this bb
// the exprSet is the bb's gen set 
// tag = 1: cse acorss bb, 
// the  exprSet is the bb's in set
//=========================================================================

static
void 
CSE(BasicBlockNode* bb, SetList<ComputeOp*>& exprSet, int tag) {
   if ( tag && exprSet.isEmpty() ) // global cse and no cse reaching bb
	return;
   BBForwIterator iter(bb);
   while ( iter.hasMoreElements() ) {
      if ( tag ) 
         CSEInInstr(iter.currentElement(), exprSet, bb);
      else
         CSEInInstr(iter.currentElement(), exprSet, 0);
      iter.nextElement();
   } 
}


//==================================================================
// MinusKill
//
// @parameter: arg,
// this function is shared by ValueProp's compIn and CopyProp's compIn
// these two need differ;
// for ValueProp: x = a + b;	only later definitions of a and b
// will kill this cse;
// but for CopyProp: x = a;	only only definition of a will kill
// this copy , the definition of x will also kill this copy
// tag == 0 indicates ValueProp; otherwise CopyProp; 
//==================================================================

void
MinusKill(SetList<ComputeOp*>& my_set, CompDefBBProperty* prop, int tag) {
   if ( prop ) {
      MyLinkedList<BaseArgument*>& def = prop->getDefList();
      MyLinkedListIterator<BaseArgument*>* iter=def.elementsForward();
      while ( iter->hasMoreElements() ) {
         removeFromGen(my_set, iter->currentElement(), tag);
         iter->nextElement();
      }
      delete iter;
   }
}



//==================================================================
// contains
//==================================================================
static
ComputeOp* 
contains(SetList<ComputeOp*>& my_set, ComputeOp* op) {
   MyLinkedListIterator<ComputeOp*>* iter = my_set.elementsForward();
   while ( iter->hasMoreElements() ) {
      ComputeOp* ret = iter->currentElement();
      if ( !compare(ret, op) ) {
         delete iter;
         return ret;
      }
      iter->nextElement();
   }
   delete iter;
   return 0;
}


//==================================================================
// exprUnion
//==================================================================
static
void
exprUnion(SetList<ComputeOp*>& targ, SetList<ComputeOp*>& source) {
   MyLinkedListIterator<ComputeOp*>* iter = source.elementsForward();
   while ( iter->hasMoreElements() ) {
      ComputeOp* sameOp = 0;
      if ( sameOp = contains(targ, iter->currentElement()) ) {
         // the op in gen set kills the op from in set
         // this is the most-nearest point in path
	 targ.remove(sameOp);
	 targ.append(iter->currentElement());
      } else
         targ.append(iter->currentElement());
      iter->nextElement();
   }
   delete iter;
}


//========================================================================
// listIntersection
//========================================================================

static
void
listIntersection(SetList<ComputeOp*>& targ,SetList<ComputeOp*>&source){
  MyLinkedListIterator<ComputeOp*>* iter = targ.elementsForward();
  //MyLinkedList<ComputeOp*> removeSet;
  while ( iter->hasMoreElements() ) {
      ComputeOp* sameOp = 0;
      ComputeOp* op = iter->currentElement();
      if ( sameOp = contains(source, op) ) {
         targ.add(sameOp); // different path, record.
         iter->nextElement();
      } else {
         iter->nextElement();
         targ.remove(op);
	 //removeSet.append(op);
      }
  }
  //targ.minus(removeSet);
  delete iter;
}

//==========================================================================
// compInOfBB
//
// given gen, compute the data flow equation
// out = in - kill U gen
// in = intersection of predecessor's out
//==========================================================================

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
               listIntersection(oneIn, out[index]);
         }
         predIter->nextElement();     
      }
      delete predIter;
      // compute out
      out[currIndex] = oneIn;
      MinusKill(out[currIndex], (CompDefBBProperty*)bb->getPtrToProperty(_COMPDEF_) , VALUEPROP);
      exprUnion(out[currIndex], gen[currIndex]);
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
            listIntersection(tmp, out[predIter->currentElement()->getDFSNum()]);
            predIter->nextElement();     
         }
         delete predIter;

         if ( tmp.operator!= (in[index]) ) {
           in[index] = tmp;
           out[index] = tmp;
           MinusKill(out[index], (CompDefBBProperty*)bb->getPtrToProperty(_COMPDEF_) , VALUEPROP);
           exprUnion(out[index], gen[index]);
           change = 1; 
	 }  
         bbIter->nextElement();
         tmp.removeAll();
      }
      bbIter->reset();
   }
}


//==================================================================
// valueProp
//
// simple cse
//==================================================================

void
Routine::valueProp() {
#ifdef DEBUG
   printf(" begin to value propagation \n");
#endif


   int numBBs = _cfg.numBBs();
   SetList<ComputeOp*>* gen = new SetList<ComputeOp*> [numBBs+1];
   ArrayIterator<BasicBlockNode*>* bbIter = _cfg.createDFSBBIter();
   while ( bbIter->hasMoreElements() ) {
      BasicBlockNode* bb = bbIter->currentElement();
      CSE(bb, gen[bb->getDFSNum()], 0); // 0 indicates cse in bb
      bbIter->nextElement();
   }
   bbIter->reset();

   SetList<ComputeOp*>* in = new SetList<ComputeOp*> [numBBs+1];
   SetList<ComputeOp*>* out = new SetList<ComputeOp*> [numBBs+1];
   compInOfBB(bbIter, gen, in, out);

   bbIter->reset();
   delete [] gen;
   delete [] out;
   delete bbIter;

   // backwards, global CSE
   // why backwards: for example:
   // x = a+b;  if ( )  y = a+b;  z = a+b;
   // backwards, so, first cse to z, so change x and y
   // if forwards, first cse to y, change x;
   // then cse to z, y has been changed, no a+b in y's operation
   bbIter = _cfg.createDFSBBIterBack();
   while ( bbIter->hasMoreElements() ) {
	   BasicBlockNode* bb = bbIter->currentElement();
      	   CSE(bb, in[bb->getDFSNum()], 1); // 1 indicates cse across bb
	   bbIter->nextElement();
   }


   // clear global 
   visit.removeAll();

   delete bbIter;
   delete [] in;

#ifdef DEBUG
   printf("end of value propagation\n");
#endif
}
