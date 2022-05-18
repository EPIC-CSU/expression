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
// SIMGraphBus.cpp: implementation of the SIMGraphBus class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\pcprogui.h"

#include "SIMDoc.h"

#include "SIMGraphBus.h"
#include "../ColorCodes.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_SERIAL( SIMGraphBus, SIMGraphObj, 1 )

SIMGraphBus::SIMGraphBus()
{

}

SIMGraphBus::SIMGraphBus(SIMGraphObj* pParent, CRect* pRect) : SIMGraphObj(pParent, pRect)
{
 borderColor=BROWN;
 type=MHI_SIM_BUS;
 parent=pParent;

 if (parent!=NULL) //fit inside parent
 {
  SIMGraphObj::FitRectangleInRectangle(&parent->rect,&rect);
 }
}

SIMGraphBus::~SIMGraphBus()
{
 POSITION pos=connections.GetHeadPosition();
 while (pos!=NULL)
 {
  delete connections.GetNext(pos);
 }

 pos=ports.GetHeadPosition();
 while (pos!=NULL)
 {
  delete ports.GetNext(pos);
 } 
}

void SIMGraphBus::OnDraw(CDC * pDC)
{
 //setup the DC params
 
 //brush
 CBrush hollowBrush;
 hollowBrush.CreateStockObject(HOLLOW_BRUSH);
 CBrush* oldBrush=pDC->SelectObject(&hollowBrush);
 
 //text attributes
 pDC->SetTextColor(WHITE);
 pDC->SetBkMode(TRANSPARENT);
 
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

 POSITION pos=ports.GetHeadPosition();
 while (pos!=NULL)
 {
  ports.GetNext(pos)->OnDraw(pDC);
 }

 pos=connections.GetHeadPosition();
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


void SIMGraphBus::OnDragDraw(CDC* pDC, CPoint& offset, SIMGraphObj* parent)
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

 POSITION pos=ports.GetHeadPosition();
 while (pos!=NULL)
 {
  ports.GetNext(pos)->OnDragDraw(pDC,offset);
 }

 pos=connections.GetHeadPosition();
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


SIMGraphObj* SIMGraphBus::getHit(CPoint& point)
{
 //walk all the component lists and see if there's a hit
 //in order: connections, ports, storage, units

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

 pos=ports.GetHeadPosition();
 while (pos!=NULL)
 {
  SIMGraphObj* hit=ports.GetNext(pos)->getHit(point);  
  if (hit!=NULL) return hit;
 }

 return this;
}



SIMGraphObj* SIMGraphBus::GetSIMGraphObjByName(CString * testName)
{
 if (name==*testName) return NULL;

 POSITION pos=connections.GetHeadPosition();
 while (pos!=NULL)
 {
  SIMGraphObj* hit=connections.GetNext(pos)->GetSIMGraphObjByName(testName);
  if (hit!=NULL) return hit;
 }

 pos=ports.GetHeadPosition();
 while (pos!=NULL)
 {
  SIMGraphObj* hit=ports.GetNext(pos)->GetSIMGraphObjByName(testName);
  if (hit!=NULL) return hit;
 }

 return NULL;
}

void SIMGraphBus::AddConnection(SIMGraphObj* obj)
{
 connections.AddTail(obj);
}

void SIMGraphBus::RemoveConnection(SIMGraphObj* obj)
{
 connections.RemoveAt(connections.Find(obj));
}

void SIMGraphBus::_AddConnection(SIMGraphObj* obj)
{
 _connections.AddTail(obj);
}

void SIMGraphBus::AddPort(SIMGraphObj* obj)
{
 ports.AddTail(obj);
}

void SIMGraphBus::RemovePort(SIMGraphObj* obj)
{
 ports.RemoveAt(ports.Find(obj));
}

void SIMGraphBus::Offset(CPoint& point, SIMGraphObj* end)
{
 rect.OffsetRect(point);

 POSITION pos=ports.GetHeadPosition();
 while (pos!=NULL)
 {
  ports.GetNext(pos)->Offset(point);  
 }

 pos=connections.GetHeadPosition();
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


void SIMGraphBus::genO(CFile* file, int level)
{
 CString spaces(_T(' '), level);
 CString out;

 out.Format("////  object properties for SIMGraphConnection %s  /////////////////////////////////////////////////////////\n\n"
     +spaces+"obj=new SIMGraphBus();\n"
     +spaces+"tempList.AddTail(obj);\n\n",
     name);
 file->Write(out, out.GetLength());

 SIMGraphObj::genO(file, level);

 out.Format(
  spaces+"((SIMGraphConnection*)obj)->customProperties=\"%s\";\n",
  customProperties
  );
 file->Write(out, out.GetLength());

 out="\n";
 file->Write(out, out.GetLength());

 SIMDoc::genOSIMGraphObjList(ports,file,level+1);
 SIMDoc::genOSIMGraphObjList(connections,file,level+1);
}

void SIMGraphBus::genC(CFile* file, int level)
{
 CString spaces(_T(' '), level);
 CString out;

 out.Format("////  object lists for SIMGraphBus %s  /////////////////////////////////////////////////////////\n\n",
     name);
 file->Write(out, out.GetLength());

 out.Format(spaces+"obj=FindSIMGraphObj(\"%s\");\n\n",name);
 file->Write(out, out.GetLength());

 SIMGraphObj::genC(file, level);

 //out.Format(spaces+"////  __latches for %s  //////////////////////////////////////////////////\n\n", name);
 //file->Write(out, out.GetLength());

 //SIMDoc::genOSIMGraphObjList(ports,file,level+1);
 POSITION pos=ports.GetHeadPosition();
 while (pos!=NULL)
 {
  out.Format(spaces+"((SIMGraphBus*)obj)->AddPort(FindSIMGraphObj(\"%s\"));\n", ports.GetAt(pos)->name);
  file->Write(out, out.GetLength());
  ports.GetNext(pos);
 }

 if (!ports.IsEmpty())
 {
  out="\n";
  file->Write(out, out.GetLength());
 }

 //SIMDoc::genOSIMGraphObjList(connections,file,level+1);
 pos=connections.GetHeadPosition();
 while (pos!=NULL)
 {
  out.Format(spaces+"((SIMGraphBus*)obj)->AddConnection(FindSIMGraphObj(\"%s\"));\n", connections.GetAt(pos)->name);
  file->Write(out, out.GetLength());
  connections.GetNext(pos);
 }

 if (!connections.IsEmpty())
 {
  out="\n";
  file->Write(out, out.GetLength());
 }

 //now recurse

 pos=ports.GetHeadPosition();
 while (pos!=NULL)
 {
  ports.GetNext(pos)->genC(file,level+1);
 }

 pos=connections.GetHeadPosition();
 while (pos!=NULL)
 {
  connections.GetNext(pos)->genC(file,level+1);
 }

 out="\n";
 file->Write(out, out.GetLength());
}

void SIMGraphBus::Serialize(CArchive& ar)
{
 SIMGraphObj::Serialize(ar);

	if (ar.IsStoring())
	{
  ar << customProperties;
 }
	else
	{
  ar >> customProperties;
	}

 SIMDoc::SerializeSIMGraphObjList(ports,ar);
 SIMDoc::SerializeSIMGraphObjList(connections,ar);
}
 
void SIMGraphBus::PatchPointers()
{
 SIMGraphObj::PatchPointers();

 POSITION pos=ports.GetHeadPosition();
 while (pos!=NULL)
 {
  ports.GetNext(pos)->PatchPointers();
 } 

 pos=connections.GetHeadPosition();
 while (pos!=NULL)
 {
  connections.GetNext(pos)->PatchPointers();
 }
}


void SIMGraphBus::AddSubtype(StringList* pCompoundunits, StringList* pUnits, StringList* pPorts, StringList* pConnections, StringList* pStorages, StringList* pLatches)
{
 //add subtypes from components first
 POSITION pos=ports.GetHeadPosition();
 while (pos!=NULL)
 {
  ports.GetNext(pos)->AddSubtype(pCompoundunits, pUnits, pPorts, pConnections, pStorages, pLatches);
 }

 pos=connections.GetHeadPosition();
 while (pos!=NULL)
 {
  connections.GetNext(pos)->AddSubtype(pCompoundunits, pUnits, pPorts, pConnections, pStorages, pLatches);
 }

 if (classname!="")
 {
  pos=pUnits->GetHeadPosition();
  while (pos!=NULL)
  {
   if (pUnits->GetNext(pos)==classname) return; //already added
  }
  pUnits->AddTail(classname); 
 } 
}


void SIMGraphBus::xArch(CFile* file, int level)
{
 CString spaces(_T(' '), level);
 CString out;

 //TEMP
 return;

 if (classname=="")
 {
  out=spaces+"(Connection "+name+"\n";
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

 //write connections as lists
 if (!connections.IsEmpty())
 {
  out=spaces+" (CONNECTIONS";
  POSITION pos=connections.GetHeadPosition();
  while (pos!=NULL)
  {
   out+=" "+connections.GetNext(pos)->name;  
  }
  out+=")\n";
  file->Write(out, out.GetLength());
 }

 //write ports as lists
 if (!ports.IsEmpty())
 {
  out=spaces+" (PORTS";
  POSITION pos=ports.GetHeadPosition();
  while (pos!=NULL)
  {
   out+=" "+ports.GetNext(pos)->name;  
  }
  out+=")\n";
  file->Write(out, out.GetLength());
 }

 //write components as objects
 POSITION pos=ports.GetHeadPosition();
 while (pos!=NULL)
 {
  ports.GetNext(pos)->xArch(file,level+1);  
 }

 pos=connections.GetHeadPosition();
 while (pos!=NULL)
 {
  connections.GetNext(pos)->xArch(file,level+1);  
 }
}

void SIMGraphBus::xPipe(CFile* file, SIMGraphObj* obj)
{ 
 //if (obj->rect.PtInRect(rect.CenterPoint())) //unit center is in stage rect, add it to the stage
 //{
 // ((SIMGraphPipelineStage*)obj)->AddPipelineStageObj(this);
 //}
}
