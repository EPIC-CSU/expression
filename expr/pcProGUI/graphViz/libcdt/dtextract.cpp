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

/*	Extract objects of a dictionary.
**
**	Written by Kiem-Phong Vo (5/25/96).
*/

#if __STD_C
Dtlink_t* dtextract(reg Dt_t* dt)
#else
Dtlink_t* dtextract(dt)
reg Dt_t*	dt;
#endif
{
	reg Dtlink_t	*list, **s, **ends;

	if(dt->data->type&(DT_OSET|DT_OBAG) )
		list = dt->data->here;
	else if(dt->data->type&(DT_SET|DT_BAG))
	{	list = dtflatten(dt);
		for(ends = (s = dt->data->htab) + dt->data->ntab; s < ends; ++s)
			*s = NIL(Dtlink_t*);
	}
	else /*if(dt->data->type&(DT_LIST|DT_STACK|DT_QUEUE))*/
	{	list = dt->data->head;
		dt->data->head = NIL(Dtlink_t*);
	}

	dt->data->type &= ~DT_FLATTEN;
	dt->data->size = 0;
	dt->data->here = NIL(Dtlink_t*);

	return list;
}
