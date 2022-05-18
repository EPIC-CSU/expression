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
#if !defined(AFX_OPSDIALOG_H__F6318C6D_D68B_11D2_BBD1_00C04FB17504__INCLUDED_)
#define AFX_OPSDIALOG_H__F6318C6D_D68B_11D2_BBD1_00C04FB17504__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// OpGroupsDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// OpGroupsDialog dialog

class ISInfo;

class OpGroupsDialog : public CDialog
{
// Construction
public:
	int GetSingleSelection(void);
	void PutDataIntoObject();
	void GetDataFromObject();
	void SetOperand3State(bool state);
	void SetOperand2State(bool state);
	void SetOperand1State(bool state);
	OpGroupsDialog(ISInfo* ISInfo, CWnd* pParent = NULL);   // standard constructor
 ISInfo* isInfo;

// Dialog Data
	//{{AFX_DATA(OpGroupsDialog)
	enum { IDD = IDD_IS_OP_GROUPS };
	CButton	m_Down;
	CButton	m_Up;
	CButton	m_DeleteOp;
	CButton	m_DeleteGroup;
	CButton	m_AddOp;
	CButton	m_AddGroup;
	CListBox	m_OpTreeList;
	int		m_Operand11;
	int		m_Operand12;
	int		m_Operand13;
	int		m_Operand14;
	int		m_Operand22;
	int		m_Operand23;
	int		m_Operand24;
	int		m_Operand31;
	int		m_Operand32;
	int		m_Operand33;
	int		m_Operand34;
	CString	m_OperandType11;
	CString	m_OperandType12;
	CString	m_OperandType13;
	CString	m_OperandType14;
	CString	m_OperandType21;
	CString	m_OperandType22;
	CString	m_OperandType23;
	CString	m_OperandType24;
	CString	m_OperandType31;
	CString	m_OperandType32;
	CString	m_OperandType33;
	CString	m_OperandType34;
	CString	m_Behavior;
	CString	m_Edit2;
	CString	m_Edit3;
	int		m_OpType;
	int		m_Operand21;
	CString	m_Name;
	BOOL	m_Check1;
	BOOL	m_Check2;
	BOOL	m_Check3;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(OpGroupsDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(OpGroupsDialog)
	afx_msg void OnAddGroup();
	afx_msg void OnAddOp();
	afx_msg void OnDeleteGroup();
	afx_msg void OnDeleteOp();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnDblClkOpTree();
	afx_msg void OnSelChangeOpTree();
	afx_msg void OnCheck1();
	afx_msg void OnCheck2();
	afx_msg void OnCheck3();
	afx_msg void OnChangeBehavior();
	afx_msg void OnChangeName();
	afx_msg void OnChangeEdit2();
	afx_msg void OnChangeEdit3();
	afx_msg void OnSelchangeOperand11();
	afx_msg void OnSelchangeOperand12();
	afx_msg void OnSelchangeOperand13();
	afx_msg void OnSelchangeOperand14();
	afx_msg void OnSelchangeOperand21();
	afx_msg void OnSelchangeOperand22();
	afx_msg void OnSelchangeOperand23();
	afx_msg void OnSelchangeOperand24();
	afx_msg void OnSelchangeOperand31();
	afx_msg void OnSelchangeOperand32();
	afx_msg void OnSelchangeOperand33();
	afx_msg void OnSelchangeOperand34();
	afx_msg void OnEditchangeOperandType11();
	afx_msg void OnEditchangeOperandType12();
	afx_msg void OnEditchangeOperandType13();
	afx_msg void OnEditchangeOperandType14();
	afx_msg void OnEditchangeOperandType21();
	afx_msg void OnEditchangeOperandType22();
	afx_msg void OnEditchangeOperandType23();
	afx_msg void OnEditchangeOperandType24();
	afx_msg void OnEditchangeOperandType31();
	afx_msg void OnEditchangeOperandType32();
	afx_msg void OnEditchangeOperandType33();
	afx_msg void OnEditchangeOperandType34();

	afx_msg void OnSelchangeType();
	afx_msg void OnApply();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPSDIALOG_H__F6318C6D_D68B_11D2_BBD1_00C04FB17504__INCLUDED_)
