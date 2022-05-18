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
#include "stdafx.h"


#include "defs_node.h"
#include "symbols.h"
#include "symtabs.h"

#include <string.h>

Symtab::~Symtab() {
  SymbolList::iterator i;
  for(i=_symlist.begin();i!=_symlist.end();i++)
    {
      delete *i;
    }
}

SymbolPtr Symtab::Declare(char *name,defs_node * dn){ // high-level declaration with symbol construction
  SymbolPtr p=FindSymbol(name);
  if(p==NULL){
    SymbolPtr sym=new Symbol(name);
    _symlist.push_back(sym);
    sym->SetDefs(dn);
    return sym;
  }
  else{
    p->SetDefs(dn);
    return p;
  }
}

SymbolPtr Symtab::FindSymbol(char * name){
  SymbolList::iterator i;
  for(i=_symlist.begin();i!=_symlist.end();i++){
    if(strcmp((*i)->Name(),name)==0) 
      return *i;
  }
  return NULL;
}

SymbolPtr Symtab::GetSymbol(char * name){
  SymbolList::iterator i;
  for(i=_symlist.begin();i!=_symlist.end();i++){
    if(strcmp((*i)->Name(),name)==0) 
      return *i;
  }
  char buf[1000];
  sprintf(buf,"%s symbol not declared",name);
  //semantic_error(buf);
  printf("Syntax error: %s\n",buf);
  _ASSERT(0);
  return NULL;
}


SymbolPtr Symtab::addSymbol(char *name){ 
  SymbolPtr sym = FindSymbol(name); // look if symbol exists
  if(sym == NULL){			// if not, create a new one
    sym = new Symbol(name);
    _symlist.push_back(sym);
  }
  return sym;
}

SymbolPtr Symtab::addSymbol(char *name, bool isPtr){ 
  SymbolPtr sym = FindSymbol(name); // look if symbol exists
  if(sym == NULL){			// if not, create a new one
    sym = new Symbol(name, isPtr);
    _symlist.push_back(sym);
  }
  else
    if (isPtr == true)
      sym->putIsPtr(isPtr);

  return sym;
}

void Symtab :: Print(){
  SymbolList::iterator i;
  for(i=_symlist.begin();i!=_symlist.end();i++){
    printf("\n%s ",(*i)->Name());
    defs_node *defsNode = (*i)->GetDefs();
    if (defsNode != NULL)
      defsNode->Print();
  }
}

int Symtab :: Traverse(int(*func)(SymbolPtr))
{
  SymbolList::iterator i;
  for(i=_symlist.begin();i!=_symlist.end();i++){
    if(func(*i)) return 1;
  }
  return 0;
}

void Symtab :: printVariables(){
  SymbolList::iterator i;
  for(i=_symlist.begin();i!=_symlist.end();i++){
    cout << "int ";
    if ((*i)->getIsPtr())
      cout << "*";		// if it is a ptr, print int *
    cout << (*i)->Name();
    cout << ";" << endl;
  }
}
