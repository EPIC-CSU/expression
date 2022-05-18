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
// InstrDescrDialog.cpp : implementation file
//

#include "stdafx.h"
#include "..\pcprogui.h"
#include "InstrDescrDialog.h"

#include "ISInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// InstrDescrDialog dialog


InstrDescrDialog::InstrDescrDialog(ISInfo* ISInfo, CWnd* pParent /*=NULL*/)
	: CDialog(InstrDescrDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(InstrDescrDialog)
	m_Bitwidth = _T("");
	m_Unit = _T("");
	//}}AFX_DATA_INIT
 isInfo=ISInfo;
}


void InstrDescrDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(InstrDescrDialog)
	DDX_Control(pDX, IDC_WORDLEN, m_Wordlen);
	DDX_Control(pDX, IDC_DELETE_SLOT, m_DeleteSlot);
	DDX_Control(pDX, IDC_TYPE, m_Type);
	DDX_Control(pDX, IDC_ADD_SLOT, m_AddSlot);
	DDX_Control(pDX, IDC_SLOT_LIST, m_SlotList);
	DDX_Control(pDX, IDC_DOWN, m_Down);
	DDX_Control(pDX, IDC_UP, m_Up);
	DDX_CBString(pDX, IDC_BITWIDTH, m_Bitwidth);
	DDX_CBString(pDX, IDC_UNIT, m_Unit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(InstrDescrDialog, CDialog)
	//{{AFX_MSG_MAP(InstrDescrDialog)
	ON_LBN_SELCHANGE(IDC_SLOT_LIST, OnSelChangeSlotList)
	ON_CBN_SELCHANGE(IDC_TYPE, OnSelChangeType)
	ON_CBN_SELCHANGE(IDC_UNIT, OnSelChangeUnit)
	ON_BN_CLICKED(IDC_ADD_SLOT, OnAddSlot)
	ON_BN_CLICKED(IDC_DELETE_SLOT, OnDeleteSlot)
	ON_BN_CLICKED(IDC_DOWN, OnDown)
	ON_BN_CLICKED(IDC_UP, OnUp)
	ON_CBN_EDITCHANGE(IDC_BITWIDTH, OnEditChangeBitwidth)
	ON_CBN_SELCHANGE(IDC_BITWIDTH, OnSelChangeBitwidth)
	ON_CBN_EDITCHANGE(IDC_UNIT, OnEditChangeUnit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// InstrDescrDialog message handlers

BOOL InstrDescrDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	//set the bitmaps
 m_Up.SetIcon(theApp.LoadIcon(IDI_UP));
 m_Down.SetIcon(theApp.LoadIcon(IDI_DOWN));
 m_AddSlot.SetIcon(theApp.LoadIcon(IDI_ADD));
 m_DeleteSlot.SetIcon(theApp.LoadIcon(IDI_DELETE));

 //set tab stops
 int stops[3];
 stops[0]=50;
 stops[1]=100;
 //stops[2]=200;
 VERIFY(m_SlotList.SetTabStops(2, (LPINT)&stops));
	
 //now load the existing instr desc info
 CString s;
 s.Format("%d",isInfo->instrDescr.wordlen);
 m_Wordlen.SetWindowText(s);

 //fill in the slots
 int count=isInfo->instrDescr.slots.GetCount();
 for (int i=0;i<count;i++)
 //POSITION pos=isInfo->instrDescr.slots.GetStartPosition();
 //while (pos!=NULL)
 {
  Slot slot;

  if (isInfo->instrDescr.slots.Lookup(i, slot))
  {
   s.Format("%s\t%d\t%s", slot.type, slot.bitwidth, slot.unit);
   m_SlotList.AddString(s);
  }
 }

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void InstrDescrDialog::OnSelChangeSlotList() 
{
 bool state=(m_SlotList.GetCurSel()!=LB_ERR);
 
 m_Up.EnableWindow(state);
 m_Down.EnableWindow(state);
 m_DeleteSlot.EnableWindow(state);
}

void InstrDescrDialog::OnEditChangeBitwidth() 
{
 //CString temp;
 //m_Bitwidth.GetWindowText(temp);
 //m_AddSlot.EnableWindow((m_Type.GetCurSel()!=LB_ERR) && (temp.GetLength()!=0) && (m_Unit.GetCurSel()!=LB_ERR));
 UpdateData();
 m_AddSlot.EnableWindow((m_Type.GetCurSel()!=LB_ERR) && (m_Bitwidth!="") && (m_Unit!=""));
}

void InstrDescrDialog::OnSelChangeBitwidth() 
{
 //CString temp;
 //m_Bitwidth.GetWindowText(temp);
 //m_AddSlot.EnableWindow((m_Type.GetCurSel()!=LB_ERR) && (temp.GetLength()!=0) && (m_Unit.GetCurSel()!=LB_ERR));
 UpdateData();
 m_AddSlot.EnableWindow((m_Type.GetCurSel()!=LB_ERR) && (m_Bitwidth!="") && (m_Unit!=""));
}

void InstrDescrDialog::OnSelChangeType() 
{
 //CString temp;
 //m_Bitwidth.GetWindowText(temp);
 //m_AddSlot.EnableWindow((m_Type.GetCurSel()!=LB_ERR) && (temp.GetLength()!=0) && (m_Unit.GetCurSel()!=LB_ERR));
 UpdateData();
 m_AddSlot.EnableWindow((m_Type.GetCurSel()!=LB_ERR) && (m_Bitwidth!="") && (m_Unit!=""));
}

void InstrDescrDialog::OnSelChangeUnit() 
{
 //CString temp;
 //m_Bitwidth.GetWindowText(temp);
 //m_AddSlot.EnableWindow((m_Type.GetCurSel()!=LB_ERR) && (temp.GetLength()!=0) && (m_Unit.GetCurSel()!=LB_ERR));
 UpdateData();
 m_AddSlot.EnableWindow((m_Type.GetCurSel()!=LB_ERR) && (m_Bitwidth!="") && (m_Unit!=""));
}

void InstrDescrDialog::OnEditChangeUnit() 
{
 //CString temp;
 //m_Bitwidth.GetWindowText(temp);
 //m_AddSlot.EnableWindow((m_Type.GetCurSel()!=LB_ERR) && (temp.GetLength()!=0) && (m_Unit.GetCurSel()!=LB_ERR));
 UpdateData();
 m_AddSlot.EnableWindow((m_Type.GetCurSel()!=LB_ERR) && (m_Bitwidth!="") && (m_Unit!=""));
}

void InstrDescrDialog::OnAddSlot() 
{
 UpdateData();
 CString s,t,u,b;

 m_Type.GetLBText(m_Type.GetCurSel(),t);
 //m_Unit.GetLBText(m_Unit.GetCurSel(),u);
 //m_Bitwidth.GetWindowText(b);

 s.Format("%s\t%s\t%s", t, m_Bitwidth, m_Unit);
 m_SlotList.AddString(s);

 //reset inputs
 m_Type.SetCurSel(-1); 
 m_Bitwidth="";//.SetCurSel(-1);
 m_Unit="";//.SetCurSel(-1);
 m_Type.SetFocus();
 UpdateData(false);
}

void InstrDescrDialog::OnDeleteSlot() 
{
 m_SlotList.DeleteString(m_SlotList.GetCurSel());
 //disable buttons if necessary
 OnSelChangeSlotList();
}

void InstrDescrDialog::OnDown() 
{
	// TODO: Add your control notification handler code here
	
}

void InstrDescrDialog::OnUp() 
{
	// TODO: Add your control notification handler code here
	
}


void InstrDescrDialog::OnOK() 
{
 CString s;

 UpdateData();

 m_Wordlen.GetWindowText(s);
 long temp=atol(s);

 if (temp<1) 
 {
  MessageBox("Word Length has to be a valid integer greater than 0.", "Invalid Word Length", MB_ICONINFORMATION);
  return;
 }

 isInfo->instrDescr.wordlen=temp;
 
 Slot slot;
 int c=m_SlotList.GetCount();
 isInfo->instrDescr.slots.RemoveAll();
 for (int i=0;i<c;i++)
 {
  CString slotNum;

  m_SlotList.GetText(i,s);
  LPTSTR t=slot.type.GetBuffer(100);
  LPTSTR u=slot.unit.GetBuffer(100);
  slot.pos=i;

  sscanf(s, "%s\t%d\t%s", t, &slot.bitwidth, u);

  slot.type.ReleaseBuffer(-1);
  slot.unit.ReleaseBuffer(-1);

  //isInfo->instrDescr.slots.SetAt(slot.unit, slot);
  isInfo->instrDescr.slots.SetAt(slot.pos, slot);
 }
	
	CDialog::OnOK();
}

