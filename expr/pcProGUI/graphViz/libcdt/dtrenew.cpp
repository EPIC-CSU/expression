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


/*	Renew a single object which must be at current finger.
**
**	Written by Kiem-Phong Vo (5/25/96)
*/

#if __STD_C
Void_t* dtrenew(Dt_t* dt, reg Void_t* obj)
#else
Void_t* dtrenew(dt, obj)
Dt_t*		dt;
reg Void_t*	obj;
#endif
{
	reg Void_t*	key;
	reg Dtlink_t	*e, *r, *t, **s;
	reg Dtdisc_t*	disc = dt->disc;

	UNFLATTEN(dt);

	if(!(e = dt->data->here) || OBJ(e,disc->link) != obj)
		return NIL(Void_t*);

	if(dt->data->type&(DT_STACK|DT_QUEUE|DT_LIST))
		return obj;
	else if(dt->data->type&(DT_OSET|DT_OBAG) )
	{	if(!(r = e->right) )
			dt->data->here = e->left;
		else if(!(t = r->left) )
		{	r->left = e->left;
			dt->data->here = r;
		}
		else	/* make smallest right tree node the new root */
		{	for(; t->left; r = t, t = t->left)
				;
			r->left = NIL(Dtlink_t*);
			t->right = e->right;
			t->left = e->left;
			dt->data->here = t;
		}
	}
	else /*if(dt->data->type&(DT_SET|DT_BAG))*/
	{	s = dt->data->htab + HINDEX(dt->data->ntab,e->hash);
		if((t = *s) == e)
			*s = e->right;
		else
		{	for(; t->right != e; t = t->right)
				;
			t->right = e->right;
		}
		key = KEY(obj,disc->key,disc->size);
		e->hash = HASH(dt,key,disc,disc->size);
		dt->data->here = NIL(Dtlink_t*);
	}

	dt->data->size -= 1;
	return (*dt->meth->searchf)(dt,(Void_t*)e,DT_RENEW) ? obj : NIL(Void_t*);
}
