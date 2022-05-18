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
#ifndef STD_TYPES_H
#define STD_TYPES_H 1

#ifndef WIN32
#include <iostream.h>
#include <string.h>
#include <stdio.h>
#endif


#ifdef WIN32
#include <string.h>
#include <stdio.h>
#endif

#ifndef NULL
#define NULL 0
#endif
#define null NULL

#ifndef WIN32
typedef int boolean;
#define true 1
#define false 0
#endif
typedef char* string;
typedef int integer;
typedef char character;

#define not(b) ((b)?false:true)

#define is_in_range(v, l, u) (l <= v && v <= u)


//NOS 
#define streq(s, t) strcmp(s,t)==0
//inline boolean streq(string s, string t)
//{
//  return strcmp(s,t)==0;
//}

inline string concat(string s, string t)
{
  char tempstr[100];
  sprintf(tempstr,"%s%s",s,t);
  return strdup(tempstr);
}

#ifndef WIN32
inline integer max(integer x, integer y)
{
 return x<y ? y : x;
}
#endif

#endif


