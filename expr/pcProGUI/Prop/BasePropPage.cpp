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
// BasePropPage.cpp : implementation file
//

#include "stdafx.h"
#include "..\pcprogui.h"
#include "BasePropPage.h"
#include "PropPageList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// BasePropPage dialog


bool BasePropPage::bNoUpdatePos=false;

BasePropPage::BasePropPage(UINT nIDTemplate, CWnd* pParent /*=NULL*/)
	: CDialog(nIDTemplate, pParent)
{
	//{{AFX_DATA_INIT(BasePropPage)
	//}}AFX_DATA_INIT
 pageID=nIDTemplate;
 bChanged=false;
 bSetting=false;
}


void BasePropPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(BasePropPage)
	DDX_Control(pDX, IDC_UNDO, m_Undo);
	DDX_Control(pDX, IDC_APPLY, m_Apply);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(BasePropPage, CDialog)
	//{{AFX_MSG_MAP(BasePropPage)
	ON_WM_MOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// BasePropPage message handlers

void BasePropPage::OnMove(int x, int y) 
{
	CDialog::OnMove(x, y);
	MovePage(this);
}

void BasePropPage::MyShowWindow(int state)
{
 if (state==SW_HIDE)
 {
  if (IsWindowVisible()) ShowWindow(state);
 }
 else
 {
  if (!IsWindowVisible()) ShowWindow(state);
 }
}

void BasePropPage::MovePage(BasePropPage* refPage)
{
 if (!BasePropPage::bNoUpdatePos)
 {
 	theApp.propPageList->UpdatePagePositions(refPage);	
 }
}


void BasePropPage::GetDataFromObject()
{
}

void BasePropPage::PutDataIntoObject()
{
}


void BasePropPage::Reset()
{
 m_Apply.EnableWindow(false);
 m_Undo.EnableWindow(false);
}
