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

#include <stdafx.h>
#include <string.h>
#include <stdio.h>
#ifndef __UNIX__
#include <crtdbg.h>
#else
#define _ASSERT assert
#endif

#include "defs_node.h"
#include "symbols.h"
#include "symtabs.h"

#include "STLIncludes.h"
#include "MyString.h"

//prototypes

void defsrestart( FILE *input_file );
int defsparse(void);
void resetLineNumber(void);
extern char *global_filename;
int has_word(defs_node *node);
int is_symbol(char *s);
int has_half(defs_node *node);



//the global definitions root
defs_node_list *global_definitions;

//the symbol table (this the only Symtab object in this project)
Symtab st;

defs_node::defs_node(void){
	name=NULL;
	children.clear();
}

defs_node::defs_node(char *n){
	name=strdup(n);
}

void defs_node::AddChild(defs_node *node){
	children.push_back(node);
}

void defs_node::AddChildren(defs_node_list *node_list){
	defs_node_list::iterator i;
	for(i=node_list->begin();i!=node_list->end();i++){
		children.push_back(*i);
	}
}


void defs_node::Print(void){
	defs_node_list::iterator i;
	if(name!=NULL){
		printf("%s:",name);
	}
	if(children.begin()!=children.end()){
		printf("(");
	}
	for(i=children.begin();i!=children.end();i++){
		(*i)->Print();
	}
	if(children.begin()!=children.end()){
		printf(") ");
	}
}

defs_node_list *defs_node::Children(void){
	return &children;
}


//Traverse traverses the defs_node IR, and calls for each node the func function.
//if func returns 1 it stops and returns
//if fund returns 0 it continues calling for the rest of the IR
//func is a function pointer with NodePtr as argument and returning an int
int defs_node::Traverse(int(*func)(defs_node *)){
	defs_node_list::iterator i;
	if((*func)(this)){
		return 1;
	}
	for(i=children.begin();i!=children.end();i++){
		if((*i)->Traverse(func)) return 1;
	}
	return 0;
}


//Returns the child with the kind kind (assumes there is exactly one)
defs_node *defs_node::Child(char *name){
	defs_node_list::iterator i;
	defs_node *node=NULL;
	int nr=0;
	for(i=children.begin();i!=children.end();i++){
		if(strcmp((*i)->Name(),name)==0){
			node=*i;
			nr++;
		}
	}
	_ASSERT(nr<=1);
	return node;
}



/////////////////////////////////////////////////////////////////////////////
//used by FindNode
static char *static_name;
static defs_node *static_node;

int findnodebyname(defs_node *node){
	if(node->Name()!=NULL && strcmp(node->Name(),static_name)==0){
		static_node=node;
		return 1;//stop traversal
	}
	return 0; //when it returns 1, the Traverse function stops the traversal
}
///////////////////////////////////////////////////////////////////////////////

//This function finds the first node with name name
//in the IR
//Can be called e.g.: ir->FindNode("ADD") and returns the first ADD node
defs_node *defs_node::FindSubNode(char *name){
	static_name=strdup(name);
	static_node=NULL;
	Traverse(&findnodebyname);
	return static_node;
}

///////////////////////////////////////////////////////////////////////////////

//appends all elements from crt_decl_list to decl_list
void append_defs_list(vector<defs_node*> *decl_list,vector<defs_node*> *crt_decl_list){
	vector<defs_node*>::iterator i;

	for(i=crt_decl_list->begin();i!=crt_decl_list->end();i++){
		decl_list->push_back(*i);
	}
}

defs_node *get_global(defs_node *node){
	defs_node_list::iterator i,j;
	//for each child of node
	for(i=node->Children()->begin();i!=node->Children()->end();i++){
		_ASSERT(*i);
		//for each grandchild of node
		for(j=(*i)->Children()->begin();j!=(*i)->Children()->end();j++){
			_ASSERT(*j);
			if(strcmp((*j)->Name(),"IS_GLOBAL")==0){
				//if this is the val definition node
				//return the element following val
				j++;
				return (*j);
			}
		}
	}
	return NULL;
}


//appends the file name to all the declarations in the list, which are static local
void append_file_name_to_static_names(vector<defs_node*> *list, char *filenamecopy){
	vector<defs_node*>::iterator i;
	defs_node *global;
	char buffer[1000];
	char *name;

	for(i=list->begin();i!=list->end();i++){
		//check if it is a local or global variable
		name=(*i)->Name();
		global=get_global(*i);
		if(global!=NULL && atoi(global->Name())==0){
			//if it is local and static
			//need to append the file name to its name, to distinguish it from other locals in other files
			sprintf(buffer,"%s_%s",filenamecopy,name);
			(*i)->Name(strdup(buffer));
		}
	}
	//set the global filename
	global_filename=strdup(filenamecopy);
}


static int is_declared_local(vector<defs_node*> *list,char *buf){
	vector<defs_node*>::iterator k;
	defs_node *global;

	for(k=list->begin();k!=list->end();k++){
		if(strcmp((*k)->Name(),buf)==0){
			//check if this is a local or global
			global=get_global(*k);
			if(global!=NULL && atoi(global->Name())==0){
				//if it is local and static
				return 1;
			}
		}
	}
	return 0;
}

static void add_filename_to_word_list(vector<defs_node*> *list, char *filename){
	vector<defs_node*>::iterator k;
	defs_node *node;
	defs_node_list::iterator i,j;
	char *name;
	char buf[1000];


	//for all the symbols in the list
	for(k=list->begin();k!=list->end();k++){
		node=*k;
		
		if(!has_word(node)) continue;
		
		//for each child of node
		for(i=node->Children()->begin();i!=node->Children()->end();i++){
			_ASSERT(*i);
			//for each grandchild of node
			for(j=(*i)->Children()->begin();j!=(*i)->Children()->end();j++){
				_ASSERT(*j);
				if(strcmp((*j)->Name(),"WORD")==0){
					//if this is the WORD node
					//add the element following WORD to the word_list
					j++;
					name=(*j)->Name();
					if(is_symbol(name)){
						sprintf(buf,"%s_%s",filename,name);
						if(is_declared_local(list,buf)){
							//if this is a local indeed
							//add the filename to the name of this word
							(*j)->Name(strdup(buf));
						}
					}
					else{
						//if it is a number (integer)
					}
				}
			}
		}
	}
}


static void add_filename_to_half_list(vector<defs_node*> *list, char *filename){
	vector<defs_node*>::iterator k;
	defs_node *node;
	defs_node_list::iterator i,j;
	char *name;
	char buf[1000];


	//for all the symbols in the list
	for(k=list->begin();k!=list->end();k++){
		node=*k;
		
		if(!has_half(node)) continue;
		
		//for each child of node
		for(i=node->Children()->begin();i!=node->Children()->end();i++){
			_ASSERT(*i);
			//for each grandchild of node
			for(j=(*i)->Children()->begin();j!=(*i)->Children()->end();j++){
				_ASSERT(*j);
				if(strcmp((*j)->Name(),"HALF")==0){
					//if this is the HALF node
					//add the element following HALF to the half_list
					j++;
					name=(*j)->Name();
					if(is_symbol(name)){
						sprintf(buf,"%s_%s",filename,name);
						if(is_declared_local(list,buf)){
							//if this is a local indeed
							//add the filename to the name of this half
							(*j)->Name(strdup(buf));
						}
					}
					else{
						//if it is a number (integer)
					}
				}
			}
		}
	}
}


//the list of declarations (DECL ...) from the defs file currently being parsed
//this will be moved into the global decl_list
vector<defs_node*> crt_decl_list;
extern vector<defs_node*> decl_list;



//reads the defs file 
void read_defs_file(Vector<MyString> & name_list){
	char *name;
	Vector<MyString>::iterator sIter;
	char filenamecopy[1000],*p;

	for (sIter = name_list.begin(); sIter != name_list.end(); sIter++)
	{
		name=(*sIter).getStr();
		
		if(name!=NULL){
			FILE * f=fopen(name, "r");

			if(f==NULL){
				printf("Could not open file %s\n",name);
				_ASSERT(0);
			}

			printf("Parsing %s...\n",name);
			
			resetLineNumber();
			defsrestart(f);

			//empty crt_decl_list
			crt_decl_list.clear();
			
			//parse the current defs file
			defsparse();

			//remove the extension ".defs" from the filename
			strcpy(filenamecopy,name);
			for(p=filenamecopy;*p!='\0' && *p!='.';p++)
				;
			*p='\0';

			//perform name mingling for the static variables in this file
			append_file_name_to_static_names(&crt_decl_list,filenamecopy);

			//append the file name to all the WORD declarations in this file
			add_filename_to_word_list(&crt_decl_list,filenamecopy);
			add_filename_to_half_list(&crt_decl_list,filenamecopy);

			//append all the defs nodes from crt_decl_list into decl_list
			append_defs_list(&decl_list,&crt_decl_list);
		}
	}

}

void printWordInfo(defs_node *node, ostream &out){
	defs_node_list::iterator i,j;
	//for each child of node
	for(i=node->Children()->begin();i!=node->Children()->end();i++){
		_ASSERT(*i);
		//for each grandchild of node
		for(j=(*i)->Children()->begin();j!=(*i)->Children()->end();j++){
			_ASSERT(*j);
			if(strcmp((*j)->Name(),"WORD")==0){
				//if this is the val definition node
				//return the element following val
				j++;
				out << "\t.word\t";  // EXPR text file
				out << (*j)->Name() << endl;
			}
		}
	}
	return;
}

void printHalfInfo(defs_node *node, ostream &out){
	defs_node_list::iterator i,j;
	//for each child of node
	for(i=node->Children()->begin();i!=node->Children()->end();i++){
		_ASSERT(*i);
		//for each grandchild of node
		for(j=(*i)->Children()->begin();j!=(*i)->Children()->end();j++){
			_ASSERT(*j);
			if(strcmp((*j)->Name(),"HALF")==0){
				//if this is the val definition node
				//return the element following val
				j++;
				out << "\t.half\t";  // EXPR text file
				out << (*j)->Name() << endl;
			}
		}
	}
	return;
}

void printByteInfo(defs_node *node, ostream &out){
	defs_node_list::iterator i,j;
	//for each child of node
	for(i=node->Children()->begin();i!=node->Children()->end();i++){
		_ASSERT(*i);
		//for each grandchild of node
		for(j=(*i)->Children()->begin();j!=(*i)->Children()->end();j++){
			_ASSERT(*j);
			if(strcmp((*j)->Name(),"BYTE")==0){
				//if this is the val definition node
				//return the element following val
				j++;
				out << "\t.byte\t";  // EXPR text file
				out << (*j)->Name() << endl;
			}
		}
	}
	return;
}

void printAlignInfo(defs_node *node, ostream &out){
	defs_node_list::iterator i,j;
	//for each child of node
	for(i=node->Children()->begin();i!=node->Children()->end();i++){
		_ASSERT(*i);
		//for each grandchild of node
		for(j=(*i)->Children()->begin();j!=(*i)->Children()->end();j++){
			_ASSERT(*j);
			if(strcmp((*j)->Name(),"ALIGN")==0){
				//if this is the val definition node
				//return the element following val
				j++;
				out << "\t.align\t";  // EXPR text file
				out << (*j)->Name() << endl;
				return;
			}
		}
	}
}

