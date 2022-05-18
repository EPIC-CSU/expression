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
// SIMGraphConnection.cpp: implementation of the SIMGraphConnection class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../pcprogui.h"
#include "SIMGraphConnection.h"

#include "../ColorCodes.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_SERIAL( SIMGraphConnection, SIMGraphObj, 1 )

#define CONN_WIDTH 5

SIMGraphConnection::SIMGraphConnection() : SIMGraphObj()
{
}

SIMGraphConnection::SIMGraphConnection(SIMGraphObj* pParent, CRect* pRect, SIMGraphObj* pStartObj, CPoint* pStartObjPoint, SIMGraphObj* pEndObj, CPoint* pEndObjPoint) 
: SIMGraphObj(pParent, pRect)
{
 ASSERT(pParent!=NULL);

 borderColor=DIRTYYELLOW;
 type=MHI_SIM_CONNECTION;

 startObj=pStartObj;
 endObj=pEndObj;

 rectStart=CRect(pStartObjPoint->x,pStartObjPoint->y,pStartObjPoint->x+CONN_WIDTH,pStartObjPoint->y+CONN_WIDTH);
 rectEnd=CRect(pEndObjPoint->x,pEndObjPoint->y,pEndObjPoint->x+CONN_WIDTH,pEndObjPoint->y+CONN_WIDTH);

 //do fitting for both ends of connection
 SIMGraphObj::FitRectangleInRectangle(&pStartObj->rect, &rectStart);
 SIMGraphObj::FitRectangleInRectangle(&pEndObj->rect, &rectEnd);
}

SIMGraphConnection::~SIMGraphConnection()
{
 //remove this connection from the connections list of startObj
 //remove this connection from the _connections list of endObj
 if ((startObj!=NULL) && (endObj!=NULL))
 {
  if (startObj->type==MHI_SIM_LATCH)
  {
   startObj->RemoveConnection(this);
   endObj->__RemoveLatch(this);
  }
  else if (endObj->type==MHI_SIM_LATCH)
  {
   startObj->__RemoveLatch(this);
   endObj->RemoveConnection(this);
  }
  else
  {
   startObj->RemoveConnection(this);
   endObj->_RemoveConnection(this);
  }
 }
}

void SIMGraphConnection::OnDraw(CDC * pDC)
{
 //setup the DC params
 
 //brush
 CBrush fullBrush;
 fullBrush.CreateSolidBrush(selected ? YELLOW : borderColor);
 CBrush* oldBrush=pDC->SelectObject(&fullBrush);
 
 CPen* newPen;
 CPen* oldPen;
 if (breakpoint) 
 { 
  newPen = new CPen(PS_SOLID,1,RED);
  oldPen = pDC->SelectObject(newPen);
  pDC->MoveTo(rectStart.CenterPoint());
  pDC->LineTo(rectEnd.CenterPoint());
  pDC->SelectObject(oldPen);
  delete newPen;
  newPen = new CPen(PS_SOLID,1,selected ? YELLOW : borderColor);
  oldPen = pDC->SelectObject(newPen);
 } 
 else 
 {
  newPen = new CPen(PS_SOLID,1,selected ? YELLOW : borderColor);
  oldPen = pDC->SelectObject(newPen);
  pDC->MoveTo(rectStart.CenterPoint());
  pDC->LineTo(rectEnd.CenterPoint());
 }

 pDC->Rectangle(rectStart);
 pDC->Rectangle(rectEnd);

 pDC->SelectObject(oldPen);
 pDC->SelectObject(oldBrush);  
 delete newPen;
}


void SIMGraphConnection::OnDragDraw(CDC* pDC, CPoint& offset, SIMGraphObj* parent)
{
 //setup the DC params
 
 //brush
 CBrush fullBrush;
 fullBrush.CreateSolidBrush(selected ? YELLOW : borderColor);
 CBrush* oldBrush=pDC->SelectObject(&fullBrush);
 
 //text attributes
 pDC->SetTextColor(WHITE);
 pDC->SetBkMode(TRANSPARENT);
 
 int lastROP2=pDC->SetROP2(R2_XORPEN);

 CPen* newPen;
 CPen* oldPen;

 CRect newRectStart=rectStart;
 CRect newRectEnd=rectEnd;
 if (parent==startObj)
 {
  newRectStart.OffsetRect(offset);
  //TODO offset other end by scrollbar values
  newRectEnd.OffsetRect(-SIMGraphObj::hScroll,-SIMGraphObj::vScroll);
 }
 else //if (parent==startObj)
 {
  newRectEnd.OffsetRect(offset);
  //TODO offset other end by scrollbar values
  newRectStart.OffsetRect(-SIMGraphObj::hScroll,-SIMGraphObj::vScroll);
 }
 //else
 // ASSERT(0);

 if (breakpoint) 
 { 
  newPen = new CPen(PS_SOLID,1,RED);
  oldPen = pDC->SelectObject(newPen);
  pDC->MoveTo(newRectStart.CenterPoint());
  pDC->LineTo(newRectEnd.CenterPoint());
  pDC->SelectObject(oldPen);
  delete newPen;
  newPen = new CPen(PS_SOLID,1,selected ? YELLOW : borderColor);
  oldPen = pDC->SelectObject(newPen);
 } 
 else 
 {
  newPen = new CPen(PS_SOLID,1,selected ? YELLOW : borderColor);
  oldPen = pDC->SelectObject(newPen);
  pDC->MoveTo(newRectStart.CenterPoint());
  pDC->LineTo(newRectEnd.CenterPoint());
 }

 pDC->Rectangle(newRectStart);
 pDC->Rectangle(newRectEnd);

 pDC->SetROP2(lastROP2);

 pDC->SelectObject(oldPen);
 pDC->SelectObject(oldBrush);  
 delete newPen;
}


SIMGraphObj* SIMGraphConnection::getHit(CPoint& point)
{
 //verify if hit is in either of end connectors

 if (rectStart.PtInRect(point)==TRUE) return this;

 if (rectEnd.PtInRect(point)==TRUE) return this;

 return NULL;
}


SIMGraphObj* SIMGraphConnection::GetSIMGraphObjByName(CString* testName)
{
 if (name==*testName) 
  return this;
 else
  return NULL;
}


void SIMGraphConnection::Offset(CPoint & point, SIMGraphObj* end)
{
 ASSERT(end!=NULL);
 if (end==startObj) rectStart.OffsetRect(point);
 if (end==endObj) rectEnd.OffsetRect(point);
}


void SIMGraphConnection::_AddConnection(SIMGraphObj* obj)
{
 ASSERT(0);
}

void SIMGraphConnection::genO(CFile* file, int level)
{
 CString spaces(_T(' '), level);
 CString out;

 out.Format("////  object properties for SIMGraphConnection %s  /////////////////////////////////////////////////////////\n\n"
     +spaces+"obj=new SIMGraphConnection();\n"
     +spaces+"tempList.AddTail(obj);\n\n",
     name);
 file->Write(out, out.GetLength());

 SIMGraphObj::genO(file, level);

 out.Format(
  spaces+"((SIMGraphConnection*)obj)->customProperties=\"%s\";\n"

  +spaces+"((SIMGraphConnection*)obj)->rectStart.top=%d;\n"
  +spaces+"((SIMGraphConnection*)obj)->rectStart.bottom=%d;\n"
  +spaces+"((SIMGraphConnection*)obj)->rectStart.left=%d;\n"
  +spaces+"((SIMGraphConnection*)obj)->rectStart.right=%d;\n"

  +spaces+"((SIMGraphConnection*)obj)->rectEnd.top=%d;\n"
  +spaces+"((SIMGraphConnection*)obj)->rectEnd.bottom=%d;\n"
  +spaces+"((SIMGraphConnection*)obj)->rectEnd.left=%d;\n"
  +spaces+"((SIMGraphConnection*)obj)->rectEnd.right=%d;\n",
  customProperties,

  rectStart.top,
  rectStart.bottom,
  rectStart.left,
  rectStart.right,

  rectEnd.top,
  rectEnd.bottom,
  rectEnd.left,
  rectEnd.right
  );
 file->Write(out, out.GetLength());

 out="\n";
 file->Write(out, out.GetLength());
}

void SIMGraphConnection::genC(CFile* file, int level)
{
 CString spaces(_T(' '), level);
 CString out;

 out.Format("////  object lists for SIMGraphConnection %s  /////////////////////////////////////////////////////////\n\n",
     name);
 file->Write(out, out.GetLength());

 out.Format(spaces+"obj=FindSIMGraphObj(\"%s\");\n\n",name);
 file->Write(out, out.GetLength());

 SIMGraphObj::genC(file, level);

 //startObj and endObj are set here
 out.Format(spaces+"((SIMGraphConnection*)obj)->startObj=FindSIMGraphObj(\"%s\");\n\n",startObj->name);
 file->Write(out, out.GetLength());
 out.Format(spaces+"((SIMGraphConnection*)obj)->endObj=FindSIMGraphObj(\"%s\");\n\n",endObj->name);
 file->Write(out, out.GetLength());

 out="\n";
 file->Write(out, out.GetLength());
}

void SIMGraphConnection::Serialize(CArchive& ar)
{
 SIMGraphObj::Serialize(ar);

 long lStartObj,lEndObj;
	if (ar.IsStoring())
	{
  lStartObj=(long)startObj;
  lEndObj=(long)endObj;
  ar << lStartObj << lEndObj << rectStart << rectEnd;
 }
	else
	{
  ar >> lStartObj >> lEndObj >> rectStart >> rectEnd;
  startObj=(SIMGraphObj*)lStartObj;
  endObj=(SIMGraphObj*)lEndObj;
	}
}

 
void SIMGraphConnection::PatchPointers()
{
 SIMGraphObj::PatchPointers();

 startObj=SIMGraphObj::getPtrMapping(startObj);
 endObj=SIMGraphObj::getPtrMapping(endObj);
}

void SIMGraphConnection::AddSubtype(StringList* pCompoundunits, StringList* pUnits, StringList* pPorts, StringList* pConnections, StringList* pStorages, StringList* pLatches)
{
 if (classname!="")
 {
  POSITION pos=pConnections->GetHeadPosition();
  while (pos!=NULL)
  {
   if (pConnections->GetNext(pos)==classname) return; //already added
  }
  pConnections->AddTail(classname); 
 } 
}

void SIMGraphConnection::xArch(CFile* file, int level)
{
 CString spaces(_T(' '), level);
 CString out;

 if (classname=="")
 {
  out=spaces+"(Connection "+name+"\n";
 }
 else
 {
  out=spaces+"(" + classname + " " +name+"\n";
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
