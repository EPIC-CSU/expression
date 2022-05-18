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
// GenericNode.cpp: implementation of the GenericNode class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#ifndef __UNIX__
#include "../pcProDllMFC.h"
#endif
#include "GenericNode.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#ifdef WIN32_GUI	//WIN32_GUI
GenericNode::GenericNode(): cfgNode(GNT_CFG), ilgNode(GNT_ILG), htgNode(GNT_HTG)
{
}
#else
GenericNode::GenericNode()
{
}
#endif //WIN32_GUI

GenericNode::~GenericNode()
{
}

#ifdef WIN32_GUI	//WIN32_GUI

bool GenericNode::ToggleExpanded(GRAPH_NODE_TYPE nodeType)\
{
 GraphNode* node=GetGraphNode(nodeType);
 return (node->expanded=!node->expanded);
}

bool GenericNode::ToggleSelected(GRAPH_NODE_TYPE nodeType)
{
 GraphNode* node=GetGraphNode(nodeType);
 return (node->selected=!node->selected);
}

//bool GenericNode::ToggleUDDU(GRAPH_NODE_TYPE nodeType)
//{
// GraphNode* node=GetGraphNode(nodeType);
// return (node->showUDDU=!node->showUDDU);
//}

void GenericNode::ToggleUDDU(GRAPH_NODE_TYPE nodeType)
{
 ToggleUD(nodeType);
 ToggleDU(nodeType);
}

void GenericNode::ToggleUD(GRAPH_NODE_TYPE nodeType)
{
 GraphNode* node=GetGraphNode(nodeType);
 node->showUD=!node->showUD;
}

void GenericNode::ToggleDU(GRAPH_NODE_TYPE nodeType)
{
 GraphNode* node=GetGraphNode(nodeType);
 node->showDU=!node->showDU;
}

GenericNode* GenericNode::FirstNotExpandedNode(GRAPH_NODE_TYPE nodeType)
{
 ASSERT_TRACE(0,"Don't call us. We'll call you!");
 return NULL;
}

MOUSEHITINFO* GenericNode::getHit(GRAPH_NODE_TYPE nodeType, CPoint & point)
{
 ASSERT_TRACE(0,"Don't call us. We'll call you!");
 return NULL;
}

 __declspec( dllexport ) 
GraphNode* GenericNode::GetGraphNode(GRAPH_NODE_TYPE nodeType)
{
 switch (nodeType)
 {
 case GNT_ILG:
  return &ilgNode;
  break;
 case GNT_CFG:
  return &cfgNode;
  break;
 case GNT_HTG:
  return &htgNode;
  break;
 default:
  return NULL;
  break;
 }
}


bool GenericNode::IsSameTypeExpanded(GRAPH_NODE_TYPE nodeType)
{
 GraphNode* node=GetGraphNode(nodeType);

 switch (nodeType)
 {
 case GNT_ILG:
      return ilgNode.expanded;
      break;
 case GNT_CFG:
      return cfgNode.expanded;
      break;
 case GNT_HTG:
      return htgNode.expanded;
      break;
 default:
      ASSERT_TRACE(0,"Oops!");
      break;
      return false;
 }
 return false;
}

CRect* GenericNode::Draw(GRAPH_NODE_TYPE nodeType, CDC * pDC)//, int xPos, int yPos)
{
 ASSERT_TRACE(0,"Don't call us. We'll call you!");
 return NULL;
}

void GenericNode::DrawInfo(GRAPH_NODE_TYPE nodeType, CDC * pDC, CRect& rect)
{
 ASSERT_TRACE(0,"Don't call us. We'll call you!");
}

void GenericNode::DOTText(GRAPH_NODE_TYPE nodeType, CString& out)
{
 ASSERT_TRACE(0,"Don't call us. We'll call you!");
}

void GenericNode::DisplayProperties(BasePropPage* page)
{
	//nothing
}

#endif //WIN32_GUI

