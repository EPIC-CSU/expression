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
#include	"ps.h"
#ifndef WIN32
#include <unistd.h>
#endif
#include <sys/stat.h>

#define	NONE	0
#define	NODE	1
#define	EDGE	2
#define	CLST	3

static	FILE	*Outfile;
static	int		Obj,N_pages,Cur_page;
static 	point	Pages;
static	box		PB;
static int		onetime = TRUE;

static char *Outline_and_Fill = "gsave 0 setgray stroke grestore fill\n";
static char	*Stroke = "stroke\n";
static char	*Newpath_Moveto = "newpath %d %d moveto\n";
static char	**U_lib;

typedef struct grcontext_t {
	char	*color,*font;
	double	size;
} grcontext_t;

#define STACKSIZE 8
static grcontext_t S[STACKSIZE];
static SP = 0;

static void
ps_reset(void)
{
	onetime = TRUE;
}

static void
ps_begin_job(FILE *ofp,graph_t *g, char **lib, char *user, char *vers, point pages)
{
	Outfile = ofp;
	Pages = pages;
	U_lib = lib;
		/* wrong when drawing more than one than one graph - use (atend) */
	N_pages = pages.x * pages.y;
	Cur_page = 0;
	fprintf(Outfile,"%%!PS-Adobe-2.0\n");
	fprintf(Outfile,"%%%%Creator: %s\n",vers);
	fprintf(Outfile,"%%%%For: %s\n",user);
	fprintf(Outfile,"%%%%Title: %s\n",g->name);
	fprintf(Outfile,"%%%%Pages: (atend)\n");
		/* remainder is emitted by first begin_graph */
}

static  void
ps_end_job(void)
{
	fprintf(Outfile,"%%%%Trailer\n");
	fprintf(Outfile,"%%%%Pages: %d\n",Cur_page);
	fprintf(Outfile,"end\nrestore\n");
	fprintf(Outfile,"%%%%EOF\n");
}

static void
ps_comment(void* obj, attrsym_t* sym)
{
	char	*str;
	str = late_string(obj,sym,"");
	if (str[0]) fprintf(Outfile,"%% %s\n",str);
}

static void
ps_begin_graph(graph_t* g, box bb, point pb)
{
	PB = bb;
	if (onetime) {
		fprintf(Outfile,"%%%%BoundingBox: %d %d %d %d\n",
			bb.LL.x,bb.LL.y,bb.UR.x+1,bb.UR.y+1);
		ps_comment(g,agfindattr(g,"comment"));
		fprintf(Outfile,"%%%%EndComments\n");
		cat_libfile(Outfile,U_lib,ps_lib);
		epsf_define();
		onetime = FALSE;
	}
}

static void
ps_end_graph(void)
{
}

static void
ps_begin_page(point page, double scale, int rot, point offset)
{
	point	sz;

	Cur_page++;
	sz = sub_points(PB.UR,PB.LL);
    fprintf(Outfile,"%%%%Page: %d %d\n",Cur_page,Cur_page);
    fprintf(Outfile,"%%%%PageBoundingBox: %d %d %d %d\n",
		PB.LL.x,PB.LL.y,PB.UR.x+1,PB.UR.y+1);
    fprintf(Outfile,"gsave\n%d %d %d %d boxprim clip newpath\n",
		PB.LL.x-1, PB.LL.y-1, sz.x + 2, sz.y + 2);
	fprintf(Outfile,"%d %d translate\n",PB.LL.x,PB.LL.y);
	if (rot) fprintf(Outfile,"gsave %d %d translate %d rotate\n",
		PB.UR.x-PB.LL.x,0,rot);
	fprintf(Outfile,"%d %d %d beginpage\n",page.x,page.y,N_pages);
	if (rot) fprintf(Outfile,"grestore\n");
	if (scale != 1.0) fprintf(Outfile,"%.4f set_scale\n",scale);
	fprintf(Outfile,"%d %d translate %d rotate\n",offset.x,offset.y,rot);
	assert(SP == 0);
	S[SP].font = S[SP].color = ""; S[SP].size = 0.0;
}

static void
ps_end_page(void)
{
	fprintf(Outfile,"endpage\ngrestore\n");
	fprintf(Outfile,"%%%%PageTrailer\n");
    fprintf(Outfile,"%%%%EndPage: %d\n",Cur_page);
	assert(SP == 0);
}

static void
ps_begin_cluster(graph_t* g)
{
	Obj = CLST;
}

static void
ps_end_cluster(void)
{
	Obj = NONE;
}

static void
ps_begin_node(node_t* n)
{
	Obj = NODE;
	fprintf(Outfile,"\n%%\t%s\n",n->name);
	ps_comment(n,N_comment);
}

static void
ps_end_node (void)
{
	Obj = NONE;
}

static void
ps_begin_edge (edge_t* e)
{
	Obj = EDGE;
	fprintf(Outfile,"\n%%\t%s -> %s\n",e->tail->name,e->head->name);
	ps_comment(e,E_comment);
}

static void
ps_end_edge (void)
{
	Obj = NONE;
}

static void
ps_begin_context(void)
{
	fprintf(Outfile,"gsave 10 dict begin\n");
	if (SP == STACKSIZE - 1) fprintf(stderr,"warning: psgen stk ovfl\n");
	else {SP++; S[SP] = S[SP-1];}
}

static void
ps_end_context(void)
{
	if (SP == 0) fprintf(stderr,"warning: psgen stk undfl\n");
	else SP--;
	fprintf(Outfile,"end grestore\n");
}

static void
ps_set_font(char* name, double size)
{
	if (strcmp(S[SP].font,name) || (size != S[SP].size)) {
		fprintf(Outfile,"%.2f /%s set_font\n",size,name);
		S[SP].font = name;
		S[SP].size = size;
	}
}

/***** Arrowheads now centralized in emit.c
static void
ps_arrowhead(point p, double theta, double scale, int type)
{
  fprintf(Outfile,"%d %d %.2f ", p.x,p.y,theta);
  switch (type) {
  case ARR_NORM:
    fprintf(Outfile,"%.2f %.2f arrowhead\n",
            scale*ARROW_LENGTH,scale*ARROW_WIDTH);
    break;
  case ARR_INV:
    fprintf(Outfile,"%.2f %.2f arrowinv\n",
            scale*ARROW_INV_LENGTH,scale*ARROW_INV_WIDTH);
    break;
  case ARR_DOT:
    fprintf(Outfile,"%.2f arrowdot\n",
            scale*ARROW_DOT_RADIUS);
    break;
  case ARR_ODOT:
    fprintf(Outfile,"%.2f arrowodot\n",
            scale*ARROW_DOT_RADIUS);
    break;
  case ARR_INVDOT:
    fprintf(Outfile,"%.2f %.2f %.2f arrowinvdot\n",
            scale*ARROW_INV_LENGTH,scale*ARROW_INV_WIDTH,
            scale*ARROW_DOT_RADIUS);
    break;
  case ARR_INVODOT:
    fprintf(Outfile,"%.2f %.2f %.2f arrowinvodot\n",
            scale*ARROW_INV_LENGTH,scale*ARROW_INV_WIDTH,
            scale*ARROW_DOT_RADIUS);
  }
}
**********/

static void
ps_set_color(char* name)
{
	static char *op[] = {"graph","node","edge","sethsb"};
	char	buf[SMALLBUF];

	if (strcmp(name,S[SP].color))
		fprintf(Outfile,"%s %scolor\n",colorxlate(name,buf),op[Obj]);
	S[SP].color = name;
}

static void
ps_set_style(char** s)
{
	char	*line,*p;

	while ((p = line = *s++)) {
		while (*p) p++; p++;
		while (*p) {
			fprintf(Outfile,"%s ",p);
			while (*p) p++; p++;
		}
		fprintf(Outfile,"%s\n",line);
	}
}

static char *
ps_string(char *s, char *auxbuf)
{
	char			*p = auxbuf;
	*p++ = LPAREN;
	while (*s)  {
		if ((*s == LPAREN) || (*s == RPAREN)) *p++ = '\\';
		*p++ = *s++;
	}
	*p++ = RPAREN;
	*p = '\0';
	return auxbuf;
}

static void
ps_textline(point p, char *str, int width, double fontsz, double align)
{
	int		len;
	char	sbuf[2*1024],*buf;

	if ((len = strlen(str)) < sizeof(sbuf)/2) buf = sbuf;
	else buf = (char*)malloc(2*len+1);

	ps_string(str,buf);
	fprintf(Outfile,"%d %d moveto %s %d %.2f %.2f alignedtext\n",
		p.x,p.y,buf,width,fontsz,align);
	if (buf != sbuf) free(buf);
}

static void
ps_bezier(point *A, int n, int arrow_at_start, int arrow_at_end)
{
	int		j;
	if (arrow_at_start || arrow_at_end)
		fprintf(stderr,"ps_bezier illegal arrow args\n");
	fprintf(Outfile,Newpath_Moveto,A[0].x,A[0].y);
	for (j = 1; j < n; j += 3)
		fprintf(Outfile,"%d %d %d %d %d %d curveto\n",
			A[j].x,A[j].y,A[j+1].x,A[j+1].y,A[j+2].x,A[j+2].y);
	fprintf(Outfile,Stroke);
}

static void
ps_polygon(point *A, int n, int filled)
{
	int		j;
	fprintf(Outfile,Newpath_Moveto,A[0].x,A[0].y);
	for (j = 1; j < n; j++) fprintf(Outfile,"%d %d lineto\n",A[j].x,A[j].y);
	fprintf(Outfile,"closepath\n");
	fprintf(Outfile, filled? Outline_and_Fill : Stroke);
}

static void
ps_ellipse(point p, int rx, int ry, int filled)
{
	fprintf(Outfile,"%d %d %d %d ellipse_path\n",p.x,p.y,rx,ry);
	fprintf(Outfile, filled? Outline_and_Fill : Stroke);
}

static void
ps_polyline(point* A, int n)
{
	int		j;

	fprintf(Outfile,Newpath_Moveto,A[0].x,A[0].y);
	for (j = 1; j < n; j ++) fprintf(Outfile,"%d %d lineto\n",A[j].x,A[j].y);
	fprintf(Outfile,Stroke);
}

static void
ps_user_shape(char *name, point *A, int sides, int filled)
{
	int		j;
	fprintf(Outfile,"[ ");
	for (j = 0; j < sides; j++) fprintf(Outfile,"%d %d ",A[j].x,A[j].y);
	fprintf(Outfile,"%d %d ",A[0].x,A[0].y);
	fprintf(Outfile,"]  %d %s %s\n",sides,(filled?"true":"false"),name);
}

#define N_EPSF 32
static int  N_EPSF_files;
static char *EPSF_contents[N_EPSF];

typedef struct epsf_s {
	int		macro_id;
	point	offset;
} epsf_t;

void epsf_init(node_t* n)
{
	char	*str,*contents;
	char	line[BUFSIZ];
	FILE	*fp;
	struct stat statbuf;
	int		i, saw_bb;
	int		lx,ly,ux,uy;
	epsf_t	*desc;

	str = agget(n,"shapefile");
	if (str && str[0] && (fp = fopen(str,"r"))) {
		/* try to find size */
		saw_bb = FALSE;
		while (fgets(line, sizeof(line), fp)) {
		  if (sscanf(line,"%%%%BoundingBox: %d %d %d %d",&lx,&ly,&ux,&uy) == 4) {
			saw_bb = TRUE;
			break;
		  }
		}

		if (saw_bb) {
			n->u.width = PS2INCH(ux - lx);
			n->u.height = PS2INCH(uy - ly);
			fstat(fileno(fp),&statbuf);
			i = N_EPSF_files++;
			n->u.shape_info = desc = NEW(epsf_t);
			desc->macro_id = i;
			desc->offset.x = -lx - (ux - lx)/2;
			desc->offset.y = -ly - (uy - ly)/2;
			contents = EPSF_contents[i] = (char*)malloc(statbuf.st_size);
			fseek(fp,0,SEEK_SET);
			fread(contents,statbuf.st_size,1,fp);
			fclose(fp);
		}
	}
}

void epsf_gencode(node_t *n)
{
	epsf_t	*desc;

	desc = (epsf_t*)(n->u.shape_info);
	CodeGen->begin_node(n);
	CodeGen->begin_context();
	fprintf(Outfile,"%d %d translate newpath user_shape_%d\n",
		n->u.coord.x+desc->offset.x,n->u.coord.y+desc->offset.y,
		desc->macro_id);
	CodeGen->end_context();
	CodeGen->end_node();
}

void epsf_define(void)
{
	int		i;
	char	*p;

	for (i = 0; i < N_EPSF_files; i++) {
		fprintf(Outfile,"/user_shape_%d {",i);
		p = EPSF_contents[i];
		while (*p) {
			if ((p[0] == '%') && (p[1] == '%') && (p[1] != '!')) {
				while (*p++ != '\n');
				continue;
			}
			do {fputc(*p,Outfile);} while (*p++ != '\n');
		}
		free(EPSF_contents[i]);
		fprintf(Outfile,"} bind def\n");
	}
	N_EPSF_files = 0;
}

codegen_t	PS_CodeGen = {
	ps_reset,
	ps_begin_job, ps_end_job,
	ps_begin_graph, ps_end_graph,
	ps_begin_page, ps_end_page,
	ps_begin_cluster, ps_end_cluster,
	ps_begin_node, ps_end_node,
	ps_begin_edge, ps_end_edge,
	ps_begin_context, ps_end_context,
	ps_set_font, ps_textline,
	ps_set_color, ps_set_style,
	ps_ellipse, ps_polygon,
	ps_bezier, ps_polyline,
	0/* ps_arrowhead */, ps_user_shape,
	ps_comment, 0	/* textsize */
};
