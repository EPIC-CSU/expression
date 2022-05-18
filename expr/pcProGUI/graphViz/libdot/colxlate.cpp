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

#include "stdafx.h"
#include "dot.h"

#ifndef NOCOLORNAMES
#include "colortbl.h"

char *
canoncolor(char *orig, char *out)
{
	char	c,*p = out;
	while ((c = *orig++)) {
		if (isalnum(c) == FALSE) continue;
		if (isupper(c)) c = tolower(c);
		*out++ = c;
	}
	*out = c;
	return p;
}

int
colorcmpf(void *p0, void *p1)
{
	int		i = (((hsbcolor_t*)p0)->name[0] - ((hsbcolor_t*)p1)->name[0]);
	return (i ? i : strcmp(((hsbcolor_t*)p0)->name,((hsbcolor_t*)p1)->name));
}

char *
colorxlate(char *str, char *buf)
{
	static	hsbcolor_t	*last;
	char				*p,canon[SMALLBUF];
	hsbcolor_t			fake;

	if ((last == NULL)||(last->name[0] != str[0])||(strcmp(last->name,str))) {
		fake.name = canoncolor(str,canon);
		last = (hsbcolor_t*) bsearch((void*)&fake,(void*)color_lib,sizeof(color_lib)/sizeof(hsbcolor_t),sizeof(fake),(bsearch_cmpf)colorcmpf);
	}
	if (last == NULL) {
		if (isdigit(canon[0]) == FALSE) {
			fprintf(stderr,"warning: %s is not a known color\n",str);
			strcpy(buf,str);
		}
		else for (p = buf; (*p = *str++); p++) if (*p == ',') *p = ' ';
	}
	else sprintf(buf,"%.3f %.3f %.3f",((double)last->h)/255,((double)last->s)/255,((double)last->b)/255);
	return buf;
}
#else
char * colorxlate(char *str, char *buf) {return str;}
#endif

void hsv2rgb(double *r, double *g, double *b, double h, double s, double v)
{
	int i;
	double f,p,q,t;

	if (s <= 0.0) {	/* achromatic */
		*r = v;
		*g = v;
		*b = v;
	}
	else {
		if (h >= 1.0) h = 0.0;
		h = 6.0 * h;
		i = (int)h;
		f = h - (double)i;
		p = v * (1 - s);
		q = v * (1 - (s * f));
		t = v * ( 1 - (s * (1 - f)));
		switch (i) {
			case 0: *r = v; *g = t; *b = p; break;
			case 1: *r = q; *g = v; *b = p; break;
			case 2: *r = p; *g = v; *b = t; break;
			case 3: *r = p; *g = q; *b = v; break;
			case 4: *r = t; *g = p; *b = v; break;
			case 5: *r = v; *g = p; *b = q; break;
		}
	}
}
