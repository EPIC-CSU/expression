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
 * Copyright (c) AT&T Corp. 1994-1997.
 * This code is licensed by AT&T Corp.  For the
 * terms and conditions of the license, see
 * http://www.research.att.com/orgs/ssr/book/reuse
 */

/*
 * Written by Stephen North and Eleftherios Koutsofios.
 */

#include "stdafx.h"
#include	"dot.h"
char		*Version = "dot version uwin98 (01-26-98)";
char		*Copyright = "Copyright (c) AT&T Corp. 1994-1998\n";

//NOS 
//int main(int argc, char** argv)
__declspec( dllexport ) int do_dot(FILE* iFile,FILE* oFile)
{
	graph_t	*g, *prev;

	prev = NIL(graph_t*);
	//dot_initialize(argc,argv);
 dot_initialize(oFile);
	//while ((g = next_input_graph(iFile))) { //use one file only
	g = next_input_graph(iFile);
	if (prev) dot_close(prev);
	dot_init(g);
	dot_rank(g);
	dot_mincross(g);
	dot_position(g);
 dot_sameports(g);
	dot_splines(g);
	dot_postprocess(g);
	dot_write(g);
	dot_terminate();
	return 1;
}
