/********************************************/
/* file: expression.y						*/
/* created: Nov. 30, 1998					*/
/* authors: Gaby Grun						*/
/* last modified: 7/25/2000 (AVS)			*/
/* contents: bison grammar for EXPRESSION	*/
/********************************************/





%token	_opgroup_
%token	_opcode_
%token	_optype_
%token	_operands_	
%token	_behavior_
%token	_asm_format_
%token	_dump_format_
%token	_asm_print_
%token	_asm_cond_
%token  _bytesize_	
%token	_dataop_	
%token	_controlop_	
%token	_flowop_	
%token	_src_	
%token	_dst_	
%token  _mem_dst_
%token  _mem_src_
%token	_vargroups_
%token  _datatype_
%token  _classtype_
%token	_wordlen_	
%token	_slots_	
%token	_type_	
%token	_data_	
%token	_control_	
%token	_flow_	
%token	_bitwidth_	
%token  _opcode_mapping_
%token  _operand_mapping_	
%token	_op_mapping_
%token	_generic_	
%token	_target_
%token  _target_risa_
%token	_regs_	
%token	_subtype_	
%token	_latch_	
%token  _input_
%token  _output_
%token  _other_
%token	_storage_	
%token	_port_	
%token	_connection_	
%token	_units_	
%token	_latches_	
%token	_ports_	
%token	_feedback_enq_	
%token	_feedback_deq_	
%token	_feedback_	
%token  _queues_enq_ 
%token  _queues_deq_
%token	_connections_	
%token	_storages_	
%token	_opcodes_	
%token	_opcodes_enq_	
%token	_opcodes_deq_	
%token	_timing_	
%token	_capacity_	
%token	_mnemonic_	
%token	_pipeline_	
%token	_dtpaths_	
%token	_uni_	
%token	_bi_	
%token	_icache_	
%token	_dcache_	
%token	_memory_	
%token	_size_	
%token	_width_	
%token	_line_
%token	_linesize_
%token	_wordsize_
%token	_num_lines_	
%token	_write_policy_
%token	_write_policy_options
%token	_replacement_policy_
%token	_replacement_policy_options	
%token	_level_	
%token  _storage_connections_
%token	_next_level_	
%token	_prev_level_	
%token	_lru_	
%token	_fifo_
%token	_write_back_
%token	_read_latency_
%token	_write_latency_
%token	_write_through_	
%token	_associativity_
%token  _num_banks_
%token	_access_mode_
%token	_num_parallel_read_
%token	_num_parallel_write_
%token	_read_write_conflict_	
%token	_addrrange_	
%token	_operations_section_	
%token	_instruction_section_	
%token	_opmapping_section_	
%token	_architecture_section_	
%token	_pipeline_section_	
%token	_storage_section_	
%token	_char_string_	
%token	_predicate_	
%token	_subcomponents_	
%token	_parallel_	
%token  _alternate_
%token	_path_	
%token	_accesstimes_	
%token	_accessports_	
%token	_accesswidths_	
%token	_stride_	
%token	_circ_	
%token	_none_	
%token	_all_	
%token	_true_	
%token  _unit_
%token  _argument_
%token	Identifier	
%token	Integer_lit
%token  _tree_mappings_
%token _end_tree_mappings_
%token ISEL_GENERIC 
%token ISEL_TARGET
%token ISEL_LHS 
%token ISEL_RHS 
%token ISEL_DST 
%token ISEL_SRC 
%token ISEL_LPAREN 
%token ISEL_RPAREN
%token ISEL_LBRACE
%token ISEL_RBRACE
%token ISEL_COLON
%token ISEL_EQUAL
%token ISEL_ADD_OPERATOR
%token ISEL_SUB_OPERATOR
%token ISEL_EQ_OPERATOR
%token ISEL_LT_OPERATOR
%token _instrin_
%token _instrout_

%token '(' ')' '*' '+' '-' ',' '/' ':' '<' '=' '>' '[' ']' '\\'

%{
#define YYDEBUG 1
#include "parser.h"
#include "malloc.h"
#include "node.h"
#include "treeparse.h"

#define MAX_OPCODES 1000
Symtab st; //the only symbol table


char buf[1000]; //temporary char buffer used in parser

char * tree_opcode[MAX_OPCODES]; // table to hold opcodes in 
char * tree_srclist[MAX_OPCODES];
char * tree_dstlist[MAX_OPCODES];
char * tree_opcode_type[MAX_OPCODES];
int combinations[100];

//vector<char *> tree_opcode;

int count_gen = 0;
// detect user defined combined instructions
int comb_detect = 0;
int comb_index = 0;

int ISelcurrentOp, ISeltempOp;
ISelRuleType targetRules;
ISelRuleType genericRules;
OpcToRuleNoType ISelOpcToRuleNo;
ISelRuleType lhsRules;
ISelRuleType rhsRules;
OpcToRuleNoType OptOpcToRuleNo;

void UpdateSymbolsLinks(NodeListPtr list);
void UpdateSymbolLinks(NodePtr node);
void DeclareSymbols(NodeListPtr list);

%}

/* Declare the alternative types of the grammar symbols. */
%union { 
    int in;
    char ch;
	NodePtr node;
	NodeListPtr nodel;
    token tok;
	char * st;
}
    /* These are the specific types of grammar symbols. */

%token <in> ISEL_NUM
%token <st> ISEL_TYPE
%token <st> ISEL_OPCODE
%type <in> dstNum srcNum val
%type <st> argType
%type <st> argIdent
%type <st> Op
%type <st> srcList dstList
%type <st> opGenList 
%type <st> genericPart 
%type <st> opTarList 
%type <st> targetPart
%type <st> lhsPart
%type <st> rhsPart

%type <in> Integer_lit
%type <st> Identifier _char_string_ base_type t_type asmPrintFormat _asm_cond_ 
%type <st> _asm_print_ asmCond asmPrint

%type <node> Operation_section Instruction_section OpMapping_section ArchComponent_section
%type <node> PipeDTPaths_section Storage_section description op_group vargroup opattribute
%type <node> one_operand optype operand_semgroup word_len slot_list slot
%type <node> instr_attr type_name pred_dr_ops oper_seq pred oper mapvar
%type <node> mapvar_id subtype_decl compinst comp_attribute sub_comps latches ports connections feedback_enq feedback_deq queue_enq queue_deq
%type <node> opcodes opcodes_enq opcodes_deq capacity instrin instrout argument pipeline_info pipeline parallel alternate dtpaths dt from to
%type <node> storage storageattr accessport addrrange integer one_comp comp_initstr cyclenum
%type <node> write_policy_options replacement_policy_options stconnection
%type <node> datatype opcode_mapping operand_mapping opcode_map operand_map
%type <node> generic_operand target_operand data_type class_type reg regs

%type <nodel> op_group_list vargroup_list op_list Identifier_list op opattribute_list
%type <nodel> operand_list operand_semgroups s_list instr_attr_list type_name_list
%type <nodel> pred_dr_ops_list pred_list operlist mapvar_list subtype_decls
%type <nodel> compinst_list Opcode_list comp_attributes Integer_lit_list cyclenum_list timings
%type <nodel> pipeline_info_list dtpath_list dt_list storage_list storageattr_list
%type <nodel> accessport_list addrrange_list subtype_list component_list dtcomponent_list 
%type <nodel> port_list latch_list subcomp_list connection_list aggregate_latchlist stconnection_list feedbacks_list queue_enq_list queue_deq_list
%type <nodel> range_list range opcode_map_list operand_map_list reg_list listOfAsmFormats listOfIRFormats


%start description

%%

description:
	{ 
		printf("Started parsing EXPRESSION ...\n"); 
	} 

	'(' _operations_section_ Operation_section ')' 
	{ 
		printf("\tFinished parsing Operation Section\n"); 
	} 
	
	'(' _instruction_section_ Instruction_section ')' 
	{ 
		printf("\tFinished parsing Instruction Section\n"); 
	} 
	
	'(' _opmapping_section_ OpMapping_section ')' 
	{ 
		printf("\tFinished parsing OpMapping Section\n"); 
	} 
	
	'(' _architecture_section_ ArchComponent_section ')' 
	{ 
		printf("\tFinished parsing ArchComponent Section\n"); 
	}

	'(' _pipeline_section_ PipeDTPaths_section ')' 
	{ 
		printf("\tFinished parsing PipeDTPaths Section\n"); 
	}

	'(' _storage_section_ Storage_section ')'	
	{
		$$=ir=new Node(nke_expression);
		ir->AddChild($4);
		ir->AddChild($9);
		ir->AddChild($14);
		ir->AddChild($19);
		ir->AddChild($24);
		ir->AddChild($29);
		printf("Finished parsing EXPRESSION\n"); 
	}
	;

/******************************** Section 1. Operations ******************************************/

Operation_section: 
	op_group_list			{
								$$=new Node(nke_operation_section);
								$$->AppendChildren($1);
							}
	|
	/* empty */				{	$$=new Node(nke_operation_section);}
	;

op_group_list: 
	op_group_list op_group	{$1->push_back($2);$$=$1;}
	| 
	op_group				{$$=new NodeList(); $$->push_back($1);}
	;

op_group: 
	'(' _opgroup_ Identifier op_list ')'		{$$=new Node(nke_op_group);$$->Name($3);$$->Symbol(st.Declare($3,$$));$$->AppendChildren($4);}
	|
	'(' _vargroups_ vargroup_list ')'			{$$=new Node(nke_var_group);$$->AppendChildren($3);}
	;

op_list: 
	op_list op				{$1->splice($1->begin(),*$2);$$=$1;}
	|
	op						{$$=new NodeList(); $$->splice($$->begin(),*$1);}
	;

op: 
	'(' _opcode_ Opcode_list opattribute_list ')'	{
															$$=$3;
															for(NodeList::iterator i=$$->begin();i!=$$->end();i++){
																(*i)->Kind(nke_opcode);
																(*i)->AppendChildren($4);
															}
														}
	|
	'(' _opcode_ Opcode_list ')'						{
															$$=$3;
															for(NodeList::iterator i=$$->begin();i!=$$->end();i++){
																(*i)->Kind(nke_opcode);
															}
														}
	;

Opcode_list: 
	Opcode_list Identifier				{
											NodePtr n=new Node(nke_invalid); n->Name($2);
											n->Symbol(st.Declare($2,n));
											$1->push_back(n);$$=$1;
										}
	|
	Identifier							{
											$$=new NodeList(); 
											NodePtr n=new Node(nke_invalid); n->Name($1);
											n->Symbol(st.Declare($1,n));
											$$->push_back(n);
										}
	;

Identifier_list: 
	Identifier_list Identifier			{
											NodePtr n=new Node(nke_identifier);n->Name($2);
											$1->push_back(n);$$=$1;
										}
	|
	Identifier							{
											NodePtr n=new Node(nke_identifier);n->Name($1);
											$$=new NodeList(); 
											$$->push_back(n);
										}
	;

opattribute_list: 
	opattribute_list opattribute		{$1->push_back($2);$$=$1;}
	|
	opattribute							{$$=new NodeList(); $$->push_back($1);}
	;

listOfAsmFormats:
	'(' asmPrintFormat ')'				{											
											$$=new NodeList();
											NodePtr n = new Node(nke_asmPrint);
											n->Name($2);
											$$->push_back(n);
										}


	| listOfAsmFormats '(' asmPrintFormat ')'
										{											
											NodePtr n = new Node(nke_asmPrint);
											n->Name($3);
											$1->push_back(n);
											$$=$1;
										}
	;

asmPrintFormat:
	asmCond  asmPrint 
	{
											/* concatenate _cond_str and _print_str */
										$$ = (char *) malloc(strlen($1)+strlen($2)+1);
										sprintf($$, "~%s~\"%s\"", $1, $2);
	}
	;

asmCond:
	'(' _asm_cond_ _char_string_ ')' 
									{
										$$ = strdup($3);
									}
									;
asmPrint:
	'(' _asm_print_ _char_string_ ')'
									{
										$$ = strdup($3);
									}
									;	
listOfIRFormats:
	'(' asmPrintFormat ')'				{											
											$$=new NodeList();
											NodePtr n = new Node(nke_asmPrint);
											n->Name($2);
											$$->push_back(n);
										}


	| listOfIRFormats '(' asmPrintFormat ')'
										{											
											NodePtr n = new Node(nke_asmPrint);
											n->Name($3);
											$1->push_back(n);
											$$=$1;
										}
	;


opattribute: 
	'(' _optype_ optype ')'				{$$=new Node(nke_optype);$$->AddChild($3);}
	|
	'(' _operands_ operand_list ')'		{$$=new Node(nke_operands);$$->AppendChildren($3);}
	|
	'(' _behavior_ _char_string_ ')'	{$$=new Node(nke_behavior);$$->Name($3);}
	|
	'(' _behavior_ _none_ ')'			{$$=new Node(nke_behavior);}
	|
	'(' _bytesize_ Integer_lit ')'		{$$=new Node(nke_bytesize); $$->Name(itoa($3, buf, 10));}
	|
	'(' _asm_format_ listOfAsmFormats')'
										{
											$$=new Node(nke_asmFormat);
											$$->AppendChildren($3);
										}
	|
	'(' _dump_format_ listOfIRFormats')'
										{
											$$=new Node(nke_dumpFormat);
											$$->AppendChildren($3);
										}
	;


optype: 
	_dataop_							{$$=new Node(nke_dataop);}
	|
	_controlop_							{$$=new Node(nke_controlop);}
	|
	_flowop_							{$$=new Node(nke_flowop);}
	;

operand_list:
	/* nothing */						{$$=new NodeList();}	
	| 
	operand_list one_operand			{$1->push_back($2);$$=$1;}
	|
	one_operand							{$$=new NodeList(); $$->push_back($1);}
	;

one_operand: 
	/*'(' operand_type Identifier operand_semgroups ')'	{$$=new Node(nke_operand);$$->Name($3);UpdateSymbolLinks($$);$$->AddChild($2);}*/
	'(' Identifier Identifier operand_semgroups ')'	{
		$$=new Node(nke_operand);
		$$->Name($2);
		Node *temp=new Node(nke_identifier);temp->Name($3);
		UpdateSymbolLinks(temp);
		$$->AddChild(temp);
	}
	|
	/*'(' operand_type Identifier ')'						{$$=new Node(nke_operand);$$->Name($3);UpdateSymbolLinks($$);$$->AddChild($2);}*/
	'(' Identifier Identifier ')'	{
		$$=new Node(nke_operand);
		$$->Name($2);
		Node *temp=new Node(nke_identifier);temp->Name($3);
		UpdateSymbolLinks(temp);
		$$->AddChild(temp);
	}
	;

/*
operand_type: 
	_src_ Integer_lit			{$$=new Node(nke_src);$$->Name(itoa($2,buf,10));}
	|
	_src_						{$$=new Node(nke_src);$$->Name("1");}
	|
	_dst_ Integer_lit			{$$=new Node(nke_dst);$$->Name(itoa($2,buf,10));}
	|
	_dst_						{$$=new Node(nke_dst);$$->Name("1");}
	|
	_mem_dst_ Integer_lit			{$$=new Node(nke_memdst);$$->Name(itoa($2,buf,10));}
	|
	_mem_dst_						{$$=new Node(nke_memdst);$$->Name("1");}	
	|
	_mem_src_ Integer_lit			{$$=new Node(nke_memsrc);$$->Name(itoa($2,buf,10));}
	|
	_mem_src_						{$$=new Node(nke_memsrc);$$->Name("1");}	
	;

*/

operand_semgroups: 
	operand_semgroups operand_semgroup		{$1->push_back($2);$$=$1;}
	|
	operand_semgroup						{$$=new NodeList(); $$->push_back($1);}
	;

operand_semgroup: 
	Identifier					{$$=new Node(nke_semgroup);$$->Name($1);}
	|
	'(' _circ_ Identifier ')'	{$$=new Node(nke_semgroup_circ);$$->Name($3);}
	;

vargroup_list: 
	vargroup_list vargroup		{$1->push_back($2);$$=$1;}
	|
	vargroup					{$$=new NodeList(); $$->push_back($1);}
	;


vargroup:
	'(' Identifier datatype regs ')'
								{
									$$=new Node(nke_var);
									$$->Name($2);
									$$->Symbol(st.Declare($2,$$));
									UpdateSymbolLinks($$);
									$$->AddChild($3);
									$$->AddChild($4);
								}
	|
	'(' Identifier regs ')'		{
									$$=new Node(nke_var);
									$$->Name($2);
									$$->Symbol(st.Declare($2,$$));
									UpdateSymbolLinks($$);
									$$->AddChild($3);
								}
	|
	'(' Identifier '(' reg_list ')' ')'	
								{
									$$=new Node(nke_var);
									$$->Name($2);
									$$->Symbol(st.Declare($2,$$));
									UpdateSymbolLinks($$);

									Node *n = new Node(nke_reg_list);
									n->Name("reg_list");
									n->Symbol(st.Declare("reg_list",n));
									UpdateSymbolLinks(n);
									n->AppendChildren($4);
									$$->AddChild(n);
								}
	;

Identifier_list:

datatype:
	'(' _datatype_ Identifier ')'
								{
									$$=new Node(nke_datatype);
									$$->Name($3);
									$$->Symbol(st.Declare($3,$$));
									UpdateSymbolLinks($$);
								}
	;

regs:
	'(' _regs_ reg_list ')'		{
									$$ = new Node(nke_reg_list);
									$$->Name("reg_list");
									$$->Symbol(st.Declare("reg_list",$$));
									UpdateSymbolLinks($$);
									$$->AppendChildren($3);
								}
	;

reg_list:
	reg							{
									$$=new NodeList(); 
									$$->push_back($1);
								}
	|
	reg_list reg				{
									$1->push_back($2);
									$$ = $1;
								}
	;


reg:
	Identifier					{
									$$=new Node(nke_regs);
									$$->Name($1);
									$$->Symbol(st.Declare($1,$$));
									UpdateSymbolLinks($$);
								}
	|
	Identifier '[' range_list ']' 
								{
									$$=new Node(nke_regs);
									$$->Name($1);
									$$->Symbol(st.Declare($1,$$));
									UpdateSymbolLinks($$); 
									$$->AppendChildren($3);
								}
	;

range_list:
	range_list range			{
									while(!$2->empty())
									{
										$$->push_back($2->front());
										$2->pop_front();
									}
								}
	|
	range						{
									$$ = $1;
								}
	;

range:
	Integer_lit					{	
									NodePtr n = new Node(nke_reg);
									n->Name(itoa($1, buf, 10));
									$$ = new NodeList();
									$$->push_back(n);
								}
	|
	Integer_lit '-' Integer_lit	{
									$$ = new NodeList();									
									for (int i=$1; i<=$3; i++)
									{
										NodePtr n = new Node(nke_reg);
										n->Name(itoa(i, buf, 10));
										$$->push_back(n);
									}
								}
	;


/******************************** Section 2. Instruction ******************************************/

Instruction_section: 
	word_len slot_list		{$$=new Node(nke_instruction_section);$$->AddChild($1);$$->AddChild($2);}
	|
	slot_list				{$$=new Node(nke_instruction_section);$$->AddChild($1);}
	|
	/* empty */				{$$=new Node(nke_instruction_section);}
	;

word_len: 
	'(' _wordlen_ Integer_lit ')'		{$$=new Node(nke_wordlen);$$->Name(itoa($3,buf,10));}
	;

slot_list: 
	'(' _slots_ s_list ')'	{$$=new Node(nke_slotlist);$$->AppendChildren($3);}
	;

s_list: 
	s_list slot		{$1->push_back($2);$$=$1;}
	|
	slot			{$$=new NodeList(); $$->push_back($1);}
	;

slot: 
	'(' instr_attr_list ')'		{$$=new Node(nke_slot);$$->AppendChildren($2);}
	;

instr_attr_list: 
	instr_attr					{$$=new NodeList(); $$->push_back($1);}
	|
	instr_attr_list instr_attr	{$1->push_back($2);$$=$1;}
	;

instr_attr: 
	'(' _type_ type_name_list ')'	{$$=new Node(nke_slottype);$$->AppendChildren($3);}
	|
	'(' _bitwidth_ Integer_lit ')'	{$$=new Node(nke_slotbitwidth);$$->Name(itoa($3,buf,10));}
	|
	'(' _unit_ Identifier ')'		{$$=new Node(nke_slotunit);$$->Name($3);}
	;

type_name_list: 
	type_name						{$$=new NodeList(); $$->push_back($1);}
	|
	type_name_list type_name		{$1->push_back($2);$$=$1;}
	;

type_name: 
	_data_							{$$=new Node(nke_dataslot);}
	|
	_control_						{$$=new Node(nke_controlslot);}
	|
	_flow_							{$$=new Node(nke_flowslot);}
	;

/******************************** Section 3. Operations Mapping ******************************************/

/* Tree mapping is for the new instruction selection.
This is an alternative to LBurg mapping which generates ISelMapping.txt */
OpMapping_section:
	opcode_mapping operand_mapping _tree_mappings_ tree_description _end_tree_mappings_
							{
								$$ = new Node(nke_opmapping_section);
								$$->AddChild($1);
								$$->AddChild($2);
							}
							|
	operand_mapping _tree_mappings_ tree_description _end_tree_mappings_
							{
								$$ = new Node(nke_opmapping_section);
								$$->AddChild($1);
							}
							|
	opcode_mapping _tree_mappings_ tree_description _end_tree_mappings_
							{
								$$ = new Node(nke_opmapping_section);
								$$->AddChild($1);
							}
							|
	_tree_mappings_ tree_description _end_tree_mappings_
							{
								$$ = new Node(nke_opmapping_section);
							}
							;

/* copying parse grammer from ISelparse.y */

tree_description:
      ruleList
	  {
	       printf("\tFinished Parsing the ISel input file...\n");
	  }
	  ;

ruleList:
      ruleList rule 
	  {
	  }
	  | rule
	  {
	  }
	  ;

rule:
      generics targets
	  {
	  }
	  |
	  lhs rhs
	  {
	  }
	  ;

generics:
      ISEL_LBRACE genericPart 
	  {
	  }
	  ;

targets:
      targetPart ISEL_RBRACE
	  {
	  }
	  ;

lhs:
      ISEL_LBRACE lhsPart 
	  {
	  }
	  ;

rhs:
      rhsPart ISEL_RBRACE
	  {
	  }
	  ;

genericPart:
      ISEL_LBRACE ISEL_GENERIC ISEL_LBRACE opGenList ISEL_RBRACE ISEL_RBRACE
	  {
           //$$ = $4;
	  }	  
	  ;

targetPart:
      ISEL_LBRACE ISEL_TARGET ISEL_LBRACE opTarList ISEL_RBRACE ISEL_RBRACE
	  {
	       //$$ = $4;
		   if (comb_index == 0)
				combinations[0] = -1;
	  }
	  ;

lhsPart:
      ISEL_LBRACE ISEL_LHS ISEL_LBRACE opGenList ISEL_RBRACE ISEL_RBRACE
	  {
           //$$ = $4;
	  }	  
	  ;

rhsPart:
      ISEL_LBRACE ISEL_RHS ISEL_LBRACE opTarList ISEL_RBRACE ISEL_RBRACE
	  {
	       //$$ = $4;
	  }
	  ;

opGenList:
      opGenList Op 
	  {
		   comb_detect++;
		   tree_opcode_type[count_gen-1]=strdup("GEN");
	  }
	  | Op
	  {
		   comb_detect++;
		   tree_opcode_type[count_gen-1]=strdup("GEN");
	  }
	  ;

opTarList:
      opTarList Op 
	  {
		   if (comb_detect > 1)
		   {
				// record the target opcode position in tree mapping table
				// denote end of list with -1
				combinations[comb_index++] = count_gen-1;
				combinations[comb_index] = -1;
		   }
		   comb_detect=0;
		   tree_opcode_type[count_gen-1]=strdup("TAR");
	  }
	  | Op
	  {
		   if (comb_detect > 1)
		   {
				// record the target opcode position in tree mapping table
				// denote end of list with -1
				combinations[comb_index++] = count_gen-1;
				combinations[comb_index] = -1;
		   }
		   comb_detect=0;
		   tree_opcode_type[count_gen-1]=strdup("TAR");
	  }
	  ;


Op:
      ISEL_LBRACE ISEL_OPCODE dstList srcList ISEL_RBRACE
	  {

		  if (strlen($2)) {
			tree_opcode[count_gen++] = strdup($2);
			}
			$$ = tree_opcode[count_gen-1];
	  }
	  ;


dstList:
      dstList dstNum ISEL_EQUAL argType argIdent 
	  {
		  tree_dstlist[count_gen] = strcat(tree_dstlist[count_gen]," ");
		  tree_dstlist[count_gen] = strcat(tree_dstlist[count_gen],$4);
		  tree_dstlist[count_gen] = strcat(tree_dstlist[count_gen],$5);

	  }
	  | dstNum ISEL_EQUAL argType argIdent
	  {
		  char * tmpstore2 = new char[100];
		  tmpstore2 = strcpy (tmpstore2,"");
		  tmpstore2 = strcat(tmpstore2,$3);
		  tmpstore2 = strcat(tmpstore2,$4);
		  tree_dstlist[count_gen] = tmpstore2;
	  }
	  |
	  {
		  tree_dstlist[count_gen] = NULL;
	  }
	  ;

srcList:
      srcList srcNum ISEL_EQUAL argType argIdent
	  {
		  tree_srclist[count_gen] = strcat(tree_srclist[count_gen]," ");
		  tree_srclist[count_gen] = strcat(tree_srclist[count_gen],$4);
		  tree_srclist[count_gen] = strcat(tree_srclist[count_gen],$5);

	  }
	  | srcNum ISEL_EQUAL argType argIdent
	  {

		  char * tmpstore = new char[100];
		  tmpstore = strcpy (tmpstore,"");
		  tmpstore = strcat(tmpstore,$3);
		  tmpstore = strcat(tmpstore,$4);
		  tree_srclist[count_gen] = tmpstore;
	  }
	  |
	  {
		  tree_srclist[count_gen] = NULL;
	  }
	  ;

dstNum:
      ISEL_DST ISEL_LPAREN val ISEL_RPAREN 
	  {
	       $$ = $3;
	  }
	  ;
srcNum:
      ISEL_SRC ISEL_LPAREN val ISEL_RPAREN
	  {
	       $$ = $3;
	  }
	  ;
val:
      ISEL_NUM
	  {
		   $$ = $1;
	  }
	  ;

argType:
      ISEL_TYPE 
      {
		  $$ = $1;
	  }	  
	  ;

argIdent:
      ISEL_LBRACE ISEL_NUM ISEL_RBRACE
	  {
			char * tmpst = new char[100];
			tmpst = strcpy(tmpst,"(");
			tmpst = strcat(tmpst,itoa($2, buf, 10));
			tmpst = strcat(tmpst,")");
			$$ = tmpst; 
	  }
	  ;

/* end copying */

opcode_mapping:
	'(' _opcode_mapping_ opcode_map_list ')'
							{
								$$ = new Node(nke_opcode_mapping);
								$$->AppendChildren($3);
							}
	;

operand_mapping:
	'(' _operand_mapping_ operand_map_list ')'		
							{
								$$ = new Node(nke_operand_mapping);
								$$->AppendChildren($3);
							}
	;

opcode_map_list: 
	opcode_map_list opcode_map	
							{
								$1->push_back($2);
								$$=$1;
							}
	|
	opcode_map				{
								$$=new NodeList(); 
								$$->push_back($1);
							}
	;

opcode_map: 
	'(' _op_mapping_ pred_dr_ops_list ')'	
							{
								$$=new Node(nke_opcode_map); 
								$$->AppendChildren($3);
							}
	;

pred_dr_ops_list: 
	pred_dr_ops						{$$=new NodeList(); $$->push_back($1);}
	|
	pred_dr_ops_list pred_dr_ops	{$1->push_back($2);$$=$1;}
	;

pred_dr_ops: 
	'(' _predicate_ pred_list oper_seq ')'	{$$=new Node(nke_predoperseq);$$->AppendChildren($3);$$->AddChild($4);}
	|
	oper_seq								{$$=$1;}
	;

pred_list: 
	pred					{$$=new NodeList(); $$->push_back($1);}
	|
	pred_list pred			{$1->push_back($2);$$=$1;}
	;

pred: 
	'(' Identifier mapvar_id ')'	{$$=new Node(nke_predicate);$$->Name($2);$$->AddChild($3);}
	|
	_true_							{$$=new Node(nke_predicate);$$->Name("TRUE");}
	;

oper_seq: 
	'(' _generic_ operlist ')'		{$$=new Node(nke_generic);$$->AppendChildren($3);}
	|
	'(' _target_ operlist ')'		{$$=new Node(nke_target);$$->AppendChildren($3);UpdateSymbolsLinks($3);}
	|
	'(' _target_risa_ operlist ')'			{$$=new Node(nke_target_risa);$$->AppendChildren($3);UpdateSymbolsLinks($3);}
	;


operlist: 
	operlist oper				{$1->push_back($2);$$=$1;}
	|
	oper						{$$=new NodeList(); $$->push_back($1);}
	;
oper: 
	'(' Identifier mapvar_list ')'	{$$=new Node(nke_operation);$$->Name($2);$$->AppendChildren($3);}
	;

mapvar_list: 
	/* nothing */				{$$ = new NodeList();}
	|
	mapvar						{$$=new NodeList(); $$->push_back($1);}
	|
	mapvar_list mapvar			{$1->push_back($2);$$=$1;}
	;

mapvar: 
	mapvar_id						{$$=$1;}
	|
	'(' Identifier mapvar_id ')'	{$$=new Node(nke_fcall);$$->Name($2);$$->AddChild($3);}
	;

mapvar_id: 
	Identifier			{$$=new Node(nke_mapvar);$$->Name($1);}
	|
	Integer_lit			{$$=new Node(nke_mapvar);$$->Name(itoa($1,buf,10));}
	|
	_dst_				{$$=new Node(nke_mapvar);$$->Name("DST");}
	;


operand_map_list:
	operand_map_list operand_map
						{
							$1->push_back($2);
							$$ = $1;
						}
	|
	operand_map			{
							$$ = new NodeList();
							$$->push_back($1);
						}
	;

operand_map:
	'(' _op_mapping_ generic_operand target_operand ')'
						{
							$$ = new Node(nke_operand_map);
							$$->AddChild($3);
							$$->AddChild($4);
						}
	;

generic_operand:
	'(' _generic_ data_type class_type ')'
						{
							$$ = new Node(nke_generic);
							$$->AddChild($3);
							$$->AddChild($4);
						}
	;

target_operand:
	'(' _target_ Identifier ')'
						{
							$$ = new Node(nke_target);
							$$->Name($3);
							UpdateSymbolLinks($$);
						}
	;
	
data_type: 
	'(' _datatype_ Identifier ')'
						{
							$$ = new Node(nke_datatype);
							$$->Name($3);
						}
	;

class_type:
	'(' _classtype_ Identifier ')'
						{
							$$ = new Node(nke_classtype);
							$$->Name($3);
						}
	;


/******************************** Section 4. Architecture ******************************************/

ArchComponent_section: 
	subtype_decls compinst_list			{$$=new Node(nke_architecture_section);$$->AppendChildren($1);$$->AppendChildren($2);}
	|
	/* empty */							{$$=new Node(nke_architecture_section);}
	;

subtype_decls: 
	subtype_decl					{$$=new NodeList(); $$->push_back($1);}
	|
	subtype_decls subtype_decl		{$1->push_back($2);$$=$1;}
	;

subtype_decl: 
	'(' _subtype_ base_type subtype_list ')'
										{
											NodePtr n=new Node(nke_subtypedecl);n->Name($3);
											n->Symbol(st.Declare($3,n));
											$$=n;$$->AppendChildren($4);
										}
	;


subtype_list: 
	subtype_list Identifier				{
											NodePtr n=new Node(nke_subtype);n->Name($2);
											n->Symbol(st.Declare($2,n));
											$1->push_back(n);$$=$1;
										}
	|
	Identifier							{
											NodePtr n=new Node(nke_subtype);n->Name($1);
											n->Symbol(st.Declare($1,n));
											$$=new NodeList(); 
											$$->push_back(n);
										}
	;


base_type: 
	_unit_				{$$="UNIT";}
	|	
	_latch_				{$$="LATCH";}
	|
	_storage_			{$$="STORAGE";}
	|
	_connection_		{$$="CONNECTION";}
	|
	_port_				{$$="PORT";}
	;


compinst_list: 
	compinst_list compinst	{$1->push_back($2);$$=$1;}
	|
	compinst				{$$=new NodeList(); $$->push_back($1);}
	;

compinst: 
	/*'(' Identifier 
		component_list 
		comp_attributes ')'		{$$=new Node(nke_compinst);$$->Name($2);UpdateSymbolLinks($$);$$->AppendChildren($3);$$->AppendChildren($4);}
	|
	*/
	'(' Identifier /*subtype name*/ 
		component_list /*list of compontents*/
		')'		{$$=new Node(nke_compinst);$$->Name($2);UpdateSymbolLinks($$);$$->AppendChildren($3);}
	;

component_list: 
	component_list one_comp				{$1->push_back($2);$$=$1;}
	|
	one_comp							{$$=new NodeList(); $$->push_back($1);}
	;

one_comp:
	Identifier	comp_initstr comp_attributes
										{
											NodePtr n=new Node(nke_component);n->Name($1);
											n->Symbol(st.Declare($1,n)); $$=n;UpdateSymbolLinks($$);
											$$->AddChild($2);$$->AppendChildren($3);
										}
	|
	Identifier	comp_initstr 
										{
											NodePtr n=new Node(nke_component);n->Name($1);
											n->Symbol(st.Declare($1,n)); $$=n;UpdateSymbolLinks($$);
											$$->AddChild($2);
										}
	|
	Identifier	comp_attributes			{
											NodePtr n=new Node(nke_component);n->Name($1);
											n->Symbol(st.Declare($1,n)); $$=n;UpdateSymbolLinks($$);
											$$->AppendChildren($2);
										}
	|
	Identifier							{
											NodePtr n=new Node(nke_component);n->Name($1);
											n->Symbol(st.Declare($1,n)); $$=n;UpdateSymbolLinks($$);
										}
	;

comp_initstr:
	'('_char_string_')'									
										{NodePtr n=new Node(nke_initstring);n->Name($2);$$=n;}	
	;


comp_attributes: 
	comp_attributes comp_attribute	{$1->push_back($2);$$=$1;}
	|
	comp_attributes timings		{$1->splice($1->begin(),*$2);$$=$1;}
	|
	comp_attribute					{$$=new NodeList(); $$->push_back($1);}
	|
	timings							{$$=$1;}
	;

comp_attribute:
	sub_comps			{$$=$1;}
	|
	latches				{$$=$1;}
	|
	ports				{$$=$1;}
	|
	connections			{$$=$1;}
	|
	opcodes				{$$=$1;}
	|
	capacity			{$$=$1;}
	|
	argument			{$$=$1;}
	|
	feedback_enq		{$$=$1;}
	|
	feedback_deq		{$$=$1;}
	|
	queue_enq			{$$=$1;}
	|
	queue_deq			{$$=$1;}
	|
	opcodes_enq			{$$=$1;}
	|
	opcodes_deq			{$$=$1;}
	| 
	instrin				{$$=$1;}
	|
	instrout			{$$=$1;}
	;

sub_comps: 
	'(' _subcomponents_ subcomp_list ')'	{$$=new Node(nke_subcomps);$$->AppendChildren($3);}
	;

subcomp_list: 
	subcomp_list Identifier				{
											NodePtr n=new Node(nke_subcomp);n->Name($2);
											$1->push_back(n);$$=$1;
										}
	|
	Identifier							{
											NodePtr n=new Node(nke_subcomp);n->Name($1);
											$$=new NodeList(); 
											$$->push_back(n);
										}
	;

latches: 
	'(' _latches_ aggregate_latchlist ')'	{$$=new Node(nke_latches);$$->AppendChildren($3);}
	|
	'(' _latches_ _none_ ')'				{$$=new Node(nke_latches);}
	;

aggregate_latchlist:
	aggregate_latchlist '(' _input_ latch_list ')'	{NodePtr n=new Node(nke_in_latches);n->AppendChildren($4);
													 $$->push_back(n);}
	|
	aggregate_latchlist '(' _output_ latch_list ')'	{NodePtr n=new Node(nke_out_latches);n->AppendChildren($4);
													 $$->push_back(n);}
	|
	aggregate_latchlist '(' _other_ latch_list ')'	{NodePtr n=new Node(nke_other_latches);n->AppendChildren($4);
													 $$->push_back(n);}
	|
											{
												$$=new NodeList(); 
											}
	;

	
latch_list: 
	latch_list Identifier				{
											NodePtr n=new Node(nke_latch);n->Name($2);
											$1->push_back(n);$$=$1;
										}
	|
	Identifier							{
											NodePtr n=new Node(nke_latch);n->Name($1);
											$$=new NodeList(); 
											$$->push_back(n);
										}
	;

ports: 
	'(' _ports_ port_list ')'				{$$=new Node(nke_ports);$$->AppendChildren($3);}
	|
	'(' _ports_ _none_ ')'					{$$=new Node(nke_ports);}
	;


port_list: 
	port_list Identifier			{
											NodePtr n=new Node(nke_port);n->Name($2);
											$1->push_back(n);$$=$1;
										}
	|
	Identifier							{
											NodePtr n=new Node(nke_port);n->Name($1);
											$$=new NodeList(); 
											$$->push_back(n);
										}
	;

feedback_enq: 
	'(' _feedback_enq_ feedbacks_list ')'			{$$=new Node(nke_feedback_enq);$$->AppendChildren($3);}
	|
	'(' _feedback_enq_ _none_ ')'					{$$=new Node(nke_feedback_enq);}
	;


feedback_deq: 
	'(' _feedback_deq_ feedbacks_list ')'			{$$=new Node(nke_feedback_deq);$$->AppendChildren($3);}
	|
	'(' _feedback_deq_ _none_ ')'					{$$=new Node(nke_feedback_deq);}
	;


feedbacks_list: 
	feedbacks_list Identifier			{
											NodePtr n=new Node(nke_feedback);n->Name($2);
											$1->push_back(n);$$=$1;
										}
	|
	Identifier							{
											NodePtr n=new Node(nke_feedback);n->Name($1);
											$$=new NodeList(); 
											$$->push_back(n);
										}
	;


queue_enq: 
	'(' _queues_enq_ queue_enq_list ')'			{$$=new Node(nke_queues_enq);$$->AppendChildren($3);}
	|
	'(' _queues_enq_ _none_ ')'					{$$=new Node(nke_queues_enq);}
	;


queue_enq_list: 
	queue_enq_list Identifier			{
											NodePtr n=new Node(nke_queue_enq);n->Name($2);
											UpdateSymbolLinks(n);
											$1->push_back(n);$$=$1;
										}
	|
	Identifier							{
											NodePtr n=new Node(nke_queue_enq);n->Name($1);
											UpdateSymbolLinks(n);
											$$=new NodeList(); 
											$$->push_back(n);
										}
	;


queue_deq: 
	'(' _queues_deq_ queue_deq_list ')'			{$$=new Node(nke_queues_deq);$$->AppendChildren($3);}
	|
	'(' _queues_deq_ _none_ ')'					{$$=new Node(nke_queues_deq);}
	;


queue_deq_list: 
	queue_deq_list Identifier			{
											NodePtr n=new Node(nke_queue_deq);n->Name($2);
											UpdateSymbolLinks(n);
											$1->push_back(n);$$=$1;
										}
	|
	Identifier							{
											NodePtr n=new Node(nke_queue_deq);n->Name($1);
											UpdateSymbolLinks(n);
											$$=new NodeList(); 
											$$->push_back(n);
										}
	;


connections: 
	'(' _connections_ connection_list ')'	{$$=new Node(nke_connections);$$->AppendChildren($3);}
	|
	'(' _connections_ _none_ ')'			{$$=new Node(nke_connections);}
	;

connection_list: 
	connection_list Identifier			{
											NodePtr n=new Node(nke_connection);n->Name($2);
											$1->push_back(n);$$=$1;
										}
	|
	Identifier							{
											NodePtr n=new Node(nke_connection);n->Name($1);
											$$=new NodeList(); 
											$$->push_back(n);
										}
	;


opcodes: 
	'(' _opcodes_ Identifier_list ')'		{$$=new Node(nke_opcodes);$$->AppendChildren($3);UpdateSymbolsLinks($3);}
	|
	'(' _opcodes_ _all_ ')'					{$$=new Node(nke_opcodes);}
	;



opcodes_enq: 
	'(' _opcodes_enq_ Identifier_list ')'		{$$=new Node(nke_opcodes_enq);$$->AppendChildren($3);UpdateSymbolsLinks($3);}
	|
	'(' _opcodes_enq_ _all_ ')'					{$$=new Node(nke_opcodes_enq);}
	;

opcodes_deq: 
	'(' _opcodes_deq_ Identifier_list ')'		{$$=new Node(nke_opcodes_deq);$$->AppendChildren($3);UpdateSymbolsLinks($3);}
	|
	'(' _opcodes_deq_ _all_ ')'					{$$=new Node(nke_opcodes_deq);}
	;



/*timing: 
	'(' _timing_ Identifier_list cyclenum_list ')'		{$$=new Node(nke_timing);$$->AppendChildren($3);$$->AppendChildren($4);}
	|
	'(' _timing_ _all_ cyclenum_list ')'				{$$=new Node(nke_timing);$$->AppendChildren($4);}
	;
*/

timings: 
	'(' _timing_ cyclenum_list ')'		{$$=$3;}
	;

cyclenum_list: 
	cyclenum							{
											$$=new NodeList(); 
											$$->push_back($1);
										}
	|
	cyclenum_list cyclenum				{
											$1->push_back($2);
											$$=$1;
										}
	;

cyclenum:
	'(' Identifier_list Integer_lit ')'	{
											$$=new Node(nke_timing);
											NodePtr n=new Node(nke_integer);n->Name(itoa($3,buf,10));
											$$->AppendChildren($2);
											UpdateSymbolsLinks($2);
											$$->AddChild(n); 
										}
	|
	'(' _all_ Integer_lit ')'			{
											$$=new Node(nke_timing);
											NodePtr n=new Node(nke_integer);n->Name(itoa($3,buf,10));
											NodePtr a=new Node(nke_all);a->Name("ALL");
											$$->AddChild(a);
											$$->AddChild(n); 
										}
	;

Integer_lit_list: 
	Integer_lit							{
											NodePtr n=new Node(nke_integer);n->Name(itoa($1,buf,10));
											$$=new NodeList(); 
											$$->push_back(n);
										}
	|
	Integer_lit_list Integer_lit		{
											NodePtr n=new Node(nke_integer);n->Name(itoa($2,buf,10));
											$1->push_back(n);$$=$1;
										}
	;

capacity: 
	'(' _capacity_ Integer_lit ')'		{
											$$=new Node(nke_capacity);
											$$->Name(itoa($3, buf, 10));
											}
	;

instrin: 
	'(' _instrin_ Integer_lit ')'		{
											$$=new Node(nke_instrin);
											$$->Name(itoa($3, buf, 10));
											}
	;

instrout: 
	'(' _instrout_ Integer_lit ')'		{
											$$=new Node(nke_instrout);
											$$->Name(itoa($3, buf, 10));
											}
	;

argument: 
	'(' _argument_ Identifier ')'			{$$=new Node(nke_argument);$$->Name($3);}
	;


/******************************** Section 5. Pipeline ******************************************/

PipeDTPaths_section: 
	pipeline_info_list '(' _dtpaths_ dtpath_list ')'		{$$=new Node(nke_pipeline_section);$$->AppendChildren($1);$$->AppendChildren($4);}
	|
	/* empty */												{$$=new Node(nke_pipeline_section);}
	;

pipeline_info_list: 
	pipeline_info							{$$=new NodeList(); $$->push_back($1);}
	|
	pipeline_info_list pipeline_info		{$1->push_back($2);$$=$1;}
	;

pipeline_info: 
	pipeline							{$$=$1;}
	|
	'(' Identifier pipeline ')'			{$$=new Node(nke_pipelineinfo);$$->Name($2);st.Declare($2,$$);$$->AddChild($3);}
	|
	'(' Identifier parallel ')'			{$$=new Node(nke_pipelineinfo);$$->Name($2);st.Declare($2,$$);$$->AddChild($3);}
	|
	'(' Identifier alternate ')'		{$$=new Node(nke_pipelineinfo);$$->Name($2);st.Declare($2,$$);$$->AddChild($3);}
	;

pipeline: 
	'(' _pipeline_ Identifier_list ')'	{$$=new Node(nke_pipeline);$$->AppendChildren($3);}
	;

parallel: 
	'(' _parallel_ Identifier_list ')'	{$$=new Node(nke_parallel);$$->AppendChildren($3);}
	;

alternate: 
	'(' _alternate_ Identifier_list ')'	{$$=new Node(nke_alternate);$$->AppendChildren($3);}
	;


dtpath_list: 
	dtpath_list dtpaths						{$1->push_back($2);$$=$1;}
	|
	dtpaths									{$$=new NodeList(); $$->push_back($1);}
	;

dtpaths: 
	'(' _type_ t_type dt_list ')'			{$$=new Node(nke_dtpaths);$$->Name($3);$$->AppendChildren($4);}
	;

t_type: 
	_uni_				{$$="UNI";}
	|
	_bi_				{$$="BI";}
	;

dt_list: 
	dt_list dt			{$1->push_back($2);$$=$1;}
	|
	dt					{$$=new NodeList(); $$->push_back($1);}
	;

dt: 
	'(' from to dtcomponent_list ')'	{$$=new Node(nke_dtpath);$$->AddChild($2);$$->AddChild($3);$$->AppendChildren($4);}
	;

from:
	Identifier	{$$=new Node(nke_from);$$->Name($1);UpdateSymbolLinks($$);}
	;

to:
	Identifier	{$$=new Node(nke_to);$$->Name($1);UpdateSymbolLinks($$);}
	;


dtcomponent_list: 
	dtcomponent_list Identifier			{
											NodePtr n=new Node(nke_dtcomponent);n->Name($2);
											UpdateSymbolLinks(n);
											$1->push_back(n);$$=$1;
										}
	|
	Identifier							{
											NodePtr n=new Node(nke_dtcomponent);n->Name($1);
											UpdateSymbolLinks(n);
											$$=new NodeList(); 
											$$->push_back(n);
										}
	;


/******************************** Section 6. Storage ******************************************/

Storage_section:		
	/* empty */					{$$=new Node(nke_storage_section);}
	|
	storage_list				{$$=new Node(nke_storage_section);$$->AppendChildren($1);}
	;

storage_list: 
	storage						{$$=new NodeList(); $$->push_back($1);}
	|
	storage_list storage		{$1->push_back($2);$$=$1;}
	;

storage: 
	'(' Identifier storageattr_list ')'		{$$=new Node(nke_storage);$$->Name($2);UpdateSymbolLinks($$);$$->AppendChildren($3);}
	|
	'(' Identifier ')'						{$$=new Node(nke_storage);$$->Name($2);UpdateSymbolLinks($$);}
	;

storageattr_list: 
	storageattr							{$$=new NodeList(); $$->push_back($1);}
	|
	storageattr_list storageattr		{$1->push_back($2);$$=$1;}
	;

storageattr: 
	'(' _type_ Identifier ')'			{$$=new Node(nke_storagetype);$$->Name($3);}
	|
	'(' _mnemonic_ _char_string_ ')'		{$$=new Node(nke_mnemonic);$$->Name($3);}
	|
	'(' _size_ Integer_lit ')'			{$$=new Node(nke_storagesize);$$->Name(itoa($3,buf,10));}
	|
	'(' _width_ Integer_lit ')'			{$$=new Node(nke_storagewidth);$$->Name(itoa($3,buf,10));}
	|
	'(' _accesstimes_ Integer_lit ')'	{$$=new Node(nke_accesstimes);$$->Name(itoa($3,buf,10));}
	|
	'(' _accessports_ accessport_list ')'	{$$=new Node(nke_accessports);$$->AppendChildren($3);}
	|
	'(' _addrrange_ addrrange_list ')'	{$$=new Node(nke_addrranges);$$->AppendChildren($3);}
	|
	'(' _write_policy_ write_policy_options ')'	{$$=new Node(nke_writepolicy);$$->AddChild($3);}
	|
	'(' _replacement_policy_ replacement_policy_options ')'	{$$=new Node(nke_replacementpolicy);$$->AddChild($3);}
	|
	'(' _wordsize_ Integer_lit ')'			{$$=new Node(nke_wordsize);$$->Name(itoa($3,buf,10));}
	|
	'(' _linesize_ Integer_lit ')'			{$$=new Node(nke_linesize);$$->Name(itoa($3,buf,10));}
	|
	'(' _num_lines_ Integer_lit ')'			{$$=new Node(nke_numlines);$$->Name(itoa($3,buf,10));}
	|
	'(' _associativity_ Integer_lit ')'			{$$=new Node(nke_associativity);$$->Name(itoa($3,buf,10));}
	|
	'(' _level_ Integer_lit ')'			{$$=new Node(nke_level);$$->Name(itoa($3,buf,10));}
	|
	'(' _next_level_ Integer_lit ')'			{$$=new Node(nke_next_level);$$->Name(itoa($3,buf,10));}
	|
	'(' _prev_level_ Integer_lit ')'			{$$=new Node(nke_prev_level);$$->Name(itoa($3,buf,10));}
	|
	'(' _storage_connections_ stconnection_list ')'		{$$=new Node(nke_stconnections);$$->Name("st_con"); $$->AppendChildren($3);}
	|
	'(' _read_latency_ Integer_lit ')'			{$$=new Node(nke_read_latency);$$->Name(itoa($3,buf,10));}
	|
	'(' _write_latency_ Integer_lit ')'			{$$=new Node(nke_write_latency);$$->Name(itoa($3,buf,10));}
	|
	'(' _num_banks_ Integer_lit ')'			{$$=new Node(nke_numbanks);$$->Name(itoa($3,buf,10));}
	|
	'(' _access_mode_ Integer_lit ')'			{$$=new Node(nke_access_mode);$$->Name(itoa($3,buf,10));}
	|
	'(' _num_parallel_read_ Integer_lit ')'			{$$=new Node(nke_num_par_read);$$->Name(itoa($3,buf,10));}
	|
	'(' _num_parallel_write_ Integer_lit ')'			{$$=new Node(nke_num_par_write);$$->Name(itoa($3,buf,10));}
	|
	'(' _read_write_conflict_ Integer_lit ')'			{$$=new Node(nke_rdwr_conflict);$$->Name(itoa($3,buf,10));}
	;

accessport_list: 
	accessport						{$$=new NodeList(); $$->push_back($1);}
	|
	accessport_list accessport		{$1->push_back($2);$$=$1;}
	;

accessport: 
	'(' Identifier ')'											{$$=new Node(nke_accessport);$$->Name($2);UpdateSymbolLinks($$);}
	|
	'(' Identifier '(' _accesswidths_ Integer_lit_list ')' ')'	{$$=new Node(nke_accessport);$$->Name($2);UpdateSymbolLinks($$);$$->AppendChildren($5);}
	;

addrrange_list: 
	addrrange						{$$=new NodeList(); $$->push_back($1);}
	|
	addrrange_list addrrange		{$1->push_back($2);$$=$1;}
	;

addrrange: 
	'(' integer integer ')'									{$$=new Node(nke_addrrange);$$->AddChild($2);$$->AddChild($3);}
	|
	'(' integer integer '(' _stride_ integer ')' ')'	{$$=new Node(nke_addrrange);$$->AddChild($2);$$->AddChild($3);$$->AddChild($6);}
	;

stconnection_list: 
	stconnection						{$$=new NodeList(); $$->push_back($1);}
	|
	stconnection_list stconnection		{$1->push_back($2);$$=$1;}
	;

stconnection: 
	'(' Identifier Identifier ')'		
	{	$$=new Node(nke_stconnection);
		$$->Name($2);
		Node *temp=new Node(nke_identifier);temp->Name($3);
		UpdateSymbolLinks(temp);
		$$->AddChild(temp);
	}
	;

write_policy_options:
	_write_back_					{$$=new Node(nke_write_back);}
	|	
	_write_through_					{$$=new Node(nke_write_through);}
	;

replacement_policy_options:
	_lru_					{$$=new Node(nke_lru);}
	|	
	_fifo_					{$$=new Node(nke_fifo);}
	;

integer:
	Integer_lit		{$$=new Node(nke_integer);$$->Name(itoa($1,buf,10));}
	;

%%

//declare the symbol references and their symbol table entries
void DeclareSymbols(NodeListPtr list){
	NodeList::iterator i;
	SymbolPtr sym;
	for(i=list->begin();i!=list->end();i++){
		sym=st.Declare((*i)->Name(),*i);
		(*i)->Symbol(sym);
	}
}

//update the linkes between the symbol references and their symbol table entries
void UpdateSymbolsLinks(NodeListPtr list){
	NodeList::iterator i;
	for(i=list->begin();i!=list->end();i++){
		UpdateSymbolLinks(*i);
	}
}

//update the links between the symbol references and their symbol table entries
void UpdateSymbolLinks(NodePtr node){
	SymbolPtr sym;
	sym=st.GetSymbol(node->Name());
	node->Symbol(sym);
	//the next line is adding the list of references to the symbol. 
	sym->AddUse(node);
}


/* This function gives you a string image of a token. */

char * token_image(token t)
{
    return (char *)yytname[YYTRANSLATE(t)];
}
