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

/*	Return the # of objects in the dictionary
**
**	Written by Kiem-Phong Vo (5/25/96)
*/

#if __STD_C
static int treecount(reg Dtlink_t* e)
#else
static int treecount(e)
reg Dtlink_t*	e;
#endif
{	return e ? treecount(e->left) + treecount(e->right) + 1 : 0;
}

#if __STD_C
int dtsize(Dt_t* dt)
#else
int dtsize(dt)
Dt_t*	dt;
#endif
{
	reg Dtlink_t*	t;
	reg int		size;

	UNFLATTEN(dt);

	if(dt->data->size < 0) /* !(dt->data->type&(DT_SET|DT_BAG)) */
	{	if(dt->data->type&(DT_OSET|DT_OBAG))
			dt->data->size = treecount(dt->data->here);
		else if(dt->data->type&(DT_LIST|DT_STACK|DT_QUEUE))
		{	for(size = 0, t = dt->data->head; t; t = t->right)
				size += 1;
			dt->data->size = size;
		}
	}

	return dt->data->size;
}
