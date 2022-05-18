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
// SIMGraphPipelineStage.cpp: implementation of the SIMGraphPipelineStage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../pcprogui.h"
#include "SIMGraphPipelineStage.h"

#include "../ColorCodes.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define PIPELINESTAGE_WIDTH 10
#define PIPELINESTAGE_HEIGHT 6

CWnd* SIMGraphPipelineStage::window=NULL;

IMPLEMENT_SERIAL( SIMGraphPipelineStage, SIMGraphObj, 1 )

SIMGraphPipelineStage::SIMGraphPipelineStage()
{
}

SIMGraphPipelineStage::SIMGraphPipelineStage(CWnd* pWindow, CRect* pRect, CPoint* point) : SIMGraphObj(NULL, pRect)
{
 borderColor=RGB(80,80,80);
 type=MHI_SIM_PIPELINESTAGE;
 yPos=point->y;
 //yPosScrolled=point->y+window->GetScrollPos(SB_VERT); //scroll the point
 window=pWindow;
 ComputeHandles();
}

SIMGraphPipelineStage::~SIMGraphPipelineStage()
{

}


void SIMGraphPipelineStage::genO(CFile* file, int level)
{
}

void SIMGraphPipelineStage::genC(CFile* file, int level)
{
}

void SIMGraphPipelineStage::Serialize(CArchive& ar)
{
 SIMGraphObj::Serialize(ar);

 if (ar.IsStoring())
 {
  ar << leftPoint << rightPoint << yPos;
 }
 else
 {
  ar >> leftPoint >> rightPoint >> yPos;
 }

}

void SIMGraphPipelineStage::OnDraw(CDC* pDC)
{
 //brush
 CBrush fullBrush;
 fullBrush.CreateSolidBrush(selected ? YELLOW : borderColor);
 CBrush* oldBrush=pDC->SelectObject(&fullBrush);
  
 //CPen* newPen = new CPen(PS_DASHDOT,1,selected ? YELLOW : borderColor);
 CPen* newPen = new CPen(PS_DOT,1,selected ? YELLOW : borderColor);
 CPen* oldPen = pDC->SelectObject(newPen);
 CPoint point=leftPoint;
 point.Offset(hScroll,0);
 pDC->MoveTo(point);
 point=rightPoint;
 point.Offset(hScroll,0);
 pDC->LineTo(point);
 pDC->SelectObject(oldPen);
 delete newPen;

 newPen = new CPen(PS_SOLID,1,selected ? YELLOW : borderColor);
 oldPen = pDC->SelectObject(newPen);

 rect.TopLeft()=leftPoint;
 rect.BottomRight()=leftPoint;
 rect.OffsetRect(hScroll,0);
 rect.InflateRect(0, PIPELINESTAGE_HEIGHT/2, PIPELINESTAGE_WIDTH, PIPELINESTAGE_HEIGHT/2);
 pDC->Rectangle(rect);

 rect.TopLeft()=rightPoint;
 rect.BottomRight()=rightPoint;
 rect.OffsetRect(hScroll,0);
 rect.InflateRect(PIPELINESTAGE_WIDTH, PIPELINESTAGE_HEIGHT/2, 0, PIPELINESTAGE_HEIGHT/2);
 pDC->Rectangle(rect);

 pDC->SelectObject(oldPen);
 pDC->SelectObject(oldBrush);  
 delete newPen;
}

void SIMGraphPipelineStage::OnDragDraw(CDC* pDC, CPoint& offset, SIMGraphObj* parent)
{
 //brush
 CBrush fullBrush;
 fullBrush.CreateSolidBrush(selected ? YELLOW : borderColor);
 CBrush* oldBrush=pDC->SelectObject(&fullBrush);

 CPoint newLeftPoint=leftPoint;
 CPoint newRightPoint=rightPoint;
 newLeftPoint.Offset(0, offset.y);
 newRightPoint.Offset(0, offset.y);
 
 //CPen* newPen = new CPen(PS_DASHDOT,1,selected ? YELLOW : borderColor);
 CPen* newPen = new CPen(PS_DOT,1,selected ? YELLOW : borderColor);
 CPen* oldPen = pDC->SelectObject(newPen);

 int lastROP2=pDC->SetROP2(R2_XORPEN);

 pDC->MoveTo(newLeftPoint);
 pDC->LineTo(newRightPoint);
 pDC->SelectObject(oldPen);
 delete newPen;

 newPen = new CPen(PS_SOLID,1,selected ? YELLOW : borderColor);
 oldPen = pDC->SelectObject(newPen);

 rect.TopLeft()=newLeftPoint;
 rect.BottomRight()=newLeftPoint;
 rect.InflateRect(0, PIPELINESTAGE_HEIGHT/2, PIPELINESTAGE_WIDTH, PIPELINESTAGE_HEIGHT/2);
 pDC->Rectangle(rect);

 rect.TopLeft()=newRightPoint;
 rect.BottomRight()=newRightPoint;
 rect.InflateRect(PIPELINESTAGE_WIDTH, PIPELINESTAGE_HEIGHT/2, 0, PIPELINESTAGE_HEIGHT/2);
 pDC->Rectangle(rect);

 pDC->SetROP2(lastROP2);

 pDC->SelectObject(oldPen);
 pDC->SelectObject(oldBrush);  
 delete newPen;
}

SIMGraphObj* SIMGraphPipelineStage::getHit(CPoint& point)
{
 rect.TopLeft()=leftPoint;
 rect.BottomRight()=leftPoint;
 rect.OffsetRect(hScroll,0);
 rect.InflateRect(0, PIPELINESTAGE_HEIGHT/2, PIPELINESTAGE_WIDTH, PIPELINESTAGE_HEIGHT/2);
 if (rect.PtInRect(point)) return this;

 rect.TopLeft()=rightPoint;
 rect.BottomRight()=rightPoint;
 rect.OffsetRect(hScroll,0);
 rect.InflateRect(PIPELINESTAGE_WIDTH, PIPELINESTAGE_HEIGHT/2, 0, PIPELINESTAGE_HEIGHT/2);
 if (rect.PtInRect(point)) return this;

 return NULL;
}

void SIMGraphPipelineStage::Offset(CPoint& point, SIMGraphObj* end)
{
 yPos+=point.y;
 leftPoint.Offset(0, point.y);
 rightPoint.Offset(0, point.y);
}


void SIMGraphPipelineStage::ComputeHandles()
{
 //given the yPos coordinate and the client rectangle of the simulor view compute the coords for the line handles

 //now compute where the handles should be
 CRect wRect;
 window->GetClientRect(&wRect);
 leftPoint=CPoint(window->GetScrollPos(SB_HORZ),yPos);
 rightPoint=CPoint(wRect.right+window->GetScrollPos(SB_HORZ),yPos);
}

void SIMGraphPipelineStage::AddPipelineStageObj(SIMGraphObj* obj)
{
 stageObjs.AddTail(obj);
}

void SIMGraphPipelineStage::SortPipelineStages(SIMGraphObjList* stages)
{
 //dumb sort
 SIMGraphObjList newList;

 while (!stages->IsEmpty())
 {
  POSITION pos=stages->GetHeadPosition();
  int minVal=((SIMGraphPipelineStage*)(stages->GetAt(pos)))->yPos;
  POSITION minPos=pos;
  while (pos!=NULL)
  {
   POSITION prevPos=pos;
   SIMGraphPipelineStage* stage=(SIMGraphPipelineStage*)stages->GetNext(pos);
   if (stage->yPos<=minVal) 
   {
    minPos=prevPos;
    minVal=stage->yPos;   
   }
  }

  //remove the minimum and add it to the newList
  SIMGraphObj* obj=stages->GetAt(minPos);
  stages->RemoveAt(minPos);
  newList.AddTail(obj);
 }

 //now copy newList back to stages
 ASSERT(stages->IsEmpty());
 POSITION pos=newList.GetHeadPosition();
 while (pos!=NULL)
 {
  SIMGraphPipelineStage* p;
  stages->AddTail(p=((SIMGraphPipelineStage*)newList.GetNext(pos)));
  //_CrtDbgReport(_CRT_WARN, __FILE__, __LINE__, "!", "Added stage with yPos = %d\n", p->yPos);
 }
}

void SIMGraphPipelineStage::ComputePipelineStageRects(SIMGraphObjList* stages)
{
 int i=0;
 SIMGraphPipelineStage* prevStage=NULL;
 POSITION pos=stages->GetHeadPosition();
 while (pos!=NULL)
 {
  SIMGraphPipelineStage* stage=(SIMGraphPipelineStage*)(stages->GetNext(pos));
  CRect* r=&stage->rect;
  if (i==0) //for the first rectangle use 0,0 as the upper left corner
  {
   r->TopLeft()=CPoint(0,0);
   r->BottomRight()=CPoint(DIAG_WIDTH,stage->yPos);
  }
  else
  {
   r->right=prevStage->rect.right;
   r->left=prevStage->rect.left;
   r->top=prevStage->rect.bottom+1;
   r->bottom=prevStage->rect.bottom+(stage->yPos-prevStage->yPos);
  }
  //_CrtDbgReport(_CRT_WARN, __FILE__, __LINE__, "!", "Stage rectangle: (t=%d, l=%d, b=%d, r=%d)\n", r->top, r->left, r->bottom, r->right);

  prevStage=stage;
  i++;
 }
}
