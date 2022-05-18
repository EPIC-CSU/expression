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
#include "crtdbg.h"

//prototypes
int contains_in_list(NodeList* elem_list,NodePtr elem);
std::list<NodeList*>::iterator * find_in_list(std::list<NodeList*>* elem_list,NodeList *elem);
NodeList::iterator * find_in_list(NodeList* elem_list,NodePtr elem);
int gen_first_config(NodePtr node,NodeList *config);


//local debug flags
int debugGenPP=0;

//initializes an empty rt
void init_rt(ResTab *rt){
	int i;
	for(i=0;i<MAX_PIPELENGTH;i++){
		(*rt)[i]=new NodeList();
		if((*rt)[i]==NULL){
			printf("EXPRESSION: Out of memory\n");
			_ASSERT(0);
		}
	}
}


//adds the rt restab to the global global_pipeline_paths variable
void add_pp(ResTab *rt,std::list<ResTab*> *global_pipeline_paths){
	global_pipeline_paths->push_back(rt);
}

///////////////////// gen_next_config //////////////////////////////////////////


int gen_next_config(NodePtr node,NodeList *config){
	NodeList::iterator i;
	NodeList *children=node->Children();
	NodePtr def;
	
	_ASSERT(node);
	_ASSERT(config);
	_ASSERT(children);
	_ASSERT(children->size()>0);

	switch(node->Kind()){
	case nke_pipelineinfo:
		_ASSERT(children->size()==1);
		return gen_next_config(children->front(),config);
	case nke_pipeline:
		for(i=children->begin();i!=children->end();i++){
			_ASSERT((*i)->Kind()==nke_identifier);
			_ASSERT((*i)->Symbol()->GetDefList()->size()==1);
			def=(*i)->Symbol()->GetFirstDef();
			if(def->Kind()==nke_pipelineinfo){
				//it is a hierarchycal pipeline stage (not a leaf one)
				//go to that node
				if(gen_next_config(def,config))
					return 1;
			}
			else{
				//it is a leaf pipeline stage
				//do nothing
			}
		}
		return 0;
	case nke_alternate:
		for(i=children->begin();i!=children->end();i++){
			_ASSERT((*i)->Kind()==nke_identifier);
			_ASSERT((*i)->Symbol()->GetDefList()->size()==1);
			if(contains_in_list(config,*i)){
				def=(*i)->Symbol()->GetFirstDef();
				if(def->Kind()==nke_pipelineinfo){
					//it is a hierarchycal pipeline stage (not a leaf one)
					//go to that node
					if(gen_next_config(def,config))
						return 1;
				}
				//need to move to the next one (since couldn't do that at lower level)
				config->erase(*find_in_list(config,*i));
				i++;
				if(i!=children->end()){
					config->push_back(*i);
					def=(*i)->Symbol()->GetFirstDef();
					if(def->Kind()==nke_pipelineinfo){
						gen_first_config(def,config);
					}
					return 1;
				}
				else{
					config->push_back(children->front());
					def=children->front()->Symbol()->GetFirstDef();
					if(def->Kind()==nke_pipelineinfo){
						gen_first_config(def,config);
					}
					return 0;
				}
			}
		}
		//didn't find any child of node in the config
		_ASSERT(0);
		break;
	case nke_parallel:
		return 0;
	default:
		_ASSERT(0);
		break;
	}
	_ASSERT(0);
	return 0;
}


///////////////////// done gen_next_config //////////////////////////////////////////



///////////////////// gen_first_config //////////////////////////////////////////

int gen_first_config(NodePtr node,NodeList *config){
	NodeList::iterator i;
	NodeList *children=node->Children();
	NodePtr def;
	
	_ASSERT(node);
	_ASSERT(config);
	_ASSERT(children);
	_ASSERT(children->size()>0);

	switch(node->Kind()){
	case nke_pipelineinfo:
		_ASSERT(children->size()==1);
		return gen_first_config(children->front(),config);
	case nke_pipeline:
		for(i=children->begin();i!=children->end();i++){
			_ASSERT((*i)->Kind()==nke_identifier);
			_ASSERT((*i)->Symbol());
			_ASSERT((*i)->Symbol()->GetDefList());
			_ASSERT((*i)->Symbol()->GetDefList()->size()==1);
			def=(*i)->Symbol()->GetFirstDef();
			if(def->Kind()==nke_pipelineinfo){
				//it is a hierarchycal pipeline stage (not a leaf one)
				//go to that node
				gen_first_config(def,config);
			}
			else{
				//it is a leaf pipeline stage
				//do nothing
			}
		}
		return 1;
	case nke_alternate:
		_ASSERT(children->front()->Kind()==nke_identifier);
		_ASSERT(children->front()->Symbol()->GetDefList()->size()==1);
		//add the first alternative of the PARALLEL to the config
		config->push_back(children->front());
		def=children->front()->Symbol()->GetFirstDef();
		if(def->Kind()==nke_pipelineinfo){
			//it is a hierarchycal pipeline stage (not a leaf one)
			//go to that node
			gen_first_config(def,config);
		}
		else{
			//it is a leaf pipeline stage
			//do nothing
		}		
		return 1;
	case nke_parallel:
		return 1;
	default:
		_ASSERT(0);
	}
	_ASSERT(0);
	return 0;
}

///////////////////// done gen_first_config //////////////////////////////////////////

///////////////////// gen_pp //////////////////////////////////////////

void gen_pp(NodePtr node,int &stage,ResTab *rt,NodeList *config){
	NodeList::iterator i;
	NodeList *children=node->Children();
	NodePtr def;
	
	_ASSERT(node);
	_ASSERT(rt);
	_ASSERT(config);
	_ASSERT(children);
	_ASSERT(children->size()>0);

	switch(node->Kind()){
	case nke_pipelineinfo:
		_ASSERT(children->size()==1);
		gen_pp(children->front(),stage,rt,config);
		break;
	case nke_pipeline:
		for(i=children->begin();i!=children->end();i++){
			_ASSERT((*i)->Kind()==nke_identifier);
			_ASSERT((*i)->Symbol()->GetDefList()->size()==1);
			def=(*i)->Symbol()->GetFirstDef();
			if(def->Kind()==nke_pipelineinfo){
				//it is a hierarchycal pipeline stage (not a leaf one)
				//go to that node
				gen_pp(def,stage,rt,config);
			}
			else{
				//it is a leaf pipeline stage
				(*rt)[stage]->push_back(*i);
				if(debugGenPP) {printf("\n%d = %s",stage,(*i)->Name());fflush(stdout);}
				stage++;
				_ASSERT(stage<MAX_PIPELENGTH);
			}
		}
		break;
	case nke_alternate:
		for(i=children->begin();i!=children->end();i++){
			_ASSERT((*i)->Kind()==nke_identifier);
			_ASSERT((*i)->Symbol()->GetDefList()->size()==1);
			if(contains_in_list(config,*i)){
				def=(*i)->Symbol()->GetFirstDef();
				if(def->Kind()==nke_pipelineinfo){
					//it is a hierarchycal pipeline stage (not a leaf one)
					//go to that node
					gen_pp(def,stage,rt,config);
				}
				else{
					//it is a leaf pipeline stage
					(*rt)[stage]->push_back(*i);
					if(debugGenPP) {printf("\n%d = %s",stage,(*i)->Name());fflush(stdout);}
					stage++;
					_ASSERT(stage<=MAX_PIPELENGTH);
				}
				break;
			}
		}
		break;
	case nke_parallel:
		if(debugGenPP) {printf("\n%d = ",stage);fflush(stdout);}
		for(i=children->begin();i!=children->end();i++){
			_ASSERT((*i)->Kind()==nke_identifier);
			_ASSERT((*i)->Symbol()->GetDefList()->size()==1);
			def=(*i)->Symbol()->GetFirstDef();
			_ASSERT(def->Kind()!=nke_pipelineinfo);
			//it should be a leaf pipeline stage
			(*rt)[stage]->push_back(*i);
			if(debugGenPP) {printf("%s ",(*i)->Name());fflush(stdout);}
			_ASSERT(stage<=MAX_PIPELENGTH);
		}
		stage++;
		break;
	default:
		_ASSERT(0);
	}
}

///////////////////// done gen_pp //////////////////////////////////////////

///////////////////// gen_pipeline_paths //////////////////////////////////////////

//This function generates the pipeline paths (Step 1)
void gen_pipeline_paths(NodePtr node,std::list<ResTab*> *global_pipeline_paths){
	NodePtr top_pipeline;
	NodeList config;
	int stage=0;
	ResTab *rt;
	int ok;
	
	//first find the top pipeline node
	top_pipeline=node->FindSubNode(nke_pipeline);
	if(top_pipeline==NULL) semantic_error("Missing top-most pipeline.");
	
	//generate first configuration
	ok=gen_first_config(top_pipeline,&config);
	while(ok){
		//generate the pipeline paths (pp)
		rt=new ResTab();
		init_rt(rt);
		stage=0;
		if(debugGenPP) printf("\nGenerating rt:");
		gen_pp(top_pipeline,stage,rt,&config);
		//add them to the global_pipeline_paths
		add_pp(rt,global_pipeline_paths);
		//geneate next configuration
		ok=gen_next_config(top_pipeline,&config);
	}
}

///////////////////// done gen_pipeline_paths //////////////////////////////////////////
