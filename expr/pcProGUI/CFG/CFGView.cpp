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
// CFGView.cpp : implementation file
//

#include "stdafx.h"
#include "../pcprogui.h"

#include "CFGView.h"
#include "CFGDoc.h"

#include "routine.h"

#include "../DOTGraph.h"

#include "../Prop/PropPageList.h"

#include "../Prop/InstrPropPage.h"
#include "../Prop/BBlockPropPage.h"

#include "../../pcProDll/include/NormalInstr.h"
#include "../../pcProDll/include/Flags.h"
#include "../../pcProDll/include/FlagsDefs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFGView

IMPLEMENT_DYNCREATE(CFGView, CScrollView)

CFGView::CFGView()
{
 noDoc=true;
 viewSelection=NULL;
 m_routine=NULL;
}

CFGView::~CFGView()
{
 theApp.propPageList->RemovePage(IDD_PROP_INSTR);
 theApp.propPageList->RemovePage(IDD_PROP_BBLOCK);
}


BEGIN_MESSAGE_MAP(CFGView, CScrollView)
//{{AFX_MSG_MAP(CFGView)
 ON_WM_LBUTTONUP()
 ON_WM_HSCROLL()
 ON_WM_VSCROLL()
 ON_WM_RBUTTONUP()
	ON_COMMAND(ID_PBB_EXPAND, OnPopupExpand)
	ON_COMMAND(ID_PBB_SHOWPROPERTIES, OnPopupProperties)
	ON_COMMAND(ID_PBB_UDDU, OnPopupUDDU)
	ON_COMMAND(ID_PBB_PRINT, OnPrintInstr)
	ON_WM_SETFOCUS()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_PBB_DU, OnPopupDU)
	ON_COMMAND(ID_PBB_UD, OnPopupUD)
	ON_COMMAND(ID_PBB_SETSTART, OnSetStartBlock)
	ON_COMMAND(ID_PBB_SETSTOP, OnSetStopBlock)
	ON_COMMAND(ID_REFRESH_GRAPH, OnRefreshGraph)
	ON_COMMAND(ID_PBB_RESETPERC, OnResetPerc)
	ON_COMMAND(ID_PBB_SETPERC, OnSetPerc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFGView drawing

void CFGView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();
 
	CSize sizeTotal;
	// TODO: calculate the total size of this view
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);
}

__declspec (dllimport) bool updateLabel;

void CFGView::OnDraw(CDC* pDC)
{
 int res;
	RECT rect;
	CSize sizeTotal;
	SCROLLINFO scrollInfo;
	CFGDoc* pDoc = dynamic_cast<CFGDoc*>(GetDocument());
	ASSERT_TRACE(pDoc,"Invalid GetDocument() return type");
 
 if (pDoc->IsModified()) 
 {
  CWaitCursor cwc;
  updateLabel=true;
  pDoc->GenerateGraphDoc();
  noDoc=false;
 }
 
 //setup the DC params
 
 //brush
 CBrush hollowBrush;
 hollowBrush.CreateStockObject(HOLLOW_BRUSH);
 CBrush* oldBrush=pDC->SelectObject(&hollowBrush);
 
 //font
	CFont new_font;
	//res = new_font.CreateFont(-10,0,0,0,0,0,0,0,0,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,"MS Sans Serif");
	res = new_font.CreateFont(FONT_SIZE,0,0,0,0,0,0,0,0,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,FONT_NAME);
	CFont* oldFont = pDC->SelectObject(&new_font);
 
 //text attributes
 pDC->SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
 pDC->SetBkColor(::GetSysColor(COLOR_WINDOW));
 pDC->SetBkMode(TRANSPARENT);
 
	// Get scroll bar positions.
 int xPos = GetScrollPos(SB_HORZ);
	int yPos = GetScrollPos(SB_VERT);
 
 //output node definitions
 LinkedListIterator<BasicBlockNode> *iter = pDoc->m_routine->getCFG().createBBIterForw();
 while (iter->hasMoreElements()) 
 {
  iter->currentElement().Draw(GNT_CFG,pDC);//,xPos,yPos);
  iter->nextElement();
 }
 delete iter;
 
 //update scrollbars
 //get window size.
	GetClientRect(&rect);
	//set the total size of this view
	sizeTotal.cx = pDoc->xBound + 2 * POINT_OFFSET;
 sizeTotal.cy = pDoc->yBound + 2 * POINT_OFFSET;
	SetScrollSizes(MM_TEXT, sizeTotal);
 //initialize the scroll page size for the vertical scroll bar
 scrollInfo.nPage = rect.bottom;
 scrollInfo.fMask = SIF_PAGE;
	VERIFY(SetScrollInfo(SB_VERT,&scrollInfo));
 //initialize the scroll page size for the horizontal scroll bar
 scrollInfo.nPage = rect.right;
 scrollInfo.fMask = SIF_PAGE;
	VERIFY(SetScrollInfo(SB_HORZ,&scrollInfo));
 
 //restore original font and brush
	pDC->SelectObject(oldFont);
 pDC->SelectObject(oldBrush);

 updateLabel=false;
 pDoc->SetModifiedFlag(false);
}

/////////////////////////////////////////////////////////////////////////////
// CFGView diagnostics

#ifdef _DEBUG
void CFGView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CFGView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CFGView message handlers

void CFGView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CFGDoc* pDoc = dynamic_cast<CFGDoc*>(GetDocument());
	ASSERT_TRACE(pDoc,"Invalid GetDocument() return type");
 
 //adjust the point for scrolling
 CPoint	scrolledPoint(point.x+GetScrollPos(SB_HORZ),point.y+GetScrollPos(SB_VERT));
 
 //go thru all basicblocks/instructions and find if the user clicked on any
 MouseHitInfo* hit=pDoc->getHit(scrolledPoint);
 if (hit!=NULL)
 {
  hit->ProcessLeftClick(GNT_CFG,&viewSelection);
  //temp, use Draw() later
  RedrawWindow();
  //delete hit;
 }
	CScrollView::OnLButtonUp(nFlags, point);
}

void CFGView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	CFGDoc* pDoc = dynamic_cast<CFGDoc*>(GetDocument());
	ASSERT_TRACE(pDoc,"Invalid GetDocument() return type");
  
 //adjust the point for scrolling
 CPoint	scrolledPoint(point.x+GetScrollPos(SB_HORZ),point.y+GetScrollPos(SB_VERT));
 
 //go thru all instructions and find if the user clicked on any
 hit=pDoc->getHit(scrolledPoint);
 if (hit!=NULL)
 {
  ClientToScreen(&point);
  hit->ProcessRightClick(GNT_CFG,point,this);
  //delete hit;
 }
 else
 {
  ClientToScreen(&point);
  CMenu menu;
  CMenu* pPopup;
  VERIFY(menu.LoadMenu(IDR_MENU_CFG));
  pPopup = menu.GetSubMenu(6);
  ASSERT(pPopup != NULL);
  VERIFY(pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, point.x, point.y, this));
 }
	CScrollView::OnRButtonUp(nFlags, point);
}

void CFGView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CScrollView::OnHScroll(nSBCode, nPos, pScrollBar);
 RedrawWindow();
}

void CFGView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CScrollView::OnVScroll(nSBCode, nPos, pScrollBar);
 RedrawWindow();
}

void CFGView::OnPopupExpand() 
{
	CFGDoc* pDoc = dynamic_cast<CFGDoc*>(GetDocument());
	ASSERT_TRACE(pDoc,"Invalid GetDocument() return type");
  
 ((GenericNode*)pDoc->hitCFG.node)->ToggleExpanded(GNT_CFG);
 pDoc->SetModifiedFlag();
 RedrawWindow();
 //delete hit;
}

void CFGView::OnPopupProperties() 
{
	//CFGDoc* pDoc = dynamic_cast<CFGDoc*>(GetDocument());
	//ASSERT_TRACE(pDoc,"Invalid GetDocument() return type");
 // 
 //((GenericNode*)hit->node)->ToggleExpanded();
 //pDoc->m_DocIsDirty=true;
 //RedrawWindow();
 //delete hit;
}

void CFGView::OnPopupUDDU() 
{
	CFGDoc* pDoc = dynamic_cast<CFGDoc*>(GetDocument());
	ASSERT_TRACE(pDoc,"Invalid GetDocument() return type");
  
 ((GenericNode*)pDoc->hitCFG.node)->ToggleUDDU(GNT_CFG);
 pDoc->SetModifiedFlag();
 RedrawWindow();
 //delete hit;
}

void CFGView::OnPopupDU() 
{
	CFGDoc* pDoc = dynamic_cast<CFGDoc*>(GetDocument());
	ASSERT_TRACE(pDoc,"Invalid GetDocument() return type");
  
 ((GenericNode*)pDoc->hitCFG.node)->ToggleDU(GNT_CFG);
 pDoc->SetModifiedFlag();
 RedrawWindow();
 //delete hit;
}

void CFGView::OnPopupUD() 
{
	CFGDoc* pDoc = dynamic_cast<CFGDoc*>(GetDocument());
	ASSERT_TRACE(pDoc,"Invalid GetDocument() return type");
  
 ((GenericNode*)pDoc->hitCFG.node)->ToggleUD(GNT_CFG);
 pDoc->SetModifiedFlag();
 RedrawWindow();
 //delete hit;
}

void CFGView::OnSetStartBlock() 
{
	CFGDoc* pDoc = dynamic_cast<CFGDoc*>(GetDocument());
	ASSERT_TRACE(pDoc,"Invalid GetDocument() return type");
  
 //pDoc->m_routine->startBlock=(BasicBlockNode*)((GenericNode*)pDoc->hitCFG.node);
 BasicBlockNode* bb=dynamic_cast<BasicBlockNode*>((GenericNode*)pDoc->hitCFG.node);
 if (bb!=NULL)
 {
  pDoc->m_routine->startBlock=bb;
 }
}

void CFGView::OnSetStopBlock() 
{
	CFGDoc* pDoc = dynamic_cast<CFGDoc*>(GetDocument());
	ASSERT_TRACE(pDoc,"Invalid GetDocument() return type");
  
 //pDoc->m_routine->stopBlock=(BasicBlockNode*)((GenericNode*)pDoc->hitCFG.node);
 BasicBlockNode* bb=dynamic_cast<BasicBlockNode*>((GenericNode*)pDoc->hitCFG.node);
 if (bb!=NULL)
 {
  pDoc->m_routine->stopBlock=bb;
 }
}

__declspec (dllimport) void Flags::setFlag(int pos, bool val);

void CFGView::OnResetPerc() 
{
	CFGDoc* pDoc = dynamic_cast<CFGDoc*>(GetDocument());
	ASSERT_TRACE(pDoc,"Invalid GetDocument() return type");
  
 BaseInstruction* in=dynamic_cast<BaseInstruction*>((GenericNode*)pDoc->hitCFG.node);
 if (in!=NULL)
 {
  in->flags.setFlag(PERC_FLAG,false);
 }
}

void CFGView::OnSetPerc() 
{
	CFGDoc* pDoc = dynamic_cast<CFGDoc*>(GetDocument());
	ASSERT_TRACE(pDoc,"Invalid GetDocument() return type");
  
 BaseInstruction* in=dynamic_cast<BaseInstruction*>((GenericNode*)pDoc->hitCFG.node);
 if (in!=NULL)
 {
  in->flags.setFlag(PERC_FLAG,true);
 }
}

void CFGView::OnRefreshGraph() 
{
	CFGDoc* pDoc = dynamic_cast<CFGDoc*>(GetDocument());
	ASSERT_TRACE(pDoc,"Invalid GetDocument() return type");  
 pDoc->SetModifiedFlag();
 RedrawWindow();
}

void CFGView::OnPrintInstr() 
{
	CFGDoc* pDoc = dynamic_cast<CFGDoc*>(GetDocument());
	ASSERT_TRACE(pDoc,"Invalid GetDocument() return type");
  
 ((NormalInstr*)viewSelection)->print();
}

void CFGView::OnSetFocus(CWnd* pOldWnd) 
{
	CScrollView::OnSetFocus(pOldWnd);

 if (viewSelection==NULL)
	 theApp.propPageList->ShowPage(IDD_PROP_BASE, NULL);
 else
 {
  if (dynamic_cast<NormalInstr*>(viewSelection)!=NULL)
  {
   theApp.propPageList->ShowPage(IDD_PROP_INSTR, viewSelection);
   return;
  }

  if (dynamic_cast<BasicBlockNode*>(viewSelection)!=NULL)
  {
   theApp.propPageList->ShowPage(IDD_PROP_BBLOCK, viewSelection);
   return;
  }

  ASSERT(0);
 }
}


int CFGView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CScrollView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
 //now add all the property pages that we need
 BasePropPage::bNoUpdatePos=true;

 InstrPropPage* instrPropPage= new InstrPropPage();
 instrPropPage->Create(instrPropPage->pageID,this);
 theApp.propPageList->AddPage(instrPropPage->pageID,instrPropPage);
	
 BBlockPropPage* bBlockPropPage= new BBlockPropPage();
 bBlockPropPage->Create(bBlockPropPage->pageID,this);
 theApp.propPageList->AddPage(bBlockPropPage->pageID,bBlockPropPage);
		
 BasePropPage::bNoUpdatePos=false;

	return 0;
}

BOOL CFGView::OnEraseBkgnd(CDC* pDC) 
{
 //set the default brush to black
 CBrush blackBrush;
 blackBrush.CreateStockObject(BLACK_BRUSH);
 CBrush* oldBrush=pDC->SelectObject(&blackBrush);
 CRect r;
 GetClientRect(r);
 pDC->Rectangle(r);
 pDC->SelectObject(oldBrush);
	
 return 1;
	//return CScrollView::OnEraseBkgnd(pDC);
}
