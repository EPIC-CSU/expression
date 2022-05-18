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
// File:          CnstProp.cc
// Purpose:       const  propagation 
// Author:        Xiaomei Ji
// 
//=========================================================================

// $Id
#include "stdafx.h"

#ifndef WIN32
#include <stdarg.h>
#include <string.h>
#endif

#include "OpCodeTable.h"
#include "SymbolTable.h"
#include "BaseOperation.h"
#include "NormalOp.h"
#include "MemoryOp.h"
#include "ComputeOp.h"
#include "IfOp.h"
#include "IConstArg.h"
#include "FConstArg.h"
#include "BaseArgument.h"
#include "BaseInstruction.h"
#include "NormalInstr.h"
#include "DataOpSlot.h"
#include "MyLinkedList.h"
#include "CompilerRTStatus.h"
#include "DUProperty.h"
#include "UDProperty.h"
#include "Routine.h"
#include "CnstProperty.h"
#include "DeleteProperty.h"
#include "BranchProperty.h"
#include "BBIterator.h"
#include "ComputeOpCodeStruct.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

extern   OpCodeTable 		globalOpCodeTable;
extern   SymbolTable* 		globalSymbolTable;
extern   RegisterFileList*	globalRegFileList;

extern void delUseFromDef(MyLinkedList<BaseOperation*>&, BaseOperation*);
extern int  SameSymbol(BaseArgument*, BaseArgument*);

static int eval_Icompute(BaseOperation*, ...);
static int eval_Dcompute(BaseOperation*, ...);

// do not change the order, the order is the same as defined
// in enum computeOpCode
// NOTES: pseudoly set canSource_2BeImm as YES in order for folding 
struct ComputeOpCodeStruct ComputeOpCodeTable[] = {
{"IASSIGN", ICONSTTYPE, eval_Icompute, _NO_, _YES_},
{"IADD", ICONSTTYPE, eval_Icompute, _YES_, _YES_},
{"IGE", ICONSTTYPE, eval_Icompute, _NO_, _YES_},
{"IASH", ICONSTTYPE, eval_Icompute, _NO_, _YES_},
{"ISUB", ICONSTTYPE, eval_Icompute, _NO_, _YES_},
{"ILSH", ICONSTTYPE, eval_Icompute, _NO_, _YES_},
{"DMUL", DCONSTTYPE, eval_Dcompute, _YES_, _NO_},
{"DSUB", DCONSTTYPE, eval_Dcompute, _NO_, _NO_},
{"ILT", ICONSTTYPE, eval_Icompute, _NO_, _YES_}
};


BaseArgProperty* CnstProperty::copy()  
   {
      BaseArgProperty* prop;
      prop = new CnstProperty(_val, _type);
      return prop;
   }



//===================================================================
// eval_Icompute
//
// evalate interger
//==================================================================

static
int
eval_Icompute(BaseOperation* useOp, ... ) {
   int tmpVal = 0;
   CnstProperty* prop;

   va_list argPtr;
   va_start(argPtr, useOp);
   int* result = va_arg(argPtr, int*);
   BaseArgument* arg = ((ComputeOp*)useOp)->ptrToOperand(_SOURCE_1_);
   if ( arg->isImmediateArg() ) {
      *result = ((IConstArg*)arg)->value();
   }
   else {
      prop = (CnstProperty*)arg->getPtrToProperty(_CNSTPROP_);
      if ( prop ) 
         *result = prop->getIConst();
      else  {
         va_end(argPtr);
	 return _NO_;
      }
   }
   arg = ((ComputeOp*)useOp)->ptrToOperand(_SOURCE_2_);
   if ( arg ) {
      if ( arg->isImmediateArg() ) {
      	tmpVal = ((IConstArg*)arg)->value();
      }
      else {
         prop = (CnstProperty*)arg->getPtrToProperty(_CNSTPROP_);
         if ( prop ) 
            tmpVal = prop->getIConst();
         else { 
            va_end(argPtr);
            return _NO_;
         }
      }
   }  
   int opCodeIndex = va_arg(argPtr, int);
   switch ( opCodeIndex ) {
      case IADD:
         *result += tmpVal;
         break;
      case ISUB:
         *result -= tmpVal;
         break;
      case IGE:
         *result = ( *result >= tmpVal ? 1 : 0 );
         break;
      case ILT:
         *result = ( *result < tmpVal ? 1 : 0 );
         break;
      case IASH:
         *result >>= tmpVal;
         break;
      case ILSH:
         *result <<= tmpVal ;
         break;
   }
   va_end(argPtr);
   return _YES_;
}


//===================================================================
// eval_Dcompute
//
// evalate double 
//==================================================================

static
int
eval_Dcompute(BaseOperation* useOp, ... ) {
   // need getType() of the cnstProp, and then get right value.
   // ????????????????????????????????????????

	//NOS
	ASSERT_TRACE(0,"Forced EXIT!");
	return 0;
}

//==================================================================
// getCnstValue
//==================================================================

static
int
getCnstValue(MyLinkedList<BaseOperation*>& defList,
	     union multiVal& value,
	     int type){
   MyLinkedListIterator<BaseOperation*>* iter = defList.elementsForward();
   int first = _YES_;
   while ( iter->hasMoreElements() ) {
      BaseOperation* def = iter->currentElement();
      char* name=globalOpCodeTable[((NormalOp*)def)->getOpCodeIndex()].opName();
      if ( type == _INT_ ) {
	if ( !strcmp(name, "ICONSTANT") ) {
	   if ( first ) {
		first = _NO_;
		value._ival = ((IConstArg*)((ComputeOp*)def)->ptrToOperand(_SOURCE_1_))->value();
	   } else {
		if ( value._ival != ((IConstArg*)((ComputeOp*)def)->ptrToOperand(_SOURCE_1_))->value() )
			return _NO_;
	   }
	} else
	   return _NO_;
      } else {
	if (!strcmp(name, "FCONSTANT") || !strcmp(name, "DCONSTANT") ){
	   if ( first ) {
		first = _NO_;
		value._fval = (float)(((FConstArg*)((ComputeOp*)def)->ptrToOperand(_SOURCE_1_))->value());
	   } else {
		if ( value._fval != ((FConstArg*)((ComputeOp*)def)->ptrToOperand(_SOURCE_1_))->value() )
			return _NO_;
	   }
	} else
	   return _NO_;
      }
      iter->nextElement();
   }
   delete iter;
   return _YES_;
}


//==================================================================
// cnstPropInArg
//
// only propagate to RealRegisterArgument
//==================================================================
static
int
cnstPropInArg(BaseArgument* arg, BaseOperation* useOp, BaseArgument* defArg) {
  if ( arg->isRealRegArg() &&
       ( defArg == NULL || SameSymbol(arg, defArg) )  ) {
    UDChainProperty* prop = (UDChainProperty*)arg->getPtrToProperty(_UDCHAIN_);
    if ( prop ) {
	    union multiVal value;
	    int	isCnst = 0;
            // set its constant property
            // check the type of register according to register file 
            if ( (*globalRegFileList)[((RealRegArg*)arg)->regFileIndex()].valType() == _INT_ ) {
	        if ( getCnstValue(prop->getDefList(), value, _INT_) ) {	
               		CnstProperty tmp(value._ival);
               		arg->addProperty(tmp);
			isCnst = 1;
		}
            } else {  // FReg
	        if ( getCnstValue(prop->getDefList(), value, _FLOAT_) ) {	
               		CnstProperty tmp(value._fval);
               		arg->addProperty(tmp);
			isCnst = 1;
		}
            }
	    if ( isCnst ) {
            	// modify du/ud property 
	    	delUseFromDef(prop->getDefList() , useOp);
            	arg->removeProperty(arg->getProperty(_UDCHAIN_));
            	return _YES_;
	    }
    }
  }
  return _NO_;
}




//==================================================================
// findOpIndex
//
// return the opcode's index in ComputeOpCodeTable
//==================================================================
int
findOpIndex(char* opName) {
	for (int index=0; index<sizeof(ComputeOpCodeTable)/sizeof(ComputeOpCodeStruct); index++ ) {
		if ( !strcmp(opName, ComputeOpCodeTable[index].name) )
                	return index;
	} 
	return _INVALID_;
}




//==================================================================
// addProperties
//==================================================================
static
void
addProperties(BaseArgument* target, BaseArgument* source) {
   LinkedListIterator<BaseArgProperty>* iter = source->propIterForw();
   while ( iter->hasMoreElements() ) {
      target->addProperty(iter->currentElement());
      iter->nextElement();
   }
   delete iter;
}




//==================================================================
// cnstFold
//==================================================================
static
int 
cnstFold(BaseOperation*& useOp, MyLinkedList<BaseOperation*>&queue, int index){
      int evalRes = _NO_; 
#ifdef DEBUG
      assert(useOp->isComputeOp() );
#endif
       if ( ComputeOpCodeTable[index].retType == ICONSTTYPE ) {
	     int returnVal;
	     evalRes = ComputeOpCodeTable[index].f(useOp, &returnVal, index);
             if ( evalRes == _YES_ ) {
	       // only change opCode and source_1, delete source_2
               ((ComputeOp*)useOp)->initOpCode(globalOpCodeTable.getIndex("ICONSTANT"));
               delete ((ComputeOp*)useOp)->ptrToOperand(_SOURCE_1_);
               IConstArg tmpConst(returnVal);
	       ((ComputeOp*)useOp)->addOperand(&tmpConst, _SOURCE_1_);
	       ((ComputeOp*)useOp)->clearOperand(_SOURCE_2_);
               queue.append(useOp);
             } 
	} else { // FTYPE
	     float returnVal;
	     evalRes = ComputeOpCodeTable[index].f(useOp, &returnVal, index);
             if ( evalRes == _YES_ ) {
               // change useOp to FCONSTANT ????????????????
               //useOp=new FConstant(((ComputeOp*)useOp)->ptrToOperand(_DEST_, _DEST_LIST_), returnVal);
               queue.append(useOp);
             } 
       }
       return evalRes;
}


//==================================================================
// commuteArgs
//==================================================================
static
void
commuteArgs(ComputeOp* op ) {
   BaseArgument* s_1 = op->ptrToOperand(_SOURCE_1_);
   BaseArgument* s_2 = op->ptrToOperand(_SOURCE_2_);
   op->addOperand(s_2, _SOURCE_1_);
   // add properties
   addProperties(((ComputeOp*)op)->ptrToOperand(_SOURCE_1_), s_2);
   CnstProperty* prop = (CnstProperty*)s_1->getPtrToProperty(_CNSTPROP_);
#ifdef DEBUG
   assert(prop);
#endif
   if ( prop->getType() == ICONSTTYPE ) {
      IConstArg tmpCon(prop->getIConst());
      op->addOperand(&tmpCon, _SOURCE_2_);
   }
/***
   else { // FTYPE
      FConstArg tmpFCon(prop->getFConst());
      op->addOperand(&tmpFCon, _SOURCE_2_);
   }
***/
   delete s_1;
   delete s_2;
}


//==================================================================
// setDelOfIf
//
// to if structure, 
// if this structure can be determined as true or false statically 
// and it is regular , 
// then first set this if statement and its true or false  branch as deleted 
//==================================================================
static
void
setDelOfIf(IfOp* op, int cond) {
/****** ?????????????
   if ( op->isRegular() ) {
        MyLinkedList<BaseOperation*>* branch;

	DeleteProperty delProp;
	op->addProperty(delProp);

	if ( cond == _YES_ ) 
	   branch = op->OpsInFalseBranch();
  	else 
	   branch = op->OpsInTrueBranch();
	MyLinkedListIterator<BaseOperation*>* iter=branch->elementsForward();
	while ( iter->hasMoreElements() ) {
	   iter->currentElement()->addProperty(delProp);
	   iter->nextElement();
	}
	delete iter;
   }
**********/
}



static void
changeArgToCnst(ComputeOp* useOp, int index) 
{
	CnstProperty* prop;

	if (index == _DEST_)
		prop = (CnstProperty*)useOp->ptrToOperand(index, _DEST_LIST_)->getPtrToProperty(_CNSTPROP_);
	else
		prop = (CnstProperty*)useOp->ptrToOperand(index, _SRC_LIST_)->getPtrToProperty(_CNSTPROP_);

	if ( prop->getType() == ICONSTTYPE ) 
	{
        IConstArg tmpICon(prop->getIConst());
		if (index == _DEST_)
			((ComputeOp*)useOp)->addOperand(&tmpICon, index, _DEST_LIST_);
		else
			((ComputeOp*)useOp)->addOperand(&tmpICon, index, _SRC_LIST_);
	} 
	else 
	{ // FTYPE ???????????
	}
}



//==================================================================
// cnstPropInOp
//==================================================================

static
void
cnstPropInOp(BaseOperation*& useOp,MyLinkedList<BaseOperation*>& queue,
	     BaseArgument* defArg = 0){
   if ( useOp->isComputeOp() ) {
      int index = findOpIndex(globalOpCodeTable[((NormalOp*)useOp)->getOpCodeIndex()].opName());
      if ( index == _INVALID_ ) 
	return;
      int source1IsCnst = 0;
      int source2MayBeImm = ComputeOpCodeTable[index].canSource_2BeImm;
      if (cnstPropInArg(((ComputeOp*)useOp)->ptrToOperand(_SOURCE_1_),useOp, defArg)){
         if ( source2MayBeImm && cnstFold( useOp, queue, index) == _YES_ )
            return;
         source1IsCnst = 1;
      }
      BaseArgument* source_2 = ((ComputeOp*)useOp)->ptrToOperand(_SOURCE_2_);
      if ( !source_2 ) {
	;
      } else {
        if ( cnstPropInArg(source_2, useOp, defArg) ) {    
         if ( source1IsCnst && cnstFold(useOp, queue, index) == _NO_ 
	   || !source1IsCnst )
            if ( ComputeOpCodeTable[index].canSource_2BeImm ) {
               // change source_2 to const arg
	       changeArgToCnst((ComputeOp*)useOp, _SOURCE_2_);
            }
         } else if ( source1IsCnst ) { 
           if ( ComputeOpCodeTable[index].commutative )
             commuteArgs((ComputeOp*)useOp);
         }
      }
   } else if ( useOp->isMemoryOp() ) {
      cnstPropInArg(((MemoryOp*)useOp)->ptrToSourceOperand(), useOp, defArg);
      if ( ((MemoryOp*)useOp)->isStore() ) 
         cnstPropInArg(((MemoryOp*)useOp)->ptrToDestOperand(), useOp, defArg);
   } else if ( useOp->isIfOp() ) {
      BaseArgument* arg = ((IfOp*)useOp)->ptrToCCReg();
      if ( cnstPropInArg(arg, useOp, defArg) ) {
         // set if's property, whether it is TRUE or FALSE
         // $cc must be an iReg 
      	 CnstProperty* prop = (CnstProperty*)arg->getPtrToProperty(_CNSTPROP_);
         if ( prop->getIConst() ) { // set TRUE
            BranchProperty tmp(_TRUE_);
	    setDelOfIf((IfOp*)useOp, _TRUE_); 
            useOp->addProperty(tmp);
         } else {
            BranchProperty tmp(_FALSE_);
	    setDelOfIf((IfOp*)useOp, _FALSE_); 
            useOp->addProperty(tmp);
         }
      }
   }
}


//==================================================================
// cnstPropInInstr
//==================================================================

static
void
cnstPropInInstr(BaseInstruction* instr, MyLinkedList<BaseOperation*>& queue) {
   // data operation
   ArrayIterator<DataOpSlot>* iter = ((NormalInstr*)instr)->getDataOpSlotIter();
   while ( iter->hasMoreElements() ) {
         BaseOperation* op = iter->currentElement().getPtrToOperation();
	 if ( op == NULL ) {
		iter->nextElement();
		continue;
	 }
	 cnstPropInOp(op, queue);
         iter->nextElement();
   }
   delete iter;

   // control operation
   ArrayIterator<ControlOpSlot>* cntlIter = ((NormalInstr*)instr)->getControlOpSlotIter();
   while ( cntlIter->hasMoreElements() ) {
         BaseOperation* op = cntlIter->currentElement().getPtrToOperation();
	 if ( op == NULL ) {
		cntlIter->nextElement();
		continue;
	 }
	 cnstPropInOp(op, queue);
         cntlIter->nextElement();
   }
   delete cntlIter;
}



//==================================================================
// cnstProp
//==================================================================
void 
Routine::cnstProp() {
#ifdef DEBUG
   printf(" Begin to constant propagation\n");
#endif
        

   MyLinkedListIterator<BaseInstruction*>* instrIter=this->instrListIteratorForw();

   // the queue is used to record the new const assignment
   // because of cnst-folding
   MyLinkedList<BaseOperation*> queue;

   while ( instrIter->hasMoreElements() ) {
      cnstPropInInstr(instrIter->currentElement(), queue);
      instrIter->nextElement();
   }
   
   delete instrIter;

   // cnst prop for the new const assignment, the result of cnst-folding
   while ( queue.isEmpty() == _NO_ ) {
      BaseOperation* op = queue.getHead();
      queue.removeFirst();
      // check it's du chain
      BaseArgument* defArg = op->ptrToOperand(_DEST_, _DEST_LIST_);
      DUChainProperty* prop=(DUChainProperty*)defArg->getPtrToProperty(_DUCHAIN_);
      if ( prop ) {
        MyLinkedListIterator<BaseOperation*>* iter=prop->useListIteratorForw();
	while ( iter->hasMoreElements() ) {
	   cnstPropInOp(iter->currentElement(), queue, defArg);
	   iter->nextElement();
        }
	delete iter;
      }
   }

#ifdef DEBUG
   printf(" end of constant propagation \n");
#endif
}
