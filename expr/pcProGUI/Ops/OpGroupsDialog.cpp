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
// OpGroupsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "..\pcprogui.h"
#include "OpGroupsDialog.h"

#include "OpGroups.h"
#include "TreeThruList.h"

#include "ISInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// OpGroupsDialog dialog


OpGroupsDialog::OpGroupsDialog(ISInfo* ISInfo, CWnd* pParent /*=NULL*/)
	: CDialog(OpGroupsDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(OpGroupsDialog)
	m_Operand11 = -1;
	m_Operand12 = -1;
	m_Operand13 = -1;
	m_Operand14 = -1;
	m_Operand22 = -1;
	m_Operand23 = -1;
	m_Operand24 = -1;
	m_Operand31 = -1;
	m_Operand32 = -1;
	m_Operand33 = -1;
	m_Operand34 = -1;
	m_OperandType11 = _T("");
	m_OperandType12 = _T("");
	m_OperandType13 = _T("");
	m_OperandType14 = _T("");
	m_OperandType21 = _T("");
	m_OperandType22 = _T("");
	m_OperandType23 = _T("");
	m_OperandType24 = _T("");
	m_OperandType31 = _T("");
	m_OperandType32 = _T("");
	m_OperandType33 = _T("");
	m_OperandType34 = _T("");
	m_Edit2 = _T("");
	m_Edit3 = _T("");
	m_Behavior = _T("");
	m_OpType = -1;
	m_Operand21 = -1;
	m_Name = _T("");
	m_Check1 = FALSE;
	m_Check2 = FALSE;
	m_Check3 = FALSE;
	//}}AFX_DATA_INIT
 isInfo=ISInfo;
}


void OpGroupsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(OpGroupsDialog)
	DDX_Control(pDX, IDC_DOWN, m_Down);
	DDX_Control(pDX, IDC_UP, m_Up);
	DDX_Control(pDX, IDC_DELETE_OP, m_DeleteOp);
	DDX_Control(pDX, IDC_DELETE_GROUP, m_DeleteGroup);
	DDX_Control(pDX, IDC_ADD_OP, m_AddOp);
	DDX_Control(pDX, IDC_ADD_GROUP, m_AddGroup);
	DDX_Control(pDX, IDC_OP_TREE, m_OpTreeList);
	DDX_CBIndex(pDX, IDC_OPERAND_11, m_Operand11);
	DDX_CBIndex(pDX, IDC_OPERAND_12, m_Operand12);
	DDX_CBIndex(pDX, IDC_OPERAND_13, m_Operand13);
	DDX_CBIndex(pDX, IDC_OPERAND_14, m_Operand14);
	DDX_CBIndex(pDX, IDC_OPERAND_22, m_Operand22);
	DDX_CBIndex(pDX, IDC_OPERAND_23, m_Operand23);
	DDX_CBIndex(pDX, IDC_OPERAND_24, m_Operand24);
	DDX_CBIndex(pDX, IDC_OPERAND_31, m_Operand31);
	DDX_CBIndex(pDX, IDC_OPERAND_32, m_Operand32);
	DDX_CBIndex(pDX, IDC_OPERAND_33, m_Operand33);
	DDX_CBIndex(pDX, IDC_OPERAND_34, m_Operand34);
	DDX_CBString(pDX, IDC_OPERAND_TYPE_11, m_OperandType11);
	DDX_CBString(pDX, IDC_OPERAND_TYPE_12, m_OperandType12);
	DDX_CBString(pDX, IDC_OPERAND_TYPE_13, m_OperandType13);
	DDX_CBString(pDX, IDC_OPERAND_TYPE_14, m_OperandType14);
	DDX_CBString(pDX, IDC_OPERAND_TYPE_21, m_OperandType21);
	DDX_CBString(pDX, IDC_OPERAND_TYPE_22, m_OperandType22);
	DDX_CBString(pDX, IDC_OPERAND_TYPE_23, m_OperandType23);
	DDX_CBString(pDX, IDC_OPERAND_TYPE_24, m_OperandType24);
	DDX_CBString(pDX, IDC_OPERAND_TYPE_31, m_OperandType31);
	DDX_CBString(pDX, IDC_OPERAND_TYPE_32, m_OperandType32);
	DDX_CBString(pDX, IDC_OPERAND_TYPE_33, m_OperandType33);
	DDX_CBString(pDX, IDC_OPERAND_TYPE_34, m_OperandType34);
	DDX_Text(pDX, IDC_BEHAVIOR, m_Behavior);
	DDX_CBIndex(pDX, IDC_TYPE, m_OpType);
	DDX_CBIndex(pDX, IDC_OPERAND_21, m_Operand21);
	DDX_Text(pDX, IDC_NAME, m_Name);
	DDX_Check(pDX, IDC_CHECK1, m_Check1);
	DDX_Check(pDX, IDC_CHECK2, m_Check2);
	DDX_Check(pDX, IDC_CHECK3, m_Check3);
	DDX_Text(pDX, IDC_EDIT2, m_Edit2);
	DDX_Text(pDX, IDC_EDIT3, m_Edit3);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(OpGroupsDialog, CDialog)
	//{{AFX_MSG_MAP(OpGroupsDialog)
	ON_BN_CLICKED(IDC_ADD_GROUP, OnAddGroup)
	ON_BN_CLICKED(IDC_ADD_OP, OnAddOp)
	ON_BN_CLICKED(IDC_DELETE_GROUP, OnDeleteGroup)
	ON_BN_CLICKED(IDC_DELETE_OP, OnDeleteOp)
	ON_WM_DRAWITEM()
	ON_LBN_DBLCLK(IDC_OP_TREE, OnDblClkOpTree)
	ON_LBN_SELCHANGE(IDC_OP_TREE, OnSelChangeOpTree)
	ON_BN_CLICKED(IDC_CHECK1, OnCheck1)
	ON_BN_CLICKED(IDC_CHECK2, OnCheck2)
	ON_BN_CLICKED(IDC_CHECK3, OnCheck3)
	ON_EN_CHANGE(IDC_BEHAVIOR, OnChangeBehavior)
	ON_EN_CHANGE(IDC_NAME, OnChangeName)
	ON_EN_CHANGE(IDC_EDIT2, OnChangeEdit2)
	ON_EN_CHANGE(IDC_EDIT3, OnChangeEdit3)
	ON_CBN_SELCHANGE(IDC_OPERAND_11, OnSelchangeOperand11)
	ON_CBN_SELCHANGE(IDC_OPERAND_12, OnSelchangeOperand12)
	ON_CBN_SELCHANGE(IDC_OPERAND_13, OnSelchangeOperand13)
	ON_CBN_SELCHANGE(IDC_OPERAND_14, OnSelchangeOperand14)
	ON_CBN_SELCHANGE(IDC_OPERAND_21, OnSelchangeOperand21)
	ON_CBN_SELCHANGE(IDC_OPERAND_22, OnSelchangeOperand22)
	ON_CBN_SELCHANGE(IDC_OPERAND_23, OnSelchangeOperand23)
	ON_CBN_SELCHANGE(IDC_OPERAND_24, OnSelchangeOperand24)
	ON_CBN_SELCHANGE(IDC_OPERAND_31, OnSelchangeOperand31)
	ON_CBN_SELCHANGE(IDC_OPERAND_32, OnSelchangeOperand32)
	ON_CBN_SELCHANGE(IDC_OPERAND_33, OnSelchangeOperand33)
	ON_CBN_SELCHANGE(IDC_OPERAND_34, OnSelchangeOperand34)
	ON_CBN_EDITCHANGE(IDC_OPERAND_TYPE_11, OnEditchangeOperandType11)
	ON_CBN_EDITCHANGE(IDC_OPERAND_TYPE_12, OnEditchangeOperandType12)
	ON_CBN_EDITCHANGE(IDC_OPERAND_TYPE_13, OnEditchangeOperandType13)
	ON_CBN_EDITCHANGE(IDC_OPERAND_TYPE_14, OnEditchangeOperandType14)
	ON_CBN_EDITCHANGE(IDC_OPERAND_TYPE_21, OnEditchangeOperandType21)
	ON_CBN_EDITCHANGE(IDC_OPERAND_TYPE_22, OnEditchangeOperandType22)
	ON_CBN_EDITCHANGE(IDC_OPERAND_TYPE_23, OnEditchangeOperandType23)
	ON_CBN_EDITCHANGE(IDC_OPERAND_TYPE_24, OnEditchangeOperandType24)
	ON_CBN_EDITCHANGE(IDC_OPERAND_TYPE_31, OnEditchangeOperandType31)
	ON_CBN_EDITCHANGE(IDC_OPERAND_TYPE_32, OnEditchangeOperandType32)
	ON_CBN_EDITCHANGE(IDC_OPERAND_TYPE_33, OnEditchangeOperandType33)
	ON_CBN_EDITCHANGE(IDC_OPERAND_TYPE_34, OnEditchangeOperandType34)
	ON_CBN_SELCHANGE(IDC_TYPE, OnSelchangeType)
	ON_BN_CLICKED(IDC_APPLY, OnApply)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// OpGroupsDialog message handlers

BOOL OpGroupsDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
 m_Up.SetIcon(theApp.LoadIcon(IDI_UP));
 m_Down.SetIcon(theApp.LoadIcon(IDI_DOWN));
 m_AddOp.SetIcon(theApp.LoadIcon(IDI_ADD_OP));
 m_AddGroup.SetIcon(theApp.LoadIcon(IDI_ADD_GROUP));
 m_DeleteOp.SetIcon(theApp.LoadIcon(IDI_DEL_OP));
 m_DeleteGroup.SetIcon(theApp.LoadIcon(IDI_DEL_GROUP));

 //add some groups and items
 if (0)
 {
  OGList& ogl=isInfo->opGroups.opGroupItemList;
  ogl.AddTail(OG(OG_GROUP,0,&CString("group0")));
  ogl.AddTail(OG(OG_OP,1,&CString("item1")));
  ogl.AddTail(OG(OG_GROUP,0,&CString("group2")));
  ogl.AddTail(OG(OG_GROUP,1,&CString("group3")));
  ogl.AddTail(OG(OG_OP,2,&CString("item4")));
  ogl.AddTail(OG(OG_OP,2,&CString("item5")));
  ogl.AddTail(OG(OG_GROUP,1,&CString("group6")));
  ogl.AddTail(OG(OG_GROUP,0,&CString("group7")));
  ogl.AddTail(OG(OG_OP,1,&CString("item8")));
  ogl.AddTail(OG(OG_GROUP,0,&CString("group9")));
  ogl.AddTail(OG(OG_OP,1,&CString("item10")));
 }

 //fill in the group treelist
 TreeThruList::FillGroupsList(isInfo, m_OpTreeList);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void OpGroupsDialog::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
 if (lpDrawItemStruct->itemID!=-1)
 {
  OG* og=(OG*)m_OpTreeList.GetItemData(lpDrawItemStruct->itemID);
  //changed by Sudeep 11/3
  ASSERT(og);

  TreeThruList::OnDrawItem(og, nIDCtl, lpDrawItemStruct);

/*
  SIZE size;
  RECT r;
  memcpy(&r, &lpDrawItemStruct->rcItem, sizeof(RECT));
  r.left+=og->level*15;
  VERIFY(GetTextExtentPoint32(lpDrawItemStruct->hDC, og->text, og->text.GetLength(), &size));
  r.right=r.left+size.cx+3;

  int saveID=SaveDC(lpDrawItemStruct->hDC);
  ASSERT(saveID);

  CBrush eraseBrush;
  CPen erasePen;
  HGDIOBJ oldBrush;
  HGDIOBJ oldPen;

  //erase the background
  erasePen.CreateStockObject(NULL_PEN);
  eraseBrush.CreateSolidBrush(GetSysColor(COLOR_WINDOW));
  oldBrush = SelectObject(lpDrawItemStruct->hDC,eraseBrush);
  oldPen = SelectObject(lpDrawItemStruct->hDC,erasePen);
  Rectangle(lpDrawItemStruct->hDC, lpDrawItemStruct->rcItem.left, lpDrawItemStruct->rcItem.top, lpDrawItemStruct->rcItem.right, lpDrawItemStruct->rcItem.bottom);
  SelectObject(lpDrawItemStruct->hDC,oldPen);
  SelectObject(lpDrawItemStruct->hDC,oldBrush);

  CBrush newBrush;
  CPen newPen;
  //if (lpDrawItemStruct->itemState & ODS_FOCUS) //draw a focus rectangle
  //{
  // newPen.CreatePen(PS_DOT, 1, GetSysColor(COLOR_GRAYTEXT));
  //}
  //else
  //{
   newPen.CreateStockObject(NULL_PEN);
  //}

  oldPen = SelectObject(lpDrawItemStruct->hDC,newPen);

  if (lpDrawItemStruct->itemState & ODS_SELECTED) //use the system selected color
  {
   newBrush.CreateSolidBrush(lpDrawItemStruct->itemState & ODS_SELECTED ? GetSysColor(COLOR_HIGHLIGHT) : GetSysColor(COLOR_GRAYTEXT));
  }
  else //no brush
  {
   newBrush.CreateSolidBrush(::GetSysColor(COLOR_WINDOW));
  }

  oldBrush = SelectObject(lpDrawItemStruct->hDC,newBrush);
  //Rectangle(lpDrawItemStruct->hDC, 
  //           lpDrawItemStruct->rcItem.left, 
  //           lpDrawItemStruct->rcItem.top, 
  //           lpDrawItemStruct->rcItem.right, 
  //           lpDrawItemStruct->rcItem.bottom);
  Rectangle(lpDrawItemStruct->hDC, r.left, r.top, r.right, r.bottom);

  SelectObject(lpDrawItemStruct->hDC,oldPen);
  SelectObject(lpDrawItemStruct->hDC,oldBrush);

  SetTextColor(lpDrawItemStruct->hDC, lpDrawItemStruct->itemState & ODS_SELECTED ? GetSysColor(COLOR_HIGHLIGHTTEXT) : GetSysColor(COLOR_WINDOWTEXT));

  //now let the stat item draw itself
  //og->OnDraw(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem);
  og->OnDraw(lpDrawItemStruct->hDC, &r);
  RestoreDC(lpDrawItemStruct->hDC,saveID);  
*/
 }
}


void OpGroupsDialog::OnDblClkOpTree() 
{
 //toggle the expanded attribute
 int index=m_OpTreeList.GetCurSel();
	OG* item=((OG*)(m_OpTreeList.GetItemData(index)));	
 item->isExpanded=!item->isExpanded;
 TreeThruList::FillGroupsList(isInfo, m_OpTreeList);
 m_OpTreeList.SetSel(index);
}


void OpGroupsDialog::OnSelChangeOpTree() 
{
 int selCount=m_OpTreeList.GetSelCount();
 int* ixs;
 if (selCount>0) //get indexes
 {
  ixs=new int[selCount];
  m_OpTreeList.GetSelItems(selCount, ixs);
 }

 switch (selCount)
 {
 case 0:
  m_AddOp.EnableWindow(false);
  m_AddGroup.EnableWindow(true);
  m_DeleteOp.EnableWindow(false);
  m_DeleteGroup.EnableWindow(false);
  m_Up.EnableWindow(false);
  m_Down.EnableWindow(false);
  break;

 case 1:
  {
   OG* item=((OG*)(m_OpTreeList.GetItemData(ixs[0])));	
   m_AddOp.EnableWindow(item->isGroup());
   m_AddGroup.EnableWindow(item->isGroup());
   m_DeleteOp.EnableWindow(!item->isGroup());
   m_DeleteGroup.EnableWindow(item->isGroup());
   m_Up.EnableWindow(true);
   m_Down.EnableWindow(true);
  }
  break;

 default: //multiple items selected
  {
   bool haveItems=false;
   bool haveGroups=false;
   m_AddOp.EnableWindow(false);
   m_AddGroup.EnableWindow(false);
   m_Up.EnableWindow(false);
   m_Down.EnableWindow(false);

   //go thru all indexes and see if we have groups and items selected
   for (int i=0;i<selCount;i++)
   {
    OG* item=((OG*)(m_OpTreeList.GetItemData(ixs[i])));	
    if (item->isGroup()) 
     haveGroups=true;
    else
     haveItems=true;
   }

   m_DeleteOp.EnableWindow(haveItems);
   m_DeleteGroup.EnableWindow(haveGroups);
  }
  break;
 }

 //now update the stuff on the right
 switch (selCount)
 {
 case 1: //for now only allow editing of single selections
  {
   OG* item=((OG*)(m_OpTreeList.GetItemData(ixs[0])));	
   GetDlgItem(IDC_NAME)->EnableWindow(true);
   GetDlgItem(IDC_TYPE)->EnableWindow(!item->isGroup());
   GetDlgItem(IDC_BEHAVIOR)->EnableWindow(!item->isGroup());
   GetDlgItem(IDC_EDIT2)->EnableWindow(!item->isGroup());
   GetDlgItem(IDC_EDIT3)->EnableWindow(!item->isGroup());
   //remaining controls are set by the next call
   GetDataFromObject();
  }
  break;

 case 0:
 default:
  GetDlgItem(IDC_CHECK1)->EnableWindow(false);
  GetDlgItem(IDC_CHECK2)->EnableWindow(false);
  GetDlgItem(IDC_CHECK3)->EnableWindow(false);
  GetDlgItem(IDC_TYPE)->EnableWindow(false);
  GetDlgItem(IDC_BEHAVIOR)->EnableWindow(false);
  GetDlgItem(IDC_EDIT2)->EnableWindow(false);
  GetDlgItem(IDC_EDIT3)->EnableWindow(false);
  SetOperand1State(false);
  SetOperand2State(false);
  SetOperand3State(false);
  break;
 }

 GetDlgItem(IDC_APPLY)->EnableWindow(false);

 if (selCount>0)
 {
  delete ixs;
 }
}



void OpGroupsDialog::SetOperand1State(bool state)
{
 GetDlgItem(IDC_OPERAND_TYPE_11)->EnableWindow(state);
 GetDlgItem(IDC_OPERAND_11)->EnableWindow(state);
 GetDlgItem(IDC_OPERAND_TYPE_12)->EnableWindow(state);
 GetDlgItem(IDC_OPERAND_12)->EnableWindow(state);
 GetDlgItem(IDC_OPERAND_TYPE_13)->EnableWindow(state);
 GetDlgItem(IDC_OPERAND_13)->EnableWindow(state);
 GetDlgItem(IDC_OPERAND_TYPE_14)->EnableWindow(state);
 GetDlgItem(IDC_OPERAND_14)->EnableWindow(state);
}

void OpGroupsDialog::SetOperand2State(bool state)
{
 GetDlgItem(IDC_OPERAND_TYPE_21)->EnableWindow(state);
 GetDlgItem(IDC_OPERAND_21)->EnableWindow(state);
 GetDlgItem(IDC_OPERAND_TYPE_22)->EnableWindow(state);
 GetDlgItem(IDC_OPERAND_22)->EnableWindow(state);
 GetDlgItem(IDC_OPERAND_TYPE_23)->EnableWindow(state);
 GetDlgItem(IDC_OPERAND_23)->EnableWindow(state);
 GetDlgItem(IDC_OPERAND_TYPE_24)->EnableWindow(state);
 GetDlgItem(IDC_OPERAND_24)->EnableWindow(state);
}

void OpGroupsDialog::SetOperand3State(bool state)
{
 GetDlgItem(IDC_OPERAND_TYPE_31)->EnableWindow(state);
 GetDlgItem(IDC_OPERAND_31)->EnableWindow(state);
 GetDlgItem(IDC_OPERAND_TYPE_32)->EnableWindow(state);
 GetDlgItem(IDC_OPERAND_32)->EnableWindow(state);
 GetDlgItem(IDC_OPERAND_TYPE_33)->EnableWindow(state);
 GetDlgItem(IDC_OPERAND_33)->EnableWindow(state);
 GetDlgItem(IDC_OPERAND_TYPE_34)->EnableWindow(state);
 GetDlgItem(IDC_OPERAND_34)->EnableWindow(state);
}



void OpGroupsDialog::OnCheck1() 
{
 UpdateData();
 SetOperand1State(m_Check1==0?false:true);
}

void OpGroupsDialog::OnCheck2() 
{
 UpdateData();
 SetOperand2State(m_Check2==0?false:true);
}

void OpGroupsDialog::OnCheck3() 
{
 UpdateData();
 SetOperand3State(m_Check3==0?false:true);
}

void OpGroupsDialog::GetDataFromObject()
{
 int selCount=m_OpTreeList.GetSelCount();
 int* ixs;
 if (selCount>0) //get indexes
 {
  ixs=new int[selCount];
  m_OpTreeList.GetSelItems(selCount, ixs);
 }

 ASSERT(selCount>0);

 OG* og=((OG*)(m_OpTreeList.GetItemData(ixs[0])));	

 m_Name=og->text;  

 GetDlgItem(IDC_CHECK1)->EnableWindow(!og->isGroup());
 GetDlgItem(IDC_CHECK2)->EnableWindow(!og->isGroup());
 GetDlgItem(IDC_CHECK3)->EnableWindow(!og->isGroup());

 if (og->isGroup())
 {
  m_Operand11 = -1;
  m_Operand12 = -1;
  m_Operand13 = -1;
  m_Operand14 = -1;
  m_Operand21 = -1;
  m_Operand22 = -1;
  m_Operand23 = -1;
  m_Operand24 = -1;
  m_Operand31 = -1;
  m_Operand32 = -1;
  m_Operand33 = -1;
  m_Operand34 = -1;

  m_OperandType11 = _T("");
  m_OperandType12 = _T("");
  m_OperandType13 = _T("");
  m_OperandType14 = _T("");
  m_OperandType21 = _T("");
  m_OperandType22 = _T("");
  m_OperandType23 = _T("");
  m_OperandType24 = _T("");
  m_OperandType31 = _T("");
  m_OperandType32 = _T("");
  m_OperandType33 = _T("");
  m_OperandType34 = _T("");

  m_Edit2 = _T("");
  m_Edit3 = _T("");

  m_Behavior = _T("");
  
  m_OpType = -1;

  m_Check1 = FALSE;
  m_Check2 = FALSE;
  m_Check3 = FALSE;

  SetOperand1State(false);
  SetOperand2State(false);
  SetOperand3State(false);
 }
 else
 {
  m_OpType=og->opType;
  m_Behavior=og->behavior;

  m_Edit2=og->edit2;
  m_Edit3=og->edit3;

  m_Check1=og->check1;
  m_Check2=og->check2;
  m_Check3=og->check3;
  SetOperand1State(m_Check1==0?false:true);
  SetOperand2State(m_Check2==0?false:true);
  SetOperand3State(m_Check3==0?false:true);
  
  m_Operand11=og->operand11;
  m_Operand12=og->operand12;
  m_Operand13=og->operand13;
  m_Operand14=og->operand14;
  m_Operand21=og->operand21;
  m_Operand22=og->operand22;
  m_Operand23=og->operand23;
  m_Operand24=og->operand24;
  m_Operand31=og->operand31;
  m_Operand32=og->operand32;
  m_Operand33=og->operand33;
  m_Operand34=og->operand34;
  m_OperandType11=og->operandType11;
  m_OperandType12=og->operandType12;
  m_OperandType13=og->operandType13;
  m_OperandType14=og->operandType14;
  m_OperandType21=og->operandType21;
  m_OperandType22=og->operandType22;
  m_OperandType23=og->operandType23;
  m_OperandType24=og->operandType24;
  m_OperandType31=og->operandType31;
  m_OperandType32=og->operandType32;
  m_OperandType33=og->operandType33;
  m_OperandType34=og->operandType34;
 }

 UpdateData(false);
 
 if (selCount>0)
 {
  delete ixs;
 }
}

void OpGroupsDialog::PutDataIntoObject()
{
 int selCount=m_OpTreeList.GetSelCount();
 int* ixs;
 if (selCount>0) //get indexes
 {
  ixs=new int[selCount];
  m_OpTreeList.GetSelItems(selCount, ixs);
 }

 //ASSERT(selCount>0);

 OG* og=((OG*)(m_OpTreeList.GetItemData(ixs[0])));	

 UpdateData();
 
 og->text=m_Name; 

 if (!og->isGroup())
 {
  og->opType=m_OpType;
  og->behavior=m_Behavior;

  og->edit2=m_Edit2;
  og->edit3=m_Edit3;

  og->check1=m_Check1;
  og->check2=m_Check2;
  og->check3=m_Check3;
  
  og->operand11=m_Operand11;
  og->operand12=m_Operand12;
  og->operand13=m_Operand13;
  og->operand14=m_Operand14;
  og->operand21=m_Operand21;
  og->operand22=m_Operand22;
  og->operand23=m_Operand23;
  og->operand24=m_Operand24;
  og->operand31=m_Operand31;
  og->operand32=m_Operand32;
  og->operand33=m_Operand33;
  og->operand34=m_Operand34;

  og->operandType11=m_OperandType11;
  og->operandType12=m_OperandType12;
  og->operandType13=m_OperandType13;
  og->operandType14=m_OperandType14;
  og->operandType21=m_OperandType21;
  og->operandType22=m_OperandType22;
  og->operandType23=m_OperandType23;
  og->operandType24=m_OperandType24;
  og->operandType31=m_OperandType31;
  og->operandType32=m_OperandType32;
  og->operandType33=m_OperandType33;
  og->operandType34=m_OperandType34;

 }

 if (selCount>0)
 {
  delete ixs;
 }
}


////////////////////////////////////////////////////////////////////////

void OpGroupsDialog::OnChangeBehavior() 
{
 GetDlgItem(IDC_APPLY)->EnableWindow(true);
}

void OpGroupsDialog::OnChangeName() 
{
 GetDlgItem(IDC_APPLY)->EnableWindow(true);
}

void OpGroupsDialog::OnChangeEdit2() 
{
 GetDlgItem(IDC_APPLY)->EnableWindow(true);
}

void OpGroupsDialog::OnChangeEdit3() 
{
 GetDlgItem(IDC_APPLY)->EnableWindow(true);
}

void OpGroupsDialog::OnSelchangeOperand11() 
{
 GetDlgItem(IDC_APPLY)->EnableWindow(true);
}

void OpGroupsDialog::OnSelchangeOperand12() 
{
 GetDlgItem(IDC_APPLY)->EnableWindow(true);
}

void OpGroupsDialog::OnSelchangeOperand13() 
{
 GetDlgItem(IDC_APPLY)->EnableWindow(true);
}

void OpGroupsDialog::OnSelchangeOperand14() 
{
 GetDlgItem(IDC_APPLY)->EnableWindow(true);
}

void OpGroupsDialog::OnSelchangeOperand21() 
{
 GetDlgItem(IDC_APPLY)->EnableWindow(true);
}

void OpGroupsDialog::OnSelchangeOperand22() 
{
 GetDlgItem(IDC_APPLY)->EnableWindow(true);
}

void OpGroupsDialog::OnSelchangeOperand23() 
{
 GetDlgItem(IDC_APPLY)->EnableWindow(true);
}

void OpGroupsDialog::OnSelchangeOperand24() 
{
 GetDlgItem(IDC_APPLY)->EnableWindow(true);
}

void OpGroupsDialog::OnSelchangeOperand31() 
{
 GetDlgItem(IDC_APPLY)->EnableWindow(true);
}

void OpGroupsDialog::OnSelchangeOperand32() 
{
 GetDlgItem(IDC_APPLY)->EnableWindow(true);
}

void OpGroupsDialog::OnSelchangeOperand33() 
{
 GetDlgItem(IDC_APPLY)->EnableWindow(true);
}

void OpGroupsDialog::OnSelchangeOperand34() 
{
 GetDlgItem(IDC_APPLY)->EnableWindow(true);
}

void OpGroupsDialog::OnEditchangeOperandType11() 
{
 GetDlgItem(IDC_APPLY)->EnableWindow(true);
}

void OpGroupsDialog::OnEditchangeOperandType12() 
{
 GetDlgItem(IDC_APPLY)->EnableWindow(true);
}

void OpGroupsDialog::OnEditchangeOperandType13() 
{
 GetDlgItem(IDC_APPLY)->EnableWindow(true);
}

void OpGroupsDialog::OnEditchangeOperandType14() 
{
 GetDlgItem(IDC_APPLY)->EnableWindow(true);
}

void OpGroupsDialog::OnEditchangeOperandType21() 
{
 GetDlgItem(IDC_APPLY)->EnableWindow(true);
}

void OpGroupsDialog::OnEditchangeOperandType22() 
{
 GetDlgItem(IDC_APPLY)->EnableWindow(true);
}

void OpGroupsDialog::OnEditchangeOperandType23() 
{
 GetDlgItem(IDC_APPLY)->EnableWindow(true);
}

void OpGroupsDialog::OnEditchangeOperandType24() 
{
 GetDlgItem(IDC_APPLY)->EnableWindow(true);
}

void OpGroupsDialog::OnEditchangeOperandType31() 
{
 GetDlgItem(IDC_APPLY)->EnableWindow(true);
}

void OpGroupsDialog::OnEditchangeOperandType32() 
{
 GetDlgItem(IDC_APPLY)->EnableWindow(true);
}

void OpGroupsDialog::OnEditchangeOperandType33() 
{
 GetDlgItem(IDC_APPLY)->EnableWindow(true);
}

void OpGroupsDialog::OnEditchangeOperandType34() 
{
 GetDlgItem(IDC_APPLY)->EnableWindow(true);
}

void OpGroupsDialog::OnSelchangeType() 
{
 GetDlgItem(IDC_APPLY)->EnableWindow(true);
}

void OpGroupsDialog::OnApply() 
{
 PutDataIntoObject();	
 GetDlgItem(IDC_APPLY)->EnableWindow(false);
 m_OpTreeList.RedrawWindow();
}

int OpGroupsDialog::GetSingleSelection()
{
 int selCount=m_OpTreeList.GetSelCount();
 ASSERT(selCount<=1);
 int ix=-1;
 if (selCount>0)
 {
  m_OpTreeList.GetSelItems(selCount, &ix);
 }
 return ix;
}

void OpGroupsDialog::OnAddGroup() 
{
 OG* og;
 int sel=GetSingleSelection();
 OGList& ogl=isInfo->opGroups.opGroupItemList;
 if (sel==-1) //adding at level 0
 {
	 og=new OG(OG_GROUP,0,&CString("NewGroup"));
  ogl.AddTail(*og);
 }
 else //adding to a group
 {
  OG* sog=(OG*)(m_OpTreeList.GetItemData(sel));
  og=new OG(OG_GROUP,sog->level+1,&CString("NewGroup"));
  //add it as part of the currently selected group
  POSITION pos=isInfo->opGroups.FindItem(sog->id);
  ogl.InsertAfter(pos, *og);
  sog->isExpanded=true;
  sog->isSelected=true;
 }	
 delete og;
 TreeThruList::FillGroupsList(isInfo, m_OpTreeList);
 //m_OpTreeList.RedrawWindow();
}

void OpGroupsDialog::OnAddOp() 
{
 OG* og;
 int sel=GetSingleSelection();
 OGList& ogl=isInfo->opGroups.opGroupItemList;
 if (sel==-1) //adding at level 0
 {
  //ASSERT(0);
	 //og=new OG(OG_GROUP,0,&CString("NewGroup"));
  //ogl.AddTail(*og);
 }
 else //adding to a group
 {
  OG* sog=(OG*)(m_OpTreeList.GetItemData(sel));
  og=new OG(OG_OP,sog->level+1,&CString("NewOp"));
  //add it as part of the currently selected group
  POSITION pos=isInfo->opGroups.FindItem(sog->id);
  ogl.InsertAfter(pos, *og);
  sog->isExpanded=true;
  sog->isSelected=true;
  delete og;
 }	

 TreeThruList::FillGroupsList(isInfo, m_OpTreeList);
 //m_OpTreeList.RedrawWindow();
}

void OpGroupsDialog::OnDeleteGroup() 
{
 int selCount=m_OpTreeList.GetSelCount();
 int* ixs;
 if (selCount>0) //get indexes
 {
  ixs=new int[selCount];
  m_OpTreeList.GetSelItems(selCount, ixs);
 }

// ASSERT(selCount>0);

 for (int i=0;i<selCount;i++)
 {
  OG* item=((OG*)(m_OpTreeList.GetItemData(ixs[i])));	
  item->isSelected=true;
 }

 if (selCount>0)
 {
  delete ixs;
 }	

 isInfo->opGroups.DeleteGroups();
 TreeThruList::FillGroupsList(isInfo, m_OpTreeList);
}

void OpGroupsDialog::OnDeleteOp() 
{
 int selCount=m_OpTreeList.GetSelCount();
 int* ixs;
 if (selCount>0) //get indexes
 {
  ixs=new int[selCount];
  m_OpTreeList.GetSelItems(selCount, ixs);
 }

 //ASSERT(selCount>0);

 for (int i=0;i<selCount;i++)
 {
  OG* item=((OG*)(m_OpTreeList.GetItemData(ixs[i])));	
  item->isSelected=true;
 }

 if (selCount>0)
 {
  delete ixs;
 }	

 isInfo->opGroups.DeleteOps();
 TreeThruList::FillGroupsList(isInfo, m_OpTreeList);
}

void OpGroupsDialog::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}



