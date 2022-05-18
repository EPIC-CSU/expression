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
// LatchPropPage.cpp : implementation file
//

#include "stdafx.h"
#include "..\pcprogui.h"
#include "LatchPropPage.h"

#include "../SIM/SIMGraphLatch.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// LatchPropPage dialog


LatchPropPage::LatchPropPage(CWnd* pParent /*=NULL*/)
	: BasePropPage(LatchPropPage::IDD, pParent)
{
	//{{AFX_DATA_INIT(LatchPropPage)
	m_ClassName = _T("");
	m_CustomProp = _T("");
	m_Name = _T("");
	//}}AFX_DATA_INIT
}


void LatchPropPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(LatchPropPage)
	DDX_Control(pDX, IDC_UNDO, m_Undo);
	DDX_Control(pDX, IDC_APPLY, m_Apply);
	DDX_Text(pDX, IDC_CLASS_NAME, m_ClassName);
	DDX_Text(pDX, IDC_CUSTOM_PROP, m_CustomProp);
	DDX_Text(pDX, IDC_NAME2, m_Name);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(LatchPropPage, CDialog)
	//{{AFX_MSG_MAP(LatchPropPage)
	ON_BN_CLICKED(IDC_APPLY, OnApply)
	ON_BN_CLICKED(IDC_UNDO, OnUndo)
	ON_WM_MOVE()
	ON_EN_CHANGE(IDC_CLASS_NAME, OnChangeClassName)
	ON_EN_CHANGE(IDC_CUSTOM_PROP, OnChangeCustomProp)
	ON_EN_CHANGE(IDC_NAME2, OnChangeName2)
	ON_BN_CLICKED(IDC_LATCH_TYPE_INPUT, OnLatchTypeInput)
	ON_BN_CLICKED(IDC_LATCH_TYPE_OTHER, OnLatchTypeOther)
	ON_BN_CLICKED(IDC_LATCH_TYPE_OUTPUT, OnLatchTypeOutput)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// LatchPropPage message handlers

void LatchPropPage::OnApply() 
{
 ONAPPLY
}

void LatchPropPage::OnUndo() 
{
 ONUNDO
}

void LatchPropPage::OnMove(int x, int y) 
{
	CDialog::OnMove(x, y);
	MovePage(this);
}

void LatchPropPage::OnChangeClassName() 
{
 USER_CHANGED_DATA; 
}

void LatchPropPage::OnChangeCustomProp() 
{
 USER_CHANGED_DATA; 
}

void LatchPropPage::OnChangeName2() 
{
 USER_CHANGED_DATA; 
}


void LatchPropPage::PutDataIntoObject()
{
 SIMGraphLatch* obj=(SIMGraphLatch*)(attachedObj);
 UpdateData(true);
	obj->customProperties = m_CustomProp;
	obj->name = m_Name;
	obj->classname = m_ClassName;
 if (IsDlgButtonChecked(IDC_LATCH_TYPE_INPUT)) obj->latchtype=1;
 if (IsDlgButtonChecked(IDC_LATCH_TYPE_OUTPUT)) obj->latchtype=2;
 if (IsDlgButtonChecked(IDC_LATCH_TYPE_OTHER)) obj->latchtype=3;
}

void LatchPropPage::GetDataFromObject()
{
 SIMGraphLatch* obj=(SIMGraphLatch*)(attachedObj);
	m_CustomProp = obj->customProperties;
	m_Name = obj->name;
	m_ClassName = obj->classname;
 switch (obj->latchtype)
 {
  case 1: 
   CheckDlgButton(IDC_LATCH_TYPE_INPUT,1);
   CheckDlgButton(IDC_LATCH_TYPE_OUTPUT,0);
   CheckDlgButton(IDC_LATCH_TYPE_OTHER,0);
   //OnPortTypeRead();
   break;
  case 2: 
   CheckDlgButton(IDC_LATCH_TYPE_INPUT,0);
   CheckDlgButton(IDC_LATCH_TYPE_OUTPUT,1);
   CheckDlgButton(IDC_LATCH_TYPE_OTHER,0);
   //OnPortTypeWrite();
   break;

  case 3: 
   CheckDlgButton(IDC_LATCH_TYPE_INPUT,0);
   CheckDlgButton(IDC_LATCH_TYPE_OUTPUT,0);
   CheckDlgButton(IDC_LATCH_TYPE_OTHER,1);
   //OnPortTypeReadwrite();
   break;
 }
 UpdateData(false);
 m_Undo.EnableWindow(false); 
 m_Apply.EnableWindow(false); 
}

void LatchPropPage::OnLatchTypeInput() 
{
 USER_CHANGED_DATA; 
}

void LatchPropPage::OnLatchTypeOther() 
{
 USER_CHANGED_DATA; 
}

void LatchPropPage::OnLatchTypeOutput() 
{
 USER_CHANGED_DATA; 
}
