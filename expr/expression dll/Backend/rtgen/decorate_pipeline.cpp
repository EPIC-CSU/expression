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
	#include <stdio.h>
#endif

#include <set>

//prototypes
std::map<int,std::list<int>*> * get_instr_formats(void);
void read_instr_formats(void);
void init_rt(ResTab *rt);
void copy_res_tab(ResTab *src,ResTab *dst);
int contains_port_in_format(std::list<int> * format,NodePtr port,int &field);
void print_res_tab(ResTab *rt);
int has_attached_argument(NodePtr port);
void print_format(std::list<int> *format);
int all_format_considered(std::list<int> *format,ResTab *rt);
void read_field_map(void);
std::list<int>::iterator find_in_list(std::list<int> *format,int val);
int contains_arg_in_object(int arg,NodePtr object);
int find_field_ident(std::map<char *,int> *global_field_map,char *name);
std::map<char *,int> *get_field_map(void);
void clear_res_tab(ResTab *rt);
int contains_in_list(std::list<int> *format,int val);
int no_writes_before_reads(ResTab *rt,int format);

int debugDPP=0;

//add resources from src to dest list
void add_res(NodeList *dest,NodeList *src){
	NodeList::iterator i;
	_ASSERT(src);
	_ASSERT(dest);

	for(i=src->begin();i!=src->end();i++){
		_ASSERT(*i);
		dest->push_back(*i);
	}
}

//counts the number of decorated pipeline paths
long decorated_pps=0;

//Decorates recursively the rt with ports from ports, depending whether they
//are characteristic ports and they are in the format or not
//rt: input, used to build the complete decorated pipeline path
//ports: input, the set of ports to choose from when decorating
//stage: input, the current stage
//port: input, the current port being considered from the stage stage in ports
//dgpp: output: the list of decorated pipeline paths
//format: input, the current instruction format considered 
void decorate_port(ResTab *rt,ResTab *ports,int stage,NodeList::iterator port,std::list<ResTab*> *dgpp,
				   std::list<int> * format,int format_nr){
	NodeList::iterator temp_port;
	//if end of this stage, or end of pipeline
	if(port==(*ports)[stage]->end()){
		if(stage==MAX_PIPELENGTH-1){
			//end of pipeline, add this reservation table
			//if(all_format_considered(format,rt)){
			//if the format is empty, all the fields have been satisfied by ports
			if(format->empty()){
				//if(no_writes_before_reads(rt,format_nr)){
				ResTab *new_rt;
				new_rt=new ResTab();
				init_rt(new_rt);
				copy_res_tab(rt,new_rt);
				dgpp->push_back(new_rt);
				decorated_pps++;
				if(debugDPP) printf("\n////////////// Added Ports: ///////////////////////////////\n");
				if(debugDPP) print_res_tab(new_rt);
				//}
			}
		}
		else{
			//progress to the next stage
			decorate_port(rt,ports,stage+1,(*ports)[stage+1]->begin(),dgpp,format,format_nr);
		}
	}
	else{
		temp_port=port;
		temp_port++;

		//test if it is a characteristic port, or freelance
		if(has_attached_argument(*port)){
			int field;
			//if the port has an attached field (argument)
			if(contains_port_in_format(format,*port,field)){
				//if it is contained in the format, add it
				//first consider the port added
				format->erase(find_in_list(format,field));
				(*rt)[stage]->push_back(*port);
				decorate_port(rt,ports,stage,temp_port,dgpp,format,format_nr);
				(*rt)[stage]->remove(*port);
				format->push_back(field);

				//then consider the port omitted (to alow for other ports to fulfill this field)
				decorate_port(rt,ports,stage,temp_port,dgpp,format,format_nr);
			}
			else{
				//if it is not contained in the format, omit it
				decorate_port(rt,ports,stage,temp_port,dgpp,format,format_nr);
			}
		}
		else{
			//if the port is a freelance port (no attached argument)
			//go with the port omitted
			decorate_port(rt,ports,stage,temp_port,dgpp,format,format_nr);
			
			//add the port
			(*rt)[stage]->push_back(*port);
			decorate_port(rt,ports,stage,temp_port,dgpp,format,format_nr);
			(*rt)[stage]->remove(*port);
		}
	}
}

//find the timing of this obj (0,1,2,...), relative to the unit it is attached to 
//ATTENTION!: currently only one timing per port is supported
NodePtr find_timing(NodePtr obj){
	NodePtr def,timing;
	_ASSERT(obj);
	//def is the definition node for the obj
	def=obj->Symbol()->GetFirstDef();
	_ASSERT(def);
	timing=def->FindSubNode(nke_timing);
	if(timing==NULL) 
		return 0;
	else{
		return timing;
	}
}

//collects the pool of ports which will decorate the pipeline path
//input: rt, the pipeline path, containing only units
//output: ports, the pool of ports collected for the input pipeline path (rt)
void collect_ports(ResTab *rt,ResTab *ports){
	NodeList::iterator k,i;
	std::set<NodePtr> history;
	int j;
	NodePtr res,def;
	NodePtr port_child;
	NodeList stage;

	//for each pipeline stage
	for(j=0;j<MAX_PIPELENGTH;j++){
		_ASSERT((*rt)[j]);
		//(*ports)[j]->clear();
		//copy the list into a temporary
		stage=*((*rt)[j]);
		//collect all ports for this stage into port_list
		//for each unit in the stage
		for(k=stage.begin();k!=stage.end();k++){
			res=*k;
			_ASSERT(res);
			_ASSERT(res->Symbol());
			_ASSERT(res->Symbol()->GetDefList()->size()==1);
			//if the resource is not a multicycled one, or it is it's first occurence (if multicycled)
			if(history.find(res)==history.end()){
				//next time shouldn't get in here
				history.insert(res);
				//def is the node defining res
				def=res->Symbol()->GetFirstDef();
				//leaf node (it should only have leaf nodes here-should be flattened out)
				_ASSERT(def->Kind()==nke_component);
				//Add all the ports for this unit to the pipeline path
				port_child=def->Child(nke_ports);
				if(port_child!=NULL){
					for(i=port_child->Children()->begin();i!=port_child->Children()->end();i++){
						(*ports)[j]->push_back(*i);
						/*
						timing=find_timing(*i);
						if(timing==NULL){
							(*ports)[j]->push_back(*i);
						}
						else{
							//for all the relative time offsets in this timing node
							for(l=timing->Children()->begin();l!=timing->Children()->end();l++){
								if((*l)->Kind()==nke_integer){
									_ASSERT(*l);
									t=atoi((*l)->Name());
									_ASSERT(j+t<PIPELENGTH);
									(*ports)[j+t]->push_back(*i);
								}
							}
						}
						*/
					}
				}
			}
		}
		
	}
}

//extracts all the fields from format referenced by units in the rt, and inserts them in temp_format
void extract_unit_fields(ResTab *rt,std::list<int>*format,std::list<int>*temp_format){
	NodePtr argument,object;
	NodeList::iterator l,j;
	int field,i;

	_ASSERT(format);
	_ASSERT(temp_format);
	_ASSERT(rt);

	//for each pipeline stage in rt
	for(i=0;i<MAX_PIPELENGTH;i++){
		//for each unit in this stage
		for(j=(*rt)[i]->begin();j!=(*rt)[i]->end();j++){
			object=*j;
			_ASSERT(object);
			//for each argument (field) for object
			for(l=object->Symbol()->GetFirstDef()->Children()->begin();l!=object->Symbol()->GetFirstDef()->Children()->end();l++){
				argument=*l;
				_ASSERT(argument);
				if(argument->Kind()==nke_argument){
					field=find_field_ident(get_field_map(),argument->Name());
					if(contains_in_list(format,field)){
						format->erase(find_in_list(format,field)); 
						temp_format->push_back(field);
					}
				}
			}
		}
	}
}


//This function decorates the pipeline paths with ports (add ports to the reservation tables)
//and returns it in dgpp (decorated gpp)
void decorate_pipeline_paths(std::list<ResTab*> *gpp,std::list<ResTab*> *dgpp,std::list<int>* format,int format_nr){
	ResTab ports;
	std::list<ResTab*>::iterator i;
	std::map<int,std::list<int>*>::iterator l;
	std::list<int> unit_format;

	_ASSERT(gpp);
	_ASSERT(dgpp);

	init_rt(&ports);

	//for each pipeline path
	for(i=gpp->begin();i!=gpp->end();i++){
		_ASSERT(*i);

		if(debugDPP) printf("\n//////////////////////// Decorating pipeline path: //////////////////////\n");
		if(debugDPP) print_res_tab(*i);

		if(debugDPP) print_format(format);

		//delete from format all the fields that are satisfied by units from the rt (*i)
		//and move them to unit_format
		extract_unit_fields(*i,format,&unit_format);


		clear_res_tab(&ports);
		//first create the ports data structure, hodling the pool of ports for this rt
		collect_ports(*i,&ports);


		if(debugDPP) printf("\n//////////////////////// Collected ports: ///////////////////////////////\n");
		if(debugDPP) print_res_tab(&ports);
		
		decorated_pps=0;

		//the ports holds all the ports for rt
		decorate_port(*i,&ports,0,ports[0]->begin(),dgpp,format,format_nr);

		//move back the fields from format removed by extract_unit_fields
		format->splice(format->begin(),unit_format);
	}
}
