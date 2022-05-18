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
#ifndef _GENERATERT_
#define _GENERATERT_

#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include "symbols.h"
#include "node.h"

#include <map>
#include <list>

#ifndef __UNIX__
#include <process.h>
#else
#define __declspec(dllexport)
#define __stdcall
#define _ASSERT assert
#include <stdio.h>
#endif

extern const int MAX_PIPELENGTH;
extern __declspec(dllexport) int PIPELENGTH;

#ifdef __UNIX__
        #define ASSERT assert
	#define _ASSERT assert
#endif

//The reservation table type
typedef std::map<int,NodeList*> ResTab;

#endif
