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
/********************************************/
/* file: node.h								*/
/* created: Dec. 3, 1998					*/
/* authors: Gaby Grun						*/
/* last modified:							*/
/* contents: IR definitions for EXPRESSION	*/
/********************************************/

#ifndef _NODE_H_
#define _NODE_H_

#pragma warning(disable:4786)
#pragma warning(disable:4503)

#include <list>

#include "symbols.h"


//The Symbol table node
class Node{
	char * _name;   //a char string containing the name of this guy 
					//(if _symbol!=NULL, _symbol will contain the same name). 
public:
	Node(NodeKindEnum k=nke_invalid);
	~Node(void);
	NodeKindEnum Kind(void);
	void Kind(NodeKindEnum k);
	NodeList *Children(void);
	NodePtr Child(NodeKindEnum kind);
	void AppendChildren(NodeListPtr l);
	void AddChild(Node *c);
	SymbolPtr Symbol(void);
	void Symbol(SymbolPtr s);
	void Parent(NodePtr parent);
	NodePtr Parent(void);
	char *Name(void);
	void Name(char *n);
	int Traverse(int(*func)(NodePtr));
	int Traverse(int(*func)(NodePtr,NodePtr));
	NodePtr FindSubNode(NodeKindEnum kind);
	NodePtr FindSubNode(char *name);
	void Print(int indent=0);
};

extern NodePtr ir; //the only Node representing the whole ir. 

extern char *node_kind_names[];

#endif
