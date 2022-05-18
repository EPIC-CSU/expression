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

#ifdef WIN32_GUI //WIN32_GUI
#include "../../pcProGUI/ColorCodes.h"


GenericNode* BaseHTGNode::FirstNotExpandedNode(GRAPH_NODE_TYPE nodeType)
{
 ASSERT_TRACE(0,"Don't call us. We'll call you!");
 return NULL;
}

CRect* BaseHTGNode::Draw(GRAPH_NODE_TYPE nodeType, CDC * pDC)//, int xPos, int yPos)
{
 GraphNode* node=GetGraphNode(nodeType);

 //not doing anything else but boxes for now...
 ASSERT_TRACE(node->shape=="box","What shape doth thou want me to draw, Master?");
 
 CPen* newPen = new CPen(PS_SOLID,2,node->selected?YELLOW:node->borderColor);
 CPen* oldPen = pDC->SelectObject(newPen);

 if (node->expanded) //draw its components (also compute new bounding box as we do that)
 {
  //?
 }

 //draw bounding rectangle
 pDC->Rectangle(node->rect);

 if (!node->expanded) //print out the label
 {  
  DrawInfo(node->nodeType, pDC, node->rect);
 }
 
 //draw all outbound edges
 POSITION p=node->edges.GetStartPosition();
 while (p!=NULL)
 {
  BezierEdge* edge;
  int key;
  node->edges.GetNextAssoc(p,key,edge);
  BezierEdge::Draw(pDC, edge);//, xPos, yPos);
 }

 pDC->SelectObject(oldPen);
 delete newPen;

 return &node->rect;
}

void BaseHTGNode::DrawInfo(GRAPH_NODE_TYPE nodeType, CDC * pDC, CRect& rect)
{
 ASSERT_TRACE(0,"Don't call us. We'll call you!");
}

void BaseHTGNode::DOTText(GRAPH_NODE_TYPE nodeType, CString& out)
{
 ASSERT_TRACE(0,"Don't call us. We'll call you!");
}

#endif //WIN32_GUI

BaseHTGNode::BaseHTGNode()
{
 _next = NULL;
}

void fixIfHead(MyLinkedList<BaseHTGNode*> *list){
 MyLinkedListIterator<BaseHTGNode *> *iter;
 MyLinkedList<BaseHTGNode *> *tmpList;
 BaseHTGNode *node,*head,*prev;
 iter=list->elementsForward();
 while(iter->hasMoreElements()){
  node=iter->currentElement();
  node->fixIfHead();
  iter->nextElement();
 }
 delete iter;
 
 tmpList=new MyLinkedList<BaseHTGNode*>(*list);
 
 iter=tmpList->elementsForward();
 prev=NULL;
 while(iter->hasMoreElements()){
  node=iter->currentElement();
  iter->nextElement();
  if(node->isIfNode()){
   head=((IfHTGNode*)node)->getIfHead();
   list->insertBefore(head,node);
   if(prev!=NULL) prev->setNext(head);
   head->setNext(node);
  }
  prev=node;
 }
 delete iter;
 delete tmpList;
}

