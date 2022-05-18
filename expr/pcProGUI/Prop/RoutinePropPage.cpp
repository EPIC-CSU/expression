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
// RoutinePropPage.cpp : implementation file
//

#include "stdafx.h"
#include "..\pcprogui.h"
#include "Routine.h"
#include "RoutinePropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// RoutinePropPage dialog


RoutinePropPage::RoutinePropPage(CWnd* pParent /*=NULL*/)
	: BasePropPage(RoutinePropPage::IDD, pParent)
{
	//{{AFX_DATA_INIT(RoutinePropPage)
	m_Name = _T("");
	//}}AFX_DATA_INIT
}


void RoutinePropPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(RoutinePropPage)
	DDX_Control(pDX, IDC_UNDO, m_Undo);
	DDX_Control(pDX, IDC_APPLY, m_Apply);
	DDX_Text(pDX, IDC_NAME, m_Name);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(RoutinePropPage, CDialog)
	//{{AFX_MSG_MAP(RoutinePropPage)
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_APPLY, OnApply)
	ON_BN_CLICKED(IDC_UNDO, OnUndo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// RoutinePropPage message handlers

void RoutinePropPage::OnMove(int x, int y) 
{
	CDialog::OnMove(x, y);
	MovePage(this);
}

void RoutinePropPage::OnApply() 
{
 ONAPPLY
}

void RoutinePropPage::OnUndo() 
{
 ONUNDO
}

void RoutinePropPage::GetDataFromObject()
{
 Routine* obj=(Routine*)(attachedObj);
 
 CString name;

	//m_CustomProp = obj->customProperties;
 name=obj->getName();
 name+= "()";
 m_Name = name;
 UpdateData(false);
 m_Undo.EnableWindow(false); 
 m_Apply.EnableWindow(false); 
}

void RoutinePropPage::PutDataIntoObject()
{
 Routine* obj=(Routine*)(attachedObj);
 UpdateData(true);

 //no name updates
 //obj->name=m_Name;
}

