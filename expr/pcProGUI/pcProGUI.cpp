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
// pcProGUI.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "pcProGUI.h"

#include "MainFrm.h"
#include "ChildFrm.h"
#include "pcProGUIDoc.h"
#include "pcProGUIView.h"

//CFG includes
#include "CFG/CFGDoc.h"
#include "CFG/CFGView.h"
#include "CFG/CFGFrame.h"

//ILG includes
#include "ILG/ILGDoc.h"
#include "ILG/ILGView.h"
#include "ILG/ILGFrame.h"

//HTG includes
#include "HTG/HTGDoc.h"
#include "HTG/HTGView.h"
#include "HTG/HTGFrame.h"

//SIM includes
#include "SIM/SIMDoc.h"
#include "SIM/SIMView.h"
#include "SIM/SIMFrame.h"

#include "Prop/PropPageList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// pcProGUIApp

BEGIN_MESSAGE_MAP(pcProGUIApp, CWinApp)
	//{{AFX_MSG_MAP(pcProGUIApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// pcProGUIApp construction

pcProGUIApp::pcProGUIApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
 simDoc=NULL;
 propPageList=new PropPageList();
}

/////////////////////////////////////////////////////////////////////////////
// The one and only pcProGUIApp object

pcProGUIApp theApp;

/////////////////////////////////////////////////////////////////////////////
// pcProGUIApp initialization

BOOL pcProGUIApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

 //NOS REMOVE IS RAND NEEDS reproducible results
 srand((unsigned)time(NULL));

 	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	AfxInitRichEdit();

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	// You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("UCI-CESD"));

	LoadStdProfileSettings(6);  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	// serve as the connection between documents, frame windows and views.

	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(
		IDR_PCPROTYPE,
		RUNTIME_CLASS(pcProGUIDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(pcProGUIView));
	AddDocTemplate(pDocTemplate);

 //NOS
	m_CFG = new CMultiDocTemplate(
		IDR_MENU_CFG,
		RUNTIME_CLASS(CFGDoc),
		RUNTIME_CLASS(CFGFrame), // custom MDI child frame
		RUNTIME_CLASS(CFGView));
	AddDocTemplate(m_CFG);

	m_ILG = new CMultiDocTemplate(
		IDR_MENU_ILG,
		RUNTIME_CLASS(ILGDoc),
		RUNTIME_CLASS(ILGFrame), // custom MDI child frame
		RUNTIME_CLASS(ILGView));
	AddDocTemplate(m_ILG);

	m_HTG = new CMultiDocTemplate(
		IDR_MENU_HTG,
		RUNTIME_CLASS(HTGDoc),
		RUNTIME_CLASS(HTGFrame), // custom MDI child frame
		RUNTIME_CLASS(HTGView));
	AddDocTemplate(m_HTG);

	m_SIM = new CMultiDocTemplate(
		IDR_SIMULATOR,
		RUNTIME_CLASS(SIMDoc),
		RUNTIME_CLASS(SIMFrame), // custom MDI child frame
		RUNTIME_CLASS(SIMView));
	AddDocTemplate(m_SIM);

	// create main MDI Frame window
	MainFrame* pMainFrame = new MainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;

	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();

	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

 //disable the creation of an empty project
 if (cmdInfo.m_nShellCommand==CCommandLineInfo::FileNew) cmdInfo.m_nShellCommand=CCommandLineInfo::FileNothing;

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The main window has been initialized, so show and update it.
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void pcProGUIApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// pcProGUIApp commands

pcProGUIApp::~pcProGUIApp()
{
 //delete all pages from m_Properties
 //delete pPropSheet;
 delete propPageList;
}
