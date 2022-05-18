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
#include		"dot.h"

#define		NONE		0
#define		NODE		1
#define		EDGE		2
#define		CLST		3

/* ISMAP font modifiers */
#define REGULAR 0
#define BOLD		1
#define ITALIC		2

/* ISMAP patterns */
#define P_SOLID		0
#define P_NONE  15
#define P_DOTTED 4				/* i wasn't sure about this */
#define P_DASHED 11				/* or this */

#define GIFSCALE (96.0/72.0)	 /* guess 1 inch = 96 pixels = 72 points */

/* ISMAP bold line constant */
#define WIDTH_NORMAL 1
#define WIDTH_BOLD 3

static FILE	*Outfile;
static int	  Obj, N_pages;
static point	Pages;
static double   Scale;
static int	  Rot;
static box	  PB;
static int	  onetime = TRUE;

typedef struct context_t {
	char			color_ix, *fontfam, fontopt, font_was_set;
	char			pen, fill, penwidth, style_was_set;
	float		   fontsz;
} context_t;

#define MAXNEST 4
static context_t cstk[MAXNEST];
static int	  SP;

static  void
ismap_reset(void)
{
	onetime = TRUE;
}

static  void
init_ismap(void)
{
	SP = 0;
	cstk[0].color_ix = 0;		/* ISMAP color index 0-7 */
	cstk[0].fontfam = "Times";		/* font family name */
	cstk[0].fontopt = REGULAR;		/* modifier: REGULAR, BOLD or ITALIC */
	cstk[0].pen = P_SOLID;		/* pen pattern style, default is sold */
	cstk[0].fill = P_NONE;
	cstk[0].penwidth = WIDTH_NORMAL;
}

static		  pointf
ismappt(pointf p)
{
	pointf		  tmp, rv;
	tmp.x = p.x * Scale;
	tmp.y = Scale * p.y;
	if (Rot == 0) {
		rv.x = tmp.x;
		rv.y = PB.UR.y - PB.LL.y - tmp.y;
	} else {
		rv.x = PB.UR.x - PB.LL.x - tmp.y;
		rv.y = tmp.x;
	}
	return rv;
}

static  void
ismap_font(context_t* cp)
{
}

static  void
ismap_color(int i)
{
}

static  void
ismap_style(context_t* cp)
{
}

static void
ismap_begin_job(FILE *ofp, graph_t *g, char **lib, char *user, char *vers, point pages)
{
	Outfile = ofp;
	Pages = pages;
	N_pages = pages.x * pages.y;
}

static void
ismap_end_job(void)
{
}

static void
ismap_begin_graph(graph_t* g, box bb, point pb)
{
	char		   *s;

	g = g;
	PB.LL.x = bb.LL.x * GIFSCALE;
	PB.LL.y = bb.LL.y * GIFSCALE;
	PB.UR.x = bb.UR.x * GIFSCALE;
	PB.UR.y = bb.UR.y * GIFSCALE;
	if (onetime) {
		init_ismap();
		onetime = FALSE;
	}
	if ((s = agget(g, "URL")) && strlen(s)) fprintf(Outfile,"default %s\n",s);
}

static void
ismap_end_graph(void)
{
}

static  void
ismap_begin_page(point page, double scale, int rot, point offset)
{
	int			 page_number;
	point		   sz;

	Scale = scale * GIFSCALE;
	Rot = rot;
	page_number = page.x + page.y * Pages.x + 1;
	sz = sub_points(PB.UR, PB.LL);
}

static  void
ismap_end_page(void)
{
}

static  void
ismap_begin_cluster(graph_t* g)
{
	Obj = CLST;
}

static  void
ismap_end_cluster(void)
{
	Obj = NONE;
}

static  void
ismap_begin_node(node_t* n)
{
	char		   *s,*s1,*s2;
	pointf			p,p1,p2;

	Obj = NODE;
	if ((s = agget(n, "URL")) && strlen(s)) {
		p.x = n->u.coord.x - n->u.lw;
		p.y = n->u.coord.y - (n->u.ht/2);
		p1 = ismappt(p);
		p.x = n->u.coord.x + n->u.rw;
		p.y = n->u.coord.y + (n->u.ht/2);
		p2 = ismappt(p);

		s = strdup(s);
		if ((s2 = strstr(s,NODENAME_ESC))) {
			s1 = n->name;
			*s2 = '\0';
			s2 += 2;
		} else {
			s1 = s2 = "";
		}

		fprintf(Outfile,"rectangle (%d,%d) (%d,%d) %s%s%s\n",
			ROUND(p1.x),ROUND(p1.y),ROUND(p2.x),ROUND(p2.y),s,s1,s2);

		free(s);
	}
}

static  void
ismap_end_node(void)
{
	Obj = NONE;
}

static  void
ismap_begin_edge(edge_t* e)
{
	Obj = EDGE;
}

static  void
ismap_end_edge(void)
{
	Obj = NONE;
}

static  void
ismap_begin_context(void)
{
	assert(SP + 1 < MAXNEST);
	cstk[SP + 1] = cstk[SP];
	SP++;
}

static  void
ismap_end_context(void)
{
	int			 c, psp = SP - 1;
	assert(SP > 0);
	if (cstk[SP].color_ix != (c = cstk[psp].color_ix))
		ismap_color(c);
	if (cstk[SP].font_was_set)
		ismap_font(&(cstk[psp]));
	if (cstk[SP].style_was_set)
		ismap_style(&(cstk[psp]));
	/* free(cstk[psp].fontfam); */
	SP = psp;
}

static  void
ismap_set_font(char* name, double size)
{
}

static  void
ismap_arrowhead(point p, double theta, double scale,int flag)
{
}


static  void
ismap_set_color(char* name)
{
}

static  void
ismap_set_style(char** s)
{
}

static  void
ismap_textline(point p, char *str, int width, double fontsz, double align)
{
}

static  void
ismap_bezier(point* A, int n, int arrow_at_start, int arrow_at_end)
{
}

static  void
ismap_polygon(point *A, int n, int filled)
{
	pointf		  p, p0, p1;
	int			 i;

	p0.x = A[0].x; p0.y = A[0].y;
	p0 = ismappt(p0);
	p.x = p0.x; p.y = p0.y;
	for (i = 1; i < n; i++) {
		p1.x = p.x; p1.y = p.y;
		p.x = A[i].x; p.y = A[i].y;
		p = ismappt(p);
/*		gdImageLine(im, ROUND(p1.x), ROUND(p1.y), 
			ROUND(p.x), ROUND(p.y), black); */
	}
/*	gdImageLine(im, ROUND(p.x), ROUND(p.y),
		ROUND(p0.x), ROUND(p0.y), black); */
}

static  void
ismap_ellipse(point p, int rx, int ry, int filled)
{
	pointf		  mp;

	mp.x = p.x; mp.y = p.y;
	mp = ismappt(mp);
/*	gdImageArc(im, ROUND(mp.x), ROUND(mp.y),
		ROUND(Scale*(rx + rx)), ROUND(Scale*(ry + ry)), 0, 360, black); */
}

static  void
ismap_polyline(point* A, int n)
{
	pointf		  p, p1;
	int			 i;

	p.x = A[0].x;
	p.y = A[0].y;
	p = ismappt(p);
	for (i = 1; i < n; i++) {
		p1.x = A[i].x;
		p1.y = A[i].y;
		p1 = ismappt(p1);
/*		gdImageLine(im, ROUND(p.x), ROUND(p.y), ROUND(p1.x), ROUND(p1.y), black); */
		p.x = p1.x;
		p.y = p1.y;
	}
}

static  void
ismap_user_shape(char *name, point *A,int  n, int filled)
{
	static boolean  onetime = TRUE;
	if (onetime) {
		fprintf(stderr, "custom shapes not available with this driver\n");
		onetime = FALSE;
	}
	ismap_polygon(A, n, filled);
}

codegen_t	   ISMAP_CodeGen = {
	ismap_reset,
	ismap_begin_job, ismap_end_job,
	ismap_begin_graph, ismap_end_graph,
	ismap_begin_page, ismap_end_page,
	ismap_begin_cluster, ismap_end_cluster,
	ismap_begin_node, ismap_end_node,
	ismap_begin_edge, ismap_end_edge,
	ismap_begin_context, ismap_end_context,
	ismap_set_font, ismap_textline,
	ismap_set_color, ismap_set_style,
	ismap_ellipse, ismap_polygon, ismap_bezier,
	ismap_polyline, 0/* ismap_arrowhead */, ismap_user_shape//,
	//0, gif_textsize
};
