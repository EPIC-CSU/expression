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
// PercentageStat.cpp: implementation of the PercentageStat class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../pcprogui.h"
#include "PercentageStat.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

PercentageStat::PercentageStat()
{
 memo="% stat";

 color= RGB((int)((float)255*(float)rand()/(float)RAND_MAX),
            (int)((float)255*(float)rand()/(float)RAND_MAX),
            (int)((float)255*(float)rand()/(float)RAND_MAX));

 minVal=0;
 maxVal=100;
 currentVal=50;
 height=20;
}

PercentageStat::~PercentageStat()
{

}

void PercentageStat::OnDraw(HDC hDC, RECT * rect)
{
 CPen newPen;
 newPen.CreatePen(PS_SOLID, 1, color);
 CBrush newBrush;
 newBrush.CreateSolidBrush(color);

 HGDIOBJ oldPen = ::SelectObject(hDC,newPen);
 HGDIOBJ oldBrush = ::SelectObject(hDC,newBrush);

 CRect r=*rect;

 r.DeflateRect(2,2,2+(int)((float)width*(float)(100-currentVal)/100.0),3);

 ::Rectangle(hDC, r.left, r.top, r.right, r.bottom);

 CString valText;
 valText.Format("%d%%",currentVal);
 ::SetBkMode(hDC, TRANSPARENT);
 ::SetTextColor(hDC, ~color & 0x00FFFFFF);
 DrawText(hDC, valText, valText.GetLength(), rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_LEFT | DT_EXPANDTABS );

 ::SelectObject(hDC,oldPen);
 ::SelectObject(hDC,oldBrush);
}

void PercentageStat::Notify()
{
 currentVal=updateThisInt;
// //TEMP
// int rVal=rand();
// currentVal+=rVal>RAND_MAX/2 ? 5 : -5;
 if (currentVal<0) currentVal=0;
 if (currentVal>100) currentVal=100;
}
