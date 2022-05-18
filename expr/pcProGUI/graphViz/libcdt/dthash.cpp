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
#include "stdafx.h"

#define _MYDEFS

#include	"dthdr.h"

#undef _MYDEFS

/*	Hash table.
**	dt:	dictionary
**	obj:	what to look for
**	type:	type of search
**
**      Written by Kiem-Phong Vo (05/25/96)
*/

/* resize the hash table */
#if __STD_C
static void dthtab(Dt_t* dt)
#else
static void dthtab(dt)
Dt_t*	dt;
#endif
{
	reg Dtlink_t	*t, *r, *p, **s, **hs, **is, **olds;
	reg int		n;

	/* compute new table size */
	if((n = dt->data->ntab) == 0)
		n = HSLOT;
	while(dt->data->size > HLOAD(n))
		n = HRESIZE(n);
	if(n <= dt->data->ntab)
		return;

	/* allocate new table */
	olds = dt->data->ntab == 0 ? NIL(Dtlink_t**) : dt->data->htab;
	if(!(s = (Dtlink_t**)(*dt->memoryf)(dt,olds,n*sizeof(Dtlink_t*),dt->disc)) )
		return;
	olds = s + dt->data->ntab;
	dt->data->htab = s;
	dt->data->ntab = n;

	/* rehash elements */
	for(hs = s+n-1; hs >= olds; --hs)
		*hs = NIL(Dtlink_t*);
	for(hs = s; hs < olds; ++hs)
	{	for(p = NIL(Dtlink_t*), t = *hs; t; t = r)
		{	r = t->right;
			if((is = s + HINDEX(n,t->hash)) == hs)
				p = t;
			else	/* move to a new chain */
			{	if(p)
					p->right = r;
				else	*hs = r;
				t->right = *is; *is = t;
			}
		}
	}
}

#if __STD_C
static Void_t* dthash(Dt_t* dt, reg Void_t* obj, int type)
#else
static Void_t* dthash(dt,obj,type)
Dt_t*		dt;
reg Void_t*	obj;
int		type;
#endif
{
	reg Dtlink_t	*t, *r, *p;
	reg Void_t	*k, *key;
	reg uint	hsh;
	reg int		lk, sz, ky;
	reg Dtcompar_f	cmpf;
	reg Dtdisc_t*	disc;
	reg Dtlink_t	**s, **ends;

	UNFLATTEN(dt);
	INITDISC(dt,disc,ky,sz,lk,cmpf);

	if(!obj)
	{	if(dt->data->size <= 0 || !(type&(DT_CLEAR|DT_FIRST|DT_LAST)) )
			return NIL(Void_t*);

		ends = (s = dt->data->htab) + dt->data->ntab;
		if(type&DT_CLEAR)
		{	/* clean out all objects */
			for(; s < ends; ++s)
			{	t = *s;
				*s = NIL(Dtlink_t*);
				if(!disc->freef && disc->link >= 0)
					continue;
				while(t)
				{	r = t->right;
					if(disc->freef)
						(*disc->freef)(dt,OBJ(t,lk),disc);
					if(disc->link < 0)
						(*dt->memoryf)(dt,(Void_t*)t,0,disc);
					t = r;
				}
			}
			dt->data->here = NIL(Dtlink_t*);
			dt->data->size = 0;
			return NIL(Void_t*);
		}
		else	/* computing the first/last object */
		{	t = NIL(Dtlink_t*);
			while(s < ends && !t )
				t = (type&DT_LAST) ? *--ends : *s++;
			if(t && (type&DT_LAST))
				for(; t->right; t = t->right)
					;

			dt->data->here = t;
			return t ? OBJ(t,lk) : NIL(Void_t*);
		}
	}

	if(type&(DT_MATCH|DT_SEARCH|DT_INSERT) )
	{	key = (type&DT_MATCH) ? obj : KEY(obj,ky,sz);
		hsh = HASH(dt,key,disc,sz);
		goto do_search;
	}
	else if(type&DT_RENEW)
	{	r = (Dtlink_t*)obj;
		obj = OBJ(r,lk);
		key = KEY(obj,ky,sz);
		hsh = r->hash;
		goto do_search;
	}
	else /*if(type&(DT_DELETE|DT_NEXT|DT_PREV))*/
	{	if((t = dt->data->here) && OBJ(t,lk) == obj)
		{	hsh = t->hash;
			s = dt->data->htab + HINDEX(dt->data->ntab,hsh);
			p = NIL(Dtlink_t*);
		}
		else
		{	key = KEY(obj,ky,sz);
			hsh = HASH(dt,key,disc,sz);
		do_search:
			t = dt->data->ntab <= 0 ? NIL(Dtlink_t*) :
			 	*(s = dt->data->htab + HINDEX(dt->data->ntab,hsh));
			for(p = NIL(Dtlink_t*); t; p = t, t = t->right)
			{	if(hsh == t->hash)
				{	k = OBJ(t,lk); k = KEY(k,ky,sz);
					if(CMP(dt,key,k,disc,cmpf,sz) == 0)
						break;
				}
			}
		}
	}

	if(type&(DT_MATCH|DT_SEARCH))
	{	if(!t)
			return NIL(Void_t*);
		if(p && (dt->data->type&DT_SET) )
		{	p->right = t->right;
			t->right = *s;
			*s = t;
		}
		dt->data->here = t;
		return OBJ(t,lk);
	}
	else if(type&DT_INSERT)
	{	if(t && (dt->data->type&DT_SET) )
		{	dt->data->here = t;
			return OBJ(t,lk);
		}

		if(disc->makef && !(obj = (*disc->makef)(dt,obj,disc)) )
			return NIL(Void_t*);
		if(lk >= 0)
			r = ELT(obj,lk);
		else
		{	r = (Dtlink_t*)(*dt->memoryf)
				(dt,NIL(Void_t*),sizeof(Dthold_t),disc);
			if(r)
				((Dthold_t*)r)->obj = obj;
			else
			{	if(disc->makef && disc->freef)
					(*disc->freef)(dt,obj,disc);
				return NIL(Void_t*);
			}
		}
		r->hash = hsh;

		/* insert object */
	do_insert:
		if((dt->data->size += 1) > HLOAD(dt->data->ntab))
			dthtab(dt);
		if(dt->data->ntab == 0)
		{	dt->data->size -= 1;
			return NIL(Void_t*);
		}
		s = dt->data->htab + HINDEX(dt->data->ntab,hsh);
		if(t)
		{	r->right = t->right;
			t->right = r;
		}
		else
		{	r->right = *s;
			*s = r;
		}
		dt->data->here = r;
		return obj;
	}
	else if(type&DT_NEXT)
	{	if(t && !(t = t->right) )
		{	for(ends = dt->data->htab+dt->data->ntab, s += 1; s < ends; ++s)
				if((t = *s) )
					break;
		}
		dt->data->here = t;
		return t ? OBJ(t,lk) : NIL(Void_t*);
	}
	else if(type&DT_RENEW)
	{	if(!t || (dt->data->type&DT_BAG) )
			goto do_insert;
		else
		{	if(disc->freef)
				(*disc->freef)(dt,obj,disc);
			if(disc->link >= 0)
				(*dt->memoryf)(dt,(Void_t*)r,0,disc);
			return t ? OBJ(t,lk) : NIL(Void_t*);
		}
	}
	else if(type&DT_PREV)
	{	if(t && !p)
		{	if((p = *s) != t)
			{	while(p->right != t)
					p = p->right;
			}
			else
			{	p = NIL(Dtlink_t*);
				for(s -= 1, ends = dt->data->htab; s >= ends; --s)
				{	if((p = *s) )
					{	while(p->right)
							p = p->right;
						break;
					}
				}
			}
		}
		dt->data->here = p;
		return p ? OBJ(p,lk) : NIL(Void_t*);
	}
	else /*if(type&DT_DELETE)*/
	{	if(!t)
			return NIL(Void_t*);
		else if(p)
			p->right = t->right;
		else if((p = *s) == t)
			*s = t->right;
		else
		{	while(p->right != t)
				p = p->right;
			p->right = t->right;
		}
		obj = OBJ(t,lk);
		dt->data->size -= 1;
		dt->data->here = p;
		if(disc->freef)
			(*disc->freef)(dt,obj,disc);
		if(disc->link < 0)
			(*dt->memoryf)(dt,(Void_t*)t,0,disc);
		return obj;
	}
}

static Dtmethod_t	_Dtset = { dthash, DT_SET };
static Dtmethod_t	_Dtbag = { dthash, DT_BAG };
Dtmethod_t*		Dtset = &_Dtset;
Dtmethod_t*		Dtbag = &_Dtbag;

#ifndef KPVDEL	/* for backward compatibility - remove next time */
Dtmethod_t		_Dthash = { dthash, DT_SET };
Dtmethod_t*		Dthash = &_Dthash;
#endif
