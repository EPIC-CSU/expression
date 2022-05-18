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
// DataPathsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "..\pcprogui.h"
#include "DataPathsDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DataPathsDialog dialog


DataPathsDialog::DataPathsDialog(CWnd* pParent /*=NULL*/, StringList* pDataPaths)
	: CDialog(DataPathsDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(DataPathsDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
 dataPaths=pDataPaths;
}


void DataPathsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DataPathsDialog)
	DDX_Control(pDX, IDC_DATAPATHS, m_DataPaths);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DataPathsDialog, CDialog)
	//{{AFX_MSG_MAP(DataPathsDialog)
	ON_BN_CLICKED(ID_REMOVE, OnRemove)
	ON_BN_CLICKED(ID_REMOVE_ALL, OnRemoveAll)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DataPathsDialog message handlers

void DataPathsDialog::OnRemove() 
{
 //TODO: upgrade to multiple select

 //int sels=m_DataPaths.GetSelCount();
 //int* selItems=new int[sels];
 //GetSelItems(sels,selItems);

 m_DataPaths.DeleteString(m_DataPaths.GetCurSel());
	
}

void DataPathsDialog::OnOK() 
{
 UpdateData();

 //copy stuff from the list to dataPaths
 dataPaths->RemoveAll();
 for (int i=0;i<m_DataPaths.GetCount();i++)
 {
  CString str;
  m_DataPaths.GetText(i,str);
  dataPaths->AddTail(str);
 }
	CDialog::OnOK();
}

void DataPathsDialog::OnRemoveAll() 
{
 m_DataPaths.ResetContent();
}

BOOL DataPathsDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
 //copy stuff from dataPaths to the list
 POSITION pos=dataPaths->GetHeadPosition();
 while (pos!=NULL)
 {
  m_DataPaths.AddString(dataPaths->GetNext(pos));
 }
 UpdateData(false);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
