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

//NOS
void emit_arrowhead (point p, double theta, double scale, int flag);

void dot_write(graph_t* g)
{
	switch (Output_lang) {
 case POSTSCRIPT:
 case HPGL:
 case PCL:
 case MIF:
 case GIF:
 case ISMAP:
  emit_graph(g); break;
 case ATTRIBUTED_DOT:
  attach_attrs(g);
  agwrite(g,Output_file); break;
 case CANONICAL_DOT:
  agwrite(g,Output_file); break;
 case PLAIN:		
  attach_attrs(g); write_plain(g,Output_file); break;
	}
	fflush(Output_file);
}

static char *rectbufp;
static void set_record_rects (node_t* n, field_t* f)
{
	int             i;
 
	if (f->n_flds == 0) {
		sprintf(rectbufp, "%d,%d,%d,%d ",
   f->b.LL.x + n->u.coord.x,
   f->b.LL.y + n->u.coord.y,
   f->b.UR.x + n->u.coord.x,
   f->b.UR.y + n->u.coord.y);
		while (*rectbufp) rectbufp++;
	}
	for (i = 0; i < f->n_flds; i++)
		set_record_rects (n, f->fld[i]);
}

static attrsym_t *safe_dcl(graph_t *g, void *obj, char *name, char *def,
                           attrsym_t*(*fun)(Agraph_t*, char*, char*))
{
	attrsym_t	*a = agfindattr(obj,name);
	if (a == NULL) a = fun(g,name,def);
	return a;
}

void attach_attrs(graph_t* g)
{
	int		i,j;
	char	buf[BUFSIZ],*p;
	node_t	*n;
	edge_t	*e;
	point	pt;
 
	safe_dcl(g,g->proto->n,"pos","",agnodeattr);
	safe_dcl(g,g->proto->n,"rects","",agnodeattr);
	N_width = safe_dcl(g,g->proto->n,"width","",agnodeattr);
	N_height = safe_dcl(g,g->proto->n,"height","",agnodeattr);
	safe_dcl(g,g->proto->e,"pos","",agedgeattr);
	if (g->u.has_edge_labels) safe_dcl(g,g->proto->e,"lp","",agedgeattr);
	if (g->u.label) {
		safe_dcl(g,g,"lp","",agraphattr);
		pt = g->u.label->p;
		sprintf(buf,"%d,%d",pt.x,pt.y);
		agset(g,"lp",buf);
	}
	safe_dcl(g,g,"bb","",agraphattr);
	for (n = agfstnode(g); n; n = agnxtnode(g,n)) {
		sprintf(buf,"%d,%d",n->u.coord.x,n->u.coord.y);
		agset(n,"pos",buf);
		sprintf(buf,"%.2f",PS2INCH(n->u.ht));
		agxset(n,N_height->index,buf);
		sprintf(buf,"%.2f",PS2INCH(n->u.lw + n->u.rw));
		agxset(n,N_width->index,buf);
		if (strcmp (n->u.shape->name, "record") == 0) {
			buf[0] = '\000', rectbufp = &buf[0];
			set_record_rects (n, (struct field_t *)n->u.shape_info);
			if (rectbufp > &buf[0]) /* get rid of last space */
				*(--rectbufp) = '\000';
			agset(n,"rects",buf);
		}
		for (e = agfstout(g,n); e; e = agnxtout(g,e)) {
			p = buf;
   if (e->u.spl == NULL)
   {fprintf(stderr,"lost spline of %s %s\n",e->tail->name,e->head->name); continue;}
			for (i = 0; i < e->u.spl->size; i++) {
				if (i > 0) *p++ = ';';
				if (e->u.spl->list[i].sflag) {
					sprintf (p, "s,%d,%d ",e->u.spl->list[i].sp.x,e->u.spl->list[i].sp.y);
					while (*p) p++;
				}
				if (e->u.spl->list[i].eflag) {
					sprintf (p, "e,%d,%d ",e->u.spl->list[i].ep.x,e->u.spl->list[i].ep.y);
					while (*p) p++;
				}
				for (j = 0; j < e->u.spl->list[i].size; j++) {
					if (j > 0) *p++ = ' ';
					pt = e->u.spl->list[i].list[j];
					sprintf(p,"%d,%d",pt.x,pt.y);
					while (*p) p++;
				}
				*p = '\0';
			}
			agset(e,"pos",buf);
			if (e->u.label) {
				pt = e->u.label->p;
				sprintf(buf,"%d,%d",pt.x,pt.y);
				agset(e,"lp",buf);
			}
		}
	}
	rec_attach_bb(g);
}


void rec_attach_bb(graph_t* g)
{
	int		c;
	char	buf[32];
 
	sprintf(buf,"%d,%d,%d,%d", g->u.bb.LL.x, g->u.bb.LL.y,
		g->u.bb.UR.x, g->u.bb.UR.y);
	agset(g,"bb",buf);
	for (c = 1; c <= g->u.n_cluster; c++) rec_attach_bb(g->u.clust[c]);
}


//this is added here to emit descriptions for cluster bounding boxes
//this is a modified version of void emit_clusters(graph_t* g)
void my_emit_clusters(graph_t* g, FILE* f)
{
 int			i,c,filled;
 graph_t		*subg;
 point		A[4];
 char		*str,**style;
 
 for (c = 1; c <= g->u.n_cluster; c++) 
 {
  subg = g->u.clust[c];
  if (clust_in_layer(subg) == FALSE) 
  {
   continue;
  }
  
  //CodeGen->begin_cluster(subg);
  //CodeGen->begin_context();
  filled = FALSE;
  if ((str = agget(subg,"style")) && str[0]) 
  {
   ASSERT(0);//,"I didn't specify any filling for this pie!");
   //CodeGen->set_style(style = parse_style(str));
   for (i = 0; style[i]; i++) 
   {
    if (strcmp(style[i],"filled")==0) 
    {
     filled = TRUE; 
     break;
    }
   }
  }
  
  //emit_background(subg, subg->u.bb.LL, subg->u.bb.UR);
  
  if ((str = agget(subg,"color")) && str[0])
  {
   ASSERT(0);//,"I didn't specify any crust for this pie!");
   //CodeGen->set_color(str);
  }
  
  A[0] = subg->u.bb.LL;
  A[2] = subg->u.bb.UR;
  A[1].x = A[2].x; A[1].y = A[0].y;
  A[3].x = A[0].x; A[3].y = A[2].y;
  //CodeGen->polygon(A,4,filled);
  
  fprintf(f,"cluster %s %.3f %.3f %.3f %.3f %.3f %.3f %.3f %.3f ",subg->name, 
   PS2INCH(A[0].x),PS2INCH(A[0].y),
   PS2INCH(A[1].x),PS2INCH(A[1].y), 
   PS2INCH(A[2].x),PS2INCH(A[2].y), 
   PS2INCH(A[3].x),PS2INCH(A[3].y));
  //fprintf(f," %.3f %.3f\n",PS2INCH(bz.sp.x),PS2INCH(bz.sp.y));
  
  
  if (subg->u.label)
  {
   //emit_label(subg->u.label,subg);
   fprintf(f,"%s", subg->u.label->text);
  }
  
  fprintf(f,"\n");
  
  //CodeGen->end_context();
  //CodeGen->end_cluster();
  
  //recurse
  my_emit_clusters(subg,f);
 }
} 


void write_plain(graph_t* g, FILE* f)
{
 int			i;
 node_t		*n;
 edge_t		*e;
 bezier		bz;
 char		buf[SMALLBUF],buf1[SMALLBUF];//,buf2[SMALLBUF];
 
 setup_graph(g);
 fprintf(f,"graph %.3f",g->u.drawing->scale); printptf(f,g->u.bb.UR); fprintf(f,"\n");
 for (n = agfstnode(g); n; n = agnxtnode(g,n)) {
  fprintf(f,"node %s ",agstrcanon(n->name,buf)); printptf(f,n->u.coord);
  fprintf(f," %.3f %.3f %s %s %s %s\n",
   n->u.width,n->u.height,agstrcanon(n->u.label->text,buf),
   late_nnstring(n,N_style,"solid"),
   n->u.shape->name,
   late_nnstring(n,N_color,DEFAULT_COLOR));
 }
 for (n = agfstnode(g); n; n = agnxtnode(g,n)) 
 {
  for (e = agfstout(g,n); e; e = agnxtout(g,e)) 
  {
   bz = e->u.spl->list[0];
   //fprintf(f,"edge %s %s %s %d",agstrcanon(e->tail->name,buf),agstrcanon(e->head->name,buf1),agstrcanon(e->u.label->text,buf2),bz.size);
   fprintf(f,"edge %s %s %d",agstrcanon(e->tail->name,buf),agstrcanon(e->head->name,buf1),bz.size);
   for (i = 0; i < bz.size; i++) 
   {
    printptf(f,bz.list[i]);
   }
   /*
   if (e->u.label) 
   {
   fprintf(f," %s",agstrcanon(e->u.label->text,buf));
   printptf(f,e->u.label->p);
   }
   */
   fprintf(f," %s %s",late_nnstring(e,E_style,"solid"),	late_nnstring(e,E_color,DEFAULT_COLOR));
   //NOS to add add arrow to edges output the end or start point of the bezier curve
   int scale=1;
   if (bz.sflag) 
   {
    fprintf(f," %.3f %.3f\n",PS2INCH(bz.sp.x),PS2INCH(bz.sp.y));
   }
   if (bz.eflag) 
   {
    fprintf(f," %.3f %.3f\n",PS2INCH(bz.ep.x),PS2INCH(bz.ep.y));
   }
  }
 }
 
 my_emit_clusters(g,f);
 
 fprintf(f,"stop\n");
}

void printptf(FILE* f, point pt)
{
	fprintf(f," %.3f %.3f",PS2INCH(pt.x),PS2INCH(pt.y));
}

int codegen_bezier_has_arrows(void)
{
	return CodeGen && 
  CodeGen->bezier_has_arrows
  /* (CodeGen->arrowhead == 0)) */;
}
