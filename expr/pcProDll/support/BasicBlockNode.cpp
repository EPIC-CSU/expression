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
// $Id: BasicBlockNode.cc,v 1.23 1998/04/09 18:47:06 pgrun Exp $
//
// File:  		BasicBlockNode.cc
// Created:		Wed Jan 15, 97
// Last modified: 	Wed Jan 15, 97
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
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
#include "ComputeOp.h"

#include "BBIterator.h"

#include "FlagsDefs.h"

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


#ifdef WIN32_GUI	//WIN32_GUI

#include "../../pcProGUI/ColorCodes.h"
#include "../../pcProGUI/Prop/BBlockPropPage.h"

void BasicBlockNode::DisplayProperties(BasePropPage* dialog)
{
 BBlockPropPage* page=dynamic_cast<BBlockPropPage*>(dialog);
 ASSERT(page!=NULL);
 
	BBForwIterator iter(this);
 char buf[2000];
 
 sprintf(buf,"%d",getDFSNum());
 page->m_Name=buf;
 
 sprintf(buf,"0x%08X",this);
 page->m_Address=buf;
 
	page->m_InstrList.ResetContent();
 
	while(iter.hasMoreElements())
 {
  BaseInstruction* instr=iter.currentElement();
		instr->printOpcodesToString(buf,"\t");
		if (buf[0]!='\0') page->m_InstrList.AddString((const char *)(&buf));
  iter.nextElement();
	}
 
 page->UpdateData(false);
}

//bool BasicBlockNode::ToggleUDDU(GRAPH_NODE_TYPE nodeType)
//{
// GraphNode* node=GetGraphNode(nodeType);
// bool retVal=(node->showUDDU=!node->showUDDU);
// 
// //now set all instrs in the block to same UDDU
// BBForwIterator it(this);
// while (it.hasMoreElements())
// {
//  it.currentElement()->GetGraphNode(nodeType)->showUDDU=retVal;
//  it.nextElement();
// }
// 
// return retVal;
//}

void BasicBlockNode::ToggleUDDU(GRAPH_NODE_TYPE nodeType)
{
 ToggleUD(nodeType);
 ToggleDU(nodeType);
}

void BasicBlockNode::ToggleUD(GRAPH_NODE_TYPE nodeType)
{
 GraphNode* node=GetGraphNode(nodeType);
 bool retVal=(node->showUD=!node->showUD);
 
 //now set all instrs in the block to same UDDU
 BBForwIterator it(this);
 while (it.hasMoreElements())
 {
  it.currentElement()->GetGraphNode(nodeType)->showUD=retVal;
  it.nextElement();
 } 
}

void BasicBlockNode::ToggleDU(GRAPH_NODE_TYPE nodeType)
{
 GraphNode* node=GetGraphNode(nodeType);
 bool retVal=(node->showDU=!node->showDU);
 
 //now set all instrs in the block to same UDDU
 BBForwIterator it(this);
 while (it.hasMoreElements())
 {
  it.currentElement()->GetGraphNode(nodeType)->showDU=retVal;
  it.nextElement();
 }
}

GenericNode* BasicBlockNode::FirstNotExpandedNode(GRAPH_NODE_TYPE nodeType)
{
 ASSERT_TRACE(0,"Don't call us. We'll call you!");
 return NULL;
}

CRect* BasicBlockNode::Draw(GRAPH_NODE_TYPE nodeType, CDC * pDC)//, int xPos, int yPos)
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
  
  //draw all the instructions in the block
  BBForwIterator it(this);
  while (it.hasMoreElements())
  {
   BaseInstruction* i=it.currentElement();
   //draw instruction and update bounding box for basic block
   node->rect.UnionRect(node->rect,i->Draw(node->nodeType, pDC));//, xPos, yPos));
   it.nextElement();
  }
  
  //expand the bounding box by a couple of pixels
  node->rect.InflateRect(3,3);
 }
 
 //draw bounding rectangle
 pDC->Rectangle(node->rect);

 if (flags.getFlag(IFCV_FLAG))
 {
  //pDC->SelectObject(oldPen);
  //delete newPen;
  //newPen = new CPen(PS_SOLID,2,GREEN);
  //oldPen = pDC->SelectObject(newPen);
  pDC->Rectangle(node->rect.right-5,node->rect.top+3,node->rect.right-3,node->rect.top+5);
 }
 
 pDC->SelectObject(oldPen);
 delete newPen;
 
 return NULL;
}

void BasicBlockNode::DrawInfo(GRAPH_NODE_TYPE nodeType, CDC * pDC, CRect& rect)
{
 GraphNode* node=GetGraphNode(nodeType);
 
 pDC->SetTextColor(node->textColor);
 pDC->SetTextAlign(TA_TOP);//TA_CENTER|TA_BASELINE);
 if (node->label=="")
 {
  char temp[50];
  sprintf(temp,"BB%d: 0x%08X",getDFSNum(), this);
  node->label=temp;
 }
 
 //shrink the rectangle by 3 points
 //rect.DeflateRect(1,1);
 
 //pDC->TextOut(node->point.x-xPos,node->point.y-yPos+4,node->label);
 
 //pDC->DrawText(node->label,rect,DT_CENTER|DT_WORDBREAK);
 pDC->TextOut(rect.left+1, rect.top,node->label,strlen(node->label));

// //NOSTEMP
// BasicBlockNode* bbFirstSucc=this->getFirstSucc();
// BasicBlockNode* bbNextsucc=this->getNextSucc();
// CString str;
// str.Format("0x%08X 0x%08X", (long)bbFirstSucc,(long)bbNextsucc);
// pDC->TextOut(rect.left+1, rect.top+7,str,str.GetLength());
// //NOSTEMP
}

void BasicBlockNode::DOTText(GRAPH_NODE_TYPE nodeType, CString& out)
{
 char temp[100];
 GraphNode* node=GetGraphNode(nodeType);
 
 //output node definitions
 if (node->expanded) //show node and instructions
 {
  ASSERT_TRACE(!isEmpty(),"Uh-oh, no instructions! Shoot the GUI designer!"); 
  
  //sprintf(temp,"subgraph cluster_%X\n{\n",(int)this);
  //out+=temp;
  //sprintf(temp,"label = \"_%X\";\n",(int)this);
  //out+=temp;
  
  //list all instructions as: i1 -> i2 -> i3 -> ....
  int j=0;
  
  BBForwIterator it(this);
  while (it.hasMoreElements())
  {
   BaseInstruction* i=it.currentElement();
   i->DOTText(node->nodeType, out, this);
   it.nextElement();
  }
  //no need for this, the last instr prints it
  //out+="}\n"; //end of cluster
 }
 else //just show the basic block and its outgoing edges
 {
  //sprintf(temp,"blok_%X [ label =\"_%X\"];\n",(int)this,(int)this);
  //if (_dfsNum==1) //rank it as minimum
  //{
  // sprintf(temp,"blok_%X [rank=min] ;\n",(int)this);
  //}
  //else
  //{
  // sprintf(temp,"blok_%X;\n",(int)this);
  //}
  sprintf(temp,"blok_%X;\n",(int)this);
  out+=temp;
  
  node->CleanEdgeMap();
  
  if (getFirstEdge()!=NULL)
  {
   BasicBlockEdge* edge=getFirstEdge();
   if (edge->getNext()!=NULL)
   {
    char temp[200];
    if (!edge->getNext()->IsSameTypeExpanded(node->nodeType))
    {
     //sprintf(temp,"blok_%X -> blok_%X [ label = \"_%X\" ];\n",(int)_prev, (int)_next, (int)this);
     sprintf(temp,"blok_%X -> blok_%X;\n",(int)this, (int)edge->getNext(), (int)this);
     
     BezierEdge* ie=new BezierEdge();
     ie->dest=TO_BASICBLOCK;
     node->edges.SetAt((int)(edge->getNext()),ie); //add it to the map
    }
    else
    {
     //sprintf(temp,"blok_%X -> inst_%X [ label = \"_%X\" ];\n",(int)_prev, (int)_next->getFirstInstr(), (int)this);
     sprintf(temp,"blok_%X -> inst_%X;\n",(int)this, (int)edge->getNext()->getFirstInstr());
     
     BezierEdge* ie=new BezierEdge();
     ie->dest=TO_INSTR;
     node->edges.SetAt((int)(edge->getNext()->getFirstInstr()),ie); //add it to the map
    }
    out+=temp;
   }
  }
  
  if (getNextEdge()!=NULL)
  {
   BasicBlockEdge* edge=getNextEdge();
   if (edge->getNext()!=NULL)
   {
    char temp[200];
    
    if (!edge->getNext()->IsSameTypeExpanded(node->nodeType))
    {
     //sprintf(temp,"blok_%X -> blok_%X [ label = \"_%X\" ];\n",(int)_prev, (int)_next, (int)this);
     sprintf(temp,"blok_%X -> blok_%X;\n",(int)this, (int)edge->getNext(), (int)this);
     
     BezierEdge* ie=new BezierEdge();
     ie->dest=TO_BASICBLOCK;
     node->edges.SetAt((int)(edge->getNext()),ie); //add it to the map
    }
    else
    {
     //sprintf(temp,"blok_%X -> inst_%X [ label = \"_%X\" ];\n",(int)_prev, (int)_next->getFirstInstr(), (int)this);
     sprintf(temp,"blok_%X -> inst_%X;\n",(int)this, (int)edge->getNext()->getFirstInstr());
     
     BezierEdge* ie=new BezierEdge();
     ie->dest=TO_INSTR;
     node->edges.SetAt((int)(edge->getNext()->getFirstInstr()),ie); //add it to the map
    }
    out+=temp;
   }
  }
  
 }
}

//find the block that target belongs to
BasicBlockNode* NormalInstr::hackGetTargetBlock(BaseInstruction* target, BasicBlockNode* block)
{
 //ASSUMPTION: each BB has 2 outbound edges
 //HACK: see if the leader in either dest block is target
 
 if (block->getFirstEdge()!=NULL)
 {
  if (block->getFirstEdge()->getNext()!=NULL)
  {
   if (block->getFirstEdge()->getNext()->getFirstInstr()==target)
   {
    return block->getFirstEdge()->getNext();
   }
  }
 }
 
 if (block->getNextEdge()!=NULL)
 {
  if (block->getNextEdge()->getNext()!=NULL)
  {
   if (block->getNextEdge()->getNext()->getFirstInstr()==target)
   {
    return block->getNextEdge()->getNext();
   }
  }
 }
 
 //if (target!=0x00000000) // let this thru for the last instruction in a routine (its target is 0x00000000)
 {
  //ASSERT(0); //Argh! I knew it!
 }
 return NULL;
}

MOUSEHITINFO* BasicBlockNode::getHit(GRAPH_NODE_TYPE nodeType, CPoint & point)
{
 //go thru all the instrs in the block
 BBForwIterator iter(this);
 while (iter.hasMoreElements())
 {
  BOOL boolsEye=false; // :)
  BaseInstruction* instr=iter.currentElement();
  
  switch (nodeType)
  {
  case GNT_ILG:
   boolsEye=instr->ilgNode.rect.PtInRect(point);
   break;
   
  case GNT_CFG:
   boolsEye=instr->cfgNode.rect.PtInRect(point);
   break;
   
  case GNT_HTG:
   boolsEye=instr->htgNode.rect.PtInRect(point);
   break;
   
  default:
   ASSERT_TRACE(0,"What?");
   break;
  }
  
  if (boolsEye)
  {
   MOUSEHITINFO* hit=new MOUSEHITINFO;
   hit->type=MHI_INSTR;
   hit->node=(void*)instr;
   return hit;
  }
  
  iter.nextElement();
 }
 
 return NULL;
}

#endif //WIN32_GUI


BasicBlockNode::BasicBlockNode() : _dfsNum(0)
{
#ifdef WIN32_GUI	//WIN32_GUI
 cfgNode.borderColor=BORDER_COLOR_BASICBLOCK;
 ilgNode.borderColor=BORDER_COLOR_BASICBLOCK;
 htgNode.borderColor=BORDER_COLOR_BASICBLOCK;
#endif //WIN32_GUI

#ifdef COUNT_BASICBLOCKS
 _numBasicBlocks++;
#endif
 
 _bbType = _INVALID_;
 
 _leader = NULL;
 _trailer = NULL;
 
 _firstSucc = NULL;
 _nextSucc = NULL;
}

BasicBlockNode::BasicBlockNode(const BasicBlockNode & a)
{
#ifdef WIN32_GUI	//WIN32_GUI
 cfgNode.borderColor=BORDER_COLOR_BASICBLOCK;
 ilgNode.borderColor=BORDER_COLOR_BASICBLOCK;
 htgNode.borderColor=BORDER_COLOR_BASICBLOCK;
#endif //WIN32_GUI

#ifdef COUNT_BASICBLOCKS
 _numBasicBlocks++;
#endif
 
 _bbType = a._bbType;
 
 _leader = a._leader;
 _trailer = a._trailer;
 
 _firstSucc = NULL;
 _nextSucc = NULL;
 
 _dfsNum = a._dfsNum;
}


#ifdef COUNT_BASICBLOCKS
int BasicBlockNode::_numBasicBlocks = 0;	
#endif

BaseInstruction *getPrev(BaseInstruction *instr,BaseInstruction *start){
 BaseInstruction *p,*q;
 if(instr==start){
  return NULL;
  printf("Error: getPrev");
  assert(0);
 }
 for(p=q=start;p!=NULL && p!=instr;p=p->getFirstSuccOnly()){
  q=p;
 }
 return q;
}

BaseInstruction *getPrevInstr(BaseInstruction *instr,BaseInstruction *start){
 BaseInstruction *p,*q;
 if(instr==start){
  return NULL;
  printf("Error: getPrev");
  assert(0);
 }
 for(p=q=start;p!=NULL && p!=instr;p=p->getFirstSuccOnly()){
  q=p;
 }
 return q;
}

#ifdef WIN32
__declspec( dllexport ) 
#endif
BasicBlockEdge* BasicBlockNode::getFirstEdge()
{
 //NOS: DO NOT REMOVE
#ifdef WIN32
 AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
 //NOS: DO NOT REMOVE
 
 //	 if (_firstSucc != NULL)
 return _firstSucc;
 
 //	 return NULL;
 //
 // Instead of returning NULL, why not throw a NoSuchElement
 // exception ? The calling routine can catch this exception
 // and recognize that the basicblock is the last basic block
 // (i.e. the exit)
 //
 // I'd like to use exceptions only to signal error cases.
}

#ifdef WIN32
__declspec( dllexport ) 
#endif
BasicBlockEdge* BasicBlockNode::getNextEdge()
{
 //NOS: DO NOT REMOVE
#ifdef WIN32
 AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
 //NOS: DO NOT REMOVE
 
 //	 if (_nextSucc != NULL)
 return _nextSucc;
 
 //       return NULL;
 //
 // See comment above.
}

BasicBlockNode* BasicBlockNode::getFirstSucc()
{
 if (_firstSucc != NULL)
 {
	 return (_firstSucc->getNext());
 }
 return NULL;
 //
 // Instead of returning NULL, why not throw a NoSuchElement
 // exception ? The calling routine can catch this exception
 // and recognize that the basicblock is the last basic block
 // (i.e. the exit)
}

BasicBlockNode* BasicBlockNode::getNextSucc()
{
 if (_nextSucc != NULL)
 {
	 return (_nextSucc->getNext());
 }
 return NULL;
 //
 // See comment above.
}

BasicBlockNode *BasicBlockNode::getFallThruNode()
{
	BasicBlockNode *retPtr;

	retPtr = getNextSucc();

	if (retPtr != NULL) return retPtr;

	retPtr = getFirstSucc();

	if (retPtr != NULL) return retPtr;

	return NULL;
}

BasicBlockNode *BasicBlockNode::getTrueNode()
{
	BasicBlockNode *retPtr = NULL;

	if (getNextSucc() != NULL)
		retPtr = getFirstSucc();

	return retPtr;
}
 

BasicBlockNode* BasicBlockNode::getSuccBB(int nextBBDirection)
{
 if (nextBBDirection == _FIRST_SUCC_) return getFirstSucc();
 else // if (nextBBDirection == _NEXT_SUCC_)
  return getNextSucc();
}

int BasicBlockNode::numInstrs()
{
	int retCount = 0;
	BaseInstruction *currInstr = _leader;

	if (_leader == NULL) return 0;

	retCount++;
	while ((currInstr != NULL) && (currInstr != _trailer))
	{
		retCount++;
		currInstr = ((NormalInstr *)currInstr)->getDefaultTargetInstr();
	}
	return retCount;
}

void BasicBlockNode::print()
{
 cout << "Type: " << _bbType << "  DFS: " << _dfsNum << endl;
 
 cout << " Ldr: " << _leader;
 if (_leader != NULL) _leader->shortPrint(cout);
 cout << " Tlr: " << _trailer;
 if (_trailer != NULL) _trailer->shortPrint(cout);
 cout << endl << "\t";
 
 if (_firstSucc != NULL)
 {
	 cout << "Succ1: ";
	 _firstSucc->print();
 }
 if (_nextSucc != NULL)
 {
	 cout << "\t Succ2: ";
	 _nextSucc->print();
 }
 cout << endl;
 
 cout << "Predecessors: ";
 cout << _controlFlowPredecessors << endl;
 
 cout << "Property:    ";
 // _propList.print();
 // this->printProperty(_COMPDEF_);
 // cout << endl;
 // this->printProperty(_LIVEDEAD_);
 // cout << endl;
}

int BasicBlockNode::isEq(BasicBlockNode* a) {
 return _NO_;
}

int BasicBlockNode::isIfHeadType()
{
 if(_firstSucc!=NULL && _nextSucc!=NULL &&
  !_firstSucc->isBackArc() && !_nextSucc->isBackArc())
  return _YES_;
 return _NO_;
}

int BasicBlockNode::isLoopTailType()
{
 if(_firstSucc!=NULL && _nextSucc!=NULL)
	 if (_firstSucc->isBackArc() || _nextSucc->isBackArc())
		 return _YES_;
 return _NO_;
}

int BasicBlockNode::isCallRetType()
{
	if (_trailer == NULL) return _NO_;

	if (((NormalInstr *)_trailer)->hasCallRetOp())
		return _YES_;
	return _NO_;
}

int BasicBlockNode::hasBackArcTo(BasicBlockNode * bb){
 if(_firstSucc!=NULL && _firstSucc->getNext()==bb && _firstSucc->isBackArc()||
  _nextSucc!=NULL && _nextSucc->getNext()==bb && _nextSucc->isBackArc())
  return _YES_;
 return _NO_;
}

int BasicBlockNode::isLoopHeadType()
{
 MyLinkedListIterator<BasicBlockNode*> *iter;
 BasicBlockNode *currBB;
 iter=_controlFlowPredecessors.createPredListIterForw();
 int retval=_NO_;
 if(iter==NULL) return _NO_;
 while (iter->hasMoreElements()){
  currBB=iter->currentElement();
  if(currBB->hasBackArcTo(this)) {
   retval=_YES_;
   break;
  }
  iter->nextElement();
 }
 delete iter;
 return retval;
}


int BasicBlockNode::isSimpleType(void){
 //temporary solution
 if(!isLoopHeadType() && !isIfHeadType()) return _YES_;
 else return _NO_;
}

void BasicBlockNode::print(int indent)
{
 BaseInstruction *crt,*last;
 
 crt=getFirstInstr();
 last=getLastInstr();
 if (crt == NULL) // i.e. dummy basic block
 {
	 printIndent(indent);
	 cout << "Dummy BasicBlock" << endl;
	 return;
 }
 while(1){
  crt->print(indent);
  if(crt==last) break;
  crt=crt->getFirstSuccOnly();
 }
}

void BasicBlockNode::printC(int indent)
{
 BaseInstruction *crt,*last;

 crt=getFirstInstr();
 last=getLastInstr();
 if (crt == NULL) // i.e. dummy basic block
 {
	 printIndent(indent);
	 cout << "Dummy BasicBlock" << endl;
	 return;
 }
 while(1){
  crt->printC(indent);
  if(crt==last) break;
  crt=crt->getFirstSuccOnly();
 }
} // end of fcn printC

void BasicBlockNode::setInstrID(int &crt,int &opcrt)
{
 BaseInstruction *instr,*last;
 
 instr=getFirstInstr();
 last=getLastInstr();
 
 // Added by SAH on 3rd Aug, 99
 //
 if ((instr == NULL) || (last == NULL))
 {
	 // This may be a dummy basic block.
	 return;
 }
 //
 // end added code.

 BBForwIterator bbiter(this);

 while (bbiter.hasMoreElements())
 {
	 instr = bbiter.currentElement();
	 instr->setID(crt++);
	 ((NormalInstr *)instr)->setOpID(opcrt);

//	 if(dynamic_cast<NormalInstr*>(instr)!=NULL) (static_cast<NormalInstr*>(instr))->setOpID(opcrt);

	 bbiter.nextElement();
 }
}

void BasicBlockNode::_justSetNextBBOnly(BasicBlockNode *nextBB, int nextBBDir, int arcType)
{
	BasicBlockEdge nextSucc(nextBB, arcType);
	
	if (nextBBDir == _FIRST_SUCC_) 
	{
		if (_firstSucc != NULL)
		{
			delete _firstSucc;
		}
		setFirstEdge(nextSucc);
	}
	else if (nextBBDir == _ALL_SUCC_)
	{
		if (_firstSucc != NULL)
		{
			delete _firstSucc;
		}
		setFirstEdge(nextSucc);
		if (_nextSucc != NULL)
		{
			delete _nextSucc;
		}
	}
	else // if (nextBBDir== _SECOND_SUCC_)
	{
		if (_nextSucc != NULL)
		{
			delete _nextSucc;
		}
		setNextEdge(nextSucc);
	}
}

void BasicBlockNode::_setNextBBInstrs(BasicBlockNode *nextBB, int nextBBDir, int arcType)
{
	
	// Note: If the direction is _FIRST_SUCC_, then there are two cases:
	//
	// a: The BB does not have a second succ.
	// b: It has a second succ, but the second succ is to be reset later.
	//
	// For case a: The last flow-op of the instruction must be properly set, 
	// while for case b: the first flow-op must be properly set.
	// As can be seen, these two conflict with each other. 
	//
	// SOLUTION:
	// Introduced a new type called _ALL_SUCC_
	// If _ALL_SUCC_ then it is case a. So, set both the first and the last flow-op.
	// If _FIRST_SUCC_ set only the first flow-op.
	
	// first check if nextBB is NULL. If so, set instrs appropriately.
 
	BaseInstruction *ldr;
 
	if (nextBB == NULL)
		ldr = NULL;
	else
		ldr = nextBB->_leader;
 
	if (nextBBDir == _FIRST_SUCC_)
	{
		_trailer->setNextInstr(ldr);
	}
	else if (nextBBDir == _ALL_SUCC_)
	{	 
		_trailer->setNextInstr(ldr);
		_trailer->setNextInstr(ldr, _LAST_NUM_);
	}
	else   // if (nextBBDir == _SECOND_SUCC_)
	{
		_trailer->setNextInstr(ldr, _LAST_NUM_);
	}
}

// This function deletes the arc, and also sets the flow-ops in the last instr of the basic-block 
// to NULL.
//
void BasicBlockNode::deleteArc(int nextBBDir, int arcType)
{
 if (nextBBDir == _FIRST_SUCC_)
	{
		if (_firstSucc != NULL)
		{
			delete _firstSucc;
   _firstSucc=NULL;
		}
	}
	else if (nextBBDir == _ALL_SUCC_)
	{
		if (_firstSucc != NULL)
		{
			delete _firstSucc;
   _firstSucc=NULL;
		}
		if (_nextSucc != NULL)
		{
			delete _nextSucc;
   _nextSucc=NULL;
		}
	}
	else // if (nextBBDir== _SECOND_SUCC_)
	{
		if (_nextSucc != NULL)
		{
			delete _nextSucc;
   _nextSucc=NULL;
		}
	}
 _setNextBBInstrs(NULL, nextBBDir, arcType);
}

void BasicBlockNode::appendBB(BasicBlockNode *prevBB, int nextBBDir, int arcType)
{
	
	// Note:
	// The assumption here is that prevBB Basic Block already contains
	// the edge to which this Basic Block is to be added.
	
	// Note: On Mon, June 29, 98. I have added code that nullifies this assumption.
	// i.e. this assumption does not need to be true for this method to work.
	
	if (prevBB == NULL)
	{
		// error condition.
		cout << "Panic: Received Null Basic Block as append parameter" << endl;
		assert(0);
	}
	
	prevBB->_justSetNextBBOnly(this, nextBBDir, arcType);
	
	// Note that the arc type has to be set properly. Also, once we do
	// this append, we have lost the dfs ordering.
	
	// Also, the newly appended BasicBlock's successors may not have been 
	// set properly.
	
	//Now, we need to set the instruction pointers appropriately.
	
	prevBB->_setNextBBInstrs(this, nextBBDir, arcType);
}

void BasicBlockNode::setNextBB(BasicBlockNode *nextBB, int nextBBDir, int arcType)
{
	_justSetNextBBOnly(nextBB, nextBBDir, arcType);
	
	// Note that the arc type has to be set properly. Also, once we do
	// this append, we have lost the dfs ordering.
	
	// Also, the newly appended BasicBlock's successors may not have been 
	// set properly.
	
	//Now, we need to set the instruction pointers appropriately.
	
	_setNextBBInstrs(nextBB, nextBBDir, arcType);
}

void BasicBlockNode::appendInstr(NormalInstr *instr,int bbDir){
 BaseInstruction *last;
 last=getLastInstr();
 if(last==NULL){
  initLeader(instr);
  initTrailer(instr);
  return;
 }
 if(_nextSucc==NULL){
  initTrailer(instr);
 }
 
 else{//TBD
 }
}


////////////////////////////////////////////////////////////////////////////////////////////

/*
void PrintLiveSet(SetList<int> *set){
SetList<int>::iterator i;
cout << "Set: " << endl;
for(i=set->begin();i!=set->end();i++){
cout << (*i) << " ";
}
cout << endl;
}
*/

extern SymbolTable* globalSymbolTable;

//Returns 1 if argument arg is live in this basic block
//0 otherwise
int BasicBlockNode::isLive(BaseArgument *arg){
 int val = globalSymbolTable->getID(arg, arg->getPtrToProperty(_SSA_));
 LiveDeadBBProperty* prop=(LiveDeadBBProperty*)this->getPtrToProperty(_LIVEDEAD_);
 if ( prop ) {
  SetList<int>& liveSet = prop->getLiveList();
  //PrintLiveSet(liveSet);
  if ( liveSet.contains(val) )
   return _YES_;
 }
 return _NO_;
}


//Returns the number of live variables (virtual registers) in this basic block
int BasicBlockNode::countLive(void){
 LiveDeadBBProperty* prop=(LiveDeadBBProperty*)this->getPtrToProperty(_LIVEDEAD_);
 if ( prop ) {
  SetList<int>& liveSet = prop->getLiveList();
  return liveSet.numItems();
 }
 return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////

BasicBlockNode *BasicBlockNode::copy() const
{
 BasicBlockNode *retPtr = new BasicBlockNode(*this);
 return retPtr;
}


BasicBlockNode *BasicBlockNode::copyBody(int numInstrs)
{
 BasicBlockNode *retPtr = new BasicBlockNode();
 
 BaseInstruction *nextItem, *prevItem, *lastItem;
 nextItem = _leader;
 prevItem = nextItem;
 
 if ((_leader == NULL)||(_trailer == NULL)) 
 {
	 // do not do anything. return the newed basicblock node.
  
	 cout << " Warning: Encountered a null basic block in copyBody" << endl;
	 return retPtr;
 }

 lastItem = _leader;
 if (numInstrs == _ALLINSTRS_) lastItem = _trailer;
 else 
 {
	 for (int count = 1; count < numInstrs; count++)
	 {
		 if ((lastItem == NULL) || (lastItem == _trailer))
		 {
			 cout << "Panic: Called BasicBlockNode::copyBody with numInstrs greater than number of instrs in node" << endl;
			 assert(0);
			 return NULL;
		 }

		 lastItem = ((NormalInstr *)lastItem)->getDefaultTargetInstr();
	 }
 }


 BaseInstruction *copyInstr, *prevInstr;
 copyInstr = nextItem->copyBody();
 prevInstr = copyInstr;
 
 currRoutine->appendInstruction(copyInstr);
 
 retPtr->initLeader(copyInstr);
 
 while (prevItem != lastItem)
 {
  nextItem = nextItem->getFirstSucc();
  copyInstr = nextItem->copyBody();
  
  prevInstr->setNextInstr(copyInstr);
  
  currRoutine->appendInstruction(copyInstr);
  
  prevItem = nextItem;
  prevInstr = copyInstr;
 }
 retPtr->initTrailer(copyInstr);
 
 return retPtr;
}

extern Routine *currRoutine;

extern void changePointers(BaseInstruction *predInstr, BaseInstruction *currInstr, BaseInstruction *newInstr);

// This method deletes the first "numInstrs" instructions from the basic block.
//
// Note: if the basicblock node had predecessor info, it also corrects it (at the instruction level).
//
// Note: We signal an error if nInstr == number of instrs in bb, cos that would leave us with a NULL basic-block.
//
void BasicBlockNode::deleteBody(int nInstrs)
{
	if (nInstrs >= numInstrs())
	{
		// error condition.
		//
		cout << "Panic: BasicBlockNode::deleteBody recieved invalid number of instrs to delete" << endl;
		cout << "(BB contains : " << numInstrs() << " instrs; tried to delete " << nInstrs << " instrs." << endl;

		assert(0);
	}

	if (nInstrs == 0) return; // don't need to do anything for this.

	// first delete the second - numInstrs instructions (i.e. leaving out the first).
	// This is relatively simple. All the instrs are guaranteed to have only one successor (and predecessor).
	//
	BaseInstruction *nextInstr = ((NormalInstr *)_leader)->getDefaultTargetInstr();
	BaseInstruction *currInstr;

	for (int i = 1; i < nInstrs; i++)
	{
		if ((nextInstr == NULL) || (nextInstr == _trailer)) // either error or done.
			break;

		// first, get the currInstr to be deleted.
		//
		currInstr = nextInstr;

		// next, keep note of the successor of the instr that is currently to be deleted.
		//
		nextInstr = ((NormalInstr *)nextInstr)->getDefaultTargetInstr();

		// next, remove the current instruction.
		//
		currRoutine->removeInstruction(currInstr);
		delete currInstr;
	}

	if (nextInstr == NULL) // signal error and return.
	{
		cout << "Warning: In BasicBlockNode::deleteBody(), while deleting, nextInstr became NULL" << endl;
		return;
	}

	// now, nextInstr points to the first valid instruction in the basic-block.

	// finally, delete the leader and set the IList pointers correctly.
	// Also, if the leader contains a label, transfer it to nextInstr.

	// get the predecessors from the basic-block.
	//	
	MyLinkedListIterator<BasicBlockNode *> *iter = getCFPred().createPredListIterForw();
	BasicBlockNode *predNode;

	while (iter->hasMoreElements())
	{
		// get each bb  predecessor, and make sure that its last instr points to nextInstr
		//
		predNode = iter->currentElement();

		changePointers(predNode->getLastInstr(), _leader, nextInstr);

		iter->nextElement();
	}
	delete iter;

	// if the leader has any label transfer it to nextInstr
	//
	if (((NormalInstr *)_leader)->hasLabel())
	{
		BaseArgument *labelArg = ((NormalInstr *)_leader)->getPtrToLabelArg();
		((NormalInstr *)nextInstr)->addLabel(labelArg);

		// addLabel makes a copy, so we need to delete the old label.
		//
		delete labelArg;
	}

	// now the pointers have been changed, it is safe to delete the leader.
	//
	currRoutine->removeInstruction(_leader);
	_leader = nextInstr;
}

extern char *getNewLabel();

void BasicBlockNode::negateIf()
{
	int numIfOps = ((NormalInstr*)_trailer)->countControlOps();
	BaseOperation *ifOp;
 
	if ( numIfOps == 0)
	{
		// error condition. 
		cout << "Warning: Tried to negateIf of a basic-block which does not contain any if statements" << endl;
		return;
	}
	else if (numIfOps > 1)
	{
		// error condition.
		cout << "warning: negateIf should not be called for a block which has more than one if" << endl;
		return;
	}
 
	ifOp = _trailer->getPtrToRootOperation();
	if (ifOp->isIfOp() == _NO_)
	{
		cout << "Warning: root operation of trailer is not IfOp for negateIf" << endl;
		return;
	}
 
	cout << " (* " << ifOp << " *) ";
	ifOp->shortPrint(cout);
	cout << endl;
 
	BaseArgument *label = ifOp->ptrToOperand(_SOURCE_2_);
	((LabelArg*)label)->initLabel(getNewLabel());
 
	BaseArgument *ccReg = ifOp->ptrToOperand(_SOURCE_1_);
	MyLinkedListIterator<BaseOperation*> *iter;
	iter = ((UDChainProperty*)(ccReg->getPtrToProperty(_UDCHAIN_)))->defListIteratorForw();
 
	BaseOperation *opToNegate = iter->currentElement();
	cout << " (* " << opToNegate << " *) ";
	opToNegate->shortPrint(cout);
	cout << endl;
	iter->nextElement();
	if (iter->hasMoreElements())
	{
		cout << " (* " << iter->currentElement() << " *) ";
		(iter->currentElement())->shortPrint(cout);
		cout << endl;
		// currently, an error condition.
		cout << "Warning: The If statement (to be negated) has more than one defining operation" << endl;
	}
	delete iter;
 
	// Should make a check to see if the operation that is being changed (opToNegate)
	// is indeed an appropriate operation.
	int newIndex = globalIfOppositesTable.getOppositeIndex(((NormalOp*)opToNegate)->getOpCodeIndex());
	if (newIndex == _INVALID_)
	{
		// error condition.
		cout << "Warning: The operation that is to be negated is not a comparision operation" << endl;
		return;
	}
	((NormalOp*)opToNegate)->changeOpCode(newIndex);
}

int BasicBlockNode::hasInstr(BaseInstruction *instr)
{
	BBForwIterator it(this);
 
	//printf("\nInstructions in bb:\n");
	while (it.hasMoreElements())
	{
		//printf("%d\n",it.currentElement()->idValue());
		if (it.currentElement() == instr)
			return _YES_;
		it.nextElement();
	}
 
	return _NO_;
}

extern int _LAST_FLOW_POS_;

void BasicBlockNode::insertAsLastOp(BaseOperation *op)
{
 
 NormalInstr *newInstr;
 JumpOp tempJumpOp;
 BaseOperation *headOp;
 
 int index;
 
 NormalInstr  *lastInstr = (NormalInstr *)getLastInstr();
 
 if (lastInstr == NULL) // i.e. the basic block is a newly created block (or a dummy block)
	{
		newInstr = new NormalInstr();
  
		newInstr->initScheduleAndAdd(op);
		//
		// Now add a flowop to nowhere.
		//
		JumpOp tempJumpOp;
		index = newInstr->addFlowOpToList(&(tempJumpOp), _LAST_FLOW_POS_);
		(newInstr->getRootOpSlot()).setNext(&(newInstr->getFlowOpSlot(index)));
  
		// add this new instr to the current routine.
		//
		currRoutine->appendInstruction((BaseInstruction *)newInstr);
  
		initLeader(newInstr);
		initTrailer(newInstr);
		return;
	}
 
	// If the basic block is not a newly created block (or a dummy block)
	// then .....
 
	headOp = lastInstr->getPtrToRootOperation();
 
	if (op->isComputeOp())
	{
  
		if (!headOp->isFlowOp())
		{
			newInstr = new NormalInstr(*lastInstr);
   
			currRoutine->insertInstrAfter(newInstr,lastInstr);
		}
		if (headOp->isComputeOp()||headOp->isMemoryOp()) {
			//inserted instruction is inserted after the last instruction of BB
			//setup FlowOp for the old last instruction
			ArrayIterator<FlowOpSlot> *fIter = lastInstr->getFlowOpSlotIter();
			while (fIter->hasMoreElements()) {
				if (fIter->currentElement().hasOper()) {
					JumpOp *jOp = (JumpOp *)fIter->currentElement().getPtrToOperation();
					jOp->setTargetInstr(newInstr);
					break;
				}
				fIter->nextElement();
			}
			delete fIter;
   
			//it must be DataOpSlot since the operation is ComputeOp, must be in DataOpSlot
			DataOpSlot& slot = (DataOpSlot&)newInstr->getRootOpSlot();
			slot.deleteOperation();
			slot.setOperation(op);
   
			initTrailer(newInstr);
		}
		else if (headOp->isIfOp()){
			//the inserted instruction is really inserted before the last instruction of BB
			//since the last instruction of BB is IF, there is no assignment to change the
			//value of some registers, doing so is same , and easy
   
			//insert the mov operation in the old last instruction
			lastInstr->removeAllOp();
			lastInstr->initAddDataOp((DataOp*)op);
   
			//setup jump for old last instruction
			tempJumpOp.setTargetInstr(newInstr);
			index = lastInstr->addFlowOpToList(&(tempJumpOp), _LAST_FLOW_POS_);
			(lastInstr->getRootOpSlot()).setNext(&(lastInstr->getFlowOpSlot(index)));
   
			initTrailer(newInstr);
		}
		else 
		{
			if (headOp->isFlowOp())
			{
    //				cout << " Encountered FlowOp: " << headOp->isGotoOp() << endl;
				BaseOpSlot & flowSlot = lastInstr->getRootOpSlot();
				lastInstr->initScheduleAndAdd(op);
				lastInstr->getRootOpSlot().setNext(&(flowSlot));
			}
			else // headOp->isControlOp()
			{
				cout << " Encountered unknown type of operation" << endl;
				assert(0);
			}
		}
	}
	else if (op->isFlowOp())
	{
		if (headOp->isComputeOp()||headOp->isMemoryOp())
		{
			ArrayIterator<FlowOpSlot> *fIter = lastInstr->getFlowOpSlotIter();
			while (fIter->hasMoreElements()) {
				if (fIter->currentElement().hasOper()) {
					BaseOperation *jOp = fIter->currentElement().getPtrToOperation();
					fIter->currentElement().addOperation(op);
					delete jOp;
					break;
				}
				fIter->nextElement();
			}
			delete fIter;
		}
		else if (headOp->isControlOp()) // then we add the flow op to the fall-thru branch.
		{
			BaseOperation *jOp = (lastInstr->getFlowOpSlot(_LAST_FLOW_POS_)).getPtrToOperation();
			index = lastInstr->addFlowOpToList((FlowOp *)op, _LAST_FLOW_POS_);
			if (jOp != NULL)
				delete jOp;
		}
		else if (headOp->isControlOp()) // then we add the flow op to the fall-thru branch.
		{
			BaseOperation *jOp = (lastInstr->getFlowOpSlot(_LAST_FLOW_POS_)).getPtrToOperation();
			index = lastInstr->addFlowOpToList((FlowOp *)op, _LAST_FLOW_POS_);
			if (jOp != NULL)
				delete jOp;
		}
	}
}

// create list of operations
void BasicBlockNode::createListOfOperations(OpList & oplist)
{
	BBForwIterator iter(this);

	while(iter.hasMoreElements())
	{
		iter.currentElement()->createListOfOperations(oplist);
		iter.nextElement();
	}
}

// Currently, we go : Routine -> BasicBlock -> Instruction -> Operation -> Argument.
// Note that HTGNode is not present in the recursion.
//
void BasicBlockNode::recursiveRemoveProperty(PropertyNames name)
{
	_propList.removeProperty(name);

	BBForwIterator iter(this);

	while (iter.hasMoreElements())
	{
		(iter.currentElement())->recursiveRemoveProperty(name);
		iter.nextElement();
	}
}

// for asm gen
void BasicBlockNode::printASMByCFG(ostream &out)
{
	BBForwIterator iter(this);
	while(iter.hasMoreElements())
	{
		(iter.currentElement())->printASMInstr(out);
		iter.nextElement();
	}
	return;
}


BaseArgument *BasicBlockNode::getLabel()
{
	if (_leader != NULL)
		return (_leader->getPtrToLabelArg());

	return NULL;
}

BaseOperation *BasicBlockNode::hasGoto()
{
	if (_trailer != NULL)
	{
		return ((NormalInstr *)_trailer)->hasGotoOp();
	}	
	return NULL;
}

BaseOperation *BasicBlockNode::hasIf()
{
	if (_trailer != NULL)
	{
		return ((NormalInstr *)_trailer)->hasIfOp();
	}	
	return NULL;
}

void BasicBlockNode::irToC(FilePointer & fp, int indent)
{
	// iterate over the list of instructions, and print it.
	//
	BBForwIterator bbIter(this);

	while (bbIter.hasMoreElements())
	{
		(bbIter.currentElement())->irToC(fp, indent);
		bbIter.nextElement();
	}
}

// if predBB does not contain an arc to bb, returns _INVALID_
// else returns _YES_ if arc from predBB to bb is a loop back arc.
// else returns _NO_;
//
int isBackArcPred(BasicBlockNode *predBB, BasicBlockNode *bb)
{
	if ((predBB == NULL) || (bb == NULL)) return _INVALID_;

	BasicBlockEdge *edge;
	edge = predBB->getFirstEdge();
 
	if (edge->getNext() == bb)
	{
		if (edge->isBackArc()) return _YES_;
		return _NO_;
	}

	edge = predBB->getNextEdge();
	if (edge->getNext() == bb)
	{
		if (edge->isBackArc()) return _YES_;
		return _NO_;
	}

	return _INVALID_;
} 

BasicBlockNode *insertDummyBBBetween(BasicBlockNode *currBB, BasicBlockNode *succBB, Routine *curR)
{
	BasicBlockNode dummyBB;

	if ((currBB->getFirstSucc() == succBB) && ((currBB->getFirstEdge())->isBackArc())) return NULL;
	if ((currBB->getNextSucc() == succBB) && ((currBB->getNextEdge())->isBackArc())) return NULL;

	(curR->getCFG()).appendBBToList(dummyBB);

	BasicBlockNode *retPtr = &((curR->getCFG()).getLastBB());

	BasicBlockEdge *edg;
	// first find the edge from currBB to succBB that needs to be changed.
	//
	if (currBB->getFirstSucc() == succBB) // then, change this edge.
	{
		edg = (currBB->getFirstEdge())->copy();

		(currBB->getFirstEdge())->setNext(retPtr);
		//
		// let the arc type be the same as the one before.
	}
	else if (currBB->getNextSucc() == succBB) // then, change this edge.
	{
		edg = (currBB->getNextEdge())->copy();

		(currBB->getNextEdge())->setNext(retPtr);
		(currBB->getNextEdge())->setArcType(_TREE_ARC_);

	}
	edg->setArcType(_TREE_ARC_);
	retPtr->setFirstEdge(edg);

	// Now, set dummyBB's predecessors to currBB, and change succBB's predecessors to include dummyBB.
	//
	retPtr->addCFPred(currBB);

	BasicBlockPredecessors & succBBPreds = succBB->getCFPred();
	succBBPreds.removePred(currBB);
	succBBPreds.addPred(retPtr);

	return retPtr;
}

void BasicBlockNode::addInstruction(BaseInstruction *instrToBeAdded, BaseInstruction *beforeOrAfterThisInstr, bool before)
{
	if (beforeOrAfterThisInstr == NULL)
	{
		beforeOrAfterThisInstr = _leader;
		before = true;
	}

	if (hasInstr(beforeOrAfterThisInstr) == _NO_) // error : the instruction was not in the basic-block.
	{
		cout << "Panic: In BasicBlockNode::addInstruction, the position instruction was not in the bb" << endl;
		assert(0);
	}

	// There are three cases:
	// 1) Normal Case : the instr is to be inserted in the middle of the basic block.
	// 2) Leader Case : the instr is to be inserted before the current leader of the basic block.
	// 3) Trailer Case : the instr is to be inserted after the current trailer of the basic block.

	// Normal Case is when:
	// a) beforeOrAfterThisInstr is not the leader or the trailer
	// b) beforeOrAfterThisInstr is leader and before == false.
	// c) beforeOrAfterThisInstr is trailer and before == true.

	int addcase;

	if ((beforeOrAfterThisInstr == _leader) && (before == true)) // leader case
		addcase = 2;
	else if ((beforeOrAfterThisInstr == _trailer) && (before == false)) // trailer case
		addcase = 3;
	else // normal case
		addcase = 1;

	if (addcase == 1)
	{
		if (before == false) // i.e. new instr is to be added after the prev instr
			((NormalInstr *)instrToBeAdded)->appendInstr(beforeOrAfterThisInstr, _FALSE_BRANCH_, 1);
		else
		{
			// need to find the instruction previous to beforeOrAfterThisInstr and then append this instr
			//
			((NormalInstr *)instrToBeAdded)->appendInstr(getPrevInstr(beforeOrAfterThisInstr, _leader), _FALSE_BRANCH_, 1);
		}
	}
	else if (addcase == 2) // need to add as leader
	{
		// first add the instruction to the instruction list.
		//
		currRoutine->insertInstrBefore(instrToBeAdded, _leader);

		// iterate over the predecessor list of bbs, and for each trailer, change its pointer from the current leader to the new instruction.
		//
		MyLinkedListIterator<BasicBlockNode *> *iter = getCFPred().createPredListIterForw();
		BasicBlockNode *predNode;

		while (iter->hasMoreElements())
		{
			// get each bb predecessor, and make sure that its last instr points to nextInstr
			//
			predNode = iter->currentElement();

			// change pointers (instrToBeChanged, instrPointedTo, instrToBePointedTo).
			//
			changePointers(predNode->getLastInstr(), _leader, instrToBeAdded);

			iter->nextElement();
		}
		delete iter;

		// if leader has any label transfer it to nextInstr
		//
		if (((NormalInstr *)_leader)->hasLabel())
		{
			BaseArgument *labelArg = ((NormalInstr *)_leader)->getPtrToLabelArg();
			((NormalInstr *)instrToBeAdded)->addLabel(labelArg);

			// addLabel makes a copy, so we delete the old label.
			//
			((NormalInstr *)_leader)->deleteLabel();
		}
		
		// now, make instrToBeAdded point to the original leader.
		//
		instrToBeAdded->setNextInstr(_leader);
		_leader = instrToBeAdded;
	}
	else if (addcase == 3) // need to add as trailer
	{
		// TBD.
		//
		assert(0);
	}
}

// This function takes a newly created basic-block and adds it in between prevNode and nextNode.
//
// It assumes that nextNode is an original successor of prevNode.
// Note: This function does not add the new basic-block to the routine cfg.
//
// This function does not add a newNode along a loop backarc, however.
// In this case it returns _NO_
//
int insertBBBetween(BasicBlockNode *prevNode, BasicBlockNode *nextNode, BasicBlockNode *newNode)
{
	// if prevNode is NULL add newNode as a predecessor to nextNode assuming nextNode is the fall-thru.

	// if nextNode is NULL add newNode as the fall-thru successor of prevNode.
	
	// if prevNode and nextNode are not NULL, then add newNode to the appropriate path.
	
	// 
	if ((prevNode->getFirstSucc() == nextNode) && ((prevNode->getFirstEdge())->isBackArc())) return _NO_;
	if ((prevNode->getNextSucc() == nextNode) && ((prevNode->getNextEdge())->isBackArc())) return _NO_;


	if (prevNode != NULL)
	{
		// first find the edge from currBB to succBB that needs to be changed.
		//
		if (prevNode->getFirstSucc() == nextNode) // then, change this edge.
		{
			(prevNode->getFirstEdge())->setNext(newNode);
			(prevNode->getFirstEdge())->setArcType(_TREE_ARC_);
		}
		else if (prevNode->getNextSucc() == nextNode) // then, change this edge.
		{
			(prevNode->getNextEdge())->setNext(newNode);
			(prevNode->getNextEdge())->setArcType(_TREE_ARC_);

		}
		else // error.
		{
			cout << "Warning: Tried to insert BB between non-consecutive basic blocks. Was unsuccessful." << endl;
			return _NO_;
		}
	}

	BasicBlockEdge edg(nextNode);
	newNode->setFirstEdge(&edg);

	// Now, set newbb's predecessors to currBB, and change succBB's predecessors to include dummyBB.
	//
	newNode->addCFPred(prevNode);

	BasicBlockPredecessors & succBBPreds = nextNode->getCFPred();
	succBBPreds.removePred(prevNode);
	succBBPreds.addPred(newNode);

	// Also, update the flow ops in the affected instructions of the bbs to point to the correct instructions.
	changePointers(prevNode->getLastInstr(), nextNode->getFirstInstr(), newNode->getFirstInstr());
	(newNode->getLastInstr())->setNextInstr(nextNode->getFirstInstr());

	return _YES_;
}
