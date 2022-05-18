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
 * This code is licensed by AT&T Corp.	For the
 * terms and conditions of the license, see
 * http://www.research.att.com/orgs/ssr/book/reuse
 */

typedef unsigned char boolean;

typedef int (*qsort_cmpf)(const void*, const void*);
typedef int (*bsearch_cmpf) (const void *, const void *);

typedef struct Agraph_t graph_t;
typedef struct Agnode_t node_t;
typedef struct Agedge_t edge_t;
typedef struct Agsym_t attrsym_t;


typedef struct pointf {
	double		x,y;
} pointf;

typedef struct point {
	int			x,y;
} point;

typedef struct box {
	point		LL,UR;
} box;

typedef struct port_t {			/* internal edge endpoint specification */
	point		p;					/* aiming point */
	float		theta;				/* slope in radians */
	boolean	constrained,defined;
	unsigned char order;			/* for mincross */
} port_t;

typedef struct path {			/* internal specification for an edge spline */
	port_t		start,end;
	point		*ulpp, *urpp, *llpp, *lrpp;	/* tangents of near splines */
	int			nbox;				/* number of subdivisions */
	box			*boxes;				/* rectangular regions of subdivision */
	void		*data;
} path;

typedef struct bezier {
	point		*list;
	int			size;
	int			sflag, eflag;
	point		sp, ep;
} bezier;

typedef struct splines {
	bezier		*list;
	int			size;
} splines;

typedef struct polygon_t {		/* mutable shape information for a node */
	int			regular;			/* true for symmetric shapes */
	int			peripheries;		/* number of periphery lines */
	int			sides;				/* number of sides */
	float		orientation;		/* orientation of shape (+ve degrees) */
	float		distortion;			/* distortion factor - as in trapezium */
	float		skew;				/* skew factor - as in parallelogram */
	int			option;				/* ROUNDED, DIAGONAL corners, etc. */
	pointf		*vertices;			/* array of vertex points */
} polygon_t;

typedef struct shape_desc {		/* read-only shape descriptor */
	char		*name;							 /* as read from graph file */
	void		(*initfn)(node_t *);			/* initializes shape from node label */
	port_t		(*portfn)(node_t *, char *);	/* finds aiming point and slope of port */
	int			(*insidefn)(node_t *, pointf, edge_t*);		/* clips incident edge spline */
	int			(*pboxfn)(node_t *, edge_t *, int, box *, int *);		/* finds box path to reach port */
	void		(*codefn)(node_t *);			/* emits graphics code for node */
	polygon_t	*polygon;						/* base polygon info */
} shape_desc;

typedef struct codegen_t {
	void	 (*reset)(void);
	void	(*begin_job)(FILE *ofp,graph_t *g, char **lib, char *user, char *vers, point pages);
	void	(*end_job)(void);
	void	(*begin_graph)(graph_t* g, box bb, point pb);
	void	(*end_graph)(void);
	void	(*begin_page)(point page, double scale, int rot, point offset);
	void	(*end_page)(void);
	void	(*begin_cluster)(graph_t *),(*end_cluster)(void);
	void	(*begin_node)(node_t *),(*end_node)(void);
	void	(*begin_edge)(edge_t *),(*end_edge)(void);
	void	(*begin_context)(void),(*end_context)(void);
	void	(*set_font)(char *fontname, double fontsize);
	void	(*textline)(point p, char *str, int width, double fontsz, double align);
	void	(*set_color)(char *name),(*set_style)(char **s);
	void	(*ellipse)(point p, int rx, int ry, int filled);
	void	(*polygon)(point *A, int n, int filled);
	void	(*beziercurve)(point *A, int n, int arrow_at_start, int arrow_at_end);
	void	(*polyline)(point *A,int n);
  /* void	(*arrowhead)(point p, double theta, double scale, int flag); */
	boolean bezier_has_arrows;
	void	(*user_shape)(char *name, point *A, int sides, int filled);
	void	(*comment)(void* obj, attrsym_t* sym);
	point	(*textsize)(char *str, char *fontname, double fontsz);
} codegen_t;

typedef struct queue {
	node_t	**store,**limit,**head,**tail;
} queue;

typedef struct adjmatrix_t {
	int		nrows,ncols;
	char	*data;
} adjmatrix_t;

typedef struct rank_t {
	int				n;			/* number of nodes in this rank			*/
	node_t			**v;		/* ordered list of nodes in rank		*/
	int				an;			/* globally allocated number of nodes	*/
	node_t			**av;		/* allocated list of nodes in rank		*/
	int			ht2;		/* height above the centerline			*/
	boolean			candidate;	/* for transpose ()						*/
	boolean			valid;
	int				cache_nc;	/* caches number of crossings			*/
	adjmatrix_t		*flat;
} rank_t;

typedef struct textline_t {
	char			*str;
	short			width;
	char			just;
} textline_t;

typedef struct textlabel_t {
	char			*text,*fontname,*fontcolor;
	float			fontsize;
	pointf			dimen;
	point			p;
	textline_t		*line;
	char			nlines;
} textlabel_t;

typedef struct layout_t {
 	float			quantum,scale,font_scale_adj;
	point			margin, page, size;
	boolean			landscape,centered;
} layout_t;

/* for "record" shapes */
typedef struct field_t {
	point		size;			/* its dimension */
	box			b;				/* its final placement */
	int			n_flds;	
	textlabel_t	*lp;			/* n_flds == 0 */
	struct field_t	**fld;		/* n_flds > 0 */
	int		LR;					/* if box list is horizontal (left to right) */
	char	*id;				/* user's identifier */
} field_t;

typedef struct hsbcolor_t {
	char			*name;
	unsigned char	h,s,b;
} hsbcolor_t;

typedef struct nlist_t {
	node_t			**list;
	int				size;
} nlist_t;

typedef struct elist {
	edge_t			**list;
	int				size;
} elist;

#define elist_fastapp(item,L)	do {L.list[L.size++] = item; L.list[L.size] = NULL;} while(0)
#define elist_append(item,L)	do {L.list = ALLOC(L.size + 2,L.list,edge_t*); L.list[L.size++] = item; L.list[L.size] = NULL;} while(0)
#define alloc_elist(n,L)		do {L.size = 0; L.list = N_NEW(n + 1,edge_t*); } while (0)
#define free_list(L)			do {if (L.list) free(L.list);} while (0)

/* note that ...
 * this decision is pretty questionable, but here goes.
 */

typedef struct Agraphinfo_t {
		/* to generate code */
	layout_t			*drawing;
	textlabel_t			*label;		/* if graph or cluster has a title */
	box					bb;				/* bounding box */

	int					n_cluster;
	graph_t				**clust;
	boolean				left_to_right, has_edge_labels;

	rank_t				*rank;
	node_t				*nlist;					/* fast graph node list */
	nlist_t				comp;					/* connected components */
	node_t				*minset,*maxset;		/* set leaders */
	long				n_nodes;				/* includes virtual */
	short				minrank,maxrank;

	/* various flags */
	boolean				has_flat_edges;
	boolean				showboxes;

	int					nodesep,ranksep;
	node_t				*ln,*rn;	/* left, right nodes of bounding box */


	/* for clusters */
	node_t				*leader,**rankleader;
	boolean				expanded;
	char				installed;
	char				set_type;
	boolean				exact_ranksep;
} Agraphinfo_t;

typedef struct Agnodeinfo_t {
	shape_desc			*shape;
	void				*shape_info;
	point				coord;
	float				width,height;
	int					ht,lw,rw;
	textlabel_t			*label;
	char				state;

	boolean				showboxes,has_port;

		/* fast graph */
	char				node_type,mark,onstack;
	char				ranktype,weight_class;
	node_t				*next,*prev;
	elist				in,out,flat_out,flat_in,other;
	graph_t				*clust;

		/* for union-find and collapsing nodes */
	int					UF_size;
	node_t				*UF_parent;
	node_t				*inleaf,*outleaf;

		/* for placing nodes */
	int					rank,order;	/* initially, order = 1 for ordered edges */
	int					mval;
	elist				save_in,save_out;

		/* for network-simplex */
	elist				tree_in,tree_out;
	edge_t				*par;
	int					low,lim;
	int					priority;

	double				pad[1];
} Agnodeinfo_t;

typedef struct Agedgeinfo_t {
	splines				*spl;
	port_t				tail_port,head_port;
	textlabel_t			*label,*head_label,*tail_label;

	char				edge_type;
	boolean				showboxes;
	short				xpenalty;
	int					weight;
	int					cutvalue,tree_index;
	short				count,minlen;

	edge_t				*to_virt,*to_orig;
} Agedgeinfo_t;
