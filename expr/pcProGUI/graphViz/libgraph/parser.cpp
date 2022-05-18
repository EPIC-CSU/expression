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
#ifndef lint
static char agsccsid[] = "@(#)yaccpar	1.9 (Berkeley) 02/21/93";
#endif
#define AGBYACC 1
#define AGMAJOR 1
#define AGMINOR 9
#define agclearin (agchar=(-1))
#define agerrok (agerrflag=0)
#define AGRECOVERING (agerrflag!=0)
#define AGPREFIX "ag"
#line 2 "parser.y"
#pragma prototyped

/*
 * Copyright (c) AT&T Corp. and Lucent Corp. 1994-97.
 * This code is licensed by AT&T Corp. and Lucent Corp.
 * For the terms and conditions of the license, see
 * http://www.research.att.com/orgs/ssr/book/reuse
 */

#include "stdafx.h"
#include	"libgraph.h"

static char		Port[SMALLBUF],*Symbol;
static char		In_decl,In_edge_stmt;
static int		Current_class,Agraph_type;
static Agraph_t		*G;
static Agnode_t		*N;
static Agedge_t		*E;
static objstack_t	*SP;
static Agraph_t		*Gstack[32];
static int			GSP;

static void push_subg(Agraph_t *g)
{
	G = Gstack[GSP++] = g;
}

static Agraph_t *pop_subg(void)
{
	Agraph_t		*g;
	if (GSP == 0) {
		fprintf(stderr,"Gstack underflow in graph parser\n"); exit(1);
	}
	g = Gstack[--GSP];
	G = Gstack[GSP - 1];
	return g;
}

static objport_t pop_gobj(void)
{
	objport_t	rv;
	rv.obj = pop_subg();
	rv.port = NULL;
	return rv;
}

static void begin_graph(char *name)
{
	Agraph_t		*g;
	g = AG.parsed_g = agopen(name,Agraph_type);
	push_subg(g);
	In_decl = TRUE;
}

static void end_graph(void)
{
	pop_subg();
}

static Agnode_t *bind_node(char *name)
{
	Agnode_t	*n = agnode(G,name);
	In_decl = FALSE;
	return n;
}

static void anonsubg(void)
{
	static int		anon_id = 0;
	char			buf[SMALLBUF];
	Agraph_t			*subg;

	In_decl = FALSE;
	sprintf(buf,"_anonymous_%d",anon_id++);
	subg = agsubg(G,buf);
	push_subg(subg);
}

static int isanonsubg(Agraph_t *g)
{
	return (strncmp("_anonymous_",g->name,11) == 0);
}

static void begin_edgestmt(objport_t objp)
{
	struct objstack_t	*new_sp;

	new_sp = NEW(objstack_t);
	new_sp->link = SP;
	SP = new_sp;
	SP->list = SP->last = NEW(objlist_t);
	SP->list->data  = objp;
	SP->list->link = NULL;
	SP->in_edge_stmt = In_edge_stmt;
	SP->subg = G;
	agpushproto(G);
	In_edge_stmt = TRUE;
}

static void mid_edgestmt(objport_t objp)
{
	SP->last->link = NEW(objlist_t);
	SP->last = SP->last->link;
	SP->last->data = objp;
	SP->last->link = NULL;
}

static void end_edgestmt(void)
{
	objstack_t	*old_SP;
	objlist_t	*tailptr,*headptr,*freeptr;
	Agraph_t		*t_graph,*h_graph;
	Agnode_t	*t_node,*h_node,*t_first,*h_first;
	Agedge_t	*e;
	char		*tport,*hport;

	for (tailptr = SP->list; tailptr->link; tailptr = tailptr->link) {
		headptr = tailptr->link;
		tport = tailptr->data.port;
		hport = headptr->data.port;
		if (TAG_OF(tailptr->data.obj) == TAG_NODE) {
			t_graph = NULL;
			t_first = (Agnode_t*)(tailptr->data.obj);
		}
		else {
			t_graph = (Agraph_t*)(tailptr->data.obj);
			t_first = agfstnode(t_graph);
		}
		if (TAG_OF(headptr->data.obj) == TAG_NODE) {
			h_graph = NULL;
			h_first = (Agnode_t*)(headptr->data.obj);
		}
		else {
			h_graph = (Agraph_t*)(headptr->data.obj);
			h_first = agfstnode(h_graph);
		}

		for (t_node = t_first; t_node; t_node = t_graph ?
		  agnxtnode(t_graph,t_node) : NULL) {
			for (h_node = h_first; h_node; h_node = h_graph ?
			  agnxtnode(h_graph,h_node) : NULL ) {
				e = agedge(G,t_node,h_node);
				if (e) {
					char	*tp = tport;
					char 	*hp = hport;
					if (e->head == t_node) {
						/* could happen with an undirected edge */
						char 	*temp;
						temp = tp; tp = hp; hp = temp;
					}
					if (tp && tp[0]) agxset(e,TAILX,tp);
					if (hp && hp[0]) agxset(e,HEADX,hp);
				}
			}
		}
	}
	tailptr = SP->list; 
	while (tailptr) {
		freeptr = tailptr;
		tailptr = tailptr->link;
		if (TAG_OF(freeptr->data.obj) == TAG_NODE)
		free(freeptr->data.port);
		free(freeptr);
	}
	if (G != SP->subg) abort();
	agpopproto(G);
	In_edge_stmt = SP->in_edge_stmt;
	old_SP = SP;
	SP = SP->link;
	In_decl = FALSE;
	free(old_SP);
	Current_class = TAG_GRAPH;
}

static Agraph_t *parent_of(Agraph_t *g)
{
	Agraph_t		*rv;
	rv = agusergraph(agfstin(g->meta_node->graph,g->meta_node)->tail);
	return rv;
}

static void attr_set(char *name, char *value)
{
	Agsym_t		*ap = NULL;
	char		*defval = "";

	if (In_decl && (G->root == G)) defval = value;
	switch (Current_class) {
		case TAG_NODE:
			ap = agfindattr(G->proto->n,name);
			if (ap == NULL)
				ap = agnodeattr(AG.parsed_g,name,defval);
			agxset(N,ap->index,value);
			break;
		case TAG_EDGE:
			ap = agfindattr(G->proto->e,name);
			if (ap == NULL)
				ap = agedgeattr(AG.parsed_g,name,defval);
			agxset(E,ap->index,value);
			break;
		case 0:		/* default */
		case TAG_GRAPH:
			ap = agfindattr(G,name);
			if (ap == NULL) 
				ap = agraphattr(AG.parsed_g,name,defval);
			agxset(G,ap->index,value);
			break;
	}
}

#line 213 "parser.y"
typedef union	{
			int					i;
			char				*str;
			struct objport_t	obj;
			struct Agnode_t		*n;
} AGSTYPE;
#line 229 "y.tab.c"
#define T_graph 257
#define T_digraph 258
#define T_strict 259
#define T_node 260
#define T_edge 261
#define T_edgeop 262
#define T_symbol 263
#define T_subgraph 264
#define AGERRCODE 256
short aglhs[] = {                                        -1,
    5,    0,    0,    0,    4,    4,    4,    4,    7,    7,
    7,    8,    8,   10,   10,   11,   12,   12,   13,   14,
    9,    6,    6,   15,   15,   16,   16,   17,   17,   17,
   17,   20,   20,    2,    1,   21,   21,   21,   21,   21,
   22,   24,   22,   23,   25,   18,   27,   28,   19,   29,
   30,   19,   26,   31,   26,   26,   32,   26,    3,   34,
    3,    3,   33,
};
short aglen[] = {                                         2,
    0,    6,    1,    0,    1,    2,    1,    2,    1,    1,
    1,    3,    0,    0,    1,    3,    2,    0,    1,    0,
    4,    1,    0,    1,    2,    1,    2,    1,    1,    1,
    1,    2,    1,    2,    1,    0,    1,    1,    2,    2,
    2,    0,    7,    2,    0,    3,    0,    0,    5,    0,
    0,    5,    2,    0,    4,    2,    0,    4,    4,    0,
    4,    1,    2,
};
short agdefred[] = {                                      0,
    3,    5,    7,    0,    0,    0,    6,    8,    1,    0,
    0,    9,   10,   11,    0,    0,   60,    0,    0,    0,
    0,    0,   33,    0,   24,    0,   28,   29,   30,    0,
    0,   63,    0,    0,    0,   34,    0,    0,   18,    0,
    0,    2,    0,   32,   25,   27,    0,    0,    0,   41,
    0,   44,   40,   39,    0,   46,    0,   48,   51,   20,
    0,    0,    0,   21,   61,   42,   17,   35,    0,    0,
   18,   18,   16,   15,    0,   59,    0,    0,    0,   49,
   52,   12,    0,   55,   58,    0,   43,
};
short agdgoto[] = {                                       5,
   18,   19,   20,    6,   10,   21,   22,   61,   23,   75,
   44,   55,   56,   31,   24,   25,   26,   27,   28,   29,
   36,   37,   38,   77,   39,   58,   40,   71,   41,   72,
   78,   79,   30,   33,
};
short agsindex[] = {                                   -232,
    0,    0,    0, -229,    0, -254,    0,    0,    0, -111,
 -107,    0,    0,    0,    0, -248,    0,  -44,    0,    0,
 -108,  -72,    0, -107,    0,  -29,    0,    0,    0, -102,
  -26,    0, -107,  -40, -227,    0,  -24,  -17,    0, -224,
 -224,    0, -218,    0,    0,    0, -107, -217,  -77,    0,
 -214,    0,    0,    0,  -72,    0, -105,    0,    0,    0,
  -43,    7,  -73,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0, -218,    0,    9, -224, -224,    0,
    0,    0, -209,    0,    0,   16,    0,
};
short agrindex[] = {                                     58,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
  -66,    0,    0,    0,  -57,    0,    0,  -49,  -36,    5,
    0,    0,    0,  -65,    0, -112,    0,    0,    0,   13,
    0,    0,  -66,    0,    0,    0,  -28,  -20,    0,    0,
    0,    0,  -32,    0,    0,    0,  -66,    0,    0,    0,
    0,    0,    0,    0,   21,    0,    0,    0,    0,    0,
    0,  -90,    0,    0,    0,    0,    0,    0,  -12,   -3,
    0,    0,    0,    0,  -32,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,
};
short aggindex[] = {                                      0,
    0,    8,   10,    0,    0,  -25,    0,  -13,  -38,    0,
   14,    0,  -39,    0,    0,   46,    0,    0,    0,    0,
    0,   35,   38,    0,    0,  -35,    0,    0,    0,    0,
    0,    0,    0,    0,
};
#define AGTABLESIZE 285
short agtable[] = {                                      51,
   35,   35,   14,   20,   62,   59,   35,   49,    9,   36,
   26,   11,   26,   34,   32,   17,   42,   17,   43,   35,
   47,   63,   45,    1,    2,    3,    4,    7,    8,   46,
   37,   80,   81,   35,   48,   52,   62,   57,   38,   35,
   34,   36,   84,   85,   60,   64,   53,   65,   66,   73,
   74,   76,   83,   86,   45,   56,   87,    4,   23,   22,
   13,   82,   37,   31,   69,   35,   70,   35,   67,   45,
   38,   62,   54,   36,   53,   36,    0,    0,   53,   19,
    0,    0,    0,    0,    0,    0,   45,   56,   45,    0,
    0,    0,    0,    0,   37,    0,   37,    0,    0,    0,
    0,    0,   38,   62,   38,    0,    0,    0,    0,    0,
   53,    0,   53,    0,    0,    0,    0,    0,    0,   56,
    0,   56,    0,    0,    0,    0,    0,   31,    0,   31,
    0,    0,    0,    0,    0,    0,    0,   62,    0,    0,
    0,    0,    0,   19,   26,   19,    0,   26,   26,   12,
   26,   26,   13,   14,    0,   15,   16,   68,   16,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,   14,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,   35,
    0,    0,   35,   35,   35,   35,   35,   36,    0,    0,
   36,   36,   36,   36,   36,    0,    0,    0,    0,    0,
   45,    0,   50,   45,   45,   47,   45,   45,   37,    0,
    0,   37,   37,   37,   37,   37,   38,    0,    0,   38,
   38,   38,   38,   38,   53,    0,    0,   53,   53,   54,
   53,   53,    0,   56,    0,    0,   56,   56,   57,   56,
   56,   31,    0,    0,   31,   31,   50,   31,   31,   62,
    0,    0,   62,   62,   62,   62,   62,   19,    0,    0,
   19,   19,    0,   19,   19,
};
short agcheck[] = {                                      40,
   58,   59,   93,   61,   43,   41,   64,   33,  263,   59,
  123,  123,  125,   58,  263,  123,  125,  123,   91,   64,
  123,   47,   59,  256,  257,  258,  259,  257,  258,   59,
   59,   71,   72,   91,   61,  263,   75,  262,   59,   64,
   58,   91,   78,   79,  263,  263,   59,  125,  263,   93,
   44,  125,   44,  263,   91,   59,   41,    0,  125,  125,
   93,   75,   91,   59,   57,  123,   57,  125,   55,   24,
   91,   59,   38,  123,   37,  125,   -1,   -1,   91,   59,
   -1,   -1,   -1,   -1,   -1,   -1,  123,   91,  125,   -1,
   -1,   -1,   -1,   -1,  123,   -1,  125,   -1,   -1,   -1,
   -1,   -1,  123,   91,  125,   -1,   -1,   -1,   -1,   -1,
  123,   -1,  125,   -1,   -1,   -1,   -1,   -1,   -1,  123,
   -1,  125,   -1,   -1,   -1,   -1,   -1,  123,   -1,  125,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,  125,   -1,   -1,
   -1,   -1,   -1,  123,  257,  125,   -1,  260,  261,  257,
  263,  264,  260,  261,   -1,  263,  264,  263,  264,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  263,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  257,
   -1,   -1,  260,  261,  262,  263,  264,  257,   -1,   -1,
  260,  261,  262,  263,  264,   -1,   -1,   -1,   -1,   -1,
  257,   -1,  263,  260,  261,  262,  263,  264,  257,   -1,
   -1,  260,  261,  262,  263,  264,  257,   -1,   -1,  260,
  261,  262,  263,  264,  257,   -1,   -1,  260,  261,  262,
  263,  264,   -1,  257,   -1,   -1,  260,  261,  262,  263,
  264,  257,   -1,   -1,  260,  261,  262,  263,  264,  257,
   -1,   -1,  260,  261,  262,  263,  264,  257,   -1,   -1,
  260,  261,   -1,  263,  264,
};
#define AGFINAL 5
#ifndef AGDEBUG
#define AGDEBUG 0
#endif
#define AGMAXTOKEN 264
#if AGDEBUG
char *agname[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,"'('","')'",0,0,"','",0,0,0,0,0,0,0,0,0,0,0,0,0,"':'","';'",0,"'='",
0,0,"'@'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"'['",0,"']'",0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"'{'",0,"'}'",0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,"T_graph","T_digraph","T_strict","T_node","T_edge","T_edgeop",
"T_symbol","T_subgraph",
};
char *agrule[] = {
"$accept : file",
"$$1 :",
"file : graph_type T_symbol $$1 '{' stmt_list '}'",
"file : error",
"file :",
"graph_type : T_graph",
"graph_type : T_strict T_graph",
"graph_type : T_digraph",
"graph_type : T_strict T_digraph",
"attr_class : T_graph",
"attr_class : T_node",
"attr_class : T_edge",
"inside_attr_list : attr_set optcomma inside_attr_list",
"inside_attr_list :",
"optcomma :",
"optcomma : ','",
"attr_list : '[' inside_attr_list ']'",
"rec_attr_list : rec_attr_list attr_list",
"rec_attr_list :",
"opt_attr_list : rec_attr_list",
"$$2 :",
"attr_set : T_symbol $$2 '=' T_symbol",
"stmt_list : stmt_list1",
"stmt_list :",
"stmt_list1 : stmt",
"stmt_list1 : stmt_list1 stmt",
"stmt : stmt1",
"stmt : stmt1 ';'",
"stmt1 : node_stmt",
"stmt1 : edge_stmt",
"stmt1 : attr_stmt",
"stmt1 : subg_stmt",
"attr_stmt : attr_class attr_list",
"attr_stmt : attr_set",
"node_id : node_name node_port",
"node_name : T_symbol",
"node_port :",
"node_port : port_location",
"node_port : port_angle",
"node_port : port_angle port_location",
"node_port : port_location port_angle",
"port_location : ':' T_symbol",
"$$3 :",
"port_location : ':' '(' T_symbol $$3 ',' T_symbol ')'",
"port_angle : '@' T_symbol",
"$$4 :",
"node_stmt : node_id $$4 opt_attr_list",
"$$5 :",
"$$6 :",
"edge_stmt : node_id $$5 edgeRHS $$6 opt_attr_list",
"$$7 :",
"$$8 :",
"edge_stmt : subg_stmt $$7 edgeRHS $$8 opt_attr_list",
"edgeRHS : T_edgeop node_id",
"$$9 :",
"edgeRHS : T_edgeop node_id $$9 edgeRHS",
"edgeRHS : T_edgeop subg_stmt",
"$$10 :",
"edgeRHS : T_edgeop subg_stmt $$10 edgeRHS",
"subg_stmt : subg_hdr '{' stmt_list '}'",
"$$11 :",
"subg_stmt : '{' $$11 stmt_list '}'",
"subg_stmt : subg_hdr",
"subg_hdr : T_subgraph T_symbol",
};
#endif
#ifdef AGSTACKSIZE
#undef AGMAXDEPTH
#define AGMAXDEPTH AGSTACKSIZE
#else
#ifdef AGMAXDEPTH
#define AGSTACKSIZE AGMAXDEPTH
#else
#define AGSTACKSIZE 500
#define AGMAXDEPTH 500
#endif
#endif
int agdebug;
int agnerrs;
int agerrflag;
int agchar;
short *agssp;
AGSTYPE *agvsp;
AGSTYPE agval;
AGSTYPE aglval;
short agss[AGSTACKSIZE];
AGSTYPE agvs[AGSTACKSIZE];
#define agstacksize AGSTACKSIZE
#define AGABORT goto agabort
#define AGREJECT goto agabort
#define AGACCEPT goto agaccept
#define AGERROR goto agerrlab
int
agparse()
{
    register int agm, agn, agstate;
#if AGDEBUG
    register char *ags;
    extern char *getenv();

    if (ags = getenv("AGDEBUG"))
    {
        agn = *ags;
        if (agn >= '0' && agn <= '9')
            agdebug = agn - '0';
    }
#endif

    agnerrs = 0;
    agerrflag = 0;
    agchar = (-1);

    agssp = agss;
    agvsp = agvs;
    *agssp = agstate = 0;

agloop:
    if (agn = agdefred[agstate]) goto agreduce;
    if (agchar < 0)
    {
        if ((agchar = aglex()) < 0) agchar = 0;
#if AGDEBUG
        if (agdebug)
        {
            ags = 0;
            if (agchar <= AGMAXTOKEN) ags = agname[agchar];
            if (!ags) ags = "illegal-symbol";
            printf("%sdebug: state %d, reading %d (%s)\n",
                    AGPREFIX, agstate, agchar, ags);
        }
#endif
    }
    if ((agn = agsindex[agstate]) && (agn += agchar) >= 0 &&
            agn <= AGTABLESIZE && agcheck[agn] == agchar)
    {
#if AGDEBUG
        if (agdebug)
            printf("%sdebug: state %d, shifting to state %d\n",
                    AGPREFIX, agstate, agtable[agn]);
#endif
        if (agssp >= agss + agstacksize - 1)
        {
            goto agoverflow;
        }
        *++agssp = agstate = agtable[agn];
        *++agvsp = aglval;
        agchar = (-1);
        if (agerrflag > 0)  --agerrflag;
        goto agloop;
    }
    if ((agn = agrindex[agstate]) && (agn += agchar) >= 0 &&
            agn <= AGTABLESIZE && agcheck[agn] == agchar)
    {
        agn = agtable[agn];
        goto agreduce;
    }
    if (agerrflag) goto aginrecovery;
#ifdef lint
    goto agnewerror;
#endif
agnewerror:
    agerror("syntax error");
#ifdef lint
    goto agerrlab;
#endif
agerrlab:
    ++agnerrs;
aginrecovery:
    if (agerrflag < 3)
    {
        agerrflag = 3;
        for (;;)
        {
            if ((agn = agsindex[*agssp]) && (agn += AGERRCODE) >= 0 &&
                    agn <= AGTABLESIZE && agcheck[agn] == AGERRCODE)
            {
#if AGDEBUG
                if (agdebug)
                    printf("%sdebug: state %d, error recovery shifting\
 to state %d\n", AGPREFIX, *agssp, agtable[agn]);
#endif
                if (agssp >= agss + agstacksize - 1)
                {
                    goto agoverflow;
                }
                *++agssp = agstate = agtable[agn];
                *++agvsp = aglval;
                goto agloop;
            }
            else
            {
#if AGDEBUG
                if (agdebug)
                    printf("%sdebug: error recovery discarding state %d\n",
                            AGPREFIX, *agssp);
#endif
                if (agssp <= agss) goto agabort;
                --agssp;
                --agvsp;
            }
        }
    }
    else
    {
        if (agchar == 0) goto agabort;
#if AGDEBUG
        if (agdebug)
        {
            ags = 0;
            if (agchar <= AGMAXTOKEN) ags = agname[agchar];
            if (!ags) ags = "illegal-symbol";
            printf("%sdebug: state %d, error recovery discards token %d (%s)\n",
                    AGPREFIX, agstate, agchar, ags);
        }
#endif
        agchar = (-1);
        goto agloop;
    }
agreduce:
#if AGDEBUG
    if (agdebug)
        printf("%sdebug: state %d, reducing by rule %d (%s)\n",
                AGPREFIX, agstate, agn, agrule[agn]);
#endif
    agm = aglen[agn];
    agval = agvsp[1-agm];
    switch (agn)
    {
case 1:
#line 230 "parser.y"
{begin_graph(agvsp[0].str);}
break;
case 2:
#line 232 "parser.y"
{AG.accepting_state = TRUE; end_graph();}
break;
case 3:
#line 234 "parser.y"
{
					if (AG.parsed_g)
						agclose(AG.parsed_g);
					AG.parsed_g = NULL;
					/*exit(1);*/
				}
break;
case 4:
#line 240 "parser.y"
{AG.parsed_g = NULL;}
break;
case 5:
#line 245 "parser.y"
{Agraph_type = AGRAPH; AG.edge_op = "--";}
break;
case 6:
#line 247 "parser.y"
{Agraph_type = AGRAPHSTRICT; AG.edge_op = "--";}
break;
case 7:
#line 249 "parser.y"
{Agraph_type = AGDIGRAPH; AG.edge_op = "->";}
break;
case 8:
#line 251 "parser.y"
{Agraph_type = AGDIGRAPHSTRICT; AG.edge_op = "->";}
break;
case 9:
#line 255 "parser.y"
{Current_class = TAG_GRAPH;}
break;
case 10:
#line 257 "parser.y"
{Current_class = TAG_NODE; N = G->proto->n;}
break;
case 11:
#line 259 "parser.y"
{Current_class = TAG_EDGE; E = G->proto->e;}
break;
case 20:
#line 278 "parser.y"
{Symbol = strdup(agvsp[0].str);}
break;
case 21:
#line 279 "parser.y"
{attr_set(Symbol,agvsp[0].str); free(Symbol);}
break;
case 32:
#line 301 "parser.y"
{Current_class = TAG_GRAPH; /* reset */}
break;
case 33:
#line 303 "parser.y"
{Current_class = TAG_GRAPH;}
break;
case 34:
#line 307 "parser.y"
{
					objport_t		rv;
					rv.obj = agvsp[-1].n;
					rv.port = strdup(Port);
					Port[0] = '\0';
					agval.obj = rv;
				}
break;
case 35:
#line 316 "parser.y"
{agval.n = bind_node(agvsp[0].str);}
break;
case 41:
#line 326 "parser.y"
{strcat(Port,":"); strcat(Port,agvsp[0].str);}
break;
case 42:
#line 327 "parser.y"
{Symbol = strdup(agvsp[0].str);}
break;
case 43:
#line 328 "parser.y"
{	char buf[SMALLBUF];
					sprintf(buf,":(%s,%s)",Symbol,agvsp[-1].str);
					strcat(Port,buf); free(Symbol);
				}
break;
case 44:
#line 335 "parser.y"
{	char buf[SMALLBUF];
					sprintf(buf,"@%s",agvsp[0].str);
					strcat(Port,buf);
				}
break;
case 45:
#line 342 "parser.y"
{Current_class = TAG_NODE; N = (Agnode_t*)(agvsp[0].obj.obj);}
break;
case 46:
#line 344 "parser.y"
{Current_class = TAG_GRAPH; /* reset */}
break;
case 47:
#line 348 "parser.y"
{begin_edgestmt(agvsp[0].obj);}
break;
case 48:
#line 350 "parser.y"
{ E = SP->subg->proto->e;
				  Current_class = TAG_EDGE; }
break;
case 49:
#line 353 "parser.y"
{end_edgestmt();}
break;
case 50:
#line 355 "parser.y"
{begin_edgestmt(agvsp[0].obj);}
break;
case 51:
#line 357 "parser.y"
{ E = SP->subg->proto->e;
				  Current_class = TAG_EDGE; }
break;
case 52:
#line 360 "parser.y"
{end_edgestmt();}
break;
case 53:
#line 363 "parser.y"
{mid_edgestmt(agvsp[0].obj);}
break;
case 54:
#line 365 "parser.y"
{mid_edgestmt(agvsp[0].obj);}
break;
case 56:
#line 368 "parser.y"
{mid_edgestmt(agvsp[0].obj);}
break;
case 57:
#line 370 "parser.y"
{mid_edgestmt(agvsp[0].obj);}
break;
case 59:
#line 375 "parser.y"
{agval.obj = pop_gobj();}
break;
case 60:
#line 376 "parser.y"
{ anonsubg(); }
break;
case 61:
#line 376 "parser.y"
{agval.obj = pop_gobj();}
break;
case 62:
#line 377 "parser.y"
{agval.obj = pop_gobj();}
break;
case 63:
#line 381 "parser.y"
{ Agraph_t	 *subg;
				if ((subg = agfindsubg(AG.parsed_g,agvsp[0].str))) aginsert(G,subg);
				else subg = agsubg(G,agvsp[0].str); 
				push_subg(subg);
				In_decl = FALSE;
				}
break;
#line 787 "y.tab.c"
    }
    agssp -= agm;
    agstate = *agssp;
    agvsp -= agm;
    agm = aglhs[agn];
    if (agstate == 0 && agm == 0)
    {
#if AGDEBUG
        if (agdebug)
            printf("%sdebug: after reduction, shifting from state 0 to\
 state %d\n", AGPREFIX, AGFINAL);
#endif
        agstate = AGFINAL;
        *++agssp = AGFINAL;
        *++agvsp = agval;
        if (agchar < 0)
        {
            if ((agchar = aglex()) < 0) agchar = 0;
#if AGDEBUG
            if (agdebug)
            {
                ags = 0;
                if (agchar <= AGMAXTOKEN) ags = agname[agchar];
                if (!ags) ags = "illegal-symbol";
                printf("%sdebug: state %d, reading %d (%s)\n",
                        AGPREFIX, AGFINAL, agchar, ags);
            }
#endif
        }
        if (agchar == 0) goto agaccept;
        goto agloop;
    }
    if ((agn = aggindex[agm]) && (agn += agstate) >= 0 &&
            agn <= AGTABLESIZE && agcheck[agn] == agstate)
        agstate = agtable[agn];
    else
        agstate = agdgoto[agm];
#if AGDEBUG
    if (agdebug)
        printf("%sdebug: after reduction, shifting from state %d \
to state %d\n", AGPREFIX, *agssp, agstate);
#endif
    if (agssp >= agss + agstacksize - 1)
    {
        goto agoverflow;
    }
    *++agssp = agstate;
    *++agvsp = agval;
    goto agloop;
agoverflow:
    agerror("yacc stack overflow");
agabort:
    return (1);
agaccept:
    return (0);
}
