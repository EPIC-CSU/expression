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
// VarGroup.cpp: implementation of the VarGroup class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\pcprogui.h"
#include "VarGroup.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_SERIAL( VarGroup, CObject, 1 )

// Fast Hashing of Variable-Length Text Strings
// Peter K. Pearson (Communications of ACM 33(6), June 1990)

UINT VarGroup::RandVals[STSIZE]= 
{
 1, 87, 49, 12, 176, 178, 102, 166, 121, 193, 6, 84, 249, 230, 44, 163,
 14, 197, 213, 181, 161, 65, 218, 80, 64, 239, 24, 226, 236, 142, 38, 200,
 110, 177, 104, 103, 141, 253, 255, 50, 77, 101, 81, 18, 45, 96, 31, 222,
 25, 107, 190, 70, 86, 237, 240, 34, 72, 242, 20, 214, 244, 227, 149, 235,
 97, 234, 57, 22, 60, 250, 82, 175, 208, 5, 127, 199, 111, 62, 135, 248,
 174, 169, 211, 58, 66, 154, 106, 195, 245, 171, 17, 187, 182, 179, 0, 243,
 132, 56, 148, 75, 128, 133, 158, 100, 130, 126, 91, 13, 153, 246, 216, 219,
 119, 68, 223, 78, 83, 88, 201, 99, 122, 11, 92, 32, 136, 114, 52, 10,
 138, 30, 48, 183, 156, 35, 61, 26, 143, 74, 251, 94, 129, 162, 63, 152,
 170, 7, 115, 167, 241, 206, 3, 150, 55, 59, 151, 220, 90, 53, 23, 131,
 125, 173, 15, 238, 79, 95, 89, 16, 105, 137, 225, 224, 217, 160, 37, 123,
 118, 73, 2, 157, 46, 116, 9, 145, 134, 228, 207, 212, 202, 215, 69, 229,
 27, 188, 67, 124, 168, 252, 42, 4, 29, 108, 21, 247, 19, 205, 39, 203,
 233, 40, 186, 147, 198, 192, 155, 33, 164, 191, 98, 204, 165, 180, 117, 76,
 140, 36, 210, 172, 41, 54, 159, 8, 185, 232, 113, 196, 231, 47, 146, 120,
 51, 65, 28, 144, 254, 221, 93, 189, 194, 139, 112, 43, 71, 109, 184, 209
};
 
template<> UINT AFXAPI HashKey(CString& key)
{
 const char* s=(LPCSTR)key;
 UINT val = 0;
 
 for (int i=(int)*s++;i;i=(int)*s++)
 {
  val=VarGroup::RandVals[val^i];
 }
 return val; 
}

VarGroup::VarGroup()
{

}

VarGroup::~VarGroup()
{

}

void VarGroup::Serialize(CArchive& ar)
{
 int count=values.GetCount();

 if (ar.IsStoring())
 {
  ar << name << count;

 // for(POSITION i=types.GetHeadPosition();i!=NULL;types.GetNext(i))
 // {
 //  ar << types.GetAt(i);
 // }  

  ar << types.GetAt(types.GetHeadPosition());

  for(POSITION i=values.GetHeadPosition();i!=NULL;values.GetNext(i))
  {
   ar << values.GetAt(i);
  }  
 }
 else
 {
  CString str;
  ar >> name >> count;
  values.RemoveAll();
  types.RemoveAll();

   ar >> str;
   types.AddTail(str);

  for (int i=0;i<count;i++)
  {
   ar >> str;
   values.AddTail(str);
  }
 }
}

/////

IMPLEMENT_SERIAL( VarGroups, CObject, 1 )

VarGroups::VarGroups()
{

}

VarGroups::~VarGroups()
{

}

void VarGroups::Serialize(CArchive& ar)
{
 int count;
 VarGroup group;

 if (ar.IsStoring())
 {
  count=groups.GetCount();
  ar << count;

  POSITION pos=groups.GetStartPosition();
  while (pos!=NULL)
  {
   CString key;
   groups.GetNextAssoc(pos,key,group);  
   group.Serialize(ar);
  }
 }
 else
 {
  ar >> count;
  groups.RemoveAll();
  for (int i=0;i<count;i++)
  {
   group.Serialize(ar);
   groups.SetAt(group.name,group);
  }
 }
}

void VarGroups::xGen(CFile* file, int level)
{
 if (groups.GetCount()<1) return;

 CString spaces(_T(' '), level);
 CString out;

 out=spaces+"(VAR_GROUPS\n";
 file->Write(out, out.GetLength());

 //output all groups
 POSITION pos=groups.GetStartPosition();
 while (pos!=NULL)
 {
  VarGroup group;
  CString key, key2;
  groups.GetNextAssoc(pos,key,group);  
  //expand values
  //changed by Sudeep
  key="REGS";

  for(POSITION i=group.values.GetHeadPosition();i!=NULL;group.values.GetNext(i))
  {
		key+=" " + group.values.GetAt(i);
  }

//  for(i=group.types.GetHeadPosition();i!=NULL;group.types.GetNext(i))
//  {
   key2+=" " + group.types.GetAt(group.types.GetHeadPosition());
//  }

  out.Format(spaces+" (%s\t(DATATYPE %s)\t(%s))\n", group.name, key2, key);
  file->Write(out, out.GetLength());
 }

 out=spaces+")\n\n";
 file->Write(out, out.GetLength());
}
