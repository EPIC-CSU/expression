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
// TempDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pcprogui.h"
#include "TempDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// TempDlg dialog


TempDlg::TempDlg(int* width, int* height, CWnd* pParent /*=NULL*/)
	: CDialog(TempDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(TempDlg)
	m_Height = 0;
	m_Width = 0;
	//}}AFX_DATA_INIT
	m_AHeight = height;
	m_AWidth = width;
}

void TempDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(TempDlg)
	DDX_Text(pDX, IDC_HEIGHT, m_Height);
	DDX_Text(pDX, IDC_WIDTH, m_Width);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(TempDlg, CDialog)
	//{{AFX_MSG_MAP(TempDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// TempDlg message handlers

void TempDlg::OnOK() 
{
 UpdateData();
	*m_AHeight = m_Height;
	*m_AWidth = m_Width;
	
	CDialog::OnOK();
}

BOOL TempDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_Height=*m_AHeight;
	m_Width=*m_AWidth;
 UpdateData(false);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
