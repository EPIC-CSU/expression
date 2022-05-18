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
/* file: symtabs.h							*/
/* created: Nov. 30, 1998					*/
/* authors: pgrun							*/
/* last modified:							*/
/* contents: symtab defintions				*/
/********************************************/
#ifndef _SYMTABS_H_
#define _SYMTABS_H_

#ifdef __UNIX__
#define _ASSERT assert
#endif


#include "defs_node.h"
#include "symbols.h"

class Symtab {
  SymbolList _symlist;		//  typedefed as  list<Symbol *> 
 public:
  Symtab(void) : _symlist() {}
  ~Symtab();
  //  Symtab(char *newSymName);
  SymbolPtr Declare(char *name,defs_node *dn); 
  SymbolPtr FindSymbol(char * name); //returns NULL if it doesn't find it
  SymbolPtr GetSymbol(char * name); //Gives error if it doesn't find it
  SymbolPtr addSymbol(char *name);
  SymbolPtr addSymbol(char *name, bool isPtr);
  
  int Traverse(int(* func)(SymbolPtr));
  void Print();
  void printVariables();
};


extern Symtab st;

#endif

