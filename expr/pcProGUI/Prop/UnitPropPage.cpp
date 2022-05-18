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
// UnitPropPage.cpp : implementation file
//

#include "stdafx.h"
#include "..\pcprogui.h"
#include "UnitPropPage.h"

#include "../SIM/SIMGraphUnit.h"
#include "../Ops/TreeThruList.h"

#include "../Ops/ISInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// UnitPropPage dialog


UnitPropPage::UnitPropPage(ISInfo* ISInfo, CWnd* pParent /*=NULL*/)
	: BasePropPage(UnitPropPage::IDD, pParent)
{
	//{{AFX_DATA_INIT(UnitPropPage)
	m_Capacity = 0;
	m_CustomProp = _T("");
	m_Name = _T("");
	m_Timing = _T("");
	m_InstrIn = 0;
	m_InstrOut = 0;
	m_ClassName = _T("");
	//}}AFX_DATA_INIT
 isInfo=ISInfo;
}


void UnitPropPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(UnitPropPage)
	DDX_Control(pDX, IDC_OP_TREE, m_OpTreeList);
	DDX_Control(pDX, IDC_UNDO, m_Undo);
	DDX_Control(pDX, IDC_APPLY, m_Apply);
	DDX_Text(pDX, IDC_CAPACITY, m_Capacity);
	DDX_Text(pDX, IDC_CUSTOM_PROP, m_CustomProp);
	DDX_Text(pDX, IDC_NAME2, m_Name);
	DDX_Text(pDX, IDC_TIMING, m_Timing);
	DDX_Text(pDX, IDC_INSTRIN, m_InstrIn);
	DDX_Text(pDX, IDC_INSTROUT, m_InstrOut);
	DDX_Text(pDX, IDC_CLASS_NAME2, m_ClassName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(UnitPropPage, CDialog)
	//{{AFX_MSG_MAP(UnitPropPage)
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_APPLY, OnApply)
	ON_BN_CLICKED(IDC_OPCODES_ALL, OnOpcodesAll)
	ON_BN_CLICKED(IDC_OPCODES_SELECTED, OnOpcodesSelected)
	ON_BN_CLICKED(IDC_UNDO, OnUndo)
	ON_EN_CHANGE(IDC_CAPACITY, OnChangeCapacity)
	ON_EN_CHANGE(IDC_CLASS_NAME2, OnChangeClassName2)
	ON_EN_CHANGE(IDC_CUSTOM_PROP, OnChangeCustomProp)
	ON_EN_CHANGE(IDC_NAME2, OnChangeName2)
	ON_EN_CHANGE(IDC_TIMING, OnChangeTiming)
	ON_EN_CHANGE(IDC_INSTRIN, OnChangeInstrIn)
	ON_EN_CHANGE(IDC_INSTROUT, OnChangeInstrOut)
	ON_WM_DRAWITEM()
	ON_LBN_DBLCLK(IDC_OP_TREE, OnDblclkOpTree)
	ON_LBN_SELCHANGE(IDC_OP_TREE, OnSelChangeOpTree)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// UnitPropPage message handlers

void UnitPropPage::OnMove(int x, int y) 
{
	CDialog::OnMove(x, y);
	MovePage(this);
}

void UnitPropPage::OnApply() 
{
 ONAPPLY
}

void UnitPropPage::OnOpcodesAll() 
{
 USER_CHANGED_DATA; 
	GetDlgItem(IDC_OPCODES)->EnableWindow(false);
}

void UnitPropPage::OnOpcodesSelected() 
{
 USER_CHANGED_DATA; 
	GetDlgItem(IDC_OPCODES)->EnableWindow(true);
}

void UnitPropPage::OnUndo() 
{
	ONUNDO
}


void UnitPropPage::GetDataFromObject()
{
 SIMGraphUnit* obj=(SIMGraphUnit*)(attachedObj);
	m_Capacity = obj->capacity;
	m_CustomProp = obj->customProperties;
	m_Name = obj->name;
	m_Timing = obj->timing;
	m_InstrIn = obj->instrin;
	m_InstrOut = obj->instrout;
	m_ClassName = obj->classname;
 
 CheckDlgButton(IDC_OPCODES_ALL, (obj->opcodes=="")?1:0);
 CheckDlgButton(IDC_OPCODES_SELECTED, (obj->opcodes=="")?0:1);

 m_OpTreeList.EnableWindow(obj->opcodes!="");
 
 //fill in the group treelist
 TreeThruList::FillGroupsList(isInfo, m_OpTreeList);

 //select the proper opcodes

 if (obj->opcodes!="")
 {
  char* sep=" ";
  char* oc=new char[obj->opcodes.GetLength()+1];
  char* tok;

  strcpy(oc,obj->opcodes);

  //tokenize str
  tok=strtok(oc,sep); //this is the first item selected
  while (tok!=NULL)
  {
   //m_OpTreeList.SelectString(-1,tok);
   int sel=m_OpTreeList.FindString(-1,tok);
   if (sel!=LB_ERR) m_OpTreeList.SetSel(sel);
   tok=strtok(NULL,sep);
  }

  delete oc;
 }

 UpdateData(false);
 m_Undo.EnableWindow(false); 
 m_Apply.EnableWindow(false); 
}

void UnitPropPage::PutDataIntoObject()
{
 SIMGraphUnit* obj=(SIMGraphUnit*)(attachedObj);
 UpdateData(true);
	obj->capacity = m_Capacity;
	obj->customProperties = m_CustomProp;
	obj->name = m_Name;
	obj->timing = m_Timing;
	obj->instrin = m_InstrIn;
	obj->instrout = m_InstrOut;
	obj->classname = m_ClassName;
 if (IsDlgButtonChecked(IDC_OPCODES_ALL))
 {
  obj->opcodes="";
 }
 else //fill in the opcodes used
 {  
  int selCount=m_OpTreeList.GetSelCount();
  int* ixs=NULL;
  if (selCount>0) //get indexes
  {
   ixs=new int[selCount];
   m_OpTreeList.GetSelItems(selCount, ixs);
  }
    
  obj->opcodes="";
  for (int i=0;i<selCount;i++)
  {
   OG* og=(OG*)(m_OpTreeList.GetItemData(ixs[i]));
   if (i!=0)
   {
    obj->opcodes+=" "+og->text;
   }
   else
   {
    obj->opcodes=og->text;
   }
  }
    
  if (ixs!=NULL) delete ixs;
 }
}

void UnitPropPage::OnChangeCapacity() 
{
 USER_CHANGED_DATA; 
}

void UnitPropPage::OnChangeInstrIn() 
{
 USER_CHANGED_DATA; 
}

void UnitPropPage::OnChangeInstrOut() 
{
 USER_CHANGED_DATA; 
}

void UnitPropPage::OnChangeClassName2() 
{
 USER_CHANGED_DATA; 
}

void UnitPropPage::OnChangeCustomProp() 
{
 USER_CHANGED_DATA; 
}

void UnitPropPage::OnChangeName2() 
{
 USER_CHANGED_DATA; 
}

void UnitPropPage::OnChangeTiming() 
{
 USER_CHANGED_DATA; 
}

void UnitPropPage::OnSelChangeOpTree() 
{
 USER_CHANGED_DATA; 
}

void UnitPropPage::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
 if (lpDrawItemStruct->itemID!=-1)
 {
  OG* og=(OG*)m_OpTreeList.GetItemData(lpDrawItemStruct->itemID);
  ASSERT(og);

  TreeThruList::OnDrawItem(og, nIDCtl, lpDrawItemStruct);
 }
}

void UnitPropPage::OnDblclkOpTree() 
{
//no group expansion for now
// //toggle the expanded attribute
// int index=m_OpTreeList.GetCurSel();
//	OG* item=((OG*)(m_OpTreeList.GetItemData(index)));	
// item->isExpanded=!item->isExpanded;
// TreeThruList::FillGroupsList(isInfo, m_OpTreeList);
// m_OpTreeList.SetSel(index);
}

