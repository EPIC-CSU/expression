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
#ifndef __UNIX__
	#include "crtdbg.h"
#else
	#include <stdio.h>
	#define __declspec(dllexport) 
#endif

#include <ctype.h>


//stores the size of the database
static int dbsize=0;

void delete_res_tab(ResTab *rt);

struct CompleteInstrType{
	char *opcode;
	std::map<int,int> *fields;
	ResTab *rt;
};

//hashing function for the opcode
long hash(char *opcode){
	char *p;
	long r=0,i=1;
	for(p=opcode;(*p)!=NULL;p++,i*=10){	// changed by Sumit on 
		// oct 15, 99, from *p!=NULL, to p!=NULL
		//changed back by Gabi: needs to stop on a NULL character, not an NULL pointer
		if(isalpha(*p)){
			if(isupper(*p))
				r+=(*p-'A')*i;
			else 
				r+=(*p-'a')*i;
		}
		else if(isdigit(*p)){
			r+=(*p-'0')*i;
		}
		else if(*p=='_'){
			r+=1*i;
		}
		else
			_ASSERT(0); //stopped here

	}
	return r;
}


//the reservation table data-base for all the instructions
//in the processor
//std::map<CompleteInstrType*,ResTab *> rt_data_base;
std::map<long,std::list<CompleteInstrType*>*> rt_data_base;

//adds an rt to the database
//opcode,instruction: input, the instruction
//rt: input, the rt for this instruction
void add_rt_to_db(char *opcode,std::map<int,int> *instruction,ResTab *rt){
	CompleteInstrType *instr;
	long h;
	//make a fresh copy of instruction
	instr=new CompleteInstrType();
	instr->opcode=opcode;
	instr->fields=new std::map<int,int>(*instruction);
	instr->rt=rt;
	h=hash(opcode);
	//store rt into db
	if(rt_data_base.find(h)==rt_data_base.end())
		rt_data_base[h]=new std::list<CompleteInstrType*>();
	rt_data_base[h]->push_front(instr);
	dbsize++;
}

//returns 1 if all the fields are equal
int equal_fields(std::map<int,int> *fields1,std::map<int,int> *fields2){
	std::map<int,int>::iterator i;

	for(i=fields1->begin();i!=fields1->end();i++){
		if((*fields1)[(*i).first]!=(*fields2)[(*i).first])
			return 0;
	}
	for(i=fields2->begin();i!=fields2->end();i++){
		if((*fields1)[(*i).first]!=(*fields2)[(*i).first])
			return 0;
	}
	return 1;
}


//returns 1 if the two instructions are equal
int equal_instructions(char *opcode1,std::map<int,int> *fields1,
					   char *opcode2,std::map<int,int> *fields2){
	if(strcmp(opcode1,opcode2)!=0) return 0;
	return equal_fields(fields1,fields2);
}


//find the reservation table for an instruction in the data base
ResTab *find_rt_in_db(char *opcode,std::map<int,int> *instruction){
	CompleteInstrType* instr;
	std::list<CompleteInstrType*>::iterator i;
	long h;

	h=hash(opcode);

	if(rt_data_base.find(h)==rt_data_base.end())
		return NULL;

	for(i=rt_data_base[h]->begin();i!=rt_data_base[h]->end();i++){
		if(equal_instructions(opcode,instruction,(*i)->opcode,(*i)->fields)){
			instr=*i;
			//first move this guy to the beginning of the db
			//rt_data_base.erase(i);
			//rt_data_base.push_front(instr);
			//then return it
			return instr->rt;
		}
	}
	return NULL;
}

//delete the reservation table data base
__declspec( dllexport ) void delete_db(void){
	CompleteInstrType* instr;
	std::list<CompleteInstrType*>::iterator i;
	std::map<long,std::list<CompleteInstrType*>*>::iterator j;
	long h;

	//for each opcode entry in the database
	for(j=rt_data_base.begin();j!=rt_data_base.end();j++){
		//h is the opcode hash
		h=(*j).first;
		//for each rt for this opcode
		for(i=rt_data_base[h]->begin();i!=rt_data_base[h]->end();){
			//delete this CompleteInstrType (*i);
			instr=*i;
			i++;
			//delete instr->opcode;
			delete instr->fields;
			delete_res_tab(instr->rt);
			delete instr;
		}
	}
}

__declspec( dllexport ) void print_db_size(void){
	FILE *f;
	printf("\nDatabase size: %d RTs\n", dbsize);
	//write to the file 
	f=fopen("tbztime.txt","a");
	fprintf(f,"& %d ", dbsize);
	//close file
	fclose(f);
}

