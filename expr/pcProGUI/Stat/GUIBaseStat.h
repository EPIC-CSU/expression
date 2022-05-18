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
// GUIBaseStat.h: interface for the GUIBaseStat class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GUIBASESTAT_H__C6F04758_CB4F_11D2_BBCA_00C04FB17504__INCLUDED_)
#define AFX_GUIBASESTAT_H__C6F04758_CB4F_11D2_BBCA_00C04FB17504__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define STAT_SEPARATOR 2

//#include "stdafx.h"
#include <afx.h>
#include <afxtempl.h>

class GUIBaseStat : public CObject  
{
public:
	GUIBaseStat();
	virtual ~GUIBaseStat();

 virtual void OnDraw(HDC hDC, RECT* rect);
 virtual void Notify();

 static int width; //all stats use the same width

 CString memo;
 COLORREF color;

 int minVal;
 int maxVal;
 int height;

 int updateThisInt;
 char* updateThisStr; //just a pointer, I'll copy the string it points to
 
};

typedef CList<GUIBaseStat*,GUIBaseStat*> GUIBaseStatList;

#endif // !defined(AFX_GUIBASESTAT_H__C6F04758_CB4F_11D2_BBCA_00C04FB17504__INCLUDED_)
