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
// pcProGUIView.h : interface of the pcProGUIView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_PCPROGUIVIEW_H__4E163C78_1CC9_11D2_B73B_00C04FB17504__INCLUDED_)
#define AFX_PCPROGUIVIEW_H__4E163C78_1CC9_11D2_B73B_00C04FB17504__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CFGView;
class CFGDoc;

class ILGView;
class ILGDoc;

class HTGView;
class HTGDoc;

class SIMView;
class SIMDoc;

class pcProGUIView : public CTreeView
{
protected: // create from serialization only
	pcProGUIView();
	DECLARE_DYNCREATE(pcProGUIView)

 CFGView* cfgView;
	CFGDoc* cfgDoc;

	ILGView* ilgView;
	ILGDoc* ilgDoc;

	HTGView* htgView;
	HTGDoc* htgDoc;

	SIMView* simView;
	SIMDoc* simDoc;

// InstrPropPage* instrPropPage;

// Attributes
public:
	pcProGUIDoc* GetDocument();
 CImageList m_ImageList;
 void* viewSelection;
 //NOS

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(pcProGUIView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	void CreateCommandLineParams();
//	void DisplayProperties();
 void ChangeSelection();
//	void AddPropPages();
	virtual ~pcProGUIView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
 ProjectFile* GetSelectedFile();

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(pcProGUIView)
	afx_msg void OnProjectAdd();
	afx_msg void OnProjectRemove();
	afx_msg void OnProjectBuild();
	afx_msg void OnProjectCompile();
	afx_msg void OnGraphCFG();
	afx_msg void OnGraphILG();
	afx_msg void OnGraphHTG();
	afx_msg void OnNewSIM();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnCompileStepcompileBuildHtg();
	afx_msg void OnCompileStepcompileSSA();
	afx_msg void OnCompileStepcompileTrailblazing();
	afx_msg void OnCompileStepcompileUDLIVE();
	afx_msg void OnCompileStepcompileInitializecompiler();
	afx_msg void OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnIfConvert();
	afx_msg void OnPrintRoutine();
	afx_msg void OnPercolate();
	afx_msg void OnMarkBlocks();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in pcProGUIView.cpp
inline pcProGUIDoc* pcProGUIView::GetDocument()
   { return (pcProGUIDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PCPROGUIVIEW_H__4E163C78_1CC9_11D2_B73B_00C04FB17504__INCLUDED_)
