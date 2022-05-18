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
// VarGroup.h: interface for the VarGroup class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VARGROUP_H__F6318C87_D68B_11D2_BBD1_00C04FB17504__INCLUDED_)
#define AFX_VARGROUP_H__F6318C87_D68B_11D2_BBD1_00C04FB17504__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <afxtempl.h>

typedef CList<CString,CString&> StringList;

#define STSIZE 256

class VarGroup : public CObject  
{
public:
 DECLARE_SERIAL( VarGroup ) //add support for serialization
	VarGroup();
	virtual ~VarGroup();

 static UINT RandVals[STSIZE];
 CString name;
 StringList values, types;

 VarGroup& operator= (VarGroup& vg)
 {
  //copy data members
  name=vg.name;
  values.RemoveAll();
  types.RemoveAll();
  for(POSITION i=vg.values.GetHeadPosition();i!=NULL;vg.values.GetNext(i))
  {
   values.AddTail(vg.values.GetAt(i));
  }

  for(i=vg.types.GetHeadPosition();i!=NULL;vg.types.GetNext(i))
  {
   types.AddTail(vg.types.GetAt(i));
  }

  
  return *this;
 }

 virtual void Serialize(CArchive& ar);
};


typedef CMap<CString, CString&, VarGroup, VarGroup&> VarGroupMap;

template<> UINT AFXAPI HashKey(CString& key);


class VarGroups : public CObject  
{
public:
 DECLARE_SERIAL( VarGroups ) //add support for serialization
	VarGroups();
	virtual ~VarGroups();

 VarGroupMap groups;
 virtual void Serialize(CArchive& ar);
	virtual void xGen(CFile* file, int level);
};



#endif // !defined(AFX_VARGROUP_H__F6318C87_D68B_11D2_BBD1_00C04FB17504__INCLUDED_)
