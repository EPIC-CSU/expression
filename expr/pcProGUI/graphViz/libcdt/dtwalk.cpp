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

/*	Walk a dictionary and all dictionaries viewed through it.
**	userf:	user function
**
**	Written by Kiem-Phong Vo (5/25/96)
*/

#if __STD_C
int dtwalk(reg Dt_t* dt, int (*userf)(Dt_t*, Void_t*, Void_t*), Void_t* data)
#else
int dtwalk(dt,userf,data)
reg Dt_t*	dt;
int(*		userf)();
Void_t*		data;
#endif
{
	reg Void_t	*obj, *next;
	reg Dt_t*	walk;
	reg int		rv;

	for(obj = dtfirst(dt); obj; )
	{	if(!(walk = dt->walk) )
			walk = dt;
		next = dtnext(dt,obj);
		if((rv = (*userf)(walk, obj, data )) < 0)
			return rv;
		obj = next;
	}
	return 0;
}
