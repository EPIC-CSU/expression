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
#include "stdafx.h"

#include "BaseOperation.h"
#include "BaseArgument.h"
#include "Routine.h"
#include "TreeBuilding.h"
#include "IburgToIR.h"
#include "UDProperty.h"
#include "DUProperty.h"
#include "Instruction.h"

#include "IRTraversal.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

#ifdef __UNIX__
#define ASSERT assert
#endif

extern Routine *currRoutine;

NormalInstr *BaseOperation::getInstr(void){
  MyLinkedListIterator<BaseInstruction*> *iter;
  BaseInstruction *instr;
  iter=currRoutine->getInstrList().elementsForward();
  while(iter->hasMoreElements()){
    if(iter->currentElement()->isNormalInstr() && 
       ((NormalInstr*)(iter->currentElement()))->containsOp(this)){
		instr=iter->currentElement();
		delete iter;
		return ((NormalInstr*)instr);
    }
    iter->nextElement();
  }
  delete iter;
  //abort();
  return NULL;
}

BaseOpSlot *BaseOperation::getSlot(BaseInstruction* parentInstr)
{
  NormalInstr *instr;
  ArrayIterator<DataOpSlot> *diter;
  DataOpSlot *slot;
  
  if (parentInstr==NULL) 
  {
   instr=this->getInstr();
  }
  else
  {
   instr=(NormalInstr*)parentInstr;
  }
  if(instr==NULL) return NULL; //abort();

  diter=instr->getDataOpSlotIter();
  while(diter->hasMoreElements()){
    if(diter->currentElement().hasOper() && 
       diter->currentElement().getPtrToOperation()==this){
		slot=&(diter->currentElement());
		delete diter;
		return slot;
    }
    diter->nextElement();
  }

  delete diter;
  
  ArrayIterator<ControlOpSlot> *citer;
  ControlOpSlot *cslot;
  citer=instr->getControlOpSlotIter();
  while(citer->hasMoreElements()){
    if(citer->currentElement().hasOper() && 
       citer->currentElement().getPtrToOperation()==this){
		cslot=&(citer->currentElement());
		delete citer;
		return cslot;
    }
    citer->nextElement();
  }
  delete citer;
  
  ArrayIterator<FlowOpSlot> *fiter;
  FlowOpSlot *fslot;
  fiter=instr->getFlowOpSlotIter();
  while(fiter->hasMoreElements())
  {
   if(fiter->currentElement().hasOper() && 
    fiter->currentElement().getPtrToOperation()==this)
   {
    fslot=&(fiter->currentElement());
    delete fiter;
    return fslot;
   }
   fiter->nextElement();
  }
  delete fiter;

  return NULL;
}


BaseOpSlot *getSlotIndexType(BaseOperation *oper,int &index,SlotType &type)
{
  NormalInstr *instr;
  ArrayIterator<DataOpSlot> *diter;
  DataOpSlot *slot;
  
  instr=oper->getInstr();
  if(instr==NULL) return NULL; //abort();

  diter=instr->getDataOpSlotIter();
  index=0;
  while(diter->hasMoreElements()){
    if(diter->currentElement().hasOper() && 
       diter->currentElement().getPtrToOperation()==oper){
		slot=&(diter->currentElement());
		delete diter;
		type=DATA_OP_SLOT;
		return slot;
    }
    diter->nextElement();
	index++;
  }

  delete diter;
  
  ArrayIterator<ControlOpSlot> *citer;
  ControlOpSlot *cslot;
  citer=instr->getControlOpSlotIter();
  index=0;
  while(citer->hasMoreElements()){
    if(citer->currentElement().hasOper() && 
       citer->currentElement().getPtrToOperation()==oper){
		cslot=&(citer->currentElement());
		delete citer;
		type=CONTROL_OP_SLOT;
		return cslot;
    }
    citer->nextElement();
	index++;
  }
  delete citer;
  
  ArrayIterator<FlowOpSlot> *fiter;
  FlowOpSlot *fslot;
  fiter=instr->getFlowOpSlotIter();
  index=0;
  while(fiter->hasMoreElements())
  {
   if(fiter->currentElement().hasOper() && 
    fiter->currentElement().getPtrToOperation()==oper)
   {
    fslot=&(fiter->currentElement());
    delete fiter;
	type=FLOW_OP_SLOT;
    return fslot;
   }
   fiter->nextElement();
   index++;
  }
  delete fiter;

  return NULL;
}


void BaseOperation::printProperties(ostream & out, PropNameList & pL)
{
	BaseOpProperty *prop;

	if (_opProperties.isEmpty()) return;

	out << " #* ";
	for (PropNameListIter i = pL.begin(); i != pL.end(); i++)
	{
		prop = _opProperties.getPtrToProperty(*i);
		if (prop != NULL)
			prop->print(out);
		out << "  ";
	}
	out << " *# ";
}

//==================================================================
//		interference 
//==================================================================


extern void linkAllDefAndUse(MyLinkedList<BaseOperation*>&,BaseOperation*,int);
extern void linkAllDefAndUse(MyLinkedList<BaseOperation*>&,BaseOperation*,BaseArgument*);
extern void linkUDDU(BaseArgument*,BaseArgument*,BaseOperation*,BaseOperation*);
extern int  SameSymbol(BaseArgument*, BaseArgument*);

//========================================================================
// linkDefAndAllUse
//
// search for uses of the variable defined in defOp and link UDs and DUs
// accordingly
//========================================================================

static
void
linkDefAndAllUse(BaseOperation* defOp, MyLinkedList<BaseOperation*>& useList) {
	//Please include such prints under a debug macro
	//cout << " (* " << defOp << " *) ";
	//defOp->shortPrint(cout);
	//cout << endl;
   BaseArgument* argOfT = defOp->ptrToOperand(_DEST_, _DEST_LIST_);
   MyLinkedListIterator<BaseOperation*>* iter = useList.elementsForward();
   while ( iter->hasMoreElements() ) {
	BaseOperation* useOp = iter->currentElement();
/*
	BaseArgument* argOfS = useOp->ptrToOperand(_SOURCE_1_);
	if ( SameSymbol(argOfT, argOfS) ) {
	   linkUDDU(argOfT, argOfS, defOp, useOp);
	}
	argOfS = useOp->ptrToOperand(_SOURCE_2_);
	if ( argOfS && SameSymbol(argOfT, argOfS) ) {
	   linkUDDU(argOfT, argOfS, defOp, useOp);
	}
*/

	BaseArgument *argOfS;

		// Changed by SAH : Wed, 16th August, 2000
		//
		// MULTIPLE_DESTS_AND_SOURCES
		//
		ArgList & a =useOp->sourceOperandList();
		ArgListIter ai;
		for (ai = a.begin(); ai != a.end(); ai++)
		{
			argOfS = *ai;
			if (argOfS != NULL){
				if ( SameSymbol(argOfT, argOfS) ){
					linkUDDU(argOfT, argOfS, defOp, useOp);
				}
			}
		}
		//
		// end change for MULTIPLE_DESTS_AND_SOURCES

		iter->nextElement();
   }

   delete iter;
}


//========================================================================
//  CopyDefsOfUse
//
// copy all definitions of the variable used in argOfS to argument(position)
// on operation target
//========================================================================
static
void
CopyDefsOfUse(BaseArgument* argOfS, BaseOperation* target, int position) {
	UDChainProperty* udProp = (UDChainProperty*)argOfS->getPtrToProperty(_UDCHAIN_);
        if ( udProp ) 
	   linkAllDefAndUse(udProp->getDefList(), target, position);
}


//========================================================================
//  updateUDOfArg
//========================================================================

static
void
updateUDOfArg(BaseOperation* target, BaseOperation* source, int position,
	      MyLinkedList<BaseOperation*>* moves) {
   
   BaseArgument* argOfT;

   // Note: Since this is update UD we are assured that the position (i.e. argument position) we are looking for
   // is in the sourceOperandsList.
   //
	  argOfT = target->ptrToOperand(position, _SRC_LIST_);


   if ( argOfT == NULL ) 
	return;
   // find this argOfT in the operand of move operations in moves,
   // find it in the reverse order
   if ( moves ) {
      MyLinkedListIterator<BaseOperation*>* iter = moves->elementsBackward();
      while ( iter->hasMoreElements() ) {
	    BaseArgument* moveOper=iter->currentElement()->ptrToOperand(_SOURCE_1_);
		if ( SameSymbol(argOfT, moveOper) ) {
			// copy ud chain  from move
			CopyDefsOfUse(moveOper, target, position);
			delete iter;
			return;
		}
		iter->nextElement();
	  }
      delete iter;
   }
   // copy ud from source
   if(source->ptrToOperand(position, _SRC_LIST_)!=NULL)
	CopyDefsOfUse(source->ptrToOperand(position, _SRC_LIST_), target, position);
}


//========================================================================
// updateUDDU
//========================================================================

void
BaseOperation::
updateUDDU(BaseOperation* old, MyLinkedList<BaseOperation*>* moves) {
   // destination
   BaseArgument* argOfS;
   if(old->hasOperand(_DEST_, _DEST_LIST_)){
	   argOfS = old->ptrToOperand(_DEST_, _DEST_LIST_);
	   DUChainProperty*   duProp = (DUChainProperty*)argOfS->getPtrToProperty(_DUCHAIN_);
	   if ( duProp )  {
			linkDefAndAllUse(this, duProp->getUseList());
		}
   }
/*
   // source 1
   updateUDOfArg(this, old, _SOURCE_1_, moves);
   // source_2
   updateUDOfArg(this, old, _SOURCE_2_, moves);
*/
		// Changed by SAH : Wed, 16th August, 2000
		//
		// MULTIPLE_DESTS_AND_SOURCES
		//
		ArgList & a = sourceOperandList();
		ArgListIter ai;
		int i = 0;
		for (ai = a.begin(); ai != a.end(); ai++)
		{
			if ((*ai) != NULL)
				updateUDOfArg(this, old, i, moves);
			i++;
		}
		//
		// end change for MULTIPLE_DESTS_AND_SOURCES
}


//PS Stuff

int BaseOperation::percMoveOP(Routine* routine)
{
 ASSERT(0);
 return 0;
}

int BaseOperation::percMigrate(Routine* routine)
{
 ASSERT(0);
 return 0;
}

int BaseOperation::percMoveCJ(Routine* routine)
{
 ASSERT(0);
 return 0;
}

// returns _YES_ if the operation defines that particular argument.
// Note: It checks if the argument values are equal.
// i.e. it does not return _YES_ only if the arg pointers are equal.
//
int defines(BaseOperation *oper, BaseArgument *arg)
{
	// iterate over the destination list.
	//
	ArgList & a = oper->destOperandList();
	ArgListIter i;

	for (i = a.begin(); i != a.end(); i++)
	{
		if ((*i) != NULL)
		{
			if (arg->isEq(*i) == _YES_)
				return _YES_;
		}
	}
	return _NO_;
}

// returns _YES_ if the operation uses that particular argument.
// Note: It checks if the argument values are equal.
// i.e. it does not return _YES_ only if the arg pointers are equal.
//
int uses(BaseOperation *oper, BaseArgument *arg)
{
	// iterate over the source list.
	//
	ArgList & a = oper->sourceOperandList();
	ArgListIter i;

	for (i = a.begin(); i != a.end(); i++)
	{
		if ((*i) != NULL)
		{
			if (arg->isEq(*i) == _YES_)
				return _YES_;
		}
	}
	return _NO_;
}
