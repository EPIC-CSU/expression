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
#if !defined(AFX_SIMVIEW_H__1CCD8335_1CC5_11D2_B73B_00C04FB17504__INCLUDED_)
#define AFX_SIMVIEW_H__1CCD8335_1CC5_11D2_B73B_00C04FB17504__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SIMView.h : header file
//

//#include "../MouseHitInfo.h"
//#include "SIMGraphObj.h"
class SIMGraphObj;

/////////////////////////////////////////////////////////////////////////////
// SIMView view

class SIMView : public CScrollView
{
protected:
	SIMView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(SIMView)

// Attributes
public:
// MouseHitInfo* hit;
 bool lockDraw;
 bool noDoc;
 bool dragging;
 bool buttonDown;
 bool bDatapath;
 bool bSimulatorActive;
 CPoint dragStart;
 SIMGraphObj* dragObject;
 SIMGraphObj* viewSelection;
 CString newDataPath;

 //for connections
 bool bSelectingStart;
 SIMGraphObj* startObj;
 CPoint startObjPoint;
 SIMGraphObj* endObj;
 CPoint endObjPoint;
 //for ports
 SIMGraphObj* ownerObj;
// Operations
public:
	void SwitchToStats(void);
	void genO(CFile* file, int level);
	void genC(CFile* file, int level);
	void RemoveObj(SIMGraphObj* obj);
 void AddSIMGraphObj(CPoint& point);
	CFont* viewFont;
 CBrush* viewBrush;
 void xArch(CFile* file, int level);
 
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SIMView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnInitialUpdate();     // first time after construct
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~SIMView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(SIMView)
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnPopupExpand();
	afx_msg void OnPopupProperties();
	afx_msg void OnPopupUDDU();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnAddUnit();
	afx_msg void OnAddStorage();
	afx_msg void OnAddPort();
	afx_msg void OnAddConnection();
	afx_msg void OnSimReset();
	afx_msg void OnSimRun();
	afx_msg void OnSimStep();
	afx_msg void OnSimStop();
	afx_msg void OnDefault();
	afx_msg void OnDelete();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnAddCompUnit();
	afx_msg void OnFileSaveExprDescr();
	afx_msg void OnFileLoadGraphicalMachineDescription();
	afx_msg void OnFileSaveGraphicalMachineDescription();
	afx_msg void OnAddLatch();
	afx_msg void OnAddPipelineStage();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnAddDatapath();
	afx_msg void OnEditDatapaths();
	afx_msg void OnResizeObj();
	afx_msg void OnTempSwitchtostatistics();
	afx_msg void OnISSetInstructionDescription();
	afx_msg void OnISSetOpMappings();
	afx_msg void OnISSetOperandMappings();
	afx_msg void OnISSetOpGroups();
	afx_msg void OnISSetVarGroups();
	afx_msg void OnSaveISDescr();
	afx_msg void OnLoadISDescr();
	afx_msg void OnAddBus();
	afx_msg void OnFileSaveExprDescrAsC();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SIMVIEW_H__1CCD8335_1CC5_11D2_B73B_00C04FB17504__INCLUDED_)
