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
// SIMGraphObj.h: interface for the SIMGraphObj class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SIMGRAPHOBJ_H__BC1EA199_AFD3_11D2_BBB6_00C04FB17504__INCLUDED_)
#define AFX_SIMGRAPHOBJ_H__BC1EA199_AFD3_11D2_BBB6_00C04FB17504__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <afxtempl.h>

//#include "../MouseHitInfoStructure.h"
//NOS #ifdef GUI_EXE
#include "../../../pcProDll/win32/MouseHitInfoStruct.h"
//NOS #endif
//#include "../Prop/BasePropPage.h"

class SIMGraphObj;
class BaseSimObject;
class GUIBaseStat;

//#include "../../SystemDLL/BaseClassDLL/BaseSimObject.h"
//#include "../../SystemDLL/NewBaseClassDLL/BaseSimObject.h"
//#include "BaseSimObject.h"

//__declspec( dllimport ) BaseSimObject *getComp(char* testStr);

typedef CList<CString,CString&> StringList;
typedef CList<SIMGraphObj*,SIMGraphObj*> SIMGraphObjList;
typedef CList<GUIBaseStat*,GUIBaseStat*> GUIBaseStatList;
typedef CMap<SIMGraphObj*, SIMGraphObj*, SIMGraphObj*, SIMGraphObj*> SIMGraphObjMap;

class SIMGraphObj : public CObject
{
public:
 DECLARE_SERIAL( SIMGraphObj ) //add support for serialization
 static long SIMObjID;
 COLORREF          borderColor;
 CRect             rect;
 CString           name;
 CString           classname;
 MHI_HIT_TYPE      type;
 bool              selected;
 bool              breakpoint; 
 SIMGraphObj*      parent;
 SIMGraphObjList   _connections; //these are connections that just 'rest' one end on this obj (we need this for mouse hit computation)
 BaseSimObject*    simObj;
 GUIBaseStatList   statList;

 static int vScroll;
 static int hScroll;

 void ToggleSelected();

	virtual void OnEndSimulation(void);
	virtual void OnBeginSimulation(void);

 //PROMIS
 virtual void genO(CFile* file, int level);
 virtual void genC(CFile* file, int level);
 //PROMIS

 virtual SIMGraphObj* GetSIMGraphObjByName(CString* testName);
 virtual void Serialize(CArchive& ar);
 virtual void OnDraw(CDC* pDC);
 virtual void OnDragDraw(CDC* pDC, CPoint& offset, SIMGraphObj* parent = NULL);
 virtual void DisplayProperties();

 virtual void AddPort(SIMGraphObj* obj){ASSERT(0);};
 virtual void AddLatch(SIMGraphObj* obj){ASSERT(0);};
 virtual void AddUnit(SIMGraphObj* obj){ASSERT(0);};
 virtual void AddConnection(SIMGraphObj* obj){ASSERT(0);};
 virtual void _AddConnection(SIMGraphObj* obj){ASSERT(0);};
 virtual void __AddLatch(SIMGraphObj* obj){ASSERT(0);};

 virtual void RemovePort(SIMGraphObj* obj){ASSERT(0);};
 virtual void RemoveLatch(SIMGraphObj* obj){ASSERT(0);};
 virtual void RemoveUnit(SIMGraphObj* obj){ASSERT(0);};
 virtual void RemoveConnection(SIMGraphObj* obj){ASSERT(0);};
 virtual void _RemoveConnection(SIMGraphObj* obj);
 virtual void __RemoveLatch(SIMGraphObj* obj){ASSERT(0);};

 virtual void AddStorage(SIMGraphObj* obj){ASSERT(0);};
 virtual SIMGraphObj* getHit(CPoint& point){ASSERT(0); return NULL;};
	virtual void ToggleBreakpoint(void);
	virtual bool PtInObj(CPoint& point);
	virtual void Offset(CPoint& point, SIMGraphObj* end = NULL){ASSERT(0);}; 
 
 static SIMGraphObjMap Ptr2PtrMap;
 static void initPtrMapping();
 static void addPtrMapping(SIMGraphObj* oldPtr, SIMGraphObj* newPtr);
 static SIMGraphObj* getPtrMapping(SIMGraphObj* oldPtr);
 virtual void PatchPointers();
	virtual bool IsAncestorOf(SIMGraphObj* child);
	virtual void AddSubtype(StringList* pCompoundunits, StringList* pUnits, StringList* pPorts, StringList* pConnections, StringList* pStorages, StringList* pLatches);

 //functions used to output various parts of the EXPRESSION description
 virtual void xArch(CFile* file, int level);
 virtual void xPipe(CFile* file, SIMGraphObj* obj);
 virtual void xStor(CFile* file, int level);
 virtual void xData(CFile* file, int level);

protected:

public:
	//virtual void AttachSimulatorObject(BaseSimObject* obj);
 SIMGraphObj();
	SIMGraphObj(SIMGraphObj* pParent, CRect* rect = NULL);
	virtual ~SIMGraphObj();

	static void FitRectangleInRectangle(CRect* outer, CRect* inner);

};

#endif // !defined(AFX_SIMGRAPHOBJ_H__BC1EA199_AFD3_11D2_BBB6_00C04FB17504__INCLUDED_)
