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
#include "parser.h"
#include "node.h"

Symbol::Symbol()
{
	_name=strdup("");
	_deflist.clear();
	_uselist.clear();
}

Symbol::Symbol(char * name,NodePtr node){
	_name=strdup(name);
	_deflist.push_back(node);
}

Symbol::Symbol(const Symbol & sym)
{
	_name=strdup(sym._name);
	_deflist=sym._deflist;
	_uselist=sym._uselist;
}

char *Symbol::Name(void){
	return _name;
}

void Symbol::Name(char *name){
	_name=strdup(name);
}


void Symbol::AddDef(NodePtr node){
	_deflist.push_back(node);
}

void Symbol::RemoveDef(NodePtr node){
	_deflist.remove(node);
}

void Symbol::AddUse(NodePtr node){
	_uselist.push_back(node);
}


//returns the list of nodes in EXPRESSION IR defining this symbol
NodeList * Symbol::GetDefList(void){
	return &_deflist;
}


//returns the list of nodes in EXPRESSION IR referencing this symbol
NodeList * Symbol::GetUseList(void){
	return &_uselist;
}

int Symbol::any_def_is(int kind)
{
	NodeList::iterator j;
	for(j = _deflist.begin(); j != _deflist.end(); j++)
	{
		if ((*j)->Kind() == kind)
			return 1;
	}
	return 0;
}

//this function should be used to get the ONLY definition in the _deflist of the symbol
//only in the case there is only one such definition. 
//for symbols which have more than one definition, this function should not be used
NodePtr Symbol::GetFirstDef(void){
	//if(_deflist.size()>1) 
	//	compiler_error("Too many elments in def list in symbol.");
	return _deflist.front();
}

SymbolPtr Symbol::Copy(void){
  SymbolPtr s;
  s=new Symbol(*this);
  return s;
}

