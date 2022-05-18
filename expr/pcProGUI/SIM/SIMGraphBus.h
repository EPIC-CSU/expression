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
// SIMGraphBus.h: interface for the SIMGraphBus class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SIMGRAPHBUS_H__7F8073DE_FCD0_11D2_BBF0_00C04FB17504__INCLUDED_)
#define AFX_SIMGRAPHBUS_H__7F8073DE_FCD0_11D2_BBF0_00C04FB17504__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "SIMGraphObj.h"

class SIMGraphBus : public SIMGraphObj  
{
public:
 DECLARE_SERIAL( SIMGraphBus ) //add support for serialization
	SIMGraphBus();
 SIMGraphBus(SIMGraphObj* pParent, CRect* pRect = NULL);
	virtual ~SIMGraphBus();

 //PROMIS
 virtual void genO(CFile* file, int level);
 virtual void genC(CFile* file, int level);
 //PROMIS

	virtual SIMGraphObj* GetSIMGraphObjByName(CString* testName);
 virtual void Serialize(CArchive& ar);
	virtual void OnDraw(CDC* pDC);
 virtual void OnDragDraw(CDC* pDC, CPoint& offset, SIMGraphObj* parent = NULL);
 virtual SIMGraphObj* getHit(CPoint& point);
 virtual void PatchPointers();

 SIMGraphObjList connections; //connections that belong to this obj
 SIMGraphObjList ports;
 CString customProperties;

 virtual void AddConnection(SIMGraphObj* obj);
 virtual void RemoveConnection(SIMGraphObj* obj);
 virtual void _AddConnection(SIMGraphObj* obj);
 virtual void AddPort(SIMGraphObj* obj);
 virtual void RemovePort(SIMGraphObj* obj);
	virtual void Offset(CPoint& point, SIMGraphObj* end = NULL); 
 virtual void AddSubtype(StringList* pCompoundunits, StringList* pUnits, StringList* pPorts, StringList* pConnections, StringList* pStorages, StringList* pLatches);

 virtual void xArch(CFile* file, int level);
 virtual void xPipe(CFile* file, SIMGraphObj* obj);
};

#endif // !defined(AFX_SIMGRAPHBUS_H__7F8073DE_FCD0_11D2_BBF0_00C04FB17504__INCLUDED_)
