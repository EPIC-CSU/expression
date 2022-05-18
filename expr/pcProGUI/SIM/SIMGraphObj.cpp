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
// SIMGraphObj.cpp: implementation of the SIMGraphObj class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../pcprogui.h"
#include "SIMGraphObj.h"
#include "../ColorCodes.h"

#include "SIMGraphConnection.h"
#include "../Stat/GUIBaseStat.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_SERIAL( SIMGraphObj, CObject, 1 )

long SIMGraphObj::SIMObjID=1;
SIMGraphObjMap SIMGraphObj::Ptr2PtrMap;
int SIMGraphObj::vScroll=0;
int SIMGraphObj::hScroll=0;

SIMGraphObj::SIMGraphObj()
{
 simObj=NULL;
 selected=false;
 breakpoint=false;
 type=MHI_INVALID;
 parent=NULL;
}

SIMGraphObj::SIMGraphObj(SIMGraphObj* pParent, CRect* pRect)
{
 char str[20];

 //set the default callback
 selected=false;

 name="Obj";
 ltoa(SIMGraphObj::SIMObjID++,str,10);
 name+=str;

 parent=pParent;
 breakpoint=false;

 ASSERT(pRect!=NULL);
 rect=*pRect;

 type=MHI_INVALID;

 simObj=NULL;
}

SIMGraphObj::~SIMGraphObj()
{
 //_CrtDbgReport(_CRT_WARN, __FILE__, __LINE__, "!", "Deleted object: %s\n", name);

 POSITION pos=_connections.GetHeadPosition(); //remove connections that come into this node
 while (pos!=NULL)
 {
  delete _connections.GetNext(pos);
 }
 
 pos=statList.GetHeadPosition();
 while (pos!=NULL)
 {
  delete statList.GetNext(pos);
 }
}


SIMGraphObj* SIMGraphObj::GetSIMGraphObjByName(CString* testName)
{
 if (name==*testName)
  return this;
 else
  return NULL;
}

void SIMGraphObj::_RemoveConnection(SIMGraphObj* obj)
{
 _connections.RemoveAt(_connections.Find(obj));
}


void SIMGraphObj::ToggleSelected()
{
 selected = !selected;
}

void SIMGraphObj::ToggleBreakpoint()
{
 breakpoint=!breakpoint;
}

void SIMGraphObj::DisplayProperties()
{
}

void SIMGraphObj::OnDraw(CDC* pDC)
{
 ASSERT(0); //Don't call us, we'll call you!
}

void SIMGraphObj::OnDragDraw(CDC* pDC, CPoint& offset, SIMGraphObj* parent)
{
 ASSERT(0); //Don't call us, we'll call you!
}

bool SIMGraphObj::PtInObj(CPoint & point)
{
 return rect.PtInRect(point)==TRUE;
}

void SIMGraphObj::FitRectangleInRectangle(CRect* outerIN, CRect* inner)
{
 //fit inner rectangle in outer rectangle (it assumes that inner _CAN_ fit inside outer) 
 CRect outer(outerIN);
 outer.DeflateRect(2,2,2,2);

 if ((outer.PtInRect(inner->TopLeft())) && (outer.PtInRect(inner->BottomRight()))) //inner is already in outer
 {
  return;
 }

 //x axis CPoint
 if (inner->left<outer.left)
 {
  inner->OffsetRect(outer.left-inner->left,0);
 }

 if (inner->right>outer.right)
 {
  inner->OffsetRect(outer.right-inner->right,0);
 }
 
 if (inner->top<outer.top)
 {
  inner->OffsetRect(0,outer.top-inner->top);
 }

 if (inner->bottom>outer.bottom)
 {
  inner->OffsetRect(0,outer.bottom-inner->bottom);
 }
}


//PROMIS
void SIMGraphObj::genO(CFile* file, int level)
{
 CString spaces(_T(' '), level);
 CString out;

 CString colorName;
 switch (borderColor)
 {
 case WHITE: colorName="WHITE"; break;
 case BLACK: colorName="BLACK"; break;
 case BLUE: colorName="BLUE"; break;
 case RED: colorName="RED"; break;
 case GREEN: colorName="GREEN"; break;
 case YELLOW: colorName="YELLOW"; break;
 case MAGENTA: colorName="MAGENTA"; break;
 case CYAN: colorName="CYAN"; break;
 case ORANGE: colorName="ORANGE"; break;
 case HOTPINK: colorName="HOTPINK"; break;
 case CREAM: colorName="CREAM"; break;
 case BROWN: colorName="BROWN"; break;
 case LITEGREEN: colorName="LITEGREEN"; break;
 case MAUVE: colorName="MAUVE"; break;
 case DIRTYYELLOW: colorName="DIRTYYELLOW"; break;
 case BURGUNDY: colorName="BURGUNDY"; break;
 case DARKCYAN: colorName="DARKCYAN"; break;
 case BLUEGRAY: colorName="BLUEGRAY"; break;
 case BLUEGREEN: colorName="BLUEGREEN"; break;
 case CAFFEAULAIT: colorName="CAFFEAULAIT"; break;
 case FUNNYGREEN: colorName="FUNNYGREEN"; break;
 case FUNNYPINK: colorName="FUNNYPINK"; break;
 case FUNNYMAUVE: colorName="FUNNYMAUVE"; break;
 case FUNNYORANGE: colorName="FUNNYORANGE"; break;
 case FUNNYBROWN: colorName="FUNNYBROWN"; break;
 case FUNNYGREY: colorName="FUNNYGREY"; break;
 case FUNNYBLUE: colorName="FUNNYBLUE"; break;
 default: ASSERT(0);break;
 }

 CString typeName;
 switch (type)
 {
 case MHI_SIM_COMPOUND_UNIT: typeName="MHI_SIM_COMPOUND_UNIT"; break;
 case MHI_SIM_UNIT: typeName="MHI_SIM_UNIT"; break;
 case MHI_SIM_PORT: typeName="MHI_SIM_PORT"; break;
 case MHI_SIM_CONNECTION: typeName="MHI_SIM_CONNECTION"; break;
 case MHI_SIM_STORAGE: typeName="MHI_SIM_STORAGE"; break;
 case MHI_SIM_LATCH: typeName="MHI_SIM_LATCH"; break;
 case MHI_SIM_PIPELINESTAGE: typeName="MHI_SIM_PIPELINESTAGE"; break;
 case MHI_SIM_BUS: typeName="MHI_SIM_BUS"; break;
 default: ASSERT(0);break;
 }

 out.Format(spaces+"obj->borderColor=%s;\n"+spaces+"obj->type=%s;\n",colorName,typeName);
 file->Write(out, out.GetLength());
 
 if (type!=MHI_SIM_CONNECTION) //connections don't use rect info
 {
  out.Format(spaces+"obj->rect.top=%d;\n"+spaces+"obj->rect.bottom=%d;\n"+spaces+"obj->rect.left=%d;\n"+spaces+"obj->rect.right=%d;\n",
   rect.top, rect.bottom, rect.left, rect.right);
  file->Write(out, out.GetLength());
 }
 
 out.Format(spaces+"obj->name=\"%s\";\n"+spaces+"obj->classname=\"%s\";\n",name,classname);
 file->Write(out, out.GetLength());

 out="\n";
 file->Write(out, out.GetLength());
}

void SIMGraphObj::genC(CFile* file, int level)
{
 CString spaces(_T(' '), level);
 CString out;

 //out.Format(spaces+"obj=FindSIMGraphObj(\"%s\");\n",name);
 //file->Write(out, out.GetLength());

 if (parent!=NULL)
 {
  out.Format(spaces+"obj->parent=FindSIMGraphObj(\"%s\");\n\n",parent->name);
  file->Write(out, out.GetLength());
 }

 //out.Format(spaces+"////  _connections for %s  //////////////////////////////////////////////////\n", name);
 //file->Write(out, out.GetLength());

 POSITION pos=_connections.GetHeadPosition();
 while (pos!=NULL)
 {
  out.Format(spaces+"obj->_AddConnection(FindSIMGraphObj(\"%s\"));\n", _connections.GetAt(pos)->name);
  file->Write(out, out.GetLength());
  _connections.GetNext(pos);
 }

 if (!_connections.IsEmpty())
 {
  out="\n";
  file->Write(out, out.GetLength());
 }
}
//PROMIS

void SIMGraphObj::Serialize(CArchive& ar)
{
 CObject::Serialize(ar);

 long lType, lParent;
	if (ar.IsStoring())
	{
  lParent=(long)parent;
  lType=(long)type;
  ar << borderColor << rect << name << classname << lType << lParent;
  _connections.Serialize(ar);
 }
	else
	{
  ar >> borderColor >> rect >> name >> classname >> lType >> lParent;
  parent=(SIMGraphObj*)lParent;
  type=(MHI_HIT_TYPE)lType;
  selected=false;
  breakpoint=false;
  breakpoint=false;
  _connections.Serialize(ar);
	}
}

void SIMGraphObj::PatchPointers()
{
 POSITION pos=_connections.GetHeadPosition();
 while (pos!=NULL)
 {
  SIMGraphObj* obj=_connections.GetAt(pos);
  _connections.SetAt(pos, SIMGraphObj::getPtrMapping(_connections.GetAt(pos)));
  obj=_connections.GetAt(pos);
  _connections.GetNext(pos);
 }
 parent=SIMGraphObj::getPtrMapping(parent);
}




void SIMGraphObj::initPtrMapping()
{
 Ptr2PtrMap.RemoveAll();
 addPtrMapping(NULL,NULL);
}


void SIMGraphObj::addPtrMapping(SIMGraphObj* oldPtr, SIMGraphObj* newPtr)
{
 SIMGraphObj* ptr;
 if (Ptr2PtrMap.Lookup(oldPtr,ptr)!=0) //see if we already have a mapping
 {
  ASSERT(ptr==newPtr); //if so, it better map oldPtr to newPtr
 }
 else
 {
  Ptr2PtrMap.SetAt(oldPtr,newPtr);  
  //_CrtDbgReport(_CRT_WARN, __FILE__, __LINE__, "!", "Recorded mapping oldPtr %08X to new ptr %08X\n", oldPtr, newPtr);
 }
}

SIMGraphObj* SIMGraphObj::getPtrMapping(SIMGraphObj* oldPtr)
{
 SIMGraphObj* ptr;
 VERIFY(Ptr2PtrMap.Lookup(oldPtr,ptr));
 //_CrtDbgReport(_CRT_WARN, __FILE__, __LINE__, "!", "Mapped oldPtr %08X to new ptr %08X\n", oldPtr, ptr);
 return ptr;
}



bool SIMGraphObj::IsAncestorOf(SIMGraphObj* pParent)
{
 SIMGraphObj* p=parent;
 while (p!=NULL)
 {
  if (p==pParent) return true;
  p=p->parent;
 }

 return false;
}

void SIMGraphObj::AddSubtype(StringList* pCompoundunits, StringList* pUnits, StringList* pPorts, StringList* pConnections, StringList* pStorages, StringList* pLatches)
{
 ASSERT(0);
}

//empty fun for now
void SIMGraphObj::xArch(CFile* file, int level){}
void SIMGraphObj::xPipe(CFile* file, SIMGraphObj* obj){}
void SIMGraphObj::xStor(CFile* file, int level){}
void SIMGraphObj::xData(CFile* file, int level){}

//void SIMGraphObj::AttachSimulatorObject(BaseSimObject* obj)
//{
// simObj=obj; 
//}

void SIMGraphObj::OnBeginSimulation()
{
 //things to do for each SIMGraphObj before starting the simulation

}

void SIMGraphObj::OnEndSimulation()
{
 //things to do for each SIMGraphObj after ending the simulation

}
