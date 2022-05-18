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
// SIMGraphStorage.cpp: implementation of the SIMGraphStorage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../pcprogui.h"
#include "SIMGraphStorage.h"
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

IMPLEMENT_SERIAL( SIMGraphStorage, SIMGraphObj, 1 )

SIMGraphStorage::SIMGraphStorage() : SIMGraphObj()
{
}

SIMGraphStorage::SIMGraphStorage(SIMGraphObj* pParent, CRect* pRect) : SIMGraphObj(pParent, pRect)
{
 borderColor=BLUEGREEN;
 type=MHI_SIM_STORAGE;
 parent=pParent;
 if (parent!=NULL) //fit inside parent
 {
  SIMGraphObj::FitRectangleInRectangle(&parent->rect,&rect);
 }
	
 addrRangeFrom=0;
	addrRangeTo=0;
	line=0;
	associativity=0;
	width=0;
	memSize=0;
	accessTime=0;
	memType=1;
}

SIMGraphStorage::~SIMGraphStorage()
{
 POSITION pos=ports.GetHeadPosition();
 while (pos!=NULL)
 {
  delete ports.GetNext(pos);
 }
 
 pos=connections.GetHeadPosition();
 while (pos!=NULL)
 {
  delete connections.GetNext(pos);
 }
}

void SIMGraphStorage::OnDraw(CDC * pDC)
{
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

 pos=__latches.GetHeadPosition();
 while (pos!=NULL)
 {
  __latches.GetNext(pos)->OnDraw(pDC);
 }
}


void SIMGraphStorage::OnDragDraw(CDC* pDC, CPoint& offset, SIMGraphObj* parent)
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

 pos=__latches.GetHeadPosition();
 while (pos!=NULL)
 {
  __latches.GetNext(pos)->OnDragDraw(pDC,offset,this);
 }
}


SIMGraphObj* SIMGraphStorage::getHit(CPoint& point)
{
 //walk all the component lists and see if there's a hit

 //see if we have a hit in this unit
 if (!rect.PtInRect(point)) return NULL;

 //order matters
 POSITION pos=connections.GetHeadPosition();
 while (pos!=NULL)
 {
  SIMGraphObj* hit=connections.GetNext(pos)->getHit(point);  
  if (hit!=NULL) return hit;
 }

 pos=__latches.GetHeadPosition();
 while (pos!=NULL)
 {
  SIMGraphObj* hit=__latches.GetNext(pos)->getHit(point);  
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


SIMGraphObj* SIMGraphStorage::GetSIMGraphObjByName(CString * testName)
{
 if (name==*testName) return this;

 //order matters
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



void SIMGraphStorage::AddPort(SIMGraphObj* obj)
{
 ports.AddTail(obj);
}

void SIMGraphStorage::RemovePort(SIMGraphObj* obj)
{
 ports.RemoveAt(ports.Find(obj));
}

void SIMGraphStorage::AddConnection(SIMGraphObj* obj)
{
 connections.AddTail(obj);
}

void SIMGraphStorage::RemoveConnection(SIMGraphObj* obj)
{
 connections.RemoveAt(connections.Find(obj));
}

void SIMGraphStorage::_AddConnection(SIMGraphObj* obj)
{
 _connections.AddTail(obj);
}


void SIMGraphStorage::Offset(CPoint& point, SIMGraphObj* end)
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

 pos=__latches.GetHeadPosition();
 while (pos!=NULL)
 {
  __latches.GetNext(pos)->Offset(point,this);
 }
}


void SIMGraphStorage::genO(CFile* file, int level)
{
 CString spaces(_T(' '), level);
 CString out;


 out.Format("////  object properties for SIMGraphStorage %s  /////////////////////////////////////////////////////////\n\n"
     +spaces+"obj=new SIMGraphStorage();\n"
     +spaces+"tempList.AddTail(obj);\n\n",
     name);
 file->Write(out, out.GetLength());

 SIMGraphObj::genO(file, level);

 out.Format(
  spaces+"((SIMGraphStorage*)obj)->customProperties=\"%s\";\n"
  +spaces+"((SIMGraphStorage*)obj)->addrRangeFrom=%d;\n"
  +spaces+"((SIMGraphStorage*)obj)->addrRangeTo=%d;\n"
  +spaces+"((SIMGraphStorage*)obj)->line=%d;\n"
  +spaces+"((SIMGraphStorage*)obj)->associativity=%d;\n"
  +spaces+"((SIMGraphStorage*)obj)->width=%d;\n"
  +spaces+"((SIMGraphStorage*)obj)->memSize=%d;\n"
  +spaces+"((SIMGraphStorage*)obj)->accessTime=%d;\n"
  +spaces+"((SIMGraphStorage*)obj)->memType=%d;\n",
  customProperties,
  addrRangeFrom,
  addrRangeTo,
  line,
  associativity,
  width,
  memSize,
  accessTime,
  memType
  );
 file->Write(out, out.GetLength());

 out="\n";
 file->Write(out, out.GetLength());

 SIMDoc::genOSIMGraphObjList(ports,file,level+1);
 SIMDoc::genOSIMGraphObjList(connections,file,level+1);
}

void SIMGraphStorage::genC(CFile* file, int level)
{
 CString spaces(_T(' '), level);
 CString out;

 out.Format("////  object lists for SIMGraphStorage %s  /////////////////////////////////////////////////////////\n\n",
     name);
 file->Write(out, out.GetLength());

 out.Format(spaces+"obj=FindSIMGraphObj(\"%s\");\n\n",name);
 file->Write(out, out.GetLength());

 SIMGraphObj::genC(file, level);

 //out.Format(spaces+"////  __latches for %s  //////////////////////////////////////////////////\n\n", name);
 //file->Write(out, out.GetLength());

 POSITION pos=__latches.GetHeadPosition();
 while (pos!=NULL)
 {
  out.Format(spaces+"((SIMGraphStorage*)obj)->__AddLatch(FindSIMGraphObj(\"%s\"));\n", __latches.GetAt(pos)->name);
  file->Write(out, out.GetLength());
  __latches.GetNext(pos);
 }

 if (!__latches.IsEmpty())
 {
  out="\n";
  file->Write(out, out.GetLength());
 }

 //SIMDoc::genOSIMGraphObjList(ports,file,level+1);
 pos=ports.GetHeadPosition();
 while (pos!=NULL)
 {
  out.Format(spaces+"((SIMGraphStorage*)obj)->AddPort(FindSIMGraphObj(\"%s\"));\n", ports.GetAt(pos)->name);
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
  out.Format(spaces+"((SIMGraphStorage*)obj)->AddConnection(FindSIMGraphObj(\"%s\"));\n", connections.GetAt(pos)->name);
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

void SIMGraphStorage::Serialize(CArchive& ar)
{
 SIMGraphObj::Serialize(ar);

 if (ar.IsStoring())
 {
  ar << customProperties << addrRangeFrom << addrRangeTo << line << associativity << width << memSize << accessTime << memType << mnemonic;
 }
 else
 {
  ar >> customProperties >> addrRangeFrom >> addrRangeTo >> line >> associativity >> width >> memSize >> accessTime >> memType >> mnemonic;
 }


 __latches.Serialize(ar);
 SIMDoc::SerializeSIMGraphObjList(ports,ar);
 SIMDoc::SerializeSIMGraphObjList(connections,ar);
}


void SIMGraphStorage::AddSubtype(StringList* pCompoundunits, StringList* pUnits, StringList* pPorts, StringList* pConnections, StringList* pStorages, StringList* pLatches)
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
  pos=pStorages->GetHeadPosition();
  while (pos!=NULL)
  {
   if (pStorages->GetNext(pos)==classname) return; //already added
  }
  pStorages->AddTail(classname); 
 } 
}


void SIMGraphStorage::PatchPointers()
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

 pos=__latches.GetHeadPosition();
 while (pos!=NULL)
 {
  SIMGraphObj* obj=__latches.GetAt(pos);
  __latches.SetAt(pos, SIMGraphObj::getPtrMapping(__latches.GetAt(pos)));
  obj=__latches.GetAt(pos);
  __latches.GetNext(pos);
 }
}


void SIMGraphStorage::xStor(CFile* file, int level)
{
 CString spaces(_T(' '), level);
 CString out;

 out=spaces+"("+name+"\n";
 file->Write(out, out.GetLength());

 switch (memType)
 {
  case 1: //REGFILE
   //type
   out.Format(spaces+" (TYPE %s)\n", "VirtualRegFile");
   file->Write(out, out.GetLength());
   //width
   out.Format(spaces+" (WIDTH %d)\n", width);
   file->Write(out, out.GetLength());
   //size
   out.Format(spaces+" (SIZE %d)\n", memSize);
   file->Write(out, out.GetLength());
   //mnemonic
   out.Format(spaces+" (MNEMONIC \"%s\")\n", mnemonic);
   file->Write(out, out.GetLength());
   break;
  
  case 2: //ICACHE
   //type
 //  out.Format(spaces+" (TYPE %s)\n", "CACHE");
   out.Format(spaces+" (TYPE %s)\n", "ICACHE");
   file->Write(out, out.GetLength());
   //width/wordsize
   out.Format(spaces+" (WORDSIZE %d)\n", width);
   file->Write(out, out.GetLength());
   //size
   out.Format(spaces+" (LINESIZE %d)\n", memSize);
   file->Write(out, out.GetLength());
   //assoc
   out.Format(spaces+" (ASSOCIATIVITY %d)\n", associativity);
   file->Write(out, out.GetLength());
   //line
   out.Format(spaces+" (NUM_LINES %d)\n", line);
   file->Write(out, out.GetLength());
   //access
   out.Format(spaces+" (ACCESS_TIMES %d)\n", accessTime);
   file->Write(out, out.GetLength());
   //addr range
   out.Format(spaces+" (ADDRESS_RANGE (%d %d))\n", addrRangeFrom, addrRangeTo);
   file->Write(out, out.GetLength());
   break;

  case 3: //DCACHE
   //type
 //  out.Format(spaces+" (TYPE %s)\n", "CACHE");
   out.Format(spaces+" (TYPE %s)\n", "DCACHE");
   file->Write(out, out.GetLength());
   //width/wordsize
   out.Format(spaces+" (WORDSIZE %d)\n", width);
   file->Write(out, out.GetLength());
   //size
   out.Format(spaces+" (LINESIZE %d)\n", memSize);
   file->Write(out, out.GetLength());
   //assoc
   out.Format(spaces+" (ASSOCIATIVITY %d)\n", associativity);
   file->Write(out, out.GetLength());
   //line
   out.Format(spaces+" (NUM_LINES %d)\n", line);
   file->Write(out, out.GetLength());
   //access
   out.Format(spaces+" (ACCESS_TIMES %d)\n", accessTime);
   file->Write(out, out.GetLength());
   //addr range
   out.Format(spaces+" (ADDRESS_RANGE (%d %d))\n", addrRangeFrom, addrRangeTo);
   file->Write(out, out.GetLength());
   break;

  case 4: //SRAM
   //type
//   out.Format(spaces+" (TYPE %s)\n", "RAM");
   out.Format(spaces+" (TYPE %s)\n", "SRAM");
   file->Write(out, out.GetLength());
   //width
   //out.Format(spaces+" (WIDTH %d)\n", width);
   //file->Write(out, out.GetLength());
   //size
   //out.Format(spaces+" (SIZE %d)\n", memSize);
   //file->Write(out, out.GetLength());
   //access
   out.Format(spaces+" (ACCESS_TIMES %d)\n", accessTime);
   file->Write(out, out.GetLength());
   //addr range
   out.Format(spaces+" (ADDRESS_RANGE (%d %d))\n", addrRangeFrom, addrRangeTo);
   file->Write(out, out.GetLength());
   break;

  case 5: //DRAM
   //type
//   out.Format(spaces+" (TYPE %s)\n", "RAM");
   out.Format(spaces+" (TYPE %s)\n", "DRAM");
   file->Write(out, out.GetLength());
   //width
   //out.Format(spaces+" (WIDTH %d)\n", width);
   //file->Write(out, out.GetLength());
   //size
   //out.Format(spaces+" (SIZE %d)\n", memSize);
   //file->Write(out, out.GetLength());
   //access
   out.Format(spaces+" (ACCESS_TIMES %d)\n", accessTime);
   file->Write(out, out.GetLength());
   //addr range
   out.Format(spaces+" (ADDRESS_RANGE (%d %d))\n", addrRangeFrom, addrRangeTo);
   file->Write(out, out.GetLength());
   break;

  default:
   ASSERT(0);
   break;
 }

 /*
 //TODO indent customProperties
 if (customProperties!="") 
 {
  out=spaces+customProperties;
  file->Write(out, out.GetLength());
 }
 */

 out=spaces+")\n\n";
 file->Write(out, out.GetLength());

 /*
 //write components as objects
 POSITION pos=ports.GetHeadPosition();
 while (pos!=NULL)
 {
  ports.GetNext(pos)->xArch(file,level+1);  
 }
 */
}



void SIMGraphStorage::xArch(CFile * file, int level)
{

 CString spaces(_T(' '), level);
 CString out;

 if (classname=="")
 {
  out=spaces+"(Storage "+name+"\n";
 }
 else
 {
  out=spaces+"(" +classname+ " " +name+"\n";
 }
 file->Write(out, out.GetLength());

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

 //TODO indent customProperties
 if (customProperties!="") 
 {
  out=spaces+customProperties;
  file->Write(out, out.GetLength());
 }

 out=spaces+")\n\n";
 file->Write(out, out.GetLength());

 //write components as objects
 POSITION pos=ports.GetHeadPosition();
 while (pos!=NULL)
 {
  ports.GetNext(pos)->xArch(file,level+1);  
 }
}

