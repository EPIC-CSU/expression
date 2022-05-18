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
/*
 * Copyright (c) AT&T Corp. 1994, 1995.
 * This code is licensed by AT&T Corp.  For the
 * terms and conditions of the license, see
 * http://www.research.att.com/orgs/ssr/book/reuse
 */

/*
 * Break cycles in a directed graph by depth-first search.
 */

#include "stdafx.h"
#include "dot.h"

void acyclic(graph_t* g)
{
	int		c;
	node_t	*n;

	for (c = 0; c < g->u.comp.size; c++) {
		g->u.nlist = g->u.comp.list[c];
		for (n = g->u.nlist; n; n = n->u.next) n->u.mark = FALSE;
		for (n = g->u.nlist; n; n = n->u.next) dfs(n);
	}
}

void dfs(node_t* n)
{
	int		i;
	edge_t	*e;
	node_t	*w;
	
	if (n->u.mark) return;
	n->u.mark = TRUE;
	n->u.onstack = TRUE;
	for (i = 0; (e = n->u.out.list[i]); i++) {
		w = e->head;
		if (w->u.onstack) { reverse_edge(e); i--; }
		else { if (w->u.mark == FALSE) dfs(w); }
	}
	n->u.onstack = FALSE;
}

void reverse_edge(edge_t* e)
{
	edge_t		*f;

	delete_fast_edge(e);
	if ((f = find_fast_edge(e->head,e->tail))) merge_oneway(e,f);
	else virtual_edge(e->head,e->tail,e);
}
