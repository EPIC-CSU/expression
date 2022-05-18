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
#ifndef WIN32
#include	<unistd.h>
#else
#include	<io.h>
#endif

void *zmalloc(size_t nbytes)
{
	char	*rv = (char	*)malloc(nbytes);
	if (rv == NULL) {fprintf(stderr,"out of memory\n"); abort();}
	memset(rv,0,nbytes);
	return rv;
}

void *zrealloc(void *ptr, size_t size, size_t elt, size_t osize)
{
	void	*p = realloc(ptr,size*elt);
	if (osize < size) memset((char*)p+(osize*elt),'\0',(size-osize)*elt);
	return p;
}

/*
 *  a queue of nodes
 */
queue *
new_queue(int sz)
{
	queue		*q = NEW(queue);

	if (sz <= 1) sz = 2;
	q->head = q->tail = q->store = N_NEW(sz,node_t*);
	q->limit = q->store + sz;
	return q;
}

void
free_queue(queue* q)
{
	free(q->store);
	free(q);
}

void
enqueue(queue* q, node_t* n)
{
	*(q->tail++) = n;
	if (q->tail >= q->limit) q->tail = q->store;
}

node_t *
dequeue(queue* q)
{
	node_t	*n;
	if (q->head == q->tail) n = NULL;
	else {
		n = *(q->head++);
		if (q->head >= q->limit) q->head = q->store;
	}
	return n;
}

/* returns index of an attribute if bound, else -1 */
int
late_attr(void* obj, char* name)
{
	attrsym_t	*a;
	if ((a = agfindattr(obj,name))) return a->index;
	else return -1;
}

int
late_int(void *obj, attrsym_t *attr, int def, int low)
{
	char	*p;
	int		rv;
	if (attr == NULL) return def;
	p = agxget(obj,attr->index);
	if (p[0]  == '\0') return def;
	if ((rv = atoi(p)) < low) rv = low;
	return rv;
}

double
late_float(void *obj,attrsym_t *attr, double def, double low)
{
	char		*p;
	float		rv;
	if (attr == NULL) return def;
	p = agxget(obj,attr->index);
	if (p[0]  == '\0') return def;
	if ((rv = atof(p)) < low) rv = low;
	return rv;
}

char *
late_string(void* obj, attrsym_t* attr, char* def)
{
	if (attr == NULL) return def;
	return agxget(obj,attr->index);
}

char *
late_nnstring(void* obj, attrsym_t* attr, char* def)
{
	char	*rv = late_string(obj,attr,def);
	if (rv[0] == '\0') rv = def;
	return rv;
}

/* counts occurences of 'c' in string 'p' */
int
strccnt(char *p, char c)
{
	int		rv = 0;
	while (*p) if (*p++ == c) rv++;
	return 	rv;
}

/* union-find */
node_t	*
UF_find(node_t* n)
{
	while (n->u.UF_parent && (n->u.UF_parent != n)) {
		if (n->u.UF_parent->u.UF_parent) n->u.UF_parent = n->u.UF_parent->u.UF_parent;
		n = n->u.UF_parent;
	}
	return n;
}

node_t	*
UF_union(node_t *u, node_t *v)
{
	if (u == v) return u;
	if (u->u.UF_parent == NULL) {u->u.UF_parent = u; u->u.UF_size = 1;}
	else u = UF_find(u);
	if (v->u.UF_parent == NULL) {v->u.UF_parent = v; v->u.UF_size = 1;}
	else v = UF_find(v);
	if (u->id > v->id) { u->u.UF_parent = v; v->u.UF_size += u->u.UF_size;}
	else {v->u.UF_parent = u; u->u.UF_size += v->u.UF_size; v = u;}
	return v;
}

void
UF_remove(node_t *u, node_t *v)
{
	assert(u->u.UF_size == 1);
	u->u.UF_parent = u;
	v->u.UF_size -= u->u.UF_size;
}

void
UF_singleton(node_t* u)
{
	u->u.UF_size = 1;
	u->u.UF_parent = NULL;
	u->u.ranktype = NORMAL;
}

void
UF_setname(node_t *u, node_t *v)
{
	assert(u == UF_find(u));
	u->u.UF_parent = v;
	v->u.UF_size += u->u.UF_size;
}

boolean spline_merge(node_t* n)
{
	return ((n->u.node_type == VIRTUAL) && ((n->u.in.size > 1) || (n->u.out.size > 1)));
}

point pointof(int x, int y)
{
	point rv;
	rv.x = x, rv.y = y;
	return rv;
}

point
cvt2pt(pointf p)
{
	point	rv;
	rv.x = POINTS(p.x);
	rv.y = POINTS(p.y);
	return rv;
}

pointf
cvt2ptf(point p)
{
	pointf	rv;
	rv.x = PS2INCH(p.x);
	rv.y = PS2INCH(p.y);
	return rv;
}

box boxof (int llx, int lly, int urx, int ury)
{
	box b;

	b.LL.x = llx, b.LL.y = lly;
	b.UR.x = urx, b.UR.y = ury;
	return b;
}

box mkbox(point p0, point p1)
{
	box		rv;

	if (p0.x < p1.x)	{	rv.LL.x = p0.x; rv.UR.x = p1.x; }
	else				{	rv.LL.x = p1.x; rv.UR.x = p0.x; }
	if (p0.y < p1.y)	{	rv.LL.y = p0.y; rv.UR.y = p1.y; }
	else				{	rv.LL.y = p1.y; rv.UR.y = p0.y; }
	return rv;
}

point add_points(point p0, point p1)
{
	p0.x += p1.x;
	p0.y += p1.y;
	return p0;
}

point sub_points(point p0, point p1)
{
	p0.x -= p1.x;
	p0.y -= p1.y;
	return p0;
}

double atan2pt(point p1, point p0)
{
	return atan2((double)(p1.y - p0.y),(double)(p1.x - p0.x));
}

/* from Glassner's Graphics Gems */
#define W_DEGREE 5

/*
 *  Bezier : 
 *	Evaluate a Bezier curve at a particular parameter value
 *      Fill in control points for resulting sub-curves if "Left" and
 *	"Right" are non-null.
 * 
 */
pointf Bezier (pointf *V, int degree, double t, pointf* Left, pointf* Right)
{
	int i, j;		/* Index variables	*/
	pointf Vtemp[W_DEGREE + 1][W_DEGREE + 1];

	/* Copy control points	*/
	for (j =0; j <= degree; j++) {
		Vtemp[0][j] = V[j];
	}

	/* Triangle computation	*/
	for (i = 1; i <= degree; i++) {	
		for (j =0 ; j <= degree - i; j++) {
	    	Vtemp[i][j].x =
	      		(1.0 - t) * Vtemp[i-1][j].x + t * Vtemp[i-1][j+1].x;
	    	Vtemp[i][j].y =
	      		(1.0 - t) * Vtemp[i-1][j].y + t * Vtemp[i-1][j+1].y;
		}
	}
	
	if (Left != NULL)
		for (j = 0; j <= degree; j++)
	    		Left[j] = Vtemp[j][0];
	if (Right != NULL)
		for (j = 0; j <= degree; j++)
	    		Right[j] = Vtemp[degree-j][j];

	return (Vtemp[degree][0]);
}

textlabel_t	*make_label(char *str, double fontsize, char *fontname, char *fontcolor, graph_t *g)
{
	textlabel_t	*rv = NEW(textlabel_t);
	rv->text = str;
	rv->fontname = fontname;
	rv->fontcolor = fontcolor;
	rv->fontsize = fontsize;
	label_size(str,rv,g);
	return rv;
}

#ifdef DEBUG
edge_t	* debug_getedge(graph_t *g, char *s0, char *s1)
{
	node_t	*n0,*n1;
	n0 = agfindnode(g,s0);
	n1 = agfindnode(g,s1);
	if (n0 && n1) return agfindedge(g,n0,n1);
	else return NULL;
}
#endif

#ifndef WIN32
#include	<pwd.h>
#endif

char * username(char* buf)
{
	char	*user = NULL;
#ifndef WIN32
	struct passwd	*p;
	p = (struct passwd *) getpwuid(getuid());
#ifdef SVR4
	if (p) {sprintf(buf,"(%s) %s",p->pw_name,p->pw_comment); user = buf;}
#else
	if (p) {sprintf(buf,"(%s) %s",p->pw_name,p->pw_gecos); user = buf;}
#endif
#endif
	if (user == NULL) user = "Bill Gates";
	return user;
}

void cat_libfile(FILE *ofp, char **arglib, char **stdlib)
{
	FILE	*fp;
	char	*p,**s,buf[BUFSIZ];
	int		i,use_stdlib = TRUE;

	if (arglib) for (i = 0; (p = arglib[i]); i++)
		if (*p == '\0') use_stdlib = FALSE;
	if (use_stdlib) for (s = stdlib; *s; s++) {fputs(*s,ofp); fputc('\n',ofp);}
#if !SERVER
	if (arglib) for (i = 0; (p = arglib[i]); i++) {
		if (p[0] && (fp = fopen(p,"r"))) {
			while (fgets(buf,sizeof(buf),fp)) fputs(buf,ofp);
		}
		else fprintf(stderr,"warning: can't open library file %s\n", p);
	}
#endif
}

int
rect_overlap(box b0, box b1)
{
	if ((b0.UR.x < b1.LL.x) || (b1.UR.x < b0.LL.x) 
		|| (b0.UR.y < b1.LL.y) || (b1.UR.y < b0.LL.y)) return FALSE;
	return TRUE;
}

int round(double f)
{
	int		rv;
	if (f > 0) rv = (int)(f + .5) ;
	else rv = (int)(f - .5);
	return rv;
}

int
maptoken(char *p,char **name, int *val)
{
	int		i;
	char	*q;

	for (i = 0; (q = name[i]); i++)
		if (p && streq(p,q)) break;
	return val[i];
}

int
mapbool(char* p)
{
	if (p == NULL) return FALSE;
	if (!stricmp(p,"false")) return FALSE;
	if (!stricmp(p,"true")) return TRUE;
	return atoi(p);
}

#if WIN32
strcasecmp(char	*s0,char	*s1)
{
	char		c0,c1;
	do {
		c0 = *s0++;
		c1 = *s1++;
		if (isupper(c0)) c0 = tolower(c0);
		if (isupper(c1)) c1 = tolower(c1);
		if (c0 != c1) break;
	} while (c0 && c1);
	return c0 - c1;
}
strncasecmp(char	*s0,char	*s1,int n)
{
	char		c0,c1;
	int			m = n;
	while (m--) {
		c0 = *s0++;
		c1 = *s1++;
		if (isupper(c0)) c0 = tolower(c0);
		if (isupper(c1)) c1 = tolower(c1);
		if (c0 != c1) break;
	}
	return c0 - c1;
}
#endif

#ifndef HAVE_PATHFIND
#if WIN32
#define R_OK 2
#define W_OK 4
#define X_OK 0
#endif
char *pathfind(char *patharg, char *basename, char *mode)
{
	char	*path;
	static char *fullname;
	char	*dir;
	char	*p, c;
	int		amode;

	amode = 0;
	for (p = mode; ((c = *p)); p++)
		switch(c) {
		case 'r':	amode |= R_OK; break;
		case 'w':	amode |= W_OK; break;
		case 'x':	amode |= X_OK; break;
		default:	break; /* i don't need to implement */
		}

	path=strdup(patharg);
	if (fullname) free(fullname);
	fullname=(char*)malloc(strlen(patharg)+strlen(basename)+2);
	for (dir = strtok(path,PATHSEPARATOR); dir; dir = strtok(0,PATHSEPARATOR)) {
		sprintf(fullname,"%s/%s",dir,basename);
		if (access(fullname,amode) == 0) break;
	}
	free(path);
	if (dir) return fullname;
	else return 0;
}
#endif
