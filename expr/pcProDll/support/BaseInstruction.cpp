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

#include "BaseInstruction.h"
#include "FlowOpSlot.h"
#include "FlowOp.h"
#include "NormalInstr.h"
#include "CodeLocProperty.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

#ifdef __UNIX__
#define ASSERT assert
#endif

#ifdef WIN32_GUI	//WIN32_GUI
#include "../../pcProGUI/ColorCodes.h"

/*
void BaseInstruction::Draw(GRAPH_NODE_TYPE nodeType, CDC * pDC)//, int x_pos, int y_pos)
{
ASSERT_TRACE(0,"Don't call us. We'll call you!");
}
*/

GenericNode* BaseInstruction::FirstNotExpandedNode(GRAPH_NODE_TYPE nodeType)
{
 //instructions can't be expanded
 return this;
}

__declspec (dllexport) bool updateLabel=false;

void BaseInstruction::DrawInfo(GRAPH_NODE_TYPE nodeType, CDC * pDC, CRect& rect)
{
 CRect tempR(rect);
 GraphNode* node=GetGraphNode(nodeType);
 char buf[2000];
 pDC->SetTextColor(node->textColor);
 pDC->SetTextAlign(TA_TOP);
 if ((node->label=="") || (updateLabel==true))
 {
  char temp[2000];
  if(isNormalInstr()){
	  this->printOpcodesToString(buf);
	  sprintf(temp,"%s",buf);
  }
  else{
	sprintf(temp,"IN: 0x%08X",this);
  }
  node->label=temp;
 }
 //pDC->TextOut(rect.left+2,rect.top+2,node->label);
 tempR.OffsetRect(0,-3);
 //pDC->DrawText(node->label,-1,tempR,DT_WORDBREAK);
 pDC->TextOut(tempR.left+2,tempR.top+2,node->label);
}

//used by ILGView
void BaseInstruction::DOTText(GRAPH_NODE_TYPE nodeType, CString& out)
{
 ASSERT_TRACE(0,"Don't call us. We'll call you!");
}

//used by CFGView and HTGView
void BaseInstruction::DOTText(GRAPH_NODE_TYPE nodeType, CString& out, BasicBlockNode* block)
{
 ASSERT_TRACE(0,"Don't call us. We'll call you!");
}

#endif //WIN32_GUI

BaseInstruction::BaseInstruction()
{
#ifdef WIN32_GUI	//WIN32_GUI
 cfgNode.borderColor=BORDER_COLOR_INSTR;
 ilgNode.borderColor=BORDER_COLOR_INSTR;
 htgNode.borderColor=BORDER_COLOR_INSTR;
#endif //WIN32_GUI
 _isBreak= 0;
}


void BaseInstruction::printProperties(ostream & out, PropNameList & pL)
{
	BaseInstrProperty *prop;
	PropNameListIter i;

	if (_propertyPtr.isEmpty()) return;

	out << " %% ";
	for (i = pL.begin(); i != pL.end(); i++)
	{
		prop = _propertyPtr.getPtrToProperty(*i);
		if (prop != NULL)
			prop->print(out);
		out << "  ";
	}
	out << " %% ";
}

int BaseInstruction::percUnify(OpList* opList, Routine* routine)
{
 ASSERT(0);
 return 0;
}

int BaseInstruction::percUnifyWrap(Routine* routine)
{
 ASSERT(0);
 return 0;
}

void BaseInstruction::updateInstrUDDU(BaseInstruction* old, MyLinkedList<BaseOperation*>* moves)
{
 ASSERT(0);
}

void BaseInstruction::ifConvert(BaseOpProperty* pred, OpList &IselList, BaseOperation* ifOp, OpList OperationsList)
{
 ASSERT(0);
}


int BaseInstruction::percMigrate(Routine* routine, BaseOperation* op)
{
 ASSERT(0);
 return 0;
}

int BaseInstruction::getLoc(void)
{
	BaseInstrProperty *ptr = _propertyPtr.getPtrToProperty(_CODELOC_);
	if (ptr != NULL)
	{
		return (((CodeLocProperty *)ptr)->getLoc());
	}
	return 0;
}

// Currently, we go : Routine -> BasicBlock -> Instruction -> Operation -> Argument.
// Note that HTGNode is not present in the recursion.
//
void BaseInstruction::recursiveRemoveProperty(PropertyNames name)
{
	_propertyPtr.removeProperty(name);
}
