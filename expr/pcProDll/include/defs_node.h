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

#ifndef _DEFS_NODE_H_
#define _DEFS_NODE_H_

#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include <vector>

#ifdef vector
#undef vector
#endif 
#define vector std::vector



class defs_node;

typedef vector<defs_node *> defs_node_list;

class defs_node{
	defs_node_list children;
	char *name;
public:
	defs_node(void);
	defs_node(char *n);
	void Name(char *n){name=strdup(n);}
	char *Name(void){ return name;}
	defs_node_list *Children(void);
	void AddChild(defs_node *node);
	void AddChildren(defs_node_list *node_list);
	void Print(void);
	int Traverse(int(*func)(defs_node *));
	defs_node * FindSubNode(char *name);
	defs_node *Child(char *name);
};



#endif

