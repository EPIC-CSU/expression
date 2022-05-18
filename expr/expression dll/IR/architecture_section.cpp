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

#ifndef __UNIX__
	#include "crtdbg.h"
#endif


//this file contains support functions for the architecture section



//the input node is a use node having as name that type (not the declaration of the type)
//the output is the node declaring
NodePtr base_type(NodePtr node){
	NodePtr base;
	_ASSERT(node);
	_ASSERT(node->Kind()==nke_compinst);
	for(base=node;base->Symbol()->GetFirstDef()!=NULL && 
		base->Symbol()->GetFirstDef()->Parent()->Kind()==nke_subtypedecl;){
		_ASSERT(base->Symbol()->GetFirstDef()->Kind()==nke_subtype);
		base=base->Symbol()->GetFirstDef()->Parent();
	}
	_ASSERT(base->Symbol()->GetFirstDef()->Kind()==nke_subtypedecl);
	return base;
}



//checks if an object is of unit type
int is_unit_type(NodePtr node){
	if(node->Kind()!=nke_component)
		node=node->Symbol()->GetFirstDef();
	_ASSERT(node->Kind()==nke_component);
	return strcmp(base_type(node->Parent())->Name(),"UNIT")==0;
}

//checks if an object is of unit type
int any_def_is_unit_type(NodePtr node)
{
	if(node->Kind() == nke_component)
		return strcmp(base_type(node->Parent())->Name(),"UNIT")==0;
	
	NodeList::iterator diter;
	for(diter=node->Symbol()->GetDefList()->begin(); 
		diter!=node->Symbol()->GetDefList()->end();
		diter++)
	{
		Node* child = (*diter);
		if (child->Kind() == nke_component)
			return strcmp(base_type(child->Parent())->Name(),"UNIT")==0;
	}
	return 0;
}

//checks if an object is of port type
int is_port_type(NodePtr node){
	if(node->Kind()!=nke_component)
		node=node->Symbol()->GetFirstDef();
	_ASSERT(node->Kind()==nke_component);
	return strcmp(base_type(node->Parent())->Name(),"PORT")==0;
}

//checks if an object is of port type
int any_def_is_port_type(NodePtr node){
	if(node->Kind() == nke_component)
		return strcmp(base_type(node->Parent())->Name(),"PORT")==0;
		
	NodeList::iterator diter;
	for(diter=node->Symbol()->GetDefList()->begin(); 
		diter!=node->Symbol()->GetDefList()->end();
		diter++)
	{
		Node* child = (*diter);
		if (child->Kind() == nke_component)
			return strcmp(base_type(child->Parent())->Name(),"PORT")==0;
	}
	return 0;
}

//checks if an object is of connection type
int is_connection_type(NodePtr node){
	if(node->Kind()!=nke_component)
		node=node->Symbol()->GetFirstDef();
	_ASSERT(node->Kind()==nke_component);
	return strcmp(base_type(node->Parent())->Name(),"CONNECTION")==0;
}

//checks if an object is of connection type
int any_def_is_connection_type(NodePtr node){
	if(node->Kind() == nke_component)
		return strcmp(base_type(node->Parent())->Name(),"CONNECTION")==0;
	
	NodeList::iterator diter;
	for(diter=node->Symbol()->GetDefList()->begin(); 
	diter!=node->Symbol()->GetDefList()->end();
	diter++)
	{
		Node* child = (*diter);
		if (child->Kind() == nke_component)
			return strcmp(base_type(child->Parent())->Name(),"CONNECTION")==0;
	}
	return 0;
}


//checks if an object is of storage type
int is_storage_type(NodePtr node){
	if(node->Kind()!=nke_component)
		node=node->Symbol()->GetFirstDef();
	_ASSERT(node->Kind()==nke_component);
	return strcmp(base_type(node->Parent())->Name(),"STORAGE")==0;
}

//checks if an object is of unit type
int any_def_is_storage_type(NodePtr node)
{
	if(node->Kind() == nke_component)
		return strcmp(base_type(node->Parent())->Name(),"STORAGE")==0;
	
	NodeList::iterator diter;
	for(diter=node->Symbol()->GetDefList()->begin(); 
		diter!=node->Symbol()->GetDefList()->end();
		diter++)
	{
		Node* child = (*diter);
		if (child->Kind() == nke_component)
			return strcmp(base_type(child->Parent())->Name(),"STORAGE")==0;
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////
//this is used by the get_holder_for_port function
NodePtr global_object;
NodePtr global_port;

int get_holder(NodePtr node){
	NodePtr ports;
	NodeList::iterator i;
	if(node->Kind()==nke_component){
		ports=node->Child(nke_ports);
		if(ports!=NULL){
			for(i=ports->Children()->begin();i!=ports->Children()->end();i++){
				_ASSERT((*i)->Kind()==nke_port);
				if(global_port->Symbol()==(*i)->Symbol()){
					global_object=node;
					return 1;
				}
			}
		}
	}
	return 0; //when it returns 1, the Traverse function stops the traversal
}



//returns the object that the port is attached to 
NodePtr get_holder_for_port(NodePtr port){
	global_object=NULL;
	global_port=port;
	ir->Traverse(&get_holder);
	//port holder not found for this port
	_ASSERT(global_object!=NULL);
	return global_object;
}

////////////////////////////////////////////////////////////////////////////////
