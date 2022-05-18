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
// OpGroups.h: interface for the OpGroups class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPGROUPS_H__F6318D0E_D68B_11D2_BBD1_00C04FB17504__INCLUDED_)
#define AFX_OPGROUPS_H__F6318D0E_D68B_11D2_BBD1_00C04FB17504__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define OG_GROUP 0
#define OG_OP 1

#include <afxtempl.h>

class OG : public CObject  
{
public:
	CString& operandToText(int operand);
 DECLARE_SERIAL( OG ) //add support for serialization

 bool isGroup();
	virtual void OnDraw(HDC hDC, RECT* rect);
	virtual void xGen(CFile* file, int level);
 virtual void Serialize(CArchive& ar);

 OG(int type = OG_GROUP, int lvl = 0, CString* txt = NULL);
	virtual ~OG();

 static int nextID;
 int id;

 int type; // OG_GROUP or OG_OP

 bool isExpanded; //only for a group

 bool isSelected;
 int level; //the level at which this item is in the tree
 CString text;

 //data for an op
	CString behavior;
	int opType;

 int check1;
 int check2;
 int check3;

	int operand11;
	int operand12;
	int operand13;
	int operand14;
	int operand21;
	int operand22;
	int operand23;
	int operand24;
	int operand31;
	int operand32;
	int operand33;
	int operand34;

	CString operandType11;
	CString operandType12;
	CString operandType13;
	CString operandType14;
	CString operandType21;
	CString operandType22;
	CString operandType23;
	CString operandType24;
	CString operandType31;
	CString operandType32;
	CString operandType33;
	CString operandType34;

	CString edit2;
	CString edit3;

 OG& operator= (OG& ogi)
 {
  //copy data members
  type=ogi.type;
  isExpanded=ogi.isExpanded;
  isSelected=ogi.isSelected;
  level=ogi.level;
  text=ogi.text;

  //now the op specific data members
	 behavior=ogi.behavior;
	 opType=ogi.opType;

  check1=ogi.check1;
  check2=ogi.check2;
  check3=ogi.check3;

	 operand11=ogi.operand11;
	 operand12=ogi.operand12;
	 operand13=ogi.operand13;
	 operand14=ogi.operand14;
	 operand21=ogi.operand21;
	 operand22=ogi.operand22;
	 operand23=ogi.operand23;
	 operand24=ogi.operand24;
	 operand31=ogi.operand31;
	 operand32=ogi.operand32;
	 operand33=ogi.operand33;
	 operand34=ogi.operand34;

	 operandType11=ogi.operandType11;
	 operandType12=ogi.operandType12;
	 operandType13=ogi.operandType13;
	 operandType14=ogi.operandType14;
	 operandType21=ogi.operandType21;
	 operandType22=ogi.operandType22;
	 operandType23=ogi.operandType23;
	 operandType24=ogi.operandType24;
	 operandType31=ogi.operandType31;
	 operandType32=ogi.operandType31;
	 operandType33=ogi.operandType33;
	 operandType34=ogi.operandType34;

	 edit2=ogi.edit2;
	 edit3=ogi.edit3;

  return *this;
 }

};

typedef CList<OG, OG&> OGList;

class OpGroups : public CObject
{
public:
	POSITION xGenGroup(CFile* file, int level, POSITION pos);
	void DeleteOps();
	void DeleteGroups();
	POSITION FindItem(int searchID);
	virtual void xGen(CFile* file, int level);
 virtual void Serialize(CArchive& ar);

 DECLARE_SERIAL( OpGroups ) //add support for serialization

 OpGroups();
	virtual ~OpGroups();

 OGList opGroupItemList;
};

#endif // !defined(AFX_OPGROUPS_H__F6318D0E_D68B_11D2_BBD1_00C04FB17504__INCLUDED_)
