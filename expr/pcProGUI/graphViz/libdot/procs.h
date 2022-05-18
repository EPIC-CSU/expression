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

#if _PACKAGE_AST
_BEGIN_EXTERNS_ /* public data */
#if _BLD_dot && defined(__EXPORT__)
#define extern	__EXPORT__
#endif
#if !_BLD_dot && defined(__IMPORT__)
#define extern	__IMPORT__
#endif
#endif

extern void		abomination(Agraph_t *);
extern void		acyclic(Agraph_t *);
extern point		add_points(point, point);
extern void		add_to_component(Agnode_t *);
extern void		add_tree_edge(Agedge_t *);
extern void		allocate_aux_edges(Agraph_t *);
extern void		allocate_ranks(Agraph_t *);
extern void		arrow_flags (edge_t *e, int *sflag, int *eflag);
extern double	arrow_length (edge_t *e, int flag);
extern double		atan2pt(point, point);
extern void		attach_attrs(Agraph_t *);
extern void		basic_merge(Agedge_t *, Agedge_t *);
extern void		begin_component(void);
extern pointf		Bezier(pointf *, int, double, pointf *, pointf *);
extern shape_desc *bind_shape(char* name);
extern box			boxof(int, int, int, int);
extern void		build_ranks(Agraph_t *, int);
extern void		build_skeleton(Agraph_t *, Agraph_t *);
extern char		*canoncolor(char *, char *);
extern void		cat_libfile(FILE *, char **, char **);
extern unsigned char bothdowncandidates(Agnode_t *, Agnode_t *);
extern unsigned char bothupcandidates(Agnode_t *, Agnode_t *);
extern unsigned char downcandidate(Agnode_t *);
extern unsigned char spline_merge(Agnode_t *);
extern unsigned char upcandidate(Agnode_t *);
extern void		class1(Agraph_t *);
extern void		class2(Agraph_t *);
extern void		cleanup1(Agraph_t *);
extern void		cleanup2(Agraph_t *, int);
extern void		cleanup_edge(Agedge_t *);
extern void		cleanup_graph(Agraph_t *);
extern void		cleanup_node(Agnode_t *);
extern Agnode_t	*clone_vn(Agraph_t *, Agnode_t *);
extern point		closest(splines* spl, point p);
extern void		clust_count_ranks(Agraph_t *, int *);
extern void		cluster_leader(Agraph_t *);
extern int			clust_in_layer(Agraph_t *);
extern int			codegen_bezier_has_arrows(void);
extern void		collapse_cluster(Agraph_t *, Agraph_t *);
extern void		collapse_leaves(Agraph_t *);
extern void		collapse_rankset(Agraph_t *, Agraph_t *, int);
extern void		collapse_sets(Agraph_t *);
extern int			colorcmpf(void*, void*);
extern char		*colorxlate(char *, char *);
extern void		compress_graph(Agraph_t *);
extern void		compute_bb(Agraph_t *, Agraph_t *);
extern void		contain_clustnodes(Agraph_t *);
extern void		contain_nodes(Agraph_t *);
extern void		contain_subclust(Agraph_t *);
extern void		count_ranks(Agraph_t *, int **);
extern void		create_aux_edges(Agraph_t *);
extern int			csfnode(node_t* n);
extern pointf		cvt2ptf(point);
extern point		cvt2pt(pointf);
extern void		decompose(Agraph_t *, int);
extern void		delete_fast_edge(Agedge_t *);
extern void		delete_fast_node(Agraph_t *, Agnode_t *);
extern void		delete_flat_edge(Agedge_t *);
extern void		delete_other_edge(Agedge_t *);
extern Agnode_t	*dequeue(queue *);
extern void		dfs(Agnode_t *);
extern void		dfs_cutval(Agnode_t *, Agedge_t *);
extern void		dfs_enter_inedge(Agnode_t *);
extern void		dfs_enter_outedge(Agnode_t *);
extern int			dfs_range(Agnode_t *, Agedge_t *, int);
extern void		do_graph_label(Agraph_t *);
extern void		do_leaves(Agraph_t *, Agnode_t *);
extern void		do_ordering(Agraph_t *);
extern void		dot_cleanup(Agraph_t *);
extern void		dot_close(Agraph_t *);
extern void		dot_concentrate(Agraph_t *);
extern void		dot_init(Agraph_t *);
//NOS
//extern void		dot_initialize(int, char **);
extern void		dot_initialize(FILE*);
extern void		dot_mincross(Agraph_t *);
extern void		dot_position(Agraph_t *);
extern void		dot_postprocess(Agraph_t *);
extern void		dot_rank(Agraph_t *);
extern void		dot_sameports(Agraph_t *);
extern void		dot_splines(Agraph_t *);
extern void		dot_terminate(void);
extern void		dot_write(Agraph_t *);
extern int			edge_in_CB(Agedge_t *);
extern int			edge_in_layer(Agraph_t *, Agedge_t *);
extern void		edgelabel_ranks(Agraph_t *);
extern double		elapsed_sec(void);
extern void		emit_attachment(textlabel_t *, splines *);
extern void		emit_background(Agraph_t *, point, point);
extern void		emit_clusters(Agraph_t *);
extern void		emit_defaults(Agraph_t *);
extern void		emit_edge(Agedge_t *);
extern void		emit_eof(void);
extern void		emit_graph(Agraph_t *);
extern void		emit_header(Agraph_t *);
extern void		emit_label(textlabel_t *, graph_t *);
extern void		emit_layer(int n);
extern void		emit_node(Agnode_t *);
extern void		emit_reset(Agraph_t *);
extern void		emit_trailer(void);
extern void		end_component(void);
extern void		enqueue_neighbors(queue *, Agnode_t *, int);
extern void		enqueue(queue *, Agnode_t *);
extern Agedge_t	*enter_edge(Agedge_t *);
extern void		epsf_define(void);
extern void		epsf_gencode(Agnode_t *);
extern void		epsf_init(Agnode_t *);
extern void		exchange(Agnode_t *, Agnode_t *);
extern void		exchange_tree_edges(Agedge_t *, Agedge_t *);
extern void		expand_cluster(Agraph_t *);
extern void		expand_leaves(Agraph_t *);
extern void		expand_ranksets(Agraph_t *);
extern Agedge_t	*fast_edge(Agedge_t *);
extern void		fast_node(Agraph_t *, Agnode_t *);
extern void		fast_nodeapp(Agnode_t *, Agnode_t *);
extern void		feasible_tree(void);
extern FILE		*file_select(char *);
extern void		find_clusters(Agraph_t *);
extern Agedge_t	*find_fast_edge(Agnode_t *, Agnode_t *);
extern Agnode_t	*find_fast_node(Agraph_t *, Agnode_t *);
extern Agedge_t	*find_flat_edge(Agnode_t *, Agnode_t *);
extern shape_desc	*find_user_shape(char* name);
extern void		findlr(Agnode_t *, Agnode_t *, int *, int *);
extern void		flat_breakcycles(Agraph_t *);
extern void		flat_edge(Agraph_t *, Agedge_t *);
extern void		flat_edges(Agraph_t *);
extern int			flat_limits(Agraph_t *, Agedge_t *);
extern int			flat_mval(Agnode_t *);
extern void		flat_node(Agedge_t *);
extern void		flat_reorder(Agraph_t *);
extern void		flat_search(Agraph_t *, Agnode_t *);
extern void		free_matrix(adjmatrix_t *);
extern void		free_queue(queue *);
extern void		free_splines(Agedge_t *);
//NOS
//extern point	gif_textsize(char *str, char *fontname, double fontsz);
extern void		global_def(char *, Agsym_t *(*fun)(Agraph_t *, char *, char *));
extern void		hsv2rgb(double *, double *, double *, double, double, double);
extern int			idealsize(Agraph_t *, double);
extern Agnode_t	*incident(Agedge_t *);
extern int			in_cross(Agnode_t *, Agnode_t *);
extern void		incr_width(Agraph_t *, Agnode_t *);
extern void		infuse(Agraph_t *, Agnode_t *);
extern void		init_cutvalues(void);
extern void		init_edge(Agedge_t *);
extern void		init_mccomp(Agraph_t *, int);
extern void		init_mincross(Agraph_t *);
extern void		init_node(Agnode_t *);
extern void		init_rank(void);
extern void		init_ugraph(Agraph_t *);
extern int			inside_cluster(Agraph_t *, Agnode_t *);
extern void		install_cluster(Agraph_t *, Agnode_t *, int, queue *);
extern void		install_in_rank(Agraph_t *, Agnode_t *);
extern void		interclexp(Agraph_t *);
extern void		interclrep(Agraph_t *, Agedge_t *);
extern void		interclust1(Agraph_t *, Agnode_t *, Agnode_t *, Agedge_t *);
extern int			is_a_normal_node_of(Agraph_t *, Agnode_t *);
extern int			is_a_vnode_of_an_edge_of(Agraph_t *, Agnode_t *);
extern int			is_cluster(Agraph_t *);
extern int			is_cluster_edge(Agedge_t *);
extern int			is_fast_node(Agraph_t *, Agnode_t *);
extern int			is_natural_number(char *);
extern void		keepout_othernodes(Agraph_t *);
extern pointf		label_size(char *, textlabel_t *lp, graph_t *);
extern Agnode_t	*label_vnode(Agraph_t *, Agedge_t *);
extern int			lang_select(char *);
extern int			late_attr(void*, char *);
extern double		late_float(void*, Agsym_t *, double, double);
extern int			late_int(void *, Agsym_t *, int, int);
extern char		*late_nnstring(void*, Agsym_t *, char *);
extern char		*late_string(void*, Agsym_t *, char *);
extern int			layerindex(char *);
extern int			layer_index(char *, int);
extern Agnode_t	*leader_of(Agraph_t *, Agnode_t *);
extern Agedge_t	*leave_edge(void);
extern int			left2right(Agraph_t *, Agnode_t *, Agnode_t *);
extern int			local_cross(elist, int);
extern void		LR_balance(void);
extern Agedge_t	*make_aux_edge(Agnode_t *, Agnode_t *, int, int);
extern void		make_chain(Agraph_t *, Agnode_t *, Agnode_t *, Agedge_t *);
extern void		make_edge_pairs(Agraph_t *);
extern void		make_interclust_chain(Agraph_t *, Agnode_t *, Agnode_t *, Agedge_t *);
extern textlabel_t	*make_label(char *, double, char *, char *, graph_t *);
extern void		make_leafslots(Agraph_t *);
extern void		make_LR_constraints(Agraph_t *);
extern void		make_lrvn(Agraph_t *);
extern int			make_new_cluster(Agraph_t *, Agraph_t *);
extern void		make_slots(Agraph_t *, int, int, int);
extern Agnode_t	*make_vn_slot(Agraph_t *, int, int);
extern int			mapbool(char *);
extern void		map_edge(Agedge_t *);
extern Agnode_t	*map_interclust_node(Agnode_t *);
extern void		map_path(Agnode_t *, Agnode_t *, Agedge_t *, Agedge_t *, int);
extern point		map_point(point);
extern int			maptoken(char *, char **, int *);
extern void		mark_clusters(Agraph_t *);
extern void		mark_lowclusters(Agraph_t *);
extern void		Mcircle_hack(node_t* n);
extern void		merge2(Agraph_t *);
extern void		merge_chain(Agraph_t *, Agedge_t *, Agedge_t *, int);
extern void		merge_components(Agraph_t *);
extern Agnode_t	*merge_leaves(Agraph_t *, Agnode_t *, Agnode_t *);
extern void		merge_oneway(Agedge_t *, Agedge_t *);
extern void		merge_ranks(Agraph_t *);
extern void		mergevirtual(Agraph_t *, int, int, int, int);
extern int			mincross(Agraph_t *, int, int);
extern int			mincross_clust(Agraph_t *, Agraph_t *);
extern void		mincross_options(Agraph_t *);
extern void		mincross_step(Agraph_t *, int);
extern void		minmax_edges(Agraph_t *);
extern box			mkbox(point, point);
extern void		Mlabel_hack(node_t* n);
extern void		mysincos(double, double *, double *);
extern int			ncross(Agraph_t *);
extern queue		*new_queue(int);
extern Agedge_t	*new_virtual_edge(Agnode_t *, Agnode_t *, Agedge_t *);
extern FILE		*next_input_file(FILE*);
extern Agraph_t	*next_input_graph(FILE*);
extern int			node_in_CB(Agnode_t *);
extern void		node_induce(Agraph_t *, Agraph_t *);
extern int			node_in_layer(Agraph_t *, Agnode_t *);
extern void		nodesize(Agnode_t *, unsigned char);
extern int			nonconstraint_edge(Agedge_t *);
extern int			nsiter2(Agraph_t *);
extern int			ordercmpf(int *, int *);
extern void		ordered_edges(Agraph_t *);
extern void		osize_label(textlabel_t *, int *, int *, int *, int *);
extern void		other_edge(Agedge_t *);
extern int			out_cross(Agnode_t *, Agnode_t *);
extern point		pagecode(char);
extern point		pageincr(point);
extern int			parse_layers(char *);
extern char		**parse_style(char *);
extern void		place_graph_label(Agraph_t *);
extern point		place_leaf(Agnode_t *, point, int);
extern Agnode_t	*plain_vnode(Agraph_t *, Agedge_t *);
extern point		pointof(int, int);
extern int			ports_eq(Agedge_t *, Agedge_t *);
extern void		pos_clusters(Agraph_t *);
extern void		potential_leaf(Agraph_t *, Agedge_t *, Agnode_t *);
extern void		printptf(FILE *, point);
extern void		rank1(Agraph_t *);
extern void		rank(Agraph_t *, int, int);
extern int			rank_set_class(Agraph_t *);
extern int			rcross(Agraph_t *, int);
extern void		rebuild_vlists(Agraph_t *);
extern void		rec_attach_bb(Agraph_t *);
extern void		rec_bb(Agraph_t *, Agraph_t *);
extern void		rec_reset_vlists(Agraph_t *);
extern void		rec_save_vlists(Agraph_t *);
extern int			rect_overlap(box, box);
extern void		remove_aux_edges(Agraph_t *);
extern void		remove_rankleaders(Agraph_t *);
extern void		renewlist(elist *);
extern void		reorder(Agraph_t *, int, int, int);
extern void		rerank(Agnode_t *, int);
extern point		resize_leaf(Agnode_t *, point);
extern void		restore_best(Agraph_t *);
extern void		reverse_edge(Agedge_t *);
extern int			round(double);
extern void		round_corners(point *, int, int);
extern void		routesplinesinit(void);
extern point		*routesplines(path *, int *);
extern void		routesplinesterm(void);
extern void		safe_delete_fast_edge(Agedge_t *);
extern void		safe_list_append(Agedge_t *, elist *);
extern void		safe_other_edge(Agedge_t *);
extern int			same_side(pointf, pointf, pointf, pointf);
extern void		save_best(Agraph_t *);
extern void		save_vlist(Agraph_t *);
extern void		scan_label_adjustments(Agraph_t *);
extern void		scan_ranks(Agraph_t *);
extern void		scan_result(void);
extern void		search_component(Agraph_t *, Agnode_t *);
extern int			selectedlayer(char *);
extern void		separate_subclust(Agraph_t *);
extern void		set_aspect(Agraph_t *);
extern void		setbounds(Agnode_t *, int *, int, int);
extern void		set_minmax(Agraph_t *);
extern void		setup_graph(Agraph_t *);
extern void		setup_page(Agraph_t *, point);
extern void		set_xcoords(Agraph_t *);
extern void		set_ycoords(Agraph_t *);
extern void		shape_clip(Agnode_t *, point *, Agedge_t *);
extern void		start_timer(void);
extern int			strccnt(char *, char);
extern point		sub_points(point, point);
extern void		TB_balance(void);
extern int			tight_tree(void);
extern void		translate_bb(Agraph_t *, int);
extern void		translate_drawing(Agraph_t *);
extern void		transpose(Agraph_t *, int);
extern int			transpose_step(Agraph_t *, int, int);
extern int			treesearch(Agnode_t *);
extern Agnode_t	*treeupdate(Agnode_t *, Agnode_t *, int, int);
extern Agnode_t	*UF_find(Agnode_t *);
extern void		UF_remove(Agnode_t *, Agnode_t *);
extern void		UF_setname(Agnode_t *, Agnode_t *);
extern void		UF_singleton(Agnode_t *);
extern Agnode_t	*UF_union(Agnode_t *, Agnode_t *);
extern void		unmerge_oneway(Agedge_t *);
extern void		update(Agedge_t *, Agedge_t *);
extern void		update_bb(Agraph_t *, point);
extern void		use_library(char *);
extern char		*username(char *);
extern int			validpage(point);
extern Agedge_t	*virtual_edge(Agnode_t *, Agnode_t *, Agedge_t *);
extern Agnode_t	*virtual_node(Agraph_t *);
extern void		virtual_weight(Agedge_t *);
extern int			vnode_not_related_to(Agraph_t *, Agnode_t *);
extern void		write_plain(Agraph_t *, FILE *);
extern void		x_cutval(Agedge_t *);
extern int			x_val(Agedge_t *, Agnode_t *, int);
extern void		zapinlist(elist *, Agedge_t *);
extern void		*zmalloc(size_t);
extern void		*zrealloc(void *, size_t , size_t , size_t);

#undef extern
#ifdef _PACKAGE_AST
_END_EXTERNS_
#endif
