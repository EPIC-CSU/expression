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
#include "node.h"
#include "GenerateRT.h"
#include "errors.h"
#include "symtabs.h"
#include "parser.h"
#ifndef __UNIX__
	#include "crtdbg.h"
#endif


std::map<SymbolPtr,int> resource_ident_map;

////////////////////////////////////////////////////////////////////////////////
int get_ident(char *name){
	SymbolPtr s;
	if(strlen(name)==0) return -1;
	s=st.FindSymbol(name);
	_ASSERT(s!=NULL);
	return resource_ident_map[s];
}

int get_ident(NodePtr node){
	SymbolPtr s;
	s=node->Symbol();
	_ASSERT(s!=NULL);
	return resource_ident_map[s];
}

char *get_name(int ident){
	std::map<SymbolPtr,int>::iterator i;
	for(i=resource_ident_map.begin();i!=resource_ident_map.end();i++){
		if((*i).second==ident){
			return (*i).first->Name();
		}
	}
	_ASSERT(0);
	return NULL;
}

NodePtr get_node(int ident){
	std::map<SymbolPtr,int>::iterator i;
	for(i=resource_ident_map.begin();i!=resource_ident_map.end();i++){
		if((*i).second==ident){
			return (st.FindSymbol((*i).first->Name()))->GetFirstDef();
		}
	}
	_ASSERT(0);
	return NULL;
}

SymbolPtr get_symbol(int ident){
	std::map<SymbolPtr,int>::iterator i;
	for(i=resource_ident_map.begin();i!=resource_ident_map.end();i++){
		if((*i).second==ident){
			return st.FindSymbol((*i).first->Name());
		}
	}
	_ASSERT(0);
	return NULL;
}

////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//used to assign int ident numbers for each resource
int crt=0;

int resource_ident(NodePtr node){
	if(node->Kind()==nke_component){
		resource_ident_map[node->Symbol()]=crt++;
	}
	return 0; //when it returns 1, the Traverse function stops the traversal
}
/////////////////////////////////////////////////////////////////////////////////


//Update map of identifiers (int)
void update_resource_ident(NodePtr node){
	//init a map from SymbolPtr to int, giving each component an int identifier
	//These identifiers will be used in the reservation tables to represent the resources
	crt=0;
	node->Traverse(&resource_ident); //the Traverse function calls the forwref function for each node 
}
