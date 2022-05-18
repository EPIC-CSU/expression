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
// ConnectionPropPage.cpp : implementation file
//

#include "stdafx.h"
#include "..\pcprogui.h"
#include "ConnectionPropPage.h"

#include "../SIM/SIMGraphConnection.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ConnectionPropPage dialog


ConnectionPropPage::ConnectionPropPage(CWnd* pParent /*=NULL*/)
: BasePropPage(ConnectionPropPage::IDD, pParent)
{
	//{{AFX_DATA_INIT(ConnectionPropPage)
	m_CustomProp = _T("");
	m_Name = _T("");
	m_ClassName = _T("");
	//}}AFX_DATA_INIT
}


void ConnectionPropPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ConnectionPropPage)
	DDX_Control(pDX, IDC_UNDO, m_Undo);
	DDX_Control(pDX, IDC_APPLY, m_Apply);
	DDX_Text(pDX, IDC_CUSTOM_PROP, m_CustomProp);
	DDX_Text(pDX, IDC_NAME2, m_Name);
	DDX_Text(pDX, IDC_CLASS_NAME2, m_ClassName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(ConnectionPropPage, CDialog)
	//{{AFX_MSG_MAP(ConnectionPropPage)
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_APPLY, OnApply)
	ON_BN_CLICKED(IDC_UNDO, OnUndo)
	ON_EN_CHANGE(IDC_CLASS_NAME2, OnChangeClassName2)
	ON_EN_CHANGE(IDC_CUSTOM_PROP, OnChangeCustomProp)
	ON_EN_CHANGE(IDC_NAME2, OnChangeName2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ConnectionPropPage message handlers

void ConnectionPropPage::OnMove(int x, int y) 
{
	CDialog::OnMove(x, y);
	MovePage(this);
}

void ConnectionPropPage::OnApply() 
{
 ONAPPLY
}

void ConnectionPropPage::OnUndo() 
{
 ONUNDO
}

void ConnectionPropPage::OnChangeClassName2() 
{
 USER_CHANGED_DATA; 
}

void ConnectionPropPage::OnChangeCustomProp() 
{
 USER_CHANGED_DATA; 
}

void ConnectionPropPage::OnChangeName2() 
{
 USER_CHANGED_DATA; 
}

void ConnectionPropPage::PutDataIntoObject()
{
 SIMGraphConnection* obj=(SIMGraphConnection*)(attachedObj);
 UpdateData(true);
	obj->customProperties = m_CustomProp;
	obj->name = m_Name;
	obj->classname = m_ClassName;
}

void ConnectionPropPage::GetDataFromObject()
{
 SIMGraphConnection* obj=(SIMGraphConnection*)(attachedObj);
	m_CustomProp = obj->customProperties;
	m_Name = obj->name;
	m_ClassName = obj->classname;
 UpdateData(false);
 m_Undo.EnableWindow(false); 
 m_Apply.EnableWindow(false); 
}
