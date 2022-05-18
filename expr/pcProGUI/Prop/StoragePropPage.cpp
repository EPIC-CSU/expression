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
// StoragePropPage.cpp : implementation file
//

#include "stdafx.h"
#include "..\pcprogui.h"
#include "StoragePropPage.h"

#include "../SIM/SIMGraphStorage.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// StoragePropPage dialog


StoragePropPage::StoragePropPage(CWnd* pParent /*=NULL*/)
	: BasePropPage(StoragePropPage::IDD, pParent)
{
	//{{AFX_DATA_INIT(StoragePropPage)
	m_AddrRangeFrom = 0;
	m_AddrRangeTo = 0;
	m_CustomProp = _T("");
	m_Line = 0;
	m_Associativity = 0;
	m_Width = 0;
	m_MemSize = 0;
	m_AccessTime = 0;
	m_Name = _T("");
	m_ClassName = _T("");
	m_mnemonic = _T("");
	//}}AFX_DATA_INIT
}


void StoragePropPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(StoragePropPage)
	DDX_Control(pDX, IDC_UNDO, m_Undo);
	DDX_Control(pDX, IDC_APPLY, m_Apply);
	DDX_Text(pDX, IDC_ADDR_RANGE_FROM, m_AddrRangeFrom);
	DDX_Text(pDX, IDC_ADDR_RANGE_TO, m_AddrRangeTo);
	DDX_Text(pDX, IDC_CUSTOM_PROP, m_CustomProp);
	DDX_Text(pDX, IDC_LINE, m_Line);
	DDX_Text(pDX, IDC_ASSOCIATIVITY, m_Associativity);
	DDX_Text(pDX, IDC_WIDTH, m_Width);
	DDX_Text(pDX, IDC_MEM_SIZE, m_MemSize);
	DDX_Text(pDX, IDC_ACCESS_TIME, m_AccessTime);
	DDX_Text(pDX, IDC_NAME2, m_Name);
	DDX_Text(pDX, IDC_CLASS_NAME2, m_ClassName);
	DDX_Text(pDX, IDC_MNEMONIC, m_mnemonic);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(StoragePropPage, CDialog)
	//{{AFX_MSG_MAP(StoragePropPage)
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_ICACHE, OnICache)
	ON_BN_CLICKED(IDC_DCACHE, OnDCache)
	ON_BN_CLICKED(IDC_SRAM, OnSRam)
	ON_BN_CLICKED(IDC_DRAM, OnDRam)
	ON_BN_CLICKED(IDC_REGFILE, OnRegfile)
	ON_BN_CLICKED(IDC_APPLY, OnApply)
	ON_BN_CLICKED(IDC_UNDO, OnUndo)
	ON_EN_CHANGE(IDC_ACCESS_TIME, OnChangeAccessTime)
	ON_EN_CHANGE(IDC_ADDR_RANGE_FROM, OnChangeAddrRangeFrom)
	ON_EN_CHANGE(IDC_ADDR_RANGE_TO, OnChangeAddrRangeTo)
	ON_EN_CHANGE(IDC_ASSOCIATIVITY, OnChangeAssociativity)
	ON_EN_CHANGE(IDC_CLASS_NAME2, OnChangeClassName2)
	ON_EN_CHANGE(IDC_CUSTOM_PROP, OnChangeCustomProp)
	ON_EN_CHANGE(IDC_LINE, OnChangeLine)
	ON_EN_CHANGE(IDC_MEM_SIZE, OnChangeMemSize)
	ON_EN_CHANGE(IDC_NAME2, OnChangeName2)
	ON_EN_CHANGE(IDC_WIDTH, OnChangeWidth)
	ON_EN_CHANGE(IDC_MNEMONIC, OnChangeMnemonic)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// StoragePropPage message handlers

void StoragePropPage::OnMove(int x, int y) 
{
	CDialog::OnMove(x, y);
	MovePage(this);
}

void StoragePropPage::OnICache() 
{

 if (!bSetting) USER_CHANGED_DATA; 

   CheckDlgButton(IDC_REGFILE,0);
   CheckDlgButton(IDC_ICACHE,1);
   CheckDlgButton(IDC_DCACHE,0);
   CheckDlgButton(IDC_SRAM,0);
   CheckDlgButton(IDC_DRAM,0);

	GetDlgItem(IDC_MNEMONIC)->EnableWindow(false);
	GetDlgItem(IDC_MNEMONIC_L)->EnableWindow(false);

	GetDlgItem(IDC_WIDTH)->EnableWindow(true);
	GetDlgItem(IDC_WIDTH_L)->EnableWindow(true);
	GetDlgItem(IDC_WIDTH_L)->SetWindowText("Word Size");

	GetDlgItem(IDC_LINE)->EnableWindow(true);
	GetDlgItem(IDC_LINE_L)->EnableWindow(true);

	GetDlgItem(IDC_MEM_SIZE)->EnableWindow(true);
	GetDlgItem(IDC_MEM_SIZE_L)->EnableWindow(true);
	GetDlgItem(IDC_MEM_SIZE_L)->SetWindowText("Line Size");

	GetDlgItem(IDC_ASSOCIATIVITY)->EnableWindow(true);
	GetDlgItem(IDC_ASSOCIATIVITY_L)->EnableWindow(true);

	//GetDlgItem(IDC_ASSOCIATIVITY_L)->SetWindowText("Associativity");

	GetDlgItem(IDC_ADDR_RANGE_L)->EnableWindow(true);
	GetDlgItem(IDC_ADDR_RANGE_FROM)->EnableWindow(true);
	GetDlgItem(IDC_ADDR_RANGE_FROM_L)->EnableWindow(true);
	GetDlgItem(IDC_ADDR_RANGE_TO)->EnableWindow(true);
	GetDlgItem(IDC_ADDR_RANGE_TO_L)->EnableWindow(true);

	GetDlgItem(IDC_ACCESS_TIME)->EnableWindow(true);
	GetDlgItem(IDC_ACCESS_TIME_L)->EnableWindow(true);
}

void StoragePropPage::OnDCache() 
{

 if (!bSetting) USER_CHANGED_DATA; 

   CheckDlgButton(IDC_REGFILE,0);
   CheckDlgButton(IDC_ICACHE,0);
   CheckDlgButton(IDC_DCACHE,1);
   CheckDlgButton(IDC_SRAM,0);
   CheckDlgButton(IDC_DRAM,0);

	GetDlgItem(IDC_MNEMONIC)->EnableWindow(false);
	GetDlgItem(IDC_MNEMONIC_L)->EnableWindow(false);

	GetDlgItem(IDC_WIDTH)->EnableWindow(true);
	GetDlgItem(IDC_WIDTH_L)->EnableWindow(true);
	GetDlgItem(IDC_WIDTH_L)->SetWindowText("Word Size");

	GetDlgItem(IDC_LINE)->EnableWindow(true);
	GetDlgItem(IDC_LINE_L)->EnableWindow(true);

	GetDlgItem(IDC_MEM_SIZE)->EnableWindow(true);
	GetDlgItem(IDC_MEM_SIZE_L)->EnableWindow(true);
	GetDlgItem(IDC_MEM_SIZE_L)->SetWindowText("Line Size");

	GetDlgItem(IDC_ASSOCIATIVITY)->EnableWindow(true);
	GetDlgItem(IDC_ASSOCIATIVITY_L)->EnableWindow(true);

	//GetDlgItem(IDC_ASSOCIATIVITY_L)->SetWindowText("Associativity");

	GetDlgItem(IDC_ADDR_RANGE_L)->EnableWindow(true);
	GetDlgItem(IDC_ADDR_RANGE_FROM)->EnableWindow(true);
	GetDlgItem(IDC_ADDR_RANGE_FROM_L)->EnableWindow(true);
	GetDlgItem(IDC_ADDR_RANGE_TO)->EnableWindow(true);
	GetDlgItem(IDC_ADDR_RANGE_TO_L)->EnableWindow(true);

	GetDlgItem(IDC_ACCESS_TIME)->EnableWindow(true);
	GetDlgItem(IDC_ACCESS_TIME_L)->EnableWindow(true);
}

void StoragePropPage::OnSRam() 
{

 if (!bSetting) USER_CHANGED_DATA; 

   CheckDlgButton(IDC_REGFILE,0);
   CheckDlgButton(IDC_ICACHE,0);
   CheckDlgButton(IDC_DCACHE,0);
   CheckDlgButton(IDC_SRAM,1);
   CheckDlgButton(IDC_DRAM,0);

 	GetDlgItem(IDC_MNEMONIC)->EnableWindow(false);
	GetDlgItem(IDC_MNEMONIC_L)->EnableWindow(false);

	GetDlgItem(IDC_WIDTH)->EnableWindow(false);
	GetDlgItem(IDC_WIDTH_L)->EnableWindow(false);
	GetDlgItem(IDC_WIDTH_L)->SetWindowText("Width");

	GetDlgItem(IDC_MEM_SIZE)->EnableWindow(false);
	GetDlgItem(IDC_MEM_SIZE_L)->EnableWindow(false);

	GetDlgItem(IDC_MEM_SIZE_L)->SetWindowText("Size");

	GetDlgItem(IDC_LINE)->EnableWindow(false);
	GetDlgItem(IDC_LINE_L)->EnableWindow(false);

	GetDlgItem(IDC_ASSOCIATIVITY)->EnableWindow(false);
	GetDlgItem(IDC_ASSOCIATIVITY_L)->EnableWindow(false);

	//GetDlgItem(IDC_ASSOCIATIVITY_L)->SetWindowText("Associativity");

	GetDlgItem(IDC_ADDR_RANGE_L)->EnableWindow(true);
	GetDlgItem(IDC_ADDR_RANGE_FROM)->EnableWindow(true);
	GetDlgItem(IDC_ADDR_RANGE_FROM_L)->EnableWindow(true);
	GetDlgItem(IDC_ADDR_RANGE_TO)->EnableWindow(true);
	GetDlgItem(IDC_ADDR_RANGE_TO_L)->EnableWindow(true);

	GetDlgItem(IDC_ACCESS_TIME)->EnableWindow(true);
	GetDlgItem(IDC_ACCESS_TIME_L)->EnableWindow(true);
}

void StoragePropPage::OnDRam() 
{

 if (!bSetting) USER_CHANGED_DATA; 

   CheckDlgButton(IDC_REGFILE,0);
   CheckDlgButton(IDC_ICACHE,0);
   CheckDlgButton(IDC_DCACHE,0);
   CheckDlgButton(IDC_SRAM,0);
   CheckDlgButton(IDC_DRAM,1);

	GetDlgItem(IDC_MNEMONIC)->EnableWindow(false);
	GetDlgItem(IDC_MNEMONIC_L)->EnableWindow(false);

	GetDlgItem(IDC_WIDTH)->EnableWindow(false);
	GetDlgItem(IDC_WIDTH_L)->EnableWindow(false);
	GetDlgItem(IDC_WIDTH_L)->SetWindowText("Width");

	GetDlgItem(IDC_MEM_SIZE)->EnableWindow(false);
	GetDlgItem(IDC_MEM_SIZE_L)->EnableWindow(false);

	GetDlgItem(IDC_MEM_SIZE_L)->SetWindowText("Size");

	GetDlgItem(IDC_LINE)->EnableWindow(false);
	GetDlgItem(IDC_LINE_L)->EnableWindow(false);

	GetDlgItem(IDC_ASSOCIATIVITY)->EnableWindow(false);
	GetDlgItem(IDC_ASSOCIATIVITY_L)->EnableWindow(false);

	//GetDlgItem(IDC_ASSOCIATIVITY_L)->SetWindowText("Associativity");

	GetDlgItem(IDC_ADDR_RANGE_L)->EnableWindow(true);
	GetDlgItem(IDC_ADDR_RANGE_FROM)->EnableWindow(true);
	GetDlgItem(IDC_ADDR_RANGE_FROM_L)->EnableWindow(true);
	GetDlgItem(IDC_ADDR_RANGE_TO)->EnableWindow(true);
	GetDlgItem(IDC_ADDR_RANGE_TO_L)->EnableWindow(true);

	GetDlgItem(IDC_ACCESS_TIME)->EnableWindow(true);
	GetDlgItem(IDC_ACCESS_TIME_L)->EnableWindow(true);
}

void StoragePropPage::OnRegfile() 
{

 if (!bSetting) USER_CHANGED_DATA; 


   CheckDlgButton(IDC_REGFILE,1);
   CheckDlgButton(IDC_ICACHE,0);
   CheckDlgButton(IDC_DCACHE,0);
   CheckDlgButton(IDC_SRAM,0);
   CheckDlgButton(IDC_DRAM,0);

	GetDlgItem(IDC_MNEMONIC)->EnableWindow(true);
	GetDlgItem(IDC_MNEMONIC_L)->EnableWindow(true);

	GetDlgItem(IDC_WIDTH)->EnableWindow(true);
	GetDlgItem(IDC_WIDTH_L)->EnableWindow(true);
	GetDlgItem(IDC_WIDTH_L)->SetWindowText("Width");


	GetDlgItem(IDC_MEM_SIZE_L)->SetWindowText("Size");
	GetDlgItem(IDC_MEM_SIZE)->EnableWindow(true);
	GetDlgItem(IDC_MEM_SIZE_L)->EnableWindow(true);

	GetDlgItem(IDC_LINE)->EnableWindow(false);
	GetDlgItem(IDC_LINE_L)->EnableWindow(false);

	GetDlgItem(IDC_ASSOCIATIVITY)->EnableWindow(false);
	GetDlgItem(IDC_ASSOCIATIVITY_L)->EnableWindow(false);

	//GetDlgItem(IDC_ASSOCIATIVITY_L)->SetWindowText("Mnemonic");

	GetDlgItem(IDC_ADDR_RANGE_L)->EnableWindow(false);
	GetDlgItem(IDC_ADDR_RANGE_FROM)->EnableWindow(false);
	GetDlgItem(IDC_ADDR_RANGE_FROM_L)->EnableWindow(false);
	GetDlgItem(IDC_ADDR_RANGE_TO)->EnableWindow(false);
	GetDlgItem(IDC_ADDR_RANGE_TO_L)->EnableWindow(false);

	GetDlgItem(IDC_ACCESS_TIME)->EnableWindow(false);
	GetDlgItem(IDC_ACCESS_TIME_L)->EnableWindow(false);
}


void StoragePropPage::GetDataFromObject()
{
 SIMGraphStorage* obj=(SIMGraphStorage*)(attachedObj);
  UpdateData(true);
	m_CustomProp = obj->customProperties;
	m_Name = obj->name;
	m_ClassName = obj->classname;
	m_AddrRangeFrom=obj->addrRangeFrom;
	m_AddrRangeTo=obj->addrRangeTo;
	m_Line=obj->line;
	m_Associativity=obj->associativity;
	m_Width=obj->width;
	m_MemSize=obj->memSize;
	m_AccessTime=obj->accessTime;
	m_mnemonic=obj->mnemonic;
 bSetting=true; 
 switch (obj->memType)
 {
  case 1: 
   OnRegfile();
   break;
  
  case 2: 
   OnICache();
   break;

  case 3: 
   OnDCache();
   break;

  case 4: 
   OnSRam();
   break;

  case 5: 
   OnDRam();
   break;
 }

 bSetting=false; 
 UpdateData(false);
 m_Undo.EnableWindow(false); 
 m_Apply.EnableWindow(false); 
}


void StoragePropPage::PutDataIntoObject()
{
 SIMGraphStorage* obj=(SIMGraphStorage*)(attachedObj);
 UpdateData(true);
	obj->customProperties = m_CustomProp;
	obj->name = m_Name;
	obj->classname = m_ClassName;
	obj->addrRangeFrom=m_AddrRangeFrom;
	obj->addrRangeTo=m_AddrRangeTo;
	obj->line=m_Line;
	obj->associativity=m_Associativity;
	obj->width=m_Width;
	obj->memSize=m_MemSize;
	obj->accessTime=m_AccessTime;
	obj->mnemonic=m_mnemonic;
 if (IsDlgButtonChecked(IDC_REGFILE)) obj->memType=1;
 if (IsDlgButtonChecked(IDC_ICACHE)) obj->memType=2;
 if (IsDlgButtonChecked(IDC_DCACHE)) obj->memType=3;
 if (IsDlgButtonChecked(IDC_SRAM)) obj->memType=4;
 if (IsDlgButtonChecked(IDC_DRAM)) obj->memType=5;
}



void StoragePropPage::OnApply() 
{
 ONAPPLY
}

void StoragePropPage::OnUndo() 
{
 ONUNDO
}

void StoragePropPage::OnChangeAccessTime() 
{
 USER_CHANGED_DATA; 
}

void StoragePropPage::OnChangeAddrRangeFrom() 
{
 USER_CHANGED_DATA; 
}

void StoragePropPage::OnChangeAddrRangeTo() 
{
 USER_CHANGED_DATA; 
}

void StoragePropPage::OnChangeMnemonic() 
{
 USER_CHANGED_DATA; 
}

void StoragePropPage::OnChangeAssociativity() 
{
 USER_CHANGED_DATA; 
}

void StoragePropPage::OnChangeClassName2() 
{
 USER_CHANGED_DATA; 
}

void StoragePropPage::OnChangeCustomProp() 
{
 USER_CHANGED_DATA; 
}

void StoragePropPage::OnChangeLine() 
{
 USER_CHANGED_DATA; 
}

void StoragePropPage::OnChangeMemSize() 
{
 USER_CHANGED_DATA; 
}

void StoragePropPage::OnChangeName2() 
{
 USER_CHANGED_DATA; 
}

void StoragePropPage::OnChangeWidth() 
{
 USER_CHANGED_DATA; 
}
