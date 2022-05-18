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
// PortPropPage.cpp : implementation file
//

#include "stdafx.h"
#include "..\pcprogui.h"
#include "PortPropPage.h"

#include "../SIM/SIMGraphPort.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// PortPropPage dialog


PortPropPage::PortPropPage(CWnd* pParent /*=NULL*/)
	: BasePropPage(PortPropPage::IDD, pParent)
{
	//{{AFX_DATA_INIT(PortPropPage)
	m_CustomProp = _T("");
	m_Name = _T("");
	m_ClassName = _T("");
	//}}AFX_DATA_INIT
}


void PortPropPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PortPropPage)
	DDX_Control(pDX, IDC_UNDO, m_Undo);
	DDX_Control(pDX, IDC_APPLY, m_Apply);
	DDX_Text(pDX, IDC_CUSTOM_PROP, m_CustomProp);
	DDX_Text(pDX, IDC_NAME2, m_Name);
	DDX_Text(pDX, IDC_CLASS_NAME2, m_ClassName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PortPropPage, CDialog)
	//{{AFX_MSG_MAP(PortPropPage)
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_APPLY, OnApply)
	ON_EN_CHANGE(IDC_CLASS_NAME2, OnChangeClassName2)
	ON_EN_CHANGE(IDC_CUSTOM_PROP, OnChangeCustomProp)
	ON_EN_CHANGE(IDC_NAME2, OnChangeName2)
	ON_BN_CLICKED(IDC_UNDO, OnUndo)
	ON_BN_CLICKED(IDC_PORT_TYPE_READ, OnPortTypeRead)
	ON_BN_CLICKED(IDC_PORT_TYPE_READWRITE, OnPortTypeReadwrite)
	ON_BN_CLICKED(IDC_PORT_TYPE_WRITE, OnPortTypeWrite)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PortPropPage message handlers

void PortPropPage::OnMove(int x, int y) 
{
	CDialog::OnMove(x, y);
	MovePage(this);
}

void PortPropPage::OnApply() 
{
 ONAPPLY
}

void PortPropPage::OnUndo() 
{
 ONUNDO
}

void PortPropPage::OnChangeClassName2() 
{
 USER_CHANGED_DATA; 
}

void PortPropPage::OnChangeCustomProp() 
{
 USER_CHANGED_DATA; 
}

void PortPropPage::OnChangeName2() 
{
 USER_CHANGED_DATA; 
}

void PortPropPage::OnPortTypeRead() 
{
 USER_CHANGED_DATA; 
}

void PortPropPage::OnPortTypeReadwrite() 
{
 USER_CHANGED_DATA; 
}

void PortPropPage::OnPortTypeWrite() 
{
 USER_CHANGED_DATA; 
}

void PortPropPage::PutDataIntoObject()
{
 SIMGraphPort* obj=(SIMGraphPort*)(attachedObj);
 UpdateData(true);
	obj->customProperties = m_CustomProp;
	obj->name = m_Name;
	obj->classname = m_ClassName;
 if (IsDlgButtonChecked(IDC_PORT_TYPE_READ)) obj->porttype=1;
 if (IsDlgButtonChecked(IDC_PORT_TYPE_WRITE)) obj->porttype=2;
 if (IsDlgButtonChecked(IDC_PORT_TYPE_READWRITE)) obj->porttype=3;
}

void PortPropPage::GetDataFromObject()
{
 SIMGraphPort* obj=(SIMGraphPort*)(attachedObj);
	m_CustomProp = obj->customProperties;
	m_Name = obj->name;
	m_ClassName = obj->classname;
 //bSetting=true; 
 switch (obj->porttype)
 {
  case 1: 
   CheckDlgButton(IDC_PORT_TYPE_READ,1);
   CheckDlgButton(IDC_PORT_TYPE_WRITE,0);
   CheckDlgButton(IDC_PORT_TYPE_READWRITE,0);
   //OnPortTypeRead();
   break;
  case 2: 
   CheckDlgButton(IDC_PORT_TYPE_READ,0);
   CheckDlgButton(IDC_PORT_TYPE_WRITE,1);
   CheckDlgButton(IDC_PORT_TYPE_READWRITE,0);
   //OnPortTypeWrite();
   break;

  case 3: 
   CheckDlgButton(IDC_PORT_TYPE_READ,0);
   CheckDlgButton(IDC_PORT_TYPE_WRITE,0);
   CheckDlgButton(IDC_PORT_TYPE_READWRITE,1);
   //OnPortTypeReadwrite();
   break;
 }
 //bSetting=false; 
 UpdateData(false);
 m_Undo.EnableWindow(false); 
 m_Apply.EnableWindow(false); 
}

