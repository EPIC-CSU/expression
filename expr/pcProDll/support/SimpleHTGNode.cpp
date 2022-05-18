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

extern RegisterFileList *globalRegFileList;

extern ComputeOp *createMoveOp(int destSSANum, BaseArgument *regArg, int srcSSANum, OpCode & opc);
extern void insertAsLastInstr(ComputeOp *moveOp, BasicBlockNode *bb);

#ifdef WIN32_GUI	//WIN32_GUI
#include "../../pcProGUI/ColorCodes.h"


MOUSEHITINFO* SimpleHTGNode::getHit(GRAPH_NODE_TYPE nodeType, CPoint & point)
{
 //pass it down to the only basic block in the node
 return getBasicBlock()->getHit(nodeType, point);
}


GenericNode* SimpleHTGNode::FirstNotExpandedNode(GRAPH_NODE_TYPE nodeType)
{
 ASSERT_TRACE(0,"Don't call us. We'll call you!");
 return NULL;
}

CRect* SimpleHTGNode::Draw(GRAPH_NODE_TYPE nodeType, CDC * pDC)//, int xPos, int yPos)
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
 else //expanded
 {
  //reset node size
  node->rect.SetRect(0,0,0,0);

  //draw the basic block and update rect size
  node->rect.UnionRect(node->rect,getBasicBlock()->Draw(node->nodeType, pDC));//, xPos, yPos));

  //expand the bounding box by a couple of pixels
  node->rect.InflateRect(3,3);
 }
 
 //draw bounding rectangle
 pDC->Rectangle(node->rect);

 pDC->SelectObject(oldPen);
 delete newPen;
 
 return &node->rect;
}

void SimpleHTGNode::DrawInfo(GRAPH_NODE_TYPE nodeType, CDC * pDC, CRect& rect)
{
 GraphNode* node=GetGraphNode(nodeType);
 
 pDC->SetTextColor(node->textColor);
 pDC->SetTextAlign(TA_TOP);//TA_CENTER|TA_BASELINE);
 if (node->label=="")
 {
  char temp[50];
  sprintf(temp,"SP: 0x%08X",this);
  node->label=temp;
 }
 
 //shrink the rectangle by 3 points
 //rect.DeflateRect(1,1);
 
 //pDC->TextOut(node->point.x-xPos,node->point.y-yPos+4,node->label);
 
 //pDC->DrawText(node->label,rect,DT_CENTER|DT_WORDBREAK);
 pDC->TextOut(rect.left+2, rect.top+2,node->label,strlen(node->label));
}

void SimpleHTGNode::DOTText(GRAPH_NODE_TYPE nodeType, CString& out)
{
 char temp[100];
 GraphNode* node=GetGraphNode(nodeType);
 
 //output node definitions
 if (node->expanded) //show node and basic block inside
 {
  //output the basic block
  getBasicBlock()->DOTText(nodeType,out);
 }
 else //just show the SIMPLE node and its outgoing edges
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


SimpleHTGNode::SimpleHTGNode()
{
#ifdef WIN32_GUI	//WIN32_GUI
 cfgNode.borderColor=BORDER_COLOR_SIMPLEHTG;
 ilgNode.borderColor=BORDER_COLOR_SIMPLEHTG;
 htgNode.borderColor=BORDER_COLOR_SIMPLEHTG;
#endif //WIN32_GUI
 _bb = _NULL_;
}

SimpleHTGNode::SimpleHTGNode(BasicBlockNode *someBB) : _bb(someBB)
{
#ifdef WIN32_GUI	//WIN32_GUI
 cfgNode.borderColor=BORDER_COLOR_SIMPLEHTG;
 ilgNode.borderColor=BORDER_COLOR_SIMPLEHTG;
 htgNode.borderColor=BORDER_COLOR_SIMPLEHTG;
#endif //WIN32_GUI
}


void SimpleHTGNode::print(int indent){
  //printIndent(indent);
  //cout << "Simple{";
  printIndent(indent);
  cout << "(* " << this << " *) ";
  cout << "//BB: " << _bb->getDFSNum() << " (* " << _bb << " *) ";
  _bb->print(indent);
  //printIndent(indent);
  //cout << "}";
  cout << endl;
}

void SimpleHTGNode::printC(int indent){
  printIndent(indent);
  _bb->printC(indent);
  cout << endl;
}

void SimpleHTGNode::setInstrID(int &crt,int &opcrt){
  _bb->setInstrID(crt,opcrt);
}


void SimpleHTGNode::renameDestValues()
{
	BBForwIterator iter(_bb);
	while (iter.hasMoreElements())
	{
		(iter.currentElement())->renameAllDestValues();
		iter.nextElement();
	}
}

void SimpleHTGNode::setLowestSSAValues()
{
	BBForwIterator iter(_bb);
	while (iter.hasMoreElements())
	{
		(iter.currentElement())->setLowestSSAValues();
		iter.nextElement();
	}
}

void SimpleHTGNode::renameSrcValues(ValuesSET & in, ValuesSET & out)
{
	ValuesSET tmp1, tmp2;

	tmp1 = in;

	BBForwIterator iter(_bb);
	while (iter.hasMoreElements())
	{
		(iter.currentElement())->renameAllSrcValues(tmp1, tmp2);
		tmp1 = tmp2;
		tmp2.reset();

		iter.nextElement();
	}
	out = tmp1;
}

void SimpleHTGNode::insertMove(int index, int ssaNum)
{
	cout << "%%%%%%%%%%%% Inserting Move: " << index << " " << ssaNum << " %%%%%%%%%" << endl;
	OpCode opc;
	opc.init("IASSIGN");

	BaseArgument *tempArg = globalRegFileList->getRealReg(index);

	ComputeOp *moveOp = createMoveOp(ssaNum, tempArg, ssaNum, opc);
	insertAsLastInstr(moveOp, _bb);

	delete tempArg;
}

void SimpleHTGNode::insertMoves(ValuesSET & moveSet)
{
	ValSETIterator i1, i2;
	i1 = moveSet.begin();
	i2 = moveSet.end();

	while (i1 != i2)
	{
		insertMove(i1->first, i1->second);
		i1++;
	}
}

void SimpleHTGNode::insertMovesPass(ValuesSET & in, ValuesSET & out)
{
	ValuesSET tmp1;

	out = in;

	BBForwIterator iter(_bb);
	while (iter.hasMoreElements())
	{
		(iter.currentElement())->getLocalDefs(tmp1);
		out.impose(tmp1);
		tmp1.reset();

		iter.nextElement();
	}
	if (out.isEmpty())
	{
		cout << "out is empty in simpleHTGNode" << endl;
	}
}

void SimpleHTGNode::insertLabel(BaseArgument *newlabel)
{
	if (_bb->isEmpty()) // checking to see if the bb is a dummy basic block.
	{
		cout << "Warning: Attempting to insert label into a dummy basic block: unsuccessfull" << endl;
		return;
	}

	(dynamic_cast<NormalInstr*>(_bb->getFirstInstr()))->addLabel(newlabel);
}



BaseHTGNode* SimpleHTGNode::copyHTGBody()
{
 BaseHTGNode *newBody;
 newBody = new SimpleHTGNode();
 
 BasicBlockNode *newBB = _bb->copyBody();
 
 (currRoutine->getCFG()).appendBBToList(*newBB);
 //
 // Note: When the basic block is appended to the list, the CFG DataStructure
 // makes a copy of the basicblock. 
 // So, the SimpleHTGNode should point to the bb in the list.
 //
 ((SimpleHTGNode*)newBody)->setBB(&((currRoutine->getCFG()).getLastBB()));
 
 delete newBB;
 return newBody;
}

BaseHTGNode *SimpleHTGNode::copyHTGBody(int numInstrs)
{
	BaseHTGNode *newBody;
	newBody = new SimpleHTGNode();
	
	BasicBlockNode *newBB = _bb->copyBody(numInstrs);
	
	if (newBB == NULL) return NULL;
	
	(currRoutine->getCFG()).appendBBToList(*newBB);
	//
	// Note: When the basic block is appended to the list, the CFG DataStructure
	// makes a copy of the basicblock. 
	// So, the SimpleHTGNode should point to the bb in the list.
	//
	((SimpleHTGNode*)newBody)->setBB(&((currRoutine->getCFG()).getLastBB()));
	
	delete newBB;
	return newBody;
}

void SimpleHTGNode::deleteHTGBody(int numInstrs)
{
	_bb->deleteBody(numInstrs);
}

void SimpleHTGNode::createListOfOperations(OpList & oplist)
{
	_bb->createListOfOperations(oplist);
}