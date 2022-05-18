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
// BezierEdge.cpp: implementation of the BezierEdge class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../pcProDllMFC.h"
#include "BezierEdge.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BezierEdge::BezierEdge()
{

}

BezierEdge::~BezierEdge()
{

}


double BezierEdge::atan2pt(POINT p1, POINT p0)
{
 return atan2((double)(p1.y - p0.y),(double)(p1.x - p0.x));
}

int BezierEdge::round(double f)
{
	if (f > 0) 
  return (int)(f + .5) ;
	else 
  return (int)(f - .5);
}


void BezierEdge::Draw(CDC * pDC, BezierEdge* edge)//, int xPos, int yPos)
{
	POINT* pts;
 int res;
 int num_points = edge->points.GetSize();
 pts = new POINT[num_points];
 
 CPen newPen(PS_SOLID,1,edge->lineColor);
 CPen* oldPen = pDC->SelectObject(&newPen);
 
 memcpy(pts,edge->points.GetData(),num_points*sizeof(POINT));
 
 POINT* p=pts;
 /*
 for (int i = 0;i < num_points;i++,p++) 
 {
  p->x -= xPos;
  p->y -= yPos;
 }
 */

 res = pDC->PolyBezier(pts,num_points);
 ASSERT_TRACE(res,"Unable to draw a PolyBezier");
 
 //draw the arrow
	if ((edge->points[0].y>edge->arrow.y) && !edge->isSelfLoop)  //forward or selfloop
  DrawArrow(pDC, edge->arrow, DEGREES(atan2pt(edge->points[0],edge->arrow)));//xPos, yPos);
 else
		DrawArrow(pDC, edge->arrow, DEGREES(atan2pt(edge->points[num_points-1],edge->arrow)));//xPos, yPos);
 
 delete pts;
 
 pDC->SelectObject(oldPen);
}

void BezierEdge::DrawArrow(CDC * pDC, POINT p, double theta)//, int xPos, int yPos)
{
 POINT a[3];
 float ux,vx,uy,vy;
 const double SCALE=1.0;
 
 theta = RADIANS(theta);
 
 ux = (float)(p.x + ARROW_LENGTH*SCALE*cos(theta));
 uy = (float)(p.y + ARROW_LENGTH*SCALE*sin(theta));
 vx = (float)(ARROW_WIDTH/2.*SCALE*cos(theta+PI/2));
 vy = (float)(ARROW_WIDTH/2.*SCALE*sin(theta+PI/2));
 a[0].x = round(ux+vx);//-xPos; 
 a[0].y = round(uy+vy);//-yPos;
 //a[1].x = p.x-xPos;          
 //a[1].y = p.y-yPos;
 a[1]=p;
 a[2].x = round(ux-vx);//-xPos; 
 a[2].y = round(uy-vy);//-yPos;
 
 pDC->Polygon(&a[0],3);
}


