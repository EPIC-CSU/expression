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
// InstrDescr.h: interface for the InstrDescr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INSTRDESCR_H__F6318C86_D68B_11D2_BBD1_00C04FB17504__INCLUDED_)
#define AFX_INSTRDESCR_H__F6318C86_D68B_11D2_BBD1_00C04FB17504__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <afxtempl.h>

class Slot : public CObject
{
public:
 DECLARE_SERIAL( Slot ) //add support for serialization
 Slot();
 virtual ~Slot();

 int pos;
 CString type;
 long bitwidth;
 CString unit;
 virtual void Serialize(CArchive& ar);

 Slot& operator= (Slot& vg)
 {
  //copy data members
  type=vg.type;
  bitwidth=vg.bitwidth;
  unit=vg.unit;
  pos=vg.pos;
  return *this;
 }

};

//typedef CMap<CString, CString&, Slot, Slot&> SlotMap;
typedef CMap<int, int&, Slot, Slot&> SlotMap;

template<> UINT AFXAPI HashKey(CString& key);

class InstrDescr : public CObject  
{
public:
 DECLARE_SERIAL( InstrDescr ) //add support for serialization
	InstrDescr();
	virtual ~InstrDescr();

 long wordlen;
 SlotMap slots;
 virtual void Serialize(CArchive& ar);
	virtual void xGen(CFile* file, int level);
};

#endif // !defined(AFX_INSTRDESCR_H__F6318C86_D68B_11D2_BBD1_00C04FB17504__INCLUDED_)
