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
// Console.cpp : implementation file
//

#include "stdafx.h"

/*
#include "../pcProGUI.h"
#include "Console.h"

#define ID_TOOLBAR 10001
#define NUM_BUTTONS 3
#define NUM_BUTTON_BITMAPS 2

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConsole dialog

HWND ConsoleWin;

CConsole::CConsole(CWnd* pParent)
	: CDialog(CConsole::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConsole)
	//}}AFX_DATA_INIT
}


void CConsole::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConsole)
	DDX_Control(pDX, IDC_STATUS, m_Status);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConsole, CDialog)
	//{{AFX_MSG_MAP(CConsole)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConsole message handlers

BOOL CConsole::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
 ConsoleWin=m_Status.m_hWnd;
	
 //add a toolbar
 m_hToolbar=CreateToolBar(this->m_hWnd);
 ::ShowWindow(m_hToolbar,SW_SHOW);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CConsole::OnClose() 
{
	ShowWindow(SW_HIDE);
	
	CDialog::OnClose();
}



HWND CConsole::CreateToolBar(HWND hwndParent) 
{ 
 HWND hwndTB; 
 TBADDBITMAP tbab; 
 TBBUTTON tbb[NUM_BUTTONS]; 
	int i=0;
 
 // Create a toolbar that the user can customize and that has a 
 // tooltip associated with it. 
 hwndTB = CreateWindowEx(0, TOOLBARCLASSNAME, (LPSTR) NULL, WS_CHILD | TBSTYLE_TOOLTIPS | TBSTYLE_WRAPABLE,0,0,0,0,hwndParent,(HMENU)ID_TOOLBAR,AfxGetInstanceHandle(),NULL); 
 
 // Send the TB_BUTTONSTRUCTSIZE message, which is required for 
 // backward compatibility. 
 ::SendMessage(hwndTB, TB_BUTTONSTRUCTSIZE,(WPARAM) sizeof(TBBUTTON), 0); 
 
 // Add the bitmap containing button images to the toolbar. 
 tbab.hInst = AfxGetInstanceHandle(); 
 tbab.nID   = IDR_CONSOLETOOLBAR; 
 ::SendMessage(hwndTB, TB_ADDBITMAP, (WPARAM) NUM_BUTTON_BITMAPS,(WPARAM) &tbab); 
 
 // Fill the TBBUTTON array with button information, and add the 
 // buttons to the toolbar. 
 tbb[i].iBitmap = 0; 
 tbb[i].idCommand = 0; 
 tbb[i].fsState = TBSTATE_ENABLED; 
 tbb[i].fsStyle = TBSTYLE_SEP; 
 tbb[i].dwData = 0; 
 tbb[i++].iString = 0; 
 
 tbb[i].iBitmap = 0; 
 tbb[i].idCommand = ID_CONSOLE_CLEAR; 
 tbb[i].fsState = TBSTATE_ENABLED; 
 tbb[i].fsStyle = TBSTYLE_BUTTON;
 tbb[i].dwData = 0; 
 tbb[i++].iString = 0; 
 
 tbb[i].iBitmap = 1; 
 tbb[i].idCommand = ID_CONSOLE_COPY; 
 tbb[i].fsState = TBSTATE_ENABLED; 
 tbb[i].fsStyle = TBSTYLE_BUTTON; 
 tbb[i].dwData = 0; 
 tbb[i++].iString = 0; 
 
  
	// add the buttons
 ::SendMessage(hwndTB, TB_ADDBUTTONS, (WPARAM) i, (LPARAM) (LPTBBUTTON) &tbb); 
 
 return hwndTB; 
} 
*/

void UpdateConsoleCOUT(char* str, int streamcount) 
{
 /*
 //use this for the release mode, less debugger-friendly
 //NOTE: must also change UpdateConsole() in ..\MainFrm.cpp
	int l = SendMessage(ConsoleWin,WM_GETTEXTLENGTH,0,0);
	SendMessage(ConsoleWin,EM_SETSEL,l,l);
	SendMessage(ConsoleWin,EM_REPLACESEL,FALSE,(LPARAM)str);
 */

 DWORD t;
 WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE),str,streamcount,&t,NULL); 
}

void UpdateConsoleCERR(char* str, int streamcount) 
{
 /*
 //use this for the release mode, less debugger-friendly
 //NOTE: must also change UpdateConsole() in ..\MainFrm.cpp
	int l = SendMessage(ConsoleWin,WM_GETTEXTLENGTH,0,0);
	SendMessage(ConsoleWin,EM_SETSEL,l,l);
	SendMessage(ConsoleWin,EM_REPLACESEL,FALSE,(LPARAM)str);
 */

 DWORD t;
 //use some high impact colors for error output
 HANDLE hO=GetStdHandle(STD_OUTPUT_HANDLE);
 SetConsoleTextAttribute(hO, FOREGROUND_GREEN|FOREGROUND_RED|BACKGROUND_RED|FOREGROUND_INTENSITY|BACKGROUND_INTENSITY);  
 WriteConsole(hO,str,streamcount,&t,NULL); 
 SetConsoleTextAttribute(hO,FOREGROUND_GREEN|FOREGROUND_INTENSITY);  
}

