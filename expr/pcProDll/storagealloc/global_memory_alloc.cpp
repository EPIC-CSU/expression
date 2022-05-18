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
#ifdef WIN32
#pragma warning(disable:4786)
#include <map>
#include <vector>
#endif

#ifdef __UNIX__
#include <ctype.h>
#endif

#include "stdafx.h"

#ifdef WIN32
#include "defs_node.h"
#include "symbols.h"
#include "symtabs.h"
#endif

#include "IConstArg.h"
#include "LabelArg.h"
#include "ComputeOp.h"
#include "CallOp.h"
#include "MemoryOp.h"
#include "Routine.h"
#include "Program.h"

#ifdef __UNIX__
#include "defs_node.h"
#include "symbols.h"
#include "symtabs.h"
#endif

#ifdef map
#undef map
#endif 
#define map std::map

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 


//prototypes
extern Routine *curRoutine;
int is_symbol(char *s);
long int get_memory_assig(char *name);
int find_memory_assig(char *name);
void print_mem_alloc(void);

//the filename of the file currently being parsed
char *global_filename;

long int get_memory_assig(char *name);
long int get_assigned_size(char *name);

long int GLOBALS_LOWER_LIMIT=110000;
//the stack pointer has to be initialized to the upper limit. 
long int GLOBAlS_UPPER_LIMIT=10000000;

//Prototypes for ASM

void printAlignInfo(defs_node *, ostream &);
void printWordInfo(defs_node *, ostream &);
void printByteInfo(defs_node *, ostream &);
void printHalfInfo(defs_node *, ostream &);
defs_node *get_global(defs_node *);


//
// Added by SAH on Mon Jan 24, 2000.
// If you make any mod to this number also, update
// STACK_START_ADDRESS in DerivedClassDll/ExpressCallSupport.cpp

//The memory organization is the following: 
// -------------------------
// Addr MEM_LOWER_LIMIT
//         global variables
// -------------------------
//         heap
// -------------------------
//         stack
// Addr MEM_UPPER_LIMIT

//the stack grows downwards from the upper limit
//the heap grows uppwards
//this is used during global memory allocation as the current highest limit
long int MEM_CRT=GLOBALS_LOWER_LIMIT;

//the default size of static variables which have no size declared by gcc
#define DEFAULT_STATIC_SIZE 100

//the default size of static variables which have no size declared by gcc
#define DEFAULT_INT_SIZE 4

//the memory assignment is stored here
map<char *,long int> memory_assignment;
//the size of each symbol (for debugging purposes)
map<char *,long int> symbol_size;

//the list of definitions (DEF ...) from the defs file
vector<defs_node*> defs_list;
//the list of declarations (DECL ...) from the defs file
vector<defs_node*> decl_list;


int debugType=0;




defs_node *get_type(defs_node *node){
	defs_node_list::iterator i,j;
	if(debugType) printf("\nDetermining type:\n");
	if(debugType) node->Print();
	//for each child of node
	for(i=node->Children()->begin();i!=node->Children()->end();i++){
		_ASSERT(*i);
		//for each grandchild of node
		for(j=(*i)->Children()->begin();j!=(*i)->Children()->end();j++){
			_ASSERT(*j);
			if(strcmp((*j)->Name(),"type")==0){
				//if this is the type definition node
				return (*i);
			}
		}
	}
	return NULL;
}

defs_node *get_size(defs_node *node){
	defs_node_list::iterator i,j;
	//for each child of node
	for(i=node->Children()->begin();i!=node->Children()->end();i++){
		_ASSERT(*i);
		//for each grandchild of node
		for(j=(*i)->Children()->begin();j!=(*i)->Children()->end();j++){
			_ASSERT(*j);
			if(strcmp((*j)->Name(),"size")==0){
				//if this is the val definition node
				//return the element following val
				j++;
				return (*j);
			}
		}
	}
	return NULL;
}

defs_node *get_space(defs_node *node){
	defs_node_list::iterator i,j;
	//for each child of node
	for(i=node->Children()->begin();i!=node->Children()->end();i++){
		_ASSERT(*i);
		//for each grandchild of node
		for(j=(*i)->Children()->begin();j!=(*i)->Children()->end();j++){
			_ASSERT(*j);
			if(strcmp((*j)->Name(),"SPACE")==0){
				//if this is the val definition node
				//return the element following val
				j++;
				return (*j);
			}
		}
	}
	return NULL;
}

defs_node *get_val(defs_node *node){
	defs_node_list::iterator i,j;
	//for each child of node
	for(i=node->Children()->begin();i!=node->Children()->end();i++){
		_ASSERT(*i);
		//for each grandchild of node
		for(j=(*i)->Children()->begin();j!=(*i)->Children()->end();j++){
			_ASSERT(*j);
			if(strcmp((*j)->Name(),"val")==0){
				//if this is the val definition node
				//return the element following val
				j++;
				return (*j);
			}
		}
	}
	return NULL;
}

int has_word(defs_node *node){
	defs_node_list::iterator i,j;
	//for each child of node
	for(i=node->Children()->begin();i!=node->Children()->end();i++){
		_ASSERT(*i);
		//for each grandchild of node
		for(j=(*i)->Children()->begin();j!=(*i)->Children()->end();j++){
			_ASSERT(*j);
			if(strcmp((*j)->Name(),"WORD")==0){
				//if this is the WORD node
				return 1;
			}
		}
	}
	return 0;
}

int has_half(defs_node *node){
	defs_node_list::iterator i,j;
	//for each child of node
	for(i=node->Children()->begin();i!=node->Children()->end();i++){
		_ASSERT(*i);
		//for each grandchild of node
		for(j=(*i)->Children()->begin();j!=(*i)->Children()->end();j++){
			_ASSERT(*j);
			if(strcmp((*j)->Name(),"HALF")==0){
				//if this is the WORD node
				return 1;
			}
		}
	}
	return 0;
}

int has_byte(defs_node *node){
	defs_node_list::iterator i,j;
	//for each child of node
	for(i=node->Children()->begin();i!=node->Children()->end();i++){
		_ASSERT(*i);
		//for each grandchild of node
		for(j=(*i)->Children()->begin();j!=(*i)->Children()->end();j++){
			_ASSERT(*j);
			if(strcmp((*j)->Name(),"BYTE")==0){
				//if this is the WORD node
				return 1;
			}
		}
	}
	return 0;
}

int count_word_nodes(defs_node *node){
	defs_node_list::iterator i,j;
	int r=0;

	//for each child of node
	for(i=node->Children()->begin();i!=node->Children()->end();i++){
		_ASSERT(*i);
		//for each grandchild of node
		for(j=(*i)->Children()->begin();j!=(*i)->Children()->end();j++){
			_ASSERT(*j);
			if(strcmp((*j)->Name(),"WORD")==0){
				//if this is the WORD node
				r++;
			}
		}
	}
	return r;
}

void print_word_nodes(defs_node *node){
	defs_node_list::iterator i,j;

	printf("\n");
	//for each child of node
	for(i=node->Children()->begin();i!=node->Children()->end();i++){
		_ASSERT(*i);
		//for each grandchild of node
		for(j=(*i)->Children()->begin();j!=(*i)->Children()->end();j++){
			_ASSERT(*j);
			if(strcmp((*j)->Name(),"WORD")==0){
				//if this is the WORD node
				j++;
				printf("{WORD %s) ",(*j)->Name());
			}
		}
	}
}

int count_half_nodes(defs_node *node){
	defs_node_list::iterator i,j;
	int r=0;

	//for each child of node
	for(i=node->Children()->begin();i!=node->Children()->end();i++){
		_ASSERT(*i);
		//for each grandchild of node
		for(j=(*i)->Children()->begin();j!=(*i)->Children()->end();j++){
			_ASSERT(*j);
			if(strcmp((*j)->Name(),"HALF")==0){
				//if this is the HALF node
				r++;
			}
		}
	}
	return r;
}

// For ASM

int count_byte_nodes(defs_node *node){
	defs_node_list::iterator i,j;
	int r=0;

	//for each child of node
	for(i=node->Children()->begin();i!=node->Children()->end();i++){
		_ASSERT(*i);
		//for each grandchild of node
		for(j=(*i)->Children()->begin();j!=(*i)->Children()->end();j++){
			_ASSERT(*j);
			if(strcmp((*j)->Name(),"BYTE")==0){
				//if this is the BYTE node
				r++;
			}
		}
	}
	return r;
}


#define _NO_SYSTEM_CALLS_

extern Program *curProgram;
#ifndef _NO_SYSTEM_CALLS_
extern int getSystemCallNameAsInt(char *name);
#endif

int get_function_address(char *name)
{
	// Go thru all the functions in currProgram.
	// if a function of that name exists, return its address.
	//
	Routine *rout = curProgram->getRoutine(name);
	// int sysVal;

	if (rout != NULL) // found it!
		return ((int)rout);

	// it could be a system/library call

#ifndef _NO_SYSTEM_CALLS_
	if ((sysVal = getSystemCallNameAsInt(name)) != _INVALID_)
		return sysVal;
#endif

	return _INVALID_;
}




vector<int> * get_word_list(defs_node *node){
	defs_node_list::iterator i,j;
	vector<int> *word_list;
	char *name;
	long int address;


	if(!has_word(node)) return NULL;

	word_list=new vector<int>;

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
					//if it is an identifier
					//find the address for this identifier
					address=get_memory_assig(name);
					/*
					if(address==-1){
					//try to see if its a local
					//filename=get_file_name();
					sprintf(buf,"%s_%s",filename,name);
					address=get_memory_assig(name);
					}
					*/
					if(address==-1){
						//if this is not an already assigned identifier
						//maybe it is a function
						address=get_function_address(name);
						if(address!=-1){
							word_list->push_back(address);
						}
						else{
							_ASSERT(0);
						}
					}
					else{
						//if found an address
						//add it
						word_list->push_back(address);
					}
				}
				else{
					//if it is a number (integer)
					word_list->push_back(atoi(name));
				}
			}
		}
	}
	return word_list;
}


vector<short> * get_half_list(defs_node *node){
	defs_node_list::iterator i,j;
	vector<short> *half_list;
	char *name;
	long int address;


	if(!has_half(node)) return NULL;

	half_list=new vector<short>;

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
					//if it is an identifier
					//find the address for this identifier
					address=get_memory_assig(name);
					/*
					if(address==-1){
						//try to see if its a local
						//filename=get_file_name();
						sprintf(buf,"%s_%s",filename,name);
						address=get_memory_assig(name);
					}
					*/
					if(address==-1){
						//if this is not an already assigned identifier
						//maybe it is a function
						address=get_function_address(name);
						if(address!=-1){
							half_list->push_back(address);
						}
						else 
							_ASSERT(0);
					}
					else{
						//if found an address
						//add it
						half_list->push_back(address);
					}
				}
				else{
					//if it is a number (integer)
					half_list->push_back(atoi(name));
				}
			}
		}
	}
	return half_list;
}

vector<char> * get_byte_list(defs_node *node){
	defs_node_list::iterator i,j;
	vector<char> *byte_list;
	char *name;
	long int address;


	if(!has_byte(node)) return NULL;

	byte_list=new vector<char>;

	//for each child of node
	for(i=node->Children()->begin();i!=node->Children()->end();i++){
		_ASSERT(*i);
		//for each grandchild of node
		for(j=(*i)->Children()->begin();j!=(*i)->Children()->end();j++){
			_ASSERT(*j);
			if(strcmp((*j)->Name(),"BYTE")==0){
				//if this is the HALF node
				//add the element following HALF to the half_list
				j++;
				name=(*j)->Name();
				if(is_symbol(name)){
					//if it is an identifier
					//find the address for this identifier
					address=get_memory_assig(name);
					/*
					if(address==-1){
						//try to see if its a local
						//filename=get_file_name();
						sprintf(buf,"%s_%s",filename,name);
						address=get_memory_assig(name);
					}
					*/
					if(address==-1){
						//if this is not an already assigned identifier
						//maybe it is a function
						address=get_function_address(name);
						if(address!=-1){
							byte_list->push_back((char)address);
						}
						else 
							_ASSERT(0);
					}
					else{
						//if found an address
						//add it
						byte_list->push_back((char)address);
					}
				}
				else{
					//if it is a char (integer)
					int tmp = atoi(name);
					byte_list->push_back((char)tmp);
				}
			}
		}
	}
	return byte_list;
}

defs_node *get_ascii(defs_node *node){
	defs_node_list::iterator i,j;
	//for each child of node
	for(i=node->Children()->begin();i!=node->Children()->end();i++){
		_ASSERT(*i);
		//for each grandchild of node
		for(j=(*i)->Children()->begin();j!=(*i)->Children()->end();j++){
			_ASSERT(*j);
			if(strcmp((*j)->Name(),"ASCII")==0){
				//if this is the val definition node
				//return the element following val
				j++;
				return (*j);
			}
		}
	}
	return NULL;
}

//returns 1 if s is a symbol (identifier, as opposed to number)
int is_symbol(char *s){
	if(isalpha(*s)) return 1;
	if(*s=='_') return 1;
	if(isdigit(*s)) return 0;
	if(*s=='-' || *s=='+') return 0;
	return 1;
}

//////////////////////////////////////////////////////////////////////////////

//prints out the memory allocation
void print_mem_alloc(void){
	defs_node *declaration,*definition,*temp;
	vector<defs_node*>::iterator i;
	int wn;
	long int start_addr,end_addr;

	printf("\nGlobal Memory Allocation:\n");
	//for each declaration node in the code
	for(i=decl_list.begin();i!=decl_list.end();i++){
		//this is the declaration node
		declaration=*i;
		start_addr=get_memory_assig(declaration->Name());
		end_addr=start_addr+get_assigned_size(declaration->Name())-1;
		printf("%s\t%ld\t%ld\t",declaration->Name(),start_addr,end_addr);
		if((temp=get_space(declaration))!=NULL){
			printf("SPACE=%s\t",temp->Name());
		}
		if((temp=get_val(declaration))!=NULL){
			printf("VAL=%s\t",temp->Name());
		}
		if((temp=get_size(declaration))!=NULL){
			printf("SIZE=%s\t",temp->Name());
		}
		if((wn=count_word_nodes(declaration))!=0){
			printf("WORD_NODES=%d\t",wn);
			print_word_nodes(declaration);
		}
		if((temp=get_ascii(declaration))!=NULL){
			printf("ASCII=%s\t",temp->Name());
		}
		printf("\n");
	}
	printf("\nLocal Memory Allocation:\n");
	//for each definition node in the code
	for(i=defs_list.begin();i!=defs_list.end();i++){
		//this is the declaration node
		definition=*i;
		printf("%s\t",definition->Name());
		if((temp=get_val(definition))!=NULL){
			printf("VAL=%s\t",temp->Name());
		}
		if((temp=get_size(definition))!=NULL){
			printf("SIZE=%s\t",temp->Name());
		}
		if((wn=count_word_nodes(definition))!=0){
			printf("WORD_NODES=%d\t",wn);
		}
		if((wn=count_half_nodes(definition))!=0){
			printf("HALF_NODES=%d\t",wn);
		}
		if((wn=count_byte_nodes(definition))!=0){
			printf("BYTE_NODES=%d\t",wn);
		}
		if((temp=get_space(definition))!=NULL){
			printf("SPACE=%s\t",temp->Name());
		}
		if((temp=get_ascii(definition))!=NULL){
			printf("ASCII=%s\t",temp->Name());
		}
		printf("\n");
	}
}

//assigns a size to a symbol
void assign_size(char *name,long int size){
	symbol_size[name]=size;
}

//returns the size of a symbol
long int get_assigned_size(char *name){
	map<char *,long int>::iterator i;
	long int r;

	for(i=symbol_size.begin();i!=symbol_size.end();i++){
		if(strcmp((*i).first,name)==0){
			r=(*i).second;
			return r;
		}
	}
	//symbol not found
	//_ASSERT(0);
	return -1;
}

//assigns a memory location (val) to a symbol (name)
void assign_memory(char *name,long int val){
	memory_assignment[name]=val;
}

//changes a memory assignment for this symbol
void change_memory(char *name,long int val){
	map<char *,long int>::iterator i;

	for(i=memory_assignment.begin();i!=memory_assignment.end();i++){
		if(strcmp((*i).first,name)==0){
			memory_assignment[(*i).first]=val;
			return;
		}
	}
}

//returns the memory lcoation assigned to this symbol (assumes it has been assigned)
long int get_memory_assig(char *name){
	map<char *,long int>::iterator i;
	long int r;

	if(strcmp(name,"__iob")==0){
		//if it is either stdout, or stdin
		//use a very large negative number, to be able to recognize
		//it in the simulator for the fprintf function
		return -10000;
	}

	for(i=memory_assignment.begin();i!=memory_assignment.end();i++){
		if(strcmp((*i).first,name)==0){
			r=(*i).second;
			return r;
		}
	}
	//symbol not found
	//_ASSERT(0);
	return -1;
}

//returns the memory lcoation assigned to this symbol (assumes it has been assigned)
defs_node *get_node(char *name){
	defs_node *declaration;
	vector<defs_node*>::iterator i;
	char *symbol_name;

	//for each declaration node in the code
	for(i=decl_list.begin();i!=decl_list.end();i++){
		//this is the declaration node
		declaration=*i;
		//find the name of this declaration
		symbol_name=declaration->Name();
		if(strcmp(name,symbol_name)==0){
			return declaration;
		}
	}
	//_ASSERT(0);
	return NULL;
}

//returns 1 if this symbol has a memory assigned
int find_memory_assig(char *name){
	map<char *,long int>::iterator i;

	for(i=memory_assignment.begin();i!=memory_assignment.end();i++){
		if(strcmp((*i).first,name)==0){
			return 1;
		}
	}
	//symbol not found
	return 0;
}

//returns the defs node corresponding to a decl node
defs_node *get_defs_for_decl(defs_node *declaration){
	vector<defs_node*>::iterator i;
	defs_node *definition,*val_node;

	//for all the definitions in (DEF ...) in the code
	for(i=defs_list.begin();i!=defs_list.end();i++){
		definition=*i;
		//find the val of this definition
		val_node=get_val(definition);
		//if it has a val node
		if(val_node!=NULL){
			//if the val of this definition is equal to the name of the declaration
			if(strcmp(declaration->Name(),val_node->Name())==0){
				//return this definition as the rezult
				return definition;
			}
		}
	}
	//definition not found
	return NULL;
}



////////////////////////////////////////////////////////////////////////////////


//traverses the decl_list, and determines the sizes of all the global symbols
void size_all_symbols(void){
	defs_node *size_node,*space_node,*ascii_node;
	defs_node *definition,*declaration;
	vector<defs_node*>::iterator i;
	char *symbol_name;
	long size;

	//for each declaration node in the code
	for(i=decl_list.begin();i!=decl_list.end();i++){
		//this is the declaration node
		declaration=*i;
		//find the name of this declaration
		symbol_name=declaration->Name();
		//find the definition corresponding to this declaration
		definition=get_defs_for_decl(declaration);
		if(definition!=NULL){
			//if there is a definition for this declaration (for most cases there should be one)
			//find the size of this symbol (in bytes)
			size_node=get_size(definition);
			if(size_node!=NULL){
				//find the actual size as a long
				size=atol(size_node->Name());
			}
			else{
				//if it has no SIZE field in the DEF
				//use the SPACE field in the DECL instead
				space_node=get_space(declaration);
				if(space_node!=NULL){
					size=atol(space_node->Name());
				}
				else{
					//default, if it doesn't have neither a SPACE, nor a SIZE,
					//we allocate the size of an INT
					size=DEFAULT_STATIC_SIZE;
				}
			}
		}
		else if(declaration!=NULL){
			//if it has no SIZE field in the DEF
			//use the SPACE field in the DECL instead
			space_node=get_space(declaration);
			ascii_node=get_ascii(declaration);

			if(space_node!=NULL){
				size=atol(space_node->Name());
			}
			else if(has_word(declaration)){
				size=count_word_nodes(declaration) * DEFAULT_INT_SIZE;
			}
			else if(has_half(declaration)){
				size=count_half_nodes(declaration) * DEFAULT_INT_SIZE;
			}
			else if(has_byte(declaration)){
				size=count_byte_nodes(declaration) * DEFAULT_INT_SIZE;
			}
			else if(ascii_node!=NULL){
				size=strlen(ascii_node->Name())+10;
			}
			else{
				//default, if it doesn't have neither a SPACE, nor a SIZE,
				//we allocate the size of 100 INTs
				size=DEFAULT_STATIC_SIZE;
			}
		}
		else{
			//if this declaration has not definition, it is a global introduced by gcc
			//since we do not know the size, we use instead DEFAULT_STATIC_SIZE
			size=DEFAULT_STATIC_SIZE;
		}
		//save the size of this guy
		assign_size(symbol_name,size);
	}
}



//traverses the decl_list, and allocates all symbols to memory 
void allocate_all_symbols(void){
	defs_node *declaration;
	vector<defs_node*>::iterator i;
	char *symbol_name;
	long size;

	//for each declaration node in the code
	for(i=decl_list.begin();i!=decl_list.end();i++){
		//this is the declaration node
		declaration=*i;
		//find the name of this declaration
		symbol_name=declaration->Name();
		//save the size of this guy
		size=get_assigned_size(symbol_name);
		//assign MEM_CRT to this guy 
		assign_memory(symbol_name,MEM_CRT);
		//update MEM_CRT with the size of this guy 
		MEM_CRT = MEM_CRT + size;
	}
}

//prints labels in the data section
void print_labels(ostream &out){
	defs_node *declaration,*temp;
	long size;
	//char *symName;
	char symName[2000];
	vector<defs_node*>::iterator i;
	int wn;

	for(i=decl_list.begin();i!=decl_list.end();i++){
		//this is the declaration node
		declaration=*i;
		//symName = (char *)malloc(sizeof(declaration->Name())+1);
		strcpy(symName, declaration->Name());
		// Global symbol info
		if((temp = get_global(declaration)) != NULL){
			if(strcmp(temp->Name(), "1")==0){
			    out << "\t.globl\t";  // EXPR text file
				out << symName << endl;  
			}
		}
		// Alignment info
		printAlignInfo(declaration, out);
		// half info
		
		if((wn=count_half_nodes(declaration))!=0){
			out << "\t.size " << symName << ", ";  // EXPR text file
			size = wn * 2;
			out << size << endl;
            out << symName << ":" << endl;
			printHalfInfo(declaration, out);
		}
		// Byte info
		if((wn=count_byte_nodes(declaration))!=0){
			out << "\t.size " << symName << ", ";  // EXPR text file
			size = wn;
			out << size << endl;
            out << symName << ":" << endl;
			printByteInfo(declaration, out);
		}
		// Word info
		if((wn=count_word_nodes(declaration))!=0){
			out << "\t.size " << symName << ", ";  // EXPR text file
			size = wn * DEFAULT_INT_SIZE;
			out << size << endl;
            out << symName << ":" << endl;
			printWordInfo(declaration, out);
		}
		// Space info

		if((temp=get_space(declaration))!=NULL){
			out << "\t.size " << symName << ", ";  // EXPR text file
			size = atol(temp->Name());
			out << size << endl;
			out << symName << ":" << endl;
			out << "\t.space\t";  // EXPR text file
			out << temp->Name() << endl;
		}
		// ASCII string
		
		if((temp=get_ascii(declaration))!=NULL){
			out << symName << ":" << endl;
			out << "\t.asciiz\t";   // EXPR text file
			out << "\"" << temp->Name() << "\"" << endl;
		}
	}
}

#include "defs_node.h"

extern vector<defs_node *> decl_list;
typedef vector<defs_node *>::iterator Defs_node_iter;

extern defs_node *get_ascii(defs_node *node);
extern Vector<int> *get_word_list(defs_node *node);
extern Vector<short> *get_half_list(defs_node *node);
extern Vector<char> *get_byte_list(defs_node *node);

extern long get_memory_assig(char *name);

const int _WORD_SIZE_ = 4;
const int _HALF_SIZE_ = 2;
const int _BYTE_SIZE_ = 1;

void dumpDataSection(ostream & out)
{
	out << "Data Section: " << endl;

	Defs_node_iter i;

	defs_node *currNode, *asciiVal;

	char *asciiStr;

	long memAddr;

	Vector<int> *wordList;
	Vector<int>::iterator wI;
	Vector<short> *halfList;
	Vector<short>::iterator hI;
	Vector<char> *byteList;
	Vector<char>::iterator bI;

	for (i = decl_list.begin(); i != decl_list.end(); i++)
	{
		if ((currNode = (*i)) != NULL) // i.e. is a valid node.
		{
			if ((asciiVal = get_ascii(currNode)) != NULL)
			{ // i.e. the decl has an ascii value that needs to be initialized.

				// First get the memory location that needs to be initialized.
				//
				// Then, get the initialization string.

				// Finally, dump the address and the value to be stored in that address.
				//
				memAddr = get_memory_assig(currNode->Name());
				asciiStr = asciiVal->Name();

				out << "ASCII: " << memAddr << " " << asciiStr << endl;
			}
			else if ((wordList = get_word_list(currNode)) != NULL)
			{
				// Iterate over wordlist, and initialize memory.
				//
				memAddr = get_memory_assig(currNode->Name());

				for (wI = wordList->begin(); wI != wordList->end(); wI++)
				{
					out << "WORD: " << memAddr << " " << *wI << endl;

					memAddr += _WORD_SIZE_;
				}
				delete wordList;
			}
			else if ((halfList = get_half_list(currNode)) != NULL)
			{
				// Iterate over halflist, and initialize memory.
				//
				memAddr = get_memory_assig(currNode->Name());

				for (hI = halfList->begin(); hI != halfList->end(); hI++)
				{
					out << "HALF: " << memAddr << " " << *hI << endl;

					memAddr += _HALF_SIZE_;
				}
				delete halfList;
			}
			else if ((byteList = get_byte_list(currNode)) != NULL)
			{
				// Iterate over byteList, and initialize memory.
				//
				memAddr = get_memory_assig(currNode->Name());

				for (bI = byteList->begin(); bI != byteList->end(); bI++)
				{
					out << "BYTE: " << memAddr << " " << *bI << endl;

					memAddr += _BYTE_SIZE_;
				}
				delete byteList;
			}
		}
	}
	out << "End Data Section " << endl;
}


