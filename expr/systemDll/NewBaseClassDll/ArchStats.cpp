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
/*************************************************************
 *file  : ArchStats.cpp                                      *
 *authors : ashok			                                 *
 *last modified :                                            *
 *                                                           *
 *compiled with : g++                                        *
 *for more info :                                            *
 *************************************************************/

#include "stdafx.h"

#include "StatList.h"
#include "ArchStats.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

#ifdef __UNIX__
	#define __declspec(dllexport) 
#endif

__declspec (dllexport) ArchStatsDB globalArchStatsDB;

StatType getStatType(char *str)
{
	if (str == NULL) return _NOSTAT_;
	if (strcmp(str, "NUMTIMES") == 0) return _NUMTIMES_;
	if (strcmp(str, "HAZARDSTALL") == 0) return _HAZARDSTALL_;
	if (strcmp(str, "TRACKNUMTIMES") == 0) return _TRACKNUMTIMES_;

	return _NOSTAT_;
}
