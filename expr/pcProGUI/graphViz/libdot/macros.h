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
/*
 * Copyright (c) AT&T Corp. 1994, 1995.
 * This code is licensed by AT&T Corp.  For the
 * terms and conditions of the license, see
 * http://www.research.att.com/orgs/ssr/book/reuse
 */

#define NOT(v)		(!(v))
#ifndef FALSE
#define	FALSE		0
#endif
#ifndef TRUE
#define TRUE		NOT(FALSE)
#endif
#define NEW(t)		 (t*)zmalloc(sizeof(t))
#define N_NEW(n,t)	 (t*)zmalloc((n)*sizeof(t))
#define ALLOC(size,ptr,type) (ptr? (type*)realloc(ptr,(size)*sizeof(type)):(type*)malloc((size)*sizeof(type)))
#define ZALLOC(size,ptr,type,osize) (ptr? (type*)zrealloc(ptr,size,sizeof(type),osize):(type*)calloc((size),sizeof(type)))
#define MIN(a,b)	((a)<(b)?(a):(b))
#define MAX(a,b)	((a)>(b)?(a):(b))
#define ABS(a)		((a) >= 0 ? (a) : -(a))
#ifndef MAXINT
#define	MAXINT		((int)(~(unsigned)0 >> 1))
#endif
#ifndef MAXSHORT
#define	MAXSHORT	(0x7fff)
#endif
#define BETWEEN(a,b,c)	(((a) <= (b)) && ((b) <= (c)))
#define ROUND(f)		(round(f))
#define RADIANS(deg)	((deg)/180.0 * PI)
#define DEGREES(rad)	((rad)/PI * 180.0)
#define DIST(x1,y1,x2,y2) (sqrt(((x1) - (x2))*((x1) - (x2)) + ((y1) - (y2))*((y1) - (y2))))
#define POINTS_PER_INCH	72.0
#define POINTS(f_inch)	(ROUND(f_inch*POINTS_PER_INCH))
#define PS2INCH(ps)		((ps)/POINTS_PER_INCH)

#define is_virtual(n)	((n)->u.node_type == VIRTUAL)
#ifndef streq
#define streq(s,t)		(!strcmp((s),(t)))
#endif
