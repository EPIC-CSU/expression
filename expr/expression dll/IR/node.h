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
/* file: node.h								*/
/* created: Dec. 3, 1998					*/
/* authors: Gaby Grun						*/
/* last modified:							*/
/* contents: IR definitions for EXPRESSION	*/
/********************************************/

#ifndef _NODE_H_
#define _NODE_H_

#ifdef WIN32
#pragma warning(disable:4786)
#pragma warning(disable:4503)
#endif

#include <list>

#include "symbols.h"

typedef enum NodeKindEnum {
	nke_invalid, //invalid node kind
	nke_expression, //the complete description
	nke_operation_section, //the operation section
	nke_instruction_section, 
	nke_opmapping_section, 
	nke_architecture_section, 
	nke_pipeline_section, 
	nke_storage_section, 
	nke_op_group, //operation group in the operation section
	nke_var_group, //var groups in the operatinos section
	nke_opcode,  //an opcode in the operatinos section
	nke_var, //a var group in the operations section
	nke_datatype, // datatype information about the var
	nke_classtype, // datatype information about the var
	nke_reg_list,  // reg list for the variable class
	nke_reg,  // reg for the variable class
	nke_regs,  // regs for the variable class
	nke_optype, //op type in opeartinons section
	nke_operands, //operands in operations section
	nke_behavior, //behavior in opeartions section
	nke_bytesize,  // size of the operation in operations section
	nke_operand, //an operand in the operations section
	nke_dataop, //data op type 
	nke_controlop, //control op type
	nke_flowop,  //flow op type
	nke_src, //src operand (it's name stores the number of the operand (1,2,3,..))
	nke_dst, //dst operand (it's name stores the number of the operand (1,2,3,..))
	nke_memdst, //mem dst operand (it's name stores the number of the operand (1,2,3,..))
	nke_memsrc, //mem src operand (it's name stores the number of the operand (1,2,3,..))	
	nke_semgroup, //semantic groups 
	nke_semgroup_circ, //circular semantic group
	nke_wordlen, //word len in instr section
	nke_slotlist, //list of slots in instr section
	nke_slot, //slot in instr section
	nke_slottype, //slot type
	nke_slotbitwidth, //slot bitwidth
	nke_slotunit, //slot unit
	nke_dataslot, //data slot
	nke_controlslot, //control slot
	nke_flowslot, ///flow slot
	nke_opcode_mapping, //opcode_map list in opmapping section
	nke_opcode_map, //an opcode_map in opmapping section
	nke_operand_mapping, //operand_map list in opmapping section
	nke_operand_map, //an operand_map in opmapping section
	nke_predoperseq, //oper seq contained in predicate in opmapping section
	nke_predicate, //predicate in opmapping
	nke_generic, //generic operations in opmapping
	nke_target, //target operations in opmapping
	nke_target_risa, //risa operations in opmapping
	nke_operation, //operation in opmapping section
	nke_mapvar, //variable in opmapping
	nke_fcall, //function call for predicates in opmapping
	//architecture section
	nke_subtypedecl, //subtype decl in architecture section
	nke_subtype, //subtype in arch section
	nke_compinst, //component instatiation in architecture section
	nke_identifier, //identifier node (used in many sections)
	nke_initstring, //initialization string for ports etc.
	nke_subcomps, //sumbomponents in arch section
	nke_subcomp, //one subcomp
	nke_component, //one component
	nke_latches, //latches in arch section
	nke_in_latches, //input latches in arch section
	nke_out_latches, //output latches in arch section
	nke_other_latches, //other latches in arch section
	nke_latch, //one latch
	nke_ports, //ports in arch section
	nke_feedback_enq, //feedbacks in arch section
	nke_feedback_deq, //feedbacks in arch section
	nke_feedback, //feedback in arch section
	nke_queues_enq, //queue enq in arch section
	nke_queue_enq, //queue enq in arch section
	nke_queues_deq, //queue deq in arch section
	nke_queue_deq, //queue deq in arch section
	nke_port, //one port
	nke_connections, //connections in arch section
	nke_connection, //one connection in arch section
	nke_opcodes, //opcodes in arch section
	nke_opcodes_enq, //opcodes enq in arch section
	nke_opcodes_deq, //opcodes deq in arch section
	nke_timing, //timing in arch section
	nke_capacity, //capacity in arch section
	nke_instrin, //num of instructions coming in 
	nke_instrout, //num of instructions going out
	nke_mnemonic, // gives the mnemoic of a storage element
	nke_integer, //integer node (used in many sections)
	//pipeline section
	nke_pipelineinfo, //pipelineinfo (contains an id and either pipeline or parallel) in pipeline section
	nke_pipeline, //the pipeline in pipeline sectino
	nke_parallel, //the parallel in pipeline section
	nke_alternate, //the alternate in pipeline section
	nke_dtpaths, //dt paths in pipeline section
	nke_dtpath, //dt path in pipeline section
	nke_from, //from in the dtpath in pipeline section
	nke_to,	//to in the dtpath in pipeline section
	nke_dtcomponent,//dt component
	nke_storage, //storage in storage section
	nke_storagetype, //storage type in storage section
	nke_storagesize, //storage size in storage section
	nke_storagewidth, //storage width in storage section
	nke_accesstimes, //access times in storage section
	nke_accessports, //access ports in storage section
	nke_addrranges, //addr rages in storage section
	nke_addrrange, //one addr range in storage section
	nke_writepolicy, //write policy for caches
	nke_write_back, //write back policy for caches
	nke_write_through, //write through policy for caches
	nke_replacementpolicy, //replacement policy for caches
	nke_lru, //least recently used replacement policy for caches
	nke_fifo, //first-in-first-out replacement policy for caches	
	nke_associativity, //associativity for caches
	nke_wordsize, //word size (number of bits in a word) for caches
	nke_linesize, //number of words in a line for caches
	nke_numlines, //number of lines in the cache
	nke_read_latency, //read latency
	nke_write_latency, //write latency
	nke_level, //current level of the cache
	nke_next_level, //next level the current cache is connected to
	nke_prev_level, //previous level the current cache is connected to
	nke_numbanks, // number of banks
	nke_num_par_read, // number of parallel reads
	nke_num_par_write, // number of parallel writes
	nke_rdwr_conflict, // whether same bank has read/write conflict or not
	nke_access_mode, // access modes
	nke_stconnections, // storage connections
	nke_stconnection,  // one storage connection
	nke_accessport, //one access port in sotrage section
	nke_argument, //the argument represented by a port
	nke_string,   // having a string value
	nke_asmFormat, // Assembly Generation Format..
	nke_asmPrintCond, // Assembly Generation Format..
	nke_asmCond, // Assembly Generation Format..
	nke_asmPrint, // Assembly Generation Format..
	nke_dumpFormat, // Assembly Generation Format..
	nke_all, //the ALL keyword used in OPCODES, TIMING, ....
	nke_imm // for creating dummy immediate nodes
};


//The EXPRESSION IR is composed of a hierarchy of Nodes
class Node{
	NodeKindEnum _kind;  //node kind
	NodeList _children; //node's children
	NodePtr _parent; //node's parent in the IR
	SymbolPtr _symbol; //symbol in symtab representing this node
	char * _name;   //a char string containing the name of this guy 
					//(if _symbol!=NULL, _symbol will contain the same name).

public:
	Node(NodeKindEnum k=nke_invalid);
	~Node(void);
	NodeKindEnum Kind(void);
	void Kind(NodeKindEnum k);
	NodeList *Children(void);
	int hasChildren(void);
	int ExistsChild(NodeKindEnum kind);
	NodePtr Child(NodeKindEnum kind);
	void AppendChildren(NodeListPtr l);
	void AddChild(Node *c);
	SymbolPtr Symbol(void);
	void Symbol(SymbolPtr s);
	void Parent(NodePtr parent);
	NodePtr Parent(void);
	char *Name(void);
	void Name(char *n);
	int Traverse(int(*func)(NodePtr));
	int Traverse(int(*func)(NodePtr,NodePtr));
	NodePtr FindSubNode(NodeKindEnum kind);
	NodePtr FindSubNode(char *name);
	void Print(int indent=0);
};

extern NodePtr ir; //the only Node representing the whole ir. 

extern char *node_kind_names[];

#endif
