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
#include	"dthdr.h"

/*	Change discipline.
**	dt :	dictionary
**	disc :	discipline
**
**	Written by Kiem-Phong Vo (5/26/96)
*/

#if __STD_C
static Void_t* dtmemory(Dt_t* dt,Void_t* addr,size_t size,Dtdisc_t* disc)
#else
static Void_t* dtmemory(dt, addr, size, disc)
Dt_t* 		dt;	/* dictionary			*/
Void_t* 	addr;	/* address to be manipulate	*/
size_t		size;	/* size to obtain		*/
Dtdisc_t* 	disc;	/* discipline			*/
#endif
{
	if(addr)
	{	if(size == 0)
		{	free(addr);
			return NIL(Void_t*);
		}
		else	return realloc(addr,size);
	}
	else	return size > 0 ? malloc(size) : NIL(Void_t*);
}

#if __STD_C
Dtdisc_t* dtdisc(Dt_t* dt, Dtdisc_t* disc, int type)
#else
Dtdisc_t* dtdisc(dt,disc,type)
Dt_t*		dt;
Dtdisc_t*	disc;
int		type;
#endif
{
	reg Dtsearch_f	searchf;
	reg Dtlink_t	*r, *t;
	reg char*	k;
	reg Dtdisc_t*	old;

	if(!(old = dt->disc) )	/* initialization call from dtopen() */
	{	dt->disc = disc;
		if(!(dt->memoryf = disc->memoryf) )
			dt->memoryf = dtmemory;
		return disc;
	}

	if(!disc)	/* only want to know current discipline */
		return old;

	searchf = dt->meth->searchf;

	UNFLATTEN(dt);

	if(old->eventf && (*old->eventf)(dt,DT_DISC,(Void_t*)disc,old) < 0)
		return NIL(Dtdisc_t*);

	dt->disc = disc;
	if(!(dt->memoryf = disc->memoryf) )
		dt->memoryf = dtmemory;

	if(dt->data->type&(DT_STACK|DT_QUEUE|DT_LIST))
		return old;
	else if(dt->data->type&DT_BAG)
	{	if(type&DT_SAMEHASH)
			return old;
		else	goto dt_renew;
	}
	else if(dt->data->type&(DT_SET|DT_BAG))
	{	if((type&DT_SAMEHASH) && (type&DT_SAMECMP))
			return old;
		else	goto dt_renew;
	}
	else /*if(dt->data->type&(DT_OSET|DT_OBAG))*/
	{	if(type&DT_SAMECMP)
			return old;
	dt_renew:
		r = dtflatten(dt);
		dt->data->type &= ~DT_FLATTEN;
		dt->data->here = NIL(Dtlink_t*);
		dt->data->size = 0;

		if(dt->data->type&(DT_SET|DT_BAG))
		{	reg Dtlink_t	**s, **ends;
			ends = (s = dt->data->htab) + dt->data->ntab;
			while(s < ends)
				*s++ = NIL(Dtlink_t*);
		}

		/* reinsert them */
		while(r)
		{	t = r->right;
			if(!(type&DT_SAMEHASH))	/* new hash value */
			{	k = (char*)OBJ(r,disc->link);
				k = (char*)(KEY((Void_t*)k,disc->key,disc->size));
				r->hash = HASH(dt,k,disc,disc->size);
			}
			(void)(*searchf)(dt,(Void_t*)r,DT_RENEW);
			r = t;
		}
	}

	return old;
}
