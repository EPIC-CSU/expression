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
#ifndef _DTHDR_H
#define _DTHDR_H	1
#ifndef _BLD_cdt
#define _BLD_cdt	1
#endif

/*	Internal definitions for libcdt.
**	Written by Kiem-Phong Vo (5/25/96)
*/

#if _PACKAGE_ast
#include	<ast.h>
#else
#include <stdio.h>
#endif

#include	"FEATURE/cdt"
#include	"cdt.h"

/* short-hand notations */
#define reg	register
#define uint	unsigned int
#define left	hl._left
#define hash	hl._hash
#define htab	hh._htab
#define head	hh._head

/* this must be disjoint from DT_METHODS */
#define DT_FLATTEN	010000	/* dictionary already flattened	*/

/* bits used for secondary hashing */
#define HIGHBIT		(~(((uint)~0) >> 1))
#define SETVISIT(h)	((h) | HIGHBIT)
#define CLRVISIT(h)	((h) & ~HIGHBIT)
#define ISVISIT(h)	((h) & HIGHBIT)
#define REHASH(h)	(((h)<<5) - (h))

/* hash start size and load factor */
#define HSLOT		(32)
#define HRESIZE(n)	((n) << 1)
#define HLOAD(s)	((s) << 1)
#define HINDEX(n,h)	((h)&((n)-1))

#define UNFLATTEN(dt) \
		((dt->data->type&DT_FLATTEN) ? dtrestore(dt,NIL(Dtlink_t*)) : 0)

/* the pointer to the actual object */
#define INITDISC(dt,d,ky,sz,lk,cmpf) \
		(d = dt->disc, ky = d->key, sz = d->size, lk = d->link, cmpf = d->comparf)
#define ELT(o,lk)	((Dtlink_t*)((char*)(o) + lk) )
#define OBJ(e,lk)	(lk < 0 ? ((Dthold_t*)(e))->obj : (Void_t*)((char*)(e) - lk) )
#define KEY(o,ky,sz)	((Void_t*)(sz < 0 ? *((char**)((char*)(o) + ky)) : \
					    ((char*)(o) + ky) ) )

/* compare and hash functions */
#define CMP(dt,k1,k2,d,cmpf,sz) \
		(cmpf ? (*cmpf)(dt,k1,k2,d) : \
		 	 sz <= 0 ? strcmp((char*)k1,(char*)k2) : memcmp(k1,k2,sz) )
#define HASH(dt,k,d,sz)	(d->hashf ? (*d->hashf)(dt,k,d) : dtstrhash(0,k,sz) )

/* tree rotation functions */
#define RROTATE(x,y)	((x)->left = (y)->right, (y)->right = (x), (x) = (y))
#define LROTATE(x,y)	((x)->right = (y)->left, (y)->left  = (x), (x) = (y))
#define RLINK(r,x)	((r) = (r)->left  = (x) )
#define LLINK(l,x)	((l) = (l)->right = (x) )

#if !_PACKAGE_ast
_BEGIN_EXTERNS_
//_kpvimport Void_t*	malloc _ARG_((int));
//_kpvimport Void_t*	realloc _ARG_((Void_t*, int));
//_kpvimport void		free _ARG_((Void_t*));
//_kpvimport int		memcmp _ARG_((const Void_t*, const Void_t*, size_t));
//_kpvimport int		strcmp _ARG_((const char*, const char*));
_END_EXTERNS_
#endif

#endif /* _DTHDR_H */
