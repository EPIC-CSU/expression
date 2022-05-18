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
// FilePropPage.cpp : implementation file
//

#include "stdafx.h"
#include "..\pcprogui.h"
#include "..\ProjectFile.h"
#include "FilePropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// FilePropPage dialog


FilePropPage::FilePropPage(CWnd* pParent /*=NULL*/)
	: BasePropPage(FilePropPage::IDD, pParent)
{
	//{{AFX_DATA_INIT(FilePropPage)
	m_Name = _T("");
	m_AfterTBZ = FALSE;
	m_DoMut = FALSE;
	m_DoTbz = FALSE;
	m_DoTbzPipe = FALSE;
	m_PrintCFG = FALSE;
	m_PrintHTG = FALSE;
	m_PrintIL = FALSE;
	m_SimHTG = FALSE;
	m_DoISel = FALSE;
	m_SimISel = FALSE;
	m_SimNoSSA = FALSE;
	m_SimSSA = FALSE;
	m_SimTbz = FALSE;
	m_PrintMC = FALSE;
	m_DoUnroll = FALSE;
	//}}AFX_DATA_INIT
}


void FilePropPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(FilePropPage)
	DDX_Control(pDX, IDC_UNDO, m_Undo);
	DDX_Control(pDX, IDC_APPLY, m_Apply);
	DDX_Text(pDX, IDC_NAME, m_Name);
	DDX_Check(pDX, IDC_AFTER_TBZ, m_AfterTBZ);
	DDX_Check(pDX, IDC_DO_MUT, m_DoMut);
	DDX_Check(pDX, IDC_DO_TBZ, m_DoTbz);
	DDX_Check(pDX, IDC_DO_TBZ_PIPE, m_DoTbzPipe);
	DDX_Check(pDX, IDC_PRINT_CFG, m_PrintCFG);
	DDX_Check(pDX, IDC_PRINT_HTG, m_PrintHTG);
	DDX_Check(pDX, IDC_PRINT_IL, m_PrintIL);
	DDX_Check(pDX, IDC_SIM_HTG, m_SimHTG);
	DDX_Check(pDX, IDC_DO_ISEL, m_DoISel);
	DDX_Check(pDX, IDC_SIM_ISEL, m_SimISel);
	DDX_Check(pDX, IDC_SIM_NOSSA, m_SimNoSSA);
	DDX_Check(pDX, IDC_SIM_SSA, m_SimSSA);
	DDX_Check(pDX, IDC_SIM_TBZ, m_SimTbz);
	DDX_Check(pDX, IDC_PRINT_MC, m_PrintMC);
	DDX_Check(pDX, IDC_DO_UNROLL, m_DoUnroll);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(FilePropPage, CDialog)
	//{{AFX_MSG_MAP(FilePropPage)
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_UNDO, OnUndo)
	ON_BN_CLICKED(IDC_APPLY, OnApply)
	ON_BN_CLICKED(IDC_AFTER_TBZ, OnAfterTbz)
	ON_BN_CLICKED(IDC_DO_ISEL, OnDoIsel)
	ON_BN_CLICKED(IDC_DO_MUT, OnDoMut)
	ON_BN_CLICKED(IDC_DO_TBZ, OnDoTbz)
	ON_BN_CLICKED(IDC_DO_TBZ_PIPE, OnDoTbzPipe)
	ON_BN_CLICKED(IDC_PRINT_CFG, OnPrintCfg)
	ON_BN_CLICKED(IDC_PRINT_HTG, OnPrintHtg)
	ON_BN_CLICKED(IDC_PRINT_IL, OnPrintIl)
	ON_BN_CLICKED(IDC_SIM_HTG, OnSimHtg)
	ON_BN_CLICKED(IDC_SIM_ISEL, OnSimIsel)
	ON_BN_CLICKED(IDC_SIM_NOSSA, OnSimNossa)
	ON_BN_CLICKED(IDC_SIM_SSA, OnSimSsa)
	ON_BN_CLICKED(IDC_SIM_TBZ, OnSimTbz)
	ON_BN_CLICKED(IDC_DO_UNROLL, OnDoUnroll)
	ON_BN_CLICKED(IDC_PRINT_MC, OnPrintMC)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// FilePropPage message handlers


void FilePropPage::OnMove(int x, int y) 
{
	CDialog::OnMove(x, y);
	MovePage(this);
}

void FilePropPage::Reset()
{
 m_Apply.EnableWindow(false);
 m_Undo.EnableWindow(false);
}

void FilePropPage::OnUndo() 
{
 ONUNDO
}

void FilePropPage::OnApply() 
{
 ONAPPLY
}

void FilePropPage::OnAfterTbz() 
{
 USER_CHANGED_DATA; 
}

void FilePropPage::OnDoIsel() 
{
 USER_CHANGED_DATA; 
}

void FilePropPage::OnDoMut() 
{
 USER_CHANGED_DATA; 
}

void FilePropPage::OnDoTbz() 
{
 USER_CHANGED_DATA; 
}

void FilePropPage::OnDoTbzPipe() 
{
 USER_CHANGED_DATA; 
}

void FilePropPage::OnPrintCfg() 
{
 USER_CHANGED_DATA; 
}

void FilePropPage::OnPrintHtg() 
{
 USER_CHANGED_DATA; 
}

void FilePropPage::OnPrintIl() 
{
 USER_CHANGED_DATA; 
}

void FilePropPage::OnSimHtg() 
{
 USER_CHANGED_DATA; 
}

void FilePropPage::OnSimIsel() 
{
 USER_CHANGED_DATA; 
}

void FilePropPage::OnSimNossa() 
{
 USER_CHANGED_DATA; 
}

void FilePropPage::OnSimSsa() 
{
 USER_CHANGED_DATA; 
}

void FilePropPage::OnSimTbz() 
{
 USER_CHANGED_DATA; 
}

void FilePropPage::OnDoUnroll() 
{
 USER_CHANGED_DATA; 
}

void FilePropPage::OnPrintMC() 
{
 USER_CHANGED_DATA; 
}

void FilePropPage::GetDataFromObject()
{
 ProjectFile* obj=(ProjectFile*)(attachedObj);

	//m_CustomProp = obj->customProperties;
 m_Name = obj->name;
	m_AfterTBZ = obj->afterTbz;
	m_DoMut = obj->doMut;
	m_DoUnroll = obj->doUnroll;
	m_DoTbz = obj->doTbz;
	//m_DoTbzPipe = obj->?;
	m_PrintCFG = obj->printCFG;
	m_PrintHTG = obj->printHTG;
	m_PrintIL = obj->printIList;
	m_SimHTG = obj->simHTG;
	m_DoISel = obj->doISel;
	m_SimISel = obj->simISel;
	m_SimNoSSA = obj->simNoSSA;
	m_SimSSA = obj->simSSA;
	m_SimTbz = obj->simTbz;
 m_PrintMC = obj->printMC;
 m_DoTbzPipe = obj->pipelinedTbz;

 UpdateData(false);
 m_Undo.EnableWindow(false); 
 m_Apply.EnableWindow(false); 
}

void FilePropPage::PutDataIntoObject()
{
 ProjectFile* obj=(ProjectFile*)(attachedObj);
 UpdateData(true);

 //no name updates
 //obj->name=m_Name;
	obj->afterTbz=m_AfterTBZ;
	obj->doMut=m_DoMut;
	obj->doUnroll=m_DoUnroll;
	obj->doTbz=m_DoTbz;
	//obj->?=m_DoTbzPipe;
	obj->printCFG=m_PrintCFG;
	obj->printHTG=m_PrintHTG;
	obj->printIList=m_PrintIL;
	obj->simHTG=m_SimHTG;
	obj->doISel=m_DoISel;
	obj->simISel=m_SimISel;
	obj->simNoSSA=m_SimNoSSA;
	obj->simSSA=m_SimSSA;
	obj->simTbz=m_SimTbz;
 obj->printMC=m_PrintMC;
 obj->pipelinedTbz=m_DoTbzPipe;

}
