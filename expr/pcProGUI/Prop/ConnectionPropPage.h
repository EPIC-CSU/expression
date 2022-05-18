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
#if !defined(AFX_CONNECTIONPROPPAGE_H__A3DC9D2F_B500_11D2_BBB8_00C04FB17504__INCLUDED_)
#define AFX_CONNECTIONPROPPAGE_H__A3DC9D2F_B500_11D2_BBB8_00C04FB17504__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ConnectionPropPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// ConnectionPropPage dialog

#include "BasePropPage.h"

class ConnectionPropPage : public BasePropPage
{
// Construction
public:
	ConnectionPropPage(CWnd* pParent = NULL);   // standard constructor
 virtual void GetDataFromObject();
 virtual void PutDataIntoObject();

// Dialog Data
	//{{AFX_DATA(ConnectionPropPage)
	enum { IDD = IDD_PROP_CONN };
	CButton	m_Undo;
	CButton	m_Apply;
	CString	m_CustomProp;
	CString	m_Name;
	CString	m_ClassName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ConnectionPropPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(ConnectionPropPage)
	afx_msg void OnMove(int x, int y);
	afx_msg void OnApply();
	afx_msg void OnUndo();
	afx_msg void OnChangeClassName2();
	afx_msg void OnChangeCustomProp();
	afx_msg void OnChangeName2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONNECTIONPROPPAGE_H__A3DC9D2F_B500_11D2_BBB8_00C04FB17504__INCLUDED_)
