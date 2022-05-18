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
 * Copyright (c) AT&T Corp. and Lucent Corp. 1994-97.
 * This code is licensed by AT&T Corp. and Lucent Corp.
 * For the terms and conditions of the license, see
 * http://www.research.att.com/orgs/ssr/book/reuse
 */

#include "stdafx.h"
#include "libgraph.h"

Agnode_t *agfindnode(Agraph_t* g, char* name)
{
	Agnode_t		*rv;

	rv =  (Agnode_t*)dtmatch(g->univ->node_dict,name);
	if (rv && (g != g->root)) rv = (Agnode_t*)dtsearch(g->nodes,rv);
	return rv;
}

Agnode_t *agidnode(Agraph_t* g, int index)
{
	Agnode_t	*rv;
	rv =  (Agnode_t*)dtmatch(g->nodes,&index);
	return rv;
}

Agnode_t *agnode(Agraph_t* g, char* name)
{
	Agnode_t		*n;
	if ((n = agfindnode(g->root,name)) == NULL) {
		n = agNEWnode(g,name,g->proto->n);
		dtinsert(g->univ->node_dict,n);
	}
	agINSnode(g,n);
	return n;
}

void agINSnode(Agraph_t* g, Agnode_t* n)
{
	Agraph_t		*meta;
	Agedge_t		*e;

	if (agidnode(g,n->id)) return;
	dtinsert(g->nodes,n);
	if (AG_IS_METAGRAPH(g) == FALSE) {
		meta = g->meta_node->graph;
		for (e = agfstin(meta,g->meta_node); e; e = agnxtin(meta,e))
			agINSnode(agusergraph(e->tail),n);
	}
}

void
agDELnode(Agraph_t* g, Agnode_t* n)
{
	Agedge_t		*e,*f;
	Agraph_t		*meta,*h;

	for (e = agfstedge(g,n); e; e = f) {
		f = agnxtedge(g,e,n);
		agDELedge(g,e);
	}

	if (AG_IS_METAGRAPH(g) == FALSE) {
		meta = g->meta_node->graph;
		for (e = agfstout(meta,g->meta_node); e; e = agnxtout(meta,e)) {
			h = agusergraph(e->head);
			if (dtsearch(h->nodes,n)) agDELnode(h,n);
		}
	}
	dtdelete(g->nodes,n);
	if (g == g->root) agFREEnode(n);
}

Agnode_t *agfstnode(Agraph_t* g)
{
	return (Agnode_t*)dtfirst(g->nodes);
}

Agnode_t * agnxtnode(Agraph_t* g, Agnode_t* n)
{
	return (Agnode_t*)dtnext(g->nodes,n);
}

Agnode_t *agNEWnode(Agraph_t* subg, char* name, Agnode_t* proto)
{
	int			i, nobj;
	Agnode_t		*n;

	n			=	(Agnode_t*) calloc(1,AG.node_nbytes);
	n->tag		=	TAG_NODE;
	n->name		=	agstrdup(name);
	n->id		=	subg->univ->max_node_id++;
	n->graph	=	subg->root;
	nobj = dtsize(subg->univ->nodeattr->dict);
	n->attr 	=	N_NEW(nobj,char*);
	for (i = 0; i < nobj; i++)
	  n->attr[i] = agstrdup(proto? proto->attr[i] :
		  subg->univ->nodeattr->list[i]->value);
	return n;
}

void agFREEnode(Agnode_t* n)
{
	int			i, nobj;
	Agdict_t	*dict = agdictof(n);

	dict = dict;
	dtdelete(n->graph->univ->node_dict,n);
	TAG_OF(n) = -1;
	agstrfree(n->name);
	if (AG_IS_METAGRAPH(n->graph) == FALSE) {
		nobj = dtsize(n->graph->univ->nodeattr->dict);
		for (i = 0; i < nobj; i++)
			agstrfree(n->attr[i]);
	}
	free(n->attr);
	free(n);
}
