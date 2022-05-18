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
/********************************************/
/* file: node.cpp							*/
/* created: Dec. 3, 1998					*/
/* authors: Gaby Grun						*/
/* last modified:							*/
/* contents: IR definitions for EXPRESSION	*/
/********************************************/

#include <string.h>
#ifdef __UNIX__
	#include <stdio.h>
	#define _ASSERT assert
#else
	#include "crtdbg.h"
#endif

#include "node.h"
#include "errors.h"

char *node_kind_names[]={
	"nke_invalid", //invalid node kind
	"nke_expression", //the complete description
	"nke_operation_section", //the operation section
	"nke_instruction_section", 
	"nke_opmapping_section", 
	"nke_architecture_section", 
	"nke_pipeline_section", 
	"nke_storage_section", 
	"nke_op_group", //operation group in the operation section
	"nke_var_group", //var groups in the operatinos section
	"nke_opcode",
	"nke_var",
	"nke_datatype",  // datatype information
	"nke_classtype",  // classtype information
	"nke_reg_list",  // reg list for the variable class
	"nke_reg",		// reg for the variable class 
	"nke_regs",  // regs for the variable class 
	"nke_optype", //op type in opeartinons section
	"nke_operands", //operands in operations section
	"nke_behavior", //behavior in opeartions section
	"nke_bytesize", // size of an operation in operations section
	"nke_operand",
	"nke_dataop", //data op type 
	"nke_controlop", //control op type
	"nke_flowop",  //flow op type
	"nke_src", //src operand (it's name stores the number of the operand (1,2,3,..))
	"nke_dst", //dst operand (it's name stores the number of the operand (1,2,3,..))
	"nke_memdst", //mem dst operand (it's name stores the number of the operand (1,2,3,..))
	"nke_memsrc", //mem src operand (it's name stores the number of the operand (1,2,3,..))
	"nke_semgroup", //semantic groups 
	"nke_semgroup_circ", //circular semantic group
	"nke_wordlen", //word len in instr section
	"nke_slotlist", //list of slots in instr section
	"nke_slot", //slot in instr section
	"nke_slottype", //slot type
	"nke_slotbitwidth", //slot bitwidth
	"nke_slotunit", //slot unit
	"nke_dataslot", //data slot
	"nke_controlslot", //control slot
	"nke_flowslot", ///flow slot
	"nke_opcode_mapping",  // list of opcode mappings of the opmapping section
	"nke_opcode_map",  // an opcode map in opmapping
	"nke_operand_mapping",  // list of operand mappings of the opmapping section
	"nke_operand_map",  // an operand map in opmapping
	"nke_predoperseq", //oper seq contained in predicate in opmapping section
	"nke_predicate", //predicate in opmapping
	"nke_generic", //generic operations in opmapping
	"nke_target", //target operations in opmapping
	"nke_target_risa", //risa operations in opmapping
	"nke_operation", //operation in opmapping section
	"nke_mapvar", //variable in opmapping
	"nke_fcall", //function call for predicates in opmapping
	"nke_subtypedecl", //subtype decl in architecture section
	//architecture section
	"nke_subtype", //component instatiation in architecture section
	"nke_compinst", //subtype in arch section
	"nke_identifier", //identifier node (used in many sections)
	"nke_initstring", //initialization string for ports etc.
	"nke_subcomps", //sumbomponents in arch section
	"nke_subcomp", //one subcomp
	"nke_component", //one component
	"nke_latches", //latches in arch section
	"nke_in_latches", //input latches in arch section
	"nke_out_latches", //output latches in arch section
	"nke_other_latches", //other latches in arch section
	"nke_latch", //one latch
	"nke_ports", //ports in arch section
	"nke_feedback_enq", //feedbacks in arch section
	"nke_feedback_deq", //feedbacks in arch section
	"nke_feedback", //feedback in arch section
	"nke_queues_enq", //queue enq in arch section
	"nke_queue_enq", //queue enq in arch section
	"nke_queues_deq", //queue deq in arch section
	"nke_queue_deq", //queue deq in arch section
	"nke_port", //one port
	"nke_connections", //connections in arch section
	"nke_connection", //one connection in arch section
	"nke_opcodes", //opcodes in arch section
	"nke_opcodes_enq", //opcodes enq in arch section
	"nke_opcodes_deq", //opcodes deq in arch section
	"nke_timing", //timing in arch section
	"nke_capacity", //capacity in arch section
	"nke_instrin", //num of instructions coming in arch unit
	"nke_instrout", //num of instructions going out of arch unit
	"nke_mnemonic", // mnemonic of a storage element
	"nke_integer", //integer node (used in many sections)
	//pipeline section
	"nke_pipelineinfo", //pipelineinfo in pipeline section
	"nke_pipeline", //the pipeline in pipeline sectino
	"nke_parallel", //the parallel in pipeline section
	"nke_alternate", //the alternate in pipeline section
	"nke_dtpaths", //dt paths in pipeline section
	"nke_dtpath", //dt path in pipeline section
	"nke_from", //from in the dtpath in pipeline section
	"nke_to",	//to in the dtpath in pipeline section
	"nke_dtcomponent",//dt component
	"nke_storage", //storage in storage section
	"nke_storagetype", //storage type in storage section
	"nke_storagesize", //storage size in storage section
	"nke_storagewidth", //storage width in storage section
	"nke_accesstimes", //access times in storage section
	"nke_accessports", //access ports in storage section
	"nke_addrranges", //addr rages in storage section
	"nke_addrrange", //one addr range in storage section
	"nke_writepolicy", //write policy for caches
	"nke_write_back", //write back policy for caches
	"nke_write_through", //write through policy for caches
	"nke_replacementpolicy", //replacement policy for caches
	"nke_lru", //least recently used replacement policy for caches
	"nke_fifo", //first-in-first-out replacement policy for caches	
	"nke_associativity", //associativity for caches
	"nke_wordsize", //word size (number of bits in a word) for caches
	"nke_linesize", //number of words in a line for caches
	"nke_numlines", //number of lines in the cache
	"nke_read_latency", //read latency
	"nke_write_latency", //write latency
	"nke_level", //current level of the cache
	"nke_next_level", //next level the current cache is connected to
	"nke_prev_level", //previous level the current cache is connected to
	"nke_numbanks", // number of banks
	"nke_num_par_read", // number of parallel reads
	"nke_num_par_write", // number of parallel writes
	"nke_rdwr_conflict", // whether same bank has read/write conflict or not
	"nke_access_mode", // access modes
	"nke_stconnections", // storage connections
	"nke_stconnection",  // one storage connection
	"nke_accessport", //one access port in sotrage section
	"nke_argument", //the argument represented by a port
	"nke_all", //the ALL keyword used in OPCODES, TIMING, ....
	"nke_imm" // for creating dummy immediate nodes
};

NodePtr ir; //the only Node representing the whole ir. 


Node::Node(NodeKindEnum k){
	_kind=k;
	_symbol=NULL;
	_name=NULL;
}

Node::~Node(void){
}


NodeKindEnum Node::Kind(void){
	return _kind;
}

void Node::Kind(NodeKindEnum k){
	_kind=k;
}

NodeList * Node::Children(void){
	return &_children;
}


int Node::hasChildren()
{
	return (_children.size());
}


// returns 1 if there exists a child of kind Kind
// else returns 0.
int Node::ExistsChild(NodeKindEnum kind)
{
	for (NodeList::iterator i = _children.begin(); i != _children.end(); i++)
	{
		if ((*i)->Kind() == kind)
			return 1;
	}
	return 0;
}

//Returns the child with the kind kind (assumes there is exactly one)
NodePtr Node::Child(NodeKindEnum kind){
	NodeList::iterator i;
	NodePtr node=NULL;
	int nr=0;
	for(i=_children.begin();i!=_children.end();i++){
		if((*i)->Kind()==kind){
			node=*i;
			nr++;
		}
	}
	_ASSERT(nr<=1);
	return node;
}

void Node::AppendChildren(NodeListPtr l){
	NodeList::iterator i;
	for(i=l->begin();i!=l->end();i++){
		_children.push_back(*i);
	}
}

void Node::AddChild(Node *c){
	_children.push_back(c);
}

SymbolPtr Node::Symbol(void){
	return _symbol;
}
void Node::Symbol(SymbolPtr s){
	_symbol=s;
}

void Node::Parent(NodePtr parent){
	_parent=parent;
}

NodePtr Node::Parent(void){
	return _parent;
}

char *Node::Name(void){
	return _name;
}

void Node::Name(char *n){
	_name=strdup(n);
}


//Traverse traverses the EXPRESSION IR, and calls for each node the func function.
//if func returns 1 it stops and returns
//if fund returns 0 it continues calling for the rest of the IR
//func is a function pointer with NodePtr as argument and returning an int
int Node::Traverse(int(*func)(NodePtr)){
	NodeList::iterator i;
	if((*func)(this)){
		return 1;
	}
	for(i=_children.begin();i!=_children.end();i++){
		if((*i)->Traverse(func)) return 1;
	}
	return 0;
}

// This traverse function is to make transformations where relationship between
// two nodes is needed, e.g finding parents of nodes, or verifying whether the
// parent and child are of the same type etc.
int Node::Traverse(int(*func)(NodePtr,NodePtr)){
	NodeList::iterator i;
	for(i=_children.begin();i!=_children.end();i++){
		if (func(this,*i)) return 1;
		if ((*i)->Traverse(func)) return 1;
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
//used by FindNode
static NodePtr static_node;
static NodeKindEnum static_kind;

int findnode(NodePtr node){
	if(node->Kind()==static_kind){
		static_node=node;
		return 1;//stop traversal
	}
	return 0; //when it returns 1, the Traverse function stops the traversal
}
///////////////////////////////////////////////////////////////////////////////

//This function finds the first node of type "kind"
//in the IR
//Can be called e.g.: ir->FindNode(nke_architecture_section) and returns the nke_arch_section node
NodePtr Node::FindSubNode(NodeKindEnum kind){
	static_kind=kind;
	static_node=NULL;
	Traverse(&findnode);
	return static_node;
}


/////////////////////////////////////////////////////////////////////////////
//used by FindNode
static char *static_name;

int findnodebyname(NodePtr node){
	if(node->Name()!=NULL && strcmp(node->Name(),static_name)==0){
		static_node=node;
		return 1;//stop traversal
	}
	return 0; //when it returns 1, the Traverse function stops the traversal
}
///////////////////////////////////////////////////////////////////////////////

//This function finds the first node with name name
//in the IR
//Can be called e.g.: ir->FindNode("ADD") and returns the first ADD node
NodePtr Node::FindSubNode(char *name){
	static_name=strdup(name);
	static_node=NULL;
	Traverse(&findnodebyname);
	return static_node;
}



void Node::Print(int indent){
	NodeList::iterator i;
	printIndent(indent);
	printf("%s %s",node_kind_names[_kind],(_name==NULL)?"NoName":_name);
	for(i=_children.begin();i!=_children.end();i++){
		(*i)->Print(indent+1);
	}
}

