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
#if !defined(AFX_HTGDOC_H__1CCD8336_1CC5_11D2_B73B_00C04FB17504__INCLUDED_)
#define AFX_HTGDOC_H__1CCD8336_1CC5_11D2_B73B_00C04FB17504__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// HTGDoc.h : header file
//

#include "../MouseHitInfo.h"

class Routine;

/////////////////////////////////////////////////////////////////////////////
// HTGDoc document

class HTGDoc : public CDocument
{
protected:
	HTGDoc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(HTGDoc)

// Attributes
public:
 CString m_graphText;
 Routine* m_routine;

 float scaleFactor;
	int xBound;
	int yBound;

 MouseHitInfo hitHTG;

// Operations
public:
 void GenerateGraphDoc();//Routine* _routine);
 MouseHitInfo* HTGDoc::getHit(CPoint & point);

private:

public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(HTGDoc)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~HTGDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(HTGDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HTGDOC_H__1CCD8336_1CC5_11D2_B73B_00C04FB17504__INCLUDED_)
