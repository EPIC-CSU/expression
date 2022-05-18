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
#include		"gd.h"
#include		"gdttf.h"
#ifdef HAVE_PATHFIND
#include		<libgen.h>				/* for pathfind() */
#else
extern char *pathfind(char*, char *, char *);
#endif

#define		NONE		0
#define		NODE		1
#define		EDGE		2
#define		CLST		3

#define		GIFSCALE	(96.0/72.0)	/* guess 1 inch = 96 pixels = 72 points */

#define BEZIERSUBDIVISION 10

/* font modifiers */
#define REGULAR 0
#define BOLD		1
#define ITALIC		2

/* patterns */
#define P_SOLID		0
#define P_NONE  15
#define P_DOTTED 4				/* i wasn't sure about this */
#define P_DASHED 11				/* or this */

/* bold line constant */
#define WIDTH_NORMAL 1
#define WIDTH_BOLD 3

typedef struct {
	unsigned char r, g, b;
} Color;


static FILE	*Outfile;
static int	  Obj, N_pages;
static point	Pages;
static double   Scale;
static int	  Rot;
static box	  PB;
static int	  onetime = TRUE;

static gdImagePtr im;

typedef struct context_t {
	char			color_ix, *fontfam, fontopt, font_was_set;
	char			pen, fill, penwidth;
	float		   fontsz;
} context_t;

#define MAXNEST 4
static context_t cstk[MAXNEST];
static int	  SP;

static void gif_reset(void)
{
	onetime = TRUE;
}

static void init_gif(void)
{
	int transparent;

	SP = 0;
	if ((transparent = gdImageGetTransparent(im)) == -1) {
		transparent = gdImageColorAllocate(im, 255, 255, 255);
		gdImageColorTransparent(im, transparent);
	}
	cstk[0].color_ix = transparent; /* set background transparent */
	cstk[0].fontfam = "times";		/* font family name */
	cstk[0].fontopt = REGULAR;		/* modifier: REGULAR, BOLD or ITALIC */
	cstk[0].pen = P_SOLID;		/* pen pattern style, default is solid */
	cstk[0].fill = P_NONE;
	cstk[0].penwidth = WIDTH_NORMAL;
}

static pointf gifpt(pointf p)
{
	pointf		  tmp, rv;
	tmp.x = p.x * Scale;
	tmp.y = Scale * p.y;
	if (Rot == 0) {
		rv.x = tmp.x;
		rv.y = PB.UR.y - PB.LL.y - tmp.y;
	} else {
		rv.x = PB.UR.x - PB.LL.x - tmp.y;
		rv.y = PB.UR.y - PB.LL.y - tmp.x;
	}
	return rv;
}

static void gif_font(context_t* cp)
{
	char		   *fw, *fa;

	fw = fa = "Regular";
	switch (cp->fontopt) {
	case BOLD:
		fw = "Bold";
		break;
	case ITALIC:
		fa = "Italic";
		break;
	}
}

static void gif_begin_job(FILE *ofp, graph_t *g, char **lib, char *user,
char *vers, point pages)
{
	Outfile = ofp;
	Pages = pages;
	N_pages = pages.x * pages.y;
}

static void gif_end_job(void) { }

static void gif_begin_graph(graph_t* g, box bb, point pb)
{
	g = g;
	PB.LL.x = bb.LL.x * GIFSCALE;
	PB.LL.y = bb.LL.y * GIFSCALE;
	PB.UR.x = bb.UR.x * GIFSCALE;
	PB.UR.y = bb.UR.y * GIFSCALE;
	if (Verbose)
		fprintf(stderr,"dot: allocating a %dK GIF image\n",
			(PB.UR.x - PB.LL.x + 2) * (PB.UR.y - PB.LL.y + 2) / 1024);
	im = gdImageCreate((PB.UR.x - PB.LL.x + 2), (PB.UR.y - PB.LL.y + 2));
	if (onetime) {
		init_gif();
		onetime = FALSE;
	}
}

static void gd_begin_graph(graph_t* g, box bb, point pb)
{
	g = g;
	PB.LL.x = bb.LL.x * GIFSCALE;
	PB.LL.y = bb.LL.y * GIFSCALE;
	PB.UR.x = bb.UR.x * GIFSCALE;
	PB.UR.y = bb.UR.y * GIFSCALE;
	im = *(gdImagePtr *)Output_file;
	if (onetime) {
		init_gif();
		onetime = FALSE;
	}
}

static void gif_end_graph(void)
{
	gdImageGif(im, Outfile);
		fflush(Outfile);
	gdImageDestroy(im);
}

static void gd_end_graph(void) { }

static void
gif_begin_page(point page, double scale, int rot, point offset)
{
	int			 page_number;
	point		sz;

	Scale = scale * GIFSCALE;
	Rot = rot;
	/* i guess we would use this if we were displaying page numbers */
	page_number = page.x + page.y * Pages.x + 1;
	sz = sub_points(PB.UR, PB.LL);
}

static void gif_end_page(void) { }

static void
gif_begin_cluster(graph_t* g)
{
	Obj = CLST;
}

static void
gif_end_cluster(void)
{
	Obj = NONE;
}

static void
gif_begin_node(node_t* n)
{
	Obj = NODE;
}

static void
gif_end_node(void)
{
	Obj = NONE;
}

static  void
gif_begin_edge(edge_t* e)
{
	Obj = EDGE;
}

static  void
gif_end_edge(void)
{
	Obj = NONE;
}

static  void
gif_begin_context(void)
{
	assert(SP + 1 < MAXNEST);
	cstk[SP + 1] = cstk[SP];
	SP++;
}

static  void
gif_end_context(void)
{
	int			 psp = SP - 1;
	assert(SP > 0);
	if (cstk[SP].font_was_set)
		gif_font(&(cstk[psp]));
	/* free(cstk[psp].fontfam); */
	SP = psp;
}

static  void
gif_set_font(char* name, double size)
{
	char		   *p, *q, buf[SMALLBUF];
	context_t	  *cp;

	cp = &(cstk[SP]);
	cp->font_was_set = TRUE;
	cp->fontsz = Scale * size;
	p = strdup(name);
	if ((q = strchr(p, '-'))) {
		*q++ = 0;
		canoncolor(q, buf);
		if (streq(buf, "italic"))
			cp->fontopt = ITALIC;
		else if (streq(q, "bold"))
			cp->fontopt = BOLD;
	}
	cp->fontfam = p;
	gif_font(&cstk[SP]);
}

/***** Arrowheads now centralized in emit.c
static  void
gif_arrowhead(point p, double theta, double scale, int flag)
{
	pointf		  p0, p1, p2;
	double		  v;
	gdPoint			points[3];

	if (cstk[SP].pen != P_NONE) {
		p0.x = p.x; p0.y = p.y;
		p0 = gifpt(p0);
		points[0].x = ROUND(p0.x); points[0].y = ROUND(p0.y);
		v = cos(RADIANS(theta+15)) * ARROW_LENGTH; p1.x = v + p.x;
		v = sin(RADIANS(theta+15)) * ARROW_LENGTH; p1.y = v + p.y;
		p1 = gifpt(p1);
		points[1].x = ROUND(p1.x); points[1].y = ROUND(p1.y);
		v = cos(RADIANS(theta-15)) * ARROW_LENGTH; p2.x = v + p.x;
		v = sin(RADIANS(theta-15)) * ARROW_LENGTH; p2.y = v + p.y;
		p2 = gifpt(p2);
		points[2].x = ROUND(p2.x); points[2].y = ROUND(p2.y);
		gdImageFilledPolygon(im, points, 3, cstk[SP].color_ix);
	}
}
******/

static void gif_set_color(char* name)
{
	double r,g,b;
	double h,s,v;
	int	R,G,B, color;
	char   result[SMALLBUF];
 
	colorxlate(name,result);
	if ((sscanf(result,"%lf %lf %lf", &h, &s, &v)) != 3) {
	  fprintf(stderr, "Unknown color %s; using black\n", name);
	  h = s = v = 0.0;
	}
	hsv2rgb(&r,&g,&b,h,s,v);
	R = (int)(r*255);
	G = (int)(g*255);
	B = (int)(b*255);

	if ((color=gdImageColorExact(im,R,G,B)) < 0)
	  if ((color=gdImageColorAllocate(im,R,G,B)) < 0)
		color=gdImageColorClosest(im,R,G,B);
	cstk[SP].color_ix = color;
}

static  void
gif_set_style(char** s)
{
	char		   *line;
	context_t	  *cp;

	cp = &(cstk[SP]);
	while ((line = *s++)) {
		if (streq(line, "solid")) {		/* no-op */
		} else if (streq(line, "dashed"))
			cp->pen = P_DASHED;
/*
 *		else if (streq(line, "dotted"))
 *			cp->pen = P_DOTTED;
 *		else if (streq(line, "bold"))
 *			cp->penwidth = WIDTH_BOLD;
 */
		else if (streq(line, "invis"))
			cp->pen = P_NONE;
		else if (streq(line, "filled"))
			cp->fill = P_SOLID;
		else if (streq(line, "unfilled")) {		/* no-op */
		} else {
			fprintf(stderr, "gif_set_style: unsupported style %s - ignoring\n",
					line);
		}
	}
}

static char	*
gif_string(char *s, char *auxbuf)
{
	char		   *p = auxbuf, esc;
	while (*s) {
#if 0
		esc = 0;
		switch (*s) {
		case '\t':
			esc = 't';
			break;
		case '>':
		case '\'':
		case '`':
		case '\\':
			esc = *s;
			break;
		}
		if (esc) {
			*p++ = '\\';
			*p++ = esc;
		} else
#endif
			*p++ = *s;
		s++;
	}
	*p = '\0';
	return auxbuf;
}

static void gif_missingfont(char *fontreq)
{
	static char				*lastmissing = 0;
	static int				n_errors = 0;

	if (n_errors >= 20) return;
	if ((lastmissing == 0) || (strcmp(lastmissing,fontreq))) {
		fprintf(stderr, "can't find font %s in %s\n",fontreq,Fontlibpath);
		if (lastmissing) free(lastmissing);
		lastmissing = strdup(fontreq);
		n_errors++;
		if (n_errors >= 20) fprintf(stderr,"(font errors suppressed)\n");
	}
}

static char *get_ttf_fontpath(char *fontreq, int warn)
{
	char  fontfile[BUFSIZ],*fontpath,*backup;

	sprintf(fontfile,"%s.ttf",fontreq);
	fontpath = pathfind(Fontlibpath, fontfile, "r");
	if (fontpath == 0) {
		backup = 0;
  //NOS
		/*
  if (strcasecmp(fontreq,"Times-Roman")==0) backup = "times.ttf";
		else if (strcasecmp(fontreq,"Times")==0) backup = "times.ttf";
		else if (strcasecmp(fontreq,"Helvetica")==0) backup = "arial.ttf";
		else if (strcasecmp(fontreq,"Courier")==0) backup = "cour.ttf";
  */
		if (stricmp(fontreq,"Times-Roman")==0) backup = "times.ttf";
		else if (stricmp(fontreq,"Times")==0) backup = "times.ttf";
		else if (stricmp(fontreq,"Helvetica")==0) backup = "arial.ttf";
		else if (stricmp(fontreq,"Courier")==0) backup = "cour.ttf";
		if (backup)
			fontpath = pathfind(Fontlibpath, backup, "r");
	}
	if ((fontpath == 0) && warn)
		gif_missingfont(fontreq);
	return fontpath;
}

static  void
gif_textline(point p, char *str, int width, double fontsz, double align)
{
	char		buf[BUFSIZ];
	char		*fontfilename;
	pointf		mp;
	int			intfontsz, brect[8];
	double		wrongness;

/* because the low-level renderer really only deals with integer font sizes,
 *  we need to compensate for the roundoff error in the "real" width.
 *  ...maybe this could be done by a different computation of font_scale_adj?
 */


	fontfilename = get_ttf_fontpath(cstk[SP].fontfam, (*str != '\0'));
	if (fontfilename) {
		fontsz *= Scale;
		intfontsz = (int)(fontsz);
		wrongness = ((fontsz - intfontsz)/fontsz) * width;
		gif_string(str,buf);
		if (align == -.5) mp.x = p.x - width/2.0; /* normal - center on p*/
		else if (align < 0.0) mp.x = p.x - width; /* right justify - left of p */
		else mp.x = p.x;														/* left justify - right of p */
		/* mp.x = mp.x + 5; */
#if 0
		mp.y = p.y - fontsz/2.0 + 2.0; /* was -2 */
#else
		mp.y = p.y - MAX(fontsz/2.0, 4);
#endif
		mp.x = mp.x + wrongness;
		mp = gifpt(mp);

		gdttf(im, brect, cstk[SP].color_ix, fontfilename, 
			(double)(intfontsz), (Rot? 90.0 : 0.0) * PI / 180.0,
			ROUND(mp.x), ROUND(mp.y), gif_string(str,buf));
	}
}

point gif_textsize(char *str, char *fontname, double fontsz)
{
	char		*fontfilename,buf[BUFSIZ];
	point		rv;
	int			brect[8];

	fontfilename = get_ttf_fontpath(fontname, (*str != '\0'));
	if (fontfilename && *str) {
/* call gdttf with invalid color to get brect (gdTransparent is not invalid) */
		gdttf(NULL, brect, -1, fontfilename, 
			ROUND(fontsz), 0, 0, 0, gif_string(str,buf));
		rv.x = (brect[2] - brect[0])  /* * 90.0 / 72.0 */;
		rv.y = (brect[3] - brect[1])  /* * 90.0 / 72.0 */;
	}
	else {
		rv.x = rv.y = 0;
	}
	return rv;
}

static  void
gif_bezier(point* A, int n, int arrow_at_start, int arrow_at_end)
{
	pointf		  p0, p1, V[4];
	int			 i, j, step;

	if (cstk[SP].pen != P_NONE) {
		V[3].x = A[0].x; V[3].y = A[0].y;
		for (i = 0; i+3 < n; i += 3) {
			V[0] = V[3];
			for (j = 1; j <= 3; j++) {
				V[j].x  = A[i+j].x; V[j].y = A[i+j].y;
			}
			p0 = gifpt(V[0]); 
			for (step = 1; step <= BEZIERSUBDIVISION; step++) {
				p1 = gifpt(Bezier(V, 3, (double)step/BEZIERSUBDIVISION, NULL, NULL));
				if (cstk[SP].pen == P_DASHED) {
					gdImageDashedLine(im, ROUND(p0.x), ROUND(p0.y),
						ROUND(p1.x), ROUND(p1.y), cstk[SP].color_ix);
				}
				else {
					gdImageLine(im, ROUND(p0.x), ROUND(p0.y),
						ROUND(p1.x), ROUND(p1.y), cstk[SP].color_ix);
				}
				p0 = p1;
			}
		}
	}
}

static  void
gif_polygon(point *A, int n, int filled)
{
	pointf		  p;
	int			 i;
	gdPoint		points[500]; /* Naughty.  Should allocate n */

	for (i = 0; i < n; i++) {
		p.x = A[i].x; p.y = A[i].y;
		p = gifpt(p);
		points[i].x = ROUND(p.x); points[i].y = ROUND(p.y);
	}
	if (filled) {
		gdImageFilledPolygon(im, points, n, cstk[SP].color_ix);
	} else {
		gdImagePolygon(im, points, n, cstk[SP].color_ix);
	}
}

static  void
gif_ellipse(point p, int rx, int ry, int filled)
{
	pointf		  mp;

	if (cstk[SP].pen != P_NONE) {
		if (Rot) {int t; t = rx; rx = ry; ry = t;}
		mp.x = p.x; mp.y = p.y;
		mp = gifpt(mp);
		gdImageArc(im, ROUND(mp.x), ROUND(mp.y),
			ROUND(Scale*(rx + rx)), ROUND(Scale*(ry + ry)),
			0, 360, cstk[SP].color_ix);
		if (filled) {
			gdImageFillToBorder(im, ROUND(mp.x), ROUND(mp.y),
				cstk[SP].color_ix, cstk[SP].color_ix);
		}
	}
}

static  void
gif_polyline(point* A, int n)
{
	pointf		  p, p1;
	int			 i;

	if (cstk[SP].pen != P_NONE) {
		p.x = A[0].x;
		p.y = A[0].y;
		p = gifpt(p);
		for (i = 1; i < n; i++) {
			p1.x = A[i].x;
			p1.y = A[i].y;
			p1 = gifpt(p1);
			if (cstk[SP].pen == P_DASHED) {
				gdImageDashedLine(im, ROUND(p.x), ROUND(p.y),
					ROUND(p1.x), ROUND(p1.y), cstk[SP].color_ix);
			} else {
				gdImageLine(im, ROUND(p.x), ROUND(p.y),
					ROUND(p1.x), ROUND(p1.y), cstk[SP].color_ix);
			}
			p.x = p1.x;
			p.y = p1.y;
		}
	}
}

static  void
gif_user_shape(char *name, point *A, int n, int filled)
{
	static boolean  onetime = TRUE;
	if (onetime) {
		fprintf(stderr, "custom shapes not available with this driver\n");
		onetime = FALSE;
	}
	gif_polygon(A, n, filled);
}

codegen_t	   GIF_CodeGen = {
	gif_reset,
	gif_begin_job, gif_end_job,
	gif_begin_graph, gif_end_graph,
	gif_begin_page, gif_end_page,
	gif_begin_cluster, gif_end_cluster,
	gif_begin_node, gif_end_node,
	gif_begin_edge, gif_end_edge,
	gif_begin_context, gif_end_context,
	gif_set_font, gif_textline,
	gif_set_color, gif_set_style,
	gif_ellipse, gif_polygon,
	gif_bezier, gif_polyline,
	0/* gif_arrowhead */, gif_user_shape,
	0 /* gif comment */, gif_textsize
};

codegen_t	   GD_CodeGen = {				/* see tcldot */
	gif_reset,
	gif_begin_job, gif_end_job,
	gd_begin_graph, gd_end_graph,  /* use open GD structure */
	gif_begin_page, gif_end_page,
	gif_begin_cluster, gif_end_cluster,
	gif_begin_node, gif_end_node,
	gif_begin_edge, gif_end_edge,
	gif_begin_context, gif_end_context,
	gif_set_font, gif_textline,
	gif_set_color, gif_set_style,
	gif_ellipse, gif_polygon,
	gif_bezier, gif_polyline,
	0/* gif_arrowhead */, gif_user_shape,
	0 /* gif comment */, gif_textsize
};
