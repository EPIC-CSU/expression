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
// ILGView.cpp : implementation file
//

#include "stdafx.h"
#include "..\pcprogui.h"

#include "ILGView.h"
#include "ILGDoc.h"

#include "../Prop/PropPageList.h"

#include "../Prop/InstrPropPage.h"

#include "Routine.h"

#include "../DOTGraph.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ILGView

IMPLEMENT_DYNCREATE(ILGView, CScrollView)

ILGView::ILGView()
{
 noDoc=true;
 viewSelection=NULL;
 m_routine=NULL;
}

ILGView::~ILGView()
{
 theApp.propPageList->RemovePage(IDD_PROP_INSTR);
}


BEGIN_MESSAGE_MAP(ILGView, CScrollView)
//{{AFX_MSG_MAP(ILGView)
 ON_WM_HSCROLL()
 ON_WM_VSCROLL()
 ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_PBB_SHOWPROPERTIES, OnPopupProperties)
	ON_COMMAND(ID_PBB_UDDU, OnPopupUDDU)
	ON_WM_SETFOCUS()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_PBB_DU, OnPopupDU)
	ON_COMMAND(ID_PBB_UD, OnPopupUD)
	ON_COMMAND(ID_REFRESH_GRAPH, OnRefreshGraph)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ILGView drawing

void ILGView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();
 
	CSize sizeTotal;
	// TODO: calculate the total size of this view
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);
}

__declspec (dllimport) bool updateLabel;

void ILGView::OnDraw(CDC* pDC)
{
 int res;
	RECT rect;
	CSize sizeTotal;
	SCROLLINFO scrollInfo;
	ILGDoc* pDoc = dynamic_cast<ILGDoc*>(GetDocument());
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
 
	int xPos = GetScrollPos(SB_HORZ);
	int yPos = GetScrollPos(SB_VERT);
 // Get scroll bar positions.
 
 //ask everyone to draw themselves
 MyLinkedListIterator<BaseInstruction *> *iter=	pDoc->m_routine->instrListIteratorForw();
 while (iter->hasMoreElements()) 
 {
  iter->currentElement()->Draw(GNT_ILG,pDC);//,xPos,yPos);
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
// ILGView diagnostics

#ifdef _DEBUG
void ILGView::AssertValid() const
{
	CScrollView::AssertValid();
}

void ILGView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// ILGView message handlers


void ILGView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CScrollView::OnHScroll(nSBCode, nPos, pScrollBar);
 RedrawWindow();
}

void ILGView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CScrollView::OnVScroll(nSBCode, nPos, pScrollBar);
 RedrawWindow();
}

void ILGView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	ILGDoc* pDoc = dynamic_cast<ILGDoc*>(GetDocument());
	ASSERT_TRACE(pDoc,"Invalid GetDocument() return type");
 
 //go thru all instructions and find if the user clicked on any
 
 //adjust the point for scrolling
 CPoint	scrolledPoint(point.x+GetScrollPos(SB_HORZ),point.y+GetScrollPos(SB_VERT));
 
 //go thru all basicblocks/instructions and find if the user clicked on any
 MouseHitInfo* hit=pDoc->getHit(scrolledPoint);

 if (hit!=NULL)
 {
  hit->ProcessLeftClick(GNT_ILG,&viewSelection);
  //temp, use Draw() later
  RedrawWindow();
  //delete hit;
 }
	CScrollView::OnLButtonUp(nFlags, point);
}

void ILGView::OnPopupUDDU() 
{
	ILGDoc* pDoc = dynamic_cast<ILGDoc*>(GetDocument());
	ASSERT_TRACE(pDoc,"Invalid GetDocument() return type");
  
 //((GenericNode*)hit->node)->ToggleUDDU(GNT_ILG);
 ((GenericNode*)pDoc->hitILG.node)->ToggleUDDU(GNT_ILG); 
 pDoc->SetModifiedFlag();
 RedrawWindow();
 //delete hit;
}

void ILGView::OnPopupDU() 
{
	ILGDoc* pDoc = dynamic_cast<ILGDoc*>(GetDocument());
	ASSERT_TRACE(pDoc,"Invalid GetDocument() return type");
  
 //((GenericNode*)hit->node)->ToggleUDDU(GNT_ILG);
 ((GenericNode*)pDoc->hitILG.node)->ToggleDU(GNT_ILG); 
 pDoc->SetModifiedFlag();
 RedrawWindow();
 //delete hit;
}

void ILGView::OnPopupUD() 
{
	ILGDoc* pDoc = dynamic_cast<ILGDoc*>(GetDocument());
	ASSERT_TRACE(pDoc,"Invalid GetDocument() return type");
  
 //((GenericNode*)hit->node)->ToggleUDDU(GNT_ILG);
 ((GenericNode*)pDoc->hitILG.node)->ToggleUD(GNT_ILG); 
 pDoc->SetModifiedFlag();
 RedrawWindow();
 //delete hit;
}

void ILGView::OnRefreshGraph() 
{
	ILGDoc* pDoc = dynamic_cast<ILGDoc*>(GetDocument());
	ASSERT_TRACE(pDoc,"Invalid GetDocument() return type");
 pDoc->SetModifiedFlag();
 RedrawWindow();
}

void ILGView::OnPopupProperties() 
{
	//ILGDoc* pDoc = dynamic_cast<ILGDoc*>(GetDocument());
	//ASSERT_TRACE(pDoc,"Invalid GetDocument() return type");
 // 
 //((GenericNode*)hit->node)->ToggleExpanded();
 //pDoc->m_DocIsDirty=true;
 //RedrawWindow();
 //delete hit;
}

void ILGView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	ILGDoc* pDoc = dynamic_cast<ILGDoc*>(GetDocument());
	ASSERT_TRACE(pDoc,"Invalid GetDocument() return type");
  
 //adjust the point for scrolling
 CPoint	scrolledPoint(point.x+GetScrollPos(SB_HORZ),point.y+GetScrollPos(SB_VERT));
 
 //go thru all instructions and find if the user clicked on any
 hit=pDoc->getHit(scrolledPoint);
 if (hit!=NULL)
 {
  ClientToScreen(&point);
  hit->ProcessRightClick(GNT_ILG,point,this);
  //delete hit;
 }
 else
 {
  ClientToScreen(&point);
  CMenu menu;
  CMenu* pPopup;
  VERIFY(menu.LoadMenu(IDR_MENU_ILG));
  pPopup = menu.GetSubMenu(6);
  ASSERT(pPopup != NULL);
  VERIFY(pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, point.x, point.y, this));
 }

	CScrollView::OnRButtonUp(nFlags, point);
}

void ILGView::OnSetFocus(CWnd* pOldWnd) 
{
	CScrollView::OnSetFocus(pOldWnd);

 if (viewSelection==NULL)
	 theApp.propPageList->ShowPage(IDD_PROP_BASE, NULL);
 else
 {
  theApp.propPageList->ShowPage(IDD_PROP_INSTR, viewSelection);
 }
}

int ILGView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CScrollView::OnCreate(lpCreateStruct) == -1)
		return -1;

 //now add all the property pages that we need
 BasePropPage::bNoUpdatePos=true;

 InstrPropPage* instrPropPage= new InstrPropPage();
 instrPropPage->Create(instrPropPage->pageID,this);
 theApp.propPageList->AddPage(instrPropPage->pageID,instrPropPage);
	
 BasePropPage::bNoUpdatePos=false;

	return 0;
}

BOOL ILGView::OnEraseBkgnd(CDC* pDC) 
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
