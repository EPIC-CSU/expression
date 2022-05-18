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
// $Id: TbzBasicBlockNode.cc,v 1.23 1998/04/09 18:47:06 pgrun Exp $
//
// File:  		BasicBlockNode.cc
// Created:		Wed Jan 15, 97
// Last modified: 	Wed Jan 15, 97
// Author:		G. Grun
// Email:		pgrun@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: INCLUDE

#include "stdafx.h"

#include "Routine.h"
#include "BasicBlockNode.h"
#include "BBIterator.h"
#include "NormalInstr.h"
#include "PropertyIncludes.h"
#include "UDProperty.h"
#include "SimpleHTGNode.h"
#include "dependency.h"
#include "JumpOp.h"
#include "IfOppositesTable.h"

extern IfOppositesTable globalIfOppositesTable;
extern Routine *currRoutine;

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

int debugTbzBasicBlockNode = 0;

extern void delInstr(BaseInstruction* instr,Routine* routine,BasicBlockNode* bb=0);
extern Routine *currRoutine;

Routine *getCrtRoutine(void){
 return currRoutine;
}

void moveOneOp(DataOpSlot *op,BaseInstruction * src,BaseInstruction *dst){
 BaseInstruction *instr;
 MyLinkedList<BaseOperation *>moves;
 int DEP=0;
 DataOpSlot * availSlot;
 if(src!=dst){
  instr=getPrev(src,dst);
  while(1){
   if(depends(op->getPtrToOperation(),instr,moves)){
    DEP=1;
    break;
   }
   if(instr==dst){
    break;
   }
   instr=getPrev(instr,dst);
  }
  if(DEP) instr=instr->getFirstSuccOnly();
  while(instr!=src && (instr->isPhiInstr() ||
   (availSlot=(DataOpSlot*)(((NormalInstr*)instr)->getAvailSlot(op->getPtrToOperation())))
   ==NULL)) 
   instr=instr->getFirstSuccOnly();
  if(instr!=src && instr->isNormalInstr() && availSlot!=NULL)
   moveOperation((NormalInstr*)src,op,(NormalInstr*)instr);
 }
}


//old basic block compaction
void BasicBlockNode::compactBBHeur(void){
 BaseInstruction * crt, *next, *last;
 //BaseOperation * op;
 ArrayIterator<DataOpSlot> *iter;
 DataOpSlot * slot;
 crt=getFirstInstr();
 last=getLastInstr();
 if(crt!=last){
  next=crt->getFirstSuccOnly();
  do{
   crt=next;
   next=crt->getFirstSuccOnly();
   if(crt->isNormalInstr()){
    iter=((NormalInstr*)crt)->getDataOpSlotIter();
    while(iter->hasMoreElements()){
     slot=&(iter->currentElement());
     iter->nextElement();
     if(slot->hasOper()){
      moveOneOp(slot,crt,getFirstInstr());
     }
    }
    if(((NormalInstr*)crt)->countDataOps()==0 && 
     ((NormalInstr*)crt)->countControlOps()==0)
     delInstr(crt,getCrtRoutine(),this);
   }
  }while(crt!=last);
 }
}

//old percolation of ifs
void moveOneCj(ControlOpSlot *op,BaseInstruction * src,BaseInstruction *dst){
 BaseInstruction *instr;
 MyLinkedList<BaseOperation *>moves;
 int DEP=0;
 DataOpSlot * availSlot;
 if(src!=dst){
  instr=getPrev(src,dst);
  while(1){
   if(depends(op->getPtrToOperation(),instr,moves)){
    DEP=1;
    break;
   }
   if(instr==dst){
    break;
   }
   instr=getPrev(instr,dst);
  }
  if(DEP) instr=instr->getFirstSuccOnly();
  while(instr!=src && (instr->isPhiInstr() ||
	  (availSlot=(DataOpSlot*)(((NormalInstr*)instr)->getAvailSlot(op->getPtrToOperation())))
	  ==NULL)) 
   instr=instr->getFirstSuccOnly();
  if(instr!=src && instr->isNormalInstr() && availSlot!=NULL){
   moveConditional((NormalInstr*)src,op,(NormalInstr*)instr);
  }
 }
}


//old percolation of ifs
void BasicBlockNode::percolateIfBBHeur(void){
 BaseInstruction *instr;
 ArrayIterator<ControlOpSlot> *iter;
 instr=getLastInstr();
 if(instr->isNormalInstr()){
  iter=((NormalInstr*)instr)->getControlOpSlotIter();
  while( iter->hasMoreElements()){
   if((iter->currentElement()).hasOper()){
    moveOneCj(&(iter->currentElement()),instr,getFirstInstr());
   }
   iter->nextElement();
  }
  delete iter;
  /*if(((NormalInstr*)instr)->countDataOps()==0 && 
  ((NormalInstr*)instr)->countControlOps()==0)
  delInstr(instr,getCrtRoutine(),this);*/
 }
}


////////////////////////////// Begin Trailblazing related ////////////////////////////////////////////

//compute the unifiable ops for this basic block
void BasicBlockNode::compute_unifiable_ops(void){
 BaseInstruction * crt, *last; //, *next;
 //BaseOperation * op;
 crt=getFirstInstr();
 last=getLastInstr();

 // Added by SAH on 3rd Aug, 99
 //
 if ((crt == NULL) || (last == NULL))
 {
	 // This may be a dummy basic block.
	 return;
 }
 //
 // end added code.

 while(1){
  if(crt->isNormalInstr()){

	  if (debugTbzBasicBlockNode)
		  crt->print(3);

   ((NormalInstr*)crt)->compute_unifiable_ops();
  }
  if(crt==last) break;
  crt=crt->getFirstSuccOnly();
 }  
}

//schedule this basic block
void BasicBlockNode::schedule(void){
	NormalInstr *instr;
	BBForwIterator iter(this);
	//for all the instruction inside this basic block
	while(iter.hasMoreElements()){
		//if normal instruction (historic: not phi instruction)
		if(iter.currentElement()->isNormalInstr()){
			instr=(NormalInstr*)(iter.currentElement());
			//move the iterator to the next element, in case we delete this instruction 
			iter.nextElement();
			//schedule this instruction
			instr->schedule();
			//delete empty instructions
			/*if(instr->countControlOps()==0 && getFirstInstr()!=getLastInstr())
				delInstr(instr,getCrtRoutine(),this);*/
		}
		else iter.nextElement();
	}
}

//returns 1 if op depends on any instruction inside this basic block
int BasicBlockNode::depends(BaseOperation *op,MyLinkedList<BaseOperation *> &moves){
	BBForwIterator iter(this);
	//for each instruction in this basic block
	while(iter.hasMoreElements()){
		if(iter.currentElement()->isNormalInstr()){
			if(((NormalInstr*)(iter.currentElement()))->depends(op,moves)) 
				return 1;
		}
		iter.nextElement();
	}
	return 0;
}

// Added by SAH on 01/31/01
//
// this function should be used in place of depends.
//
int BasicBlockNode::cannotMoveBeyond(BaseOperation *op, MyLinkedList<BaseOperation *> &moves){
	BBForwIterator iter(this);
	//for each instruction in this basic block
	while(iter.hasMoreElements()){
		if(iter.currentElement()->isNormalInstr()){
			if(((NormalInstr*)(iter.currentElement()))->cannotMoveBeyond(op,moves)) 
				return 1;
		}
		iter.nextElement();
	}
	return 0;
}

//returns 1 if it contains oper in unifiable ops
int BasicBlockNode::contains_unifiable_op(BaseOperation *oper){
 BaseInstruction *crt,*last;
 crt=getFirstInstr();
 last=getLastInstr();

 // Added by SAH on 3rd Aug, 99
 //
 if ((crt == NULL) || (last == NULL))
 {
	 // This may be a dummy basic block.
	 return 0;
 }
 //
 // end added code.

 while(1){
  if(crt->isNormalInstr()){
   if(((NormalInstr*)crt)->contains_unifiable_op(oper)) return 1;
  }
  if(crt==last) break;
  crt=crt->getFirstSuccOnly();
 }
 return 0;
}



