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
#ifndef _STD_HDR_H
#define _STD_HDR_H

#include <stdio.h>
// #include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <malloc.h>
#ifndef WIN32
#include <alloca.h>
#include <std.h>
#endif
#include <iostream.h>
#ifndef WIN32
#include <strstream.h>
#endif
//#include <String.h>
#include <string.h>
#include <assert.h>

#ifndef TRUE
const int TRUE = 1;
#endif

#ifndef TRUE
const int FALSE = 0;
#endif

#ifndef WIN32
#include <sys/time.h>
#else
#include <time.h>
#endif
#ifndef CLOCKS_PER_SEC
#define CLOCKS_PER_SEC 1e6
#endif

const char tab = '\t';

extern void panic ( char *fmt, ... );
extern void warning ( char *fmt, ... );
//extern void error(char *why);

#ifndef linux
struct qelem {
	struct    qelem *q_forw;
	struct    qelem *q_back;
	char *q_data;
};

// extern void bcopy(void *b1, void *b2, int length); 

extern "C" 
{
#ifndef WIN32
	extern void bcopy(char *b1, char *b2, int length); 
	extern void bzero(char *b, int length);
	extern int bcmp(char *b1, char *b2, int length);
#else
	#include <memory.h> 
	#define bcopy(src,dst,size) memcpy(src,dst,size)
	#define bzero(dst,size) memcpy(dst,'\0',size)
	#define bcmp(b1,b2,length) memcmp(b1,b2,length)
#endif

	extern int ffs (int);
	extern void insque(struct qelem *elem, struct qelem *pred);
	extern void remque(struct qelem *elem);
#ifndef WIN32
	extern clock_t clock(void);
#endif
}
#endif

#endif /*STDHDR_H */

