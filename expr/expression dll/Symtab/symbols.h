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
/* file: symbols.h							*/
/* created: Nov. 30, 1998					*/
/* authors: pgrun							*/
/* last modified:							*/
/* contents: symbols definitions for symtab	*/
/********************************************/

#ifndef _SYMBOLS_H_
#define _SYMBOLS_H_

#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include <string.h>
#include <list>

typedef class Symbol * SymbolPtr;
#ifdef  __UNIX__
	typedef list<SymbolPtr> SymbolList;
#else
	typedef std::list<SymbolPtr> SymbolList;
#endif
typedef SymbolList * SymbolListPtr;


//Node declarations needed here, because node.h includes symbols.h
class Node;
typedef class Node * NodePtr;
#ifdef  __UNIX__
	typedef list<NodePtr> NodeList;
#else
	typedef std::list<NodePtr> NodeList;
#endif
typedef NodeList *NodeListPtr;




class Symbol{
  NodeList _deflist;//the nodes in the EXPRESSION IR defining this symbol
  NodeList _uselist;//the nodes in the EXPRESSION IR using this symbol
  char *_name;//the symbol's name
public:
  Symbol();
  Symbol(char * name,Node *node);
  Symbol(const Symbol & sym);
  SymbolPtr Copy(void);
  char *Name(void);
  void Name(char *);
  void AddDef(Node *node);
  void RemoveDef(Node *node);
  void AddUse(Node *node);
  int NoOfDefs(NodePtr node);
 Node *GetFirstDef(void);
  NodeList *GetDefList(void);
  NodeList *GetUseList(void);
  int any_def_is(int kind);

};

#endif
