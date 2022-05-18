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

extern Dtmethod_t*	Dttree;

typedef struct printdict_t {
	Dict_t	*nodesleft, *edgesleft, *subgleft, *e_insubg, *n_insubg;
} printdict_t;


Agraph_t *agread(FILE* fp)
{
	aglexinit(fp);
	agparse();
	return AG.parsed_g;
}

int agerrors(void)
{
	return AG.syntax_errors;
}

/*
 * canonicalize a string for printing.
 * changes to the semantics of this function
 * also involve the string scanner in lexer.c
 */
char *agstrcanon(char* arg, char* buf)
{
	char	*s = arg;
	char	*p = buf;
	int		cnt = 0;
	int		has_special = FALSE;
	int		maybe_num = (isdigit(arg[0]) || (arg[0] == '.'));

	if (ISEMPTYSTR(arg)) return "\"\"";
	*p++ = '\"';
	while (*s) {
		if (*s == '\"') { *p++ = '\\'; has_special = TRUE; }
		else {
			if (!isalnum(*s) && ((*s != '_'))) has_special = TRUE;
			else if (maybe_num && (!isdigit(*s) && (*s != '.')))
				has_special = TRUE;
		}
		*p++ = *s++;
		cnt++;
		if (cnt % SMALLBUF == 0) {*p++ = '\\'; *p++ = '\n';}
	}
	*p++ = '\"'; *p = '\0';
	if (has_special) return buf;

	/* use quotes to protect tokens (example, a node named "node") */
	if (agtoken(arg) >= 0) return buf;
	return arg;
}

static void tabover(FILE* fp, int tab)
{
	while (tab--) putc('\t',fp);
}

static void write_dict(Agdict_t* dict, FILE* fp)
{
	int			i,cnt = 0;
	Agsym_t		*a;

	for (i = 0; i < dtsize(dict->dict); i++) {
		a = dict->list[i];
		if (ISEMPTYSTR(a->value) == FALSE) {
			if (cnt++ == 0) fprintf(fp, "\t%s [", dict->name);
			else fprintf(fp,",\n\t");
			fprintf(fp,"\t%s = %s",a->name,agstrcanon(a->value,AG.linebuf));
		}
	}
	if (cnt > 0) fprintf(fp," ];\n");
}

static void write_diffattr(FILE* fp, int indent, void* obj, void* par, Agdict_t* dict)
{
	Agsym_t*	a;
	int			i;
	char		*p,*q;

	for (i = 0; i < dtsize(dict->dict); i++) {
		a = dict->list[i];
		if (a->printed == FALSE) continue;
		p = agxget(obj,a->index);
		if (par) q = agxget(par,a->index);
		else q = a->value;
		if (strcmp(p,q)) {
			tabover(fp,indent);
			fprintf(fp,"%s [%s",dict->name,agstrcanon(a->name,AG.linebuf));
			fprintf(fp,"= %s];\n",agstrcanon(p,AG.linebuf));
		}
	}
}

static void writeattr(FILE	*fp, int *npp, char	*name, char *val, char *buf)
{
	fprintf(fp,++(*npp) > 1?", " : " [");
	fprintf(fp,"%s=",agstrcanon(name,buf));
	fprintf(fp,"%s",agstrcanon(val,buf));
}

void agwrnode(Agraph_t* g, FILE* fp, Agnode_t* n, int full, int indent)
{
	char		*myval,*defval,*buf=AG.linebuf;
	int  		i,didwrite = FALSE;
	int			nprint = 0;
	Agdict_t	*d = n->graph->univ->nodeattr;
	Agsym_t	*a;

	if (full) {
		for (i = 0; i < dtsize(d->dict); i++) {
			a = d->list[i];
			if (a->printed == FALSE) continue;
			myval = agget(n,a->name);
			if (g == n->graph) defval = a->value;
			else defval = agget(g->proto->n,a->name);
			if (strcmp(defval,myval)) {
				if (didwrite == FALSE) {
					tabover(fp,indent);
					fprintf(fp,"%s",agstrcanon(n->name,buf));
					didwrite = TRUE;
				}
				writeattr(fp,&nprint,a->name,myval,buf);
			}
		}
		if (didwrite) {
			fprintf(fp,(nprint>0? "];\n" : ";\n"));
			return;
		}
	}
	if ((agfstout(g,n) == NULL) && (agfstin(g,n) == NULL)) {
		tabover(fp,indent);
		fprintf(fp,"%s;\n",agstrcanon(n->name,buf));
	}
}

void agwredge(Agraph_t* g, FILE* fp, Agedge_t* e, int list_all)
{
	char		*myval,*defval,*buf=AG.linebuf,*edgeop,*tport,*hport;
	int			i,nprint = 0;
	Agdict_t	*d = e->tail->graph->univ->edgeattr;
	Agsym_t	*a;

	if (e->attr) {tport = e->attr[TAILX]; hport = e->attr[HEADX];}
	else tport = hport = "";
	if (g->kind && AGFLAG_DIRECTED) edgeop = "->"; else edgeop = "--";
	fprintf(fp,"%s%s %s",agstrcanon(e->tail->name,buf),tport,edgeop);
	fprintf(fp," %s%s",agstrcanon(e->head->name,buf),hport);
	if (list_all) {
		for (i = 0; i < dtsize(d->dict); i++) {
			a = d->list[i];
			if ((a->printed == FALSE)||((i == KEYX) && (e->printkey != MUSTPRINT)))
				continue;
			myval = agget(e,a->name);
			if (g == g->root) defval = a->value;
			else defval = agget(g->proto->e,a->name);
			if (strcmp(defval,myval)) writeattr(fp,&nprint,a->name,myval,buf);
		}
	}
	fprintf(fp,(nprint>0? "];\n" : ";\n"));
}

Dtdisc_t agEdgedisc = {
	offsetof(Agedge_t,id),
	sizeof(int),
	-1,
	NIL(Dtmake_f),
	NIL(Dtfree_f),
	NIL(Dtcompar_f),			/* use memcmp */
	NIL(Dthash_f),
	NIL(Dtmemory_f),
	NIL(Dtevent_f)
};
static void
write_subg(Agraph_t* g, FILE* fp, Agraph_t* par, int indent, printdict_t* state)
{
	char			*buf = AG.linebuf;
	Agraph_t		*subg,*meta;
	Agnode_t		*n;
	Agedge_t		*e;
	Dict_t			*save_e, *save_n;

	if (indent) {
		tabover(fp,indent++);
		if (dtsearch(state->subgleft,g->meta_node)) {
			if (strncmp(g->name,"_anonymous",10))
				fprintf(fp,"subgraph %s {\n",agstrcanon(g->name,buf));
			else fprintf(fp,"{\n");	/* no name printed for anonymous subg */
			write_diffattr(fp,indent,g,par,g->univ->globattr);
			write_diffattr(fp,indent,g->proto->n,par->proto->n,g->univ->nodeattr);
			write_diffattr(fp,indent,g->proto->e,par->proto->e,g->univ->edgeattr);
			dtdelete(state->subgleft,g->meta_node);
		}
		else {
			fprintf(fp,"subgraph %s;\n",agstrcanon(g->name,buf));
			return;
		}
	}
	else write_diffattr(fp,++indent,g,NULL,g->univ->globattr);

	save_n = state->n_insubg;
	save_e = state->e_insubg;
	meta = g->meta_node->graph;
	state->n_insubg = dtopen(&agNamedisc,Dttree);
	state->e_insubg = dtopen(&agOutdisc,Dttree);
	for (e = agfstout(meta,g->meta_node); e; e = agnxtout(meta,e)) {
		subg = agusergraph(e->head);
		write_subg(subg,fp,g,indent,state);
	}
	for (n = agfstnode(g); n; n = agnxtnode(g,n)) {
		if (dtsearch(state->nodesleft,n)) {
			agwrnode(g,fp,n,TRUE,indent);
			dtdelete(state->nodesleft,n);
		}
		else {
			if (dtsearch(state->n_insubg,n) == NULL) {
				agwrnode(g,fp,n,FALSE,indent);
			}
		}
		dtinsert(save_n,n);
	}

	dtdisc(g->outedges,&agEdgedisc,0);	/* sort by id */
	for (e = (Agedge_t*)dtfirst(g->outedges); e; e = (Agedge_t*)dtnext(g->outedges,e)) {
		if (dtsearch(state->edgesleft,e)) {
			tabover(fp,indent);
			agwredge(g,fp,e,TRUE);
			dtdelete(state->edgesleft,e);
		}
		else {
			if (dtsearch(state->e_insubg,e) == NULL) {
				tabover(fp,indent);
				agwredge(g,fp,e,FALSE);
			}
		}
		dtinsert(save_e,e);
	}
	dtdisc(g->outedges,&agOutdisc,0);	/* sort by name */
	dtclose(state->n_insubg); state->n_insubg = save_n;
	dtclose(state->e_insubg); state->e_insubg = save_e;

	if (indent > 1) {
		tabover(fp,indent-1);
		fprintf(fp,"}\n");
	}
}

static Dict_t	*Copy;
static int copydictf( Dict_t	*d, void *a, void *ignored)
{
	dtinsert(Copy,(Agsym_t*)a);
	return 0;
}

static void copydict(Dict_t* from, Dict_t* to)
{
	Copy = to;
	dtwalk(from,copydictf,0);
}

static printdict_t *new_printdict_t(Agraph_t* g)
{
	printdict_t	*rv = NEW(printdict_t);
	rv->nodesleft	= dtopen(&agNodedisc,Dttree);
	copydict(g->nodes,rv->nodesleft);
	rv->edgesleft	= dtopen(&agEdgedisc,Dttree);
	copydict(g->outedges,rv->edgesleft);
	rv->n_insubg	= dtopen(&agNodedisc,Dttree);
	rv->e_insubg	= dtopen(&agOutdisc,Dttree);
	rv->subgleft	= dtopen(&agNodedisc,Dttree);
	copydict(g->meta_node->graph->nodes,rv->subgleft);
	return rv;
}

static void free_printdict_t(printdict_t* dict)
{
	dtclose(dict->nodesleft);
	dtclose(dict->n_insubg);
	dtclose(dict->edgesleft);
	dtclose(dict->e_insubg);
	dtclose(dict->subgleft);
	free(dict);
}

int agwrite(Agraph_t* g, FILE* fp)
{
	printdict_t		*p;
	char			*buf,*t0,*t1;

	if (AG.linebuf == NULL)
		aglexinit(stdin);
	buf = AG.linebuf;

	/* write the graph header */
	t0 = (AG_IS_STRICT(g)) ? "strict " : "";
	t1 = (AG_IS_DIRECTED(g)) ? "digraph" : "graph";
	agstrcanon(g->name,buf);
	fprintf(fp, "%s%s %s {\n", t0, t1, buf);

	/* write the top level attribute defs */
	write_dict(g->univ->globattr,fp);
	write_dict(g->univ->nodeattr,fp);
	write_dict(g->univ->edgeattr,fp);

	/* write the graph contents */
	p = new_printdict_t(g);
	write_subg(g,fp,(Agraph_t*)0,0,p);
	fprintf(fp,"}\n");
	free_printdict_t(p);
	return ferror(fp);
}
