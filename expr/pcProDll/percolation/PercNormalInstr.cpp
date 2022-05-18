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

#include "PercDefines.h"

#include "IRTraversal.h"
#include "Routine.h"
#include "Instruction.h"

#include "FlagsDefs.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

extern int depends(BaseOperation *oper,BaseInstruction *instr,MyLinkedList<BaseOperation*> &moves);

//a wrapper for percUnify()
int NormalInstr::percUnifyWrap(Routine* routine)
{
 OpList opList;
 InstrList& pred=routine->iWalker->getNext(this);
 
 NormalInstr* nextInstr;
 NormalInstr* instr=(NormalInstr*)this->getFirstSucc();//*pred.begin();
 
 //find the ops that can be unified (i.e. same op in all preds)
 ArrayIterator<DataOpSlot> *dIter=instr->getDataOpList()->elements();
 while (dIter->hasMoreElements())
 {
  if ((dIter->currentElement()).hasOper() == _YES_)
  {
   bool canUnify=true;
   BaseOperation* oper=(dIter->currentElement()).getPtrToOperation();
   
   //now look at all the siblings to see if they have the same op
			nextInstr = (NormalInstr*)this->getFirstSucc();
   opList.clear();
			while (nextInstr != NULL)
			{
    BaseOperation* unifOp=nextInstr->hasSameOp(oper);
    if (unifOp==NULL)
    {
     canUnify=false;
     break;
    }				
    opList.push_back(unifOp);    
				nextInstr = (NormalInstr*)this->getNextSucc();
			}
			this->deleteSuccIter();
   
   if (canUnify)
   {
    //we're only unifying one set of ops at a time
    //the next set of unifiable ops will be dealt with upon invoking percUnifyWrap again
    return percUnify(&opList,routine); 
   }
  }
  dIter->nextElement();
 }
 delete dIter;
 
 return PERC_NO_UNIFY;
}

int NormalInstr::percUnify(OpList* opList, Routine* routine) //op list is list of ops that can be unified
{
 OpList::iterator i;

 for (i=opList->begin();i!=opList->end();i++)
 {
  BaseOperation* oper=*i;
  //get instruction that op belongs to
  BaseInstruction* instr=routine->oMapper->getInstr(oper);
  //get a list of predecessors
  InstrList& pred=routine->iWalker->getNext(instr);

  if (pred.size()!=1) //the op's instr has more than one predecessor
  {
   //NOSFIX still to do... 
   //make a copy of the instruction
   //link the copy into CFG   
  }

  //extract the op
  instr->extract(oper);
  if (oper!=*opList->begin()) //if not the first one in the list
  {
   //remove this op from op2instr mapping
   routine->oMapper->removeOper(oper);
   //delete it too
   delete oper;
  }
 }

 //see if this has an empty slot
 if ((*opList->begin())->isDataOp())
 {
  DataOpSlot *availSlot=(DataOpSlot*)this->getAvailSlot(*opList->begin());
  if (availSlot==NULL)
  {
   return -1;
  }
  //place the first op in opList here
	 availSlot->setOperation(*opList->begin());
  //update the op2instr mapping
  routine->oMapper->changeOper(*opList->begin(), this);
  return 1;
 }
 else if ((*opList->begin())->isControlOp())
 {
  ControlOpSlot *availSlot=(ControlOpSlot*)this->getAvailSlot(*opList->begin());
  if (availSlot==NULL)
  {
   return -1;
  }
  return 0;
 }
 else //can't unify this
 {
  ASSERT(0);
  return -9999;
 } 
}

void NormalInstr::updateInstrUDDU(BaseInstruction* old, MyLinkedList<BaseOperation*>* moves)
{
	// calls updateUDDU() for all ops in instr
 //it assumes the same order of ops in this and old
 
 ArrayIterator<DataOpSlot> *dIter = getDataOpSlotIter();
 ArrayIterator<DataOpSlot> *dIterOld = ((NormalInstr*)old)->getDataOpSlotIter();
 
 while (dIter->hasMoreElements())
 {
  ASSERT(dIterOld->hasMoreElements());
  if ((dIter->currentElement()).hasOper())
  {
   ASSERT((dIterOld->currentElement()).hasOper());
   (dIter->currentElement()).getPtrToOperation()->updateUDDU((dIterOld->currentElement()).getPtrToOperation(), moves);
  }  
  dIter->nextElement();
  dIterOld->nextElement();
 }
 delete dIter;
 delete dIterOld;
 
 ArrayIterator<ControlOpSlot> *cIter = getControlOpSlotIter();
 ArrayIterator<ControlOpSlot> *cIterOld = ((NormalInstr*)old)->getControlOpSlotIter();
 
 while (cIter->hasMoreElements())
 {
  ASSERT(cIterOld->hasMoreElements());
  if ((cIter->currentElement()).hasOper())
  {
   ASSERT((cIterOld->currentElement()).hasOper());
   (cIter->currentElement()).getPtrToOperation()->updateUDDU((cIterOld->currentElement()).getPtrToOperation(), moves);
  }  
  cIter->nextElement();
  cIterOld->nextElement();
 }
 delete cIter;
 delete cIterOld;
 
 ArrayIterator<FlowOpSlot> *fIter = getFlowOpSlotIter();
 ArrayIterator<FlowOpSlot> *fIterOld = ((NormalInstr*)old)->getFlowOpSlotIter();
 
 while (fIter->hasMoreElements())
 {
  ASSERT(fIterOld->hasMoreElements());
  if ((fIter->currentElement()).hasOper())
  {
   ASSERT((fIterOld->currentElement()).hasOper());
   (fIter->currentElement()).getPtrToOperation()->updateUDDU((fIterOld->currentElement()).getPtrToOperation(), moves);
  }
  fIterOld->nextElement();
  fIter->nextElement();
 }
 delete fIter;
 delete fIterOld;
}

bool addOpHW(NormalInstr* instr, BaseOperation *op) //adds op only if a slot that supports the op is available
{
	Vector<int> indexes;
 
	simpleVLIWInstrTemplate->getSlotListForOper(op, indexes);
 
	if (indexes.size()==0) // Error condition. This opcode does not have a valid slot.
 {		
		cout << "Panic: This opcode is not supported by the architecture" << endl;
		assert(0);
	}

 if(op->isDataOp())
 {
  for (int i = 0; i < indexes.size(); i++)
  {
		 if(indexes[i]<0) abort();
		 if(!instr->getDataOpList()->elementAt(indexes[i]).hasOper())
   {
    instr->getDataOpList()->elementAt(indexes[i]).addOperation(op,true);
    return true;
   }
  }
 }
 else if(op->isFlowOp())
 {
  for (int i = 0; i < indexes.size(); i++)
  {
   if(!instr->getFlowOpList()->elementAt(indexes[i]).hasOper())
   {
    instr->getFlowOpList()->elementAt(indexes[i]).addOperation(op,true);
    return true;
   }
  }
 }
 else if(op->isControlOp())
 {
  for (int i = 0; i < indexes.size(); i++)
  {
   if(!instr->getControlOpList()->elementAt(indexes[i]).hasOper())
   {
    instr->getControlOpList()->elementAt(indexes[i]).addOperation(op,true);
    return true;
   }
  }
 }

 return false;
}


int NormalInstr::percMigrate(Routine* routine, BaseOperation* op)
{
 MyLinkedList<BaseOperation*> dummy;

 if (flags.getFlag(PERC_FLAG)==false) return PERC_STOP; //not marked to be percolated

 //the assumption is that the op can be migrated in 'this' 
 //but before we do that we'll try to migrate it into the predecessor(s)
 //if that fails, we'll just migrate it here

 int depCount=0;
 
 //get a list of predecessors
 InstrList& pred=routine->iWalker->getNext(this);
 InstrList::iterator i;
 
// ASSERT(pred.size()<=1); //one predecessor for now

 //for each predecessor
	for (i=pred.begin(); i != pred.end(); i++)
 {
  if (::depends(op, *i, dummy)) depCount++;
 }
 
 if ((depCount==0) && pred.size()>0) //if op !depends on any of the ancestors and not the first instr
 {
  int res=(*pred.begin())->percMigrate(routine, op);
  if (res!=PERC_OK) //placement in one of the pred failed
  {
   //ASSERT(res!=PERC_UNKNOWN);
   if (::depends(op, this, dummy)) //can't move it here if it depends on this instr
   {
    return PERC_DEPENDENCE;
   }
       
   //should not try to move it in the same instr that it belongs to
   //this can happen went backtracking on the chain of instrs tried for migrate
   if (routine->oMapper->getInstr(op)==this) 
   {
    return PERC_SAME_INSTR;
   }

   //try here now
   //if (addOperation(op,_INVALID_,true)!=_INVALID_)
   if (addOpHW(this, op)==true)
   {
#ifndef __UNIX__
    _CrtDbgReport(_CRT_WARN, __FILE__, __LINE__, "!", "percMigrate(): moved op 0x%08X to instr 0x%08X\n", (long)op, (long)this);
#endif
    //remove op from old instr
    routine->oMapper->getInstr(op)->extract(op, true);
    //update the op2instr mapping
    routine->oMapper->changeOper(op, this);
    return PERC_OK;
   }
   else
   {
    return PERC_NO_SLOT;
   }
  }

  return PERC_OK;
 }
 else //place it here
 {
  if (routine->oMapper->getInstr(op)==this) 
  {
   return PERC_SAME_INSTR;
  }

  BaseInstruction* oldInstr=routine->oMapper->getInstr(op);
  //if (addOperation(op,_INVALID_,true)!=_INVALID_)
  if (addOpHW(this, op)==true)
  {
#ifdef WIN32
   _CrtDbgReport(_CRT_WARN, __FILE__, __LINE__, "!", "percMigrate(): moved op 0x%08X to instr 0x%08X\n", (long)op, (long)this);
#endif
   //remove op from old instr
   oldInstr->extract(op, true);
   //update the op2instr mapping
   routine->oMapper->changeOper(op, this);
   return PERC_OK;
  }
  else
  {
   return PERC_NO_SLOT;
  }
 }

 //return PERC_UNKNOWN;
}
