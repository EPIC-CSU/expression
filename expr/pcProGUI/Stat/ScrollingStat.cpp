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
// ScrollingStat.cpp: implementation of the ScrollingStat class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../pcprogui.h"
#include "ScrollingStat.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ScrollingStat::ScrollingStat()
{
 memo="scrolling stat";

 color= RGB((int)((float)255*(float)rand()/(float)RAND_MAX),
            (int)((float)255*(float)rand()/(float)RAND_MAX),
            (int)((float)255*(float)rand()/(float)RAND_MAX));

 //allocate the data buffer
 vals=new int[width];
 //memset(vals, 0, sizeof(int)*width);

 //TEMP
 maxVal=(int)((float)60*(float)rand()/(float)RAND_MAX)+10;
 height=maxVal-minVal+1+STAT_SEPARATOR;

 //TEMP
 for (int i=0;i<width;i++)
 {
  vals[i]=(int)((float)maxVal*(float)rand()/(float)RAND_MAX);
 }
}

ScrollingStat::~ScrollingStat()
{
 delete vals;
}


void ScrollingStat::OnDraw(HDC hDC, RECT* rect)
{
 CPen newPen;
 newPen.CreatePen(PS_SOLID, 1, color);

 HGDIOBJ oldPen = SelectObject(hDC,newPen);
 MoveToEx(hDC, rect->left, rect->bottom - (vals[0] - minVal) - STAT_SEPARATOR, NULL);
 for (int i=1;i<width;i++)
 {
  LineTo(hDC, rect->left+i, rect->bottom - (vals[i] - minVal) - STAT_SEPARATOR);
 }

 SelectObject(hDC,oldPen);
}


void ScrollingStat::Notify()
{
 //shift data to the left
 memmove(vals, vals+1, sizeof(int)*(width-1));
 vals[width-1]=updateThisInt;
 //add new value (TEMP)
 //vals[width-1]=(int)((float)maxVal*(float)rand()/(float)RAND_MAX);
}
