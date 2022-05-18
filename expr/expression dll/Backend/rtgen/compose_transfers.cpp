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
#endif

//prototypes
int contains_in_list(NodeList* elem_list,NodePtr elem);
std::list<NodeList*>::iterator * find_in_list(std::list<NodeList*>* elem_list,NodeList *elem);
int is_unit_type(NodePtr node);
int any_def_is_unit_type(NodePtr node);
int is_port_type(NodePtr node);
int is_connection_type(NodePtr node);
int is_storage_type(NodePtr node);
int any_def_is_storage_type(NodePtr node);


//////////////////////////////////////// collect_complete_transfers ////////////////////////////////
//Collect all the complete segments starting from the segment given as argument
//segment will be the first segment in the complete_transfers
//when it finds a complete trnafer, it adds it to complete_transfers
void collect_complete_transfers(NodeList *segment,std::list<NodeList*> *complete_transfer,
								std::list<NodeList*> *complete_transfers,
								std::list<NodeList*> *dt_list){
	NodeList tmp,*seg;
	std::list<NodeList*>::iterator i;
	NodePtr last;
	_ASSERT(segment);
	_ASSERT(complete_transfer);
	_ASSERT(complete_transfers);
	
	//Add this segment to the complete_transfer
	complete_transfer->push_back(segment);
	last=complete_transfer->back()->back();
	_ASSERT(last);
	if(!any_def_is_unit_type(last) && !any_def_is_storage_type(last)){
		//if end point is a port or conn (try to continue)
		//if it's not unit or storage, ie it's a port or a connection, 
		//try to continue the complete transfer
		for(i=dt_list->begin();i!=dt_list->end();i++){
			_ASSERT(*i);
			if((*i)->front()==last){
				if(*find_in_list(complete_transfer,*i)!=complete_transfer->end()){
					collect_complete_transfers(*i,complete_transfer,complete_transfers,dt_list);
				}
				else 
					_ASSERT(0);
			}
		}
	}
	else{
		//end point is a UNIT or STORAGE (Have to stop)
		//copy all the segments from current complete_transfer into seg
		seg=new NodeList();
		for(i=complete_transfer->begin();i!=complete_transfer->end();i++){
			tmp=**i;
			seg->splice(seg->end(),tmp);
			
		}
		//add seg to the list of complete transfers
		complete_transfers->push_back(seg);
	}
}
//////////////////////////////////////// done collect_complete_transfers ////////////////////////////////


void add_to_segment(NodeList *segment,NodePtr node){
	_ASSERT(node);
	_ASSERT(segment);
	_ASSERT(node->Kind()==nke_from || node->Kind()==nke_to ||
		node->Kind()==nke_dtcomponent);
	//for now add them all to the segments
	//eventually, only ports and connections will be added exclusively
	segment->push_back(node->Symbol()->GetFirstDef());
}


/////////////////////////////// get_dt_segment /////////////////////////////////////////////////
//This guy collects the segments from the IR, considering the UNI and BI options
std::list<NodeList*> * get_dt_segment(NodePtr node){
	NodeList *segment;
	std::list<NodeList*> *segments;
	NodeList::iterator i,j;
	NodePtr tmp;
	_ASSERT(node);
	segments=new std::list<NodeList*>();
	if(strcmp(node->Name(),"UNI")==0){
		for(j=node->Children()->begin();j!=node->Children()->end();j++){
			segment=new NodeList();
			tmp=(*j)->FindSubNode(nke_from);
			_ASSERT(tmp);
			add_to_segment(segment,tmp);
			
			
			for(i=(*j)->Children()->begin();i!=(*j)->Children()->end();i++){
				_ASSERT(*i);
				if((*i)->Kind()==nke_dtcomponent){
					_ASSERT((*i)->Symbol()->GetFirstDef());
					add_to_segment(segment,*i);
				}
			}
			
			tmp=(*j)->FindSubNode(nke_to);
			_ASSERT(tmp);
			add_to_segment(segment,tmp);
			
			segments->push_back(segment);
		}
	}
	else if(strcmp(node->Name(),"BI")==0){
		for(j=node->Children()->begin();j!=node->Children()->end();j++){
			segment=new NodeList();
			tmp=(*j)->FindSubNode(nke_from);
			_ASSERT(tmp);
			add_to_segment(segment,tmp);
			
			
			for(i=(*j)->Children()->begin();i!=(*j)->Children()->end();i++){
				if((*i)->Kind()==nke_dtcomponent){
					_ASSERT((*i)->Symbol()->GetFirstDef());
					add_to_segment(segment,*i);
				}
			}
			
			tmp=(*j)->FindSubNode(nke_to);
			_ASSERT(tmp);
			add_to_segment(segment,tmp);
			
			segments->push_back(segment);
			
			segment=new NodeList(*segment);
			segment->reverse();
			
			segments->push_back(segment);
		}
	}
	else
		_ASSERT(0);

	return segments;
}

/////////////////////////////// done get_dt_segment /////////////////////////////////////////////////


//Composes the dt segments into complete end to end transfers
//node is the entire IR
//complete_transfers will store the complete transfers, after composition
void compose_transfers(NodePtr node,std::list<NodeList*>  *complete_transfers){
	NodePtr pipe_section;
	std::list<NodeList*> transfer_list;
	NodeList::iterator i;
	std::list<NodeList*>::iterator j;
	std::list<NodeList*> complete_transfer;//the current complete transfer
	
	_ASSERT(node);
	_ASSERT(complete_transfers);
	
	pipe_section=node->FindSubNode(nke_pipeline_section);
	if(pipe_section==NULL) semantic_error("Missing pipeline section.");
	
	//collect into transfer_list all the nke_dtpaths nodes from the IR
	for(i=pipe_section->Children()->begin();i!=pipe_section->Children()->end();i++){
		if((*i)->Kind()==nke_dtpaths){
			transfer_list.splice(transfer_list.end(),*get_dt_segment(*i));
		}
	}

	//traverse the segments collected above, and compose the complete ppp transfers
	for(j=transfer_list.begin();j!=transfer_list.end();j++){
		complete_transfer.clear();
		collect_complete_transfers(*j,&complete_transfer,complete_transfers,&transfer_list);
	}
}
