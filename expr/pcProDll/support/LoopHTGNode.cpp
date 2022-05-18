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
#include "NormalInstr.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

#ifdef WIN32_GUI	//WIN32_GUI
#include "../../pcProGUI/ColorCodes.h"


MOUSEHITINFO* LoopHTGNode::getHit(GRAPH_NODE_TYPE nodeType, CPoint & point)
{
 MOUSEHITINFO* res;

 //go thru all the nodes in the loop
 MyLinkedListIterator<BaseHTGNode *> *iter=createLoopIterForw();
 while(iter->hasMoreElements())
 {
  if (res=iter->currentElement()->getHit(nodeType,point)) break;
  iter->nextElement();
 }
 delete iter;
 return res;
}


GenericNode* LoopHTGNode::FirstNotExpandedNode(GRAPH_NODE_TYPE nodeType)
{
 ASSERT_TRACE(0,"Don't call us. We'll call you!");
 return NULL;
}

CRect* LoopHTGNode::Draw(GRAPH_NODE_TYPE nodeType, CDC * pDC)//, int xPos, int yPos)
{
 GraphNode* node=GetGraphNode(nodeType);
 
 //not doing anything else but boxes for now...
 ASSERT_TRACE(node->shape=="box","What shape doth thou want me to draw, Master?");
 
 CPen* newPen = new CPen(PS_SOLID,2,node->selected?YELLOW:node->borderColor);
 CPen* oldPen = pDC->SelectObject(newPen);
 
 if (!node->expanded)
 {
  //print out the label
  DrawInfo(node->nodeType, pDC, node->rect);
  
  //draw all outbound edges
  POSITION p=node->edges.GetStartPosition();
  while (p!=NULL)
  {
   BezierEdge* edge;
   int key;
   node->edges.GetNextAssoc(p,key,edge);
   BezierEdge::Draw(pDC, edge);//, xPos, yPos);
  }
  
 }
 else // expanded
 {
  //draw the nodes in the loop
  MyLinkedListIterator<BaseHTGNode *> *iter=createLoopIterForw();
  while(iter->hasMoreElements())
  {
   iter->currentElement()->Draw(nodeType, pDC);//, xPos, yPos);
   iter->nextElement();
  }
  delete iter;
 }
 
 //draw bounding rectangle
 pDC->Rectangle(node->rect);

 /*
 if (node->fillColor!=NO_FILL)
 {
  pDC->SelectObject(oldBrush);
 }
 */

 pDC->SelectObject(oldPen);
 delete newPen;
 
 return NULL;
}

void LoopHTGNode::DrawInfo(GRAPH_NODE_TYPE nodeType, CDC * pDC, CRect& rect)
{
 GraphNode* node=GetGraphNode(nodeType);
 
 pDC->SetTextColor(node->textColor);
 pDC->SetTextAlign(TA_TOP);//TA_CENTER|TA_BASELINE);
 if (node->label=="")
 {
  char temp[50];
  sprintf(temp,"LP: 0x%08X",this);
  node->label=temp;
 }
 
 //shrink the rectangle by 3 points
 //rect.DeflateRect(1,1);
 
 //pDC->TextOut(node->point.x-xPos,node->point.y-yPos+4,node->label);
 
 //pDC->DrawText(node->label,rect,DT_CENTER|DT_WORDBREAK);
 pDC->TextOut(rect.left+2, rect.top+2,node->label,strlen(node->label));
}

void LoopHTGNode::DOTText(GRAPH_NODE_TYPE nodeType, CString& out)
{
 char temp[100];
 GraphNode* node=GetGraphNode(nodeType);
 
 //output node definitions
 if (node->expanded) //show node and instructions
 {
  //list all components as as: c1 -> c2 -> c3 -> ....
  int j=0;
  
  /*
  BBForwIterator it(this);
  while (it.hasMoreElements())
  {
   BaseInstruction* i=it.currentElement();
   i->DOTText(node->nodeType, out, this);
   it.nextElement();
  }
  */
 }
 else //just show the LOOP node and its outgoing edges
 {
  sprintf(temp,"htgn_%X;\n",(int)this);
  out+=temp;
  
  node->CleanEdgeMap();
  
  BaseHTGNode* nextNode=getNext();
  
  if (nextNode!=NULL) //here's an edge
  {
   char temp[200];
   
   if (!nextNode->IsSameTypeExpanded(node->nodeType)) //not expanded, easy!
   {
    sprintf(temp,"htgn_%X -> htgn_%X;\n",(int)this, (int)nextNode);
    
    BezierEdge* ie=new BezierEdge();
    ie->dest=TO_INSTR;
    node->edges.SetAt((int)nextNode,ie); //add it to the map
   }
   else //expanded
   {
    /*
    sprintf(temp,"blok_%X -> blok_%X;\n",(int)this, (int)edge->getNext(), (int)this);
    
    BezierEdge* ie=new BezierEdge();
    ie->dest=TO_BASICBLOCK;
    node->edges.SetAt((int)(edge->getNext()),ie); //add it to the map
    */
   }
   out+=temp;
  }
 }
}
#endif //WIN32_GUI


LoopHTGNode::LoopHTGNode()
{
#ifdef WIN32_GUI	//WIN32_GUI
 cfgNode.borderColor=BORDER_COLOR_LOOPHTG;
 ilgNode.borderColor=BORDER_COLOR_LOOPHTG;
 htgNode.borderColor=BORDER_COLOR_LOOPHTG;
#endif //WIN32_GUI
}


BaseHTGNode* LoopHTGNode::copyHTGBody()
{
 BaseHTGNode *tempNodePtr, *copyNodePtr;
 BaseHTGNode *newBody;
 newBody = new LoopHTGNode();
 
 tempNodePtr = getLoopHead();
 copyNodePtr = tempNodePtr->copyHTGBody();
 ((LoopHTGNode*)(newBody))->setNewLoopHead(copyNodePtr);
 
 tempNodePtr = tempNodePtr->getNext();
 
 while (tempNodePtr != NULL)
 {
	 copyNodePtr = tempNodePtr->copyHTGBody();
	 ((LoopHTGNode*)(newBody))->appendNode(copyNodePtr);
	 
	 tempNodePtr = tempNodePtr->getNext();
 }
 return newBody;
}

BasicBlockNode* LoopHTGNode::linkBBs(BasicBlockNode *nextBB, int direction, int arcType)
{
   MyLinkedListIterator<BaseHTGNode *> *loopListIter;
   loopListIter = _loopList.elementsBackward();
   
   BasicBlockNode *tempBB = nextBB;

   int tempDir;

   if (direction == _ALL_SUCC_)
   {
	   // we currently assume that the fall thru of the loop is its second succ.
	   // this may not always be correct.
	   //
	   tempDir = _SECOND_SUCC_;
   }
   else
	   tempDir = direction;
   
   int tempArcType = arcType;
   
   BaseHTGNode *currHTGNode;
   while (loopListIter->hasMoreElements())
   { 
	 currHTGNode = loopListIter->currentElement();
	 tempBB = currHTGNode->linkBBs(tempBB, tempDir, tempArcType);
	 tempDir = _ALL_SUCC_;
	 tempArcType = _TREE_ARC_;
	 
	 loopListIter->nextElement();
   }
   BasicBlockNode *lastBB = getLastBB();
   if ((direction == _SECOND_SUCC_) || (direction == _ALL_SUCC_))
	   tempBB->appendBB(lastBB, _FIRST_SUCC_, _BACK_ARC_);
   else
	   tempBB->appendBB(lastBB, _SECOND_SUCC_, _BACK_ARC_);
   
   delete loopListIter;

   return tempBB;
}

BasicBlockNode* LoopHTGNode::getFirstBB()
{
	return (_loopList.getHead())->getFirstBB();
}

BasicBlockNode* LoopHTGNode::getLastBB()
{
   return (_loopList.getTail())->getLastBB();
}

void LoopHTGNode::renameDestValues()
{
	MyLinkedListIterator<BaseHTGNode *> *iter;
	iter = _loopList.elementsForward();
	while (iter->hasMoreElements())
	{
		(iter->currentElement())->renameDestValues();
		iter->nextElement();
	}
	delete iter;
}

void LoopHTGNode::insertMove(int index, int ssaNum)
{
	getExitNode()->insertMove(index, ssaNum);
}

void LoopHTGNode::insertMoves(ValuesSET & moveSet)
{
	getExitNode()->insertMoves(moveSet);
}

void LoopHTGNode::insertMovesPass(ValuesSET & in, ValuesSET & out)
{
	ValuesSET tmp1, tmp2;

	MyLinkedListIterator<BaseHTGNode *> *iter;
	iter = _loopList.elementsForward();
	tmp1 = in;
	
	while (iter->hasMoreElements())
	{
		(iter->currentElement())->insertMovesPass(tmp1, tmp2);
		tmp1 = tmp2;
		tmp2.reset();
		iter->nextElement();
	}

	// Now, compare in and tmp1. If in and tmp1 have an entry with the same index, but with
	// different data, insert a move at the end of the loop and force it to have dest.
	// as (index, ssaNum_of_in)
	// 
	// Note: this could be inefficient, because the move is inside the loop.
	// It could be removed if the definition outside the loop is renamed.
	// That can be an optimization that is done later.

	ValuesSET diff;
	diff.setAsDataDiff(in, tmp1);

	BaseHTGNode *exitBlock = getExitNode();

	// Note:
	// The next step is not entirely correct. The moves have to be inserted in the back-arc portion
	// of the loop.
	//
	// As we have done it now, it will work, but this means that there is re-definition of the 
	// same variable (one def. outside the loop and the other inside the loop).
	//
	// It would be better if this were changed.
	exitBlock->insertMoves(diff);

	out = tmp1;
	out.impose(diff);
	
	delete iter;
}

void LoopHTGNode::renameSrcValues(ValuesSET & in, ValuesSET & out)
{
	ValuesSET tmp1, tmp2;

	MyLinkedListIterator<BaseHTGNode *> *iter;
	iter = _loopList.elementsForward();
	tmp1 = in;

	while (iter->hasMoreElements())
	{
		(iter->currentElement())->renameSrcValues(tmp1, tmp2);
		tmp1 = tmp2;
		tmp2.reset();
		iter->nextElement();
	}

	delete iter;
	out = tmp1;
}

void LoopHTGNode::setLowestSSAValues()
{
	MyLinkedListIterator<BaseHTGNode *> *iter;
	iter = _loopList.elementsForward();
	while (iter->hasMoreElements())
	{
		(iter->currentElement())->setLowestSSAValues();
		iter->nextElement();
	}
	delete iter;
}

void LoopHTGNode::insertLabel(BaseArgument *newlabel)
{
	getLoopHead()->insertLabel(newlabel);
}

void LoopHTGNode::insertBefore(BaseHTGNode *thisNode, BaseHTGNode *nextNode)
{
	try
	{
		_loopList.insertBefore(thisNode, nextNode);
	}
	catch (Exception *e)
	{
		cout << "Panic: Tried to insert a node in the wrong place in LoopHTGNode::insertBefore" << endl;
		delete e;
		assert(0);
	}

	if (nextNode == getLoopHead())
		setLoopHead(thisNode);
}

void LoopHTGNode::print(int indent){
 MyLinkedListIterator<BaseHTGNode *> *iter=createLoopIterForw();
 printIndent(indent);
 cout << "Loop (* " << this << " *) {";
 while(iter->hasMoreElements()){
  iter->currentElement()->print(indent+1);
  iter->nextElement();
 }
 printIndent(indent);
 cout << "}";
}

void LoopHTGNode::printC(int indent){
 MyLinkedListIterator<BaseHTGNode *> *iter=createLoopIterForw();
 printIndent(indent);
 // cout << "do {";
 cout << "{";			// the do-while loop is removed since
				// it is being implemented as if-goto
 while(iter->hasMoreElements()){
  iter->currentElement()->printC(indent+1);
  iter->nextElement();
 }
 printIndent(indent);
 // cout << "} while (1);";
 cout << "}";			// implemented as if-goto loop
} // end of fcn printC

void LoopHTGNode::compute_unifiable_ops(void){
 //nothing
}

void LoopHTGNode::setInstrID(int &crt,int &opcrt){
 MyLinkedListIterator<BaseHTGNode *> *iter;
 iter=_loopList.elementsForward();
 while(iter->hasMoreElements()){
  iter->currentElement()->setInstrID(crt,opcrt);
  iter->nextElement();
 }
 delete iter; 
}


void LoopHTGNode::fixIfHead(void){
 ::fixIfHead(&_loopList);
 setLoopHead(_loopList.getHead());
}

int LoopHTGNode::loopType()
{
	// if loop is irreducible returns _LOOP_TYPE_0
	// if loop is reducible:
	//   if back arc is conditional returns _LOOP_TYPE_1
	//   if back arc is unconditional returns _LOOP_TYPE_2
	// default return = _LOOP_TYPE_0
	
	if (_ptrToLoopDS != NULL)
	{
		if (_ptrToLoopDS->isIrreducible())
			return _LOOP_TYPE_0;
	}
	
	BaseHTGNode *currPtr;
	BasicBlockNode *exitBB;

	currPtr = _loopList.getTail();
	
	if (currPtr != NULL)
	{
		if (currPtr->isSimpleNode() == _NO_)
		{
			// we currently do not handle this case.
			//
			cout << "Warning: In get loop type, atleast one of the exit nodes is not of SimpleHTGNode type." << endl;
			return _LOOP_TYPE_0;
		}
		if ((exitBB = currPtr->getLastBB()) == NULL)
		{
			// error condition.
			//
			cout << "Panic: Found a simple htg node (loop exit) that didn't have a last BB." << endl;
			assert(0);
		}
		if (((NormalInstr *)(exitBB->getLastInstr()))->hasIfOp() == NULL)
		{
			// i.e. found a last instruction that does not have an if operation.
			//
			return _LOOP_TYPE_2;
		}
		else
		{
			return _LOOP_TYPE_1;
		}
	}
	return _LOOP_TYPE_0;
}

int LoopHTGNode::containsCallNode()
{
	int retVal = _NO_;
	MyLinkedListIterator<BaseHTGNode *> *iter;
	iter=_loopList.elementsForward();
	while(iter->hasMoreElements()){
		if ((iter->currentElement())->containsCallNode() == _YES_)
			retVal = _YES_;
		iter->nextElement();
	}
	delete iter;
	return retVal;
}

int LoopHTGNode::containsIfNode()
{
	int retVal = _NO_;
	MyLinkedListIterator<BaseHTGNode *> *iter;
	iter=_loopList.elementsForward();
	while(iter->hasMoreElements()){
		if ((iter->currentElement())->containsIfNode() == _YES_)
			retVal = _YES_;
		iter->nextElement();
	}
	delete iter;
	return retVal;
}

void LoopHTGNode::createListOfOperations(OpList &oplist)
{
	MyLinkedListIterator<BaseHTGNode *> *iter;
	iter=_loopList.elementsForward();
	while(iter->hasMoreElements())
	{
		(iter->currentElement())->createListOfOperations(oplist);
		iter->nextElement();
	}
	delete iter;
}