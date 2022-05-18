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
// PipelineStagePropPage.cpp : implementation file
//

#include "stdafx.h"
#include "..\pcprogui.h"
#include "PipelineStagePropPage.h"

#include "../SIM/SIMGraphPipelineStage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// PipelineStagePropPage dialog


PipelineStagePropPage::PipelineStagePropPage(CWnd* pParent /*=NULL*/)
	: BasePropPage(PipelineStagePropPage::IDD, pParent)
{
	//{{AFX_DATA_INIT(PipelineStagePropPage)
	m_Name = _T("");
	//}}AFX_DATA_INIT
}


void PipelineStagePropPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PipelineStagePropPage)
	DDX_Control(pDX, IDC_UNDO, m_Undo);
	DDX_Control(pDX, IDC_APPLY, m_Apply);
	DDX_Text(pDX, IDC_NAME2, m_Name);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PipelineStagePropPage, CDialog)
	//{{AFX_MSG_MAP(PipelineStagePropPage)
	ON_BN_CLICKED(IDC_APPLY, OnApply)
	ON_BN_CLICKED(IDC_UNDO, OnUndo)
	ON_EN_CHANGE(IDC_NAME2, OnChangeName2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PipelineStagePropPage message handlers

void PipelineStagePropPage::OnApply() 
{
	ONAPPLY
}

void PipelineStagePropPage::OnUndo() 
{
 ONUNDO
}

void PipelineStagePropPage::OnChangeName2() 
{
 USER_CHANGED_DATA; 
}




void PipelineStagePropPage::PutDataIntoObject()
{
 SIMGraphPipelineStage* obj=(SIMGraphPipelineStage*)(attachedObj);
 UpdateData(true);
	obj->name = m_Name;
}

void PipelineStagePropPage::GetDataFromObject()
{
 SIMGraphPipelineStage* obj=(SIMGraphPipelineStage*)(attachedObj);
	m_Name = obj->name;
 UpdateData(false);
 m_Undo.EnableWindow(false); 
 m_Apply.EnableWindow(false); 
}
