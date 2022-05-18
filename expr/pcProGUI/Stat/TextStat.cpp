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
// TextStat.cpp: implementation of the TextStat class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../pcprogui.h"
#include "TextStat.h"

#include "../ColorCodes.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TextStat::TextStat()
{
 memo="text stat";
 text="Value = ";
 fullText=text;
 color=YELLOW;
 minVal=0;
 maxVal=20;
 height=maxVal-minVal+1+STAT_SEPARATOR; //STAT_SEPARATOR pixels of border
}

TextStat::~TextStat()
{

}


void TextStat::OnDraw(HDC hDC, RECT* rect)
{
 SetBkMode(hDC, TRANSPARENT);
 SetTextColor(hDC, color);
 DrawText(hDC, fullText, fullText.GetLength(), rect, DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_LEFT | DT_EXPANDTABS );
}


void TextStat::Notify()
{
 if (updateThisStr!=NULL)
 {
  text=CString(updateThisStr);
  updateThisStr=NULL;
 }

 fullText.Format(text + "%d", updateThisInt);
}

