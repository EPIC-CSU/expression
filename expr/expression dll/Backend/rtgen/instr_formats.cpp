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
#include <iostream.h>

#ifndef __UNIX__
	#include "crtdbg.h"
#else
	#include <stdio.h>
#endif


//prototypes
char *find_field_name(std::map<char *,int> *global_field_map,int val);
std::map<char *,int> *get_field_map(void);
char *get_name(int ident);
extern char *field_map_file;
extern char *instr_format_file;

int debugInstrFormats = 0;

//This is a global variable storing the instr formats as read from the instr_formats file
std::map<int,std::list<int>*> global_instr_formats;
//the number of formats in the global_instr_formats
int global_formats_number=0;
//holds the map between the field names and their identifier
std::map<char *,int> global_field_map;


//used by the partial orders for formats
struct PartialOrderType{
	int field1,field2;
	PartialOrderType(int f1,int f2):field1(f1),field2(f2){}
};

//holds the partial orders for each format
std::map<int,std::list<PartialOrderType*>*> format_partial_orders;

void print_instruction(std::map<int,int> *instruction){
	std::map<int,int>::iterator i;
	for(i=instruction->begin();i!=instruction->end();i++){
		printf("%s (%d) = %s (%d)\n",
			find_field_name(get_field_map(),(*i).first),(*i).first,
			get_name((*i).second),(*i).second);
	}
}

std::map<int,std::list<PartialOrderType*>*> *get_partial_orders(void){
	return &format_partial_orders;
}


std::map<char *,int> *get_field_map(void){
	return &global_field_map;
}

char *find_field_name(std::map<char *,int> *global_field_map,int val){
	std::map<char *,int>::iterator i;
	for(i=global_field_map->begin();i!=global_field_map->end();i++){
		if((*i).second==val) 
			return (*i).first;
	}
	_ASSERT(0);
	return NULL;
}

int find_field_ident(std::map<char *,int> *global_field_map,char *name){
	std::map<char *,int>::iterator i;
	for(i =global_field_map->begin();i!=global_field_map->end();i++){
		if(strcmp((*i).first,name)==0) 
			return (*i).second;
	}
	_ASSERT(0);
	return -1;
}

std::map<int,std::list<int>*> * get_instr_formats(void){
	return & global_instr_formats;
}

int contains_in_list(std::list<int> *format,int val){
	std::list<int>::iterator i;
	for(i=format->begin();i!=format->end();i++){
		if(*i==val) return 1;
	}
	return 0;
}

std::list<int>::iterator find_in_list(std::list<int> *format,int val){
	std::list<int>::iterator i;
	for(i=format->begin();i!=format->end();i++){
		if(*i==val) return i;
	}
	_ASSERT(0);
	return i;
}


//finds the format corresonding to the instruction
int find_format(std::map<int,int>*instruction){
	std::map<int,std::list<int>*> * instr_formats;
	std::map<int,std::list<int>*>::iterator l;
	std::map<int,int>::iterator j;
	std::list<int>::iterator k;
	std::list<int> * format;
	int i,good;
	int instr_field,instr_obj,format_field;


	instr_formats=get_instr_formats();

	//for each format
	for(l=instr_formats->begin(),i=0;l!=instr_formats->end();l++,i++){
		format=(*l).second;
		_ASSERT(format);
		good=1;

		if (debugInstrFormats)
		{
			cout << "Instruction : ";
			for(j=instruction->begin();good && j!=instruction->end();j++)
				cout << (*j).first << " " << (*j).second << endl;

			cout << "Format : ";
			for(k=format->begin();good && k!=format->end();k++)
				cout << (*k) << endl;
		}

		//first check if all the fields in the instruction are included in this format
		for(j=instruction->begin();good && j!=instruction->end();j++){
			instr_field=(*j).first;
			instr_obj=(*j).second;
			_ASSERT(instr_field);
			_ASSERT(instr_obj);
			if(!contains_in_list(format,instr_field)){
				//this foramt is not good for this instruction, move to the next one
				good=0;
			}
		}
		if(good){
			//now check if all the fields in the format are included in the instruction
			for(k=format->begin();good && k!=format->end();k++){
				format_field=*k;
				_ASSERT(format_field);
				if(instruction->find(format_field)==instruction->end()){
					//this foramt is not good for this instruction, move to the next one
					good=0;
				}
			}
		}
		if(good) return i;
	}
	print_instruction(instruction);
	_ASSERT(0);
	return -1;
}


void print_format(std::list<int> *format){
	std::list<int>::iterator i;
	printf("\nFormat: ");
	for(i=format->begin();i!=format->end();i++){
		printf("%s ",find_field_name(&global_field_map,*i));
	}
}

//returns 1 if the object contains the field arg
int contains_arg_in_object(int arg,NodePtr object){
	NodePtr argument;
	NodeList::iterator l;
	for(l=object->Symbol()->GetFirstDef()->Children()->begin();l!=object->Symbol()->GetFirstDef()->Children()->end();l++){
		argument=*l;
		_ASSERT(argument);
		if(argument->Kind()==nke_argument){
			if(arg==find_field_ident(&global_field_map,argument->Name())) 
				return 1;
		}
	}
	return 0;
}

//returns the subnode of object of the specified kind
NodePtr find_node_in_obj(NodePtr object,NodeKindEnum kind){
	NodePtr node;
	NodeList::iterator l;
	for(l=object->Symbol()->GetFirstDef()->Children()->begin();l!=object->Symbol()->GetFirstDef()->Children()->end();l++){
		node=*l;
		_ASSERT(node);
		if(node->Kind()==kind){
			return node;
		}
	}
	return NULL;
}

//returns whether the reservation table rt contains a port corresponding to the argument arg
int contains_arg_in_rt(int arg,ResTab *rt){
	NodeList::iterator j;
	int k;
	for(k=0;k<PIPELENGTH;k++){
		for(j=(*rt)[k]->begin();j!=(*rt)[k]->end();j++){
			if(contains_arg_in_object(arg,*j))
				return 1;
		}
	}
	return 0;
}

//returns whether each element in the format has a port
//corresponding to it
int all_format_considered(std::list<int> *format,ResTab *rt){
	std::list<int>::iterator i;
	for(i=format->begin();i!=format->end();i++){
		if(!contains_arg_in_rt(*i,rt)) return 0;
	}
	return 1;
}

//Returns whether the port is contained in the format
//format: input, the fomat
//port: input, the port to try to find in the format
//field: output, the field identifier corresponding to the port, if it was found
//const _DEST_ = 1;
//const _SOURCE_1_ = 2;
//const _SOURCE_2_ = 3;
//const _MEM_DEST_ = 4;
//const _MEM_SRC_  = 5;
int contains_port_in_format(std::list<int> * format,NodePtr port,int &field){
	NodePtr argument;
	NodeList::iterator i,j;
	int tmp;
	for(j=port->Symbol()->GetFirstDef()->Children()->begin();j!=port->Symbol()->GetFirstDef()->Children()->end();j++){
		argument=*j;
		_ASSERT(argument);
		if(argument->Kind()==nke_argument){
			if(contains_in_list(format,tmp=find_field_ident(&global_field_map,argument->Name()))){
				field=tmp;
				return 1;
			}
		}
	}
	return 0;
}

int has_attached_argument(NodePtr port){
	NodePtr argument;
	argument=port->Symbol()->GetFirstDef()->FindSubNode(nke_argument);
	return argument!=NULL;
}

//const _DEST_ = 1;
//const _SOURCE_1_ = 2;
//const _SOURCE_2_ = 3;
//const _MEM_DEST_ = 4;
//const _MEM_SRC_  = 5;
void read_instr_formats(void){
	std::list<int> * format;
	char *field1,*field2;
	_ASSERT(instr_format_file);
	FILE * file=fopen(instr_format_file,"r");
	char line[256],*token;
	//initialize for reentrance
	global_formats_number=0;
	if(file==NULL){
		printf("\nCouldn not open the instruction formats file...\n");
		abort();
	}
	do{
		//read one line from the formats file
		fgets((char*)(&line),256,file);
		//parse the line, and insert the arguments into the format
		if(strlen((char*)(&line))>1 && !feof(file)){
			format=new std::list<int>();
			//read one token from the line
			token = strtok( line, " \t\n" );
			if(strcmp(token,"FORMAT")==0){
				//it is a format definition line
				//read first token from the format
				token = strtok( NULL, " \t\n" );
				while(token!=NULL){
					format->push_back(find_field_ident(&global_field_map,token));
					//read the next token
					token = strtok( NULL, " \t\n");
				}
				//insert the format in the global instr_formats variable
				global_instr_formats[global_formats_number++]=format;
				//initialize the format_partial_orders
				format_partial_orders[global_formats_number-1]=new std::list<PartialOrderType*>();
			}
			else{
				//it is a line defining the partial order within the current format

				//it should have read an actual format already at this point
				_ASSERT(global_formats_number>0);
				//the first token, already read is the first field in the partial order
				field1=strdup(token);

				//read the -> sign
				token = strtok( NULL, " \t\n" );
				_ASSERT(strcmp(token,"->")==0);

				//read the second field in the partial order line
				token = strtok( NULL, " \t\n" );
				field2=strdup(token);

				//add this partial order to the list of partial orders for the current format
				format_partial_orders[global_formats_number-1]->push_back(
					new PartialOrderType(find_field_ident(get_field_map(),field1),
										 find_field_ident(get_field_map(),field2)));
			}
		}
	}while(!feof(file) && strlen(line)>1);
	fclose(file);
}

void read_field_map(void){
	char *name;
	int val;
	_ASSERT(field_map_file);
	FILE * file=fopen(field_map_file,"r");
	char line[256],*token;
	//clear it for reentrance
	global_field_map.clear();

	if(file==NULL){
		printf("\nCouldn not open the field map file...\n");
		abort();
	}
	do{
		//read one line from the formats file
		fgets((char*)(&line),256,file);
		//parse the line, and insert the arguments into the format
		if(strlen((char*)(&line))>1 && !feof(file)){
			//read one token from the line (char *)
			token = strtok( line, " \t\n" );
			_ASSERT(token);
			name=strdup(token);
			//read the second token, which should be the identifier (int)
			token = strtok( NULL, " \t\n");
			_ASSERT(token);
			val=atoi(token);
			_ASSERT(val>0);
			//insert in global_field_map
			global_field_map[name]=val;
		}
	}while(!feof(file) && strlen(line)>1);
	fclose(file);
}

