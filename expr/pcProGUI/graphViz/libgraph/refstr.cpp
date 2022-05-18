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
 * Copyright (c) AT&T Corp. and Lucent Corp. 1994-97.
 * This code is licensed by AT&T Corp. and Lucent Corp.
 * For the terms and conditions of the license, see
 * http://www.research.att.com/orgs/ssr/book/reuse
 */


#include "stdafx.h"
#include	"libgraph.h"

extern Dtmethod_t*	Dttree;

typedef struct refstr_t {
	Dtlink_t		link;
	unsigned int	refcnt;
	char			s[1];
} refstr_t;

static Dtdisc_t Refstrdisc = {
	offsetof(refstr_t,s[0]),
	0,
	0,
	NIL(Dtmake_f),
	NIL(Dtfree_f),
	NIL(Dtcompar_f),			/* use strcmp */
	NIL(Dthash_f),
	NIL(Dtmemory_f),
	NIL(Dtevent_f)
};

static Dict_t*	StringDict;

#ifdef DEBUG
static int refstrprint(refstr_t* r)
{
	fprintf(stderr,"%s\n",r->s); return 0;
}

agrefstrdump(void)
{
	dtwalk(StringDict,refstrprint);
}
#endif

static void initialize_strings(void)
{
	StringDict	= dtopen(&Refstrdisc,Dttree);
}

char *agstrdup(char* s)
{
	refstr_t		*key,*r;

	if (StringDict == NULL) initialize_strings();
	if (s == NULL) return s;

	key = (refstr_t*)(s - offsetof(refstr_t,s[0]));
	r = (refstr_t*) dtsearch(StringDict,key);
	if (r) r->refcnt++;
	else {
		r = (refstr_t*) malloc(sizeof(refstr_t)+strlen(s));
		r->refcnt = 1;
		strcpy(r->s,s);
		dtinsert(StringDict,r);
	}
	return r->s;
}

void agstrfree(char* s)
{
	refstr_t		*key,*r;

	if ((StringDict == NULL) || (s == NULL)) return;
	key = (refstr_t*)(s - offsetof(refstr_t,s[0]));
	r = (refstr_t*) dtsearch(StringDict,key);

	if (r) {
		r->refcnt--;
		if (r->refcnt <= 0) dtdelete(StringDict,r);
	}
	else fprintf(stderr,"agstrfree lost %s\n",s);
}
