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
// OpGroups.cpp: implementation of the OpGroupItem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\pcprogui.h"
#include "OpGroups.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define INDENT_LEVEL 15

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_SERIAL( OG, CObject, 1 )

int OG::nextID=0;

OG::OG(int typ, int lvl, CString* txt)
{
 id=nextID++;
 isExpanded=false;
 isSelected=false;
 type=typ;
 level=lvl;
 if (txt!=NULL) text=*txt; 

 opType=-1;

 check1=0;
 check2=0;
 check3=0;

	operand11=-1;
	operand12=-1;
	operand13=-1;
	operand14=-1;

	operand21=-1;
	operand22=-1;
	operand23=-1;
	operand24=-1;

	operand31=-1;
	operand32=-1;
	operand33=-1;
	operand34=-1;
}

OG::~OG()
{

}


bool OG::isGroup()
{
 return type==OG_GROUP;
}

void OG::Serialize(CArchive& ar)
{
 if (ar.IsStoring())
 {
  ar << type << level << text;
  ar << behavior << opType;
  ar << edit2 << edit3;
  ar << check1 << check2 << check3;
  ar << operand11 << operand12 << operand13 << operand14 << operand21 << operand22 << operand23 << operand24 << operand31 << operand32 << operand33 << operand34;
 	ar << operandType11 << operandType12 << operandType13 << operandType14 << operandType21 << operandType22 << operandType23 << operandType24 << operandType31 << operandType32 << operandType33 << operandType34;
 }
 else
 {
  ar >> type >> level >> text;
  ar >> behavior >> opType;
  ar >> edit2 >> edit3;
  ar >> check1 >> check2 >> check3;
  ar >> operand11 >> operand12 >> operand13 >> operand14 >> operand21 >> operand22 >> operand23 >> operand24 >> operand31 >> operand32 >> operand33 >> operand34;
 	ar >> operandType11 >> operandType12 >> operandType13 >> operandType14 >> operandType21 >> operandType22 >> operandType23 >> operandType24 >> operandType31 >> operandType32 >> operandType33 >> operandType34;
 }
}

CString& OG::operandToText(int operand)
{
 static CString operandString;
 switch(operand)
 {
 case 1: operandString="_DEST_"; break;
 case 2: operandString="_MEM_DEST_"; break;
 case 3: operandString="_MEM_SRC_"; break;
 case 4: operandString="_REG_DEST_"; break;
 case 5: operandString="_REG_SRC_"; break;
 case 6: operandString="_SOURCE_1_"; break;
 case 7: operandString="_SOURCE_2_"; break;
 case 8: operandString="_SOURCE_3_"; break;


 default: ASSERT(0);break;
 }

 return operandString;
}


void OG::xGen(CFile* file, int level)
{
 CString spaces(_T(' '), level);
 CString out;

 if (isGroup())
 {
  ASSERT(0);
  /*
  out=spaces+"(OP_GROUP " + text + "\n";
  file->Write(out, out.GetLength());

  //from ++pos list ll the opcodes until the level goes back to level of item at this level
  POSITION i=

  out=spaces+")\n\n";
  file->Write(out, out.GetLength());
  */
 }
 else
 {
  out=spaces+"(OPCODE " + text + "\n";
  file->Write(out, out.GetLength());
 
  //the optype
  if (opType>0)
  {
   out=spaces+" (OP_TYPE ";
   switch(opType)
   {
   case 0: out+="CONTROL_OP)\n"; break;
   case 1: out+="DATA_OP)\n"; break;
   case 2: out+="FLOW_OP)\n";break;
   default:
    //ASSERT(0);
    break;
   }
   file->Write(out, out.GetLength());
  }

  //operands if any
  if (check1)
  {
   out=spaces+" (OPERANDS";
   if (operand11>0) out+=" (" + operandToText(operand11) + " " + operandType11 + ")";
   if (operand12>0) out+=" (" + operandToText(operand12) + " " + operandType12 + ")";
   if (operand13>0) out+=" (" + operandToText(operand13) + " " + operandType13 + ")";
   if (operand14>0) out+=" (" + operandToText(operand14) + " " + operandType14 + ")";
   out+=")\n";
   file->Write(out, out.GetLength());
  }

  if (check2)
  {
   out=spaces+" (OPERANDS";
   if (operand21>0) out+=" (" + operandToText(operand21) + " " + operandType21 + ")";
   if (operand22>0) out+=" (" + operandToText(operand22) + " " + operandType22 + ")";
   if (operand23>0) out+=" (" + operandToText(operand23) + " " + operandType23 + ")";
   if (operand24>0) out+=" (" + operandToText(operand24) + " " + operandType24 + ")";
   out+=")\n";
   file->Write(out, out.GetLength());
  }

  if (check3)
  {
   out=spaces+" (OPERANDS";
   if (operand31>0) out+=" (" + operandToText(operand31) + " " + operandType31 + ")";
   if (operand32>0) out+=" (" + operandToText(operand32) + " " + operandType32 + ")";
   if (operand33>0) out+=" (" + operandToText(operand33) + " " + operandType33 + ")";
   if (operand34>0) out+=" (" + operandToText(operand34) + " " + operandType34 + ")";
   out+=")\n";
   file->Write(out, out.GetLength());
  }

  //behavior if any
  if (behavior!="")
  {
   out=spaces+" (BEHAVIOR \"" + behavior + "\")\n";
   file->Write(out, out.GetLength());
  }

  //asm dump format if any
  if (edit3!="")
  {
   out=spaces+" (ASMFORMAT " + edit3 + "\n"+spaces+")\n";
   file->Write(out, out.GetLength());
  }

  //ir dump format if any
  if (edit2!="")
  {
   out=spaces+" (IRDUMPFORMAT " + edit2 + "\n"+spaces + ")\n";
   file->Write(out, out.GetLength());
  }

  out=spaces+")\n\n";
  file->Write(out, out.GetLength());
 }
}

void OG::OnDraw(HDC hDC, RECT * rect)
{
 //RECT r;
 //memcpy(&r, rect, sizeof(RECT));
 //
 //r.left+=level*INDENT_LEVEL;
 //
 SetBkMode(hDC, TRANSPARENT);
 //DrawText(hDC, text, text.GetLength(), &r, DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_LEFT | DT_EXPANDTABS );
 DrawText(hDC, text, text.GetLength(), rect, DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_LEFT | DT_EXPANDTABS );
}


///////////////////////////////////////////////////////////////

IMPLEMENT_SERIAL( OpGroups, CObject, 1 )

OpGroups::OpGroups()
{

}

OpGroups::~OpGroups()
{

}

void OpGroups::Serialize(CArchive& ar)
{
 //save/load the list of OpGroupItems
 int count=opGroupItemList.GetCount();

 if (ar.IsStoring())
 {
  ar << count;
  for(POSITION i=opGroupItemList.GetHeadPosition();i!=NULL;opGroupItemList.GetNext(i))
  {
   opGroupItemList.GetAt(i).Serialize(ar);
  }  
 }
 else
 {
  OG ogi;
  ar >> count;
  opGroupItemList.RemoveAll();
  for (int i=0;i<count;i++)
  {
   ogi.Serialize(ar);
   opGroupItemList.AddTail(ogi);
  }
 }

}

POSITION OpGroups::FindItem(int searchID)
{
 for(POSITION i=opGroupItemList.GetHeadPosition();i!=NULL;opGroupItemList.GetNext(i))
 {
  if (opGroupItemList.GetAt(i).id==searchID) 
  {
   return i;
  }
 }  

 return NULL;
}

void OpGroups::DeleteGroups()
{
	// lots of bugs because of this function!
	// GetAt and GetNext should not be passed value 0

 for(POSITION i=opGroupItemList.GetHeadPosition();i!=NULL;opGroupItemList.GetNext(i))
 {
  if ((opGroupItemList.GetAt(i).isSelected) && (opGroupItemList.GetAt(i).isGroup()))
  {
   int level=opGroupItemList.GetAt(i).level;
   POSITION newPos=i;
   if (newPos == NULL)
	   break;
   opGroupItemList.GetNext(newPos);
   if (newPos != NULL)
   {
	   OG* og=&opGroupItemList.GetAt(newPos);
	   while ((newPos!=NULL) && (og->level>level))
	   {
		   POSITION oldPos=newPos;
		   if (newPos == NULL)
			   break;
		   opGroupItemList.GetNext(newPos);
		   if (newPos!=NULL) og=&opGroupItemList.GetAt(newPos);
		   opGroupItemList.RemoveAt(oldPos);
	   }
   }
   opGroupItemList.RemoveAt(i);
   //lazy 
   i=opGroupItemList.GetHeadPosition();
   if (i == NULL)
	   break;
  }
 }  
}

void OpGroups::DeleteOps()
{
 for(POSITION i=opGroupItemList.GetHeadPosition();i!=NULL;opGroupItemList.GetNext(i))
 {
  if ((opGroupItemList.GetAt(i).isSelected) && (!opGroupItemList.GetAt(i).isGroup()))
  {
   opGroupItemList.RemoveAt(i);
   //lazy 
   i=opGroupItemList.GetHeadPosition();
  }
 }  
}


void OpGroups::xGen(CFile* file, int level)
{
 if (opGroupItemList.GetCount()<1) return;

 CString spaces(_T(' '), level);
 CString out;
 
 //first generate a dummy group containing all the ops with their full definitions
 out=spaces+"(OP_GROUP _all_ops_\n";
 file->Write(out, out.GetLength());
 
 POSITION i;

 for(i=opGroupItemList.GetHeadPosition();i!=NULL;opGroupItemList.GetNext(i))
 {
  if (!opGroupItemList.GetAt(i).isGroup())
  {
   opGroupItemList.GetAt(i).xGen(file,level+1);
  }
 }  

 out=spaces+")\n\n";
 file->Write(out, out.GetLength());

 //now just generate the hierarchical groups with just the opcodes

 i=opGroupItemList.GetHeadPosition();
 while (i!=NULL)
 {
  if (opGroupItemList.GetAt(i).isGroup())
  {
   i=xGenGroup(file,level,i);
  }
  else
  {
   opGroupItemList.GetNext(i);
  }
 }  
 
}



POSITION OpGroups::xGenGroup(CFile * file, int level, POSITION pos)
{
 CString spaces(_T(' '), level);
 CString out;

 OG* cog=&opGroupItemList.GetAt(pos);
 //opGroupItemList.GetAt(i).xGen(file,level+1, i);
 out=spaces+"(OP_GROUP " + cog->text + "\n";
 //file->Write(out, out.GetLength());
 
 //from ++pos list all the opcodes until the level goes back to level of item at pos
 POSITION i=pos;
 opGroupItemList.GetNext(i);
 
 if (i!=NULL)
 {
  out+=spaces+" (OPCODE";
  OG* og=&opGroupItemList.GetAt(i);
  while ((i!=NULL) && (og->level>cog->level))
  {
   out+=" " + og->text;
   if (og->isGroup()) 
   {
    i=xGenGroup(file, level, i);
   }
   else
   {
    opGroupItemList.GetNext(i);
   }
   if (i!=NULL) og=&opGroupItemList.GetAt(i);
  }
  out+=")\n";
  file->Write(out, out.GetLength());
 }

 out=spaces+")\n\n";
 file->Write(out, out.GetLength());

 return i;
}

