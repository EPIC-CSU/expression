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
#include "ScheduleOpProperty.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 


extern int LastDependency;

void BaseOperation::renameOperands(MyLinkedList<BaseOperation*> &moves){
  BaseArgument *arg;
  MyLinkedListIterator<BaseOperation*>*iter;
  BaseOperation *move;
  iter=moves.elementsForward();
  while(iter->hasMoreElements()){
    if(iter->currentElement()->isMoveOp()){
      move=iter->currentElement();
      if((arg=ptrToOperand(_SOURCE_1_))!=NULL){
	if(*arg==*(move->ptrToOperand(_DEST_, _DEST_LIST_))){
	  addOperand(move->ptrToOperand(_SOURCE_1_), _SOURCE_1_);
	}
      }
      if((arg=ptrToOperand(_SOURCE_2_))!=NULL){
	if(*arg==*(move->ptrToOperand(_DEST_, _DEST_LIST_))){
	  addOperand(move->ptrToOperand(_SOURCE_1_), _SOURCE_2_);
	}
      }
    }
    iter->nextElement();
  }
  delete iter;
}

extern int hasScheduleOpProperty(BaseOperation *oper, int flag);
extern int getScheduleOpPropertyCode(BaseOperation *oper, int flg);

//computes to which unifiable ops can this operation be added, 
//and adds it
void BaseOperation::compute_unifiable_ops(void){
  BaseInstruction *instr;
  BaseHTGNode *node;
  MyLinkedList<BaseOperation *> moves;

  if(!this->isDataOp()) return;

  // Added by SAH on 11/31/01
  //
  // Check the Schedule Properties for any barriers or no code motion.
  //
  if (hasScheduleOpProperty(this, _NOCODEMOTION_) == _YES_)
  {
	  if (getScheduleOpPropertyCode(this, _NOCODEMOTION_) == _DEFAULT_ALL_)
		  return;
  }

  instr=this->getInstr();
  if(instr==NULL || !instr->isNormalInstr()) return;
  node=prevTB((NormalInstr*)instr,this);
  if(node!=NULL){
	  LastDependency=0;
	  //find the destination of this operation
	  node->find_unifop_destination(this,moves);
  }
}

