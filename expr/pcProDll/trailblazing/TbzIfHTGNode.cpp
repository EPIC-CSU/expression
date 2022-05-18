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

#include "IfHTGNode.h"
#include "BaseInstruction.h"
#include "NormalInstr.h"
#include "SimpleHTGNode.h"
#include "ComputeOp.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

extern int tbz_allow_splitting;

//perform trailblazing for this If HTG node
void IfHTGNode::trailblaze(void){
 MyLinkedListIterator<BaseHTGNode *> *iter;
 iter=_trueBranch.elementsForward();
 while(iter->hasMoreElements()){
  iter->currentElement()->trailblaze();
  iter->nextElement();
 }
 iter=_falseBranch.elementsForward();
 while(iter->hasMoreElements()){
  iter->currentElement()->trailblaze();
  iter->nextElement();
 }
 delete iter;
}

//computes the unifiable ops for all operations inside this if
void IfHTGNode::compute_unifiable_ops(void){
 MyLinkedListIterator<BaseHTGNode *> *iter;
 iter=_trueBranch.elementsForward();
 while(iter->hasMoreElements()){
  iter->currentElement()->compute_unifiable_ops();
  iter->nextElement();
 }
 delete iter;
 iter=_falseBranch.elementsForward();
 while(iter->hasMoreElements()){
  iter->currentElement()->compute_unifiable_ops();
  iter->nextElement();
 }
 delete iter;
}

//finds the highest destination for oper inside or before this conditional,
//and adds it to the unifiable ops of that destination
int IfHTGNode::find_unifop_destination(BaseOperation *oper,MyLinkedList<BaseOperation *> moves){
 BaseHTGNode *node,*n1,*n2; 
 int r1=0,r2=0;
 MyLinkedList<BaseOperation *> tmpMoves(moves);
 BaseInstruction *instr;
 instr=oper->getInstr();
 
 removeUnifiableOps(oper);
 
 //if oper depends on anything inside this conditional
 if(!this->depends(oper,tmpMoves)){
  //if oper is live at the beginnning of this conditional
  if(this->hasLive(oper)) return 0;
  node=prevTB(this,oper);
  if(node!=NULL){
   if(node->find_unifop_destination(oper,tmpMoves))
    return 1;
  }
 }
 int v=setNotGoUp(1);
 if((n1=createOrGetLastTrueNode(_NO_))!=NULL) r1=n1->find_unifop_destination(oper,moves);
 if((n2=createOrGetLastFalseNode(_NO_))!=NULL) r2=n2->find_unifop_destination(oper,moves);
 setNotGoUp(v);
 return r1 || r2;
}


void IfHTGNode::schedule(void){
 MyLinkedListIterator<BaseHTGNode *> *iter;
 iter=_trueBranch.elementsForward();
 while(iter->hasMoreElements()){
  iter->currentElement()->schedule();
  iter->nextElement();
 }
 delete iter;
 iter=_falseBranch.elementsForward();
 while(iter->hasMoreElements()){
  iter->currentElement()->schedule();
  iter->nextElement();
 }
 delete iter;
}

//migrate this operation oper into this conditional
//oper: input, the operation to migrate into "this"
//path: input, output, the path that has been taken so far by oper
int IfHTGNode::migrate(BaseOperation *oper,vector<BaseHTGNode *> *path){
 BaseHTGNode *n1,*n2;
 BaseHTGNode *node;
 BaseInstruction *instr;
 path->push_back(this);
 instr=oper->getInstr();
 int r1=0,r2=0,can1,can2;
 //if oper is not in the unifiable ops of any instruction inside this conditional
 if(!this->contains_unifiable_op(oper)){
  //find the HTG node previous to "this"
  node=prevTB(this,oper,path);
  if(node!=NULL){
   //try to migrate into the previous HTG node
   if(node->migrate(oper,path)) return 1;
  }
 }
 //if coultn't migrate higher than this conditional
 //try to migrate into this conditional

 //if want to allow movement of operations into this conditional
 if(tbz_allow_splitting){
	 //find if oper is in the unifialbe ops in the true or false branch
	 can1=::contains_unifiable_op(&_trueBranch,oper);
	 can2=::contains_unifiable_op(&_falseBranch,oper);
	 //find the last true/false nodes
	 n1=createOrGetLastTrueNode(_NO_);
	 n2=createOrGetLastFalseNode(_NO_);
	 //if oper is not in the unifiable ops of at least one of the branches (and that branch is not empty)
	 if((!can1 && n1!=NULL) || (!can2 && n2!=NULL)){
		 //try to re-add oper to the unifiable ops of the branches of this conditional
		 oper->compute_unifiable_ops();
	 }
	 can1=::contains_unifiable_op(&_trueBranch,oper);
	 can2=::contains_unifiable_op(&_falseBranch,oper);
	 //if still is not present in the unifiable ops of both branches
	 if((!can1 && n1!=NULL) || (!can2 && n2!=NULL)) 
		 return 0;
	 //try to migrate the operation into both the branches of this conditional
	 int v=setNotGoUp(1);
	 if(can1 && n1!=NULL){
		 r1=n1->migrate(oper,path);
	 }
	 if(can2 && n2!=NULL){ 
		 r2=n2->migrate(oper,path);    
	 }
	 setNotGoUp(v);
	 //if couldn't migrate in any of the branches, try to split the next instruction after the conditional
	 if(!r1)
		 r1=this->splitTrue(oper);
	 if(!r2)
		 r2=this->splitFalse(oper);
	 
	 return r1 || r2;
 }
 return 0;
}

//returns the next instruction for node
NormalInstr *nextInstr(BaseHTGNode *node){
 BaseHTGNode *next;
 BaseInstruction *instr,*last;
 next=node->getNext();
 
 //
 // This code was added by Ashok on Mon, Jul 13th.
 //
 if (next == NULL) return NULL;
 
 if(next->isSimpleNode()){
  instr=((SimpleHTGNode*)next)->getBasicBlock()->getFirstInstr();
  last=((SimpleHTGNode*)next)->getBasicBlock()->getLastInstr();
  while(instr!=NULL && instr!=last && !instr->isNormalInstr()) 
   instr=instr->getFirstSuccOnly();
  if(instr->isNormalInstr())
   return (NormalInstr*)instr;
 }
 return NULL;
}

//split an instruction, and add it to the true branch
int IfHTGNode::splitTrue(BaseOperation *oper){
 //if cannot insert oper into the instruction after the if,
 //create new instruction and add oper to it. 
 NormalInstr *next;
 NormalInstr *instr1, *instr2,*instr;
 
 if((next=nextInstr(this))!=NULL && next->getAvailSlot(oper)!=NULL){
  instr1=new NormalInstr(*next);
  instr1->setID(-2);
  instr2=new NormalInstr(*next);
  instr2->setID(-3);
  if(oper->isComputeOp())
   instr1->addDataOpToList((ComputeOp*)(oper->copy()));
  this->appendInstrToTrue(instr1);
  this->appendInstrToFalse(instr2);
  //next->disconnect(); error
 }
 else{
  instr=new NormalInstr();
  instr->initAddDataOp((DataOp*)oper->copy());
  this->appendInstrToTrue(instr);
 }
 return 1;
}

//split an instruction, and add it to the false branch
int IfHTGNode::splitFalse(BaseOperation *oper){
 //if cannot insert oper into the instruction after the if,
 //create new instruction and add oper to it. 
 NormalInstr *next;
 NormalInstr *instr1, *instr2,*instr;

 if((next=nextInstr(this))!=NULL && next->getAvailSlot(oper)!=NULL){
  instr1=new NormalInstr(*next);
  instr2=new NormalInstr(*next);
  instr1->setID(-4);
  instr2->setID(-5);
  if(oper->isComputeOp())
   instr2->addDataOpToList((ComputeOp*)(oper->copy()));
  this->appendInstrToTrue(instr1);
  this->appendInstrToFalse(instr2);
  //next->disconnect();
 }
 else{
  instr=new NormalInstr();
  instr->initAddDataOp((DataOp*)oper->copy());
  this->appendInstrToFalse(instr);
 }
 return 1;
}

//returns the previous instruction of node
NormalInstr *prevInstr(BaseHTGNode *node){
 BaseHTGNode *prev;
 BaseInstruction *instr; //,*last;
 prev=node->getPrev();
 if(prev->isSimpleNode()){
  instr=((SimpleHTGNode*)prev)->getBasicBlock()->getLastInstr();
  if(instr!=NULL && instr->isNormalInstr())
   return (NormalInstr*)instr;
 }
 return NULL;
}

//returns 1 if oper is in the unifiable ops of any instruction inside this conditional
int IfHTGNode::contains_unifiable_op(BaseOperation *oper){
 MyLinkedListIterator<BaseHTGNode *> *titer=createTrueBranchIterForw();
 MyLinkedListIterator<BaseHTGNode *> *fiter=createFalseBranchIterForw();
 while(titer->hasMoreElements()){
  if(titer->currentElement()->contains_unifiable_op(oper)) return 1;
  titer->nextElement();
 }
 while(fiter->hasMoreElements()){
  if(fiter->currentElement()->contains_unifiable_op(oper)) return 1;
  fiter->nextElement();
 }
 return 0;
}


//returns 1 if op depends on anything inside this conditional 
int IfHTGNode::depends(BaseOperation *op,MyLinkedList<BaseOperation *> &moves){
 MyLinkedListIterator<BaseHTGNode *> *titer=createTrueBranchIterForw();
 MyLinkedListIterator<BaseHTGNode *> *fiter=createFalseBranchIterForw();
 while(titer->hasMoreElements()){
  if(titer->currentElement()->depends(op,moves)) return 1;
  titer->nextElement();
 }
 while(fiter->hasMoreElements()){
  if(fiter->currentElement()->depends(op,moves)) return 1;
  fiter->nextElement();
 }
 return 0;
}

//removes oper from the unifiable ops of this conditional
void IfHTGNode::removeUnifiableOps(BaseOperation *oper){
 MyLinkedListIterator<BaseHTGNode *> *iter;
 iter=_trueBranch.elementsForward();
 while(iter->hasMoreElements()){
  iter->currentElement()->removeUnifiableOps(oper);
  iter->nextElement();
 }
 iter=_falseBranch.elementsForward();
 while(iter->hasMoreElements()){
  iter->currentElement()->removeUnifiableOps(oper);
  iter->nextElement();
 }
 delete iter;
}

//appends an instruction to the true branch
void IfHTGNode::appendInstrToTrue(NormalInstr *instr){
 BaseHTGNode *tnode;
 SimpleHTGNode *snode;
 
 tnode=createOrGetLastTrueNode(_NO_);
 if(tnode==NULL || !tnode->isSimpleNode()){
  BasicBlockNode *bb=new BasicBlockNode();
  snode=new SimpleHTGNode(bb);
  if(tnode!=NULL) tnode->setNext(snode);
  appendTrueNode(snode);
  instr->appendInstr(prevInstr(this),_TRUE_BRANCH_,1);
  snode->appendInstr(instr,_FIRST_SUCC_);
 }
 else{
  snode=(SimpleHTGNode*)tnode;
  instr->appendInstr(snode->getBasicBlock()->getLastInstr(),-1,0);
  snode->appendInstr(instr,_FIRST_SUCC_);
 }
}

//appends an instruction to the false branch
void IfHTGNode::appendInstrToFalse(NormalInstr *instr){
 BaseHTGNode *fnode;
 SimpleHTGNode *snode;
 
 fnode=createOrGetLastFalseNode(_NO_);
 if(fnode==NULL || !fnode->isSimpleNode()){
  BasicBlockNode *bb=new BasicBlockNode();
  snode=new SimpleHTGNode(bb);
  if(fnode!=NULL) fnode->setNext(snode);
  appendFalseNode(snode);
  instr->appendInstr(prevInstr(this),_FALSE_BRANCH_,1);
  snode->appendInstr(instr,_FIRST_SUCC_);
 }
 else{
  snode=(SimpleHTGNode*)fnode;
  instr->appendInstr(snode->getBasicBlock()->getLastInstr(),-1,0);
  snode->appendInstr(instr,_FIRST_SUCC_);
 }
}


//returns 1 if this if the destination of oper is live in any of the branches of this conditional
int IfHTGNode::hasLive(BaseOperation *oper){
 MyLinkedListIterator<BaseHTGNode *> *iter;
 iter=_trueBranch.elementsForward();
 if(!iter->hasMoreElements()){
	 delete iter;
	 return 1; /*fix for empty branch live information*/
 }
 while(iter->hasMoreElements()){
	 if(iter->currentElement()->hasLive(oper)){
		delete iter;
		return 1;
	 }
  iter->nextElement();
 }
 delete iter;
 iter=_falseBranch.elementsForward();
 if(!iter->hasMoreElements()){
	 delete iter;
	 return 1; /*fix for empty branch live information*/
 }
 while(iter->hasMoreElements()){
	 if(iter->currentElement()->hasLive(oper)){
		 delete iter;
		 return 1;
	 }
  iter->nextElement();
 }
 delete iter;
 return 0;
}

//returns 1 if this if the destination of oper is live in the True Branch of this conditional
int IfHTGNode::hasLiveInTrueBranch(BaseOperation *oper){
 MyLinkedListIterator<BaseHTGNode *> *iter;
 iter=_trueBranch.elementsForward();
 if(!iter->hasMoreElements()){
	 delete iter;
	 return 1; /*fix for empty branch live information*/
 }
 while(iter->hasMoreElements()){
	 if(iter->currentElement()->hasLive(oper)){
		delete iter;
		return 1;
	 }
  iter->nextElement();
 }
 delete iter;
 return 0;
}

//returns 1 if this if the destination of oper is live in the False Branch of this conditional
int IfHTGNode::hasLiveInFalseBranch(BaseOperation *oper){
 MyLinkedListIterator<BaseHTGNode *> *iter;
 iter=_falseBranch.elementsForward();
 if(!iter->hasMoreElements()){
	 delete iter;
	 return 1; /*fix for empty branch live information*/
 }
 while(iter->hasMoreElements()){
	 if(iter->currentElement()->hasLive(oper)){
		delete iter;
		return 1;
	 }
  iter->nextElement();
 }
 delete iter;
 return 0;
}