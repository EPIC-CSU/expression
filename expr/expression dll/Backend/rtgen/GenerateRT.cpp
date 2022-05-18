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

#ifndef __UNIX__
	#include "crtdbg.h"
#else
	#include <stdio.h>
#endif

#include <time.h>
//#include <process.h>

#ifndef __UNIX__
#include <process.h>
#else
#define __declspec(dllexport)
#define __stdcall
#define _ASSERT assert
#include <stdio.h>
#endif

#include "symtabs.h"

//the max length of the pipeline
const int MAX_PIPELENGTH=40;
__declspec (dllexport) int PIPELENGTH;

//set to 1 if you want debug prints for rtgen
int debugGRT=0;

NodePtr imm_node = new Node(nke_imm);

//prototypes
void gen_pipeline_paths(NodePtr node,std::list<ResTab*> *global_pipeline_paths);
void decorate_pipeline_paths(std::list<ResTab*> *gpp,std::list<ResTab*> *dgpp,std::list<int>* format,int format_nr);
void compose_transfers(NodePtr node,std::list<NodeList*> *complete_transfers);
void append_dt_segments(std::list<ResTab*> *gpp,std::list<ResTab*> *grt,
						std::list<NodeList*>*complete_segments,std::list<int> *format);
void update_resource_ident(NodePtr node);
void test_rt(std::map<int,std::list<ResTab*>*> *global_traces);
void print_segment(NodeList*segment);
void print_segments(std::list<NodeList*>*segments);
int get_ident(NodePtr node);

int is_unit_type(NodePtr node);
int is_port_type(NodePtr node);
int is_connection_type(NodePtr node);
int is_storage_type(NodePtr node);

int any_def_is_unit_type(NodePtr node);
int any_def_is_port_type(NodePtr node);
int any_def_is_connection_type(NodePtr node);
int any_def_is_storage_type(NodePtr node);

void read_field_map(void);
void read_instr_formats(void);
std::map<int,std::list<int>*> * get_instr_formats(void);
void print_format(std::list<int> *format);
void gen_instrs(void);

extern int calc_pipelength(std::list<ResTab*> *global_traces);

//global traces variable
//for each format (identified by the int), contains the list of traces
std::map<int,std::list<ResTab*>*> global_traces;


//copy src res tab into dst
//assumes that the destination has been newed already (including each stage's list)
void copy_res_tab(ResTab *src,ResTab *dst){
	int i;
	std::list<NodePtr>::iterator j;
	_ASSERT(src);
	_ASSERT(dst);
	for(i=0;i<MAX_PIPELENGTH;i++){
		_ASSERT((*src)[i]);
		_ASSERT((*dst)[i]);
		for(j=(*src)[i]->begin();j!=(*src)[i]->end();j++){
			_ASSERT(*j);
			(*dst)[i]->push_back(*j);
		}
	}
}

//frees the memory occupied by the reservation table
void delete_res_tab(ResTab *rt){
	int i;
	_ASSERT(rt);
	for(i=0;i<MAX_PIPELENGTH;i++){
		_ASSERT((*rt)[i]);
		(*rt)[i]->clear();
		delete (*rt)[i];
	}
	delete rt;
	//rt=NULL;
}

//clears the lists for each pipeline stage in the reservation table rt
void clear_res_tab(ResTab *rt){
	int i;
	_ASSERT(rt);
	for(i=0;i<MAX_PIPELENGTH;i++){
		_ASSERT((*rt)[i]);
		(*rt)[i]->clear();
	}
}

clock_t start1,stop1;
/*
#include <windows.h>
#include <iostream.h>
#include <process.h>

typedef struct
{
 HANDLE handle;
 FILETIME creationTime;
 FILETIME exitTime;
 FILETIME kernelTime;
 FILETIME userTime;
}
 THREADINFO;
static THREADINFO *threadI;
double startu, stopu,startk,stopk;

//sets the thread info from the pcProDll
__declspec( dllexport ) void setTreadInfo(THREADINFO *t){
	threadI=t;
}
*/

void start_time(void){
	//start counting time
	start1 = clock();
	/*

	GetThreadTimes(threadI->handle,
		&threadI->creationTime,
		&threadI->exitTime,
		&threadI->kernelTime,
		&threadI->userTime); 
	
	//if compile time is > 429.4967296 seconds must also use the threadI->*.dwHighDateTime
	startu=(double)threadI->userTime.dwLowDateTime/10000000.0;
	startk=(double)threadI->kernelTime.dwLowDateTime/10000000.0;
	*/
}

void stop_time(char *message){
	double  duration;
	//stop counting clock
	stop1=clock();
	duration = (double)(stop1 - start1) / CLOCKS_PER_SEC;
	printf( "\n%s: %2.2f seconds\n", message,duration );

	/*
	GetThreadTimes(threadI->handle,
		&threadI->creationTime,
		&threadI->exitTime,
		&threadI->kernelTime,
		&threadI->userTime); 
	
	//if compile time is > 429.4967296 seconds must also use the threadI->*.dwHighDateTime
	stopu=(double)threadI->userTime.dwLowDateTime/10000000.0;
	stopk=(double)threadI->kernelTime.dwLowDateTime/10000000.0;
	double userTime=stopu-startu;
	double kernelTime=stopk-startk;
	
	cerr << " Time: " << userTime + kernelTime << " secs (user: " << userTime << " secs) (kernel: " << kernelTime << " secs)" << endl ;
	*/
}



void print_node(NodePtr node){
	//printf("%s(%s) ",node->Name(),node_kind_names[node->Kind()]);
	//printf("%s ",node->Name());
	printf("%s",node->Name());
	if(any_def_is_unit_type(node)) printf("(U) ");
	if(any_def_is_port_type(node->Symbol()->GetFirstDef())) printf("(P) ");
	if(any_def_is_connection_type(node->Symbol()->GetFirstDef())) printf("(C) ");
	if(any_def_is_storage_type(node)) printf("(S) ");
	//printf("%d ",get_ident(node));
}

void print_res_tab(ResTab *rt){
	int i;
	std::list<NodePtr>::iterator j;
	printf("Reservation Table:\n");
	// for(i=0;i<MAX_PIPELENGTH;i++){
	for(i=0;i<PIPELENGTH;i++){
		printf("%d: ",i);
		for(j=(*rt)[i]->begin();j!=(*rt)[i]->end();j++){
			print_node(*j);
		}
		printf("\n");
	}
}

void print_res_tabs(std::list<ResTab*> *restabs){
	std::list<ResTab*>::iterator i;
	int j;
	for(i=restabs->begin(),j=0;i!=restabs->end();i++,j++){
		printf("%d: ",j);
		print_res_tab(*i);
	}
}


#define max(a, b) ((a < b) ? b : a)

extern Symtab st; //the only symbol table
extern int crt;
extern std::map<SymbolPtr,int> resource_ident_map;

//This function generates the traces
//input: node is the whole IR
void generate_traces(NodePtr node){
	//Stores the pipeline paths
	std::list<ResTab*> global_pipeline_paths;
	std::map<int,std::list<ResTab*>*> global_decorated_pipeline_paths;
	std::list<NodeList*> complete_transfers;
	std::map<int,std::list<int>*> * instr_formats;
	std::map<int,std::list<int>*>::iterator l;
	int i;
	long decorated_pp_num=0,traces_num=0;
	
	//Step 0
	//create a map from the symbols to ints, representing the identifiers of each resource. 
	update_resource_ident(node);
	imm_node->Name("IMM");
	imm_node->Symbol(st.Declare("IMM",imm_node));
	resource_ident_map[imm_node->Symbol()]=crt++;

	//clear pipeline paths (for reentrance). 
	global_pipeline_paths.clear();
	
	//Step 1
	//generate all pipeline paths
	gen_pipeline_paths(node,&global_pipeline_paths);

	if(debugGRT) printf("\n/////////////////////////////// Pipeline paths: %d /////////////////////////////////\n",global_pipeline_paths.size());

	if(debugGRT) print_res_tabs(&global_pipeline_paths);

	read_field_map();
	read_instr_formats();


	start_time();

	instr_formats=get_instr_formats();


	//Step 3
	//Compose end to end transfers from segements
	//should not be in the loop
	compose_transfers(node,&complete_transfers);
	
	if(debugGRT) printf("\n///////////////////////////////// Complete transfer segments: %d ///////////////////////////////////\n",complete_transfers.size());
	if(debugGRT) print_segments(&complete_transfers);
	
	PIPELENGTH = 0;
	//for each instruction format
	for(l=instr_formats->begin(),i=0;l!=instr_formats->end();l++,i++){

		if(debugGRT) printf("\n//////////////////////////////// Format: ///////////////////////////////////////////////////////\n");
		if(debugGRT) print_format((*l).second);

		global_decorated_pipeline_paths[i]=new std::list<ResTab*>();

		//Step 2
		//decorate pipeline paths with ports
		decorate_pipeline_paths(&global_pipeline_paths,global_decorated_pipeline_paths[i],(*l).second,i);

		if(debugGRT) printf("\nFormat %d\n",i);
		if(debugGRT) printf("Decorated pipeline paths: %d\n",global_decorated_pipeline_paths[i]->size());
		
		if(debugGRT) printf("\n/////////////////////////////// Decorated Pipeline paths: %d /////////////////////////////////\n",global_decorated_pipeline_paths[i]->size());
		if(debugGRT) print_res_tabs(global_decorated_pipeline_paths[i]);
		

		global_traces[i]=new std::list<ResTab*>();

		//Step 4
		//Append DT segments to the pipeline paths (while duplicating the pipeline paths if necessary)
		//Store the resulting rts into global_traces
		append_dt_segments(global_decorated_pipeline_paths[i],global_traces[i],&complete_transfers,(*l).second);

		printf("Traces: %d\n",global_traces[i]->size());
		
		PIPELENGTH = max(PIPELENGTH, calc_pipelength(global_traces[i]));

		if(debugGRT) printf("\n/////////////////////////////// Traces: %d /////////////////////////////////\n",global_traces[i]->size());
		
		if(debugGRT) print_res_tabs(global_traces[i]);

		//update the number of decorated pipelines and traces (statistics)
		decorated_pp_num+=global_decorated_pipeline_paths[i]->size();
		traces_num+=global_traces[i]->size();
	}

	//stop counting time
	stop_time("Time to compute traces");

	if (PIPELENGTH > MAX_PIPELENGTH) PIPELENGTH = MAX_PIPELENGTH;

	//print out statistical results

	printf("\nPipeline paths: %d",global_pipeline_paths.size());
	printf("\nComplete transfer segments: %d",complete_transfers.size());
	printf("\nDecorated pipeline paths: %d",decorated_pp_num);
	printf("\nTraces: %d",traces_num);
}


//This function generates the reservation tables
//input: node is the whole IR
void generateRT(NodePtr node){

	//first generate the traces
	generate_traces(node);

	//Given a fully qualified operation, traverse the traces (rts) computed in Step 4 
	//and generate a reservation table for that operation

	//test_rt(&global_traces);

	//start counting time
	start_time();

	//generate all the instructions
	gen_instrs();

	//stop time computation
	stop_time("Time to compute RTs");
}