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
// SIMGraphConnection.h: interface for the SIMGraphConnection class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SIMGRAPHCONNECTION_H__7D269C5B_B647_11D2_BBB8_00C04FB17504__INCLUDED_)
#define AFX_SIMGRAPHCONNECTION_H__7D269C5B_B647_11D2_BBB8_00C04FB17504__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "SIMGraphObj.h"

class SIMGraphConnection : public SIMGraphObj  
{
public:
	virtual SIMGraphObj* GetSIMGraphObjByName(CString* testName);
 DECLARE_SERIAL( SIMGraphConnection ) //add support for serialization
	virtual void Offset(CPoint& point, SIMGraphObj* end = NULL);

	SIMGraphConnection();
	SIMGraphConnection(SIMGraphObj* pParent, CRect* pRect, SIMGraphObj* pStartObj, CPoint* pStartObjPoint, SIMGraphObj* pEndObj, CPoint* pEndObjPoint);
	virtual ~SIMGraphConnection();

 SIMGraphObj* startObj;
 SIMGraphObj* endObj;

 CRect rectStart;
 CRect rectEnd;

 CString customProperties;

 //PROMIS
 virtual void genO(CFile* file, int level);
 virtual void genC(CFile* file, int level);
 //PROMIS

 virtual void Serialize(CArchive& ar);
 virtual void _AddConnection(SIMGraphObj* obj);
 virtual void OnDraw(CDC * pDC);
 virtual void OnDragDraw(CDC* pDC, CPoint& offset, SIMGraphObj* parent = NULL);
 virtual SIMGraphObj* getHit(CPoint& point);
 virtual void PatchPointers();
 virtual void AddSubtype(StringList* pCompoundunits, StringList* pUnits, StringList* pPorts, StringList* pConnections, StringList* pStorages, StringList* pLatches);
 virtual void xArch(CFile* file, int level);
};

#endif // !defined(AFX_SIMGRAPHCONNECTION_H__7D269C5B_B647_11D2_BBB8_00C04FB17504__INCLUDED_)
