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

#ifdef WIN32_GUI	//WIN32_GUI
#include "../../pcProGUI/ColorCodes.h"


MOUSEHITINFO* IfHTGNode::getHit(GRAPH_NODE_TYPE nodeType, CPoint & point)
{
 MOUSEHITINFO* res;

 //go thru all the nodes in the block
 
 //the head
 res=getIfHead()->getHit(nodeType, point);
 if (res) return res;
 
 //the TRUE branch
 MyLinkedListIterator<BaseHTGNode *> *iter=createTrueBranchIterForw();
 while(iter->hasMoreElements())
 {
  if (res=iter->currentElement()->getHit(nodeType, point)) 
  {
   delete iter;
   return res;
  }
  iter->nextElement();
 }
 delete iter;
 
 //the FALSE branch
 iter=createFalseBranchIterForw();
 while(iter->hasMoreElements())
 {
  if (res=iter->currentElement()->getHit(nodeType, point)) 
  {
   delete iter;
   return res;
  }
  iter->nextElement();
 }
 delete iter;
 
 //the join node
 return getIfJoin()->getHit(nodeType, point);
}


GenericNode* IfHTGNode::FirstNotExpandedNode(GRAPH_NODE_TYPE nodeType)
{
 ASSERT_TRACE(0,"Don't call us. We'll call you!");
 return NULL;
}

CRect* IfHTGNode::Draw(GRAPH_NODE_TYPE nodeType, CDC * pDC)//, int xPos, int yPos)
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
  //reset the bounding box size
  node->rect.SetRect(0,0,0,0);

  //draw the head
  node->rect.UnionRect(node->rect,getIfHead()->Draw(nodeType, pDC));
  
  //draw the TRUE branch
  MyLinkedListIterator<BaseHTGNode *> *iter=createTrueBranchIterForw();
  while(iter->hasMoreElements())
  {
   node->rect.UnionRect(node->rect,iter->currentElement()->Draw(nodeType, pDC));
   iter->nextElement();
  }
  delete iter;
  
  //draw the FALSE branch
  iter=createFalseBranchIterForw();
  while(iter->hasMoreElements())
  {
   node->rect.UnionRect(node->rect,iter->currentElement()->Draw(nodeType, pDC));
   iter->nextElement();
  }
  delete iter;
  
  //draw the join node
  node->rect.UnionRect(node->rect,getIfJoin()->Draw(nodeType, pDC));

  //expand the bounding box by a couple of pixels
  node->rect.InflateRect(3,3);
 }
 
 //draw bounding rectangle
 pDC->Rectangle(node->rect);

 pDC->SelectObject(oldPen);
 delete newPen;
 
 return NULL;
}

void IfHTGNode::DrawInfo(GRAPH_NODE_TYPE nodeType, CDC * pDC, CRect& rect)
{
 GraphNode* node=GetGraphNode(nodeType);
 
 pDC->SetTextColor(node->textColor);
 pDC->SetTextAlign(TA_TOP);//TA_CENTER|TA_BASELINE);
 if (node->label=="")
 {
  char temp[50];
  sprintf(temp,"IF: 0x%08X",this);
  node->label=temp;
 }
 
 //shrink the rectangle by 3 points
 //rect.DeflateRect(1,1);
 
 //pDC->TextOut(node->point.x-xPos,node->point.y-yPos+4,node->label);
 
 //pDC->DrawText(node->label,rect,DT_CENTER|DT_WORDBREAK);
 pDC->TextOut(rect.left+2, rect.top+2,node->label,strlen(node->label));
}

void IfHTGNode::DOTText(GRAPH_NODE_TYPE nodeType, CString& out)
{
 char temp[100];
 GraphNode* node=GetGraphNode(nodeType);
 
 //output node definitions
 if (node->expanded) //show node and components
 {
  /*
  //draw the head
  getIfHead()->DOTText(nodeType, pDC);
  
  //draw the TRUE branch
  MyLinkedListIterator<BaseHTGNode *> *iter=createTrueBranchIterForw();
  while(iter->hasMoreElements())
  {
   iter->currentElement()->DOTText(nodeType, pDC, target);
   iter->nextElement();
  }
  delete iter;
  
  //draw the FALSE branch
  iter=createFalseBranchIterForw();
  while(iter->hasMoreElements())
  {
   iter->currentElement()->DOTText(nodeType, pDC);//, xPos, yPos);
   iter->nextElement();
  }
  delete iter;
  
  //draw the join node
  getIfJoin()->DOTText(nodeType, pDC);//, xPos, yPos);
  */
 }
 else //just show the IF node and its outgoing edges
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


IfHTGNode::IfHTGNode()  : _ifHead(NULL), _dummyIfJoin(NULL)
{
#ifdef WIN32_GUI	//WIN32_GUI
 cfgNode.borderColor=BORDER_COLOR_IFHTG;
 ilgNode.borderColor=BORDER_COLOR_IFHTG;
 htgNode.borderColor=BORDER_COLOR_IFHTG;
#endif //WIN32_GUI
}


BaseHTGNode* IfHTGNode::copyHTGBody()
{
 BaseHTGNode *tempNodePtr, *copyNodePtr;
 BaseHTGNode *newBody;
 newBody = new IfHTGNode();
 
 tempNodePtr = getIfHead();
 copyNodePtr = tempNodePtr->copyHTGBody();
 ((IfHTGNode*)(newBody))->setIfHead(copyNodePtr);
// ((IfHTGNode*)(newBody))->setIfHead(NULL);
 
 MyLinkedListIterator<BaseHTGNode *> *tIter;
 tIter = _trueBranch.elementsForward();
 
 while (tIter->hasMoreElements())
 {
	 tempNodePtr = tIter->currentElement();
	 
  copyNodePtr = tempNodePtr->copyHTGBody();
	 ((IfHTGNode*)(newBody))->appendTrueNode(copyNodePtr);
	 
	 tIter->nextElement();
 }
 delete tIter;
 
 tIter = _falseBranch.elementsForward();
 
 while (tIter->hasMoreElements())
 {
	 tempNodePtr = tIter->currentElement();
	 
  copyNodePtr = tempNodePtr->copyHTGBody();
	 ((IfHTGNode*)(newBody))->appendFalseNode(copyNodePtr);
	 
	 tIter->nextElement();
 }
 delete tIter;
 
 if (_dummyIfJoin != NULL)
	 ((IfHTGNode*)(newBody))->setIfJoin(_dummyIfJoin->copyHTGBody());
 
 return newBody;
}

BasicBlockNode *IfHTGNode::linkBBs(BasicBlockNode *nextBB, int direction, int arcType)
{
	BasicBlockNode *tempBB = nextBB;
	BasicBlockNode *trueBB = nextBB;
	BasicBlockNode *falseBB = nextBB;
 
	int tempDir, tempArcType;
	tempDir = direction;
	tempArcType = arcType;
	
 BaseHTGNode *currHTGNode;
 
 
 MyLinkedListIterator<BaseHTGNode *> *listIter;
 listIter = _trueBranch.elementsBackward();
 
 while (listIter->hasMoreElements())
 {
	 currHTGNode = listIter->currentElement();
	 trueBB = currHTGNode->linkBBs(trueBB, tempDir, tempArcType);
  
	 tempDir = _ALL_SUCC_;
	 tempArcType = _TREE_ARC_;
  
	 listIter->nextElement();
 }
 
 delete listIter;
 
 //   tempBB = _dummyIfJoin->linkBBs(tempBB, tempDir, _TREE_ARC_);
 
 listIter = _falseBranch.elementsBackward();
	tempDir = direction;
	tempArcType = _CROSS_ARC_;
 
 while (listIter->hasMoreElements())
 {
	 currHTGNode = listIter->currentElement();
	 falseBB = currHTGNode->linkBBs(falseBB, tempDir, tempArcType);
  
	 tempDir = _ALL_SUCC_;
	 tempArcType = _TREE_ARC_;
  
	 listIter->nextElement();
 }
 
 delete listIter;
 
	tempBB = _ifHead->linkBBs(trueBB, _FIRST_SUCC_, _TREE_ARC_);
 
	(falseBB)->appendBB(_ifHead->getLastBB(), _SECOND_SUCC_, _TREE_ARC_);
 
 
	return tempBB;
 //   tempBB = _dummyIfJoin->linkBBs(tempBB, tempDir, _CROSS_ARC_);
 
}

void IfHTGNode::renameDestValues()
{
	_ifHead->renameDestValues();
 
	MyLinkedListIterator<BaseHTGNode *> *iter;
	
	iter = _trueBranch.elementsForward();
	while (iter->hasMoreElements())
	{
		(iter->currentElement())->renameDestValues();
		iter->nextElement();
	}
	delete iter;
 
	iter = _falseBranch.elementsForward();
	while (iter->hasMoreElements())
	{
		(iter->currentElement())->renameDestValues();
		iter->nextElement();
	}
	delete iter;
}

void IfHTGNode::setLowestSSAValues()
{
	_ifHead->setLowestSSAValues();
 
	MyLinkedListIterator<BaseHTGNode *> *iter;
	
	iter = _trueBranch.elementsForward();
	while (iter->hasMoreElements())
	{
		(iter->currentElement())->setLowestSSAValues();
		iter->nextElement();
	}
	delete iter;
 
	iter = _falseBranch.elementsForward();
	while (iter->hasMoreElements())
	{
		(iter->currentElement())->setLowestSSAValues();
		iter->nextElement();
	}
	delete iter;
}

void IfHTGNode::renameSrcValues(ValuesSET & in, ValuesSET & out)
{
	ValuesSET tmp1, tmp2, trueTmp, falseTmp;
 
	_ifHead->renameSrcValues(in, tmp1);
 
/*	cout << " Printing contents of IfHead in RenameSrcs" << endl;
	tmp1.print(cout);
	cout << endl;
 */
	trueTmp = tmp1;
 
	MyLinkedListIterator<BaseHTGNode *> *iter;
	
	iter = _trueBranch.elementsForward();
	while (iter->hasMoreElements())
	{
		(iter->currentElement())->renameSrcValues(trueTmp, tmp2);
		trueTmp = tmp2;
		tmp2.reset();
		iter->nextElement();
	}
	delete iter;
 
	falseTmp = tmp1;
	iter = _falseBranch.elementsForward();
	while (iter->hasMoreElements())
	{
		(iter->currentElement())->renameSrcValues(falseTmp, tmp2);
		falseTmp = tmp2;
		tmp2.reset();

/*		cout << "Printing FalseTmp ###################" << endl;
		falseTmp.print(cout);
		cout << "End printing FalseTmp ###################" << endl;
  */
		iter->nextElement();
	}
	delete iter;
 
	// Now we need to compose the two final SETs.
	// (i.e.: Out of lastnode_in_truebranch U Out of lastnode_in_falsebranch)
 
/*	cout << " Printing contents of TrueTmp in RenameSrcs" << endl;
	trueTmp.print(cout);
	cout << endl;
 
	cout << " Printing contents of FalseTmp in RenameSrcs" << endl;
	falseTmp.print(cout);
	cout << endl;
 */
	out.setAsUnion(trueTmp, falseTmp);
}

void IfHTGNode::insertMove(int index, int ssaNum)
{
	createOrGetLastTrueNode()->insertMove(index, ssaNum);
	createOrGetLastFalseNode()->insertMove(index, ssaNum);
}

void IfHTGNode::insertMoves(ValuesSET & moveSet)
{
	createOrGetLastTrueNode()->insertMoves(moveSet);
	createOrGetLastFalseNode()->insertMoves(moveSet);
}

void IfHTGNode::insertMovesPass(ValuesSET & in, ValuesSET & out)
{
	int index, ssaNum, newSSANum;
 
	ValuesSET tmp1, tmp2, trueTmp, falseTmp;
 
	_ifHead->insertMovesPass(in, tmp1);

/*	cout << " Printing contents of IfHead in InsertMovesPass" << endl;
	tmp1.print(cout);
	cout << " End printing IfHead" << endl;
*/
	trueTmp = tmp1;
 
	MyLinkedListIterator<BaseHTGNode *> *iter;
	
	iter = _trueBranch.elementsForward();
	while (iter->hasMoreElements())
	{
		(iter->currentElement())->insertMovesPass(trueTmp, tmp2);
		trueTmp = tmp2;
		tmp2.reset();
		iter->nextElement();
	}
	delete iter;
 
	falseTmp = tmp1;
 /*
	cout << "Printing FalseTmp $$$$$$$$$$$$$$$$$$$$$$$$$$$$" << endl;
	falseTmp.print(cout);
	cout << "End printing FalseTmp $$$$$$$$$$$$$$$$$$$$$$$$$$$$" << endl;
 */
	iter = _falseBranch.elementsForward();
	while (iter->hasMoreElements())
	{
		(iter->currentElement())->insertMovesPass(falseTmp, tmp2);
		falseTmp = tmp2;
		tmp2.reset();
		iter->nextElement();
  
		if (falseTmp.isEmpty())
		{
			cout << "******** falseTmp is empty" << endl;
		}
  
/*		cout << "Printing FalseTmp ###################" << endl;
  falseTmp.print(cout);
  cout << "End printing FalseTmp ###################" << endl;
  */
	}
	delete iter;
 
	// At this point, we are left with out set of:
	//		1) if-head
	//		2) last_node of true_branch
	//		3) last_node of false_branch
 
	// We need to determine the differences between (2) and (3) and insert the moves
	// appropriately.
 
	ValuesSET diff;
	BaseHTGNode *lastTrueNode, *lastFalseNode;
 
	lastTrueNode = createOrGetLastTrueNode();
	lastFalseNode = createOrGetLastFalseNode();
 
	// Note: I have commented out the next section of code, because it does not make
	// any difference. I first included it for sake of completeness, but soon discovered
	// that if a new variable was defined along one path (and not in the other), I do not
	// need to insert any moves to take care of the new variable in the other path.
	//
 /*
	diff.setAsIndexDiff(trueTmp, falseTmp);
	//
	// diff now contains the set of indexes that are in trueTmp, but not in falseTmp.
 
   (lastFalseNode)->insertMoves(diff);
   
     diff.setAsIndexDiff(falseTmp, trueTmp);
     //
     // diff now contains the set of indexes that are in falseTmp, but not in trueTmp.
     
       (lastTrueNode)->insertMoves(diff);
 */
 
	// Now, for each entry where the ssaNumbers are different, insert a move appropriately.
	// This is where the out_set of the if-head comes in use.
 /*
	cout << "Printing FalseTmp **********************" << endl;
	falseTmp.print(cout);
	cout << "End printing FalseTmp *******************" << endl;
 */
	diff.setAsDataDiff(trueTmp, falseTmp);
 
	ValSETIterator i1, i2, i3, i4, i5;
	i1 = diff.begin();
	i2 = diff.end();
	i3 = tmp1.end();
 
	while (i1 != i2)
	{
		index = i1->first;
		ssaNum = i1->second;
		if ((i4 = tmp1.find(index)) != i3) // trying to find if the index also belongs to tmp1
		{
			i5 = falseTmp.find(index);
   
			if (i4->second != ssaNum)
			{
				cout << "*** Insert Move in false path ***" << endl;
				(lastFalseNode)->insertMove(index, ssaNum);
				i5->second = ssaNum;
			}
			else
			{
				newSSANum = i5->second;
				cout << "*** Insert Move in true path ***" << endl;
				(lastTrueNode)->insertMove(index, newSSANum);
				trueTmp.find(index)->second = newSSANum;
			}
		}
		else // index does not belong to tmp1. Which means that we have arbitrarily decided to
		{	 // insert a move in the false branch. This can be made more scientific, by deciding
   // whether it is better to add the move to the true or the false branch
   
			cout << "*** Insert Move in false path ***" << endl;
			(lastFalseNode)->insertMove(index, ssaNum);
		}
		i1++;
	}
 /*
	cout << " Printing contents of TrueTmp in InsertMovesPass" << endl;
	trueTmp.print(cout);
	cout << " End printing trueTmp" << endl;
 
	cout << " Printing contents of FalseTmp in InsertMovesPass" << endl;
	falseTmp.print(cout);
	cout << " End printing falseTmp" << endl;
 */
	out.setAsUnion(trueTmp, falseTmp);
}

void IfHTGNode::insertLabel(BaseArgument *newlabel)
{
	_ifHead->insertLabel(newlabel);
}

void IfHTGNode::insertBefore(BaseHTGNode *thisNode, BaseHTGNode *nextNode)
{
	if (nextNode == _ifHead)
	{
		// This is an error condition.
		cout << "Warning: Tried to insert a node before an if head node" << endl;
		return;
	}

	int notdone = 0;

	try
	{
		_trueBranch.insertBefore(thisNode, nextNode);
	}
	catch (Exception *e)
	{
		// cout << "Did not find the next element in true branch" << endl;
		notdone = 1;
		delete e;
	}

	if (notdone)
	{
		try
		{
			_falseBranch.insertBefore(thisNode, nextNode);
		}
		catch (Exception *e)
		{
			// error condition.
			cout << "Panic: Could not find the next element in the if block (for IfHTGNode::insertBefore) " << endl;
			delete e;
			assert(0);
		}
	}
}

BasicBlockNode* IfHTGNode::getFirstBB()
{
	return _ifHead->getFirstBB();
}

	
BasicBlockNode* IfHTGNode::getLastBB()
{
	BasicBlockNode *retPtr = NULL;

	// SAH, Nov 10, 2000
	// first print a warning : We should not really be executing this piece of code.
	//
	// We currently may need to execute this during If-Conversion, because we do not
	// update the HTG structure when we convert an If to a Simple HTG node.
	//
	cout << "Warning: Trying to get Last BB of a IfHTGNode" << endl;

	// Currently, we return the last node of the false branch.
	//
	retPtr = (_falseBranch.getTail())->getLastBB();

	return retPtr;
}

void IfHTGNode::print(int indent){
 MyLinkedListIterator<BaseHTGNode *> *titer=createTrueBranchIterForw();
 MyLinkedListIterator<BaseHTGNode *> *fiter=createFalseBranchIterForw();
 //do not print the If Head, because the if head has been moved by FixIfHead in the higher HTG level 
 //_ifHead->print(indent);
 printIndent(indent);
 cout << "If (* " << this << " *) {";
 while(titer->hasMoreElements()){
  titer->currentElement()->print(indent+1);
  titer->nextElement();
 }
 printIndent(indent);
 cout << "}";
 printIndent(indent);
 cout << "else{";
 while(fiter->hasMoreElements()){
  fiter->currentElement()->print(indent+1);
  fiter->nextElement();
 }
 printIndent(indent);
 cout << "}";
}

void IfHTGNode::printC(int indent){
 MyLinkedListIterator<BaseHTGNode *> *titer=createTrueBranchIterForw();
 MyLinkedListIterator<BaseHTGNode *> *fiter=createFalseBranchIterForw();
 // _ifHead->printC(indent);   // The If head is known to be a repeat
 // code which is already in the top of the if statement
 // printIndent(indent);
 // cout << "If (* " << this << " *) {";
 while(titer->hasMoreElements()){
  titer->currentElement()->printC(indent+1);
  titer->nextElement();
 }
 // printIndent(indent);
 /* cout << "}";
 printIndent(indent);
 cout << "else{";
 */
 while(fiter->hasMoreElements()){
  fiter->currentElement()->printC(indent+1);
  fiter->nextElement();
 }
 // printIndent(indent);
 // cout << "}";
} // end of fcn printC

BaseHTGNode* IfHTGNode::createOrGetFirstTrueNode(int yesOrNo)
{
 
 BaseHTGNode *firstTrueNode;
 
 firstTrueNode = _trueBranch.getHead();
 
 if ((firstTrueNode == NULL) && (yesOrNo == _YES_))
 {
  // As of now, print warning.
  cout << "Warning: Encountered NULL truebranch in createOrGetFirstTrueNode. The program may no longer be correct" << endl;
 }
 return firstTrueNode;
}

BaseHTGNode* IfHTGNode::createOrGetFirstFalseNode(int yesOrNo)
{
 
 BaseHTGNode *firstFalseNode;
 
 firstFalseNode = _falseBranch.getHead();
 
 if ((firstFalseNode == NULL) && (yesOrNo == _YES_))
 {
  // As of now, print warning.
  cout << "Warning: Encountered NULL truebranch in createOrGetFirstFalseNode. The program may no longer be correct" << endl;
 }
 return firstFalseNode;
}

BaseHTGNode* IfHTGNode::createOrGetLastTrueNode(int yesOrNo)
{
 
 BaseHTGNode *lastTrueNode = NULL;
 
 if ((_trueBranch.isEmpty()) && (yesOrNo == _YES_))
 {
  // As of now, print warning.
  cout << "Warning: Encountered NULL truebranch in createOrGetLastTrueNode. The program may no longer be correct" << endl;
  
  BasicBlockNode *newBB;
  newBB = new BasicBlockNode();
  
  BaseHTGNode *newNode;
  newNode = new SimpleHTGNode(newBB);
  _trueBranch.append(newNode);

  lastTrueNode = newNode;
  
 }
 else
	 if (!_trueBranch.isEmpty())
		 lastTrueNode = _trueBranch.getTail();
 
 return lastTrueNode;
}

BaseHTGNode* IfHTGNode::createOrGetLastFalseNode(int yesOrNo)
{
 
 BaseHTGNode *lastFalseNode;
 
 lastFalseNode = _falseBranch.getTail();
 
 if ((lastFalseNode == NULL) && (yesOrNo == _YES_))
 {
  // As of now, print warning.
  cout << "Warning: Encountered NULL truebranch in createOrGetLastFalseNode. The program may no longer be correct" << endl;
 }
 return lastFalseNode;
}

void IfHTGNode::setInstrID(int &crt,int &opcrt){
 MyLinkedListIterator<BaseHTGNode *> *iter;
 iter=_trueBranch.elementsForward();
 while(iter->hasMoreElements())
 {
  iter->currentElement()->setInstrID(crt,opcrt);
  iter->nextElement();
 }
 delete iter;
 iter=_falseBranch.elementsForward();
 while(iter->hasMoreElements())
 {
  iter->currentElement()->setInstrID(crt,opcrt);
  iter->nextElement();
 }
 delete iter;
}

void IfHTGNode::fixIfHead(void){
 ::fixIfHead(&_trueBranch);
 ::fixIfHead(&_falseBranch);
}

int IfHTGNode::containsCallNode()
{
	int retVal = _NO_;
	MyLinkedListIterator<BaseHTGNode *> *iter;
	iter=_trueBranch.elementsForward();
	while(iter->hasMoreElements()){
		if ((iter->currentElement())->containsCallNode() == _YES_)
			retVal = _YES_;
		iter->nextElement();
	}
	delete iter;
	iter=_falseBranch.elementsForward();
	while(iter->hasMoreElements()){
		if ((iter->currentElement())->containsCallNode() == _YES_)
			retVal = _YES_;
		iter->nextElement();
	}
	delete iter;
	
	return retVal;
}

int IfHTGNode::containsLoopNode()
{
	int retVal = _NO_;
	MyLinkedListIterator<BaseHTGNode *> *iter;
	iter=_trueBranch.elementsForward();
	while(iter->hasMoreElements()){
		if ((iter->currentElement())->containsLoopNode() == _YES_)
			retVal = _YES_;
		iter->nextElement();
	}
	delete iter;
	iter=_falseBranch.elementsForward();
	while(iter->hasMoreElements()){
		if ((iter->currentElement())->containsLoopNode() == _YES_)
			retVal = _YES_;
		iter->nextElement();
	}
	delete iter;
	
	return retVal;
}
