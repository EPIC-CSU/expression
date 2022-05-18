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
// SIMGraphPort.cpp: implementation of the SIMGraphPort class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../pcprogui.h"
#include "SIMGraphPort.h"
//#include "SIMGraphUnit.h"
//#include "SIMGraphStorage.h"
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

IMPLEMENT_SERIAL( SIMGraphPort, SIMGraphObj, 1 )

SIMGraphPort::SIMGraphPort() : SIMGraphObj()
{
}

SIMGraphPort::SIMGraphPort(SIMGraphObj* pParent, CRect* pRect, SIMGraphObj* pOwner) : SIMGraphObj(pParent, pRect)
{
 ASSERT(pParent!=NULL);

 borderColor=ORANGE;
 porttype=1;
 type=MHI_SIM_PORT;
 parent=pParent;
 SIMGraphObj::FitRectangleInRectangle(&parent->rect,&rect);
}

SIMGraphPort::~SIMGraphPort()
{
 POSITION pos=connections.GetHeadPosition();
 while (pos!=NULL)
 {
  delete connections.GetNext(pos);
 }

 //remove this port from the ports list of parent
 parent->RemovePort(this);
}

void SIMGraphPort::OnDraw(CDC * pDC)
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

 pos=_connections.GetHeadPosition();
 while (pos!=NULL)
 {
  _connections.GetNext(pos)->OnDraw(pDC);
 }
}


void SIMGraphPort::OnDragDraw(CDC* pDC, CPoint& offset, SIMGraphObj* parent)
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

 pos=_connections.GetHeadPosition();
 while (pos!=NULL)
 {
  _connections.GetNext(pos)->OnDragDraw(pDC,offset,this);
 }
}

SIMGraphObj* SIMGraphPort::getHit(CPoint& point)
{
 //see if we have a hit in this unit
 if (!rect.PtInRect(point)) return NULL;

 //order matters
 POSITION pos=connections.GetHeadPosition();
 while (pos!=NULL)
 {
  SIMGraphObj* hit=connections.GetNext(pos)->getHit(point);  
  if (hit!=NULL) return hit;
 }

 pos=_connections.GetHeadPosition();
 while (pos!=NULL)
 {
  SIMGraphObj* hit=_connections.GetNext(pos)->getHit(point);  
  if (hit!=NULL) return hit;
 }

 return this;
}


SIMGraphObj* SIMGraphPort::GetSIMGraphObjByName(CString * testName)
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

void SIMGraphPort::AddConnection(SIMGraphObj* obj)
{
 connections.AddTail(obj);
}

void SIMGraphPort::RemoveConnection(SIMGraphObj* obj)
{
 connections.RemoveAt(connections.Find(obj));
}

void SIMGraphPort::_AddConnection(SIMGraphObj* obj)
{
 _connections.AddTail(obj);
}

void SIMGraphPort::Offset(CPoint& point, SIMGraphObj* end)
{
 rect.OffsetRect(point);

 POSITION pos=connections.GetHeadPosition();
 while (pos!=NULL)
 {
  connections.GetNext(pos)->Offset(point,this);
 }

 pos=_connections.GetHeadPosition();
 while (pos!=NULL)
 {
  _connections.GetNext(pos)->Offset(point,this);
 }
}

void SIMGraphPort::genO(CFile* file, int level)
{
 CString spaces(_T(' '), level);
 CString out;

 out.Format("////  object properties for SIMGraphPort %s  /////////////////////////////////////////////////////////\n\n"
     +spaces+"obj=new SIMGraphPort();\n"
     +spaces+"tempList.AddTail(obj);\n\n",
     name);
 file->Write(out, out.GetLength());

 SIMGraphObj::genO(file, level);

 out.Format(
  spaces+"((SIMGraphPort*)obj)->customProperties=\"%s\";\n"
  +spaces+"((SIMGraphPort*)obj)->porttype=%d;\n",
  customProperties,
  porttype
  );
 file->Write(out, out.GetLength());

 out="\n";
 file->Write(out, out.GetLength());

 SIMDoc::genOSIMGraphObjList(connections,file,level+1);
}

void SIMGraphPort::genC(CFile* file, int level)
{
 CString spaces(_T(' '), level);
 CString out;

 out.Format("////  object lists for SIMGraphPort %s  /////////////////////////////////////////////////////////\n\n",
     name);
 file->Write(out, out.GetLength());

 out.Format(spaces+"obj=FindSIMGraphObj(\"%s\");\n\n",name);
 file->Write(out, out.GetLength());

 SIMGraphObj::genC(file, level);

 //out.Format(spaces+"////  __latches for %s  //////////////////////////////////////////////////\n\n", name);
 //file->Write(out, out.GetLength());

 //SIMDoc::genOSIMGraphObjList(connections,file,level+1);
 POSITION pos=connections.GetHeadPosition();
 while (pos!=NULL)
 {
  out.Format(spaces+"((SIMGraphPort*)obj)->AddConnection(FindSIMGraphObj(\"%s\"));\n", connections.GetAt(pos)->name);
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

void SIMGraphPort::Serialize(CArchive& ar)
{
 SIMGraphObj::Serialize(ar);

 if (ar.IsStoring())
 {
  ar << porttype << customProperties;
 }
 else
 {
  ar >> porttype >> customProperties;
 }

 SIMDoc::SerializeSIMGraphObjList(connections,ar);
}


void SIMGraphPort::PatchPointers()
{
 SIMGraphObj::PatchPointers();

 POSITION pos=connections.GetHeadPosition();
 while (pos!=NULL)
 {
  connections.GetNext(pos)->PatchPointers();
 }

}

void SIMGraphPort::AddSubtype(StringList* pCompoundunits, StringList* pUnits, StringList* pPorts, StringList* pConnections, StringList* pStorages, StringList* pLatches)
{
 //add subtypes from components first
 POSITION pos=connections.GetHeadPosition();
 while (pos!=NULL)
 {
  connections.GetNext(pos)->AddSubtype(pCompoundunits, pUnits, pPorts, pConnections, pStorages, pLatches);
 }

 if (classname!="")
 {
  pos=pPorts->GetHeadPosition();
  while (pos!=NULL)
  {
   if (pPorts->GetNext(pos)==classname) return; //already added
  }
  pPorts->AddTail(classname); 
 } 
}

void SIMGraphPort::xArch(CFile* file, int level)
{
 CString spaces(_T(' '), level);
 CString out;

 if (classname=="")
 {
  out=spaces+"(Port " + name + "(\"";
 }
 else
 {
  out=spaces+"(" +classname + " " + name + "(\"";
 }
 file->Write(out, out.GetLength());

 //now write the type
 switch (porttype)
 {
  case 1: out="_READ_"; break;
  case 2: out="_WRITE_"; break;
  case 3: out="_READWRITE_"; break;
 }

 out+="\")\n";
 file->Write(out, out.GetLength());

 //TODO indent customProperties
 if (customProperties!="") 
 {
  out=spaces+customProperties;
  file->Write(out, out.GetLength());
 }

 //anything else?

 out=spaces+")\n\n";
 file->Write(out, out.GetLength());

 //write components as objects
 POSITION pos=connections.GetHeadPosition();
 while (pos!=NULL)
 {
  connections.GetNext(pos)->xArch(file,level+1);  
 }
}


