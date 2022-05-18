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
// $Id: ControlOp.cc,v 1.3 1998/01/22 23:46:01 pgrun Exp $
//
// File:  		ControlOp.h
// Created:		Wed Nov 20, 96
// Last modified: 	Wed Nov 20, 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: OPERATION

#include "stdafx.h"

#include "PercDefines.h"

#include "ControlOp.h"

#include "IRTraversal.h"
#include "Routine.h"

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

int ControlOp::percMoveCJ(Routine* routine)
{
 MyLinkedList<BaseOperation*> dummy;
 int depCount=0;
 
 //get instruction that op belongs to
 BaseInstruction* instr=routine->oMapper->getInstr(this);
 //get a list of predecessors
 InstrList& pred=routine->iWalker->getNext(instr);
 InstrList::iterator i;
 
 if (pred.size()==0) //this is the first instr nowhere else to go
 {
  return PERC_OK;
 }

 //for each predecessor
	for (i=pred.begin(); i != pred.end(); i++)
 {
  if (depends(this, *i, dummy)) depCount++;
 }
 
 if (depCount==pred.size()) //this op can't move
 {
  return PERC_DEPENDENCE;
 }
 
 //it can move up to at least one of the predecessors
 
 if (pred.size()==1) //simple case, straight line code
 {
  NormalInstr* nInstr=(NormalInstr*)*pred.begin();
 	DataOpSlot *availSlot=(DataOpSlot*)nInstr->getAvailSlot(this);
 	if(availSlot!=NULL)
  {
   //remove instr from old instr
   instr->extract(this);
   //insert in new one
 	 availSlot->setOperation(this);
   //update mapping
   routine->oMapper->changeOper(this, nInstr);
   return PERC_OK;
  }
  else
  {
   //we need to insert a new instr or enlarge the current one
   return PERC_UNKNOWN;
  }
 }
 else //more than one predecessor, more complex
 {
  if (depCount==0) //will percolate up on all preds so no need to duplicate current instr
  {
   //extract this from its current location
   instr->extract(this);
   //remove this op from op2instr mapping
   routine->oMapper->removeOper(this);
   
   //for each predecessor
   for (i=pred.begin(); i != pred.end(); i++)
   {
    NormalInstr* nInstr=(NormalInstr*)*i;
    DataOpSlot *availSlot=(DataOpSlot*)nInstr->getAvailSlot(this);
    if(availSlot!=NULL)
    {
     //duplicate this (also copying UDDU chains)
     BaseOperation* newOp=this->copyBody(true);

     //update UDDU
     updateUDDU(newOp, &dummy);

     //insert in new one
     availSlot->setOperation(newOp);

     //update op2instr mapping
     routine->oMapper->addOper(newOp, nInstr);
    }
    else
    {
     ASSERT(0);
     //we need to insert a new instr or enlarge the current one
     return PERC_UNKNOWN;
    }
   }

   //delete the initial copy of this
   delete this;
   return PERC_OK;
  }
  else //will only percolate up to some of the predecessors
  {
   //make a copy of the current instr
   BaseInstruction* newInstr=instr->copyBody();//make copy of instr where this resides
   //add it to the routine
   routine->appendInstruction(newInstr);
   //update UDDU for all the ops in newInstr
   newInstr->updateInstrUDDU(instr, &dummy);
   //add instr to mappings
   routine->oMapper->addInstr(newInstr);
   routine->iWalker->addNewInstr(newInstr);

   //extract this from its current location
   instr->extract(this);
   //remove this op from op2instr mapping
   routine->oMapper->removeOper(this);

   //now go thru all predecessors
   for (i=pred.begin(); i != pred.end(); i++)
   {
    if (depends(this, *i, dummy))//special case, link this instr to newInstr
    {
     //find the link that goes to instr and update it to point to newInstr
    	ArrayIterator<FlowOpSlot> *flowIter=((NormalInstr*)(*i))->getFlowOpSlotIter();
     while (flowIter->hasMoreElements())
     {
      if (flowIter->currentElement().hasOper() == _YES_)
      {
       FlowOp* fOp=(FlowOp*)flowIter->currentElement().getPtrToOperation();

       if (fOp->getTargetInstr()==instr)
       {
        fOp->setTargetInstr(newInstr);
        //also update mapping
        routine->iWalker->addPred(newInstr, *i);
       }
      }
      flowIter->nextElement();
     } 
     delete flowIter;
    }
    else //normal case
    {
     NormalInstr* nInstr=(NormalInstr*)*i;
     DataOpSlot *availSlot=(DataOpSlot*)nInstr->getAvailSlot(this);
     if(availSlot!=NULL)
     {
      //duplicate this (also copying UDDU chains)
      BaseOperation* newOp=this->copyBody(true);
      
      //update UDDU
      updateUDDU(newOp, &dummy);

      //insert in new one
      availSlot->setOperation(newOp);
      
      //update op2instr mapping
      routine->oMapper->addOper(newOp, nInstr);
     }
     else
     {
      ASSERT(0);
      //we need to insert a new instr or enlarge the current one
      return PERC_UNKNOWN;
     }
    }
   }

   //delete the initial copy of this
   delete this;
   return PERC_OK;
  }
 } 
}

int ControlOp::percMigrate(Routine* routine)
{
 return PERC_UNKNOWN;
}