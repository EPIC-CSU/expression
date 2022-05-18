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
#if !defined(AFX_FILEPROPPAGE_H__A3DC9D2E_B500_11D2_BBB8_00C04FB17504__INCLUDED_)
#define AFX_FILEPROPPAGE_H__A3DC9D2E_B500_11D2_BBB8_00C04FB17504__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// FilePropPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// FilePropPage dialog

#include "BasePropPage.h"

class FilePropPage : public BasePropPage
{
// Construction
public:
	virtual void PutDataIntoObject();
	virtual void GetDataFromObject();
	virtual void Reset(void);
	FilePropPage(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(FilePropPage)
	enum { IDD = IDD_PROP_FILE };
	CButton	m_Undo;
	CButton	m_Apply;
	CString	m_Name;
	BOOL	m_AfterTBZ;
	BOOL	m_DoMut;
	BOOL	m_DoTbz;
	BOOL	m_DoTbzPipe;
	BOOL	m_PrintCFG;
	BOOL	m_PrintHTG;
	BOOL	m_PrintIL;
	BOOL	m_SimHTG;
	BOOL	m_DoISel;
	BOOL	m_SimISel;
	BOOL	m_SimNoSSA;
	BOOL	m_SimSSA;
	BOOL	m_SimTbz;
	BOOL	m_PrintMC;
	BOOL	m_DoUnroll;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(FilePropPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(FilePropPage)
	afx_msg void OnMove(int x, int y);
	afx_msg void OnUndo();
	afx_msg void OnApply();
	afx_msg void OnAfterTbz();
	afx_msg void OnDoIsel();
	afx_msg void OnDoMut();
	afx_msg void OnDoTbz();
	afx_msg void OnDoTbzPipe();
	afx_msg void OnPrintCfg();
	afx_msg void OnPrintHtg();
	afx_msg void OnPrintIl();
	afx_msg void OnSimHtg();
	afx_msg void OnSimIsel();
	afx_msg void OnSimNossa();
	afx_msg void OnSimSsa();
	afx_msg void OnSimTbz();
	afx_msg void OnDoUnroll();
	afx_msg void OnPrintMC();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILEPROPPAGE_H__A3DC9D2E_B500_11D2_BBB8_00C04FB17504__INCLUDED_)
