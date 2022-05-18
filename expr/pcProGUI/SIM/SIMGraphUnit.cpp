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
// SIMGraphUnit.cpp: implementation of the SIMGraphUnit class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../pcprogui.h"
#include "SIMGraphUnit.h"
#include "SIMGraphConnection.h"
#include "SIMGraphLatch.h"
#include "SIMGraphPipelineStage.h"
#include "SIMDoc.h"

// cant include vector here - why??
//	vector<CString> unit_inputlatch[50];
//vector<CString> unit_outputlatch[50];

CString unit_inputlatch[50][50];
CString unit_outputlatch[50][50];
int unitnumber=0;
#include "../ColorCodes.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_SERIAL( SIMGraphUnit, SIMGraphObj, 1 )

SIMGraphUnit::SIMGraphUnit() : SIMGraphObj()
{
}

SIMGraphUnit::SIMGraphUnit(SIMGraphObj* pParent, CRect* pRect) : SIMGraphObj(pParent, pRect)
{
 borderColor=BLUEGRAY;
 type=MHI_SIM_UNIT;
 parent=pParent;
 capacity=1;
 //timing=1;
 instrin=0;
 instrout=0;

 if (parent!=NULL) //fit inside parent
 {
  SIMGraphObj::FitRectangleInRectangle(&parent->rect,&rect);
 }
}

SIMGraphUnit::~SIMGraphUnit()
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

 if (parent!=NULL) //remove this unit from the units list of compound unit
 {
  parent->RemoveUnit(this);
 }
}

void SIMGraphUnit::OnDraw(CDC * pDC)
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

 pos=latches.GetHeadPosition();
 while (pos!=NULL)
 {
  latches.GetNext(pos)->OnDraw(pDC);
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


void SIMGraphUnit::OnDragDraw(CDC* pDC, CPoint& offset, SIMGraphObj* parent)
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

 pos=latches.GetHeadPosition();
 while (pos!=NULL)
 {
  latches.GetNext(pos)->OnDragDraw(pDC,offset,this);
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


SIMGraphObj* SIMGraphUnit::getHit(CPoint& point)
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

 pos=latches.GetHeadPosition();
 while (pos!=NULL)
 {
  SIMGraphObj* hit=latches.GetNext(pos)->getHit(point);  
  if (hit!=NULL) return hit;
 }

 return this;
}



SIMGraphObj* SIMGraphUnit::GetSIMGraphObjByName(CString * testName)
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

 pos=latches.GetHeadPosition();
 while (pos!=NULL)
 {
  SIMGraphObj* hit=latches.GetNext(pos)->GetSIMGraphObjByName(testName);
  if (hit!=NULL) return hit;
 }

 return NULL;
}

void SIMGraphUnit::AddPort(SIMGraphObj* obj)
{
 ports.AddTail(obj);
}

void SIMGraphUnit::RemovePort(SIMGraphObj* obj)
{
 ports.RemoveAt(ports.Find(obj));
}

void SIMGraphUnit::AddLatch(SIMGraphObj* obj)
{
 latches.AddTail(obj);
}

void SIMGraphUnit::RemoveLatch(SIMGraphObj* obj)
{
 latches.RemoveAt(latches.Find(obj));
}

void SIMGraphUnit::AddConnection(SIMGraphObj* obj)
{
 connections.AddTail(obj);
}

void SIMGraphUnit::RemoveConnection(SIMGraphObj* obj)
{
 connections.RemoveAt(connections.Find(obj));
}

void SIMGraphUnit::_AddConnection(SIMGraphObj* obj)
{
 _connections.AddTail(obj);
}

void SIMGraphUnit::__AddLatch(SIMGraphObj* obj)
{
 __latches.AddTail(obj);
}

void SIMGraphUnit::__RemoveLatch(SIMGraphObj* obj)
{
 __latches.RemoveAt(__latches.Find(obj));
}




void SIMGraphUnit::Offset(CPoint& point, SIMGraphObj* end)
{
 rect.OffsetRect(point);

 POSITION pos=ports.GetHeadPosition();
 while (pos!=NULL)
 {
  ports.GetNext(pos)->Offset(point);  
 }

 pos=latches.GetHeadPosition();
 while (pos!=NULL)
 {
  latches.GetNext(pos)->Offset(point,this);  
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


void SIMGraphUnit::genO(CFile* file, int level)
{
 CString spaces(_T(' '), level);
 CString out;

 out.Format("////  object properties for SIMGraphUnit %s  /////////////////////////////////////////////////////////\n\n"
     +spaces+"obj=new SIMGraphUnit();\n"
     +spaces+"tempList.AddTail(obj);\n\n",
     name);
 file->Write(out, out.GetLength());

 SIMGraphObj::genO(file, level);

 out.Format(
  spaces+"((SIMGraphUnit*)obj)->customProperties=\"%s\";\n"
  +spaces+"((SIMGraphUnit*)obj)->opcodes=\"%s\";\n"
  +spaces+"((SIMGraphUnit*)obj)->capacity=%d;\n"
  +spaces+"((SIMGraphUnit*)obj)->timing=%s;\n"
  +spaces+"((SIMGraphUnit*)obj)->instrin=%d;\n"
  +spaces+"((SIMGraphUnit*)obj)->instrout=%d;\n",
  customProperties,
  opcodes,
  capacity,
  timing,
  instrin,
  instrout
  );
 file->Write(out, out.GetLength());

 out="\n";
 file->Write(out, out.GetLength());

 SIMDoc::genOSIMGraphObjList(ports,file,level+1);
 SIMDoc::genOSIMGraphObjList(connections,file,level+1);
 SIMDoc::genOSIMGraphObjList(latches,file,level+1);
}

void SIMGraphUnit::genC(CFile* file, int level)
{
 CString spaces(_T(' '), level);
 CString out;

 out.Format("////  object lists for SIMGraphUnit %s  /////////////////////////////////////////////////////////\n\n",
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
  out.Format(spaces+"((SIMGraphUnit*)obj)->__AddLatch(FindSIMGraphObj(\"%s\"));\n", __latches.GetAt(pos)->name);
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
  out.Format(spaces+"((SIMGraphUnit*)obj)->AddPort(FindSIMGraphObj(\"%s\"));\n", ports.GetAt(pos)->name);
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
  out.Format(spaces+"((SIMGraphUnit*)obj)->AddConnection(FindSIMGraphObj(\"%s\"));\n", connections.GetAt(pos)->name);
  file->Write(out, out.GetLength());
  connections.GetNext(pos);
 }

 if (!connections.IsEmpty())
 {
  out="\n";
  file->Write(out, out.GetLength());
 }

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

 pos=latches.GetHeadPosition();
 while (pos!=NULL)
 {
  latches.GetNext(pos)->genC(file,level+1);
 }

 out="\n";
 file->Write(out, out.GetLength());
}

void SIMGraphUnit::Serialize(CArchive& ar)
{
 SIMGraphObj::Serialize(ar);

	if (ar.IsStoring())
	{

  ar << customProperties << opcodes << capacity << timing << instrin << instrout;
//	ar << customProperties << opcodes << capacity << timing;
 }
	else
	{

  ar >> customProperties >> opcodes >> capacity >> timing >> instrin >> instrout;
//ar >> customProperties >> opcodes >> capacity >> timing;
	}

 __latches.Serialize(ar);

 SIMDoc::SerializeSIMGraphObjList(ports,ar);
 SIMDoc::SerializeSIMGraphObjList(connections,ar);
 SIMDoc::SerializeSIMGraphObjList(latches,ar);
}
 
void SIMGraphUnit::PatchPointers()
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


void SIMGraphUnit::AddSubtype(StringList* pCompoundunits, StringList* pUnits, StringList* pPorts, StringList* pConnections, StringList* pStorages, StringList* pLatches)
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

 pos=latches.GetHeadPosition();
 while (pos!=NULL)
 {
  latches.GetNext(pos)->AddSubtype(pCompoundunits, pUnits, pPorts, pConnections, pStorages, pLatches);
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


void SIMGraphUnit::xArch(CFile* file, int level)
{
 CString spaces(_T(' '), level);
 CString out;

 if (classname=="")
 {
  out=spaces+"(Unit "+name+"\n";
 }
 else
 {
  out=spaces+"(" +classname+ " " +name+"\n";
 }
 file->Write(out, out.GetLength());
 //write properties

 if (classname.CompareNoCase("fetchunit") && classname.CompareNoCase("decodeunit") && classname.CompareNoCase("writebackunit"))
 {
 unit_inputlatch[unitnumber][0] = name;
 unit_outputlatch[unitnumber][0] = name;
 }

 if (capacity != 0)
 {
	 out.Format(spaces+" (CAPACITY %d)\n",capacity);
	 file->Write(out, out.GetLength());
 }

 if (instrin != 0)
 {
	 out.Format(spaces+" (INSTR_IN %d)\n",instrin);
	 file->Write(out, out.GetLength());
 }

  if (instrout != 0)
 {
	 out.Format(spaces+" (INSTR_OUT %d)\n",instrout);
	 file->Write(out, out.GetLength());
 }
 
 //out.Format(spaces+" (TIMING (all %d))\n",timing);
 out.Format(spaces+" (TIMING %s)\n",timing);
 file->Write(out, out.GetLength());
 if (opcodes=="")
 {
  out=spaces+" (OPCODES all)\n";
 }
 else
 {
  out.Format(spaces+" (OPCODES %s)\n",opcodes);
 }
 file->Write(out, out.GetLength());

 //TODO indent customProperties
 if (customProperties!="") 
 {
  out=spaces+customProperties;
  file->Write(out, out.GetLength());
 }

 int latchinnum = 1;
 int latchoutnum = 1;

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
	if (classname.CompareNoCase("fetchunit") && classname.CompareNoCase("decodeunit") && classname.CompareNoCase("writebackunit"))
	unit_inputlatch[unitnumber][latchinnum++] = latch->name;
    out+=" (IN "+latch->name + ")"; 	
    break;

   case 2:
	if (classname.CompareNoCase("fetchunit") && classname.CompareNoCase("decodeunit") && classname.CompareNoCase("writebackunit"))
	unit_outputlatch[unitnumber][latchoutnum++] = latch->name;
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
	if (classname.CompareNoCase("fetchunit") && classname.CompareNoCase("decodeunit") && classname.CompareNoCase("writebackunit"))
	unit_outputlatch[unitnumber][latchinnum++] = latch->name;
    break;

   case 2:
    out+=" (IN "+latch->name + ")";  
	if (classname.CompareNoCase("fetchunit") && classname.CompareNoCase("decodeunit") && classname.CompareNoCase("writebackunit"))
	unit_inputlatch[unitnumber][latchinnum++] = latch->name;
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

 if (classname.CompareNoCase("fetchunit") && classname.CompareNoCase("decodeunit") && classname.CompareNoCase("writebackunit"))
 unitnumber++;

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

 out=spaces+")\n\n";
 file->Write(out, out.GetLength());

 //write components as objects
 POSITION pos=latches.GetHeadPosition();
 while (pos!=NULL)
 {
  latches.GetNext(pos)->xArch(file,level+1);  
 }

 pos=ports.GetHeadPosition();
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

void SIMGraphUnit::xPipe(CFile* file, SIMGraphObj* obj)
{ 
 if (obj->rect.PtInRect(rect.CenterPoint())) //unit center is in stage rect, add it to the stage
 {
  ((SIMGraphPipelineStage*)obj)->AddPipelineStageObj(this);
 }
}
