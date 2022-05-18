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
// pcProGUI.h : main header file for the PCPROGUI application
//

#if !defined(AFX_PCPROGUI_H__198B295D_1C3E_11D2_B73A_00C04FB17504__INCLUDED_)
#define AFX_PCPROGUI_H__198B295D_1C3E_11D2_B73A_00C04FB17504__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

//#include "MainFrm.h" 
class MainFrame;

//#include "Prop/PropSheet.h"

//#include "Prop/PropPageList.h"
class PropPageList;

//#include "SIM/SIMDoc.h"
class SIMDoc;

/////////////////////////////////////////////////////////////////////////////
// pcProGUIApp:
// See pcProGUI.cpp for the implementation of this class
//

class pcProGUIApp : public CWinApp
{
public:
	CMultiDocTemplate* m_CFG;
	CMultiDocTemplate* m_ILG;
	CMultiDocTemplate* m_HTG;
	CMultiDocTemplate* m_SIM;
 //PropSheet* pPropSheet;

 MainFrame* pMainForm;

 PropPageList* propPageList;

 SIMDoc* simDoc;

public:
	 ~pcProGUIApp();
	pcProGUIApp();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(pcProGUIApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(pcProGUIApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern pcProGUIApp theApp;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PCPROGUI_H__198B295D_1C3E_11D2_B73A_00C04FB17504__INCLUDED_)
