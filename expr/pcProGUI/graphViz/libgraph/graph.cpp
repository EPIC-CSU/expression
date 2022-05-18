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
#include "libgraph.h"
/*
 * Copyright (c) AT&T Corp. and Lucent Corp. 1994-97.
 * This code is licensed by AT&T Corp. and Lucent Corp.
 * For the terms and conditions of the license, see
 * http://www.research.att.com/orgs/ssr/book/reuse
 */

extern Dtmethod_t*	Dttree;

Dtdisc_t agNamedisc = {
	offsetof(Agnode_t,name),
	-1,
	-1,							/* link offset */
	NIL(Dtmake_f),
	NIL(Dtfree_f),
	NIL(Dtcompar_f),			/* use strcmp */
	NIL(Dthash_f),
	NIL(Dtmemory_f),
	NIL(Dtevent_f)
};

Dtdisc_t agNodedisc = {
	offsetof(Agnode_t,id),
	sizeof(int),				
	-1,							/* link offset */
	NIL(Dtmake_f),
	NIL(Dtfree_f),
	NIL(Dtcompar_f),			/* use memcmp on ids */
	NIL(Dthash_f),
	NIL(Dtmemory_f),
	NIL(Dtevent_f)
};

Dtdisc_t agIndisc = {
	0,							/* pass whole object as key */
	0,
	-1,							/* link offset */
	NIL(Dtmake_f),
	NIL(Dtfree_f),
	(Dtcompar_f)agcmpin,
	NIL(Dthash_f),
	NIL(Dtmemory_f),
	NIL(Dtevent_f)
};

Dtdisc_t agOutdisc = {
	0,							/* pass whole object as key */
	0,
	-1,							/* link offset */
	NIL(Dtmake_f),
	NIL(Dtfree_f),
	(Dtcompar_f)agcmpout,
	NIL(Dthash_f),
	NIL(Dtmemory_f),
	NIL(Dtevent_f)
};

#ifdef DEBUG
static int
myinedgecmp(e0, e1)
Agedge_t		*e0,*e1;
{
	int rv = myinedgecmp(e0,e1);
	printf("compare (%s,%s:%s),(%s,%s:%s) = %d\n",
		e0->head?e0->head->name:"nil",
		e0->tail?e0->tail->name:"nil",
		e0->key? e0->key : "nil",
		e1->head?e1->head->name:"nil",
		e1->tail?e1->tail->name:"nil",
		e1->key? e1->key : "nil",
		rv);
	return rv;
}
#endif

static int agcmpedge(Agedge_t* e0, Agedge_t* e1, int flag)
{
	Agnode_t	*maj0,*maj1,*min0,*min1;
	char		*key0,*key1;

	if (flag) {
		maj0 = e0->tail; maj1 = e1->tail;
		min0 = e0->head; min1 = e1->head;
	}
	else {
		maj0 = e0->head; maj1 = e1->head;
		min0 = e0->tail; min1 = e1->tail;
	}
	key0 = e0->attr? e0->attr[KEYX] : NULL;
	key1 = e1->attr? e1->attr[KEYX] : NULL;

	if (maj0 != maj1) {
		if (maj0 && maj1) return maj0->id - maj1->id;
		else return (maj0? 1 : -1);
	}

	if (min0 != min1) {
		if (min0 && min1) return min0->id - min1->id;
		else return (min0? 1 : -1);
	}
	if (key0 == NULL) return (key1? -1 : 0);
	if (key1 == NULL) return 1;
	return strcmp(key0,key1);
}

int agcmpin(Dict_t* d, Agedge_t* e0, Agedge_t* e1, Dtdisc_t* disc)
{
	return agcmpedge(e0,e1,FALSE);
}

int agcmpout(Dict_t* d, Agedge_t* e0, Agedge_t* e1, Dtdisc_t* disc)
{
	return agcmpedge(e0,e1,TRUE);
}

static Agdata_t *agnewdata(void)
{
	Agdata_t		*rv;

	rv = NEW(Agdata_t);
	rv->node_dict	= dtopen(&agNamedisc,Dttree);
	rv->globattr	= agNEWdict("graph");
	rv->nodeattr	= agNEWdict("node");
	rv->edgeattr	= agNEWdict("edge");
	if (AG.proto_g) {
		agcopydict(rv->globattr,AG.proto_g->univ->globattr);
		agcopydict(rv->nodeattr,AG.proto_g->univ->nodeattr);
		agcopydict(rv->edgeattr,AG.proto_g->univ->edgeattr);
	}
	return rv;
}

static void agfreedata(Agraph_t* g)
{
	agFREEdict(g,g->univ->globattr);
	agFREEdict(g,g->univ->nodeattr);
	agFREEdict(g,g->univ->edgeattr);
	dtclose(g->univ->node_dict);
	free(g->univ);
}

static void dup_proto(Agraph_t* g, Agproto_t* proto)
{
	Agnode_t	*n = NULL;
	Agedge_t	*e = NULL;
	Agproto_t	*s = NEW(Agproto_t);

	s->prev = g->proto;
	if (proto) {n = proto->n; e = proto->e;}
	s->n = agNEWnode(g,"\001proto",n);
	s->e = agNEWedge(g,s->n,s->n,e);
	g->proto = s;
}

void agpushproto(Agraph_t* g)
{
	dup_proto(g,g->proto);
}

void agpopproto(Agraph_t* g)
{
	Agproto_t		*s = g->proto;
	if (s != NULL) {
		g->proto = s->prev;
		s->e->tail = s->e->head = s->n;
		agFREEedge(s->e);
		agFREEnode(s->n);
		free(s);
	}
}

static Agraph_t *agNEWgraph(char* name, Agraph_t* parent, int kind)
{
	int				i, nobj;
	Agraph_t		*g;

	if (AG.init_called == FALSE) {
		fprintf(stderr,"libag fatal error -- aginit() was not called\n");
		exit(1);
	}
	g			= (Agraph_t*) calloc(1,AG.graph_nbytes);
	g->tag		= TAG_GRAPH;
	g->kind		= kind;
	g->nodes	= dtopen(&agNodedisc,Dttree);
	g->inedges	= dtopen(&agIndisc,Dttree);
	g->outedges	= dtopen(&agOutdisc,Dttree);

	if (parent == NULL) {
		g->univ		= agnewdata();
		g->root		= g;
		nobj = dtsize(g->univ->globattr->dict);
		g->attr	= N_NEW(nobj,char*);
		for (i = 0; i < nobj; i++)
			g->attr[i] = agstrdup(AG.proto_g->attr[i]);
	}
	else {
		g->univ		= parent->univ;
		g->root 	= parent->root;
		nobj = dtsize(parent->univ->globattr->dict);
		g->attr 	= N_NEW(nobj,char*);
		for (i = 0; i < nobj; i++)
			g->attr[i] = agstrdup(parent->attr[i]);
	}

	g->meta_node 	= NULL;
	g->name			= agstrdup(name);
	g->proto		= NULL;

	if (parent) dup_proto(g,parent->proto);
	else agpushproto(g);
	return g;
}

static int reach0(Dict_t* m, Agnode_t* from, Agnode_t* to)
{
	Agedge_t		*e;

	if (from == to) return TRUE;
	if (agfindedge(from->graph->root,from,to)) return TRUE;
	dtinsert(m,from);
	for (e = agfstout(from->graph,from); e; e = agnxtout(from->graph,e))
		if ((dtsearch(m,e->head) == NULL) && reach0(m,e->head,to))
			return TRUE;
	return FALSE;
}

static int reach(Agnode_t* from, Agnode_t* to)
{
	Dict_t		*m;
	int			rv;

	m = dtopen(&agNodedisc,Dttree);
	rv = reach0(m,from,to);
	dtclose(m);
	return rv;
}

Agraph_t *agusergraph(Agnode_t* n)
{
	return (n->graph->meta_node ? NULL : (Agraph_t*)(n->attr[0]));
}

Agraph_t *agopen(char* name, int kind)
{
	Agraph_t *g,*meta;

	g = agNEWgraph(name,NULL,kind);
	meta = agNEWgraph(name,NULL,AGMETAGRAPH);
	agnodeattr(meta, "agusergraph", NULL);
	g->meta_node = agnode(meta,name);
	g->meta_node->attr[0] = (char*) g;
	return g;
}

Agraph_t *agsubg(Agraph_t* g, char* name)
{
	Agraph_t	*subg,*meta;
	Agnode_t	*n;

	meta = g->meta_node->graph;
	n = agfindnode(meta,name);
	if (n) subg = agusergraph(n);
	else {
		subg = agNEWgraph(name,g,g->kind);
		n = agnode(meta,name);
		subg->meta_node = n;
		n->attr[0] = (char*) subg;
	}
	agINSgraph(g,subg);
	return subg;
}

Agraph_t *agfindsubg(Agraph_t* g, char* name)
{
	Agnode_t	*n;

	if (g->meta_node) {
		n = agfindnode(g->meta_node->graph,name);
		if (n) return agusergraph(n);
	}
	return NULL;
}

void agINSgraph(Agraph_t* g, Agraph_t* subg)
{
	Agnode_t	*h,*t;
	t = g->meta_node;
	h = subg->meta_node;
	if (t && h && (reach(h,t) == FALSE))
		agedge(t->graph,t,h);
}

void agclose(Agraph_t* g)
{
	Agedge_t	*e,*f;
	Agnode_t	*n,*nn;
	Agraph_t	*meta = NULL;
	int			i,nobj,flag,is_meta;

	if ((g == NULL) || (TAG_OF(g) != TAG_GRAPH)) return;
	is_meta = AG_IS_METAGRAPH(g);
	if (is_meta == FALSE) {
		meta = g->meta_node->graph;
		/* recursively remove its subgraphs */
		do {	/* better semantics would be to find strong component */
			flag = FALSE;
			for (e = agfstout(meta,g->meta_node); e; e = f) {
				f = agnxtout(meta,e);
				if (agnxtin(meta,agfstin(meta,e->head)) == NULL) {
					agclose(agusergraph(e->head));
					flag = TRUE;
				}
			}
		} while (flag);
	}
	while (g->proto) agpopproto(g);
	if (is_meta == FALSE) {
		nobj = dtsize(g->univ->globattr->dict);
		for (i = 0; i < nobj; i++)
			agstrfree(g->attr[i]);
	}
	free(g->attr);
	if (g == g->root) {
		for (n = agfstnode(g); n; n = nn) {
			nn = agnxtnode(g,n);
			agDELnode(g,n);
		}
		if (is_meta == FALSE) agclose(g->meta_node->graph);
		agfreedata(g);
	}
	else {
		if (is_meta == FALSE) agdelete(meta,g->meta_node);
	}
	/*
	dtclose(g->nodes);
	dtclose(g->inedges);
	dtclose(g->outedges);
	agstrfree(g->name);
	*/
	TAG_OF(g) = -1;
	free(g);
}

int agcontains(Agraph_t* g, void* obj)
{
	switch(TAG_OF(obj)) {
		case TAG_NODE: return(agidnode(g,((Agnode_t*)obj)->id) != NULL);
		case TAG_EDGE: return(dtsearch(g->inedges,(Agedge_t*)obj) != NULL);
		case TAG_GRAPH: return(reach(g->meta_node,((Agraph_t*)obj)->meta_node));
	}
	return FALSE;
}

void aginsert(Agraph_t* g, void* obj)
{
	switch (TAG_OF(obj)) {
		case TAG_NODE: 	agINSnode(g,(struct Agnode_t *)obj); break;
		case TAG_EDGE:	agINSedge(g,(struct Agedge_t *)obj); break;
		case TAG_GRAPH:	agINSgraph(g,(struct Agraph_t *)obj); break;
	}
}

void agdelete(Agraph_t* g, void* obj)
{
	switch (TAG_OF(obj)) {
		case TAG_NODE: 	agDELnode(g,(struct Agnode_t *)obj); break;
		case TAG_EDGE:	agDELedge(g,(struct Agedge_t *)obj); break;
		case TAG_GRAPH:	agclose((struct Agraph_t *)obj); break;
	}
}

int agnnodes(Agraph_t* g)
{ return dtsize(g->nodes); }
int agnedges(Agraph_t* g)
{ return dtsize(g->outedges); }
