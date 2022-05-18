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

/*	Change search method.
**
**	Written by Kiem-Phong Vo (05/25/96)
*/

#if __STD_C
Dtmethod_t* dtmethod(Dt_t* dt, Dtmethod_t* meth)
#else
Dtmethod_t* dtmethod(dt, meth)
Dt_t*		dt;
Dtmethod_t*	meth;
#endif
{
	reg Dtlink_t	*list, *r;
	reg Dtdisc_t*	disc = dt->disc;
	reg Dtmethod_t*	oldmeth = dt->meth;

	if(!meth || meth->type == oldmeth->type)
		return oldmeth;

	if(disc->eventf &&
	   (*disc->eventf)(dt,DT_METH,(Void_t*)meth,disc) < 0)
		return NIL(Dtmethod_t*);

	/* get the list of elements */
	list = dtflatten(dt);

	if(dt->data->type&(DT_LIST|DT_STACK|DT_QUEUE) )
		dt->data->head = NIL(Dtlink_t*);
	else if(dt->data->type&(DT_SET|DT_BAG) )
	{	if(dt->data->ntab > 0)
			(*dt->memoryf)(dt,(Void_t*)dt->data->htab,0,disc);
		dt->data->ntab = 0;
		dt->data->htab = NIL(Dtlink_t**);
	}

	dt->data->here = NIL(Dtlink_t*);
	dt->data->type = (dt->data->type&~(DT_METHODS|DT_FLATTEN)) | meth->type;
	dt->meth = meth;
	if(dt->searchf == oldmeth->searchf)
		dt->searchf = meth->searchf;

	if(meth->type&(DT_LIST|DT_STACK|DT_QUEUE) )
	{	if(!(oldmeth->type&(DT_LIST|DT_STACK|DT_QUEUE)) )
		{	if((r = list) )
			{	reg Dtlink_t*	t;
				for(t = r->right; t; r = t, t = t->right )
					t->left = r;
				list->left = r;
			}
		}
		dt->data->head = list;
	}
	else if(meth->type&(DT_OSET|DT_OBAG))
	{	dt->data->size = 0;
		while(list)
		{	r = list->right;
			(*meth->searchf)(dt,(Void_t*)list,DT_RENEW);
			list = r;
		}
	}
	else if(!((meth->type&DT_BAG) && (oldmeth->type&DT_SET)) )
	{	int	rehash;
		if((meth->type&(DT_SET|DT_BAG)) && !(oldmeth->type&(DT_SET|DT_BAG)))
			rehash = 1;
		else	rehash = 0;

		dt->data->size = 0;
		while(list)
		{	r = list->right;
			if(rehash)
			{	reg Void_t* key = OBJ(list,disc->link);
				key = KEY(key,disc->key,disc->size);
				list->hash = HASH(dt,key,disc,disc->size);
			}
			(void)(*meth->searchf)(dt,(Void_t*)list,DT_RENEW);
			list = r;
		}
	}

	return oldmeth;
}
