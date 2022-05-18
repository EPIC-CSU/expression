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

#include "stdafx.h"
#include	"dot.h"

static boolean	Flip;
static point	Offset;

#define M1 \
	"/pathbox { /Y exch %d sub def /X exch %d sub def /y exch %d sub def /x exch %d sub def newpath x y moveto X y lineto X Y lineto x Y lineto closepath stroke } def\n"
#define M2 \
	"/pathbox { /X exch neg %d sub def /Y exch %d sub def /x exch neg %d sub def /y exch %d sub def newpath x y moveto X y lineto X Y lineto x Y lineto closepath stroke } def\n"

void dot_postprocess(graph_t* g)
{
	if ((Flip = g->u.left_to_right)) {
		if (g->u.label) {
			int		yd = POINTS(g->u.label->dimen.x);
			g->u.bb.LL.x -= POINTS(g->u.label->dimen.y);
				/* in case label is wide than the rest of the drawing */
			if (yd > g->u.bb.UR.y - g->u.bb.LL.y) {
				yd = yd/2;
				g->u.bb.LL.y -= yd; g->u.bb.UR.y += yd;
			}
		}
		Offset.x = -g->u.bb.UR.y;
		Offset.y = g->u.bb.LL.x;
	}
	else {
		if (g->u.label) {
			int		xd = POINTS(g->u.label->dimen.x);
			g->u.bb.LL.y -= POINTS(g->u.label->dimen.y);
			if (xd > g->u.bb.UR.x - g->u.bb.LL.x) {
				xd = xd/2;
				g->u.bb.LL.x -= xd; g->u.bb.UR.x += xd;
			}
		}
		Offset = g->u.bb.LL;
	}
	translate_drawing(g);
	place_graph_label(g);

	if (Show_boxes) {
		if (Flip)
			fprintf (stderr, M2, Offset.x, Offset.y, Offset.x, Offset.y);
		else
			fprintf (stderr, M1, Offset.y, Offset.x, Offset.y, Offset.x);
	}
}

point
map_point(point p)
{
	int		x = p.x;
	int 	y = p.y;

	if (Flip) { p.x = -y - Offset.x; p.y = x - Offset.y; }
	else { p.x = x - Offset.x; p.y = y - Offset.y; }
	return p;
}

void
map_edge(edge_t* e)
{
	int			j,k;
	bezier		bz;

if (e->u.spl == NULL) {
	if ((Concentrate == FALSE) || (e->u.edge_type != IGNORED))
		fprintf(stderr,"lost %s %s edge\n",e->tail->name,e->head->name);
	return;
}
	for (j = 0; j < e->u.spl->size; j++) {
		bz = e->u.spl->list[j];
		for (k = 0; k < bz.size; k++)
			bz.list[k] = map_point(bz.list[k]);
		if (bz.sflag)
			e->u.spl->list[j].sp = map_point (e->u.spl->list[j].sp);
		if (bz.eflag)
			e->u.spl->list[j].ep = map_point (e->u.spl->list[j].ep);
	}
	if (e->u.label) e->u.label->p = map_point(e->u.label->p);
    /* vladimir */
	if (e->u.head_label) e->u.head_label->p = map_point(e->u.head_label->p);
	if (e->u.tail_label) e->u.tail_label->p = map_point(e->u.tail_label->p);
}

void translate_bb(graph_t* g, int lr)
{
	int			c;
	box			bb,new_bb;

	bb = g->u.bb;
	if (lr) {
		new_bb.LL = map_point(pointof(bb.LL.x,bb.UR.y));
		new_bb.UR = map_point(pointof(bb.UR.x,bb.LL.y));
	}
	else {
		new_bb.LL = map_point(pointof(bb.LL.x,bb.LL.y));
		new_bb.UR = map_point(pointof(bb.UR.x,bb.UR.y));
	}
	g->u.bb = new_bb;
	for (c = 1; c <= g->u.n_cluster; c++) translate_bb(g->u.clust[c],lr);
}

void translate_drawing(graph_t* g)
{
	node_t		*v;
	edge_t		*e;

	for (v = agfstnode(g); v; v = agnxtnode(g,v)) {
		nodesize(v,FALSE);
		v->u.coord = map_point(v->u.coord);
		for (e = agfstout(g,v); e; e = agnxtout(g,e)) map_edge(e);
	}
	translate_bb(g,g->u.left_to_right);
}

void osize_label(textlabel_t *label, int *b, int* t ,int *l, int *r)
{
	point	pt,sz2;
	sz2.x = POINTS(label->dimen.x)/2;
	sz2.y = POINTS(label->dimen.y)/2;
	pt = add_points(label->p,sz2);
	if (*r < pt.x) *r = pt.x;
	if (*t < pt.y) *t = pt.y;
	pt = sub_points(label->p,sz2);
	if (*l > pt.x) *l = pt.x;
	if (*b > pt.y) *b = pt.y;
}

void place_graph_label(graph_t* g)
{
	int			c,maxy;
	point		p,d;

	if (g->u.label) {
		d = cvt2pt(g->u.label->dimen);
		if (g == g->root) {
			p.x = (g->u.bb.LL.x + g->u.bb.UR.x)/2;
			p.y = g->u.bb.LL.y + d.y/2;
		}
		else {	/* cluster */
			p.x = g->u.bb.LL.x + d.x/2;
			p.y = g->u.bb.UR.y + d.y/2;
			maxy = g->u.bb.UR.y + d.y;
			if (g->root->u.bb.UR.y < maxy) g->root->u.bb.UR.y = maxy;
		}
		g->u.label->p = p;
	}
	for (c = 1; c <= g->u.n_cluster; c++)
		place_graph_label(g->u.clust[c]);
}

void label_adj(Agraph_t *g, textlabel_t *lp)
{
	int		line;
	double	actual, adjust;

	if (lp) {
		for (line = 0; line < lp->nlines; line++) {
			actual = CodeGen->textsize(lp->line[line].str,
				lp->fontname,(double)(lp->fontsize)).x;
			if (actual > 0.0) {
				adjust = lp->line[line].width / actual;
				if (g->root->u.drawing->font_scale_adj > adjust)
					g->root->u.drawing->font_scale_adj = adjust;
			}
		}
	}
}

void graph_label_adj(Agraph_t *g)
{
	int	c;

	label_adj(g,g->u.label);
	for (c = 1; c <= g->u.n_cluster; c++)
		graph_label_adj(g->u.clust[c]);
}

void scan_label_adjustments(Agraph_t *g)
{
	Agnode_t	*n;
	Agedge_t	*e;

	if (CodeGen && CodeGen->textsize) {
		graph_label_adj(g);
		for (n = agfstnode(g); n; n = agnxtnode(g,n)) {
			label_adj(g,n->u.label);
			for (e = agfstout(g,n); e; e = agnxtout(g,e))
				label_adj(g,e->u.label);
		}
	}
}
