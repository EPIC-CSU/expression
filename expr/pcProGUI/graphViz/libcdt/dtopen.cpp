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
static char     *Version = "\n@(#)Version 5/25/96-cdt-kpv\n";

extern Dtmethod_t*	Dttree;
extern Dtmethod_t	_Dttree;

/* 	Make a new dictionary
**
**	Written by Kiem-Phong Vo (5/25/96)
*/

#if __STD_C
Dt_t* dtopen(Dtdisc_t* disc, Dtmethod_t* meth)
#else
Dt_t*	dtopen(disc, meth)
Dtdisc_t*	disc;
Dtmethod_t*	meth;
#endif
{
	Dt_t*		dt = (Dt_t*)Version;	/* shut-up unuse warning */
	reg int		e;
	Dtdata_t*	data;

#ifdef _WIN32
	Dttree = &_Dttree;
#endif

	if(!disc || !meth)
		return NIL(Dt_t*);

	/* allocate space for dictionary */
	if(!(dt = (Dt_t*) malloc(sizeof(Dt_t))))
		return NIL(Dt_t*);

	/* initialize all absolutely private data */
	dt->searchf = NIL(Dtsearch_f);
	dt->meth = NIL(Dtmethod_t*);
	dt->disc = NIL(Dtdisc_t*);
	dtdisc(dt,disc,0);
	dt->nview = 0;
	dt->view = dt->walk = NIL(Dt_t*);

	if(disc->eventf)
	{	/* if shared/persistent dictionary, get existing data */
		data = NIL(Dtdata_t*);
		if((e = (*disc->eventf)(dt,DT_OPEN,(Void_t*)(&data),disc)) != 0)
		{	if(e < 0 || !data || !(data->type&meth->type) )
			{	free((Void_t*)dt);
				return NIL(Dt_t*);
			}
			else	goto done;
		}
	}

	/* allocate sharable data */
	data = (Dtdata_t*)(dt->memoryf)(dt,NIL(Void_t*),sizeof(Dtdata_t),disc);
	if(!data)
	{	free((Void_t*)dt);
		return NIL(Dt_t*);
	}
	data->type = meth->type;
	data->here = NIL(Dtlink_t*);
	data->htab = NIL(Dtlink_t**);
	data->ntab = data->size = 0;

done:
	dt->data = data;
	dt->searchf = meth->searchf;
	dt->meth = meth;
	return dt;
}
