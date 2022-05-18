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
// GraphNode.h: interface for the GraphNode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRAPHNODE_H__6E562C31_28C6_11D2_B74D_00C04FB17504__INCLUDED_)
#define AFX_GRAPHNODE_H__6E562C31_28C6_11D2_B74D_00C04FB17504__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "BezierEdge.h"

typedef enum {GNT_INVALID, GNT_ILG, GNT_CFG, GNT_HTG, GNT_SIM} GRAPH_NODE_TYPE; 

#define NO_FILL 0xFFFFFFFF

class GraphNode  
{
public:
	GraphNode();
	GraphNode(GRAPH_NODE_TYPE _nodeType);
	virtual ~GraphNode();

 bool expanded;
 bool selected;
 //bool showUDDU;
 bool showUD;
 bool showDU;

 GRAPH_NODE_TYPE nodeType;

 //drawing attributes 
 CRect rect;
 CString style;
 CString shape;
 CString label;
 COLORREF borderColor;
 COLORREF fillColor; //0xFF...F for no fill
 COLORREF textColor;
 
 BezierEdgeMap edges; //these are graph edges emanating from this node 
 
 void CleanEdgeMap();
};

#endif // !defined(AFX_GRAPHNODE_H__6E562C31_28C6_11D2_B74D_00C04FB17504__INCLUDED_)
