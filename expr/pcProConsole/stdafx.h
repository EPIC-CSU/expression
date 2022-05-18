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
//dummy stdafx.h so this silly thing can compile my_stream.h and console.h
#ifdef WIN32
#pragma warning(disable:4786)
#endif

#ifndef __STDAFXsmall_H__
#define __STDAFXsmall_H__

#ifndef __HUBBA_HUBBA__
#define __HUBBA_HUBBA__
#endif//ndef __HUBBA_HUBBA__

#ifndef __UNIX__
	#include <windows.h>
#else
	#include <stdio.h>
    char *itoa(int value, char *str, int radix);
#endif

#endif	
