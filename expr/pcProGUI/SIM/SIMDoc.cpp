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
// SIMDoc.cpp : implementation file
//

#include "stdafx.h"
#include "../pcProGUI.h"

#include "SIMDoc.h"

#include "SIMGraphCompoundUnit.h"
#include "SIMGraphUnit.h"
#include "SIMGraphStorage.h"
#include "SIMGraphLatch.h"
#include "SIMGraphConnection.h"
#include "SIMGraphBus.h"
#include "SIMGraphPort.h"
#include "SIMGraphPipelineStage.h"

#include "../Stat/GUIBaseStat.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define POINT_POS 72
#define POINT_SIZE 72

/////////////////////////////////////////////////////////////////////////////
// SIMDoc

IMPLEMENT_DYNCREATE(SIMDoc, CDocument)

int SIMDoc::storeVersion = 101; //means x.xx
int SIMDoc::loadVersion = 000; //not set

SIMDoc::SIMDoc()
{
 theApp.simDoc=this;
}

BOOL SIMDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

SIMDoc::~SIMDoc()
{
 //delete objects in SIM
 POSITION pos=simObjs.GetHeadPosition();
 while (pos!=NULL)
 {
  delete simObjs.GetNext(pos);  
 }

 pos=pipelineStages.GetHeadPosition();
 while (pos!=NULL)
 {
  delete pipelineStages.GetNext(pos);  
 }

 pos=simStatList.GetHeadPosition();
 while (pos!=NULL)
 {
  delete simStatList.GetNext(pos);
 }
}


BEGIN_MESSAGE_MAP(SIMDoc, CDocument)
	//{{AFX_MSG_MAP(SIMDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SIMDoc diagnostics

#ifdef _DEBUG
void SIMDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void SIMDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// SIMDoc serialization

void SIMDoc::Serialize(CArchive& ar)
{
 CWaitCursor wait;

 if (!ar.IsStoring())
 {
  SIMGraphObj::initPtrMapping();
 }

 if (ar.IsStoring())
 {
  loadVersion=storeVersion; //make sure we're storing with the latest format
  ar << loadVersion;
  ar << dataPaths.GetCount();
  POSITION pos=dataPaths.GetHeadPosition();
  while (pos!=NULL)
  {
   ar << dataPaths.GetNext(pos);
  }
 }
 else
 {
  int count;
  ar >> loadVersion;
  ar >> count;
  while (count-->0)
  {
   CString str;
   ar >> str;
   dataPaths.AddTail(str);
  }
 }

 SerializeSIMGraphObjList(simObjs,ar);
 SerializeSIMGraphObjList(pipelineStages,ar);

 if (!ar.IsStoring())
 {
  PatchPointers();
 }
}


/////////////////////////////////////////////////////////////////////////////
// SIMDoc commands


SIMGraphObj* SIMDoc::getHit(CPoint & point)
{
 //walk all the component lists and see if there's a hit

 POSITION pos=simObjs.GetHeadPosition();
 while (pos!=NULL)
 {
  SIMGraphObj* hit=simObjs.GetNext(pos)->getHit(point);  
  if (hit!=NULL) //we have our hit
  {   
   return hit;
  }
 }

 pos=pipelineStages.GetHeadPosition();
 while (pos!=NULL)
 {
  SIMGraphObj* hit=pipelineStages.GetNext(pos)->getHit(point);  
  if (hit!=NULL) //we have our hit
  {   
   return hit;
  }
 }

 return NULL;
}


SIMGraphObj* SIMDoc::GetSIMGraphObjByName(char* name)
{
 CString strName(name);
 //walk all the component lists and see if there's a hit

 POSITION pos=simObjs.GetHeadPosition();
 while (pos!=NULL)
 {
  SIMGraphObj* hit=simObjs.GetNext(pos)->GetSIMGraphObjByName(&strName);
  if (hit!=NULL) //we have our hit
  {   
   return hit;
  }
 }

 /*
 pos=pipelineStages.GetHeadPosition();
 while (pos!=NULL)
 {
  SIMGraphObj* hit=pipelineStages.GetNext(pos)->GetSIMGraphObjByName(strName);  
  if (hit!=NULL) //we have our hit
  {   
   return hit;
  }
 }
 */

 return NULL;
}



void SIMDoc::SerializeSIMGraphObjList(SIMGraphObjList& objList, CArchive & ar)
{
 long count,type, addr;
 SIMGraphObj* obj;

	if (ar.IsStoring())
	{
  count = (long)(objList.GetCount());
  ar << count;
  POSITION pos=objList.GetHeadPosition();
  while (pos!=NULL)
  {
   obj=objList.GetNext(pos);  
   type = (long)(obj->type);
   addr = (long)obj;
   ar << type << addr;
   switch (obj->type)
   {
   case MHI_INVALID:
    ASSERT(0);
    break;
    
   case MHI_SIM_COMPOUND_UNIT:
    ((SIMGraphCompoundUnit*)obj)->Serialize(ar);
    break;
    
   case MHI_SIM_UNIT:
    ((SIMGraphUnit*)obj)->Serialize(ar);
    break;
    
   case MHI_SIM_LATCH:
    ((SIMGraphLatch*)obj)->Serialize(ar);
    break;
    
   case MHI_SIM_PORT:
    ((SIMGraphPort*)obj)->Serialize(ar);
    break;
    
   case MHI_SIM_CONNECTION:
    ((SIMGraphConnection*)obj)->Serialize(ar);
    break;
    
   case MHI_SIM_BUS:
    ((SIMGraphBus*)obj)->Serialize(ar);
    break;
    
   case MHI_SIM_STORAGE:
    ((SIMGraphStorage*)obj)->Serialize(ar);
    break;

   case MHI_SIM_PIPELINESTAGE:
    ((SIMGraphStorage*)obj)->Serialize(ar);
    break;
   }
  }
	}
	else
	{
  MHI_HIT_TYPE realType;
  SIMGraphObj* realAddr;
  ar >> count;
  //delete objects in SIM
  POSITION pos=objList.GetHeadPosition();
  while (pos!=NULL)
  {
   delete objList.GetNext(pos);  
  }
  objList.RemoveAll(); 
  while (count--)
  {
   ar >> type >> addr;
   realType=(MHI_HIT_TYPE)type;
   realAddr=(SIMGraphObj*)addr;
   switch (type)
   {
   case MHI_INVALID:
    ASSERT(0);
    break;
    
   case MHI_SIM_COMPOUND_UNIT:
    obj=new SIMGraphCompoundUnit();
    ((SIMGraphCompoundUnit*)obj)->Serialize(ar);
    objList.AddTail(obj);
    break;
    
   case MHI_SIM_UNIT:
    obj=new SIMGraphUnit();
    ((SIMGraphUnit*)obj)->Serialize(ar);
    objList.AddTail(obj);
    break;
    
   case MHI_SIM_LATCH:
    obj=new SIMGraphLatch();
    ((SIMGraphLatch*)obj)->Serialize(ar);
    objList.AddTail(obj);
    break;
    
   case MHI_SIM_PORT:
    obj=new SIMGraphPort();
    ((SIMGraphPort*)obj)->Serialize(ar);
    objList.AddTail(obj);
    break;
    
   case MHI_SIM_CONNECTION:
    obj=new SIMGraphConnection();
    ((SIMGraphConnection*)obj)->Serialize(ar);
    objList.AddTail(obj);
    break;
    
   case MHI_SIM_BUS:
    obj=new SIMGraphBus();
    ((SIMGraphBus*)obj)->Serialize(ar);
    objList.AddTail(obj);
    break;
    
   case MHI_SIM_STORAGE:
    obj=new SIMGraphStorage();
    ((SIMGraphStorage*)obj)->Serialize(ar);
    objList.AddTail(obj);
    break;

   case MHI_SIM_PIPELINESTAGE:
    obj=new SIMGraphPipelineStage();
    ((SIMGraphPipelineStage*)obj)->Serialize(ar);
    objList.AddTail(obj);
    break;
   }
   SIMGraphObj::addPtrMapping((SIMGraphObj*)addr,obj);
  }
	}
}

void SIMDoc::genOSIMGraphObjList(SIMGraphObjList& objList, CFile* file, int level)
{
 CWaitCursor wait;

 //long count,type,addr;
 SIMGraphObj* obj;
 
 //count = (long)(objList.GetCount());
 POSITION pos=objList.GetHeadPosition();
 while (pos!=NULL)
 {
  obj=objList.GetNext(pos);  
  //type = (long)(obj->type);
  //addr = (long)obj;
  //ar << type << addr;
  switch (obj->type)
  {
  case MHI_INVALID:
   ASSERT(0);
   break;
   
  case MHI_SIM_COMPOUND_UNIT:
   ((SIMGraphCompoundUnit*)obj)->genO(file, level+1);
   break;
   
  case MHI_SIM_UNIT:
   ((SIMGraphUnit*)obj)->genO(file, level+1);
   break;
   
  case MHI_SIM_LATCH:
   ((SIMGraphLatch*)obj)->genO(file, level+1);
   break;
   
  case MHI_SIM_PORT:
   ((SIMGraphPort*)obj)->genO(file, level+1);
   break;
   
  case MHI_SIM_CONNECTION:
   ((SIMGraphConnection*)obj)->genO(file, level+1);
   break;
   
  case MHI_SIM_BUS:
   ((SIMGraphBus*)obj)->genO(file, level+1);
   break;
   
  case MHI_SIM_STORAGE:
   ((SIMGraphStorage*)obj)->genO(file, level+1);
   break;
   
  case MHI_SIM_PIPELINESTAGE:
   ((SIMGraphStorage*)obj)->genO(file, level+1);
   break;
  }
 }
}


void SIMDoc::PatchPointers()
{
 POSITION pos=simObjs.GetHeadPosition();
 while (pos!=NULL)
 {
  simObjs.GetNext(pos)->PatchPointers();  
 }
}

void SIMDoc::OutputSubtypeDefs(CFile* file, int level)
{
 CString spaces(_T(' '), level);
 CString out;

 StringList compoundunits;
 StringList units;
 StringList ports;
 StringList connections;
 StringList storages;
 StringList latches;

 POSITION pos=simObjs.GetHeadPosition();
 while (pos!=NULL)
 {
  simObjs.GetNext(pos)->AddSubtype(&compoundunits, &units, &ports, &connections, &storages, &latches);
 }

 if (!compoundunits.IsEmpty())
 {
  out=spaces + "(SUBTYPE UNIT";
  pos=compoundunits.GetHeadPosition();
  while (pos!=NULL)
  {
   out+=" " + compoundunits.GetNext(pos);
  }

  file->Write(out, out.GetLength());

  if (units.IsEmpty())
  {
   out=") ;actually compound units\n";
   file->Write(out, out.GetLength());
  }
 }

 if (!units.IsEmpty())
 {
  if (compoundunits.IsEmpty())
  {
   out=spaces + "(SUBTYPE UNIT";
   file->Write(out, out.GetLength());
  }

  pos=units.GetHeadPosition();
  out="";

  while (pos!=NULL)
  {
   out+=" " + units.GetNext(pos);
  }

  // add the architecture unit and the control unit
  out+=" ArchUnit ControlUnit ";

  out+=")\n";
  file->Write(out, out.GetLength());
 }

 if (!ports.IsEmpty())
 {
  out=spaces + "(SUBTYPE PORT";
  pos=ports.GetHeadPosition();
  while (pos!=NULL)
  {
   out+=" " + ports.GetNext(pos);
  }
  out+=")\n";
  file->Write(out, out.GetLength());
 }

 if (!connections.IsEmpty())
 {
  out=spaces + "(SUBTYPE CONNECTION";
  pos=connections.GetHeadPosition();
  while (pos!=NULL)
  {
   out+=" " + connections.GetNext(pos);
  }
  out+=")\n";
  file->Write(out, out.GetLength());
 }

 if (!storages.IsEmpty())
 {
  out=spaces + "(SUBTYPE STORAGE";
  pos=storages.GetHeadPosition();
  while (pos!=NULL)
  {
   out+=" " + storages.GetNext(pos);
  }
 // out+=")\n";
 // file->Write(out, out.GetLength());
 }

 if (!latches.IsEmpty())
 {
 // out=spaces + "(SUBTYPE LATCH";
  pos=latches.GetHeadPosition();
  while (pos!=NULL)
  {
   out+=" " + latches.GetNext(pos);
  }
  out+=")\n";
  file->Write(out, out.GetLength());
 }

 out="\n";
 file->Write(out, out.GetLength());
}

void SIMDoc::UpdatePipelineStages()
{
 POSITION pos=pipelineStages.GetHeadPosition();
 while (pos!=NULL)
 {
  ((SIMGraphPipelineStage*)(pipelineStages.GetNext(pos)))->ComputeHandles();  
 }

}
