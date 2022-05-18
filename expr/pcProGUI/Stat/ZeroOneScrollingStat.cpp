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
// ZeroOneScrollingStat.cpp: implementation of the ZeroOneScrollingStat class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../pcprogui.h"
#include "ZeroOneScrollingStat.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define ZOOM 30

ZeroOneScrollingStat::ZeroOneScrollingStat()
{
 memo="0/1 scrolling stat";

 color= RGB((int)((float)255*(float)rand()/(float)RAND_MAX),
            (int)((float)255*(float)rand()/(float)RAND_MAX),
            (int)((float)255*(float)rand()/(float)RAND_MAX));

 //allocate the data buffer
 vals=new int[width];
 memset(vals, 0, sizeof(int)*width);

 maxVal=1;
 height=ZOOM * (maxVal-minVal)+1+2*STAT_SEPARATOR;

 //TEMP
 //for (int i=0;i<width;i++)
 //{
 // vals[i]= rand()>RAND_MAX/4 ? 0 : 1;
 //}
 //TEMP
}

ZeroOneScrollingStat::~ZeroOneScrollingStat()
{
 delete vals;
}


void ZeroOneScrollingStat::OnDraw(HDC hDC, RECT* rect)
{
 CPen newPen;
 newPen.CreatePen(PS_SOLID, 1, color);

 HGDIOBJ oldPen = ::SelectObject(hDC,newPen);

 MoveToEx(hDC, rect->left, rect->bottom - ZOOM * vals[0] - STAT_SEPARATOR, NULL);
 LineTo(hDC, rect->left, rect->bottom - ZOOM * vals[0] - 1 - STAT_SEPARATOR);
 for (int i=1;i<width;i++)
 {
  if (vals[i]==0)
  {
   if (vals[i-1]==0) // 0->0
   {
    MoveToEx(hDC, rect->left+i, rect->bottom - STAT_SEPARATOR, NULL);
    LineTo(hDC, rect->left+i, rect->bottom - 1 - STAT_SEPARATOR);
   }
   else // 0->1
   {
    MoveToEx(hDC, rect->left+i, rect->bottom - STAT_SEPARATOR, NULL);
    LineTo(hDC, rect->left+i, rect->bottom - ZOOM - 2 - STAT_SEPARATOR);
   }
  }
  else
  {
   if (vals[i-1]==1) //1->1
   {
    MoveToEx(hDC, rect->left+i, rect->bottom - ZOOM - 1 - STAT_SEPARATOR, NULL);
    LineTo(hDC, rect->left+i, rect->bottom - ZOOM - STAT_SEPARATOR);
   }
   else //1->0
   {
    MoveToEx(hDC, rect->left+i, rect->bottom - ZOOM - 1 - STAT_SEPARATOR, NULL);
    LineTo(hDC, rect->left+i, rect->bottom + 1 - STAT_SEPARATOR);
   }
  }
 }

 ::SelectObject(hDC,oldPen);

}


void ZeroOneScrollingStat::Notify()
{
 //shift data to the left
 memmove(vals, vals+1, sizeof(int)*(width-1));
 vals[width-1]=updateThisInt;
 //add new value (TEMP)
 //vals[width-1]= rand()>RAND_MAX/4 ? 0 : 1;
}
