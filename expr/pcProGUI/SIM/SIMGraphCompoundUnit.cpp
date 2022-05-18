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
// SIMGraphCompoundUnit.cpp: implementation of the SIMGraphCompoundUnit class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../pcprogui.h"
#include "SIMGraphUnit.h"
#include "SIMGraphCompoundUnit.h"
#include "SIMGraphLatch.h"
#include "SIMGraphConnection.h"

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

IMPLEMENT_SERIAL( SIMGraphCompoundUnit, SIMGraphObj, 1 )

SIMGraphCompoundUnit::SIMGraphCompoundUnit() : SIMGraphObj()
{
}

SIMGraphCompoundUnit::SIMGraphCompoundUnit(SIMGraphObj* pParent, CRect* pRect) : SIMGraphObj(pParent, pRect)
{
 borderColor=DARKCYAN;
 type=MHI_SIM_COMPOUND_UNIT;
 parent=pParent;
 if (parent!=NULL) //fit inside parent
 {
  SIMGraphObj::FitRectangleInRectangle(&parent->rect,&rect);
 }
}

SIMGraphCompoundUnit::~SIMGraphCompoundUnit()
{
 POSITION pos=units.GetHeadPosition();
 while (pos!=NULL)
 {
  delete units.GetNext(pos);
 }

 if (SIMDoc::loadVersion>=100)
 {
  pos=latches.GetHeadPosition();
  while (pos!=NULL)
  {
   delete latches.GetNext(pos);
  }
  
  pos=__latches.GetHeadPosition();
  while (pos!=NULL)
  {
   delete __latches.GetNext(pos);
  }
 }

 if (parent!=NULL) //remove this unit from the units list of compound unit
 {
  parent->RemoveUnit(this);
 }
}

void SIMGraphCompoundUnit::OnDraw(CDC * pDC)
{
 //brush
 CBrush hollowBrush;
 hollowBrush.CreateStockObject(HOLLOW_BRUSH);
 CBrush* oldBrush=pDC->SelectObject(&hollowBrush);
 
 //text attributes
 pDC->SetTextColor(WHITE);
 pDC->SetBkMode(TRANSPARENT);
 
 //COLORREF c;
 //if (!breakpoint) { c=selected?YELLOW:borderColor;} else {c=RED;}
 //CPen* newPen = new CPen(PS_SOLID,2,c);
 //CPen* oldPen = pDC->SelectObject(newPen);
 //
 //pDC->Rectangle(&rect);
 //pDC->TextOut(rect.left+2,rect.top+2,label);
 //
 //pDC->SelectObject(oldPen);
 //delete newPen;
 //
 //pDC->SelectObject(oldBrush);  
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

 pDC->TextOut(rect.left+4,rect.top+4,name);
 pDC->SelectObject(oldPen);
 pDC->SelectObject(oldBrush);  
 delete newPen;

 POSITION pos=units.GetHeadPosition();
 while (pos!=NULL)
 {
  units.GetNext(pos)->OnDraw(pDC);
 }

 if (SIMDoc::loadVersion>=100)
 {
  pos=latches.GetHeadPosition();
  while (pos!=NULL)
  {
   latches.GetNext(pos)->OnDraw(pDC);
  }
  
  pos=__latches.GetHeadPosition();
  while (pos!=NULL)
  {
   __latches.GetNext(pos)->OnDraw(pDC);
  }
 }
}


void SIMGraphCompoundUnit::OnDragDraw(CDC* pDC, CPoint& offset, SIMGraphObj* parent)
{
 //setup the DC params
 
 //brush
 CBrush hollowBrush;
 hollowBrush.CreateStockObject(HOLLOW_BRUSH);
 CBrush* oldBrush=pDC->SelectObject(&hollowBrush);
 
 //COLORREF c;
 //if (!breakpoint) { c=selected?YELLOW:borderColor;} else {c=RED;}
 //CPen* newPen = new CPen(PS_SOLID,2,c);
 //CPen* oldPen = pDC->SelectObject(newPen);
 //
 //int lastROP2=pDC->SetROP2(R2_XORPEN);
 //CRect newRect=rect;
 //newRect.OffsetRect(offset);
 //pDC->Rectangle(&newRect);
 //pDC->SetROP2(lastROP2);
 //
 //pDC->SelectObject(oldPen);
 //delete newPen;
 //
 //pDC->SelectObject(oldBrush);  
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

 POSITION pos=units.GetHeadPosition();
 while (pos!=NULL)
 {
  units.GetNext(pos)->OnDragDraw(pDC,offset);
 }

 if (SIMDoc::loadVersion>=100)
 {
  pos=latches.GetHeadPosition();
  while (pos!=NULL)
  {
   latches.GetNext(pos)->OnDragDraw(pDC,offset,this);
  }
  
  pos=__latches.GetHeadPosition();
  while (pos!=NULL)
  {
   __latches.GetNext(pos)->OnDragDraw(pDC,offset,this);
  }
 }
}


SIMGraphObj* SIMGraphCompoundUnit::getHit(CPoint& point)
{
 //walk all the component lists and see if there's a hit
 //in order: connections, ports, storage, units

 //see if we have a hit in this unit
 if (!rect.PtInRect(point)) return NULL;

 //order matters
 POSITION pos=units.GetHeadPosition();
 while (pos!=NULL)
 {
  SIMGraphObj* hit=units.GetNext(pos)->getHit(point);  
  if (hit!=NULL) return hit;
 }

 if (SIMDoc::loadVersion>=100)
 {
  pos=__latches.GetHeadPosition();
  while (pos!=NULL)
  {
   SIMGraphObj* hit=__latches.GetNext(pos)->getHit(point);  
   if (hit!=NULL) return hit;
  }
  
  pos=latches.GetHeadPosition();
  while (pos!=NULL)
  {
   SIMGraphObj* hit=latches.GetNext(pos)->getHit(point);  
   if (hit!=NULL) return hit;
  }
 }

 return this;
}


SIMGraphObj* SIMGraphCompoundUnit::GetSIMGraphObjByName(CString* testName)
{
 if (name==*testName) return this;

 POSITION pos=units.GetHeadPosition();
 while (pos!=NULL)
 {
  SIMGraphObj* hit=units.GetNext(pos)->GetSIMGraphObjByName(testName);  
  if (hit!=NULL) return hit;
 }

 if (SIMDoc::loadVersion>=100)
 {
  pos=latches.GetHeadPosition();
  while (pos!=NULL)
  {
   SIMGraphObj* hit=latches.GetNext(pos)->GetSIMGraphObjByName(testName);
   if (hit!=NULL) return hit;
  }
 }

 return NULL;
}

void SIMGraphCompoundUnit::AddUnit(SIMGraphObj* obj)
{
 units.AddTail(obj);
}

void SIMGraphCompoundUnit::RemoveUnit(SIMGraphObj* obj)
{
 units.RemoveAt(units.Find(obj));
}

void SIMGraphCompoundUnit::AddLatch(SIMGraphObj* obj)
{
 latches.AddTail(obj);
}

void SIMGraphCompoundUnit::RemoveLatch(SIMGraphObj* obj)
{
 latches.RemoveAt(latches.Find(obj));
}

void SIMGraphCompoundUnit::__AddLatch(SIMGraphObj* obj)
{
 __latches.AddTail(obj);
}

void SIMGraphCompoundUnit::__RemoveLatch(SIMGraphObj* obj)
{
 __latches.RemoveAt(__latches.Find(obj));
}

void SIMGraphCompoundUnit::Offset(CPoint& point, SIMGraphObj* end)
{
 rect.OffsetRect(point);

 POSITION pos=units.GetHeadPosition();
 while (pos!=NULL)
 {
  units.GetNext(pos)->Offset(point);  
 }

 if (SIMDoc::loadVersion>=100)
 {
  pos=latches.GetHeadPosition();
  while (pos!=NULL)
  {
   latches.GetNext(pos)->Offset(point,this);  
  }
  
  pos=__latches.GetHeadPosition();
  while (pos!=NULL)
  {
   __latches.GetNext(pos)->Offset(point,this);
  }
 }
}

void SIMGraphCompoundUnit::genO(CFile* file, int level)
{
 CString spaces(_T(' '), level);
 CString out;

 out.Format("////  object properties for SIMGraphConnection %s  /////////////////////////////////////////////////////////\n\n"
     +spaces+"obj=new SIMGraphCompoundUnit();\n"
     +spaces+"tempList.AddTail(obj);\n\n",
     name);
 file->Write(out, out.GetLength());

 SIMGraphObj::genO(file, level);

 out.Format(
  spaces+"((SIMGraphCompoundUnit*)obj)->customProperties=\"%s\";\n",
  customProperties
  );
 file->Write(out, out.GetLength());

 out="\n";
 file->Write(out, out.GetLength());

 SIMDoc::genOSIMGraphObjList(units,file,level+1);
 if (SIMDoc::loadVersion>=100)
 {
  SIMDoc::genOSIMGraphObjList(latches,file,level+1);
 }
}

void SIMGraphCompoundUnit::genC(CFile* file, int level)
{
 CString spaces(_T(' '), level);
 CString out;

 out.Format("////  object lists for SIMGraphCompoundUnit %s  /////////////////////////////////////////////////////////\n\n",
     name);
 file->Write(out, out.GetLength());

 out.Format(spaces+"obj=FindSIMGraphObj(\"%s\");\n\n",name);
 file->Write(out, out.GetLength());

 SIMGraphObj::genC(file, level);

 //out.Format(spaces+"////  __latches for %s  //////////////////////////////////////////////////\n\n", name);
 //file->Write(out, out.GetLength());

 POSITION pos;

 if (SIMDoc::loadVersion>=100)
 {
  pos=__latches.GetHeadPosition();
  while (pos!=NULL)
  {
   out.Format(spaces+"((SIMGraphUnit*)obj)->__AddLatch(FindSIMGraphObj(\"%s\"));\n", __latches.GetAt(pos)->name);
   file->Write(out, out.GetLength());
   __latches.GetNext(pos);
  }
  
  if (!__latches.IsEmpty())
  {
   out="\n";
   file->Write(out, out.GetLength());
  }
 }

 pos=units.GetHeadPosition();
 while (pos!=NULL)
 {
  out.Format(spaces+"((SIMGraphCompoundUnit*)obj)->AddUnit(FindSIMGraphObj(\"%s\"));\n", units.GetAt(pos)->name);
  file->Write(out, out.GetLength());
  units.GetNext(pos);
 }

 if (!units.IsEmpty())
 {
  out="\n";
  file->Write(out, out.GetLength());
 }

 if (SIMDoc::loadVersion>=100)
 {
  //SIMDoc::genOSIMGraphObjList(latches,file,level+1);
  pos=latches.GetHeadPosition();
  while (pos!=NULL)
  {
   out.Format(spaces+"((SIMGraphUnit*)obj)->AddLatch(FindSIMGraphObj(\"%s\"));\n", latches.GetAt(pos)->name);
   file->Write(out, out.GetLength());
   latches.GetNext(pos);
  }
  
  if (!latches.IsEmpty())
  {
   out="\n";
   file->Write(out, out.GetLength());
  }
 }

 //now recurse

 pos=units.GetHeadPosition();
 while (pos!=NULL)
 {
  units.GetNext(pos)->genC(file,level+1);
 }

 if (SIMDoc::loadVersion>=100)
 {
  pos=latches.GetHeadPosition();
  while (pos!=NULL)
  {
   latches.GetNext(pos)->genC(file,level+1);
  }
 }

 out="\n";
 file->Write(out, out.GetLength());
}

void SIMGraphCompoundUnit::Serialize(CArchive& ar)
{
 SIMGraphObj::Serialize(ar);

 SIMDoc::SerializeSIMGraphObjList(units,ar);

 if (SIMDoc::loadVersion>100)
 {
  __latches.Serialize(ar);
  SIMDoc::SerializeSIMGraphObjList(latches,ar);  
 }
}


void SIMGraphCompoundUnit::PatchPointers()
{
 SIMGraphObj::PatchPointers();

 POSITION pos=units.GetHeadPosition();
 while (pos!=NULL)
 {
  units.GetNext(pos)->PatchPointers();
 }

 if (SIMDoc::loadVersion>=100)
 {
  pos=latches.GetHeadPosition();
  while (pos!=NULL)
  {
   latches.GetNext(pos)->PatchPointers();
  }
  
  pos=__latches.GetHeadPosition();
  while (pos!=NULL)
  {
   SIMGraphObj* obj=__latches.GetAt(pos);
   __latches.SetAt(pos, SIMGraphObj::getPtrMapping(__latches.GetAt(pos)));
   obj=__latches.GetAt(pos);
   __latches.GetNext(pos);
  }
 }

}

void SIMGraphCompoundUnit::AddSubtype(StringList* pCompoundunits, StringList* pUnits, StringList* pPorts, StringList* pConnections, StringList* pStorages, StringList* pLatches)
{
 //add subtypes from components first
 POSITION pos=units.GetHeadPosition();
 while (pos!=NULL)
 {
  units.GetNext(pos)->AddSubtype(pCompoundunits, pUnits, pPorts, pConnections, pStorages, pLatches);
 }

 if (SIMDoc::loadVersion>=100)
 {
  pos=latches.GetHeadPosition();
  while (pos!=NULL)
  {
   latches.GetNext(pos)->AddSubtype(pCompoundunits, pUnits, pPorts, pConnections, pStorages, pLatches);
  }
 }

 if (classname!="")
 {
  pos=pCompoundunits->GetHeadPosition();
  while (pos!=NULL)
  {
   if (pCompoundunits->GetNext(pos)==classname) return; //already added
  }
  pCompoundunits->AddTail(classname); 
 } 
}


void SIMGraphCompoundUnit::xArch(CFile* file, int level)
{
 CString spaces(_T(' '), level);
 CString out;

 //out+=spaces+"(COMPOUND " + name + "()\n";
 if (classname=="")
 {
  out=spaces+"(COMPOUNDUNIT " + name + "\n";
 }
 else
 {
  out=spaces+"(" +classname + " " + name + "\n";
 }

 file->Write(out, out.GetLength());
 out=spaces+" (SUBCOMPONENTS ";

 //write components
 POSITION pos=units.GetHeadPosition();
 while (pos!=NULL)
 {
  out+=" "+units.GetNext(pos)->name;  
 }

 out+=")\n" + spaces + ")\n\n";
 file->Write(out, out.GetLength());

 if (SIMDoc::loadVersion>=100)
 {
  //write latches as lists
  if (!latches.IsEmpty())
  {
   out=spaces+" (LATCHES";
   POSITION pos=latches.GetHeadPosition();
   while (pos!=NULL)
   {
    SIMGraphLatch* latch=(SIMGraphLatch*)latches.GetNext(pos);
    switch (latch->latchtype)
    {
    case 1:
     out+=" (IN "+latch->name + ")";  
     break;
     
    case 2:
     out+=" (OUT "+latch->name + ")";  
     break;
     
    case 3:
     out+=" (OTHER "+latch->name + ")";  
     break;
     
    default:
     //ASSERT(0);
     out+=" (UNKNOWN "+latch->name + ")";  
     break;
    }
    //out+=" "+latches.GetNext(pos)->name;  
   }
   out+=")\n";
   file->Write(out, out.GetLength());
  }
  
  //write _latches as lists
  if (!__latches.IsEmpty())
  {
   out=spaces+" (LATCHES";
   POSITION pos=__latches.GetHeadPosition();
   while (pos!=NULL)
   {
    SIMGraphLatch* latch=(SIMGraphLatch*)(((SIMGraphConnection*)(__latches.GetNext(pos)))->startObj);
    switch (latch->latchtype)
    {
    case 1:
     out+=" (OUT "+latch->name + ")";  //these are reversed to account for the fact that the 
     break;
     
    case 2:
     out+=" (IN "+latch->name + ")";  
     break;
     
    case 3:
     out+=" (OTHER "+latch->name + ")";  
     break;
     
    default:
     //ASSERT(0);
     out+=" (UNKNOWN "+latch->name + ")";  
     break;
    }
    //out+=" "+((SIMGraphConnection*)(__latches.GetNext(pos)))->startObj->name;  
   }
   out+=")\n";
   file->Write(out, out.GetLength());
  }
 }
 
 //write components as objects
 pos=units.GetHeadPosition();
 while (pos!=NULL)
 {
  units.GetNext(pos)->xArch(file,level+1);  
 }

 if (SIMDoc::loadVersion>=100)
 {
  POSITION pos=latches.GetHeadPosition();
  while (pos!=NULL)
  {
   latches.GetNext(pos)->xArch(file,level+1);  
  }
 }

}


void SIMGraphCompoundUnit::xPipe(CFile* file, SIMGraphObj* obj)
{
 POSITION pos=units.GetHeadPosition();
 while (pos!=NULL)
 {
  units.GetNext(pos)->xPipe(file,obj);  
 }
}

