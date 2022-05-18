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

int debugDTSeg=0;

//prototypes
void init_rt(ResTab *rt);
void print_res_tab(ResTab *rt);
void print_res_tabs(std::list<ResTab*> *restabs);
int is_unit_type(NodePtr node);
int any_def_is_unit_type(NodePtr node);
int is_storage_type(NodePtr node);
int any_def_is_storage_type(NodePtr node);
void copy_res_tab(ResTab *src,ResTab *dst);
int gen_rt(ResTab *pp,std::map<int,std::list<NodeList*>*> *config,ResTab *rt);
void delete_res_tab(ResTab *rt);
void complete_fields_in_format(std::map<int,std::list<NodeList*>*> *config,ResTab *rt,std::list<int> *format);
int has_attached_argument(NodePtr port);
int contains_arg_in_object(int arg,NodePtr object);
int get_ident(NodePtr node);
void print_segments(std::list<NodeList*>*segments);


//stores the instruction fields supported by each reservation table
std::map<ResTab*,std::map<int,int> *> supported_instruction_map;
//stores the config which created each rt (the set of DT segments)
std::map<ResTab*,std::map<int,std::list<NodeList*>*>*> *rt_to_config_map;

////////////////////////////////////////////////////////////////////////////////
void print_segment(NodeList*segment){
	NodeList::iterator i;
	for(i=segment->begin();i!=segment->end();i++){
		printf("%s ",(*i)->Name());
	}
}


void print_segments(std::list<NodeList*>*segments){
	std::list<NodeList*>::iterator i;
	int j;

	printf("\nSegments:\n");
	for(j=0,i=segments->begin();i!=segments->end();i++,j++){
		printf("%d: ",j);
		print_segment(*i);
		printf("\n");
	}
}

////////////////////////////////////////////////////////////////////////////////


int contains_in_list(NodeList* elem_list,NodePtr elem){
	NodeList::iterator i;
	_ASSERT(elem);
	_ASSERT(elem_list);
	for(i=elem_list->begin();i!=elem_list->end();i++){
		if(*i==elem) return 1;
	}
	return 0;
}

NodeList::iterator * find_in_list(NodeList* elem_list,NodePtr elem){
	NodeList::iterator *i;
	_ASSERT(elem);
	_ASSERT(elem_list);
	i=new NodeList::iterator();
	for(*i=elem_list->begin();*i!=elem_list->end();(*i)++){
		if(**i==elem) return i;
	}
	return NULL;
}


std::list<NodeList*>::iterator * find_in_list(std::list<NodeList*>* elem_list,NodeList *elem){
	std::list<NodeList*>::iterator *i;
	i=new std::list<NodeList*>::iterator();
	_ASSERT(elem);
	_ASSERT(elem_list);
	for(*i=elem_list->begin();*i!=elem_list->end();(*i)++){
		if(**i==elem) return i;
	}
	return NULL;
}

int equal_res_tab(ResTab *rt1,ResTab *rt2){
	int i;
	NodeList::iterator j;
	for(i=0;i<MAX_PIPELENGTH;i++){
		if((*rt1)[i]->size()!=(*rt2)[i]->size()) return 0;
		for(j=(*rt1)[i]->begin();j!=(*rt1)[i]->end();j++){
			if(!contains_in_list((*rt2)[i],*j)) return 0;
		}
		for(j=(*rt2)[i]->begin();j!=(*rt2)[i]->end();j++){
			if(!contains_in_list((*rt1)[i],*j)) return 0;
		}
	}
	return 1;
}

int contains_res_tab(std::list<ResTab*> *global_traces,ResTab *rt){
	std::list<ResTab*>::iterator i;
	for(i=global_traces->begin();i!=global_traces->end();i++){
		if(equal_res_tab(*i,rt)) return 1;
	}
	return 0;
}


///////////////////////////////////////////////////////////////////////////////////
//initialize the config
void init_config(std::map<int,std::list<NodeList*>*> *config){
	int i;
	for(i=0;i<MAX_PIPELENGTH;i++)
		(*config)[i]=new std::list<NodeList*>();
}
///////////////////////////////////////////////////////////////////////////////////

//Returns 1 if end_point is the start of the segment
//It should be only the first or the second element in the segment
//or the last or next to last element
int start_of_segment(NodeList* segment,NodePtr end_point){
	NodeList::iterator i;
	_ASSERT(segment);
	_ASSERT(end_point);
	_ASSERT(segment->size()!=0);
	i=segment->begin();
	if(i!=segment->end() && *i==end_point) return 1;
	i++;
	if(i!=segment->end() && *i==end_point) return 1;
	i=segment->end();
	i--;
	if(i!=segment->begin() && *i==end_point) return 1;
	i--;
	if(i!=segment->begin() && *i==end_point) return 1;
	return 0;
}

void print_config(std::map<int,std::list<NodeList*>*> *config,std::list<NodeList*>*complete_segments){
	int i,l;
	std::list<NodeList*>::iterator j,k;
	printf("\nConfig:");
	//for each pipeline stage
	for(i=0;i<11;i++){
		printf("\n%d: ",i);
		for(j=(*config)[i]->begin();j!=(*config)[i]->end();j++){
			for(l=0,k=complete_segments->begin();k!=complete_segments->end();k++,l++){
				if(*k==*j){
					printf("%d ",l);
					break;
				}
			}
		}
	}
}

//traces counts the traces acutally generated
long traces=0;
//false_paths counts the number of traces not generated due to false paths (conflicts in the RT). 
long false_paths=0;

///////////////////// gen_dt_config //////////////////////////////////////////
//input: end_points contains the ports and connections from the pipeline path (rt)
//input: complete_segments contains the complete segments in the IR
//output: config will store the current configuration (the list of segments to be added in each stage)
//input: end_point the end_point for which currently generating segments
//input: stage the pipeline stage in which the end_point is active
//Output: global_traces the list of final reservation tables. When finding a new rt, we add it to this list
//Input: pp the pipeline paths
//This function traverses recursively the end_points from the end_points (extracted earlier from pp)
//and finds for each the corresponding segments. 
//When it finds a complete cover with segments, it generates the rt and adds it to global_traces. 
//global_traces is the result
//format: input, the format corresponding to this pp
int gen_dt_config(std::map<int,std::list<NodeList*>*> *config,ResTab *end_points,
						std::list<NodeList*>*complete_segments,
						NodeList::iterator end_point,int stage,
						std::list<ResTab*> *global_traces,ResTab *pp,
						std::list<int> *format){
	int ok;
	std::list<NodeList*>::iterator l;
	NodeList::iterator save_end_point;
	
	_ASSERT(end_points);
	_ASSERT(config);
	
	//check if any end_point is valid
	if(end_point==(*end_points)[stage]->end()){
		//no more end points left in this stage (reached the end of this stage)
		if(stage+1==MAX_PIPELENGTH){
			//Reached the end of the pipeline. Generate reservation table
			ResTab *rt=new ResTab();
			init_rt(rt);
			ok=gen_rt(pp,config,rt);
			if(ok){
				//if the trace has not already been added
				if(!contains_res_tab(global_traces,rt)){
					//no conflict, the trace is valid

					//add this rt and config to the rt_to_config_map
					//rt_to_config_map[rt]=config;


					complete_fields_in_format(config,rt,format);

					if(debugDTSeg) printf("\n/////////////// Trace: %d ///////////////////////////\n",traces);
					traces++;
					//print_res_tab(rt);
					if(debugDTSeg) print_config(config,complete_segments);
					//_ASSERT(!contains_res_tab(global_traces,rt));
					global_traces->push_back(rt);
				}
			}
			else
				delete_res_tab(rt);
			return ok;
		}
		else{
			//not the end of the pipeline
			//go to the next stage
			return gen_dt_config(config,end_points,complete_segments,
				(*end_points)[stage+1]->begin(),stage+1,global_traces,pp,format);
		}
	}
	
	//end_point should be valid at this point
	_ASSERT(*end_point);
	_ASSERT((*end_point)->Symbol());

	NodePtr end_point_node = (*end_point);

	//for each segment 
	for(l=complete_segments->begin();l!=complete_segments->end();l++){
		//if the segment corresponds to this end point,
		if(start_of_segment(*l,(*end_point)->Symbol()->GetFirstDef())){
			_ASSERT(find_in_list((*config)[stage],*l)==NULL);
			//add the new segment
			(*config)[stage]->push_back(*l);
			//go to the next end_point
			
			if(debugDTSeg) print_config(config,complete_segments);

			save_end_point=end_point;
			save_end_point++;
			gen_dt_config(config,end_points,complete_segments,save_end_point,stage,global_traces,pp,format);
			//undo the push_back
			(*config)[stage]->remove(*l);
		}
	}
	return 0;
}

///////////////////// done gen_dt_config //////////////////////////////////////////


///////////////////// collect_end_points //////////////////////////////////////////
//Collects the initial end points for a pipeline path (only the ports or connections)
//and removes them from pp
//Input is a pipeline path, not a reservation table (no dt segments appended yet). 
void collect_end_points(ResTab *pp,ResTab *end_points){
	int i;
	NodePtr node;
	NodeList::iterator j,k;

	_ASSERT(pp);
	_ASSERT(end_points);

	for(i=0;i<MAX_PIPELENGTH;i++){
		_ASSERT((*pp)[i]);
		for(j=(*pp)[i]->begin();j!=(*pp)[i]->end();){
			_ASSERT(*j);
			node=*j;
			k=j;
			j++;
			if(node->Kind()==nke_port){
				(*end_points)[i]->push_back(node);
				//these guys are gonna be inserted again when adding the dt segments
				//so we delete them from here for now
				(*pp)[i]->erase(k);
			}
		}
	}
}

///////////////////// done collect_end_points //////////////////////////////////////////


///////////////////// gen_rt //////////////////////////////////////////
//Input: pp,config
//Output: rt
//The rt will hold the reservation table composed by unionizing pp and the
//segments in config
int gen_rt(ResTab *pp,std::map<int,std::list<NodeList*>*> *config,ResTab *rt){
	int i;
	std::list<NodeList*>::iterator j;
	NodeList::iterator k;
	_ASSERT(pp);
	_ASSERT(config);
	_ASSERT(rt);

	//unionize the pipeline path (pp) and the config into rt

	//copy the pipeilne path pp into rt
	copy_res_tab(pp,rt);

	//add the segments to rt
	//for each pipeline stage
	for(i=0;i<MAX_PIPELENGTH;i++){
		_ASSERT((*config)[i]);
		//for each segment in config
		for(j=(*config)[i]->begin();j!=(*config)[i]->end();j++){
			//first check and see if any of the exclusive resources generate conflicts with 
			//this segment 
			for(k=(*j)->begin();k!=(*j)->end();k++){
				//for ports and connections, look for conflicts
				if(!any_def_is_unit_type(*k) && !any_def_is_storage_type(*k)){
					if(contains_in_list((*rt)[i],*k)){
						//there was a conflict in this rt, so it is invalid
						//conflicts are due to false paths
						if(debugDTSeg){
							printf("\n////////////////////////////////////// Conflict: %d ///////////////////////////////\n",false_paths);
							print_res_tab(rt);
							print_segments((*config)[i]);
						}
						false_paths++;
						//_ASSERT(0);
						return 0;
					}
				}
			}
			//no conflicts with the rest of the rt, so we can add this segment
			//for each object in segment *j
			for(k=(*j)->begin();k!=(*j)->end();k++){
				//add only ports and connections
				//units do not need to be added, since they are already there from the pipeline path
				//storage should be added only if it's not there already
				if(any_def_is_storage_type(*k)){
					//add it if it's not already in (don't duplicate it due to ports)
					//or better, just have it in
					//if(!contains_in_list((*rt)[i],*k)){
						(*rt)[i]->push_back(*k);
					//}
				}
				else if(!is_unit_type(*k) && !is_storage_type(*k)){
					//add these guys, since they are exclusive resources
					(*rt)[i]->push_back(*k);
				}
			}
		}
	}

	return 1;
}

///////////////////// done gen_rt //////////////////////////////////////////


//finds the objects in this RT corresponding to the fields in the format,
//and adds them to the supported_instruction_map 
void complete_fields_in_format(std::map<int,std::list<NodeList*>*> *config,ResTab *rt,std::list<int> *format){
	int i;
	std::list<NodeList*>::iterator j;
	NodeList::iterator k,m;
	std::list<int>::iterator l;
	std::map<int,int> *instruction;
	NodePtr storage;
	_ASSERT(config);
	_ASSERT(rt);
	_ASSERT(format);

	instruction=new std::map<int,int>();

	supported_instruction_map[rt]=instruction;

	//for each pipeline stage
	for(i=0;i<MAX_PIPELENGTH;i++){
		_ASSERT((*config)[i]);
		//for each segment in config
		for(j=(*config)[i]->begin();j!=(*config)[i]->end();j++){
			//for each object *k in the segment *j
			for(k=(*j)->begin();k!=(*j)->end();k++){
				//if the object *k is a port or connection
				if(!any_def_is_unit_type(*k) && !any_def_is_storage_type(*k)){
					//if object *k has an associated label (aka field, or argument)
					if(has_attached_argument(*k)){
						//find the field that is covered by this segment (and the storage in it)
						//for each field in this format
						for(l=format->begin();l!=format->end();l++){
							//if this object (*k) supports this field (*l)
							if(contains_arg_in_object(*l,*k)){
								//find the storage object in this data segment
								//for all the objects in this data segment
								for(m=(*j)->begin();m!=(*j)->end();m++){
									if(any_def_is_storage_type(*m)){
										storage=*m;
										break;
									}
								}
								//storage is the storage object this transfer is done to/from
								(*instruction)[*l]=get_ident(storage);
								break;
							}
						}
					}
				}
			}
		}
	}
}



//gpp contains the pipeline paths collected earlier
//global_traces will store the resulting traces
//complete_segments: intput: stores the compelte dt segments
void append_dt_segments(std::list<ResTab*> *gpp,std::list<ResTab*> *global_traces,
						std::list<NodeList*>*complete_segments,std::list<int> *format){
	std::list<ResTab*>::iterator i;
	int j;
	ResTab *pp;
	ResTab end_points;
	std::map<int,std::list<NodeList*>*> *config;
	_ASSERT(gpp);
	_ASSERT(global_traces);
	_ASSERT(complete_segments);

	rt_to_config_map=new std::map<ResTab*,std::map<int,std::list<NodeList*>*>*>();

	if(debugDTSeg) print_segments(complete_segments);

	//for each pipeline path in the gpp
	//generate all the rts by adding all the data segments (multiple rts get generated for each pp)
	for(j=0,i=gpp->begin();i!=gpp->end();i++,j++){
		pp=*i;
		_ASSERT(pp);
		end_points.clear();
		init_rt(&end_points);
		collect_end_points(pp,&end_points);

		config=new std::map<int,std::list<NodeList*>*>();
		init_config(config);

		if(debugDTSeg) printf("\n///////////////////////////////// End points: %d ////////////////////////////////////\n",j);
		if(debugDTSeg) print_res_tab(&end_points);

		gen_dt_config(config,&end_points,complete_segments,end_points[0]->begin(),0,global_traces,pp,format);
		//delete_res_tab(&end_points);
	}
	if(debugDTSeg) printf("\nTraces: %d",traces);
	if(debugDTSeg) printf("\nConflicts: %d\n",false_paths);
}

int calc_timing_temp_time;

#define max(a, b) ((a < b) ? b : a)

// if a node is of type nke_timing
//
// then, get its integer child.
//
int setTiming(NodePtr np)
{
	_ASSERT(np);

	if (np->Kind() != nke_timing) return 0;

	NodePtr tmg = np->Child(nke_integer);

	if (tmg == NULL) return 0;

	int timing = atoi(tmg->Name());

	calc_timing_temp_time = max(calc_timing_temp_time, timing);

	return 0;
}

// The node ptr that is recieved will be an identifier.
//
// from the symboltable, get its deflist and that will be a component.
//
int maxTiming(NodePtr np)
{
	_ASSERT(np);

	if (np->Symbol() == NULL) return 0;

	// Just get the first def. and ignore the rest.
	//
	NodePtr comp = (np->Symbol())->GetFirstDef();

	if (comp->Kind() != nke_component) return 0;

	calc_timing_temp_time = -1;

	comp->Traverse(setTiming);

	return calc_timing_temp_time;
}

// Added by SAH on 09/21/01.
//
// Given a RT, calculates the length of the pipeline,
// by adding the maximum timings for each stage.
//
int calc_pipelength(ResTab *rt)
{
	_ASSERT(rt);

	int i;
	std::list<NodePtr>::iterator j;

	int pipelen = 0;
	for(i=0;i<MAX_PIPELENGTH;i++){
		// for each stage in the reservation table.
		//
		int currMaxTiming = 0;
		for(j=(*rt)[i]->begin();j!=(*rt)[i]->end();j++){
			// for each node in that particular stage.
			//
			// first find the max timings with all the stages.
			// add it to pipelen.
			//
			currMaxTiming = max(currMaxTiming, maxTiming(*j));
		}
		pipelen += currMaxTiming;
	}
	return pipelen;
}

// Added by SAH on 09/21/01.
//
// This function calculates the maximum length of the pipeline in all the traces.
//
int calc_pipelength(std::list<ResTab*> *global_traces)
{
	_ASSERT(global_traces);

	std::list<ResTab*>::iterator i;

	int retPipeLength = -1;
	for (i = global_traces->begin(); i != global_traces->end(); i++)
	{
		retPipeLength = max(retPipeLength, calc_pipelength(*i));
	}

	return retPipeLength;
}
