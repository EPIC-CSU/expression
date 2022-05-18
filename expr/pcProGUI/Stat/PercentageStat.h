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
// PercentageStat.h: interface for the PercentageStat class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PERCENTAGESTAT_H__6BFBD489_D002_11D2_BBCB_00C04FB17504__INCLUDED_)
#define AFX_PERCENTAGESTAT_H__6BFBD489_D002_11D2_BBCB_00C04FB17504__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "GUIBaseStat.h"

class PercentageStat : public GUIBaseStat  
{
public:
	void Notify();
	void OnDraw(HDC hDC, RECT* rect);
	PercentageStat();
	virtual ~PercentageStat();

 int currentVal;

};

#endif // !defined(AFX_PERCENTAGESTAT_H__6BFBD489_D002_11D2_BBCB_00C04FB17504__INCLUDED_)
