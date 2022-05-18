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
// GraphNode.cpp: implementation of the GraphNode class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../pcProDllMFC.h"
#include "GraphNode.h"

#include "../../pcProGUI/ColorCodes.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GraphNode::GraphNode()
{
 ASSERT_TRACE(0,"No can do!");
}

GraphNode::GraphNode(GRAPH_NODE_TYPE _nodeType)
{
 nodeType=_nodeType; 

 //borderColor=::GetSysColor(COLOR_WINDOWTEXT);
 fillColor = NO_FILL; //0xFF...F for no fill
 textColor = TEXT_COLOR_FORE;

 expanded=false;
 selected=false;
 //showUDDU=false;
 showUD=false;
 showDU=false;
}

GraphNode::~GraphNode()
{
 CleanEdgeMap();
}


void GraphNode::CleanEdgeMap()
{
 POSITION p=edges.GetStartPosition();
 while (p!=NULL)
 {
  BezierEdge* edge;
  int key;
  edges.GetNextAssoc(p,key,edge);
  delete edge;
 }
 edges.RemoveAll();
}
