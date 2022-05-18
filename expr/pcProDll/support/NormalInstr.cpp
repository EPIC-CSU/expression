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
// $Id: NormalInstr.cc,v 1.43 1998/04/15 18:02:06 pgrun Exp $
//
// File:  		NormalInstr.cc
// Created:		Wed Nov 20, 96
// Last modified: 	Wed Nov 20, 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: INSTRUCTION

#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include "stdafx.h"

#include "IburgToIR.h"

#include "GlobalIncludes.h"

#include "NormalInstr.h"
#include "JumpOp.h"
#include "CallOp.h"
#include "Instruction.h"
#include "dependency.h"
#include "BasicBlockNode.h"
#include "BaseHTGNode.h"
#include "SimpleHTGNode.h"
#include "IfHTGNode.h"
#include "ComputeOp.h"
#include "PropertyIncludes.h"
#include "UDProperty.h"
#include "DUProperty.h"
#include "Routine.h"
#include "BaseInstruction.h"
#include "MemoryOp.h"
#include "Instruction.h"
#include "RenameTable.h"
#include "UDDU.h"
#include "BaseOperation.h"

#include "CodeLocProperty.h"

//#include "DerivedClassIncludes.h"
#include "IRTraversal.h"

#include "FlagsDefs.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

extern Routine *currRoutine;

#ifdef WIN32_GUI//WIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUI

#include "../../pcProGUI/ColorCodes.h"
#include "../../pcProGUI/Prop/InstrPropPage.h"

/*
void NormalInstr::Draw(GRAPH_NODE_TYPE nodeType, CDC * pDC)//, int x_pos, int y_pos)
{
ASSERT_TRACE(0,"Don't call us. We'll call you!");
}
*/


extern void printIndent(FilePointer & fp, int indent);

void showUD(GraphNode* node, BaseInstruction* instr, BaseOpSlot& opSlot, CString& out)
{
	MyLinkedList<BaseOperation*> *defList;
	MyLinkedListIterator<BaseOperation*> *defIter;
	BaseInstruction *target;
	char temp[100];
	
	if(opSlot.hasOper())
	{
		BaseOperation* op=opSlot.getPtrToOperation();
		
		//first operand
		if(op->hasOperand(_SOURCE_1_)==_YES_)
		{
			defList=GetDefList(op->ptrToOperand(_SOURCE_1_));
			defIter=defList->elementsForward();
			while(defIter->hasMoreElements())
			{
				target=defIter->currentElement()->getInstr();
				if(target!=NULL)
				{
					sprintf(temp,"inst_%X -> inst_%X [color = %d];\n",(int)instr,(int)(target),EDGE_COLOR_UD);
					out+=temp;
					BezierEdge* ie=new BezierEdge();
					ie->dest=TO_INSTR;
					node->edges.SetAt((int)(target),ie); //add it to the map
				}
				defIter->nextElement();
			}
			delete defIter;
		}
		
		//second operand
		if(op->hasOperand(_SOURCE_2_)==_YES_)
		{
			defList=GetDefList(op->ptrToOperand(_SOURCE_2_));
			defIter=defList->elementsForward();
			while(defIter->hasMoreElements())
			{
				target=defIter->currentElement()->getInstr();
				if(target!=NULL)
				{
					sprintf(temp,"inst_%X -> inst_%X [color = %d];\n",(int)instr,(int)(target),EDGE_COLOR_UD);
					out+=temp;
					BezierEdge* ie=new BezierEdge();
					ie->dest=TO_INSTR;
					node->edges.SetAt((int)(target),ie); //add it to the map
				}
				defIter->nextElement();
			}
			delete defIter;
		}
	}
}


void showDU(GraphNode* node, BaseInstruction* instr, BaseOpSlot& opSlot, CString& out)
{
	MyLinkedList<BaseOperation*> *useList;
	MyLinkedListIterator<BaseOperation*> *useIter;
	BaseInstruction *target;
	char temp[100];
	
	if(opSlot.hasOper())
	{
		BaseOperation* op=opSlot.getPtrToOperation();
		
		if(op->hasOperand(_DEST_, _DEST_LIST_)==_YES_)
		{
			useList=GetUseList(op->ptrToOperand(_DEST_, _DEST_LIST_));
			useIter=useList->elementsForward();
			while(useIter->hasMoreElements())
			{
				target=useIter->currentElement()->getInstr();
				if(target!=NULL){
					sprintf(temp,"inst_%X -> inst_%X [color = %d];\n",(int)instr,(int)(target),EDGE_COLOR_DU);
					out+=temp;
					BezierEdge* ie=new BezierEdge();
					ie->dest=TO_INSTR;
					node->edges.SetAt((int)(target),ie); //add it to the map
				}
				useIter->nextElement();
			}
			delete useIter;
		}
	}
}

//this version used by ILGView
void NormalInstr::DOTText(GRAPH_NODE_TYPE nodeType, CString& out)
{
	ASSERT_TRACE(nodeType==GNT_ILG,"Nah-uh!");
	
	char temp[100];
	GraphNode* node=GetGraphNode(nodeType);
	
	//issue the instruction node first
	//sprintf(temp,"inst_%X [ label =\"_%X\" ];\n",(int)this,(int)this);
	sprintf(temp,"inst_%X;\n",(int)this);
	out+=temp; 
	
	node->CleanEdgeMap();
	
	//need to iterate thru all flow ops to figure out targets
	ArrayIterator<FlowOpSlot> *iter=getFlowOpSlotIter();
	while(iter->hasMoreElements())
	{
		if(iter->currentElement().hasOper())
		{
			BaseInstruction* target=iter->currentElement().getPtrToOperation()->getTargetInstr();
			if (target!=0)
			{//output edge
				//sprintf(temp,"inst_%X -> inst_%X [label =\"_%X\" ];\n",(int)this,(int)(target),(int)this);
				sprintf(temp,"inst_%X -> inst_%X;\n",(int)this,(int)(target));
				out+=temp;
				BezierEdge* ie=new BezierEdge();
				ie->dest=TO_INSTR;
				node->edges.SetAt((int)(target),ie); //add it to the map
			}
		}
		iter->nextElement();
	}
	delete iter;
	
	if (node->showUD)
	{
		ArrayIterator<DataOpSlot> *dataIter=getDataOpSlotIter();
		while(dataIter->hasMoreElements())
		{
			showUD(node,this,dataIter->currentElement(),out);
			dataIter->nextElement();
		}
		delete dataIter;
		
		ArrayIterator<ControlOpSlot> *controlIter=getControlOpSlotIter();
		while(controlIter->hasMoreElements())
		{
			showUD(node,this,controlIter->currentElement(),out);
			controlIter->nextElement();
		}
		delete controlIter;
	}
	
	if (node->showDU)
	{
		ArrayIterator<DataOpSlot> *dataIter=getDataOpSlotIter();
		while(dataIter->hasMoreElements())
		{
			showDU(node,this,dataIter->currentElement(),out);
			dataIter->nextElement();
		}
		delete dataIter;
		
		ArrayIterator<ControlOpSlot> *controlIter=getControlOpSlotIter();
		while(controlIter->hasMoreElements())
		{
			showDU(node,this,controlIter->currentElement(),out);
			controlIter->nextElement();
		}
		delete controlIter;
	}
	
}


//this version used by CFGView and HTGView
void NormalInstr::DOTText(GRAPH_NODE_TYPE nodeType, CString& out, BasicBlockNode* from_block)
{
	ASSERT_TRACE(nodeType!=GNT_ILG,"Nah-uh!");
	ASSERT_TRACE(from_block->GetGraphNode(nodeType)->expanded,"No reason to draw this when the block is not expanded.");
	
	GraphNode* node=GetGraphNode(nodeType);
	
	char temp[100];
	ArrayIterator<FlowOpSlot> *i;
	
	node->CleanEdgeMap();
	
	i=getFlowOpSlotIter();
	
	//issue the instruction node first
	sprintf(temp,"inst_%X;\n",(int)this);
	out+=temp; 
	
	//need to iterate thru all flow ops to figure out targets 
	
	//for each target of this instr's flowOps 
	while (i->hasMoreElements())
	{
		if ((i->currentElement()).hasOper() == _YES_)
		{
			BaseInstruction* target=i->currentElement().getPtrToOperation()->getTargetInstr();
			
			if (target!=NULL) //NULL is only returned for the last instruction in the routine (the one that points to 0x00000000)
			{
				if (this==from_block->getLastInstr()) //the last instruction links to the next node
				{
					//there's an edge from this instr to a target in another basic block
					BasicBlockNode* to_block=hackGetTargetBlock(target,from_block);
					//NOSTEMP
					if (to_block==NULL)
					{
						i->nextElement();
						continue;
					}
					
					//now we have INSTR, FROM_BLOCK and TO_BLOCK, we can output the edge definition
					
					//we point to...
					if (!to_block->IsSameTypeExpanded(node->nodeType)) // .. the next block if not expanded
					{
						sprintf(temp,"inst_%X -> blok_%X;\n",(int)this,(int)to_block);
						BezierEdge* ie=new BezierEdge();
						ie->dest=TO_BASICBLOCK;
						node->edges.SetAt((int)to_block,ie); //add it to the map
					}
					else //... the first instruction in the next block if expanded
					{
						sprintf(temp,"inst_%X -> inst_%X;\n",(int)this,(int)(to_block->getFirstInstr()));
						BezierEdge* ie=new BezierEdge();
						ie->dest=TO_INSTR;
						node->edges.SetAt((int)(to_block->getFirstInstr()),ie); //add it to the map
					}
					out+=temp; 
				}
				else
				{
					//output the instruction and then an edge from it to the next instruction
					sprintf(temp,"inst_%X -> inst_%X;\n",(int)this,(int)(this->getFirstSuccOnly()));
					out+=temp;
					BezierEdge* ie=new BezierEdge();
					ie->dest=TO_INSTR;
					node->edges.SetAt((int)(this->getFirstSuccOnly()),ie); //add it to the map
				}
			}
		}
		i->nextElement();
	}
	delete i;
	
	/*
	if (node->showUD)
	{
	//need to iterate thru all data ops to figure out targets
	ArrayIterator<DataOpSlot> *dataIter=getDataOpSlotIter();
	MyLinkedList<BaseOperation*> *defList;
	MyLinkedListIterator<BaseOperation*> *defIter;
	BaseInstruction *target;
	while(dataIter->hasMoreElements())
	{
	if(dataIter->currentElement().hasOper())
	{
    //first operand
    if(dataIter->currentElement().getPtrToOperation()->hasOperand(_SOURCE_1_)==_YES_)
    {
	defList=GetDefList(dataIter->currentElement().getPtrToOperation()->ptrToOperand(_SOURCE_1_));
	defIter=defList->elementsForward();
	while(defIter->hasMoreElements())
	{
	target=defIter->currentElement()->getInstr();
	if(target!=NULL){
	sprintf(temp,"inst_%X -> inst_%X [color = %d];\n",(int)this,(int)(target),EDGE_COLOR_UD);
	out+=temp;
	BezierEdge* ie=new BezierEdge();
	ie->dest=TO_INSTR;
	node->edges.SetAt((int)(target),ie); //add it to the map
	}
	defIter->nextElement();
	}
	delete defIter;
    }
	
	  //second operand
	  if(dataIter->currentElement().getPtrToOperation()->hasOperand(_SOURCE_2_)==_YES_)
	  {
	  defList=GetDefList(dataIter->currentElement().getPtrToOperation()->ptrToOperand(_SOURCE_2_));
	  defIter=defList->elementsForward();
	  while(defIter->hasMoreElements())
	  {
      target=defIter->currentElement()->getInstr();
      if(target!=NULL){
	  sprintf(temp,"inst_%X -> inst_%X [color = %d];\n",(int)this,(int)(target),EDGE_COLOR_UD);
	  out+=temp;
	  BezierEdge* ie=new BezierEdge();
	  ie->dest=TO_INSTR;
	  node->edges.SetAt((int)(target),ie); //add it to the map
      }
      defIter->nextElement();
	  }
	  delete defIter;
	  }
	  
		}
		dataIter->nextElement();
		}
		delete dataIter;
		}
		
		  if (node->showDU)
		  {
		  //need to iterate thru all data ops to figure out targets
		  ArrayIterator<DataOpSlot> *dataIter=getDataOpSlotIter();
		  MyLinkedList<BaseOperation*> *useList;
		  MyLinkedListIterator<BaseOperation*> *useIter;
		  BaseInstruction *target;
		  while(dataIter->hasMoreElements())
		  {
		  if(dataIter->currentElement().hasOper())
		  {
		  if(dataIter->currentElement().getPtrToOperation()->hasOperand(_DEST_, _DEST_LIST_)==_YES_)
		  {
		  useList=GetUseList(dataIter->currentElement().getPtrToOperation()->ptrToOperand(_DEST_, _DEST_LIST_));
		  useIter=useList->elementsForward();
		  while(useIter->hasMoreElements())
		  {
		  target=useIter->currentElement()->getInstr();
		  if(target!=NULL){
		  sprintf(temp,"inst_%X -> inst_%X [color = %d];\n",(int)this,(int)(target),EDGE_COLOR_DU);
		  out+=temp;
		  BezierEdge* ie=new BezierEdge();
		  ie->dest=TO_INSTR;
		  node->edges.SetAt((int)(target),ie); //add it to the map
		  }
		  useIter->nextElement();
		  }
		  delete useIter;
		  }
		  }
		  dataIter->nextElement();
		  }
		  delete dataIter;
		  }
	*/
	
	if (node->showUD)
	{
		ArrayIterator<DataOpSlot> *dataIter=getDataOpSlotIter();
		while(dataIter->hasMoreElements())
		{
			showUD(node,this,dataIter->currentElement(),out);
			dataIter->nextElement();
		}
		delete dataIter;
		
		ArrayIterator<ControlOpSlot> *controlIter=getControlOpSlotIter();
		while(controlIter->hasMoreElements())
		{
			showUD(node,this,controlIter->currentElement(),out);
			controlIter->nextElement();
		}
		delete controlIter;
	}
	
	if (node->showDU)
	{
		ArrayIterator<DataOpSlot> *dataIter=getDataOpSlotIter();
		while(dataIter->hasMoreElements())
		{
			showDU(node,this,dataIter->currentElement(),out);
			dataIter->nextElement();
		}
		delete dataIter;
		
		ArrayIterator<ControlOpSlot> *controlIter=getControlOpSlotIter();
		while(controlIter->hasMoreElements())
		{
			showDU(node,this,controlIter->currentElement(),out);
			controlIter->nextElement();
		}
		delete controlIter;
	}
}

extern bool updateLabel;

void NormalInstr::DrawInfo(GRAPH_NODE_TYPE nodeType, CDC * pDC, CRect& rect)
{
	CRect tempR(rect);
	GraphNode* node=GetGraphNode(nodeType);
	char buf[2000];
	pDC->SetTextColor(node->textColor);
	pDC->SetTextAlign(TA_TOP);
	
	char temp[2000];
	sprintf(temp,"0x%08X",(long)this, buf);
	tempR.OffsetRect(0,-2);
	//pDC->DrawText(node->label,-1,tempR,DT_WORDBREAK);
	pDC->TextOut(tempR.left+2,tempR.top+2,temp);
	
	if ((node->label=="") || (updateLabel==true))
	{
		this->printOpcodesToString(buf);
		sprintf(temp,"%s",buf);
		node->label=temp;
	}
	//pDC->TextOut(rect.left+2,rect.top+2,node->label);
	//tempR.OffsetRect(0,7);
	tempR.top=tempR.top+9;
	tempR.left=tempR.left+2;
	tempR.right=tempR.right-2;
	int height=pDC->DrawText(node->label,-1,tempR,DT_WORDBREAK|DT_END_ELLIPSIS);
	
	if (flags.getFlag(PERC_FLAG)==true)
	{
		//pDC->SelectObject(oldPen);
		//delete newPen;
		//newPen = new CPen(PS_SOLID,2,GREEN);
		//oldPen = pDC->SelectObject(newPen);
		pDC->Rectangle(rect.right-5,rect.top+3,rect.right-3,rect.top+5);
	}
	
}
#endif // of WIN32_GUI


//set this to 1 in order to print out expilcitly all NOPs in the code
int explicit_nops=0;


NormalInstr::~NormalInstr()
{
	
	delete _instrLabel;
	delete _succListIter;
	
	/*	 delete _dataOpList;
	delete _controlOpList;
	delete _flowOpList;
	
	  delete _listOfSuccessorIDs;
	*/
	
#ifdef COUNT_NORMALINSTRS
	_numNormalInstrs--;
#endif
}    

void NormalInstr::initAddDataOp(DataOp *someOpPtr, int pos, bool noCopy)
{
	// This is the first operation to be added to the operation
	// tree. Therefore, make header op point to this opslot.
	
	int listIndex;
	
	listIndex = addDataOpToList(someOpPtr, pos, noCopy);
	
	// now use listIndex to initialize the headerOp.
	
	_opTreeHeader.setIndex(listIndex);
	_opTreeHeader.setType(DATA_OP);
	
}

int NormalInstr::addDataOpToList(DataOp *someOpPtr, int pos, bool noCopy)
{
	if (pos == _INVALID_)
	{
		ArrayIterator<DataOpSlot> *dIter;
		dIter = _dataOpList.elements();
		while (dIter->hasMoreElements())
		{
			if ((dIter->currentElement()).hasOper() == _NO_)
			{
				(dIter->currentElement()).addOperation(someOpPtr, noCopy);
				int temp = dIter->currentIndex();
				delete dIter;
				return temp;
			}
			dIter->nextElement();
		}
		delete dIter;
	}
	else
	{
		_dataOpList.elementAt(pos).addOperation(someOpPtr, noCopy);
		return pos;
	}
	return _INVALID_;
}

void NormalInstr::initAddControlOp(ControlOp *someOpPtr, int pos, bool noCopy)
{
	int listIndex;
	
	listIndex = addControlOpToList(someOpPtr, pos, noCopy);
	
	// See initAddDataOp for further explanations.
	
	_opTreeHeader.setIndex(listIndex);
	_opTreeHeader.setType(CONTROL_OP);
	
}

int NormalInstr::addControlOpToList(ControlOp *someOpPtr, int pos, bool noCopy)
{
	if (pos == _INVALID_)
	{
		ArrayIterator<ControlOpSlot> *cIter;
		cIter = _controlOpList.elements();
		while (cIter->hasMoreElements())
		{
			if ((cIter->currentElement()).hasOper() == _NO_)
			{
				(cIter->currentElement()).addOperation(someOpPtr, noCopy);
				int temp = cIter->currentIndex();
				delete cIter;
				return temp;
			}
			cIter->nextElement();
		}
		delete cIter;
	}
	else
	{
		_controlOpList.elementAt(pos).addOperation(someOpPtr, noCopy);
		return pos;
	}
	return _INVALID_;
}

void NormalInstr::initAddFlowOp(FlowOp *someOpPtr, int pos, bool noCopy)
{
	int listIndex;
	
	listIndex = addFlowOpToList(someOpPtr, pos, noCopy);
	
	// See initAddDataOp for further explanations.
	
	_opTreeHeader.setIndex(listIndex);
	_opTreeHeader.setType(FLOW_OP);
	
}

int NormalInstr::addFlowOpToList(FlowOp *someOpPtr, int pos, bool noCopy)
{
	if (pos == _INVALID_)
	{
		ArrayIterator<FlowOpSlot> *fIter;
		fIter = _flowOpList.elements();
		while (fIter->hasMoreElements())
		{
			if ((fIter->currentElement()).hasOper() == _NO_)
			{
				(fIter->currentElement()).addOperation(someOpPtr, noCopy);
				int temp= fIter->currentIndex();
				delete fIter;
				return temp;
			}
			fIter->nextElement();
		}
		delete fIter;
	}
	else
	{
		_flowOpList.elementAt(pos).addOperation(someOpPtr, noCopy);
		return pos;
	}
	return _INVALID_;
}

int NormalInstr::addOperation(BaseOperation *someOpPtr, int pos, bool noCopy)
{
	if (someOpPtr->isDataOp() == _YES_)
	{
		return addDataOpToList((DataOp *)someOpPtr, pos, noCopy);
	}
	else if (someOpPtr->isControlOp() == _YES_)
	{
		return addControlOpToList((ControlOp *)someOpPtr, pos, noCopy);
	}
	else if (someOpPtr->isFlowOp() == _YES_)
	{
		return addFlowOpToList((FlowOp *)someOpPtr, pos, noCopy);
	}
	return _INVALID_;
}

void NormalInstr::setRootOp(BaseOpSlot *slot)
{
	if(slot==NULL) return;
	if (slot->isDataOpSlot()) {
		setRootOp(DATA_OP, getSlotIndex(slot));
	} else if (slot->isControlOpSlot()) {
		setRootOp(CONTROL_OP, getSlotIndex(slot));
	} else { // slot->isFlowOpSlot
		setRootOp(FLOW_OP, getSlotIndex(slot));
	}
}

BaseOperation* NormalInstr::getPtrToRootOperation()
{
	int type = _opTreeHeader.getType();
	int index = _opTreeHeader.getIndex();
	
	if (type == DATA_OP)
	{
		return (_dataOpList.elementAt(index)).getPtrToOperation();
	}
	else if (type == CONTROL_OP)
	{
		return (_controlOpList.elementAt(index)).getPtrToOperation();
	}
	else if (type == FLOW_OP)
	{
		return (_flowOpList.elementAt(index)).getPtrToOperation();
	}
	else   // if (type == NO_OP)
	{
		// exception. Invalid operation type.
	}
	
	//NOS
	ASSERT_TRACE(0,"Forced EXIT!");
	return NULL;
}

BaseOpSlot & NormalInstr::getRootOpSlot()
{
	int type = _opTreeHeader.getType();
	int index = _opTreeHeader.getIndex();
	
	if (type == DATA_OP)
	{
		return (_dataOpList.elementAt(index));
	}
	else if (type == CONTROL_OP)
	{
		return (_controlOpList.elementAt(index));
	}
	else if (type == FLOW_OP)
	{
		return (_flowOpList.elementAt(index));
	}
	else   // if (type == NO_OP)
	{
		// exception. Invalid operation type.
	}
	
	//NOS
	ASSERT_TRACE(0,"Forced EXIT!");
	return (_dataOpList.elementAt(index));
}

BaseInstruction* NormalInstr::getFirstSucc()
{
	BaseOperation *flowOpPtr;
	
	if (_succListIter != NULL)
	{
	       // error condition.
	       //
		//	       cout << "Warning: Need to delete existing iterator first. Call NormalInstr::deleteSuccIter()" << endl;
	       delete _succListIter;
		   _succListIter = NULL;
	}
	
	BaseOperation *oper;
	BaseInstruction *tgt = NULL;
	
	if ((oper = hasCallRetOp()) != NULL)
	{
		// We do not want to return a pointer to an instruction in another routine.
		// So, we ignore the instr that is the target of a call.
		
		tgt = ((CallRetOp *)oper)->getTargetInstr();
	}
	BaseInstruction *succ;
	
	_succListIter = _flowOpList.elements();
	while (_succListIter->hasMoreElements())
	{
		flowOpPtr = (_succListIter->currentElement()).getPtrToOperation();
		
		if (flowOpPtr != NULL)
		{
			succ = flowOpPtr->getTargetInstr();
			
			if (succ != tgt) // i.e. if succ is not the target of a call/ret
				return succ;
		}
		_succListIter->nextElement();
	}
	
	return NULL;
	
	//NOS
	ASSERT_TRACE(0,"Forced EXIT!");
	return NULL;
}

BaseInstruction* NormalInstr::getNextSucc()
{
	BaseOperation *flowOpPtr;
	
	if (_succListIter == NULL)
	{
		// error condition.
		//
		cout << "Panic: Need to create iterator first. Call NormalInstr::getFirstSucc()" << endl;
		assert(0);
	}
	
	BaseOperation *oper;
	BaseInstruction *tgt = NULL;
	
	if ((oper = hasCallRetOp()) != NULL)
	{
		// We do not want to return a pointer to an instruction in another routine.
		// So, we ignore the instr that is the target of a call.
		
		tgt = ((CallRetOp *)oper)->getTargetInstr();
	}
	BaseInstruction *succ;
	
	_succListIter->nextElement();
	while (_succListIter->hasMoreElements())
	{
		flowOpPtr = (_succListIter->currentElement()).getPtrToOperation();
		if (flowOpPtr != NULL)
		{
			succ = flowOpPtr->getTargetInstr();
			if (succ != tgt) // i.e. if succ is not the target of a call/ret
				return succ;
		}
		_succListIter->nextElement();
	}
	
	return NULL;
}

BaseInstruction* NormalInstr::getDefaultTargetInstr()
{
	BaseOperation *tempOp;
	
	try 
	{
		if ((tempOp = _flowOpList.elementAt(numFlowOpsPerInstr
			- 1).getPtrToOperation()) == NULL) 
		{
			// This could mean that the default next instr
			// has not been set yet, or that it is the last
			// instr. 
			
			cout << "Warning: Returning NULL as result of NormalInstr::getDefaultTargetInstr()" << endl; 
			
			return NULL;
		}
		
		if (!tempOp->isFlowOp())
		{
			cout << "Panic: Expected flow op, received something else. In NormalInstr::getDefaultTargetInstr()" << endl;
			assert(0);
		}
		return (((FlowOp *)tempOp)->getTargetInstr());
	}
	catch(Exception *e)
	{
		cout << "Panic: Trouble encountered in NormalInstr::getDefaultTargetInstr" << endl;
#ifdef WIN32
		cout << "Exception: " << *e;
#endif
		assert(0);
	}
	
	ASSERT(0);
	return NULL;
}

#ifdef WIN32_GUI
void NormalInstr::printToString(char *buf)
{
	ArrayIterator<DataOpSlot> *dataIter;
	
	buf[0]='\0';
	dataIter = _dataOpList.elements();
	while (dataIter->hasMoreElements())
	{
		if(dataIter->currentElement().hasOper())
		{
			dataIter->currentElement().getPtrToOperation()->printToString(buf);
			strcat(buf,"\n");
		}
		dataIter->nextElement();
	}
	
	//now the control ops
	ArrayIterator<ControlOpSlot> *ctrlIter;
	
	buf[0]='\0';
	ctrlIter = _controlOpList.elements();
	while (ctrlIter->hasMoreElements())
	{
		if(ctrlIter->currentElement().hasOper())
		{
			ctrlIter->currentElement().getPtrToOperation()->printToString(buf);
			strcat(buf,"\n");
		}
		ctrlIter->nextElement();
	}
	
	//now the flow ops
	ArrayIterator<FlowOpSlot> *flowIter;
	
	buf[0]='\0';
	flowIter = _flowOpList.elements();
	while (flowIter->hasMoreElements())
	{
		if(flowIter->currentElement().hasOper())
		{
			flowIter->currentElement().getPtrToOperation()->printToString(buf);
			strcat(buf,"\n");
		}
		flowIter->nextElement();
	}
}

void NormalInstr::printOpcodesToString(char *buf, char* delim )
{
	buf[0]='\0';
	
	ArrayIterator<DataOpSlot> *dataIter;
	BaseOperation *op;
	int i=0;
	
	//data ops
	dataIter = _dataOpList.elements();
	while (dataIter->hasMoreElements())
	{
		if(dataIter->currentElement().hasOper())
		{
			op=dataIter->currentElement().getPtrToOperation();
			if(op->isNormalOp())
			{
				//strcat(buf,globalOpCodeTable[((NormalOp*)op)->getOpCodeIndex()].opName());
				MyString str;
				op->getOpcodeName(str);
				strcat(buf,str.getStr());
				if (delim==NULL)
					strcat(buf," ");
				else
					strcat(buf,delim);
			}
		}
		dataIter->nextElement();
	}
	
	//control ops
	ArrayIterator<ControlOpSlot> *ctrlIter;
	
	ctrlIter = _controlOpList.elements();
	while (ctrlIter->hasMoreElements())
	{
		if(ctrlIter->currentElement().hasOper())
		{
			op=ctrlIter->currentElement().getPtrToOperation();
			//strcat(buf,globalOpCodeTable[((NormalOp*)op)->getOpCodeIndex()].opName());
			MyString str;
			op->getOpcodeName(str);
			strcat(buf,str.getStr());
			if (delim==NULL)
				strcat(buf," ");
			else
				strcat(buf,delim);
		}
		ctrlIter->nextElement();
	}
	
	//flow ops
	ArrayIterator<FlowOpSlot> *flowIter;
	
	flowIter = _flowOpList.elements();
	while (flowIter->hasMoreElements())
	{
		if(flowIter->currentElement().hasOper())
		{
			op=flowIter->currentElement().getPtrToOperation();
			//strcat(buf,globalOpCodeTable[((NormalOp*)op)->getOpCodeIndex()].opName());
			MyString str;
			op->getOpcodeName(str);
			strcat(buf,str.getStr());
			
			//   //NOSTEMP
			//   JumpOp* jOp=(JumpOp*)op;
			//   char tgt[20];
			//   sprintf(tgt,"(0x%08X)", (long)(jOp->getTargetInstr()));
			//   strcat(buf,tgt);
			//   //NOSTEMP
			
			if (delim==NULL)
				strcat(buf," ");
			else
				strcat(buf,delim);
		}
		flowIter->nextElement();
	}
}


void NormalInstr::DisplayProperties(BasePropPage* dialog)
{
	InstrPropPage* page=dynamic_cast<InstrPropPage*>(dialog);
	
	ASSERT(page!=NULL);
	
	ArrayIterator<DataOpSlot> *dataIter;
	char buf[2000];
	int i;
	
	sprintf(buf,"0x%08X",this);
	page->m_Address=buf;
	
	page->m_OpList.ResetContent();
	
	dataIter = _dataOpList.elements();
	i=0;
	while (dataIter->hasMoreElements()){
		if(dataIter->currentElement().hasOper()){
			buf[0]='\0';
			dataIter->currentElement().getPtrToOperation()->printToString(buf);
			char str[20];
			strcat(buf,"\t\t\tSlot: ");strcat(buf, itoa(i,str,10));
			int entry=page->m_OpList.AddString((const char *)(&buf));
			assert((entry!=LB_ERRSPACE) && (entry!=LB_ERRSPACE));
			page->m_OpList.SetItemData(entry, (long)(dataIter->currentElement().getPtrToOperation()));
		}
		dataIter->nextElement();
		i++;
	}
	
	ArrayIterator<ControlOpSlot> *controlIter;
	controlIter = _controlOpList.elements();
	while (controlIter->hasMoreElements()){
		if(controlIter->currentElement().hasOper()){
			buf[0]='\0';
			controlIter->currentElement().getPtrToOperation()->printToString(buf);
			char str[20];
			strcat(buf,"\t\t\tSlot: ");strcat(buf, itoa(i,str,10));
			int entry=page->m_OpList.AddString((const char *)(&buf));
			assert((entry!=LB_ERRSPACE) && (entry!=LB_ERRSPACE));
			page->m_OpList.SetItemData(entry, (long)(controlIter->currentElement().getPtrToOperation()));
		}
		controlIter->nextElement();
	}
	
	ArrayIterator<FlowOpSlot> *flowIter;
	flowIter = _flowOpList.elements();
	while (flowIter->hasMoreElements()){
		if(flowIter->currentElement().hasOper()){
			buf[0]='\0';
			flowIter->currentElement().getPtrToOperation()->printToString(buf);
			char str[20];
			strcat(buf,"\t\t\tSlot: ");strcat(buf, itoa(i,str,10));
			int entry=page->m_OpList.AddString((const char *)(&buf));
			assert((entry!=LB_ERRSPACE) && (entry!=LB_ERRSPACE));
			page->m_OpList.SetItemData(entry, (long)(flowIter->currentElement().getPtrToOperation()));
		}
		flowIter->nextElement();
	}
	
	
	delete dataIter;
	delete controlIter;
	delete flowIter;
	
	page->UpdateData(false);
}

#endif //WIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUI

extern RenameTable globalRenameTable;

void setLowestSSAValue(BaseArgument *argPtr)
{
	if (argPtr->isRegisterArg() == _NO_)
		return;
	
	if (argPtr->isRealRegArg() == _NO_)
	{
		// currently we do not handle this situation.
		cout << "Warning: renameValue recieved a non realreg arg type" << endl;
		return;
	}
	
	BaseArgProperty *ssaProp = argPtr->getPtrToProperty(_SSA_);
	if (ssaProp == NULL)
	{
		// error condition.
		cout << "Panic: ssaproperty of argument is null (in renameValue)" << endl;
		assert(0);
	}
	
	globalRenameTable.setLowestValue(argPtr, ssaProp);
	
}


void renameValue(BaseArgument *argPtr)
{
	if (argPtr->isRegisterArg() == _NO_)
		return;
	
	if (argPtr->isRealRegArg() == _NO_)
	{
		// currently we do not handle this situation.
		cout << "Warning: renameValue recieved a non realreg arg type" << endl;
		return;
	}
	
	BaseArgProperty *ssaProp = argPtr->getPtrToProperty(_SSA_);
	if (ssaProp == NULL)
	{
		// error condition.
		cout << "Panic: ssaproperty of argument is null (in renameValue)" << endl;
		assert(0);
	}
	
	globalRenameTable.renameValue(argPtr, ssaProp);
	
}

void renameValue(BaseArgument *argPtr, ValuesSET & in)
{
	if (argPtr->isRegisterArg() == _NO_)
		return;
	
	if (argPtr->isRealRegArg() == _NO_)
	{
		// currently we do not handle this situation.
		cout << "Warning: renameValue recieved a non realreg arg type" << endl;
		return;
	}
	
	BaseArgProperty *ssaProp = argPtr->getPtrToProperty(_SSA_);
	if (ssaProp == NULL)
	{
		// error condition.
		cout << "Panic: ssaproperty of argument is null (in renameValue)" << endl;
		assert(0);
	}
	int newNum = in.getEntry(argPtr);
	if (newNum == -1) // i.e there is no entry in the set for that particular register
		return;
	((SSAArgProperty *)ssaProp)->setSSANum(newNum);
}



void printIndent(int);

// extern Simple_VLIW_Instr *simpleVLIWInstrTemplate;

#ifdef COUNT_NORMALINSTRS
int NormalInstr::_numNormalInstrs = 0;	
#endif

#define NUM_OF_OPS_PER_INSTR 10

// These variables will be assigned in the BuildSystem function for the architecture
int numDataOpsPerInstr;
int numControlOpsPerInstr;
int numFlowOpsPerInstr;
//
// See normalInstr.h for an explanation of these variables.

NormalInstr::NormalInstr() : _dataOpList(numDataOpsPerInstr),
_controlOpList(numControlOpsPerInstr), _flowOpList(numFlowOpsPerInstr),
_unifiableOps(){
	DataOpSlot dc;
	int i;
	
	setID(-1);
	
	for (i = 0; i < numDataOpsPerInstr; i++)
	{
		_dataOpList.insertElement(dc);
	}
	
	ControlOpSlot cc;
	for (i = 0; i < numControlOpsPerInstr; i++)
	{
		_controlOpList.insertElement(cc);
	}
	
	FlowOpSlot fc;
	for (i = 0; i < numFlowOpsPerInstr; i++)
	{
		_flowOpList.insertElement(fc);
	}
	
	_instrLabel = NULL;
	
	_succListIter = NULL;
	
#ifdef COUNT_NORMALINSTRS
	_numNormalInstrs++;	
#endif
}

NormalInstr::NormalInstr(NormalInstr & copyInstr):
_dataOpList(copyInstr._dataOpList),
_flowOpList(copyInstr._flowOpList),
_controlOpList(copyInstr._controlOpList),
_opTreeHeader(copyInstr._opTreeHeader),
_unifiableOps()
{
	
	setID(-1);
	_instrLabel = NULL;
	_succListIter = NULL;
	
	
	// First we copy the label, if any.
	
	if (copyInstr._instrLabel != NULL)
		_instrLabel = (copyInstr._instrLabel)->copyBody();
	
	
	// Next, we set the headerOp to point to the correct operation.
	
	setRootOp(copyInstr._opTreeHeader.getType(), copyInstr._opTreeHeader.getIndex());
	
	// Then, we set the tree pointers : next for dataOpSlot; true,false for controlOpSlot
	
	ArrayIterator<DataOpSlot> *dIter;
	BaseOpSlot *tempSlot;
	
	dIter = (copyInstr._dataOpList).elements();
	while (dIter->hasMoreElements())
	{
		if (dIter->currentElement().hasOper())
		{
			if(dIter->currentElement().getPtrToNextSlot()!=NULL){
				tempSlot = getCorespSlot(dIter->currentElement().getPtrToNextSlot(), &(copyInstr));
				(_dataOpList.elementAt(dIter->currentIndex())).setNext(tempSlot);
			}
			else
				cout << "Warning: A data op has no next flow op" << endl;
		}
		dIter->nextElement();
	}
	delete dIter;
	
	
	ArrayIterator<ControlOpSlot> *cIter;
	cIter = (copyInstr._controlOpList).elements();
	while (cIter->hasMoreElements())
	{
		if (cIter->currentElement().hasOper())
		{
			tempSlot = getCorespSlot((cIter->currentElement()).getPtrToTrueBranch(), &(copyInstr));
			(_controlOpList.elementAt(cIter->currentIndex())).setTrueBranch(tempSlot);
			tempSlot = getCorespSlot((cIter->currentElement()).getPtrToFalseBranch(), &(copyInstr));
			(_controlOpList.elementAt(cIter->currentIndex())).setFalseBranch(tempSlot);
		}
		cIter->nextElement();
	}
	delete cIter;
    
#ifdef COUNT_NORMALINSTRS
	_numNormalInstrs++;	
#endif
}

BaseOpSlot * NormalInstr::getPtrToSlot(BaseOperation *op)
{
	
	BaseOpSlot *slot, *retPtr;
	if (op->isDataOp())
	{
		ArrayIterator<DataOpSlot> *dIter;
		dIter = _dataOpList.elements();
		
		while (dIter->hasMoreElements())
		{
			slot = & (dIter->currentElement());
			if (slot->hasOper())
			{
				if (op == (slot->getPtrToOperation()))
				{
					retPtr = slot;
					break;
				}
			}
			dIter->nextElement();
		}
		delete dIter;
	}
	else if (op->isControlOp())
	{
		ArrayIterator<ControlOpSlot> *cIter;
		cIter = _controlOpList.elements();
		
		while (cIter->hasMoreElements())
		{
			slot = & (cIter->currentElement());
			if (slot->hasOper())
			{
				if (op == (slot->getPtrToOperation()))
				{
					retPtr = slot;
					break;
				}
			}
			cIter->nextElement();
		}
		delete cIter;
	}
	else if (op->isFlowOp())
	{
		ArrayIterator<FlowOpSlot> *fIter;
		fIter = _flowOpList.elements();
		
		while (fIter->hasMoreElements())
		{
			slot = & (fIter->currentElement());
			if (slot->hasOper())
			{
				if (op == (slot->getPtrToOperation()))
				{
					retPtr = slot;
					break;
				}
			}
			fIter->nextElement();
		}
		delete fIter;
	}
	
	return retPtr;
}
BaseOperation * NormalInstr::getPtrToOperation(IDType index)
{
	BaseOperation *retPtr;
	
	ArrayIterator<DataOpSlot> *dataOpListIterator;
	
	dataOpListIterator = _dataOpList.elements();
	
	while (dataOpListIterator->hasMoreElements())
	{
		retPtr = (dataOpListIterator->currentElement()).getPtrToOperation();
		//
		//
		// Note: If retPtr = NULL, then the next portion of code will seg. fault.
		// This has to be taken care of.
		//
		//
		if (retPtr->idValue() == index)
		{
			return  retPtr;
			//
			// Memory leak!!
		}
		dataOpListIterator->nextElement();
	}
	
	ArrayIterator<ControlOpSlot> *controlOpListIterator;
	
	controlOpListIterator = _controlOpList.elements();
	
	while (controlOpListIterator->hasMoreElements())
	{
		retPtr = (controlOpListIterator->currentElement()).getPtrToOperation();
		
		if (retPtr->idValue() == index)
			return retPtr;
	}
	
	ArrayIterator<FlowOpSlot> *flowOpListIterator;
	
	flowOpListIterator = _flowOpList.elements();
	
	while (flowOpListIterator->hasMoreElements())
	{
		retPtr = (flowOpListIterator->currentElement()).getPtrToOperation();
		
		if (retPtr->idValue() == index)
			return retPtr;
	}
	
	return _NULL_;
}

void NormalInstr::initListOfSuccessorIDs()
{
	ArrayIterator<FlowOpSlot> * flowOpListIterator;
	//IDType nextSuccID;
	
	flowOpListIterator = _flowOpList.elements();
	while (flowOpListIterator->hasMoreElements())
	{
	/*	 nextSuccID =
	((JumpOp *)((flowOpListIterator->currentElement()).getPtrToOperation()))->getTargetInstrID();
	
	  _listOfSuccessorIDs.append(nextSuccID);
		*/	 
		flowOpListIterator->nextElement();
	}
	
	delete flowOpListIterator;
}

void NormalInstr::print(ostream& out)
{
	out << ">>>>> " << this << " >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << endl;
	
	// out<<"ID:"<< _uniqueID<<" ";
	if (_instrLabel != NULL)
		((LabelArg *)_instrLabel)->print(out);
	out << endl;
	ArrayIterator<DataOpSlot> *dataIter;
	
	dataIter = _dataOpList.elements();
	
	out << "Data Ops:" << endl;
	
	while (dataIter->hasMoreElements())
	{
		out<<"  ";
		dataIter->currentElement().print(out);
		dataIter->nextElement();
	}
	ArrayIterator<ControlOpSlot> *controlIter;
	
	controlIter = _controlOpList.elements();
	
	out << "Control Ops:" << endl;
	
	while (controlIter->hasMoreElements())
	{
		out<<"  ";
		controlIter->currentElement().print(out);
		controlIter->nextElement();
	}
	/*
	ArrayIterator<FlowOpSlot> *flowIter;
	
	  flowIter = _flowOpList.elements();
	  
		out << "Flow Ops:" << endl;
		
		  while (flowIter->hasMoreElements())
		  {
		  flowIter->currentElement().print(out);
		  flowIter->nextElement();
		  }
		  
			delete flowIter;
	*/
	delete dataIter;
	delete controlIter;
	
	out << "<<<<< " << this << " <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;
}

void NormalInstr::shortPrint(ostream& out)
{
	if (_instrLabel != NULL){
		((LabelArg *)_instrLabel)->print(out);
		out << endl;
	}
	
	out << "(* " << this << " *) ";
	
	ArrayIterator<DataOpSlot> *dataIter;
	
	dataIter = _dataOpList.elements();
	
	while (dataIter->hasMoreElements())
	{
		dataIter->currentElement().shortPrint(out);
        dataIter->nextElement();
	}
	ArrayIterator<ControlOpSlot> *controlIter;
	
	controlIter = _controlOpList.elements();
	
	while (controlIter->hasMoreElements())
	{
		controlIter->currentElement().shortPrint(out);
		controlIter->nextElement();
	}
	delete dataIter;
	delete controlIter;
	
	// SAH, Mon, Dec 06, 99. Temporary debug code
	
	// /***********************
	ArrayIterator<FlowOpSlot> *flowIter;
	
	flowIter = _flowOpList.elements();
	
	while (flowIter->hasMoreElements())
	{
		flowIter->currentElement().shortPrint(out);
		flowIter->nextElement();
	}
	delete flowIter;
	// ************************/
	
	// End temporary debug code.
	
	out << endl;
}

void NormalInstr::printProperties(ostream & out, PropNameList & pL)
{
	if (_instrLabel != NULL){
		((LabelArg *)_instrLabel)->print(out);
		out << endl;
	}
	BaseInstruction::printProperties(out, pL);
	
	out << "(* " << this << " *) ";
	
	ArrayIterator<DataOpSlot> *dataIter;
	
	dataIter = _dataOpList.elements();
	
	while (dataIter->hasMoreElements())
	{
		(dataIter->currentElement()).printProperties(out, pL);
		dataIter->nextElement();
	}
	ArrayIterator<ControlOpSlot> *controlIter;
	
	controlIter = _controlOpList.elements();
	
	while (controlIter->hasMoreElements())
	{
		controlIter->currentElement().printProperties(out, pL);
		controlIter->nextElement();
	}
	delete dataIter;
	delete controlIter;
	
	BaseInstruction::printProperties(out, pL);
	out << endl;
}

//the mask is in fact the BBid of the BB where this instruction resides, we
//can use it in the computation of PhiInstr.
void NormalInstr::setTargetMask(int mask)
{
	ArrayIterator<FlowOpSlot> *flowIter;
	
	flowIter = _flowOpList.elements();
	
	while (flowIter->hasMoreElements()) {
		if (flowIter->currentElement().hasOper())
			((FlowOp *)((flowIter->currentElement()).getPtrToOperation()))->setTargetMask(mask);
		flowIter->nextElement();
	}
}



int NormalInstr::countDataOps(void){
	ArrayIterator<DataOpSlot> *iter;
	int rez=0;
	iter=_dataOpList.elements();
	while(iter->hasMoreElements()){
		if(iter->currentElement().hasOper())
			rez++;
		iter->nextElement();
	}  
	delete iter;
	return rez;
}


int NormalInstr::countControlOps(void){
	ArrayIterator<ControlOpSlot> *iter;
	int rez=0;
	iter=_controlOpList.elements();
	while(iter->hasMoreElements()){
		if(iter->currentElement().hasOper())
			rez++;
		iter->nextElement();
	}  
	delete iter;
	return rez;
}

//
// Note: This method has a lot in common with NormalInstr::NormalInstr(NormalInstr &)
// The same code is in two places because, I first wrote this method and then re-wrote 
// the constructor by copying segments of this code. 
// 
// I think that this method can be re-written (to call NormalInstr::NormalInstr) without 
// much trouble (and will be re-written as soon as I get this to stabilize).

BaseInstruction* NormalInstr::copyBody()
{
	BaseInstruction *retPtr = new NormalInstr();
	BaseOperation *tempOpPtr;
	NormalInstr *normretPtr = (NormalInstr*)retPtr;
	
	// First we copy the label, if any.
	
	if (_instrLabel != NULL)
		normretPtr->_instrLabel = _instrLabel->copyBody();
	
	// Next we copy each list : DataOpList, ControlOpList, FlowOpList.
	
	ArrayIterator<DataOpSlot> *dIter;
	dIter = _dataOpList.elements();
	while (dIter->hasMoreElements())
	{
		if (dIter->currentElement().hasOper())
		{
			tempOpPtr = (dIter->currentElement().getPtrToOperation())->copyBody();
			normretPtr->addDataOpToList((DataOp*)tempOpPtr, dIter->currentIndex());
		}
		dIter->nextElement();
	}
	delete dIter;
	
	ArrayIterator<ControlOpSlot> *cIter;
	cIter = _controlOpList.elements();
	while (cIter->hasMoreElements())
	{
		if (cIter->currentElement().hasOper())
		{
			tempOpPtr = (cIter->currentElement().getPtrToOperation())->copyBody();
			normretPtr->addControlOpToList((ControlOp*)tempOpPtr, cIter->currentIndex());
		}
		cIter->nextElement();
	}
	delete cIter;
	
	ArrayIterator<FlowOpSlot> *fIter;
	fIter = _flowOpList.elements();
	while (fIter->hasMoreElements())
	{
		if (fIter->currentElement().hasOper())
		{
			tempOpPtr = (fIter->currentElement().getPtrToOperation())->copyBody();
			normretPtr->addFlowOpToList((FlowOp*)tempOpPtr, fIter->currentIndex());
		}
		fIter->nextElement();
	}
	delete fIter;
	
	// Next, we set the headerOp to point to the correct operation.
	
	normretPtr->setRootOp(_opTreeHeader.getType(), _opTreeHeader.getIndex());
	
	// Then, we set the tree pointers : next for dataOpSlot; true,false for controlOpSlot
	
	BaseOpSlot *tempSlot;
	
	dIter = _dataOpList.elements();
	while (dIter->hasMoreElements())
	{
		if (dIter->currentElement().hasOper())
		{
			tempSlot = normretPtr->getCorespSlot(dIter->currentElement().getPtrToNextSlot(), this);
			(normretPtr->_dataOpList.elementAt(dIter->currentIndex())).setNext(tempSlot);
		}
		dIter->nextElement();
	}
	delete dIter;
	
	cIter = _controlOpList.elements();
	while (cIter->hasMoreElements())
	{
		if (cIter->currentElement().hasOper())
		{
			tempSlot = normretPtr->getCorespSlot((cIter->currentElement()).getPtrToTrueBranch(), this);
			(normretPtr->_controlOpList.elementAt(cIter->currentIndex())).setTrueBranch(tempSlot);
			tempSlot = normretPtr->getCorespSlot((cIter->currentElement()).getPtrToFalseBranch(), this);
			(normretPtr->_controlOpList.elementAt(cIter->currentIndex())).setFalseBranch(tempSlot);
		}
		cIter->nextElement();
	}
	delete cIter;
	
	// Currently, we do not copy any properties of the instruction.
	/*
	MyLinkedList<PropertyNames> propList;
	propList.appendElement(<insert_property_name_here>
	
	  (retPtr->getPropertyList()).makeACopyOf(_propertyPtr, propList);
	*/
	
	return retPtr;
}

void NormalInstr::setNextInstr(BaseInstruction *instr, int flowOpNum)
{
	BaseOperation *correctFlowOp = NULL;
	ArrayIterator<FlowOpSlot> *iter;
	iter = _flowOpList.elements();
	
	int skipToNext = 0;
	
	if (flowOpNum == _DEFAULT_NUM_) // i.e the instr has only one flow op.
	{
		// if it is a callret op, do not change the first flow op, cos it will
		// point to the instr in the called routine.
		//
		if (this->hasCallRetOp() != NULL)
		{
			skipToNext = 1;
		}
		
		while (iter->hasMoreElements())
		{
			if ((iter->currentElement()).hasOper())
			{
				if (skipToNext) // then go to the next flow op, i.e. we assume that the first flow op points to a diff routine.
				{
					skipToNext = 0;
				}
				else
				{
					correctFlowOp = (iter->currentElement()).getPtrToOperation();
					break;
				}
			}
			iter->nextElement();
		}
	}
	else if (flowOpNum == _LAST_NUM_)
	{
		int last = 1;
		while (iter->hasMoreElements())
		{
			if (((iter->currentElement()).hasOper()) && (last == numFlowOpsPerInstr))
			{
				correctFlowOp = (iter->currentElement()).getPtrToOperation();
				break;
			}
			else 
				last++;
			
			iter->nextElement();
		}
	}
	delete iter;
	
	((FlowOp*)correctFlowOp)->setTargetInstr(instr);
}

void NormalInstr::setTargetInstr(BaseInstruction *instr){
	ArrayIterator<FlowOpSlot> *iter;
	JumpOp * jump;
	iter=_flowOpList.elements();
	while(iter->hasMoreElements()){
		if(!iter->currentElement().hasOper()){
			break;
		}
		iter->nextElement();
	}
	// 
	// ***********Important***************
	//
	// This next piece of code is "wierd".
	// 
	if(iter->hasMoreElements() && !iter->currentElement().hasOper()){
		jump=new JumpOp();
		jump->setTargetInstr(instr);
		iter->currentElement().addOperation(jump);
		ArrayIterator<FlowOpSlot>*fiter;
		fiter=_flowOpList.elements();
		while(fiter->hasMoreElements()){
			if(fiter->currentElement().hasOper() && 
				(fiter->currentElement().getPtrToNextSlot())->isFlowOpSlot()){
				fiter->currentElement().addOperation(jump);
			}
			fiter->nextElement();
		}
		delete fiter;
	}
	delete iter;  
}

void NormalInstr::setTargetInstrForAllFlowOps(BaseInstruction *instr){
	ArrayIterator<FlowOpSlot> *iter;
	iter=_flowOpList.elements();
	while(iter->hasMoreElements()){
		if(!iter->currentElement().hasOper()){
			(iter->currentElement().getPtrToOperation())->setTargetInstr(instr);
		}
		iter->nextElement();
	}
	delete iter;
}

int NormalInstr::getDataSlotIndex(DataOpSlot *slot,Array<DataOpSlot> &slotList){
	ArrayIterator<DataOpSlot> *iter;
	int rez;
	iter=slotList.elements();
	while( iter->hasMoreElements()){
		if(&(iter->currentElement())==slot){
			rez=iter->currentIndex();
			delete iter;
			return rez;
		}
		iter->nextElement();
	}
	delete iter;
	return -1;
}

int NormalInstr::getControlSlotIndex(ControlOpSlot *slot,Array<ControlOpSlot> &slotList){
	ArrayIterator<ControlOpSlot> *iter;
	int rez;
	iter=slotList.elements();
	while( iter->hasMoreElements()){
		if(&(iter->currentElement())==slot){
			rez=iter->currentIndex();
			delete iter;
			return rez;
		}
		iter->nextElement();
	}
	delete iter;
	return -1;
}

int NormalInstr::getFlowSlotIndex(FlowOpSlot *slot,Array<FlowOpSlot> &slotList){
	ArrayIterator<FlowOpSlot> *iter;
	int rez;
	iter=slotList.elements();
	while( iter->hasMoreElements()){
		if(&(iter->currentElement())==slot){
			rez=iter->currentIndex();
			delete iter;
			return rez;
		}
		iter->nextElement();
	}
	delete iter;
	return -1;
}


BaseOpSlot *NormalInstr::getCorespSlot(BaseOpSlot * slot,NormalInstr * instr){
	int i;
	if((i=getDataSlotIndex((DataOpSlot*)slot,instr->_dataOpList))!=-1){
		return &(_dataOpList.elementAt(i));
	}
	if((i=getControlSlotIndex((ControlOpSlot*)slot,instr->_controlOpList))!=-1){
		return &(_controlOpList.elementAt(i));
	}
	if((i=getFlowSlotIndex((FlowOpSlot*)slot,instr->_flowOpList))!=-1){
		return &(_flowOpList.elementAt(i));
	}
	assert(0);
	return NULL;
}

int NormalInstr::getSlotIndex(BaseOpSlot *slot)
{
	if (slot->isDataOpSlot())
		return getDataSlotIndex((DataOpSlot*)slot, _dataOpList);
	if (slot->isControlOpSlot())
		return getControlSlotIndex((ControlOpSlot*)slot, _controlOpList);
	if (slot->isFlowOpSlot())
		return getFlowSlotIndex((FlowOpSlot*)slot, _flowOpList);
	return -1;
}


//the physical to virtual slot mappings
struct slot_type{
	int nr;
	SlotType type;
};
extern map<int,set<slot_type*>*> slot_mappings;

int find_slot(set<slot_type*>*slot_list,int n,SlotType t){
	set<slot_type*>::iterator i;
	//for all the slots in the slot_list
	for(i=slot_list->begin();i!=slot_list->end();i++){
		if((*i)->nr==n && (*i)->type==t){
			return 1;
		}
	}
	return 0;
}


//set this to 1 in order to print using phyisical slot mappings 
int PhysicalSlotPrinting=0;


void NormalInstr::print(int indent){
	printIndent(indent);
	int i;
	int j;
	ArrayIterator<DataOpSlot> *dataIter;
	ArrayIterator<ControlOpSlot> *controlIter;
	map<int,set<slot_type*>*>::iterator k;
	set<slot_type*> *slot_list;

	cout << "--i"<< idValue() << "----------------------------------------";
	if (_instrLabel != NULL)
	{
		printIndent(indent);
		_instrLabel->print();
	}
		cout << " (* " << this << " *) ";
	if(!PhysicalSlotPrinting){
		//normal printing, each virtual slot in order
		if(0){
			cout << ", UniOps: ";
			MyLinkedListIterator<UnifiableOp*> *iter;
			iter=_unifiableOps.elementsForward();
			while(iter->hasMoreElements()){
				BaseInstruction *instr=iter->currentElement()->oper->getInstr();
				if(instr!=NULL){
					cout << instr->idValue();
				}
				iter->nextElement();
			}
		}
		dataIter = _dataOpList.elements();
		i=0;
		while (dataIter->hasMoreElements()){
			if(dataIter->currentElement().hasOper()){
				dataIter->currentElement().getPtrToOperation()->print(indent);
				//  cout << "\tSlot: " << i;
			}
			else if(explicit_nops){
				for(j=0;j<indent;j++) cout << " ";
				cout << endl << "  : NOP" ;
			}
			dataIter->nextElement();
			i++;
		}
		
		controlIter = _controlOpList.elements();
		i=0;
		while (controlIter->hasMoreElements()){
			if(controlIter->currentElement().hasOper()){
				controlIter->currentElement().getPtrToOperation()->print(indent);
				// cout << "\tSlot: " << i;
			}
			/*else if(explicit_nops){
			for(j=0;j<indent;j++) cout << " ";
			cout << endl << "  : NOP" ;
		}*/
			controlIter->nextElement();
			i++;
		}
	}
	else{
		//new printing, taking the physical slots, and the physical to virtual slot mapping,
		//and printing the operations in order of the physical slots
		int found=0;
		//for each physical slot
		for(k=slot_mappings.begin();k!=slot_mappings.end();k++){
			slot_list=(*k).second;
			dataIter = _dataOpList.elements();
			found=0;
			i=0;
			//for each virtual data slot
			while (dataIter->hasMoreElements()){
				//if this virtual slot is in the mapping for this current physical slot
				if(find_slot(slot_list,i,DATA_OP_SLOT)){
					//if(slot_list->find(i)!=slot_list->end() && (*(slot_list->find(i)))->type==DATA_OP_SLOT){
					//if found, then print this operation
					if(dataIter->currentElement().hasOper()){
						found=1;
						dataIter->currentElement().getPtrToOperation()->print(indent);
						//  cout << "\tSlot: " << i;
					}
				}
				dataIter->nextElement();
				i++;
			}
			
			controlIter = _controlOpList.elements();
			i=0;
			while (controlIter->hasMoreElements()){
				//if this virtual slot is in the mapping for this current physical slot
				if(find_slot(slot_list,i,CONTROL_OP_SLOT)){
					if(controlIter->currentElement().hasOper()){
						found=1;
						controlIter->currentElement().getPtrToOperation()->print(indent);
						// cout << "\tSlot: " << i;
					}
				}
				controlIter->nextElement();
				i++;
			}
			if(explicit_nops && !found){
				for(j=0;j<indent;j++) cout << " ";
				cout << endl << "  : NOP" ;
			}
		}
	}
	delete dataIter;
	delete controlIter;
	//	delete flowIter;
}



void NormalInstr::printC(int indent){
	// printIndent(indent);
	int i;
	if (_instrLabel != NULL)
	{
		_instrLabel->print();
		cout << ":" << endl;
	}

	{//GG: july 2000, print out the location of the instructions
		int loc;
		loc=this->getLoc();
		printIndent(indent);
		cout << "/* Location: " << loc << " */" << endl;
	}
	
	if(0){
		cout << ", UniOps: ";
		MyLinkedListIterator<UnifiableOp*> *iter;
		iter=_unifiableOps.elementsForward();
		while(iter->hasMoreElements()){
			BaseInstruction *instr=iter->currentElement()->oper->getInstr();
			if(instr!=NULL){
				cout << instr->idValue();
			}
			iter->nextElement();
		}
	}
	ArrayIterator<DataOpSlot> *dataIter;
	dataIter = _dataOpList.elements();
	i=0;
	while (dataIter->hasMoreElements()){
		if(dataIter->currentElement().hasOper()){
			dataIter->currentElement().getPtrToOperation()->printC(indent); cout << ";" << endl;
		}
		dataIter->nextElement();
		i++;
	}
	
	ArrayIterator<ControlOpSlot> *controlIter;
	controlIter = _controlOpList.elements();
	i=0;
	while (controlIter->hasMoreElements()){
		if(controlIter->currentElement().hasOper()){
			controlIter->currentElement().getPtrToOperation()->printC(indent); cout << ";";
		}
		controlIter->nextElement();
		i++;
	}
	
	ArrayIterator<FlowOpSlot> *flowIter;
	flowIter = _flowOpList.elements();
	while (flowIter->hasMoreElements()){
		if(flowIter->currentElement().hasOper()){
			flowIter->currentElement().getPtrToOperation()->printC(indent);
		}
		flowIter->nextElement();
	}
	
	delete dataIter;
	delete controlIter;
	delete flowIter;
} // end of fcn printC

void NormalInstr::createListOfOperations(OpList & oplist)
{
	// data Operations
	ArrayIterator<DataOpSlot> *dataIter;
	dataIter = _dataOpList.elements();
	while (dataIter->hasMoreElements()){
		if(dataIter->currentElement().hasOper())
		{
			oplist.push_back(dataIter->currentElement().getPtrToOperation());
		}
		dataIter->nextElement();
	}
	
	ArrayIterator<ControlOpSlot> *controlIter;
	controlIter = _controlOpList.elements();
	while (controlIter->hasMoreElements()){
		if(controlIter->currentElement().hasOper()){
			oplist.push_back(controlIter->currentElement().getPtrToOperation());
		}
		controlIter->nextElement();
	}
}

void NormalInstr :: createListOfOperands(Symtab *operandSymTable)
{
	// if (_instrLabel != NULL){
	//  ((LabelArg *)_instrLabel)->print(out);
	//  out << endl;
	// }
	// out << "(* " << this << " *) ";
	
	ArrayIterator<DataOpSlot> *dataIter;
	dataIter = _dataOpList.elements();
	
	while (dataIter->hasMoreElements())
	{
		dataIter->currentElement().createListOfOperands(operandSymTable);
		dataIter->nextElement();
	}
	ArrayIterator<ControlOpSlot> *controlIter;
	
	controlIter = _controlOpList.elements();
	
	while (controlIter->hasMoreElements())
	{
		//  controlIter->currentElement().shortPrint(out);
		controlIter->nextElement();
	}
	delete dataIter;
	delete controlIter;
	// out << endl;
	
} // end of fcn createListOfOperands

void NormalInstr::irToC(FilePointer & fp, int indent)
{
//	printIndent(indent);

	int i;
	if (_instrLabel != NULL)
	{
		_instrLabel->irToC(fp, indent - 1);
		fprintf(fp, ":\n");
	}

/* SAH : Tue, Jun 27, 2000 : Uncomment this section if you want to include information about source code location.
// Also, put comments around the (LOCATION %d) string.

	// Now, check to see if the instruction has an attached Code Location Property.
	//
	BaseInstrProperty *codelocprop;
	if ((codelocprop = _propertyPtr.getPtrToProperty(_CODELOC_)) != NULL)
	{
		fprintf(fp, " (LOCATION %d) \n", ((CodeLocProperty *)codelocprop)->getLoc());
	}
*/
//	printIndent(indent);

	ArrayIterator<DataOpSlot> *dataIter;
	dataIter = _dataOpList.elements();
	i=0;
	while (dataIter->hasMoreElements()){
		if(dataIter->currentElement().hasOper()){
			dataIter->currentElement().getPtrToOperation()->irToC(fp, indent);
			fprintf(fp, "\n");
		}
		dataIter->nextElement();
		i++;
	}
	
	ArrayIterator<ControlOpSlot> *controlIter;
	controlIter = _controlOpList.elements();
	i=0;
	while (controlIter->hasMoreElements()){
		if(controlIter->currentElement().hasOper()){
			controlIter->currentElement().getPtrToOperation()->irToC(fp, indent);
			fprintf(fp, "\n");
		}
		controlIter->nextElement();
		i++;
	}
	
	ArrayIterator<FlowOpSlot> *flowIter;
	flowIter = _flowOpList.elements();
	while (flowIter->hasMoreElements()){
		if(flowIter->currentElement().hasOper()){
			flowIter->currentElement().getPtrToOperation()->irToC(fp, indent);
			fprintf(fp, "\n");
		}
		flowIter->nextElement();
	}
	
	delete dataIter;
	delete controlIter;
	delete flowIter;

	// Added code for counting cycles.
	//
	printIndent(fp, indent);
	fprintf(fp, "cycle_count++;\n");

	printIndent(fp, indent);
	fprintf(fp, "DoRFUpdate();\n\n");
}

void NormalInstr::printOpsProperty(ostream & out)
{
	ArrayIterator<DataOpSlot> *dataIter;
	dataIter = _dataOpList.elements();
	
	out << "Data Op Properties:" << endl;
	
	while (dataIter->hasMoreElements())
	{
		if (dataIter->currentElement().hasOper()){
			cout << "Op: " << (dataIter->currentElement()).getPtrToOperation();
			((dataIter->currentElement()).getPtrToOperation())->printOpProperty(out);
		}
		out << endl;
		dataIter->nextElement();
	}
	ArrayIterator<ControlOpSlot> *controlIter;
	
	controlIter = _controlOpList.elements();
	
	out << "Control Op Properties:" << endl;
	
	while (controlIter->hasMoreElements())
	{
		if (controlIter->currentElement().hasOper())
			((controlIter->currentElement()).getPtrToOperation())->printOpProperty(out);
		out << endl;
		controlIter->nextElement();
	}
	ArrayIterator<FlowOpSlot> *flowIter;
	
	flowIter = _flowOpList.elements();
	
	out << "Flow Op Properties:" << endl;
	
	while (flowIter->hasMoreElements())
	{
		if (flowIter->currentElement().hasOper())
			((flowIter->currentElement()).getPtrToOperation())->printOpProperty(out);
		out << endl;
		flowIter->nextElement();
	}
	
	out << endl;
	
	delete dataIter;
	delete controlIter;
	delete flowIter;
}

void NormalInstr::printArgsProperty(ostream & out)
{
	ArrayIterator<DataOpSlot> *dataIter;
	dataIter = _dataOpList.elements();
	
	out << "Args  Properties:" << endl;
	
	while (dataIter->hasMoreElements())
	{
		if (dataIter->currentElement().hasOper()) {
			out << " ------------  op ----------------------- " << endl;
			BaseOperation* op = ((dataIter->currentElement()).getPtrToOperation());
			
			out << "--------------  arg ----------------------" << endl;
			if (op->hasOperand(_DEST_, _DEST_LIST_))
			{
				if ( op->ptrToOperand(_DEST_, _DEST_LIST_)->isRegisterArg() )
					((RegisterArg*)op->ptrToOperand(_DEST_, _DEST_LIST_))->printProp(out);
			}
			else // i.e. no dest
			{
				out << " No dest arg " << endl;
			}
			
			out << "--------------  arg ----------------------" << endl;
			if (op->hasOperand(_SOURCE_1_))
			{
				if ( op->ptrToOperand(_SOURCE_1_)->isRegisterArg() )
					((RegisterArg*)op->ptrToOperand(_SOURCE_1_))->printProp(out);
			}
			else // i.e. no source1
			{
				out << " No Src1 arg " << endl;
			}
			
			out << "--------------  arg ----------------------" << endl;
			if (op->hasOperand(_SOURCE_2_))
			{
				if ( op->ptrToOperand(_SOURCE_2_) &&
					op->ptrToOperand(_SOURCE_2_)->isRegisterArg() )
					((RegisterArg*)op->ptrToOperand(_SOURCE_2_))->printProp(out);
			}
			else // i.e. no source2
			{
				out << " No Src2 arg "  << endl;
			}
		}
		out << endl;
		dataIter->nextElement();
	}
	ArrayIterator<ControlOpSlot> *controlIter;
	
	controlIter = _controlOpList.elements();
	
	out << "Control Op Properties:" << endl;
	
	while (controlIter->hasMoreElements())
	{
		if (controlIter->currentElement().hasOper()) {
			out << " ------------  op ----------------------- " << endl;
			BaseOperation* op = ((controlIter->currentElement()).getPtrToOperation());
			
			out << "--------------  arg source_1 ---------------" << endl;
			if (op->hasOperand(_SOURCE_1_))
			{
				if ( op->ptrToOperand(_SOURCE_1_)->isRegisterArg() )
					((RegisterArg*)op->ptrToOperand(_SOURCE_1_))->printProp(out);
			}
			else // i.e no source1
			{
				out << " No Src1 arg " << endl;
			}
			
			out << "--------------  arg source_2 ---------------" << endl;
			if (op->hasOperand(_SOURCE_2_))
			{
				if ( op->ptrToOperand(_SOURCE_2_)->isRegisterArg() )
					((RegisterArg*)op->ptrToOperand(_SOURCE_2_))->printProp(out);
			}
			else // i.e. no source2
			{
				out << " No Src2 arg "<< endl;
			}
		}	
		out << endl;
		controlIter->nextElement();
	}
	   
	delete dataIter;
	delete controlIter;
}

void NormalInstr::print()
{
	cout << ">>>>> " << this << " >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << endl;
	
	if (_instrLabel != NULL)
	{
		cout << " \t Lab: ";
		
		_instrLabel->print();
		cout << endl;
		
	}
	
	ArrayIterator<DataOpSlot> *dataIter;
	
	dataIter = _dataOpList.elements();
	
	if (dataIter->hasMoreElements())
	{
		cout << "=== Data Ops" << endl;
		
		while (dataIter->hasMoreElements())
		{
			cout << dataIter->currentElement();
			dataIter->nextElement();
		}
	}
	
	ArrayIterator<ControlOpSlot> *controlIter;
	
	controlIter = _controlOpList.elements();
	
	if (controlIter->hasMoreElements())
	{
		cout << "=== Control Ops" << endl;
		
		while (controlIter->hasMoreElements())
		{
			cout << controlIter->currentElement() << endl;
			controlIter->nextElement();
		}
	}
	
	ArrayIterator<FlowOpSlot> *flowIter;
	
	flowIter = _flowOpList.elements();
	
	if (flowIter->hasMoreElements())
	{
		cout << "=== Flow Ops" << endl;
		
		while (flowIter->hasMoreElements())
		{
			cout << flowIter->currentElement() << endl;
			flowIter->nextElement();
		}
	}
	
	delete dataIter;
	delete controlIter;
	delete flowIter;
	
	cout << "<<<<< " << this << " <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;
}

void NormalInstr::removeAllOp()
{
	ArrayIterator<DataOpSlot> *dIter;
	dIter = _dataOpList.elements();
	while (dIter->hasMoreElements()) {
		if ((dIter->currentElement()).hasOper() != _NO_) 
			(dIter->currentElement()).deleteOperation();
		(dIter->currentElement()).setNext(NULL);
		dIter->nextElement();
	}
	delete dIter;
	
	ArrayIterator<FlowOpSlot> *fIter;
	fIter = _flowOpList.elements();
	while (fIter->hasMoreElements()) {
		if ((fIter->currentElement()).hasOper() != _NO_) 
			(fIter->currentElement()).deleteOperation();
		(fIter->currentElement()).setNext(NULL);
		fIter->nextElement();
	}
	delete fIter;
	
	ArrayIterator<ControlOpSlot> *cIter;
	cIter = _controlOpList.elements();
	while (cIter->hasMoreElements()) {
		if ((cIter->currentElement()).hasOper() != _NO_) 
			(cIter->currentElement()).deleteOperation();
		(cIter->currentElement()).setNext(NULL);
		cIter->nextElement();
	}
	delete cIter;
	
	if (_instrLabel!=NULL){
		delete _instrLabel;
		_instrLabel = NULL;
	}
}

//
// If change == 1, then it means that we can change the instr (for ex: delete flow ops),
// while appending it.
//
void NormalInstr::appendInstr(BaseInstruction *prevInstr, int nextInstrDir,int change)
{
	int hasFlow=0;
	if (prevInstr == NULL)
	{
		cout << "panic: Recieved NULL instruction" << endl;
		assert(0);
	}
	
	if (!prevInstr->isNormalInstr())
	{
		// error condition.
		cout << "Panic: Can only append instr to a NormalInstr";
		cout << endl;
		assert(0);
	}
	
	// First insert the instruction to the IList.
	//
	currRoutine->insertInstrAfter(this,prevInstr);
	
	BaseOperation *nextOp;
	
	if (nextInstrDir == _TRUE_BRANCH_)
	{
		nextOp = (((NormalInstr*)prevInstr)->getFlowOpSlot(0)).getPtrToOperation();
	}
	else if (nextInstrDir == _FALSE_BRANCH_)
	{
		nextOp = (((NormalInstr*)prevInstr)->getFlowOpSlot(numFlowOpsPerInstr - 1)).getPtrToOperation();
	}
	else if(nextInstrDir == -1){
		for (int i = 0; i < numFlowOpsPerInstr; i++){
			nextOp = (((NormalInstr*)prevInstr)->getFlowOpSlot(i)).getPtrToOperation();       
			if(nextOp!=NULL){
				nextInstrDir=i;
				break;
			}
		}
	}
	else{
	}
	
	
	if (nextOp == NULL)
	{
		// error condition.
		cout << "Panic: Tried to append instr to non-existant path" << endl;
		assert(0);
	}
	BaseInstruction *oldTarget=((FlowOp*)nextOp)->getTargetInstr();
	
	((FlowOp*)nextOp)->setTargetInstr(this);
	
	for (int i = 0; i < numFlowOpsPerInstr; i++){
		if(this->getFlowOpSlot(i).hasOper()){
			if (change)
			{
				(this->getFlowOpSlot(i)).deleteOperation();
			}
			else
			{
				hasFlow=1;
				break;
			}
		}
	}
	
	
	if(!hasFlow){
		FlowOp* newFlow=new JumpOp(oldTarget);
		if (nextInstrDir == _TRUE_BRANCH_)
		{
			this->addFlowOpToList(newFlow,0);
		}
		else if (nextInstrDir == _FALSE_BRANCH_)
		{
			this->addFlowOpToList(newFlow,numFlowOpsPerInstr - 1);
		}
		
		BaseOpSlot *newTgtSlot = &(this->getRootOpSlot());
		BaseOpSlot *prevSlot = NULL;
		
		int nextSlot = 0;
		while ((newTgtSlot != NULL) && (newTgtSlot->isDataOpSlot())){
			prevSlot = newTgtSlot;
			newTgtSlot = newTgtSlot->getPtrToNextSlot();
			nextSlot = 1;
		}
		
		if (newTgtSlot!=NULL && newTgtSlot->isControlOpSlot()){
			cout << "Panic: There should be no Control Ops in the new instruction" << endl;
			assert(0);
		}
		if (newTgtSlot!=NULL && newTgtSlot->isFlowOpSlot())
		{
			// the head op of this instruction is a flow op. Do not need to set the next pointers.
			// However, make a check to see if the headerop is pointing to the correct flow op.
			if (!newTgtSlot->hasOper())
			{
				// as of now, this is an error condition. in future, we could change the headerop to
				// point to the newly added flow op.
				//
				cout << "Warning: In appendInstr, the header op does not point to the correct flow op" << endl;
			}
		}
		if (nextSlot)
		{
			prevSlot->setNext(& (this->getFlowOpSlot(nextInstrDir)));
		}
	}
	else{//TBD
	}
}

void NormalInstr::initScheduleAndAdd(BaseOperation *someOpPtr, bool noCopy){
	int pos;
	Vector<int> indexes;
	
	simpleVLIWInstrTemplate->getSlotListForOper(someOpPtr, indexes);
	
	if (indexes.size() == 0)
	{
		// Error condition. This opcode does not have a valid slot.
		//
		MyString str; 
		someOpPtr->getOpcodeName(str);
		cout << "Panic: Opcode " << str.getStr() << " is not supported by the architecture" << endl;
		assert(0);
	}
	/*
	cout << "*************** Testing for indexes[0] ************" << endl;
	cout << indexes[0] << endl;
	cout << "*************** Testing for indexes[0] ************" << endl;
	*/			
	// We arbitrarily choose indexes[0] to be the first place where the operation is placed.
	//
	pos = indexes[0];
	
	if (someOpPtr->isDataOp() == _YES_)
	{
		initAddDataOp((DataOp *)someOpPtr, pos, noCopy);
	}
	else if (someOpPtr->isControlOp() == _YES_)
	{
		initAddControlOp((ControlOp *)someOpPtr, pos, noCopy);
	}
	else if (someOpPtr->isFlowOp() == _YES_)
	{
		initAddFlowOp((FlowOp *)someOpPtr, pos, noCopy);
	}
}

BaseInstruction *NormalInstr::getFirstSuccOnly(void){
	BaseInstruction *tmp;
	int cnt=0;
	tmp=this->getFirstSucc();
	while(tmp==NULL && cnt<numFlowOpsPerInstr){
		tmp=this->getNextSucc();
		cnt++;
	}
	this->deleteSuccIter();
	return tmp;
}

/*
void NormalInstr::makeFullCopy(BaseInstruction* & instrPtr)
{
instrPtr = new NormalInstr();

  // TBD - this is not finished yet.
  }
*/ 

BaseInstruction *NormalInstr::copy()
{
	BaseInstruction *retPtr;
	retPtr = new NormalInstr(*this);
	
	return retPtr;
}


BasicBlockNode *NormalInstr::getBasicBlock(void){
	if (currRoutine->isUnstructured()) { // we cannot use _htg, bacause it is not computed
		ControlFlowGraph & cfg = currRoutine->getCFG();

		LinkedListIterator<BasicBlockNode> *bbIter = cfg.createBBIterForw();
		while (bbIter->hasMoreElements()) {
			BasicBlockNode &bb = bbIter->currentElement();

			if (bb.hasInstr(this)) {
				delete bbIter;
				return &bb;
			}

			bbIter->nextElement();
		}
		delete bbIter;

		return NULL;
	} else { // use _htg
		BaseHTGNode *node=this->getUp();
		if(node!=NULL && node->isSimpleNode()){
			return ((SimpleHTGNode*)node)->getBasicBlock();
		}
		return NULL;
	}
}



void NormalInstr::renameAllSrcValues(ValuesSET & in, ValuesSET & out)
{
	BaseOperation *currOp;
	
	ArgList alist;
	ArgListIter aiter;

	ArrayIterator<DataOpSlot> *dIter;
	dIter = _dataOpList.elements();
	while (dIter->hasMoreElements())
	{
		if ((dIter->currentElement()).hasOper())
		{
			currOp = (dIter->currentElement()).getPtrToOperation();
			currOp->sourceOperandsList(alist);

			for (aiter = alist.begin(); aiter != alist.end(); aiter++)
			{
				renameValue(*aiter, in);
			}

			alist.clear();
		}
		dIter->nextElement();
	}
	delete dIter;
	
	ArrayIterator<ControlOpSlot> *cIter;
	cIter = _controlOpList.elements();
	while (cIter->hasMoreElements())
	{
		if ((cIter->currentElement()).hasOper())
		{
			currOp = (cIter->currentElement()).getPtrToOperation();
			currOp->sourceOperandsList(alist);

			for (aiter = alist.begin(); aiter != alist.end(); aiter++)
			{
				renameValue(*aiter, in);
			}

			alist.clear();
		}
		
		// If the current element is a call operation, need to also
		// change the parm and savedregs lists
		//
		if (currOp->isCallRetOp())
		{
			ArgList & aL = ((CallRetOp *)currOp)->getParmList();
			ArgListIter i;
			for(i = aL.begin(); i != aL.end(); i++)
				renameValue((*i), in);
			
			ArgList & sL = ((CallRetOp *)currOp)->getSavedRegList();
			for(i = sL.begin(); i != sL.end(); i++)
				renameValue((*i), in);
		}
		
		cIter->nextElement();
	}
	delete cIter;
	//
	// Note: Currently, no control-op or flow-op has any destination argument.
	// (i.e. they do not define any value which will be used later on).
	//
	
	// Now, set the out set = in set + localdefs.
	
	ValuesSET tmp;
	getLocalDefs(tmp);
	
	out = in;
	out.impose(tmp);
}


void NormalInstr::getLocalDefs(ValuesSET & defSet)
{
	BaseOperation *currOp;
	
	ArgList alist;
	ArgListIter aiter;

	ArrayIterator<DataOpSlot> *dIter;
	dIter = _dataOpList.elements();
	while (dIter->hasMoreElements())
	{
		if ((dIter->currentElement()).hasOper())
		{
			currOp = (dIter->currentElement()).getPtrToOperation();
			currOp->destOperandsList(alist);

			for (aiter = alist.begin(); aiter != alist.end(); aiter++)
			{
				defSet.addEntry(*aiter);
			}

			alist.clear();
		}
		dIter->nextElement();
	}
	delete dIter;
	
	//
	// Note: Currently, no control-op or flow-op has any destination argument.
	// (i.e. they do not define any value which will be used later on).
	//
	// So, the next section of code has been commented out. if the above assumption 
	// is changed then, this section must be un-commented.
	
	/*
	ArrayIterator<ControlOpSlot> *cIter;
	cIter = _controlOpList.elements();
	while (cIter->hasMoreElements())
	{
	if ((cIter->currentElement()).hasOper())
	{
	currOp = (cIter->currentElement()).getPtrToOperation();
	
	  if (currOp->hasOperand(_DEST_, _DEST_LIST_))
	  {
	  out.addEntry(currOp->ptrToOperand(_DEST_, _DEST_LIST_));
	  }
	  }
	  cIter->nextElement();
	  }
	  delete cIter;
	*/
}

void NormalInstr::setLowestSSAValues(int type)
{
	if (type != _DEST_LIST_)
	{
		// currently,we do not support this method for any position other than
		// the destination argument.
		
		cout << " Panic: setLowestSSAValues called for type other than _DEST_LIST_" << endl;
		assert(0);
	}
	
	BaseOperation *currOp;
	
	ArgList alist;
	ArgListIter aiter;

	ArrayIterator<DataOpSlot> *dIter;
	dIter = _dataOpList.elements();
	while (dIter->hasMoreElements())
	{
		if ((dIter->currentElement()).hasOper())
		{
			currOp = (dIter->currentElement()).getPtrToOperation();
			currOp->destOperandsList(alist);

			for (aiter = alist.begin(); aiter != alist.end(); aiter++)
			{
				setLowestSSAValue(*aiter);
			}

			alist.clear();
		}
		dIter->nextElement();
	}
	delete dIter;
	
	// Note: Currently, no control-op or flow-op has any destination argument.
	// (i.e. they do not define any value which will be used later on).
	//
}


void NormalInstr::renameAllDestValues()
{
	BaseOperation *currOp;
	
	ArgList alist;
	ArgListIter aiter;

	ArrayIterator<DataOpSlot> *dIter;
	dIter = _dataOpList.elements();
	while (dIter->hasMoreElements())
	{
		if ((dIter->currentElement()).hasOper())
		{
			currOp = (dIter->currentElement()).getPtrToOperation();
			currOp->destOperandsList(alist);

			for (aiter = alist.begin(); aiter != alist.end(); aiter++)
			{
				renameValue(*aiter);
			}

			alist.clear();
		}
		dIter->nextElement();
	}
	delete dIter;
	
	// Note: Currently, no control-op or flow-op has any destination argument.
	// (i.e. they do not define any value which will be used later on).
	//
}



void NormalInstr::setOpID(int &opcrt){
	BaseOperation *currOp;
	ArrayIterator<DataOpSlot> *dIter;
	dIter = _dataOpList.elements();
	while (dIter->hasMoreElements())
	{
		if ((dIter->currentElement()).hasOper())
		{
			currOp = (dIter->currentElement()).getPtrToOperation();
			currOp->setID(opcrt++);
		}
		dIter->nextElement();
	}
	delete dIter;
	
	ArrayIterator<ControlOpSlot> *cIter;
	cIter = _controlOpList.elements();
	while (cIter->hasMoreElements())
	{
		if ((cIter->currentElement()).hasOper())
		{
			currOp = (cIter->currentElement()).getPtrToOperation();
			currOp->setID(opcrt++);
		}
		cIter->nextElement();
	}
	delete cIter;
	
	ArrayIterator<FlowOpSlot> *fIter;
	fIter = _flowOpList.elements();
	while (fIter->hasMoreElements())
	{
		if ((fIter->currentElement()).hasOper())
		{
			currOp = (fIter->currentElement()).getPtrToOperation();
			currOp->setID(opcrt++);
		}
		fIter->nextElement();
	}
	delete fIter;
}

BaseOperation * NormalInstr::getOpWithArg(BaseArgument *arg, int pos)
{
	int doData, doControl, doFlow;
	doData = doControl = doFlow = 0;
	
	if (pos == _INVALID_){ doData = doControl = doFlow = 1;}
	else if (pos == DATA_OP) doData = 1;
	else if (pos == CONTROL_OP) doControl = 1;
	else if (pos == FLOW_OP) doFlow = 1;
	
	BaseOperation *retVal = NULL;
	BaseOperation *currOper;
	if (doData)
	{
		ArrayIterator<DataOpSlot> *dIter;
		dIter = _dataOpList.elements();
		while (dIter->hasMoreElements())
		{
			if ((dIter->currentElement()).hasOper())
			{
				currOper = (dIter->currentElement()).getPtrToOperation();
				if (currOper->hasArg(arg))
				{
					retVal = currOper;
					break;
				}
			}
			dIter->nextElement();
		}
		delete dIter;
		if (retVal != NULL) return retVal;
	}
	if (doControl)
	{
		ArrayIterator<ControlOpSlot> *cIter;
		cIter = _controlOpList.elements();
		while (cIter->hasMoreElements())
		{
			if ((cIter->currentElement()).hasOper())
			{
				currOper = (cIter->currentElement()).getPtrToOperation();
				if (currOper->hasArg(arg))
				{
					retVal = currOper;
					break;
				}
			}
			cIter->nextElement();
		}
		delete cIter;
		if (retVal != NULL) return retVal;
	}
	if (doFlow)
	{
		ArrayIterator<FlowOpSlot> *fIter;
		fIter = _flowOpList.elements();
		while (fIter->hasMoreElements())
		{
			if ((fIter->currentElement()).hasOper())
			{
				currOper = (fIter->currentElement()).getPtrToOperation();
				if (currOper->hasArg(arg))
				{
					retVal = currOper;
					break;
				}
			}
			fIter->nextElement();
		}
		delete fIter;
	}
	return retVal;
}


int NormalInstr::hasMultipleSuccs(void){
	int count=0;
	BaseOperation *flowOpPtr;
	ArrayIterator<FlowOpSlot> *succListIter = _flowOpList.elements();
	while (succListIter->hasMoreElements())
	{
		flowOpPtr = (succListIter->currentElement()).getPtrToOperation();
		if (flowOpPtr != NULL)  
			count++;
		
		succListIter->nextElement();
	}
	delete succListIter;
	if(count>1) return 1;
	return 0;
}

BaseOperation* NormalInstr::hasCallRetOp()
{
	BaseOperation *cOp, *ret = NULL;
	
	ArrayIterator<ControlOpSlot> *cIter = _controlOpList.elements();
	while (cIter->hasMoreElements())
	{
		if ((cIter->currentElement()).hasOper())
		{
			cOp = (cIter->currentElement()).getPtrToOperation();
			if (cOp->isCallRetOp())
			{
				ret = cOp;
				break;
			}
		}
		cIter->nextElement();
	}
	delete cIter;
	
	return ret;
}

BaseOperation* NormalInstr::hasIfOp()
{
	BaseOperation *cOp, *ret = NULL;
	
	ArrayIterator<ControlOpSlot> *cIter = _controlOpList.elements();
	while (cIter->hasMoreElements())
	{
		if ((cIter->currentElement()).hasOper())
		{
			cOp = (cIter->currentElement()).getPtrToOperation();
			if (cOp->isIfOp())
			{
				ret = cOp;
				break;
			}
		}
		cIter->nextElement();
	}
	delete cIter;
	
	return ret;
}

BaseOperation* NormalInstr::hasGotoOp()
{
	BaseOperation *fOp, *ret = NULL;
	
	ArrayIterator<FlowOpSlot> *fIter = _flowOpList.elements();
	while (fIter->hasMoreElements())
	{
		if ((fIter->currentElement()).hasOper())
		{
			fOp = (fIter->currentElement()).getPtrToOperation();
			if (fOp->isGotoOp())
			{
				ret = fOp;
				break;
			}
		}
		fIter->nextElement();
	}
	delete fIter;
	
	return ret;
}

BaseOperation* NormalInstr::hasSameOp(BaseOperation* op) //semantically the same (i.e. same opcode and operands)
{
	ArrayIterator<DataOpSlot> *dIter=getDataOpList()->elements();
	while (dIter->hasMoreElements())
	{
		if ((dIter->currentElement()).hasOper() == _YES_)
		{
			BaseOperation* oper=(dIter->currentElement()).getPtrToOperation();
			if (oper->isEq(op))
			{
				return oper;
			}
		}
		dIter->nextElement();
	}
	delete dIter;
	
	return NULL; 
}

// for ASM gen
void NormalInstr::printASMInstr(ostream& out)
{
	/* For debugging : Print the location in the source code */
	int loc;
	map<int,set<slot_type*>*>::iterator k;
	set<slot_type*> *slot_list;
	ArrayIterator<DataOpSlot> *dataIter;
	ArrayIterator<ControlOpSlot> *controlIter;
	
	loc=this->getLoc();
	
	// out << "# Location: " << loc << endl;

	// Printing out the label
	if (_instrLabel != NULL){
		((LabelArg *)_instrLabel)->printAsm(out);
		out << endl;
	}
	// debug:PB
	// out << "Instruction:" << endl;
	if(!PhysicalSlotPrinting){

		
		dataIter = _dataOpList.elements();
		
		while (dataIter->hasMoreElements())
		{
			dataIter->currentElement().printASMInstr(out);  // dataOpSlot
			dataIter->nextElement();
		}
		
		controlIter = _controlOpList.elements();
		
		while (controlIter->hasMoreElements())
		{
			controlIter->currentElement().printASMInstr(out);  // controlOpSlot
			controlIter->nextElement();
		}
	}
	else   // PhysicalSlotPrinting = TRUE
	{
	    //new printing, taking the physical slots, and the physical to virtual slot mapping,
		//and printing the operations in order of the physical slots
		int found=0;
		//for each physical slot
		for(k=slot_mappings.begin();k!=slot_mappings.end();k++)
		{
			slot_list=(*k).second;
			dataIter = _dataOpList.elements();
			found=0;
			int i=0;
			//for each virtual data slot
			while (dataIter->hasMoreElements()){
				//if this virtual slot is in the mapping for this current physical slot
				if(find_slot(slot_list,i,DATA_OP_SLOT)){
					//if(slot_list->find(i)!=slot_list->end() && (*(slot_list->find(i)))->type==DATA_OP_SLOT){
					//if found, then print this operation
					if(dataIter->currentElement().hasOper()){
						found=1;
						dataIter->currentElement().printASMInstr(out);
						//  cout << "\tSlot: " << i;
					}
				}
				dataIter->nextElement();
				i++;
			}

			controlIter = _controlOpList.elements();
			i=0;
			while (controlIter->hasMoreElements()){
				//if this virtual slot is in the mapping for this current physical slot
				if(find_slot(slot_list,i,CONTROL_OP_SLOT)){
					if(controlIter->currentElement().hasOper()){
						found=1;
						controlIter->currentElement().printASMInstr(out);
						// cout << "\tSlot: " << i;
					}
				}
				controlIter->nextElement();
				i++;
			}
			if(explicit_nops && !found){
				// cout << "\tNOP" << endl;
				out << "\t4\tnop\t()\t()\n";
			}
			
		}
	}
	delete dataIter;
	delete controlIter;
	return;
}

// Currently, we go : Routine -> BasicBlock -> Instruction -> Operation -> Argument.
// Note that HTGNode is not present in the recursion.
//
void NormalInstr::recursiveRemoveProperty(PropertyNames name)
{
	BaseInstruction::recursiveRemoveProperty(name);

	ArrayIterator<DataOpSlot> *dIter;
	dIter= _dataOpList.elements();

	while (dIter->hasMoreElements())
	{
		if ((dIter->currentElement()).hasOper())
		{
			((dIter->currentElement()).getPtrToOperation())->recursiveRemoveProperty(name);
		}
		dIter->nextElement();
	}
	delete dIter;

	ArrayIterator<ControlOpSlot> *cIter;
	cIter= _controlOpList.elements();

	while (cIter->hasMoreElements())
	{
		if ((cIter->currentElement()).hasOper())
		{
			((cIter->currentElement()).getPtrToOperation())->recursiveRemoveProperty(name);
		}
		cIter->nextElement();
	}
	delete cIter;

	ArrayIterator<FlowOpSlot> *fIter;
	fIter= _flowOpList.elements();

	while (fIter->hasMoreElements())
	{
		if ((fIter->currentElement()).hasOper())
		{
			((fIter->currentElement()).getPtrToOperation())->recursiveRemoveProperty(name);
		}
		fIter->nextElement();
	}
	delete fIter;
}

// See NormalInstr.h for an explanation of this function.
//
void changePointers(BaseInstruction *predInstr, BaseInstruction *currInstr, BaseInstruction *newInstr)
{
	if (predInstr == NULL) return;

	// convert each base instr to a normal instr
	//
	NormalInstr *nPred = (NormalInstr *)predInstr;
	NormalInstr *nCurr = (NormalInstr *)currInstr;
	NormalInstr *nNew = (NormalInstr *)newInstr;

	// go thru the pred instr, and for each flow op that points to currInstr change it to point to newInstr
	//
	ArrayIterator<FlowOpSlot> *succListIter = nPred->getFlowOpSlotIter();

	FlowOpSlot *flowOpSlotPtr;
	FlowOp *jumpOp;

	while (succListIter->hasMoreElements())
	{
		flowOpSlotPtr = &(succListIter->currentElement());

		if (flowOpSlotPtr->hasOper())
		{
			jumpOp = (FlowOp *)(flowOpSlotPtr->getPtrToOperation());

			if (jumpOp->getTargetInstr() == currInstr)
			{
				// change it to point to newInstr
				//
				jumpOp->setTargetInstr(newInstr);
			}
		}
		succListIter->nextElement();
	}
	delete succListIter;
}

extern int defines(BaseOperation *oper, BaseArgument *arg);
extern int uses(BaseOperation *oper, BaseArgument *arg);

// returns _YES_ if the instruction defines that particular argument.
// Note: It checks if the argument values are equal.
// i.e. it does not return _YES_ only if the arg pointers are equal.
//
int defines(BaseInstruction *instr, BaseArgument *arg)
{
	int retVal = _NO_;

	// iterate over each operation.
	//
	ArrayIterator<DataOpSlot> *dIter;
	dIter= ((NormalInstr *)instr)->getDataOpSlotIter();

	while (dIter->hasMoreElements())
	{
		if ((dIter->currentElement()).hasOper())
		{
			if (defines((dIter->currentElement()).getPtrToOperation(), arg))
			{
				retVal = _YES_;
				break;
			}
		}
		dIter->nextElement();
	}
	delete dIter;

	if (retVal == _YES_) return _YES_;

	ArrayIterator<ControlOpSlot> *cIter;
	cIter= ((NormalInstr *)instr)->getControlOpSlotIter();

	while (cIter->hasMoreElements())
	{
		if ((cIter->currentElement()).hasOper())
		{
			if (defines((cIter->currentElement()).getPtrToOperation(), arg))
			{
				retVal = _YES_;
				break;
			}
		}
		cIter->nextElement();
	}
	delete cIter;

	return retVal;
}

// returns _YES_ if the instruction uses that particular argument.
// Note: It checks if the argument values are equal.
// i.e. it does not return _YES_ only if the arg pointers are equal.
//
int uses(BaseInstruction *instr, BaseArgument *arg)
{
	int retVal = _NO_;

	// iterate over each operation.
	//
	ArrayIterator<DataOpSlot> *dIter;
	dIter= ((NormalInstr *)instr)->getDataOpSlotIter();

	while (dIter->hasMoreElements())
	{
		if ((dIter->currentElement()).hasOper())
		{
			if (uses((dIter->currentElement()).getPtrToOperation(), arg))
			{
				retVal = _YES_;
				break;
			}
		}
		dIter->nextElement();
	}
	delete dIter;

	if (retVal == _YES_) return _YES_;

	ArrayIterator<ControlOpSlot> *cIter;
	cIter= ((NormalInstr *)instr)->getControlOpSlotIter();

	while (cIter->hasMoreElements())
	{
		if ((cIter->currentElement()).hasOper())
		{
			if (uses((cIter->currentElement()).getPtrToOperation(), arg))
			{
				retVal = _YES_;
				break;
			}
		}
		cIter->nextElement();
	}
	delete cIter;

	if (retVal == _YES_) return _YES_;

	ArrayIterator<FlowOpSlot> *fIter;
	fIter= ((NormalInstr *)instr)->getFlowOpSlotIter();

	while (fIter->hasMoreElements())
	{
		if ((fIter->currentElement()).hasOper())
		{
			if (uses((fIter->currentElement()).getPtrToOperation(), arg))
			{
				retVal = _YES_;
				break;
			}
		}
		fIter->nextElement();
	}
	delete fIter;

	return retVal;
}
