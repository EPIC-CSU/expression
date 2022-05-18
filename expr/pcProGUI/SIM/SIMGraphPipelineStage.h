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
// SIMGraphPipelineStage.h: interface for the SIMGraphPipelineStage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SIMGRAPHPIPELINESTAGE_H__91E3FBC3_BD4B_11D2_BBBE_00C04FB17504__INCLUDED_)
#define AFX_SIMGRAPHPIPELINESTAGE_H__91E3FBC3_BD4B_11D2_BBBE_00C04FB17504__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "SIMGraphObj.h"

#define DIAG_WIDTH 2000
#define DIAG_HEIGHT 3000

class SIMGraphPipelineStage : public SIMGraphObj  
{
public:
 DECLARE_SERIAL( SIMGraphPipelineStage ) //add support for serialization
	SIMGraphPipelineStage();
	SIMGraphPipelineStage(CWnd* pWindow, CRect* pRect, CPoint* point);
	virtual ~SIMGraphPipelineStage();

 static CWnd* window;
 CPoint leftPoint;
 CPoint rightPoint;
 int yPos; //the y coordinate
 //int yPosScrolled; //the y coordinate

 SIMGraphObjList stageObjs;

 //PROMIS
 virtual void genO(CFile* file, int level);
 virtual void genC(CFile* file, int level);
 //PROMIS

 virtual void Serialize(CArchive& ar);
 virtual void OnDraw(CDC* pDC);
 virtual void OnDragDraw(CDC* pDC, CPoint& offset, SIMGraphObj* parent = NULL);
 virtual SIMGraphObj* getHit(CPoint& point);
	virtual void Offset(CPoint& point, SIMGraphObj* end = NULL); 
 void ComputeHandles();

 static void ComputePipelineStageRects(SIMGraphObjList* stages);
 static void SortPipelineStages(SIMGraphObjList* stages);
 void AddPipelineStageObj(SIMGraphObj* obj);
};

#endif // !defined(AFX_SIMGRAPHPIPELINESTAGE_H__91E3FBC3_BD4B_11D2_BBBE_00C04FB17504__INCLUDED_)
