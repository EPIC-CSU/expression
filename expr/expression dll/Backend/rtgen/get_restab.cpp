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
#else 
	#include <stdio.h>
#endif

//prototypes
int get_ident(char *name);
int get_ident(NodePtr node);
void print_res_tab(ResTab *rt);
std::map<char *,int> *get_field_map(void);
char *find_field_name(std::map<char *,int> *global_field_map,int val);
void read_field_map(void);
int contains_arg_in_object(int arg,NodePtr object);
int find_field_ident(std::map<char *,int> *global_field_map,char *name);
ResTab * get_rt(char *opcode,std::map<int,int> *instruction,int format,int &computed,int dbEnabled=0);
int has_attached_argument(NodePtr port);
NodePtr get_holder_for_port(NodePtr port);

int is_unit_type(NodePtr node);
int is_port_type(NodePtr node);
int is_connection_type(NodePtr node);
int is_storage_type(NodePtr node);

int any_def_is_unit_type(NodePtr node);
int any_def_is_port_type(NodePtr node);
int any_def_is_connection_type(NodePtr node);
int any_def_is_storage_type(NodePtr node);

char *get_name(int ident);
void print_node(NodePtr node);
int find_format(std::map<int,int>*instruction);
int contains_in_list(NodeList* elem_list,NodePtr elem);
struct PartialOrderType{
	int field1,field2;
	PartialOrderType(int f1,int f2):field1(f1),field2(f2){}
};
std::map<int,std::list<PartialOrderType*>*> *get_partial_orders(void);
NodePtr get_node(int ident);
void print_instruction(std::map<int,int> *instruction);
char *find_field_name(std::map<char *,int> *global_field_map,int val);
ResTab *find_rt_in_db(char *opcode,std::map<int,int> *instruction);
void add_rt_to_db(char *opcode,std::map<int,int> *instruction,ResTab *rt);
void copy_res_tab(ResTab *src,ResTab *dst);
extern int dbEnabled;
extern std::map<ResTab*,std::map<int,int> *> supported_instruction_map;
NodePtr find_timing(NodePtr obj);
void init_rt(ResTab *rt);
extern std::map<ResTab*,std::map<int,std::list<NodeList*>*>*> *rt_to_config_map;
NodeList::iterator * find_in_list(NodeList* elem_list,NodePtr elem);
NodePtr find_node_in_obj(NodePtr object,NodeKindEnum kind);
int supports_opcode(NodePtr node,char *opcode);
int supports_opcode_enq(NodePtr node,char *opcode);
int supports_opcode_deq(NodePtr node,char *opcode);


//global traces variable
extern std::map<int,std::list<ResTab*>*> global_traces;

int debugRT=0;

int printRT=0;

int debugRWTime=0;

int debugDeepRT=0;


void print_instruction(char *opcode,std::map<int,int> *instruction){
	std::map<int,int>::iterator i;
	printf("\nInstr: %s ",opcode);
	for(i=instruction->begin();i!=instruction->end();i++){
		printf("%s=%s ",find_field_name(get_field_map(),(*i).first),get_name((*i).second));
	}
}

int isNOP(char *opcode){
	if(strcmp(opcode,"NOP")==0) return 1;
	if(strcmp(opcode,"nop")==0) return 1;
	if(strcmp(opcode,"X_NOP")==0) return 1;
	return 0;
}



//////////////////////////////////////////////////////////////////////
//input field, rt. Field contains an object from the architecture (resource)
//output 1 if rt contains field 0 otherwise
//if field is -1, means that it is a non-existent field (e.g., the source 2 in a MV)
int contains_unit(int field,ResTab *rt){
	int i;
	NodeList::iterator j;

	//for the invalid field (-1), consider it is contained, since the field doesn't exist
	if(field==-1) return 1;

	for(i=0;i<PIPELENGTH;i++){
		for(j=(*rt)[i]->begin();j!=(*rt)[i]->end();j++){
			if(field==get_ident(*j)) return 1;
		}
	}
	return 0;
}
////////////////////////////////////////////////////////////////////////


//Returns the pipeline stage in which the argument specified by argument
//is read. Argument may be:
//const _DEST_ = 1;
//const _SOURCE_1_ = 2;
//const _SOURCE_2_ = 3;
//const _MEM_DEST_ = 4;
//const _MEM_SRC_  = 5;

int get_read_write_time(char *opcode,std::map<int,int>*instruction,int format,int argument,int &computed){
	ResTab *rt;
	NodePtr port=NULL;
	int i,stage=-1;
	NodeList::iterator j,k;

	//rt=get_rt(opcode,instruction,find_format(instruction));
	//1 means database enabled
	rt=get_rt(opcode,instruction,format,computed,dbEnabled);

	_ASSERT(rt);

	if(debugRWTime){
		print_instruction(opcode,instruction);
		print_res_tab(rt);
	}

	if(isNOP(opcode)){
		//for NOPs return both read and write time as 0
		return 0;
	}

	//for each stage of the pipeline i
	for(i=0;port==NULL && i<PIPELENGTH;i++){
		//for each resource in this pipeline stage 
		for(j=(*rt)[i]->begin();port==NULL && j!=(*rt)[i]->end();j++){
			//if this reosource is a port
			if(any_def_is_port_type(*j)){
				//if this port contains the argument argument
				if(contains_arg_in_object(argument,*j)){
					//remember this port, and pipeline stage
					port=*j;
					stage=i;
				}
			}
		}
	}

	//port should be set by now
	//port contains the port corresponding to the argument
	if(port==NULL || stage<=-1){
		print_instruction(opcode,instruction);
		print_res_tab(rt);
	}
	_ASSERT(port);
	_ASSERT(stage>-1);

	return stage;
}



/////////////////////////////////////////////////////////////////////////////////////////



//Returns the list of valid latencies due to feedback paths between the two operations

void get_feedback_latencies(char *opcode1,std::map<int,int>*instruction1,int format1,int &computed1,
						   char *opcode2,std::map<int,int>*instruction2,int format2,int &computed2,
						   int *latencies){
	ResTab *rt1,*rt2;
	NodePtr fb1=NULL,fb2=NULL;
	int i,m,diff,curr_lat=0;
	NodeList::iterator j,k,l,n;
	NodeList *fbs1,*fbs2;
	char *fbname1,*fbname2;
	char *unitname;

	//rt=get_rt(opcode,instruction,find_format(instruction));
	//1 means database enabled
	rt1=get_rt(opcode1,instruction1,format1,computed1,dbEnabled);
	rt2=get_rt(opcode2,instruction2,format2,computed2,dbEnabled);

	_ASSERT(rt1);
	_ASSERT(rt2);

	if(debugRWTime){
		print_instruction(opcode1,instruction1);
		print_res_tab(rt1);
		print_instruction(opcode2,instruction2);
		print_res_tab(rt2);
	}

	if(isNOP(opcode1)){
		//for NOPs return both read and write time as 0
		return;
	}

	if(isNOP(opcode2)){
		//for NOPs return both read and write time as 0
		return;
	}

	//for each stage i of the pipeline 
	for(i=0;i<PIPELENGTH;i++){
		//for each resource in this pipeline stage 
		for(j=(*rt1)[i]->begin();j!=(*rt1)[i]->end();j++){
			//if this reosource is a unit
			if(any_def_is_unit_type(*j)){
				unitname=(*j)->Name();
				fb1=find_node_in_obj((*j),nke_feedback_deq);
				if(fb1==NULL) continue;
				fbs1=fb1->Children();
				if(fbs1==NULL) continue;
				//for all the feedback paths from this unit
				for(k=fbs1->begin();k!=fbs1->end();k++){
					fbname1=(*k)->Name();
					//find the stage (if any) in which this feedback path shows up in rt2 (different then stage i)
					//for each stage of the pipeline 
					for(m=0;m<PIPELENGTH;m++){
						//for each resource in this pipeline stage 
						for(n=(*rt2)[m]->begin();n!=(*rt2)[m]->end();n++){
							//if this reosource is a unit
							if(is_unit_type(*n)){
								fb2=find_node_in_obj((*n),nke_feedback_enq);
								if(fb2==NULL) continue;
								fbs2=fb2->Children();
								if(fbs2==NULL) continue;
								//for all the feedback paths from this unit
								for(l=fbs2->begin();l!=fbs2->end();l++){
									fbname2=(*l)->Name();
									if(i!=m && strcmp(fbname1,fbname2)==0){
										//if the two feedbacks have the same name and are in different stages in op1 and op2
										//add the difference between the two stages to the list of latencies
										diff=abs(i-m);
										//_ASSERT(diff);
										//add diff to list of latencies
										latencies[curr_lat++]=diff;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	latencies[curr_lat]=-1;
}



//Returns the list of valid latencies due to feedback paths between the two operations

int exists_queue_between(char *opcode1,std::map<int,int>*instruction1,int format1,int &computed1,
						   char *opcode2,std::map<int,int>*instruction2,int format2,int &computed2,
						   int latency){
	ResTab *rt1,*rt2;
	NodePtr fb1=NULL,fb2=NULL;
	int i,m,diff;
	NodeList::iterator j,k,l,n;
	NodeList *fbs1,*fbs2;
	char *fbname1,*fbname2;
	char *unitname;

	//rt=get_rt(opcode,instruction,find_format(instruction));
	//1 means database enabled
	rt1=get_rt(opcode1,instruction1,format1,computed1,dbEnabled);
	rt2=get_rt(opcode2,instruction2,format2,computed2,dbEnabled);

	_ASSERT(rt1);
	_ASSERT(rt2);

	if(debugRWTime){
		print_instruction(opcode1,instruction1);
		print_res_tab(rt1);
		print_instruction(opcode2,instruction2);
		print_res_tab(rt2);
	}

	if(isNOP(opcode1)){
		//for NOPs return both read and write time as 0
		return 0;
	}

	if(isNOP(opcode2)){
		//for NOPs return both read and write time as 0
		return 0;
	}

	//for each stage i of the pipeline of rt1
	for(i=0;i<PIPELENGTH;i++){
		//for each resource in this pipeline stage 
		for(j=(*rt1)[i]->begin();j!=(*rt1)[i]->end();j++){
			//if this reosource is a unit
			if(any_def_is_unit_type(*j)){
				unitname=(*j)->Name();
				fb1=find_node_in_obj((*j),nke_queues_enq);
				if(fb1==NULL) continue;
				fbs1=fb1->Children();
				if(fbs1==NULL) continue;
				//for all the queues deq from this unit
				for(k=fbs1->begin();k!=fbs1->end();k++){
					fbname1=(*k)->Name();
					//if this queue supports opcode1
					if(supports_opcode_enq((*k),opcode1)){
						//find the stage (if any) in which this feedback path shows up in rt2 (different then stage i)
						//for each stage of the pipeline of rt2
						for(m=0;m<PIPELENGTH;m++){
							//for each resource in this pipeline stage 
							for(n=(*rt2)[m]->begin();n!=(*rt2)[m]->end();n++){
								//if this reosource is a unit
								if(is_unit_type(*n)){
									fb2=find_node_in_obj((*n),nke_queues_deq);
									if(fb2==NULL) continue;
									fbs2=fb2->Children();
									if(fbs2==NULL) continue;
									//for all the queues enq from this unit
									for(l=fbs2->begin();l!=fbs2->end();l++){
										fbname2=(*l)->Name();
										if(supports_opcode_deq((*l),opcode2)){
											if(strcmp(fbname1,fbname2)==0){
												//if the two feedbacks have the same name and are in different stages in op1 and op2
												//add the difference between the two stages to the list of latencies
												diff=abs(i-m);
												//_ASSERT(diff);
												//add diff to list of latencies
												//latency=diff;
												//latency should be always 0, independent of the differenece
												latency=0;
												return 1;
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	return 0;
}


/////////////////////////////////////////////////////////////////////////////////////////

//Returns 1 if the unit node supports opcode
//node is a unit nke_compinst
int supports_opcode(NodePtr node,char *opcode){
	NodePtr def,opcode_child,opcode_def;
	NodeList::iterator i;

	_ASSERT(node);
	_ASSERT(node->Symbol());
	_ASSERT(opcode);
	def=node->Symbol()->GetFirstDef();
	_ASSERT(def);
	opcode_child=def->FindSubNode(nke_opcodes);
	if(opcode_child==NULL || opcode_child->Children()->size()==0){
		//it has no opcode declaration, or it is empty: default is ALL opcodes
		return 1;
	}
	//for each child of the opcode declaration for the unit given in node in the architecture section
	for(i=opcode_child->Children()->begin();i!=opcode_child->Children()->end();i++){
		//*i is an opcode declaration in the node
		_ASSERT(*i);
		//opcode_def=(*i)->Symbol()->GetFirstDef();
		opcode_def=(*i)->Symbol()->GetDefList()->front();
		_ASSERT(opcode_def);
		//opcode_def is the opcode definition in the opccode section
		switch(opcode_def->Kind()){
		case nke_op_group:
			//if it is an opcode group
			if(opcode_def->FindSubNode(opcode)!=NULL)
				return 1;
		case nke_opcode:
			//if it is a single opcode
			if(strcmp(opcode_def->Name(),opcode)==0)
				return 1;
		}
	}
	//none of the opcode declarations for this node were equal to opcode
	return 0;
}


//Returns 1 if the unit node supports opcode
//node is a unit nke_compinst
int supports_opcode_enq(NodePtr node,char *opcode){
	NodePtr def,opcode_child,opcode_def;
	NodeList::iterator i;

	_ASSERT(node);
	_ASSERT(node->Symbol());
	_ASSERT(opcode);
	def=node->Symbol()->GetFirstDef();
	_ASSERT(def);
	opcode_child=def->FindSubNode(nke_opcodes_enq);
	if(opcode_child==NULL || opcode_child->Children()->size()==0){
		//it has no opcode declaration, or it is empty: default is ALL opcodes
		return 1;
	}
	//for each child of the opcode declaration for the unit given in node in the architecture section
	for(i=opcode_child->Children()->begin();i!=opcode_child->Children()->end();i++){
		//*i is an opcode declaration in the node
		_ASSERT(*i);
		//opcode_def=(*i)->Symbol()->GetFirstDef();
		opcode_def=(*i)->Symbol()->GetDefList()->front();
		_ASSERT(opcode_def);
		//opcode_def is the opcode definition in the opccode section
		switch(opcode_def->Kind()){
		case nke_op_group:
			//if it is an opcode group
			if(opcode_def->FindSubNode(opcode)!=NULL)
				return 1;
		case nke_opcode:
			//if it is a single opcode
			if(strcmp(opcode_def->Name(),opcode)==0)
				return 1;
		}
	}
	//none of the opcode declarations for this node were equal to opcode
	return 0;
}

//Returns 1 if the unit node supports opcode
//node is a unit nke_compinst
int supports_opcode_deq(NodePtr node,char *opcode){
	NodePtr def,opcode_child,opcode_def;
	NodeList::iterator i;

	_ASSERT(node);
	_ASSERT(node->Symbol());
	_ASSERT(opcode);
	def=node->Symbol()->GetFirstDef();
	_ASSERT(def);
	opcode_child=def->FindSubNode(nke_opcodes_deq);
	if(opcode_child==NULL || opcode_child->Children()->size()==0){
		//it has no opcode declaration, or it is empty: default is ALL opcodes
		return 1;
	}
	//for each child of the opcode declaration for the unit given in node in the architecture section
	for(i=opcode_child->Children()->begin();i!=opcode_child->Children()->end();i++){
		//*i is an opcode declaration in the node
		_ASSERT(*i);
		//opcode_def=(*i)->Symbol()->GetFirstDef();
		opcode_def=(*i)->Symbol()->GetDefList()->front();
		_ASSERT(opcode_def);
		//opcode_def is the opcode definition in the opccode section
		switch(opcode_def->Kind()){
		case nke_op_group:
			//if it is an opcode group
			if(opcode_def->FindSubNode(opcode)!=NULL)
				return 1;
		case nke_opcode:
			//if it is a single opcode
			if(strcmp(opcode_def->Name(),opcode)==0)
				return 1;
		}
	}
	//none of the opcode declarations for this node were equal to opcode
	return 0;
}


//returns 1 if all the units in the trace support the opcode
int all_units_support_opcode(ResTab *trace,char * opcode){
	int i;
	NodeList::iterator j;

	_ASSERT(trace);
	_ASSERT(opcode);

	for(i=0;i<PIPELENGTH;i++){
		for(j=(*trace)[i]->begin();j!=(*trace)[i]->end();j++){
			if(!supports_opcode(*j,opcode)){
				if(debugRT) printf("\n%s doesn't support the opcode %s\n",(*j)->Name(),opcode);
				return 0;
			}
		}
	}
	return 1;
}

extern std::map<SymbolPtr,int> resource_ident_map;
extern NodePtr imm_node;

//returns 1 if the trace covers all the fields in instruction
int covers_all_fields(ResTab *trace,std::map<int,int> *instruction)
{
	int i,object,found,field;
	NodeList::iterator j;
	std::map<int,int>::iterator k;
	NodeList history;
	std::map<int,int>::iterator l;


	_ASSERT(trace);
	_ASSERT(instruction);

	if(debugRT)
	{
		printf("\nConvering:\n");
		fflush(stdout);
	}

	if(debugRT)
	{
		printf("##################\nInstruction :\n");
		for(k=instruction->begin();k!=instruction->end();k++)
		{
			object=(*k).second;
			field=(*k).first;
			printf("%s : %s\n", find_field_name(get_field_map(),field), get_name(object)); 
		}

		printf("Trace :\n");
		for(l=supported_instruction_map[trace]->begin();l!=supported_instruction_map[trace]->end();l++)
		{
			int trace_object=(*l).second;
			int trace_field=(*l).first;
			printf("%s : %s\n", find_field_name(get_field_map(),trace_field), get_name(trace_object)); 
		}
		printf("#######################\n");
	}

	//for each field in the instruction
	for(k=instruction->begin();k!=instruction->end();k++)
	{
		object=(*k).second;
		field=(*k).first;
		found=0;

		if (object == resource_ident_map[imm_node->Symbol()])
			continue;

		if(debugRT){
			printf("Covering: %s=%s\n",find_field_name(get_field_map(),field),get_name(object));
		}

		//see if this field is covered in the supported_instruction_map for this trace
		//for each field in the supported_instruction_map
		for(l=supported_instruction_map[trace]->begin();l!=supported_instruction_map[trace]->end();l++)
		{
			int trace_object=(*l).second;
			int trace_field=(*l).first;
			
			if((trace_field == field) && (trace_object == object))
			{
				found=1;
				break;
			}
		}

		//see if this field is covered by some resource in the trace
		//for each pipeline stage
		for(i=0;!found && i<PIPELENGTH;i++)
		{
			//for each object in this pipeline stage in this trace
			for(j=(*trace)[i]->begin();!found && j!=(*trace)[i]->end();j++)
			{
				if(!contains_in_list(&history,*j))
				{
					//for ports, units, storage and connections,
					//verify if the object and field are satified by the object *j
					if(any_def_is_port_type(*j))
					{
						if(debugDeepRT)
						{
							printf("%s, ",get_name(get_ident(*j)));
						}
						//if the object *j contains this field as attached argument
						if(contains_arg_in_object(field,*j))
						{
							if(debugDeepRT)
							{
								printf("(contains arg) ");
							}
							//if the object from the instruction is equal to the thing the port is attached to
							if(get_ident(get_holder_for_port(*j))==object)
							{
								if(debugDeepRT)
								{
									printf("(equal to holder) ");
								}
								found=1;
							}
						}
						//otherwise, forget it. Ports should have an attached argument
					}
					else if(any_def_is_unit_type(*j))
					{
						//if the object is equal to the current object from the trace
						if(object==get_ident(*j))
						{
							if(!has_attached_argument(*j))
							{
								//if has no field attached (e.g., for _UNIT_)
								found=1;
							}
							else if(contains_arg_in_object(field,*j))
							{
								//if it contains that field (e.g., _SOURCE_1_)
								found=1;
							}
						}
					}
					else if(any_def_is_storage_type(*j))
					{
						//don't do nothin
					}
					else if(is_connection_type(*j))
					{
						//don't do nothing for now. Connections should'nt be referenced from the instruction
					}
					else
						_ASSERT(0);
					if(found)
					{
						if(debugRT)
						{
							printf("Covered: %s=%s",find_field_name(get_field_map(),field),get_name(object));
							printf(" by ");
							print_node(*j);
							history.push_back(*j);
						}
					}
				}
			}
		}
		//if(found1==1 && found==0){
			//print_res_tab(trace);
			//_ASSERT(0);
		//}
		if(!found)
		{
			if(debugRT)
			{
				printf("\nNot covered: %s\n",find_field_name(get_field_map(),field));
			}
			return 0;
		}
		else
		{
			if(debugRT)
			{
				printf("\nCovered: %s\n",find_field_name(get_field_map(),field));
			}
		}
		//end for each field
	}

	if(debugRT) printf("\nSuccessfully covered\n");

	return 1;
}

//////////////////////////////////////////////////////////////////////////////////////////////

//returns 1 if there are no writes before reads in the pipeline for this reservation table rt
//i.e, the all the partial orders for this format are satisfied by this rt
int no_writes_before_reads(ResTab *rt,int format){
	int i,field1,field2;
	NodeList::iterator j;
	std::map<int,std::list<PartialOrderType*>*> *format_partial_orders;
	std::list<PartialOrderType*>::iterator k;
	int found=0;

	format_partial_orders=get_partial_orders();

	//for all the partial orders for this format
	for(k=(*format_partial_orders)[format]->begin();k!=(*format_partial_orders)[format]->end();k++){
		_ASSERT(*k);
		//field1 and field2 are the two fields in the partial order field1 -> field2
		field1=(*k)->field1;
		field2=(*k)->field2;

		found=0;
		for(i=0;i<PIPELENGTH;i++){
			for(j=(*rt)[i]->begin();j!=(*rt)[i]->end();j++){
				if(found){
					if(contains_arg_in_object(field1,*j)){
						if(debugRT){
							printf("\nWrites before reads (partial order broken) in RT: \n");
							print_res_tab(rt);
						}
						return 0;
					}
				}
				else if(contains_arg_in_object(field2,*j)){
					found=1;
					//break out of this for loop
					//it is ok if the two fields appear in the same stage
					//it is wrong only if field1 appears in a later stage than field2
					break;
				}
			}
		}
	}
	if(debugRT) printf("\nNo writes before reads\n");
	return 1;
}

//returns 1 if this node contains this opcode
int node_contains_opcode(NodePtr node,char *opcode){
	NodePtr def;
	NodeList::iterator i;

	if(node->Kind()==nke_integer) return 0;

	if(node->Kind()==nke_all) return 1;

	_ASSERT(node->Symbol());

	def=node->Symbol()->GetFirstDef();

	_ASSERT(def);

	if(def->Kind()==nke_opcode && strcmp(def->Name(),opcode)==0) return 1;
	
	if(def->Kind()==nke_op_group){
		for(i=def->Children()->begin();i!=def->Children()->end();i++){
			_ASSERT((*i)->Kind()==nke_opcode);
			if(node_contains_opcode(*i,opcode))
				return 1;
		}
	}
	return 0;
}

//returns 1 if this timing node (timing) contains an entry for this opcode
//and returns in time this value
int timing_contains_opcode(char *opcode,NodePtr timing,int &time){
	NodeList::iterator l;
	int found;

	time=1;
	found=0;
	//for each child of this timing node
	for(l=timing->Children()->begin();!found && l!=timing->Children()->end();l++){
		_ASSERT(*l);
		//if it contains this opcode
		if(node_contains_opcode(*l,opcode)){
			found=1;
			break;
		}
	}

	if(!found) return 0;
	
	for(l=timing->Children()->begin();l!=timing->Children()->end();l++){
		_ASSERT(*l);
		if((*l)->Kind()==nke_integer){
			//set the value of time
			time=atoi((*l)->Name());
			return 1;
		}
	}
	_ASSERT(0);
	return 0;
}

//finds the timing of an object for an opcode
int find_timing_for_opcode(NodePtr obj,char *opcode){
	NodePtr def;
	NodeList::iterator k;
	int found,timing;

	def=obj->Symbol()->GetFirstDef();
	found=0;
	timing=1;
	//for each child of the definition
	for(k=def->Children()->begin();!found && k!=def->Children()->end();k++){
		if((*k)->Kind()==nke_timing){
			//if this is a timing node
			if(timing_contains_opcode(opcode,*k,timing)){
				found=1;
			}
		}
	}
				
	return timing;
}

//returns the DT segment containing the resource res in config
NodeList *find_segment_containing(NodePtr res,std::map<int,std::list<NodeList*>*> *config){
	int i;
	std::list<NodeList*>::iterator j;
	NodeList::iterator k;
	NodeList *seg;

	//for each pipeline stage
	for(i=0;i<PIPELENGTH;i++){
		//for each segment in this pipeline stage
		for(j=(*config)[i]->begin();j!=(*config)[i]->end();j++){
			seg=*j;
			_ASSERT(seg);
			//for each resource in this segment
			for(k=seg->begin();k!=seg->end();k++){
				if((*k)->Symbol()==res->Symbol()){
					return seg;
				}
			}
		}
	}
	return NULL;
}


//inserts delays for the units and ports which have timing for this opcode
//rt: input, output, the RT to be modified
//opcode: input, the opcode of this instruction
ResTab *insert_delay_for_timing(ResTab *rt,char *opcode){
	int i;
	NodeList::iterator j;
	ResTab *rt1;
	int new_stage;
	int increment;
	int l;
	int timing;
	std::map<int,std::list<NodeList*>*> *config;
	NodeList *seg;
	NodeList::iterator k,temp_k;

	if (printRT)
	{ 
		printf("Input Reservation Table: \n");
		print_res_tab(rt); 
	}

	//create the new RT
	rt1=new ResTab();
	init_rt(rt1);

	_ASSERT(opcode);

	//first, delay only the multicycled units

	//for each pipeline stage
	for(i=0,new_stage=0;i<PIPELENGTH;i++,new_stage+=increment){
		//the next pipeline stage we add objects to depends on the multicycled units in this stage
		increment=1;
		//for each object in this pipeline stage
		for(j=(*rt)[i]->begin();j!=(*rt)[i]->end();j++){
			if(any_def_is_unit_type(*j)){
				//if this object is a unit, find its timing
				timing=find_timing_for_opcode(*j,opcode);
				
				_ASSERT(i+timing-1<PIPELENGTH);
				
				//for the following "timing" stages, add this resource to the RT as a multicycled resource
				for(l=0;l<timing;l++){
					//add this resource to the new RT
					(*rt1)[new_stage+l]->push_back(*j);
				}
				if(timing>increment) increment=timing;
			}
			else{
				//for all others, just add it where it was
				(*rt1)[new_stage]->push_back(*j);
			}
		}
	}

	//next, delay the ports, together with their complete DT segments

	//for each pipeline stage
	for(i=0;i<PIPELENGTH;i++){
		//for each object in this pipeline stage
		for(j=(*rt1)[i]->begin();j!=(*rt1)[i]->end();j++){
			if(any_def_is_port_type(*j)){
				//if it is a port
				//find its timing
				timing=find_timing_for_opcode(*j,opcode);
				if(timing>1){
					//if it has a timing greater than 1, 
					//move its whole DT segment <timing-1> stages later
					config=(*rt_to_config_map)[rt];
					seg=find_segment_containing(*j,config);
					//for each resource in this segment
					for(k=seg->begin();k!=seg->end();k++){
						temp_k=*find_in_list((*rt1)[i],*k);
						if(is_port_type(*k) || is_connection_type(*k) || is_storage_type(*k)){
							//if it is port or connection or storage
							//move it to the new pipeline stage
							(*rt1)[i]->erase(temp_k);
							(*rt1)[i+timing-1]->push_back(*k);
						}
					}
				}
			}
		}
	}

	if (printRT)
	{ 
		printf("Output Reservation Table: \n");
		print_res_tab(rt1); 
	}

	return rt1;
}



//Given an operation (opcode, unit, srces, dest)
//and the list of traces in global_traces,
//it creates the reservation table for that operation
//Input: opcode, unit, src.., dst,global_traces
//Return: res_tab
//input: dbEnabled: if this is one, it first tries to search in the RT data base (default is 0)
// dbEnabled = 1 precomputed db, =2 remember
// computed returns 1 if RT was actually computed, or 0 if it was retrieved from db
//ResTab * get_rt(char *opcode,char *unit,char *src1,char *src2,char *dst){
ResTab * get_rt(char *opcode,std::map<int,int> *instruction,int format,int &computed,int dbEnabled)
{
	std::list<ResTab*>::iterator i;
	ResTab *rt=NULL;
	int j;
	int count=0;

	computed=1;

	if(printRT)
	{
		printf("\nget_rt:");
		print_instruction(opcode,instruction);
		printf("Format: %d\n",format);
	}

	_ASSERT(&global_traces);
	_ASSERT(global_traces[format]);

	if(isNOP(opcode))
	{
		//if the instruction is a NOP, return an empty RT
		//NOPs should not consume any resources
		rt=new ResTab();
		init_rt(rt);
		return rt;
	}

	if(dbEnabled==1)
	{
		//Precomputed database
		//try to find the rt in the data base
		rt=find_rt_in_db(opcode,instruction);
		//_ASSERT(rt);
		if(rt!=NULL){
			if(printRT)
				print_res_tab(rt);

			computed=0;
			return rt;
		}
	}
	else if(dbEnabled==2)
	{
		//remember this rt in the database after computation
		//try to find the rt in the data base
		rt=find_rt_in_db(opcode,instruction);
		if(rt!=NULL){
			if(printRT)
				print_res_tab(rt);

			computed=0;
			return rt;
		}
	}

	//for all the traces for this format
	for(j=0,i=global_traces[format]->begin();i!=global_traces[format]->end();i++,j++)
	{
		if(debugRT)
		{
			printf("Trying to fit trace %d for format %d\n",j,format);
			printf("Trace %d:",j);
			print_res_tab((*i));
		}

		//the reservation table (trace) should contain all the resources referenced in the operation
		if(covers_all_fields(*i,instruction))
		{
			if(all_units_support_opcode(*i,opcode))
			{
				if(no_writes_before_reads(*i,format))
				{
					//All the objects in the trace have to support this opcode
					//create a new copy of this RT
					//rt=new ResTab();
					//copy_res_tab(*i,rt);
					//rt=*i;
					//create a new copy of this RT and
					//delay the units and ports which have timing for this opcode
					rt=insert_delay_for_timing(*i,opcode);
					count++;

					if(debugRT) print_res_tab(rt);

					//we want the first reservation table to be returned
					//break;
				}
			}
		}
	}

	if((dbEnabled==1 || dbEnabled==2) && rt!=NULL)
	{
		//add the rt to the database
		add_rt_to_db(opcode,instruction,rt);
	}

	if(rt==NULL)
	{
		printf("\nget_rt:");
		print_instruction(opcode,instruction);
	}

	_ASSERT(rt);
	
	if(printRT)
	{
		print_res_tab(rt);
	}

	return rt;
	
	//No reservation table found to fit this operation
	_ASSERT(count==1);
	return rt;
}

//This guy tests the reservation table generation
void test_rt(std::map<int,std::list<ResTab*>*> *global_traces){
	ResTab *rt;
	std::map<int,int> instruction;
	int format;
	int computed;

	//read the field map from the field_map file (global_field_map)
	read_field_map();

	/*
	
	instruction[find_field_ident(get_field_map(),"_UNIT_")]=get_ident("L1_E1");
	instruction[find_field_ident(get_field_map(),"_SOURCE_1_")]=get_ident("RFA");
	instruction[find_field_ident(get_field_map(),"_SOURCE_2_")]=get_ident("RFB");
	instruction[find_field_ident(get_field_map(),"_DEST_")]=get_ident("RFA");

	format=find_format(&instruction);

	printf("\nADD L1_E1 RFA RFA RFB:\n\n");
	rt=get_rt("ADD",&instruction,format);
	
	*/
	///////////////////////////////////////////

	/*
	instruction.clear();
	instruction[find_field_ident(get_field_map(),"_UNIT_")]=get_ident("L1_E1");
	instruction[find_field_ident(get_field_map(),"_SOURCE_1_")]=get_ident("RFA");
	instruction[find_field_ident(get_field_map(),"_DEST_")]=get_ident("RFA");

	format=find_format(&instruction);

	printf("\nMV L1_E1 RFA RFA:\n\n");
	rt=get_rt("MV",&instruction,format);
	*/

	return;

	//DLX testing
	instruction.clear();
	instruction[find_field_ident(get_field_map(),"_UNIT_")]=get_ident("IALU");
	instruction[find_field_ident(get_field_map(),"_SOURCE_1_")]=get_ident("REGFILE");
	instruction[find_field_ident(get_field_map(),"_DEST_")]=get_ident("REGFILE");

	format=find_format(&instruction);

	printf("\nASSIGN IALU REGFILE REGFILE:\n\n");
	rt=get_rt("ASSIGN",&instruction,format,computed);

	print_res_tab(rt);


	instruction.clear();
	instruction[find_field_ident(get_field_map(),"_UNIT_")]=get_ident("IALU");
	instruction[find_field_ident(get_field_map(),"_SOURCE_1_")]=get_ident("REGFILE");
	instruction[find_field_ident(get_field_map(),"_DEST_")]=get_ident("REGFILE");
	instruction[find_field_ident(get_field_map(),"_MEM_BYTE_")]=get_ident("DLXMEMORY");

	format=find_format(&instruction);

	printf("\nIVLOAD IALU REGFILE REGFILE DXLMEMORY:\n\n");
	rt=get_rt("IVLOAD",&instruction,format,computed);

	print_res_tab(rt);


	instruction.clear();
	instruction[find_field_ident(get_field_map(),"_UNIT_")]=get_ident("IALU");
	instruction[find_field_ident(get_field_map(),"_SOURCE_1_")]=get_ident("REGFILE");
	instruction[find_field_ident(get_field_map(),"_SOURCE_2_")]=get_ident("REGFILE");
	instruction[find_field_ident(get_field_map(),"_MEM_BYTE_")]=get_ident("DLXMEMORY");

	format=find_format(&instruction);

	printf("\nIVSTORE IALU REGFILE REGFILE DXLMEMORY:\n\n");
	rt=get_rt("IVSTORE",&instruction,format,computed);

	print_res_tab(rt);

	//ATTENTION!!!!!!!!!!!!!!
	return;

	///////////////////////////////////////////

	instruction.clear();
	instruction[find_field_ident(get_field_map(),"_UNIT_")]=get_ident("D1_E1");
	instruction[find_field_ident(get_field_map(),"_SOURCE_1_")]=get_ident("RFA");
	instruction[find_field_ident(get_field_map(),"_DEST_")]=get_ident("RFA");

	format=find_format(&instruction);

	printf("\nMV D1_E1 RFA RFA:\n\n");
	rt=get_rt("MV",&instruction,format,computed);

	///////////////////////////////////////////

	/*
	instruction.clear();
	instruction[find_field_ident(get_field_map(),"_UNIT_")]=get_ident("D2_E1");
	instruction[find_field_ident(get_field_map(),"_SOURCE_1_")]=get_ident("RFB");
	instruction[find_field_ident(get_field_map(),"_DEST_")]=get_ident("RFB");

	format=find_format(&instruction);

	printf("\nMV D2_E1 RFB RFB:\n\n");
	rt=get_rt("MV",&instruction,format);
	*/

	///////////////////////////////////////////

	instruction.clear();
	instruction[find_field_ident(get_field_map(),"_UNIT_")]=get_ident("D1_E1");
	instruction[find_field_ident(get_field_map(),"_SOURCE_1_")]=get_ident("RFA");
	instruction[find_field_ident(get_field_map(),"_SOURCE_2_")]=get_ident("RFA");
	instruction[find_field_ident(get_field_map(),"_MEM_DEST_")]=get_ident("RFA");
	instruction[find_field_ident(get_field_map(),"_MEM_BYTE_")]=get_ident("Memory_Bank0");
	//instruction[find_field_ident(get_field_map(),"_MEM_BYTE_")]=get_ident("Memory_Bank1");

	format=find_format(&instruction);

	printf("\nSTH D1_E1 RFA RFA RFA MBank0 :\n\n");
	rt=get_rt("STH",&instruction,format,computed);

	///////////////////////////////////////////

	instruction.clear();
	instruction[find_field_ident(get_field_map(),"_UNIT_")]=get_ident("D1_E1");
	instruction[find_field_ident(get_field_map(),"_SOURCE_1_")]=get_ident("RFA");
	instruction[find_field_ident(get_field_map(),"_SOURCE_2_")]=get_ident("RFA");
	instruction[find_field_ident(get_field_map(),"_MEM_SRC_")]=get_ident("RFA");
	instruction[find_field_ident(get_field_map(),"_MEM_BYTE_")]=get_ident("Memory_Bank0");
	//instruction[find_field_ident(get_field_map(),"_MEM_BYTE_")]=get_ident("Memory_Bank1");

	format=find_format(&instruction);

	printf("\nLDH D1_E1 RFA RFA RFA MBank0 :\n\n");
	rt=get_rt("LDH",&instruction,format,computed);

	///////////////////////////////////////////

	/*
	instruction.clear();
	instruction[find_field_ident(get_field_map(),"_UNIT_")]=get_ident("D1_E1");
	instruction[find_field_ident(get_field_map(),"_SOURCE_1_")]=get_ident("RFA");
	instruction[find_field_ident(get_field_map(),"_SOURCE_2_")]=get_ident("RFA");
	instruction[find_field_ident(get_field_map(),"_MEM_SRC_")]=get_ident("RFB");
	instruction[find_field_ident(get_field_map(),"_MEM_BYTE_")]=get_ident("Memory_Bank0");
	//instruction[find_field_ident(get_field_map(),"_MEM_BYTE_")]=get_ident("Memory_Bank1");

	format=find_format(&instruction);

	printf("\nLDH D1_E1 RFA RFA RFB MBank0 :\n\n");
	rt=get_rt("LDH",&instruction,format);

  */
	///////////////////////////////////////////

	instruction.clear();
	instruction[find_field_ident(get_field_map(),"_UNIT_")]=get_ident("M1_E1");
	instruction[find_field_ident(get_field_map(),"_SOURCE_1_")]=get_ident("RFA");
	instruction[find_field_ident(get_field_map(),"_SOURCE_2_")]=get_ident("RFA");
	instruction[find_field_ident(get_field_map(),"_DEST_")]=get_ident("RFA");

	format=find_format(&instruction);

	printf("\nMPY M1_E1 RFA RFA RFA:\n\n");
	rt=get_rt("MPY",&instruction,format,computed);
}
