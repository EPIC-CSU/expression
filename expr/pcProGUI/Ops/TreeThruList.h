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
// TreeThruList.h: interface for the TreeThruList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TREETHRULIST_H__98E80441_E309_11D2_BBDD_00C04FB17504__INCLUDED_)
#define AFX_TREETHRULIST_H__98E80441_E309_11D2_BBDD_00C04FB17504__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class OG;
class ISInfo;

class TreeThruList  
{
public:
	TreeThruList();
	virtual ~TreeThruList();

 static void OnDrawItem(OG* og, int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
 static void FillGroupsList(ISInfo* ISInfo, CListBox& listBox);
};

#endif // !defined(AFX_TREETHRULIST_H__98E80441_E309_11D2_BBDD_00C04FB17504__INCLUDED_)
