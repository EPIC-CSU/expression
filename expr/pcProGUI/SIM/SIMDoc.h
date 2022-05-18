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
#if !defined(AFX_SIMDOC_H__1CCD8336_1CC5_11D2_B73B_00C04FB17504__INCLUDED_)
#define AFX_SIMDOC_H__1CCD8336_1CC5_11D2_B73B_00C04FB17504__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SIMDoc.h : header file
//

//#include "routine.h"
//#include "../MouseHitInfo.h"

#include <afxtempl.h>

class SIMGraphObj;
typedef CList<SIMGraphObj*,SIMGraphObj*> SIMGraphObjList;
typedef CList<CString,CString&> StringList;

class GUIBaseStat;
typedef CList<GUIBaseStat*,GUIBaseStat*> GUIBaseStatList;

//#include "../Stat/DefaultStatProp.h"

class Routine;

//#include "..\Ops\ISInfo.h"

/////////////////////////////////////////////////////////////////////////////
// SIMDoc document

class SIMDoc : public CDocument
{
protected:
	SIMDoc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(SIMDoc)

// Attributes
public:
 static int storeVersion; //version used to save GMD files
 static int loadVersion; //version used to load current GMD file (0.0 = invalid)

 float scaleFactor;
	int xBound;
	int yBound;

 //ISInfo isInfo;
 void* isInfo;

 SIMGraphObjList pipelineStages;
 SIMGraphObjList simObjs;
 //SIMGraphObjList units;
 //SIMGraphObjList compoundunits;
 //SIMGraphObjList storages;
 StringList dataPaths;
 GUIBaseStatList simStatList;

// Operations
public:
 SIMGraphObj* SIMDoc::getHit(CPoint & point);

private:

public:
 void PatchPointers();
 SIMGraphObj* GetSIMGraphObjByName(char* name);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SIMDoc)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	void UpdatePipelineStages();
	void OutputSubtypeDefs(CFile* file, int level);
	static void SerializeSIMGraphObjList(SIMGraphObjList& objList, CArchive& ar);
 static void genOSIMGraphObjList(SIMGraphObjList& objList, CFile* file, int level);
	virtual ~SIMDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(SIMDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SIMDOC_H__1CCD8336_1CC5_11D2_B73B_00C04FB17504__INCLUDED_)
