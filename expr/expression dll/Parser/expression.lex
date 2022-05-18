/********************************************/
/* file: expression.lex						*/
/* created: Nov. 30, 1998					*/
/* authors: pgrun							*/
/* last modified:							*/
/* contents: flex scanner for EXPRESSION	*/
/********************************************/

DIGIT	[0-9]
LETTER	[A-Za-z]
ALFA	{LETTER}|{DIGIT}
digit		[0-9]
alpha		[a-zA-Z_]
alphanum	[a-zA-Z_0-9]

%{

#include "parser.h"
#include <ctype.h>
#include <string.h>
#include <io.h>
#include <stdio.h>
#include <iostream.h>


int ISelLexDebug = 0;

#define yywrap() 1

#define isatty _isatty

char *copy(char *);
char * strip_quotes_and_copy(char *s,int len);
FILE * fpsel;

//Controls some debug prints in the lexer
#define PARSER_DEBUG 0

%}

%s TREESTART

%%

\(TREE\_MAPPING								{
												BEGIN TREESTART;
												/* write into a file called ISelMapping.txt */
												fpsel = fopen("./ISelMapping.txt", "w");
												if (fpsel == NULL)
												{
													printf("Could not open ISelMapping.txt file\n");
												}

												if(PARSER_DEBUG) printf("%s\n",yytext);
												fprintf(fpsel, "%s", yytext);
												return _tree_mappings_;
											}

<TREESTART>END\_TREE\_MAPPING\)				{
												BEGIN 0;
												if(PARSER_DEBUG) printf("%s\n",yytext);
												fprintf(fpsel, "%s", yytext);
												fclose(fpsel);
												return _end_tree_mappings_;
											}

<TREESTART>GENERIC							{	
												if(PARSER_DEBUG) printf("%s\n",yytext);
												fprintf(fpsel, "%s", yytext);	
												return ISEL_GENERIC; 
											}

<TREESTART>TARGET							{	
												if(PARSER_DEBUG) printf("%s\n",yytext);
												fprintf(fpsel, "%s", yytext);
												return ISEL_TARGET; 
											}

<TREESTART>LHS								{	
												if(PARSER_DEBUG) printf("%s\n",yytext);
												fprintf(fpsel, "%s", yytext);
												return ISEL_LHS; 
											}

<TREESTART>DST								{	
												if(PARSER_DEBUG) printf("%s\n",yytext);
												fprintf(fpsel, "%s", yytext);
												return ISEL_DST; 
											}

<TREESTART>SRC								{
												if(PARSER_DEBUG) printf("%s\n",yytext);
												fprintf(fpsel, "%s", yytext);
												return ISEL_SRC; 
											}

<TREESTART>REG|IMM|LAB						{
												if(PARSER_DEBUG) printf("%s\n",yytext);
												fprintf(fpsel, "%s", yytext);		
												yylval.st=copy(yytext);
												 return ISEL_TYPE;
											}

<TREESTART>{alpha}{alphanum}*				{ 
												if(PARSER_DEBUG) printf("%s\n",yytext);
												fprintf(fpsel, "%s", yytext);		
												yylval.st=copy(yytext);
												return ISEL_OPCODE;
											}

<TREESTART>-?{digit}+						{ 
												if(PARSER_DEBUG) printf("%s\n",yytext);
												fprintf(fpsel, "%s", yytext);
												yylval.in = atoi(yytext);
												return ISEL_NUM;
											}

<TREESTART>"("								{  
												if(PARSER_DEBUG) printf("%s\n",yytext);
												fprintf(fpsel, "%s", yytext);
												return ISEL_LBRACE;
											}

<TREESTART>")"								{ 
												if(PARSER_DEBUG) printf("%s\n",yytext); 
												fprintf(fpsel, "%s", yytext);
												return ISEL_RBRACE;
											}

<TREESTART>"="								{  
												if(PARSER_DEBUG) printf("%s\n",yytext);
												fprintf(fpsel, "%s", yytext);
												return ISEL_EQUAL;
											}

<TREESTART>"\["								{  
												if(PARSER_DEBUG) printf("%s\n",yytext);
												fprintf(fpsel, "%s", yytext);
												return ISEL_LPAREN;
											}

<TREESTART>"\]"								{  
												if(PARSER_DEBUG) printf("%s\n",yytext);
												fprintf(fpsel, "%s", yytext);
												return ISEL_RPAREN;
											}

<TREESTART>":"								{ 
												if(PARSER_DEBUG) printf("%s\n",yytext);
												fprintf(fpsel, "%s", yytext);
												return ISEL_COLON; 
											}

<TREESTART>\;.*								{
												if(PARSER_DEBUG) printf("%s\n",yytext);
												fprintf(fpsel, "%s", yytext);
												//increment_line_number();
											}

<TREESTART>\;\*(\;|[^;*]|(\*)+[^;*])*(\*)+\;	{
												// because of these comments, line nos
												// are incorrectly assigned
												if(PARSER_DEBUG) printf("%s\n",yytext);
												fprintf(fpsel, "%s", yytext);
												}

<TREESTART>"+"								{
												if(PARSER_DEBUG) printf("%s\n",yytext);
												fprintf(fpsel, "%s", yytext);
												return ISEL_ADD_OPERATOR; 
											}

<TREESTART>"-"								{
												if(PARSER_DEBUG) printf("%s\n",yytext);
												fprintf(fpsel, "%s", yytext);
												return ISEL_SUB_OPERATOR; 
											}

<TREESTART>[\t ]							{
												if(PARSER_DEBUG) printf("%s\n",yytext);						
												fprintf(fpsel, "%s", yytext);
											}

<TREESTART>\n								{
												if(PARSER_DEBUG) printf("%s\n",yytext);
												fprintf(fpsel, "%s", yytext);
												increment_line_number();
											}

<TREESTART>.								{
												if(PARSER_DEBUG) printf("%s\n",yytext);
												fprintf(fpsel, "%s", yytext);
												//increment_line_number();
											}


OP\_GROUP		return 	_opgroup_;
OPCODE			return 	_opcode_;
OP\_TYPE		return 	_optype_;
OPERANDS		return 	_operands_;	
BEHAVIOR		return 	_behavior_;	
DATA\_OP		return 	_dataop_;	
CONTROL\_OP		return 	_controlop_;	
FLOW\_OP		return 	_flowop_;	
SRC				return 	_src_;	
DST				return 	_dst_;	
MEMDST			return  _mem_dst_;
MEMSRC			return  _mem_src_;
VAR\_GROUPS		return 	_vargroups_;
DATATYPE		return  _datatype_;	
CLASSTYPE		return  _classtype_;	
WORDLEN			return 	_wordlen_;	
SLOTS			return 	_slots_;	
TYPE			return 	_type_;	
DATA			return 	_data_;	
CONTROL			return 	_control_;	
FLOW			return 	_flow_;	
BITWIDTH		return 	_bitwidth_;	
OPCODE_MAPPING  return  _opcode_mapping_;
OPERAND_MAPPING return  _operand_mapping_;
OP_MAPPING		return 	_op_mapping_;
BYTESIZE		return  _bytesize_;	
GENERIC			return 	_generic_;	
TARGET			return 	_target_;	
TARGET_RISA		return 	_target_risa_;
REGS			return  _regs_;	
SUBTYPE			return 	_subtype_;	
LATCH			return 	_latch_;	
STORAGE			return 	_storage_;	
PORT			return 	_port_;	
CONNECTION		return 	_connection_;	
UNITS			return 	_units_;	
LATCHES			return 	_latches_;
IN				return  _input_;
OUT				return  _output_;
OTHER			return  _other_;	
PORTS			return 	_ports_;	
FEEDBACK\_ENQ	return 	_feedback_enq_;	
FEEDBACK\_DEQ	return 	_feedback_deq_;	
QUEUE\_ENQ		return  _queues_enq_;
QUEUE\_DEQ		return  _queues_deq_;
CONNECTIONS		return 	_connections_;	
STORAGES		return 	_storages_;	
OPCODES			return 	_opcodes_;	
OPCODES\_ENQ	return 	_opcodes_enq_;	
OPCODES\_DEQ	return 	_opcodes_deq_;	
TIMING			return 	_timing_;	
CAPACITY		return 	_capacity_;
MNEMONIC		return  _mnemonic_;	
PIPELINE		return 	_pipeline_;	
DTPATHS			return 	_dtpaths_;	
UNI				return 	_uni_;	
BI				return 	_bi_;	
MEMORY			return 	_memory_;	
SIZE			return 	_size_;	
WIDTH			return 	_width_;	
LINESIZE		return 	_linesize_;
WORDSIZE		return 	_wordsize_;	
NUM\_LINES		return 	_num_lines_;
WRITE\_BACK		return 	_write_back_;
WRITE\_THROUGH	return 	_write_through_;
LRU				return 	_lru_;
FIFO			return 	_fifo_;
REPLACEMENT\_POLICY		return 	_replacement_policy_;
WRITE\_POLICY	return 	_write_policy_;
READ_LATENCY	return 	_read_latency_;
WRITE_LATENCY	return 	_write_latency_;
LEVEL			return 	_level_;
NEXT_LEVEL		return 	_next_level_;
PREV_LEVEL		return 	_prev_level_;
ASSOCIATIVITY	return 	_associativity_;	
ADDRESS\_RANGE	return 	_addrrange_;
NUM\_BANKS		return	_num_banks_;
ACCESS\_MODE	return	_access_mode_;
NUM\PARALLEL\_READ		return	_num_parallel_read_;
NUM\PARALLEL\_WRITE		return	_num_parallel_write_;
READ\_WRITE\_CONFLICT	return	_read_write_conflict_;
STORAGE\_CONNECTIONS	return	_storage_connections_;	
OPERATIONS\_SECTION		return 	_operations_section_;	
INSTRUCTION\_SECTION	return 	_instruction_section_;	
OPMAPPING\_SECTION		return 	_opmapping_section_;	
ARCHITECTURE\_SECTION	return 	_architecture_section_;	
PIPELINE\_SECTION		return 	_pipeline_section_;	
STORAGE\_SECTION		return 	_storage_section_;	
PREDICATE		return 	_predicate_;	
SUBCOMPONENTS	return 	_subcomponents_;	
PARALLEL		return 	_parallel_;	
ALTERNATE		return 	_alternate_;
PATH			return 	_path_;	
ACCESS\_TIMES	return 	_accesstimes_;	
ACCESS\_PORTS	return 	_accessports_;	
ACCESS\_WIDTHS	return 	_accesswidths_;	
STRIDE			return 	_stride_;	
CIRC			return 	_circ_;	
none			return 	_none_;	
all				return 	_all_;	
TRUE			return 	_true_;	
UNIT			return  _unit_;
ARGUMENT		return  _argument_;
INSTR_IN		return	_instrin_;
INSTR_OUT		return	_instrout_;
ASMFORMAT		return  _asm_format_;
IRDUMPFORMAT    return  _dump_format_;
COND			return _asm_cond_;
PRINT		    return _asm_print_;

[-()*+,/:<=>\[\]\\]        {
                              yylval.ch=yytext[0];
							  if(PARSER_DEBUG) printf("%c",yytext[0]);
                              return yytext[0];
                           }



{DIGIT}+                   {
                               yylval.in=atoi(yytext);
							   if(PARSER_DEBUG) printf("Integer_lit\n");
                               return	Integer_lit;
						   }

(_|{ALFA})+				   {
                               yylval.st=copy(yytext);
							   if(PARSER_DEBUG) printf("%s\n",yytext);
                               return	Identifier;
                           }

\"([^"]|\"\")*\"		   {
								yylval.st=strip_quotes_and_copy(yytext, yyleng);
								return	_char_string_;
						   }

\;.*						{/*nothing*/}
[\t ]						{/*nothing*/}
\n							{increment_line_number();}
.							{ syntax_error("unexpected character in source program"); }
%%


char * copy(char * s){
      char * r;
      r=new char[strlen(s)+1];
      strcpy(r,s);
      return r;
}

char * strip_quotes_and_copy(char *s,int len){
       char * rez,*r;
       r=rez=new char[len-1];
       s++;
       do{
            if(*s=='"') s++;
            if(*s!='\0') *rez++=*s++;
            else *rez=*s;
       }while(*s!='\0');
       return r;
}
