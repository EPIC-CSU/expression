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
// InstrDescr.cpp: implementation of the InstrDescr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\pcprogui.h"
#include "InstrDescr.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_SERIAL( Slot, CObject, 1 )

Slot::Slot()
{
}

Slot::~Slot()
{
}

void Slot::Serialize(CArchive& ar)
{
 if (ar.IsStoring())
 {
  ar << type << bitwidth << unit << pos;
 }
 else
 {
  ar >> type >> bitwidth >> unit >> pos;
 }
}

////

IMPLEMENT_SERIAL( InstrDescr, CObject, 1 )

InstrDescr::InstrDescr()
{
 wordlen=0;
}

InstrDescr::~InstrDescr()
{

}

void InstrDescr::Serialize(CArchive& ar)
{
 int count;
 Slot slot;

 if (ar.IsStoring())
 {
  count=slots.GetCount();
  ar << wordlen << count;

  POSITION pos=slots.GetStartPosition();
  while (pos!=NULL)
  {
   int key;
   slots.GetNextAssoc(pos,key,slot);  
   slot.Serialize(ar);
  }
 }
 else
 {
  ar >> wordlen >> count;
  slots.RemoveAll();
  for (int i=0;i<count;i++)
  {
   slot.Serialize(ar);
   //slots.SetAt(slot.unit,slot);
   slots.SetAt(slot.pos,slot);
  }
 }
}

void InstrDescr::xGen(CFile* file, int level)
{
 if (slots.GetCount()<1) return;

 CString spaces(_T(' '), level);
 CString out;

 //out=spaces+"(INSTR\n";
 //file->Write(out, out.GetLength());

 out.Format(spaces+" (WORDLEN %d)\n", wordlen);
 file->Write(out, out.GetLength());

 out=spaces+" (SLOTS\n";
 file->Write(out, out.GetLength());

 //output all slots
 int count=slots.GetCount();
 for (int i=0;i<count;i++)
 //POSITION pos=slots.GetStartPosition();
 //while (pos!=NULL)
 {
  Slot slot;
  //int key;
  //slots.GetNextAssoc(pos,key,slot);  

  if (slots.Lookup(i, slot))
  {
   out.Format(spaces+"  ((TYPE %s) (BITWIDTH %d) (UNIT %s))\n", slot.type, slot.bitwidth, slot.unit);
   file->Write(out, out.GetLength());
  }
 }

 out=spaces+" )\n";
 file->Write(out, out.GetLength());

 //out=spaces+")\n";
 //file->Write(out, out.GetLength());
 
}
