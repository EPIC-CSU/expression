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
#include "LoopHTGNode.h"
#include "SimpleHTGNode.h"
#include "IfHTGNode.h"
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


//set to 1 for trailblazing to move operations speculatively out of conditionals 
int tbz_allow_speculative=0;
//set to 1 for trailblazing to move operations into conditionals, possibly by splitting
int tbz_allow_splitting=0;

//the current routine
extern Routine *currRoutine;


BaseHTGNode *getUp(MyLinkedList<BaseHTGNode *>&list,BaseHTGNode *parent,
                   BaseHTGNode *node);

BaseHTGNode *getPrev(BaseHTGNode *node,MyLinkedListIterator<BaseHTGNode *>*iter){
 while(iter->hasMoreElements()){
  if(iter->currentElement()==node){
   iter->nextElement();
   if(iter->hasMoreElements()) return iter->currentElement();
   else return NULL;
  }
  iter->nextElement();
 }
 delete iter;
 return NULL;
}

//get the previous HTG node
BaseHTGNode *BaseHTGNode::getPrev(){
 BaseHTGNode *up,*tmp;
 up=this->getUp();
 if(up==NULL){
  MyLinkedListIterator<BaseHTGNode *> *iter;
  iter=currRoutine->getHTG()->elementsBackward();
  tmp=::getPrev(this,iter);
  return tmp;
 }
 if(up->isLoopNode()){
  tmp=::getPrev(this,((LoopHTGNode*)up)->createLoopIterBack());
  return tmp;
 }
 if(up->isIfNode()){
  tmp=::getPrev(this,((IfHTGNode*)up)->createTrueBranchIterBack());
  if(tmp!=NULL) return tmp;
  tmp=::getPrev(this,((IfHTGNode*)up)->createFalseBranchIterBack());
  return tmp;    
 }
 if(up->isSimpleNode() && this->isNormalInstr()){
  tmp=::getPrev((NormalInstr*)this,((SimpleHTGNode*)up)->getBasicBlock()->getFirstInstr());
  return tmp;
 }
 return NULL;
}

BaseHTGNode *getUp(MyLinkedList<BaseHTGNode *>*list,BaseHTGNode *parent,
                   BaseHTGNode *node){
 MyLinkedListIterator<BaseHTGNode *> *iter;
 MyLinkedListIterator<BaseHTGNode *> *firstIter;
 BaseHTGNode *tmp;
 iter=list->elementsForward();
 firstIter = iter;
 while(iter->hasMoreElements()){
  if(iter->currentElement()==node)
	{
	  delete firstIter;
	  return parent;
	}
  iter->nextElement();
 }
 delete firstIter;
 iter=list->elementsForward();
 firstIter = iter;
 while(iter->hasMoreElements()){
  if(iter->currentElement()->isIfNode()){
   IfHTGNode *ifn=(IfHTGNode*)(iter->currentElement());
   tmp=getUp(&(ifn->getTrueBranch()),ifn,node);
   if (tmp!=NULL) 
     {
       delete firstIter;
       return tmp;
     }
   tmp=getUp(&(ifn->getFalseBranch()),ifn,node);
   if(tmp!=NULL) 
     {
       delete firstIter;
       return tmp;
     }
  }
  else if(iter->currentElement()->isLoopNode()){
   LoopHTGNode *loopn=(LoopHTGNode*)(iter->currentElement());
   tmp=getUp(&(loopn->getLoopList()),loopn,node);
   if(tmp!=NULL) 
     {
       delete firstIter;
       return tmp;
     }
  }
  else if(iter->currentElement()->isSimpleNode()){
   SimpleHTGNode *simpn;
   simpn=(SimpleHTGNode*)(iter->currentElement());
   BaseInstruction *instr,*last;
   instr=simpn->getBasicBlock()->getFirstInstr();
   last=simpn->getBasicBlock()->getLastInstr();
   while(1){
    if(instr==node) 
     {
       delete firstIter;
       return simpn;
     }
    if(instr==last) 
      break;
    instr=instr->getFirstSuccOnly();
   }  
  }
  iter->nextElement();
 }
 delete firstIter;
 return NULL;
}

BaseHTGNode *BaseHTGNode::getUp(){
 return ::getUp(currRoutine->getHTG(),NULL,this);
}


//Compute the unifiable ops for a list of HTG nodes
void compute_unifiable_ops(MyLinkedList<BaseHTGNode*> *list){
 MyLinkedListIterator<BaseHTGNode *> *iter;
 iter=list->elementsForward();
 while(iter->hasMoreElements()){
  iter->currentElement()->compute_unifiable_ops();
  iter->nextElement();
 }
 delete iter;
}


//schedule the instructions in this list of HTG nodes
void schedule_instructions(MyLinkedList<BaseHTGNode*> *list){
 MyLinkedListIterator<BaseHTGNode *>*iter;
 for(iter=list->elementsForward();iter->hasMoreElements();iter->nextElement()){
  iter->currentElement()->schedule();
 }
 delete iter;
}

//returns 1 if the operation "oper" can be moved inside any of the HTG nodes in this list of 
//HTG nodes
int contains_unifiable_op(MyLinkedList<BaseHTGNode*> *list,BaseOperation *oper){
 MyLinkedListIterator<BaseHTGNode *>*iter;
 //for each node in this list of HTG nodes
 for(iter=list->elementsForward();iter->hasMoreElements();iter->nextElement()){
  if(iter->currentElement()->contains_unifiable_op(oper)) 
	  return 1;
 }
 delete iter;
 return 0;
}

//////////////////////////////////////////////////////////////////
//global flag for HTG traversal
//if 1, it does not allow going up in the HTG, in the prevTB routine
static int _notGoUp=0;

//sets the _notGoUp global variable
int setNotGoUp(int v){
 int tmp=_notGoUp;
 _notGoUp=v;
 return tmp;
}

//returns the _notGoUp global variable
int getNotGoUp(void){
 return _notGoUp;
}
//////////////////////////////////////////////////////////////////

//returns the previous HTG node, in the order required by Trailblazing 
//node: input, the node we want the previous for
//oper: input, the operation we are trying to move along this path 
//path: input, output: the current path traversed during trailblazing
BaseHTGNode * prevTB(BaseHTGNode *node,BaseOperation *oper,vector<BaseHTGNode *> *path/*=NULL*/){
 BaseHTGNode *tmp;
 //get the previous HTG node for node, inside this HTG level (w/o going up in the HTG)
 tmp=node->getPrev();
 //if reached the begining of this HTG level
 if(tmp==NULL){
  //go up one level in the HTG (to the parent of node)
  tmp=node->getUp();
  //update the path (history of the movement)
  if(tmp!=NULL && path!=NULL) path->push_back(tmp);
  //if the parent (tmp) is an if HTG node
  if(dynamic_cast<IfHTGNode*>(tmp)!=NULL)
	  //if we want speculative moves (out of conditionals) 
	  if(tbz_allow_speculative){
		  if(tmp->hasLive(oper)){
				//if oper is live at the beginning of the conditional
				//cannot move speculatively (it kills the value) 
				return NULL;
		  }
	  }
	  else 
		  return NULL;
  //if the parent (tmp) is NULL or a LOOP node, or we should not go up in the HTG anymore
  if(tmp==NULL || tmp->isLoopNode() || getNotGoUp()) 
	  return NULL;
  else{
	  //continue from the parent of node
	  return prevTB(tmp,oper,path);
  }
 }
 else{
	 //return the previous HTG node
	 return tmp;
 }
}

