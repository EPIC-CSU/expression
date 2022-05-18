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

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 



int LoopHTGNode::find_unifop_destination(BaseOperation *oper,MyLinkedList<BaseOperation *> moves){
 BaseHTGNode *node; 
 if(!this->depends(oper,moves)){
  node=prevTB(this,oper);
  if(node!=NULL){
   return node->find_unifop_destination(oper,moves);
  }
 }
 return 0;
}

void LoopHTGNode::trailblaze(void){
 MyLinkedListIterator<BaseHTGNode *> *iter;
 
 ::compute_unifiable_ops(&_loopList);
 ::schedule_instructions(&_loopList);
 
 //do trail blazing for all the inner loops
 iter=_loopList.elementsForward();
 while(iter->hasMoreElements()){
  iter->currentElement()->trailblaze();
  iter->nextElement();
 }
 delete iter;
}

void LoopHTGNode::schedule(void){
 //nothing
}

int LoopHTGNode::migrate(BaseOperation *oper,vector<BaseHTGNode *> *path){
 BaseHTGNode *node;
 path->push_back(this);
 node=prevTB(this,oper,path);
 if(node!=NULL){
  return node->migrate(oper,path);
 }
 return 0;
}

int LoopHTGNode::contains_unifiable_op(BaseOperation *oper){
 return 0;
}

int LoopHTGNode::depends(BaseOperation *op,MyLinkedList<BaseOperation *> &moves){
 MyLinkedListIterator<BaseHTGNode *> *iter=createLoopIterForw();
 while(iter->hasMoreElements()){
  if(iter->currentElement()->depends(op,moves)) return 1;
  iter->nextElement();
 }
 return 0;
}


int LoopHTGNode::hasLive(BaseOperation *oper){
 MyLinkedListIterator<BaseHTGNode *> *iter;
 iter=_loopList.elementsForward();
 while(iter->hasMoreElements()){
  if(iter->currentElement()->hasLive(oper)) return 1;
  iter->nextElement();
 }
 delete iter; 
 return 0;
}


