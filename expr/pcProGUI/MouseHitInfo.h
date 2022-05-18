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
// MouseHitInfo.h: interface for the MouseHitInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOUSEHITINFO_H__3AAEC710_25B1_11D2_B743_00C04FB17504__INCLUDED_)
#define AFX_MOUSEHITINFO_H__3AAEC710_25B1_11D2_B743_00C04FB17504__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//#include "../../pcProDll/include/GenericNode.h"
#include "../../pcProDll/win32/MouseHitInfoStruct.h"
#include "../../pcProDll/win32/GraphNode.h"

class GenericNode;

//#include "../../pcProDll/win32/MouseHitInfoStruct.h"

#define POINT_POS 72
#define POINT_SIZE 72
#define POINT_OFFSET 72

class MouseHitInfo  
{
public:
	MouseHitInfo(MHI_HIT_TYPE _type=MHI_INVALID,void* _node=NULL);
	virtual ~MouseHitInfo();
 CMenu menu;
 
 void ProcessRightClick(GRAPH_NODE_TYPE nodeType, CPoint& point, CWnd* wnd);
 void ProcessLeftClick(GRAPH_NODE_TYPE nodeType, GenericNode** prevSelection, bool simulatorMode = false);

 MHI_HIT_TYPE hitType;
 void* node;
};

#endif // !defined(AFX_MOUSEHITINFO_H__3AAEC710_25B1_11D2_B743_00C04FB17504__INCLUDED_)
