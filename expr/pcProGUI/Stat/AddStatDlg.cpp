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
// AddStatDlg.cpp : implementation file
//

#include "stdafx.h"
#include "..\pcprogui.h"
#include "AddStatDlg.h"

#include "ScrollingStat.h"
#include "ZeroOneScrollingStat.h"
#include "TextStat.h"
#include "PercentageStat.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AddStatDlg dialog


AddStatDlg::AddStatDlg(CWnd* pParent /*=NULL*/)
	: CDialog(AddStatDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(AddStatDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
 newStat=NULL;
}


void AddStatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AddStatDlg)
	DDX_Control(pDX, IDOK, m_OK);
	DDX_Control(pDX, IDC_DISP_LIST, m_DisplayList);
	DDX_Control(pDX, IDC_STAT_LIST, m_StatList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AddStatDlg, CDialog)
	//{{AFX_MSG_MAP(AddStatDlg)
	ON_LBN_SELCHANGE(IDC_DISP_LIST, OnSelChangeDispList)
	ON_LBN_SELCHANGE(IDC_STAT_LIST, OnSelChangeStatList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AddStatDlg message handlers

void AddStatDlg::OnOK() 
{
	int stat=m_StatList.GetItemData(m_StatList.GetCurSel());
	int disp=m_DisplayList.GetItemData(m_DisplayList.GetCurSel());

 //create a new stat and add it to the simulator object

 //create a new GUI stat and 
 switch(disp)
 {
 case 2:
  newStat=new ScrollingStat();
  break;

 case 1:
  newStat=new ZeroOneScrollingStat();
  break;

 case 4:
  newStat=new TextStat();
  break;

 case 3:
  newStat=new PercentageStat();
  break;

 default:
  ASSERT(0);
  break;
 }
	
 //tell the simulator to hook the simulator stat to the GUI stat
 

	CDialog::OnOK();
}

BOOL AddStatDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_DisplayList.SetItemData(m_DisplayList.AddString("0/1 Scrolling Statistic"), 1);
	m_DisplayList.SetItemData(m_DisplayList.AddString("Scrolling Statistic"), 2);
	m_DisplayList.SetItemData(m_DisplayList.AddString("Percentage Statistic"), 3);
	m_DisplayList.SetItemData(m_DisplayList.AddString("Text Statistic"), 4);
	
	m_StatList.SetItemData(m_StatList.AddString("Some Stat"), 1);

	return TRUE;
}

void AddStatDlg::OnSelChangeDispList() 
{
	m_OK.EnableWindow(m_DisplayList.GetCurSel()!=LB_ERR && m_StatList.GetCurSel()!=LB_ERR);	
}

void AddStatDlg::OnSelChangeStatList() 
{
	m_OK.EnableWindow(m_DisplayList.GetCurSel()!=LB_ERR && m_StatList.GetCurSel()!=LB_ERR);	
}
