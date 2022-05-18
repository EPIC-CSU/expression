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
// InstrPropPage.cpp : implementation file
//

#include "stdafx.h"
#include "..\pcprogui.h"
#include "InstrPropPage.h"

#include "../MainFrm.h"

//#include "../CFG/CFGDoc.h"
#include "../CFG/CFGView.h"

#include "../ILG/ILGDoc.h"
#include "../ILG/ILGView.h"

#include "../HTG/HTGView.h"

#include "Routine.h"

#include "../../pcProDll/percolation/PercDefines.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// InstrPropPage dialog

__declspec (dllimport) bool updateLabel;

InstrPropPage::InstrPropPage(CWnd* pParent /*=NULL*/)
	: BasePropPage(InstrPropPage::IDD, pParent)
{
	//{{AFX_DATA_INIT(InstrPropPage)
	m_Name = _T("");
	m_Address = _T("");
	//}}AFX_DATA_INIT
}


void InstrPropPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(InstrPropPage)
	DDX_Control(pDX, IDC_UNIFY, m_Unify);
	DDX_Control(pDX, IDC_MOVE_OP, m_MoveOP);
	DDX_Control(pDX, IDC_MOVE_CJ, m_MoveCJ);
	DDX_Control(pDX, IDC_APPLY, m_Apply);
	DDX_Control(pDX, IDC_UNDO, m_Undo);
	DDX_Control(pDX, IDC_OP_LIST, m_OpList);
	DDX_Text(pDX, IDC_NAME, m_Name);
	DDX_Text(pDX, IDC_ADDRESS2, m_Address);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(InstrPropPage, CDialog)
	//{{AFX_MSG_MAP(InstrPropPage)
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_MOVE_CJ, OnMoveCJ)
	ON_BN_CLICKED(IDC_MOVE_OP, OnMoveOP)
	ON_BN_CLICKED(IDC_UNIFY, OnUnify)
	ON_BN_CLICKED(IDC_MIGRATE, OnMigrate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// InstrPropPage message handlers

void InstrPropPage::OnMove(int x, int y) 
{
	CDialog::OnMove(x, y);
	MovePage(this);
}

void InstrPropPage::Reset()
{
 m_Apply.EnableWindow(false);
 m_Undo.EnableWindow(false);
}

void InstrPropPage::OnMoveCJ() 
{
	//find if there's a selection first
 int curSel=m_OpList.GetCurSel();
	
 if (curSel==LB_ERR)
 {
  MessageBox("Select an operation first!", "Percolation Scheduling - moveCJ()", MB_ICONINFORMATION);
  return;
 }

 //find the op address
 BaseOperation* op=(BaseOperation*)m_OpList.GetItemData(curSel);
 
 if (!op->isControlOp())
 {
  MessageBox("Select a ControlOp for moveCJ(). Use moveOp() for DataOp!", "Percolation Scheduling - moveOP()", MB_ICONINFORMATION);
  return;
 }

	POSITION pos = theApp.m_ILG->GetFirstDocPosition();
	ILGDoc* ilgDoc = static_cast<ILGDoc*>(theApp.m_ILG->GetNextDoc(pos)); 

 //percolate it
 int res=op->percMoveCJ(ilgDoc->m_routine);
 
 //refresh the CFG view
 updateLabel=true;
 pos = ilgDoc->GetFirstViewPosition();
 ilgDoc->SetModifiedFlag();
	ILGView* ilgView = static_cast<ILGView*>(ilgDoc->GetNextView(pos));
 ilgView->RedrawWindow();
 updateLabel=false;

 //refresh the instr properties
 ((BaseInstruction*)attachedObj)->DisplayProperties(this);

 if (res!=PERC_OK)
 {
  switch(res)
  {
  case PERC_DEPENDENCE:
   MessageBox("Data dependences prevent operation from percolating!", "Percolation Scheduling - moveCJ()", MB_ICONINFORMATION);
   break;
  case PERC_NO_SLOT:
   MessageBox("Data dependences permit operation percolation\nbut there are no operation slots available!", "Percolation Scheduling - moveCJ()", MB_ICONINFORMATION);
   break;
  default:
   MessageBox("Invalid error code returned by moveCJ()!", "Percolation Scheduling - moveCJ()", MB_ICONINFORMATION);
   break;
  }
 }
}

void InstrPropPage::OnUnify() 
{
	POSITION pos = theApp.m_ILG->GetFirstDocPosition();
	ILGDoc* ilgDoc = static_cast<ILGDoc*>(theApp.m_ILG->GetNextDoc(pos)); 

 //percolate it
 int res=((BaseInstruction*)attachedObj)->percUnifyWrap(ilgDoc->m_routine);

 //refresh the CFG view
 updateLabel=true;
 pos = ilgDoc->GetFirstViewPosition();
 ilgDoc->SetModifiedFlag();
	ILGView* ilgView = static_cast<ILGView*>(ilgDoc->GetNextView(pos));
 ilgView->RedrawWindow();
 updateLabel=false;

 //refresh the instr properties
 ((BaseInstruction*)attachedObj)->DisplayProperties(this);

 if (res!=PERC_OK)
 {
  switch(res)
  {
  case 0:
   MessageBox("Data dependences prevent operation from percolating!", "Percolation Scheduling - unify()", MB_ICONINFORMATION);
   break;
  case PERC_NO_SLOT:
   MessageBox("Data dependences permit operation percolation\nbut there are no operation slots available!", "Percolation Scheduling - unify()", MB_ICONINFORMATION);
   break;
  default:
   MessageBox("Invalid error code returned by unify()!", "Percolation Scheduling - unify()", MB_ICONINFORMATION);
   break;
  }
 } 
}

void InstrPropPage::OnMoveOP() 
{
	//find if there's a selection first
 int curSel=m_OpList.GetCurSel();
	
 if (curSel==LB_ERR)
 {
  MessageBox("Select an operation first!", "Percolation Scheduling - moveOP()", MB_ICONINFORMATION);
  return;
 }

 //find the op address
 BaseOperation* op=(BaseOperation*)m_OpList.GetItemData(curSel);
 
 if (!op->isDataOp())
 {
  MessageBox("Select a DataOp for moveOp(). Use moveCJ() for ControlOp!", "Percolation Scheduling - moveOP()", MB_ICONINFORMATION);
  return;
 }

	POSITION pos = theApp.m_ILG->GetFirstDocPosition();
	ILGDoc* ilgDoc = static_cast<ILGDoc*>(theApp.m_ILG->GetNextDoc(pos)); 

 //percolate it
 //MessageBox("percolate(op)");
 int res=op->percMoveOP(ilgDoc->m_routine);
 
 //refresh the CFG view
 updateLabel=true;
 pos = ilgDoc->GetFirstViewPosition();
 ilgDoc->SetModifiedFlag();
	ILGView* ilgView = static_cast<ILGView*>(ilgDoc->GetNextView(pos));
 ilgView->RedrawWindow();
 updateLabel=false;

 //refresh the instr properties
 ((BaseInstruction*)attachedObj)->DisplayProperties(this);

 if (res!=PERC_OK)
 {
  switch(res)
  {
  case PERC_DEPENDENCE:
   MessageBox("Data dependences prevent operation from percolating!", "Percolation Scheduling - moveOP()", MB_ICONINFORMATION);
   break;
  case PERC_NO_SLOT:
   MessageBox("Data dependences permit operation percolation\nbut there are no operation slots available!", "Percolation Scheduling - moveOP()", MB_ICONINFORMATION);
   break;
  default:
   MessageBox("Invalid error code returned by moveOP()!", "Percolation Scheduling - moveOP()", MB_ICONINFORMATION);
   break;
  }
 }
}


void InstrPropPage::OnMigrate() 
{
	//find if there's a selection first
 int curSel=m_OpList.GetCurSel();
	
 if (curSel==LB_ERR)
 {
  MessageBox("Select an operation first!", "Percolation Scheduling - moveOP()", MB_ICONINFORMATION);
  return;
 }

 //find the op address
 BaseOperation* op=(BaseOperation*)m_OpList.GetItemData(curSel);

 //CView* activeView=theApp.pMainForm->GetActiveView();

CMDIFrameWnd *pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
CMDIChildWnd *pChild = (CMDIChildWnd *) pFrame->GetActiveFrame();
CView* activeView = pChild->GetActiveView();

//	POSITION pos = theApp.m_ILG->GetFirstDocPosition();
//	ILGDoc* ilgDoc = static_cast<ILGDoc*>(theApp.m_ILG->GetNextDoc(pos)); 
//
// //percolate it
// //MessageBox("percolate(op)");
// int res=op->percMigrate(ilgDoc->m_routine);
 int res;
 
	ILGView* ilgView = dynamic_cast<ILGView*>(activeView); 
 if (ilgView!=NULL)
 {
  res=op->percMigrate(ilgView->m_routine);
 }
 else
 {
 	CFGView* cfgView = dynamic_cast<CFGView*>(activeView); 
  if (cfgView!=NULL)
  {
   res=op->percMigrate(cfgView->m_routine);
  }
  else
  {
   HTGView* htgView = dynamic_cast<HTGView*>(activeView); 
   if (htgView!=NULL)
   {
    res=op->percMigrate(htgView->m_routine);
   }
   else
   {
    ASSERT(0);
   }
  }
 }
 
 //refresh the CFG view
// updateLabel=true;
// POSITION pos = ilgDoc->GetFirstViewPosition();
// ilgDoc->SetModifiedFlag();
//	ILGView* ilgView = static_cast<ILGView*>(ilgDoc->GetNextView(pos));
// ilgView->RedrawWindow();
// updateLabel=false;
 extern void NotifyDocChange();
 NotifyDocChange();

 //refresh the instr properties
 ((BaseInstruction*)attachedObj)->DisplayProperties(this);

 if (res!=PERC_OK)
 {
  switch(res)
  {
  case PERC_DEPENDENCE:
   MessageBox("Data dependences prevent operation from percolating!", "Percolation Scheduling - moveOP()", MB_ICONINFORMATION);
   break;
  case PERC_NO_SLOT:
   MessageBox("Data dependences permit operation percolation\nbut there are no operation slots available!", "Percolation Scheduling - moveOP()", MB_ICONINFORMATION);
   break;
  default:
   MessageBox("Invalid error code returned by moveOP()!", "Percolation Scheduling - moveOP()", MB_ICONINFORMATION);
   break;
  }
 }
	
}
