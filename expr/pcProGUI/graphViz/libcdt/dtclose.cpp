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

/*	Close a dictionary
**
**	Written by Kiem-Phong Vo (05/25/96)
*/
#if __STD_C
int dtclose(reg Dt_t* dt)
#else
int dtclose(dt)
reg Dt_t*	dt;
#endif
{
	if(dt->nview > 0 ) /* can't close if being viewed */
		return -1;

	if(dt->view)	/* turn off viewing */
		dtview(dt,NIL(Dt_t*));

	/* announce the close event */
	if(dt->disc->eventf &&
	   (*dt->disc->eventf)(dt,DT_CLOSE,NIL(Void_t*),dt->disc) < 0)
		return -1;

	/* release all allocated data */
	(void)(*(dt->meth->searchf))(dt,NIL(Void_t*),DT_CLEAR);
	if(dtsize(dt) > 0)
		return -1;

	if(dt->data->ntab > 0)
		(*dt->memoryf)(dt,(Void_t*)dt->data->htab,0,dt->disc);
	(*dt->memoryf)(dt,(Void_t*)dt->data,0,dt->disc);

	free((Void_t*)dt);

	return 0;
}
