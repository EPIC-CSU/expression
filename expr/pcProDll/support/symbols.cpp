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

Symbol::Symbol()
{
  _name = NULL;
  _defsnode = NULL;
  _isPtr = false;
}

Symbol::Symbol(char * name){
  _name=strdup(name);
  _defsnode = NULL;
  _isPtr = false;
}

Symbol::Symbol(char * name, bool isPtr){
  _name=strdup(name);
  _defsnode = NULL;
  _isPtr = isPtr;
}

Symbol::Symbol(const Symbol & sym)
{
  _name = strdup(sym._name);
  _isPtr = sym._isPtr;
}

char *Symbol::Name(void){
  return _name;
}

void Symbol::Name(char *name){
  _name=strdup(name);
}


SymbolPtr Symbol::Copy(void){
  SymbolPtr s;
  s=new Symbol(*this);
  return s;
}


void Symbol::SetDefs(defs_node *dn){
	_defsnode=dn;
}

defs_node *Symbol::GetDefs(void){
	return _defsnode;
}

bool Symbol :: getIsPtr() {
  return _isPtr;
}

void Symbol :: putIsPtr(bool isPtr) {
  _isPtr = isPtr;
}
