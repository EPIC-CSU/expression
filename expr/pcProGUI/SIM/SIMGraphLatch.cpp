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
// SIMGraphLatch.cpp: implementation of the SIMGraphLatch class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../pcprogui.h"
#include "SIMGraphLatch.h"
//#include "SIMGraphUnit.h"
#include "SIMDoc.h"

#include "../ColorCodes.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_SERIAL( SIMGraphLatch, SIMGraphObj, 1 )

SIMGraphLatch::SIMGraphLatch() : SIMGraphObj()
{

}

SIMGraphLatch::SIMGraphLatch(SIMGraphObj* pParent, CRect* pRect, SIMGraphObj* pOtherEnd) : SIMGraphObj(pParent, pRect)
{
 //ASSERT(pParent!=NULL); //restore when latches can only be part of a unit

 borderColor=CAFFEAULAIT;
 type=MHI_SIM_LATCH;
 latchtype=1;
 otherEnd=pOtherEnd;
 if (parent!=NULL)
  SIMGraphObj::FitRectangleInRectangle(&parent->rect,&rect);
}


SIMGraphLatch::~SIMGraphLatch()
{
 POSITION pos=connections.GetHeadPosition();
 while (pos!=NULL)
 {
  delete connections.GetNext(pos);
 }

 if (parent!=NULL)
  parent->RemoveLatch(this);
}

void SIMGraphLatch::genO(CFile* file, int level)
{
 CString spaces(_T(' '), level);
 CString out;

 out.Format("////  object properties for SIMGraphLatch %s  /////////////////////////////////////////////////////////\n\n"
     +spaces+"obj=new SIMGraphLatch();\n"
     +spaces+"tempList.AddTail(obj);\n\n",
     name);
 file->Write(out, out.GetLength());

 SIMGraphObj::genO(file, level);

 out.Format(
  spaces+"((SIMGraphLatch*)obj)->customProperties=\"%s\";\n"
  +spaces+"((SIMGraphLatch*)obj)->latchtype=%d;\n",
  customProperties,
  latchtype
  );
 file->Write(out, out.GetLength());

 out="\n";
 file->Write(out, out.GetLength());

 SIMDoc::genOSIMGraphObjList(connections,file,level+1);
}

void SIMGraphLatch::genC(CFile* file, int level)
{
 CString spaces(_T(' '), level);
 CString out;

 out.Format("////  object lists for SIMGraphLatch %s  /////////////////////////////////////////////////////////\n\n",
     name);
 file->Write(out, out.GetLength());

 out.Format(spaces+"obj=FindSIMGraphObj(\"%s\");\n\n",name);
 file->Write(out, out.GetLength());

 SIMGraphObj::genC(file, level);

 //out.Format(spaces+"////  __latches for %s  //////////////////////////////////////////////////\n\n", name);
 //file->Write(out, out.GetLength());

 //otherEnd is set here...
 if (otherEnd!=NULL)
 {
  out.Format(spaces+"((SIMGraphLatch*)obj)->otherEnd=FindSIMGraphObj(\"%s\");\n\n",otherEnd->name);
  file->Write(out, out.GetLength());
 }

 //SIMDoc::genOSIMGraphObjList(connections,file,level+1);
 POSITION pos=connections.GetHeadPosition();
 while (pos!=NULL)
 {
  out.Format(spaces+"((SIMGraphLatch*)obj)->AddConnection(FindSIMGraphObj(\"%s\"));\n", connections.GetAt(pos)->name);
  file->Write(out, out.GetLength());
  connections.GetNext(pos);
 }

 if (!connections.IsEmpty())
 {
  out="\n";
  file->Write(out, out.GetLength());
 }

 //now recurse

 pos=connections.GetHeadPosition();
 while (pos!=NULL)
 {
  connections.GetNext(pos)->genC(file,level+1);
 }

 out="\n";
 file->Write(out, out.GetLength());
}

void SIMGraphLatch::Serialize(CArchive& ar)
{
 SIMGraphObj::Serialize(ar);

 long lOtherEnd;
	if (ar.IsStoring())
	{
  lOtherEnd=(long)otherEnd;
  ar << latchtype << customProperties << lOtherEnd;
 }
	else
	{
  ar >> latchtype;
  ar >> customProperties >> lOtherEnd;
  otherEnd=(SIMGraphObj*)lOtherEnd;
	}

 SIMDoc::SerializeSIMGraphObjList(connections,ar);
}



void SIMGraphLatch::OnDraw(CDC* pDC)
{
 //setup the DC params
 
 //brush
 CBrush hollowBrush;
 hollowBrush.CreateStockObject(HOLLOW_BRUSH);
 CBrush* oldBrush=pDC->SelectObject(&hollowBrush);
 
 //text attributes
 //pDC->SetTextColor(WHITE);
 //pDC->SetBkMode(TRANSPARENT);
 
 CPen* newPen;
 CPen* oldPen;
 if (breakpoint) 
 {
  CRect newRect=rect;
  newPen = new CPen(PS_SOLID,2,selected ? YELLOW : borderColor);
  oldPen = pDC->SelectObject(newPen);  
  pDC->Rectangle(&newRect);
  pDC->SelectObject(oldPen);
  delete newPen;
  newPen = new CPen(PS_SOLID,1,RED);
  oldPen = pDC->SelectObject(newPen);  
  newRect.InflateRect(-2,-2);
  pDC->Rectangle(&newRect);
 }
 else
 {
  newPen = new CPen(PS_SOLID,2,selected ? YELLOW : borderColor);
  oldPen = pDC->SelectObject(newPen);  
  pDC->Rectangle(&rect);
 }

 //pDC->TextOut(rect.left+4,rect.top+4,name);

 pDC->SelectObject(oldPen);
 pDC->SelectObject(oldBrush);  
 delete newPen;

 POSITION pos=connections.GetHeadPosition();
 while (pos!=NULL)
 {
  connections.GetNext(pos)->OnDraw(pDC);
 }
}

void SIMGraphLatch::OnDragDraw(CDC* pDC, CPoint& offset, SIMGraphObj* parent)
{
 //setup the DC params
 
 //brush
 CBrush hollowBrush;
 hollowBrush.CreateStockObject(HOLLOW_BRUSH);
 CBrush* oldBrush=pDC->SelectObject(&hollowBrush);
 
 CPen* newPen;
 CPen* oldPen;
 int lastROP2=pDC->SetROP2(R2_XORPEN);
 CRect newRect=rect;
 newRect.OffsetRect(offset);
 if (breakpoint) 
 {
  newPen = new CPen(PS_SOLID,2,selected ? YELLOW : borderColor);
  oldPen = pDC->SelectObject(newPen);  
  pDC->Rectangle(&newRect);
  pDC->SelectObject(oldPen);
  delete newPen;
  newPen = new CPen(PS_SOLID,1,RED);
  oldPen = pDC->SelectObject(newPen);  
  newRect.InflateRect(-2,-2);
  pDC->Rectangle(&newRect);
 }
 else
 {
  newPen = new CPen(PS_SOLID,2,selected ? YELLOW : borderColor);
  oldPen = pDC->SelectObject(newPen);  
  pDC->Rectangle(&newRect);
 }

 pDC->SetROP2(lastROP2);
 pDC->SelectObject(oldPen);
 pDC->SelectObject(oldBrush);  
 delete newPen;

 POSITION pos=connections.GetHeadPosition();
 while (pos!=NULL)
 {
  connections.GetNext(pos)->OnDragDraw(pDC,offset,this);
 }
}

SIMGraphObj* SIMGraphLatch::getHit(CPoint& point)
{
 if (!rect.PtInRect(point)) return NULL;

 //order matters
 POSITION pos=connections.GetHeadPosition();
 while (pos!=NULL)
 {
  SIMGraphObj* hit=connections.GetNext(pos)->getHit(point);  
  if (hit!=NULL) return hit;
 }

 return this;
}


SIMGraphObj* SIMGraphLatch::GetSIMGraphObjByName(CString * testName)
{
 if (name==*testName) return this;

 POSITION pos=connections.GetHeadPosition();
 while (pos!=NULL)
 {
  SIMGraphObj* hit=connections.GetNext(pos)->GetSIMGraphObjByName(testName);  
  if (hit!=NULL) return hit;
 }

 return NULL;
}


void SIMGraphLatch::Offset(CPoint& point, SIMGraphObj* end)
{
 rect.OffsetRect(point);

 POSITION pos=connections.GetHeadPosition();
 while (pos!=NULL)
 {
  connections.GetNext(pos)->Offset(point,this);  
 }

}

void SIMGraphLatch::PatchPointers()
{
 SIMGraphObj::PatchPointers();

 POSITION pos=connections.GetHeadPosition();
 while (pos!=NULL)
 {
  connections.GetNext(pos)->PatchPointers();
 }

 otherEnd=SIMGraphObj::getPtrMapping(otherEnd);
}

void SIMGraphLatch::AddSubtype(StringList* pCompoundunits, StringList* pUnits, StringList* pPorts, StringList* pConnections, StringList* pStorages, StringList* pLatches)
{
 //add subtypes from components first
 POSITION pos=connections.GetHeadPosition();
 while (pos!=NULL)
 {
  connections.GetNext(pos)->AddSubtype(pCompoundunits, pUnits, pPorts, pConnections, pStorages, pLatches);
 }

 if (classname!="")
 {
  pos=pLatches->GetHeadPosition();
  while (pos!=NULL)
  {
   if (pLatches->GetNext(pos)==classname) return; //already added
  }
  pLatches->AddTail(classname); 
 } 
}

void SIMGraphLatch::AddConnection(SIMGraphObj* obj)
{
 connections.AddTail(obj);
}

void SIMGraphLatch::RemoveConnection(SIMGraphObj* obj)
{
 connections.RemoveAt(connections.Find(obj));
}

void SIMGraphLatch::xArch(CFile* file, int level)
{
 CString spaces(_T(' '), level);
 CString out;

 if (classname=="")
 {
  out=spaces+"(Latch "+name+"\n";
 }
 else
 {
  out=spaces+"(" +classname+ " " +name+"\n";
 }
 file->Write(out, out.GetLength());

 //write properties
 //TODO indent customProperties
 if (customProperties!="") 
 {
  out=spaces+customProperties;
  file->Write(out, out.GetLength());
 }

// //write connections as lists
// POSITION pos=connections.GetHeadPosition();
// while (pos!=NULL)
// {
//  out+=" "+connections.GetNext(pos)->name;  
// }

 out=spaces+")\n\n";
 file->Write(out, out.GetLength());

 //don't write connections for a latch (they're just used to pinpoint the units that are connected by the latch)
}


