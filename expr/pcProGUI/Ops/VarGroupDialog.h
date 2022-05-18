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
#if !defined(AFX_VARGROUPDIALOG_H__F6318D0F_D68B_11D2_BBD1_00C04FB17504__INCLUDED_)
#define AFX_VARGROUPDIALOG_H__F6318D0F_D68B_11D2_BBD1_00C04FB17504__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// VarGroupDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// VarGroupDialog dialog

//#include "ISInfo.h"
class ISInfo;

class VarGroupDialog : public CDialog
{
// Construction
public:
	VarGroupDialog(ISInfo* ISInfo, CWnd* pParent = NULL);   // standard constructor
 ISInfo* isInfo;

// Dialog Data
	//{{AFX_DATA(VarGroupDialog)
	enum { IDD = IDD_IS_VAR_GROUP };
	CListBox	m_GroupList;
	CButton	m_DeleteVarGroup;
	CButton	m_AddVarGroup;
	CString	m_Name;
	CString	m_Datatype;
	CString	m_Components;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VarGroupDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VarGroupDialog)
	afx_msg void OnChangeComponents();
	afx_msg void OnChangeName();
	afx_msg void OnChangeDatatype();
	afx_msg void OnSelChangeVarGroups();
	virtual void OnOK();
	afx_msg void OnAddVarGroup();
	afx_msg void OnDeleteVarGroup();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VARGROUPDIALOG_H__F6318D0F_D68B_11D2_BBD1_00C04FB17504__INCLUDED_)
