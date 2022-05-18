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
#if !defined(AFX_INSTRUCTIONDESCRIPTION_H__F6318C6E_D68B_11D2_BBD1_00C04FB17504__INCLUDED_)
#define AFX_INSTRUCTIONDESCRIPTION_H__F6318C6E_D68B_11D2_BBD1_00C04FB17504__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// InstrDescrDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// InstrDescrDialog dialog

class ISInfo;

class InstrDescrDialog : public CDialog
{
// Construction
public:
	InstrDescrDialog(ISInfo* ISInfo, CWnd* pParent = NULL);   // standard constructor
 ISInfo* isInfo;

// Dialog Data
	//{{AFX_DATA(InstrDescrDialog)
	enum { IDD = IDD_IS_INSTR_DESCR };
	CComboBox	m_Wordlen;
	CButton	m_DeleteSlot;
	CComboBox	m_Type;
	CButton	m_AddSlot;
	CListBox	m_SlotList;
	CButton	m_Down;
	CButton	m_Up;
	CString	m_Bitwidth;
	CString	m_Unit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(InstrDescrDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(InstrDescrDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelChangeSlotList();
	afx_msg void OnSelChangeType();
	afx_msg void OnSelChangeUnit();
	afx_msg void OnAddSlot();
	afx_msg void OnDeleteSlot();
	afx_msg void OnDown();
	afx_msg void OnUp();
	afx_msg void OnEditChangeBitwidth();
	afx_msg void OnSelChangeBitwidth();
	virtual void OnOK();
	afx_msg void OnEditChangeUnit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INSTRUCTIONDESCRIPTION_H__F6318C6E_D68B_11D2_BBD1_00C04FB17504__INCLUDED_)
