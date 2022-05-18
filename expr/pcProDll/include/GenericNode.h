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
// GenericNode.h: interface for the GenericNode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GENERICNODE_H__6EFE842E_28BA_11D2_B74D_00C04FB17504__INCLUDED_)
#define AFX_GENERICNODE_H__6EFE842E_28BA_11D2_B74D_00C04FB17504__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifdef WIN32_GUI//WIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUI
#include "../win32/GraphNode.h"
#include "../win32/MouseHitInfoStruct.h"
#include "..\..\pcProGUI\Resource.h"
//#include "..\..\pcProGUI\Prop\BasePropPage.h"
#endif //WIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUI

//wrapper class for BasicBlockNode and BaseHTGNode to make the Win32 GUI easier

class BasePropPage;

class GenericNode  
{
public:
	GenericNode();
	virtual ~GenericNode();

#ifdef WIN32_GUI//WIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUI
 GraphNode cfgNode; //nodes in the ILG
 GraphNode ilgNode; //nodes in the CFG
 GraphNode htgNode; //nodes in the HTG
 __declspec( dllexport ) GraphNode* GetGraphNode(GRAPH_NODE_TYPE nodeType);
 virtual CRect* Draw(GRAPH_NODE_TYPE nodeType, CDC * pDC);//, int x_pos, int y_pos);
 virtual void DrawInfo(GRAPH_NODE_TYPE nodeType, CDC * pDC, CRect& rect);
 virtual void DOTText(GRAPH_NODE_TYPE nodeType, CString& out);
 bool IsSameTypeExpanded(GRAPH_NODE_TYPE nodeType);
 virtual GenericNode* FirstNotExpandedNode(GRAPH_NODE_TYPE nodeType);
 virtual MOUSEHITINFO* getHit(GRAPH_NODE_TYPE nodeType, CPoint & point);
 virtual bool ToggleExpanded(GRAPH_NODE_TYPE nodeType);
 virtual bool ToggleSelected(GRAPH_NODE_TYPE nodeType);
 virtual void ToggleUDDU(GRAPH_NODE_TYPE nodeType);
 virtual void ToggleUD(GRAPH_NODE_TYPE nodeType);
 virtual void ToggleDU(GRAPH_NODE_TYPE nodeType);
 virtual void DisplayProperties(BasePropPage* dialog);
#endif //WIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUI
};

#endif // !defined(AFX_GENERICNODE_H__6EFE842E_28BA_11D2_B74D_00C04FB17504__INCLUDED_)
