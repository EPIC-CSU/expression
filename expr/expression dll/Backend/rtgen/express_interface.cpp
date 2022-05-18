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
#endif

#include "node.h"
#include "GenerateRT.h"
#include "errors.h"
#ifndef __UNIX__
	#include "crtdbg.h"
#else
	#define __declspec(dllexport) 
#endif

#include <set>

//this file contains functions to interface the EXPRESSION RT generation to EXPRESS

//prototypes from EXPRESSION
ResTab * get_rt(char *opcode,std::map<int,int> *instruction,int format,int &computed,int dbEnabled=0);
int find_field_ident(std::map<char *,int> *global_field_map,char *name);
int get_ident(char *name);
std::map<char *,int> *get_field_map(void);
int get_ident(NodePtr node);
int get_read_write_time(char *opcode,std::map<int,int>*instruction,int format,int argument,int &computed);
NodePtr get_node(int ident);
char *get_name(int ident);
extern int dbEnabled;
void delete_db(void);
void get_feedback_latencies(char *opcode1,std::map<int,int>*instruction1,int format1,int &computed1,
						   char *opcode2,std::map<int,int>*instruction2,int format2,int &computed2,
						   int *latencies);
int exists_queue_between(char *opcode1,std::map<int,int>*instruction1,int format1,int &computed1,
						   char *opcode2,std::map<int,int>*instruction2,int format2,int &computed2,
						   int latency);

//prototypes from EXPRESS
typedef std::map<int,std::set<int>*> ResTabType;

//the max size of the array of ints representing the rt used to transmit back to EXPRESS the rt
//This is needed because of the dll parameter transfer problem of STL
const int MAXRT=1000;


//computes the reservation table, and provides it to EXPRESS
__declspec( dllexport ) void GetReservationTableEXPRESSION(char *opcode,char **instruction,char *instr_type,int format,int *rt,int &computed){
	std::map<int,int> expr_instruction;
	char **i;
	ResTab *expr_rt;
	int j,l;
	NodeList::iterator k;
	char *field,*unit;
	char opcode_buf[100];

	if(instr_type!=NULL){
		//concatenate the instr type with the opcode to form the complete opcode (e.g., LDW_FPM)
		strcpy(opcode_buf,opcode);
		strcat(opcode_buf,"_");
		strcat(opcode_buf,instr_type);
		//ATTENTION: remove the last character from the opcode
		opcode_buf[strlen(opcode_buf)-1]='\0';
	}
	else
		strcpy(opcode_buf,opcode);

	//first create the expr_instruction from instruction
	for(i=instruction;*i!=NULL;){
		field=*i++;
		unit=*i++;
		expr_instruction[find_field_ident(get_field_map(),field)]=get_ident(unit);
	}

	//compute the reservation table
	//1 means database enabled
	expr_rt=get_rt(opcode_buf,&expr_instruction,format,computed,dbEnabled);

	//convert the reservation table expr_rt to EXPRESS format
	for(j=0,l=0;j<PIPELENGTH;j++){
		for(k=(*expr_rt)[j]->begin();k!=(*expr_rt)[j]->end();k++){
			rt[l++]=get_ident(*k);
			_ASSERT(l<MAXRT);
		}
		//insert the stage separator 
		rt[l++]=-1;
		_ASSERT(l<MAXRT);
	}
}


__declspec( dllexport ) int GetReadWriteTimeEXPRESSION(char *opcode,char **instruction,char *instr_type,int format,int argument,int &computed){
	std::map<int,int> expr_instruction;
	char **i;
	char opcode_buf[100];

	if(instr_type!=NULL){
		//concatenate the instr type with the opcode to form the complete opcode (e.g., LDW_FPM)
		strcpy(opcode_buf,opcode);
		strcat(opcode_buf,"_");
		strcat(opcode_buf,instr_type);
		//ATTENTION: remove the last character from the opcode
		opcode_buf[strlen(opcode_buf)-1]='\0';
	}
	else
		strcpy(opcode_buf,opcode);

	//first create the expr_instruction from instruction
	for(i=instruction;*i!=NULL;){
		expr_instruction[find_field_ident(get_field_map(),*i++)]=get_ident(*i++);
	}

	//compute the read or write time
	return get_read_write_time(opcode_buf,&expr_instruction,format,argument,computed);
}


__declspec( dllexport ) void GetFeedbackLatenciesEXPRESSION(char *opcode1,char **instruction1,char *instr_type1,int format1,int &computed1,
														   char *opcode2,char **instruction2,char *instr_type2,int format2,int &computed2,int *latencies){
	std::map<int,int> expr_instruction1,expr_instruction2;
	char **i;
	char opcode_buf1[100],opcode_buf2[100];

	if(instr_type1!=NULL){
		//concatenate the instr type with the opcode to form the complete opcode (e.g., LDW_FPM)
		strcpy(opcode_buf1,opcode1);
		strcat(opcode_buf1,"_");
		strcat(opcode_buf1,instr_type1);
		//ATTENTION: remove the last character from the opcode
		opcode_buf1[strlen(opcode_buf1)-1]='\0';
	}
	else
		strcpy(opcode_buf1,opcode1);

	if(instr_type2!=NULL){
		//concatenate the instr type with the opcode to form the complete opcode (e.g., LDW_FPM)
		strcpy(opcode_buf2,opcode2);
		strcat(opcode_buf2,"_");
		strcat(opcode_buf2,instr_type2);
		//ATTENTION: remove the last character from the opcode
		opcode_buf2[strlen(opcode_buf2)-1]='\0';
	}
	else
		strcpy(opcode_buf2,opcode2);

	//first create the expr_instruction from instruction
	for(i=instruction1;*i!=NULL;){
		expr_instruction1[find_field_ident(get_field_map(),*i++)]=get_ident(*i++);
	}

	//first create the expr_instruction from instruction
	for(i=instruction2;*i!=NULL;){
		expr_instruction2[find_field_ident(get_field_map(),*i++)]=get_ident(*i++);
	}

	//compute the latencies due to feedback paths between the two operations
	get_feedback_latencies(opcode_buf1,&expr_instruction1,format1,computed1,
		opcode_buf2,&expr_instruction2,format2,computed2,latencies);
}


__declspec( dllexport ) int ExistsQueueBetweenEXPRESSION(char *opcode1,char **instruction1,char *instr_type1,int format1,int &computed1,
			char *opcode2,char **instruction2,char *instr_type2,int format2,int &computed2,int latency){
	std::map<int,int> expr_instruction1,expr_instruction2;
	char **i;
	char opcode_buf1[100],opcode_buf2[100];

	if(instr_type1!=NULL){
		//concatenate the instr type with the opcode to form the complete opcode (e.g., LDW_FPM)
		strcpy(opcode_buf1,opcode1);
		strcat(opcode_buf1,"_");
		strcat(opcode_buf1,instr_type1);
		//ATTENTION: remove the last character from the opcode
		opcode_buf1[strlen(opcode_buf1)-1]='\0';
	}
	else
		strcpy(opcode_buf1,opcode1);

	if(instr_type2!=NULL){
		//concatenate the instr type with the opcode to form the complete opcode (e.g., LDW_FPM)
		strcpy(opcode_buf2,opcode2);
		strcat(opcode_buf2,"_");
		strcat(opcode_buf2,instr_type2);
		//ATTENTION: remove the last character from the opcode
		opcode_buf2[strlen(opcode_buf2)-1]='\0';
	}
	else
		strcpy(opcode_buf2,opcode2);

	//first create the expr_instruction from instruction
	for(i=instruction1;*i!=NULL;){
		expr_instruction1[find_field_ident(get_field_map(),*i++)]=get_ident(*i++);
	}

	//first create the expr_instruction from instruction
	for(i=instruction2;*i!=NULL;){
		expr_instruction2[find_field_ident(get_field_map(),*i++)]=get_ident(*i++);
	}

	//compute the latencies due to feedback paths between the two operations
	return exists_queue_between(opcode_buf1,&expr_instruction1,format1,computed1,
		opcode_buf2,&expr_instruction2,format2,computed2,latency);
}



//returns the capacity of the unit
__declspec( dllexport ) int get_capacity(char * unit){
	NodePtr node,capacity;
	int r;

	//get the node corresponding to this unit
	node=get_node(get_ident(unit));

	//find the capacity subnode of node
	capacity = node->Child(nke_capacity);

	//default is 1
	if(capacity==NULL) return 1;

	//find the capacity
	r=atoi(capacity->Name());

	return r;
}

//returns the ident of the unit
__declspec( dllexport ) int get_unit_nr(char * unit){
	int r;

	//get the ident corresponding to this unit
	r=get_ident(unit);

	return r;
}


//returns the name of the unit
__declspec( dllexport ) char * get_unit_name(int unit){
	char * r;

	//get the ident corresponding to this unit
	r=get_name(unit);

	return r;
}
