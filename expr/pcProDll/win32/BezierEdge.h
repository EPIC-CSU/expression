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
// BezierEdge.h: interface for the BezierEdge class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BEZIEREDGE_H__6E562C2A_28C6_11D2_B74D_00C04FB17504__INCLUDED_)
#define AFX_BEZIEREDGE_H__6E562C2A_28C6_11D2_B74D_00C04FB17504__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <afxtempl.h>
#include <math.h>

#define PI 3.14159265358979323846
#define RADIANS(deg)	((deg)/180.0 * PI)
#define DEGREES(rad)	((rad)/PI * 180.0)

#define		ARROW_LENGTH	10
#define		ARROW_WIDTH		6

typedef CArray<POINT,POINT&> PointArray;

typedef enum {TO_INSTR, TO_BASICBLOCK} TO_EDGE_TYPE;

class BezierEdge  
{
public:
 TO_EDGE_TYPE dest; //used to figure out (using dynamic casting) the object to which this edge points
 //the map key is the void* to that object: dynamic_cast<object_type>(map_key)
 //drawing attributes 
 PointArray points; //describe the bezier curve that denotes the edge
 POINT arrow; //the end/start point used to draw arrows on the edges
	CString style;
	DWORD lineColor; //windows system color id
 bool isSelfLoop;
 //InstrEdge();

 static double atan2pt(POINT p1, POINT p0);
 static int round(double f);

 static void Draw(CDC * pDC, BezierEdge* edge);//, int x_pos, int y_pos);
 static void DrawArrow(CDC * pDC, POINT p, double theta);//, int x_pos, int y_pos);

	BezierEdge();
	virtual ~BezierEdge();

};

typedef CMap<int,int,BezierEdge*,BezierEdge*> BezierEdgeMap;

#endif // !defined(AFX_BEZIEREDGE_H__6E562C2A_28C6_11D2_B74D_00C04FB17504__INCLUDED_)
