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
#ifndef __HOST_H__
#define __HOST_H__


#define SYMCAT(X,Y)	X##Y

/* host-dependent canonical type definitions */
typedef int bool_t;					/* generic boolean type */
typedef unsigned char byte_t;		/* byte - 8 bits */
typedef signed char sbyte_t;
typedef unsigned short half_t;		/* half - 16 bits */
typedef signed short shalf_t;
typedef unsigned int word_t;		/* word - 32 bits */
typedef signed int sword_t;
typedef float sfloat_t;				/* single-precision float - 32 bits */
typedef double dfloat_t;			/* double-precision float - 64 bits */

/* DEBUG defined - dump instructions */
/* level 1 of debugging - dump writes */
/* level 2 of debugging - reads too */
#define DEBUG	2

#if DEBUG >= 1
#define DBG_MSG1(PAR1)								\
	printf(PAR1)
#define DBG_MSG2(PAR1, PAR2)						\
	printf(PAR1, PAR2)
#define DBG_MSG3(PAR1, PAR2, PAR3)					\
	printf(PAR1, PAR2, PAR3)
#else
#define DBG_MSG1(PAR1)						1
#define DBG_MSG2(PAR1, PAR2)				1
#define DBG_MSG3(PAR1, PAR2, PAR3)			1
#endif


#if DEBUG >= 2
#define DBG_MSG11(PAR1)								\
	printf(PAR1)
#define DBG_MSG21(PAR1, PAR2)						\
	printf(PAR1, PAR2)
#define DBG_MSG31(PAR1, PAR2, PAR3)					\
	printf(PAR1, PAR2, PAR3)
#else
#define DBG_MSG11(PAR1)						1
#define DBG_MSG21(PAR1, PAR2)				1
#define DBG_MSG31(PAR1, PAR2, PAR3)			1
#endif


#endif	/* __HOST_H__ */