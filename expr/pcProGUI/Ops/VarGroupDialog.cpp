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
// VarGroupDialog.cpp : implementation file
//

#include "stdafx.h"
#include "..\pcprogui.h"
#include "VarGroupDialog.h"

#include "ISInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VarGroupDialog dialog


VarGroupDialog::VarGroupDialog(ISInfo* ISInfo, CWnd* pParent /*=NULL*/)
	: CDialog(VarGroupDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(VarGroupDialog)
	m_Name = _T("");
	m_Datatype = _T("");
	m_Components = _T("");	
	//}}AFX_DATA_INIT
 isInfo=ISInfo;
}


void VarGroupDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VarGroupDialog)
	DDX_Control(pDX, IDC_VAR_GROUPS, m_GroupList);
	DDX_Control(pDX, IDC_DELETE_VAR_GROUP, m_DeleteVarGroup);
	DDX_Control(pDX, IDC_ADD_VAR_GROUP, m_AddVarGroup);
	DDX_Text(pDX, IDC_NAME, m_Name);
	DDX_Text(pDX, IDC_DATATYPE, m_Datatype);
	DDX_Text(pDX, IDC_COMPONENTS, m_Components);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(VarGroupDialog, CDialog)
	//{{AFX_MSG_MAP(VarGroupDialog)
	ON_EN_CHANGE(IDC_COMPONENTS, OnChangeComponents)
	ON_EN_CHANGE(IDC_NAME, OnChangeName)
	ON_EN_CHANGE(IDC_DATATYPE, OnChangeName)
	ON_LBN_SELCHANGE(IDC_VAR_GROUPS, OnSelChangeVarGroups)
	ON_BN_CLICKED(IDC_ADD_VAR_GROUP, OnAddVarGroup)
	ON_BN_CLICKED(IDC_DELETE_VAR_GROUP, OnDeleteVarGroup)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VarGroupDialog message handlers

void VarGroupDialog::OnChangeComponents() 
{
 UpdateData();
 m_AddVarGroup.EnableWindow((m_Name!="") && (m_Components!="") && (m_Datatype!=""));
}

void VarGroupDialog::OnChangeName() 
{
 UpdateData();
 m_AddVarGroup.EnableWindow((m_Name!="") && (m_Components!="") && (m_Datatype!=""));
}

void VarGroupDialog::OnChangeDatatype() 
{
 UpdateData();
 m_AddVarGroup.EnableWindow((m_Name!="") && (m_Components!="") && (m_Datatype!=""));
}

void VarGroupDialog::OnAddVarGroup() 
{
 UpdateData();
 //add it
 m_GroupList.AddString(m_Name+"\t"+m_Datatype+"\t"+m_Components);

 m_Name="";
 m_Datatype="";
 m_Components="";
 UpdateData(false);
	m_AddVarGroup.EnableWindow(false);
 GetDlgItem(IDC_NAME)->SetFocus();
}

void VarGroupDialog::OnSelChangeVarGroups() 
{
 m_DeleteVarGroup.EnableWindow(m_GroupList.GetCurSel()!=LB_ERR);
}

void VarGroupDialog::OnDeleteVarGroup() 
{
	m_GroupList.DeleteString(m_GroupList.GetCurSel());
 OnSelChangeVarGroups();
}

BOOL VarGroupDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
 int stops[1];
 stops[0]=70;
 //stops[2]=200;
 VERIFY(m_GroupList.SetTabStops(1, (LPINT)&stops));

 m_AddVarGroup.SetIcon(theApp.LoadIcon(IDI_ADD));
 m_DeleteVarGroup.SetIcon(theApp.LoadIcon(IDI_DELETE));
	
 //now load the existing instr desc info
 //fill in the groups
 CString key, key2;
 POSITION pos=isInfo->varGroups.groups.GetStartPosition();
 while (pos!=NULL)
 {
  VarGroup group;
  isInfo->varGroups.groups.GetNextAssoc(pos,key,group);  

  key=group.name+"\t";

//  for(POSITION i=group.types.GetHeadPosition();i!=NULL;group.types.GetNext(i))
//  {
//   if (i==group.types.GetHeadPosition()) //first value
//   {
    key+=group.types.GetAt(group.types.GetHeadPosition());
//   }
//   else
//   {
//    key+=" " + group.types.GetAt(i);
 //  }
//  }  
  
   key+="\t";

  for(POSITION i=group.values.GetHeadPosition();i!=NULL;group.values.GetNext(i))
  {
   if (i==group.values.GetHeadPosition()) //first value
   {
    key+=group.values.GetAt(i);
   }
   else
   {
    key+=" " + group.values.GetAt(i);
   }
  }  
  m_GroupList.AddString(key);
 }

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void VarGroupDialog::OnOK() 
{
 char* sep=" |\t";
 char* tok, *tok2;
 char* str;
 //char* tstr, *tstr2, *tstr3;
 int c=m_GroupList.GetCount();

 isInfo->varGroups.groups.RemoveAll();
 for (int i=0;i<c;i++)
 {
  CString s, k;
  VarGroup* group=new VarGroup;
 

  m_GroupList.GetText(i,s);
  str=new char[s.GetLength()+1];
  strcpy(str,s);
  //tokenize str
  tok=strtok(str,sep); //this is the name
  group->name=tok;

  tok2=strtok(NULL,sep); //this is the datatype
  CString temp2(tok2);

    group->types.AddTail(temp2);
//	group->values.AddTail(temp2);
  tok=strtok(NULL,sep); //now the values


 /* 
  m_GroupList.GetText(i,s);
  str=new char[s.GetLength()+1];
  strcpy(str,s);
  //tokenize str
  tok=strtok(str,sep); //this is the name
  group->name=tok;

  tok2=strtok(str,sep); //this is the datatype

  
  tok=strtok(NULL,sep); //now the values
*/
  while (tok!=NULL)
  {
   CString temp(tok);
   group->values.AddTail(temp);
   tok=strtok(NULL,sep);
  }

  isInfo->varGroups.groups.SetAt(group->name, *group);
  delete str;
  delete group;
  //delete tstr, tstr2, tstr3;
 }

	CDialog::OnOK();
}

