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
// PropPageList.h: interface for the PropPageList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROPPAGELIST_H__F75AE54B_B56E_11D2_BBB8_00C04FB17504__INCLUDED_)
#define AFX_PROPPAGELIST_H__F75AE54B_B56E_11D2_BBB8_00C04FB17504__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <afxtempl.h>
#include "BasePropPage.h"

class GenericNode;
class ProjectFile;

class BasePropPageCombo
{
public:
 BasePropPage* page;
 int count;

 BasePropPageCombo(BasePropPage* newPage = NULL)
 {
  page=newPage;
  count=1;
 }

 ~BasePropPageCombo()
 {
  if (page!=NULL) delete page;
 }
};

typedef CMap<UINT,UINT,BasePropPageCombo*,BasePropPageCombo*> PropPageMap;

class PropPageList  
{
public:
	void UpdatePagePositions(BasePropPage* page);
	BasePropPage* GetPage(UINT pageID);
	void RemovePage(UINT pageID);
	void AddPage(UINT pageID, BasePropPage* page);
	PropPageList();
	virtual ~PropPageList();

	void ShowPage(UINT pageID, void* selectedObj);

 PropPageMap propPageMap;
 BasePropPage* activePage;
};

#endif // !defined(AFX_PROPPAGELIST_H__F75AE54B_B56E_11D2_BBB8_00C04FB17504__INCLUDED_)
