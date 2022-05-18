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

#ifndef EXTERN
#define EXTERN extern
#endif

EXTERN char		**Files;			/* from command line */
EXTERN char		**Lib;				/* from command line */
EXTERN boolean	Verbose;			/* print informative messages */
EXTERN int		Output_lang;		/* POSTSCRIPT, DOT, etc. */
EXTERN codegen_t	*CodeGen;
EXTERN FILE		*Output_file;
EXTERN char 	*Fontlibpath;
extern char		*Version;			/* from dot.c */

EXTERN int		Show_boxes;			/* emit code for correct box coordinates */
EXTERN int		CL_type;			/* NONE, LOCAL, GLOBAL */
EXTERN boolean	Concentrate;		/* if parallel edges should be merged */

extern codegen_t GIF_CodeGen, GD_CodeGen, HPGL_CodeGen, ISMAP_CodeGen,
	MIF_CodeGen, PIC_CodeGen, PS_CodeGen; 

EXTERN attrsym_t	
					*N_height, *N_width, *N_shape, *N_color,
					*N_fontsize, *N_fontname, *N_fontcolor,
					*N_label, *N_style, *N_showboxes,
					*N_sides,*N_peripheries,*N_orientation,
					*N_skew,*N_distortion,*N_fixed,*N_layer,
					*N_group,*N_comment;

EXTERN	attrsym_t	*E_weight, *E_minlen, *E_color,
					*E_fontsize, *E_fontname, *E_fontcolor,
					*E_label, *E_dir, *E_style, *E_decorate,
					*E_showboxes,*E_arrowsz,*E_constr,*E_layer,
					*E_comment;

/* vladimir */
EXTERN	attrsym_t	*E_samehead, *E_sametail,
					*E_arrowhead, *E_arrowtail,
					*E_headlabel, *E_taillabel,
					*E_labelfontsize, *E_labelfontname, *E_labelfontcolor,
					*E_labeldistance, *E_labelangle;
