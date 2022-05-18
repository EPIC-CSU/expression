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

#include "GlobalIncludes.h"

#include "BaseArgument.h"
#include "BaseHTGNode.h"
#include "GlobalIncludes.h"
#include "BasicBlockNode.h"
#include "SimpleHTGNode.h"
#include "NormalInstr.h"
#include "BaseOperation.h"
#include "ComputeOp.h"
#include "BBIterator.h"
#include "RegisterFileList.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

//The maximum number of live variables allowed during trailblazing. 
const int MAXLIVE=32;

void SimpleHTGNode::trailblaze(void){
  //nothing
}

void SimpleHTGNode::compute_unifiable_ops(void){
  _bb->compute_unifiable_ops();
}

int SimpleHTGNode::find_unifop_destination(BaseOperation *oper,MyLinkedList<BaseOperation *> moves){
  BaseInstruction *last;
  BaseHTGNode *node;
  int r,v;
  MyLinkedList<BaseOperation *> tmpMoves(moves);

  removeUnifiableOps(oper);

  if(!this->depends(oper,tmpMoves)){
    if(this->hasLive(oper) || this->getBasicBlock()->countLive()>MAXLIVE) return 0;
    node=prevTB(this,oper);
    if(node!=NULL){
      if(node->find_unifop_destination(oper,tmpMoves))
		return 1;
    }
  }
  if((last=_bb->getLastInstr())!=NULL) {
    v=setNotGoUp(1);
    r=last->find_unifop_destination(oper,moves);
    setNotGoUp(v);
    return r;
  }
  else return 0;
}

//schedule this simple HTG node
void SimpleHTGNode::schedule(void){
  _bb->schedule();
}

//trailblaze inside this simple HTG node
int SimpleHTGNode::migrate(BaseOperation *oper,vector<BaseHTGNode *> *path){
  BaseHTGNode *node;
  path->push_back(this);
  if(!this->contains_unifiable_op(oper)){
    node=prevTB(this,oper,path);
    if(node!=NULL){
      return node->migrate(oper,path);
    }
  }
  else{
    return _bb->getLastInstr()->migrate(oper,path);
  }
  // This piece of code should never be executed.
  ASSERT_TRACE(0,"Forced EXIT!");
  return -1;
}

// Changed by SAH on 01/31/01
//
// changed _bb->depends(op,moves); to _bb->cannotMoveByond(op,moves);
//
int SimpleHTGNode::depends(BaseOperation *op,MyLinkedList<BaseOperation *> &moves){
  return _bb->cannotMoveBeyond(op,moves);
}

int SimpleHTGNode::contains_unifiable_op(BaseOperation *oper){
  return _bb->contains_unifiable_op(oper);
}

void SimpleHTGNode::appendInstr(NormalInstr *instr,int bbDir){
  _bb->appendInstr(instr,bbDir);
}

void SimpleHTGNode::removeUnifiableOps(BaseOperation *oper){
  BaseInstruction *instr,*last;
  instr=_bb->getFirstInstr();
  last=_bb->getLastInstr();

 // Added by SAH on 3rd Aug, 99
 //
 if ((instr == NULL) || (last == NULL))
 {
	 // This may be a dummy basic block.
	 return;
 }
 //
 // end added code.

  while(1){
    instr->removeUnifiableOps(oper);
    if(instr==last) break;
    instr=instr->getFirstSuccOnly();
    if(instr==NULL) break;
  }  
}
  

//returns 1 if the destination of oper is live in this basic block
int SimpleHTGNode::hasLive(BaseOperation *oper){
  BaseArgument *arg;
  if(oper->isComputeOp()){
    arg=((ComputeOp*)oper)->ptrToOperand(_DEST_, _DEST_LIST_);
    if(arg!=NULL && _bb->isLive(arg)) return 1;
  }
  return 0;
}

