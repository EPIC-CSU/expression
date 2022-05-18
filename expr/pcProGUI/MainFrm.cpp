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
// MainFrm.cpp : implementation of the MainFrame class
//

#include "stdafx.h"
#include "pcProGUI.h"
#include "MainFrm.h"

#include "Prop/BasePropPage.h"

#include "Prop/PropPageList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//NOS
//int pcProGUIMain(char*, my_ostream*);
//void pcProGUIMain(void*);
void pcProMain(void*);
void UpdateConsoleCOUT(char* str, int streamcount);
void UpdateConsoleCERR(char* str, int streamcount);

/////////////////////////////////////////////////////////////////////////////
// MainFrame

IMPLEMENT_DYNAMIC(MainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(MainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(MainFrame)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
	// Global help commands
	ON_COMMAND(ID_HELP_FINDER, CMDIFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_HELP, CMDIFrameWnd::OnHelp)
	ON_COMMAND(ID_CONTEXT_HELP, CMDIFrameWnd::OnContextHelp)
	ON_COMMAND(ID_DEFAULT_HELP, CMDIFrameWnd::OnHelpFinder)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// MainFrame construction/destruction

MainFrame::MainFrame()
{
	//m_pPropFrame = NULL;
 theApp.pMainForm=this;
}

MainFrame::~MainFrame()
{
 theApp.propPageList->RemovePage(IDD_PROP_BASE);
}

int MainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.Create(this) ||	!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create MAIN toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndExpressionBar.Create(this) || !m_wndExpressionBar.LoadToolBar(IDR_EXPRESSION))
	{
		TRACE0("Failed to create EXPRESSION toolbar\n");
		return -1;      // fail to create
	}

	/*if (!m_wndSimulatorBar.Create(this) || !m_wndSimulatorBar.LoadToolBar(IDR_SIMULATOR))
	{
		TRACE0("Failed to create EXPRESSION toolbar\n");
		return -1;      // fail to create
	}*/

	/*if (!m_wndCompilerBar.Create(this) || !m_wndCompilerBar.LoadToolBar(IDR_COMPILER))
	{
		TRACE0("Failed to create EXPRESSION toolbar\n");
		return -1;      // fail to create
	}*/

	/*if (!m_wndGraphsBar.Create(this) || !m_wndGraphsBar.LoadToolBar(IDR_GRAPHS))
	{
		TRACE0("Failed to create EXPRESSION toolbar\n");
		return -1;      // fail to create
	}*/

 //set toolbar titles
 m_wndToolBar.GetToolBarCtrl().SetWindowText("Project");
 m_wndExpressionBar.GetToolBarCtrl().SetWindowText("Components");
 //m_wndSimulatorBar.GetToolBarCtrl().SetWindowText("Simulator");
 //m_wndCompilerBar.GetToolBarCtrl().SetWindowText("Compiler");
 //m_wndGraphsBar.GetToolBarCtrl().SetWindowText("Graphs");

	if (!m_wndStatusBar.Create(this) ||		!m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Remove this if you don't want tool tips or a resizeable toolbar
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |	CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	m_wndExpressionBar.SetBarStyle(m_wndExpressionBar.GetBarStyle() |	CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
 //m_wndSimulatorBar.SetBarStyle(m_wndToolBar.GetBarStyle() |	CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
 //m_wndCompilerBar.SetBarStyle(m_wndToolBar.GetBarStyle() |	CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
 //m_wndGraphsBar.SetBarStyle(m_wndToolBar.GetBarStyle() |	CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

 //dock the toolbars 
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar,AFX_IDW_DOCKBAR_TOP);

 CRect r;
 //m_wndToolBar.GetWindowRect(&r);
 //m_wndToolBar.GetToolBarCtrl().GetWindowRect(&r);

	//m_wndCompilerBar.EnableDocking(CBRS_ALIGN_ANY);
	//EnableDocking(CBRS_ALIGN_ANY);
	//DockControlBar(&m_wndSimulatorBar,AFX_IDW_DOCKBAR_TOP, &r);
	//DockControlBarLeftOf(&m_wndCompilerBar, &m_wndToolBar);

	//m_wndGraphsBar.EnableDocking(CBRS_ALIGN_ANY);
	//EnableDocking(CBRS_ALIGN_ANY);
	//DockControlBar(&m_wndSimulatorBar,AFX_IDW_DOCKBAR_TOP, &r);
	//DockControlBarLeftOf(&m_wndGraphsBar, &m_wndCompilerBar);

//	m_wndSimulatorBar.EnableDocking(CBRS_ALIGN_ANY);
//	EnableDocking(CBRS_ALIGN_ANY);
	//DockControlBar(&m_wndSimulatorBar,AFX_IDW_DOCKBAR_TOP, &r);
//	DockControlBarLeftOf(&m_wndSimulatorBar, &m_wndGraphsBar);

	m_wndExpressionBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndExpressionBar,AFX_IDW_DOCKBAR_LEFT);

 //setup the expression toolbar
 for (int i=0;i<10;i++) m_wndExpressionBar.SetButtonStyle(i,TBBS_CHECKGROUP);
 m_wndExpressionBar.GetToolBarCtrl().CheckButton(ID_DEFAULT);

 /*
 //use this for the release mode, less debugger-friendly (see below)
 //NOTE: must also change UpdateConsoleCOUT() in console\console.cpp
 //create and show the console
 m_Console.Create(IDD_CONSOLE,this);
 m_Console.ShowWindow(SW_SHOW);
 */

 //NOS
 //for debugging purposes a "real" console is better for debugging since it runs in a separate thread
 //therefore it's not affected by the debugger stopping the main program
 AllocConsole();
 HANDLE hO=GetStdHandle(STD_OUTPUT_HANDLE); 
 SetConsoleTitle("EXPRESS Console");
 //CONSOLE_SCREEN_BUFFER_INFO csbi;
 //GetConsoleScreenBufferInfo(hConsoleOutput,&csbi);
 /*
 SMALL_RECT sr;
 sr.Left=1;
 sr.Top=1;
 sr.Right=101;
 sr.Bottom=51;
 SetConsoleWindowInfo(hO,TRUE,&sr);
 COORD c;
 c.X=100;
 c.Y=8000;
 SetConsoleScreenBufferSize(hO,c); 
 */


 //create a new stream to capture "cout" output
 //m_cout=new my_ostream(&UpdateConsole);

 //CWinThread* hT=AfxBeginThread((AFX_THREADPROC)pcProGUIMain, m_cout);
 //unsigned long hT=_beginthread( pcProGUIMain, 0, m_cout );


	// If mini frame does not already exist, create a new one.
	// Otherwise, unhide it

/*
	if (m_pPropFrame == NULL)
	{
		m_pPropFrame = new PropFrame;
		CRect rect(0, 0, 0, 0);
		CString strTitle;
		//strTitle.LoadString(IDS_OBJECT_PROPERTIES);
  strTitle="Object Properties";
		if (!m_pPropFrame->Create(NULL, strTitle, WS_POPUP | WS_CAPTION | WS_SYSMENU, rect, this))
		{
			m_pPropFrame = NULL;
			return 0;
		}
		m_pPropFrame->CenterWindow();
	}
*/
 
	//// Before unhiding the modeless property sheet, update its
	//// settings to reflect the currently selected shape.
	//CShapesView* pView = STATIC_DOWNCAST(CShapesView, MDIGetActive()->GetActiveView());
	//ASSERT_VALID(pView);
	//if (pView->m_pShapeSelected != NULL)
	//{
 // m_pPropFrame->m_pModelessShapePropSheet->SetSheetPropsFromShape(pView->m_pShapeSelected);
	//}
 
 /*
	if (m_pPropFrame != NULL && !m_pPropFrame->IsWindowVisible()) m_pPropFrame->ShowWindow(SW_SHOW);
 */

 //add prop pages for this view
 BasePropPage::bNoUpdatePos=true;

 BasePropPage* basePropPage= new BasePropPage();
 basePropPage->Create(basePropPage->pageID,this);

 //move the first prop page out of the way
 CRect r1,r2;
 CPoint p1,p2,p3;
 theApp.pMainForm->GetWindowRect(r1);
 basePropPage->GetWindowRect(r2);
 int h=r2.Height();
 int w=r2.Width();
 r2.top=r1.top+100;
 r2.left=r1.right-200;
 r2.bottom=r2.top+h;
 r2.right=r2.left+w;
 basePropPage->MoveWindow(r2);

 theApp.propPageList->AddPage(basePropPage->pageID,basePropPage);

 BasePropPage::bNoUpdatePos=false;

	return 0;
}

BOOL MainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CMDIFrameWnd::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// MainFrame diagnostics

#ifdef _DEBUG
void MainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void MainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// MainFrame message handlers

void MainFrame::UpdateExpressionToolbarAndMenus(UINT uID)
{
 CMenu* hMenu=theApp.m_pMainWnd->GetMenu();
 
 hMenu->CheckMenuItem(ID_DEFAULT,MF_BYCOMMAND|(uID==ID_DEFAULT?MF_CHECKED:MF_UNCHECKED));

 hMenu->CheckMenuItem(ID_ADD_COMP_UNIT,MF_BYCOMMAND|(uID==ID_ADD_COMP_UNIT?MF_CHECKED:MF_UNCHECKED));
 hMenu->CheckMenuItem(ID_ADD_UNIT,MF_BYCOMMAND|(uID==ID_ADD_UNIT?MF_CHECKED:MF_UNCHECKED));
 hMenu->CheckMenuItem(ID_ADD_STORAGE,MF_BYCOMMAND|(uID==ID_ADD_STORAGE?MF_CHECKED:MF_UNCHECKED));
 hMenu->CheckMenuItem(ID_ADD_LATCH,MF_BYCOMMAND|(uID==ID_ADD_LATCH?MF_CHECKED:MF_UNCHECKED)); 
 hMenu->CheckMenuItem(ID_ADD_CONNECTION,MF_BYCOMMAND|(uID==ID_ADD_CONNECTION?MF_CHECKED:MF_UNCHECKED));
 hMenu->CheckMenuItem(ID_ADD_BUS,MF_BYCOMMAND|(uID==ID_ADD_BUS?MF_CHECKED:MF_UNCHECKED));
 hMenu->CheckMenuItem(ID_ADD_PORT,MF_BYCOMMAND|(uID==ID_ADD_PORT?MF_CHECKED:MF_UNCHECKED));

 hMenu->CheckMenuItem(ID_ADD_PIPELINESTAGE,MF_BYCOMMAND|(uID==ID_ADD_PIPELINESTAGE?MF_CHECKED:MF_UNCHECKED));

 hMenu->CheckMenuItem(ID_BREAK,MF_BYCOMMAND|(uID==ID_BREAK?MF_CHECKED:MF_UNCHECKED));  

 hMenu->CheckMenuItem(ID_ADD_DATAPATH,MF_BYCOMMAND|(uID==ID_ADD_DATAPATH?MF_CHECKED:MF_UNCHECKED));  

 m_wndExpressionBar.GetToolBarCtrl().CheckButton(uID);  
}

void MainFrame::HideModelessPropSheet()
{
	//if (m_pPropFrame != NULL)	m_pPropFrame->ShowWindow(SW_HIDE);
}

void MainFrame::OnSetFocus(CWnd* pOldWnd) 
{
	CMDIFrameWnd::OnSetFocus(pOldWnd);

	//theApp.propPageList.ShowPage(IDD_PROP_BASE);
}

void MainFrame::DockControlBarLeftOf(CToolBar * Bar, CToolBar * LeftOf)
{
	CRect rect;
	DWORD dw;
	UINT n;

	// get MFC to adjust the dimensions of all docked ToolBars
	// so that GetWindowRect will be accurate
	RecalcLayout();
	LeftOf->GetWindowRect(&rect);
	rect.OffsetRect(1,0);
	dw=LeftOf->GetBarStyle();
	n = 0;
	n = (dw&CBRS_ALIGN_TOP) ? AFX_IDW_DOCKBAR_TOP : n;
	n = (dw&CBRS_ALIGN_BOTTOM && n==0) ? AFX_IDW_DOCKBAR_BOTTOM : n;
	n = (dw&CBRS_ALIGN_LEFT && n==0) ? AFX_IDW_DOCKBAR_LEFT : n;
	n = (dw&CBRS_ALIGN_RIGHT && n==0) ? AFX_IDW_DOCKBAR_RIGHT : n;

	// When we take the default parameters on rect, DockControlBar will dock
	// each Toolbar on a seperate line.  By calculating a rectangle, we in effect
	// are simulating a Toolbar being dragged to that location and docked.
	DockControlBar(Bar,n,&rect);
}
