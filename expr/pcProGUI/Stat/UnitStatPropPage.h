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
#if !defined(AFX_UNITSTATPROPPAGE_H__C6F04772_CB4F_11D2_BBCA_00C04FB17504__INCLUDED_)
#define AFX_UNITSTATPROPPAGE_H__C6F04772_CB4F_11D2_BBCA_00C04FB17504__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// UnitStatPropPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// UnitStatPropPage dialog

class GUIBaseStat;

class UnitStatPropPage : public BasePropPage
{
// Construction
public:
	void AddStat(GUIBaseStat* stat);
	void GetDataFromObject(void);
	UnitStatPropPage(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(UnitStatPropPage)
	enum { IDD = IDD_STAT_UNIT };
	CListBox	m_Stats;
	CString	m_Memo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(UnitStatPropPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(UnitStatPropPage)
	afx_msg void OnMove(int x, int y);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UNITSTATPROPPAGE_H__C6F04772_CB4F_11D2_BBCA_00C04FB17504__INCLUDED_)
