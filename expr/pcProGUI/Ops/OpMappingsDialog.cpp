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
// OpMappingsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "..\pcprogui.h"
#include "OpMappingsDialog.h"

#include "ISInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// OpMappingsDialog dialog


OpMappingsDialog::OpMappingsDialog(ISInfo* ISInfo, CWnd* pParent /*=NULL*/)
	: CDialog(OpMappingsDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(OpMappingsDialog)
	m_Mappings = _T("");
	//}}AFX_DATA_INIT
 isInfo=ISInfo;
}


void OpMappingsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(OpMappingsDialog)
	//DDX_Text(pDX, IDC_MAPPING_TEXT, m_Mappings);
	DDX_Text(pDX, IDC_RICHTEXTCTRL1, m_Mappings);	
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(OpMappingsDialog, CDialog)
	//{{AFX_MSG_MAP(OpMappingsDialog)
	//ON_EVENT(OpMappingsDialog, IDC_RICHTEXTCTRL1, -603 /* KeyPress */, OnKeyPressRichtextctrl1, VTS_PI2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// OpMappingsDialog message handlers

void OpMappingsDialog::OnOK() 
{
 UpdateData();
	isInfo->opMappings.mappings=m_Mappings;
	
	CDialog::OnOK();
}

/*void OpMappingsDialog::OnKeyPressRichtextctrl1(short FAR* KeyAscii) 
{
	// TODO: Add your control notification handler code here
	if (*KeyAscii == VK_RETURN) {
			int a = a+1;	
		}

	
}*/


BOOL OpMappingsDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_Mappings=isInfo->opMappings.mappings;
 UpdateData(false);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
