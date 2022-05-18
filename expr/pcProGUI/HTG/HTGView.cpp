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
// HTGView.cpp : implementation file
//

#include "stdafx.h"
#include "../pcProGUI.h"

#include "HTGView.h"
#include "HTGDoc.h"

#include "Routine.h"

#include "../DOTGraph.h"

#include "../Prop/PropPageList.h"

#include "../Prop/InstrPropPage.h"
#include "../Prop/BBlockPropPage.h"
#include "../Prop/HTGCompundPropPage.h"
#include "../Prop/HTGSimplePropPage.h"
#include "../Prop/HTGLoopPropPage.h"
#include "../Prop/HTGIfPropPage.h"

#include "../../pcProDll/include/NormalInstr.h"
#include "../../pcProDll/include/IfHTGNode.h"
#include "../../pcProDll/include/SimpleHTGNode.h"
#include "../../pcProDll/include/LoopHTGNode.h"
//#include "../../pcProDll/include/CompundHTGNode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// HTGView

IMPLEMENT_DYNCREATE(HTGView, CScrollView)

HTGView::HTGView()
{
 noDoc=true;
 viewSelection=NULL;
 m_routine=NULL;
}

HTGView::~HTGView()
{
 theApp.propPageList->RemovePage(IDD_PROP_INSTR);
 theApp.propPageList->RemovePage(IDD_PROP_BBLOCK);
 theApp.propPageList->RemovePage(IDD_PROP_HTG_COMPOUND);
 theApp.propPageList->RemovePage(IDD_PROP_HTG_IF);
 theApp.propPageList->RemovePage(IDD_PROP_HTG_SIMPLE);
 theApp.propPageList->RemovePage(IDD_PROP_HTG_LOOP);
}


BEGIN_MESSAGE_MAP(HTGView, CScrollView)
//{{AFX_MSG_MAP(HTGView)
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_COMMAND(ID_PBB_EXPAND, OnPopupExpand)
	ON_COMMAND(ID_PBB_SHOWPROPERTIES, OnPopupProperties)
	ON_COMMAND(ID_PBB_UDDU, OnPopupUDDU)
	ON_WM_SETFOCUS()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_PBB_UD, OnPopupUD)
	ON_COMMAND(ID_PBB_DU, OnPopupDU)
	ON_COMMAND(ID_REFRESH_GRAPH, OnRefreshGraph)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// HTGView drawing

void HTGView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();
 
	CSize sizeTotal;
	// TODO: calculate the total size of this view
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);
}

__declspec (dllimport) bool updateLabel;

void HTGView::OnDraw(CDC* pDC)
{
 int res;
	RECT rect;
	CSize sizeTotal;
	SCROLLINFO scrollInfo;
	HTGDoc* pDoc = dynamic_cast<HTGDoc*>(GetDocument());
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
 res = new_font.CreateFont(FONT_SIZE,0,0,0,0,0,0,0,0,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,FONT_NAME);
	CFont* oldFont = pDC->SelectObject(&new_font);
 
 //text attributes
 pDC->SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
 pDC->SetBkColor(::GetSysColor(COLOR_WINDOW));
 pDC->SetBkMode(TRANSPARENT);
 
 //Get scroll bar positions.
	int xPos = GetScrollPos(SB_HORZ);
	int yPos = GetScrollPos(SB_VERT);
 
 //output node definitions

 MyLinkedListIterator<BaseHTGNode *> *iter=pDoc->m_routine->getHTG()->elementsForward();
	while(iter->hasMoreElements())
 {
		iter->currentElement()->Draw(GNT_HTG,pDC);//,xPos,yPos);
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
 
 // Replace original font and brush
	pDC->SelectObject(oldFont);
 pDC->SelectObject(oldBrush);

 updateLabel=false;
 pDoc->SetModifiedFlag(false);
}

/////////////////////////////////////////////////////////////////////////////
// HTGView diagnostics

#ifdef _DEBUG
void HTGView::AssertValid() const
{
	CScrollView::AssertValid();
}

void HTGView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// HTGView message handlers

void HTGView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	HTGDoc* pDoc = dynamic_cast<HTGDoc*>(GetDocument());
	ASSERT_TRACE(pDoc,"Invalid GetDocument() return type");
 
 //adjust the point for scrolling
 CPoint	scrolledPoint(point.x+GetScrollPos(SB_HORZ),point.y+GetScrollPos(SB_VERT));
 
 //go thru all instructions and find if the user clicked on any
 MouseHitInfo* hit=pDoc->getHit(scrolledPoint);
 if (hit!=NULL)
 {
  hit->ProcessLeftClick(GNT_HTG,&viewSelection);
  //temp, use Draw() later
  RedrawWindow();
  //delete hit;
 }

	CScrollView::OnLButtonUp(nFlags, point);
}

void HTGView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	HTGDoc* pDoc = dynamic_cast<HTGDoc*>(GetDocument());
	ASSERT_TRACE(pDoc,"Invalid GetDocument() return type");
  
 //adjust the point for scrolling
 CPoint	scrolledPoint(point.x+GetScrollPos(SB_HORZ),point.y+GetScrollPos(SB_VERT));
 
 //go thru all instructions and find if the user clicked on any
 hit=pDoc->getHit(scrolledPoint);
 if (hit!=NULL)
 {
  ClientToScreen(&point);
  hit->ProcessRightClick(GNT_HTG,point,this);
  //delete hit;
 }
 else
 {
  ClientToScreen(&point);
  CMenu menu;
  CMenu* pPopup;
  VERIFY(menu.LoadMenu(IDR_MENU_HTG));
  pPopup = menu.GetSubMenu(6);
  ASSERT(pPopup != NULL);
  VERIFY(pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, point.x, point.y, this));
 }

	CScrollView::OnRButtonUp(nFlags, point);
}

void HTGView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CScrollView::OnHScroll(nSBCode, nPos, pScrollBar);
 RedrawWindow();
}

void HTGView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CScrollView::OnVScroll(nSBCode, nPos, pScrollBar);
 RedrawWindow();
}

void HTGView::OnPopupExpand() 
{
	HTGDoc* pDoc = dynamic_cast<HTGDoc*>(GetDocument());
	ASSERT_TRACE(pDoc,"Invalid GetDocument() return type");
  
 ((GenericNode*)pDoc->hitHTG.node)->ToggleExpanded(GNT_HTG);
 pDoc->SetModifiedFlag();
 RedrawWindow();
 //delete hit;
}

void HTGView::OnPopupProperties() 
{
	//HTGDoc* pDoc = dynamic_cast<HTGDoc*>(GetDocument());
	//ASSERT_TRACE(pDoc,"Invalid GetDocument() return type");
 // 
 //((GenericNode*)hit->node)->ToggleExpanded();
 //pDoc->m_DocIsDirty=true;
 //RedrawWindow();
 //delete hit;
}

void HTGView::OnPopupUDDU() 
{
	HTGDoc* pDoc = dynamic_cast<HTGDoc*>(GetDocument());
	ASSERT_TRACE(pDoc,"Invalid GetDocument() return type");
  
 ((GenericNode*)pDoc->hitHTG.node)->ToggleUDDU(GNT_HTG);
 pDoc->SetModifiedFlag();
 RedrawWindow();
 //delete hit;
}

void HTGView::OnPopupUD() 
{
	HTGDoc* pDoc = dynamic_cast<HTGDoc*>(GetDocument());
	ASSERT_TRACE(pDoc,"Invalid GetDocument() return type");
  
 ((GenericNode*)pDoc->hitHTG.node)->ToggleUD(GNT_HTG);
 pDoc->SetModifiedFlag();
 RedrawWindow();
 //delete hit;
}

void HTGView::OnPopupDU() 
{
	HTGDoc* pDoc = dynamic_cast<HTGDoc*>(GetDocument());
	ASSERT_TRACE(pDoc,"Invalid GetDocument() return type");
  
 ((GenericNode*)pDoc->hitHTG.node)->ToggleDU(GNT_HTG);
 pDoc->SetModifiedFlag();
 RedrawWindow();
 //delete hit;
}

void HTGView::OnRefreshGraph() 
{
	HTGDoc* pDoc = dynamic_cast<HTGDoc*>(GetDocument());
	ASSERT_TRACE(pDoc,"Invalid GetDocument() return type");
 pDoc->SetModifiedFlag();
 RedrawWindow();
}

void HTGView::OnSetFocus(CWnd* pOldWnd) 
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

  if (dynamic_cast<IfHTGNode*>(viewSelection)!=NULL)
  {
   theApp.propPageList->ShowPage(IDD_PROP_HTG_IF, viewSelection);
   return;
  }

  if (dynamic_cast<SimpleHTGNode*>(viewSelection)!=NULL)
  {
   theApp.propPageList->ShowPage(IDD_PROP_HTG_SIMPLE, viewSelection);
   return;
  }

  if (dynamic_cast<LoopHTGNode*>(viewSelection)!=NULL)
  {
   theApp.propPageList->ShowPage(IDD_PROP_HTG_LOOP, viewSelection);
   return;
  }

  //if (dynamic_cast<CompundHTGNode*>(viewSelection)!=NULL)
  //{
  // theApp.propPageList->ShowPage(IDD_PROP_HTG_IF);
  // return;
  //}

  ASSERT(0);
 }
}


int HTGView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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
		
 HTGCompundPropPage* htgCompundPropPage= new HTGCompundPropPage();
 htgCompundPropPage->Create(htgCompundPropPage->pageID,this);
 theApp.propPageList->AddPage(htgCompundPropPage->pageID,htgCompundPropPage);
		
 HTGIfPropPage* htgIfPropPage= new HTGIfPropPage();
 htgIfPropPage->Create(htgIfPropPage->pageID,this);
 theApp.propPageList->AddPage(htgIfPropPage->pageID,htgIfPropPage);
		
 HTGLoopPropPage* htgLoopPropPage= new HTGLoopPropPage();
 htgLoopPropPage->Create(htgLoopPropPage->pageID,this);
 theApp.propPageList->AddPage(htgLoopPropPage->pageID,htgLoopPropPage);
		
 HTGSimplePropPage* htgSimplePropPage= new HTGSimplePropPage();
 htgSimplePropPage->Create(htgSimplePropPage->pageID,this);
 theApp.propPageList->AddPage(htgSimplePropPage->pageID,htgSimplePropPage);
		
 BasePropPage::bNoUpdatePos=false;

	return 0;
}

BOOL HTGView::OnEraseBkgnd(CDC* pDC) 
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

