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
#include "symtabs.h"
#ifndef __UNIX__
	#include "crtdbg.h"
#else 
	#include <stdio.h>
#endif



//prototypes
extern Symtab st;
int find_field_ident(std::map<char *,int> *global_field_map,char *name);
std::map<char *,int> *get_field_map(void);
int get_ident(NodePtr node);
int contains_arg_in_object(int arg,NodePtr object);
int supports_opcode(NodePtr node,char *opcode);
void print_instruction(char *opcode,std::map<int,int> *instruction);
void print_res_tab(ResTab *rt);
int find_format(std::map<int,int>*instruction);
ResTab * get_rt(char *opcode,std::map<int,int> *instruction,int format,int &computed,int dbEnabled=0);
void add_rt_to_db(char *opcode,std::map<int,int> *instruction,ResTab *rt);
extern int dbEnabled;



//local value used by traverse_units
static NodeList unit_list;

//counts the total number of instructions
int countInstructions=0;

int debugGenInstr=0;

//traverses the IR to compute the list of units in the architecture
int traverse_units(NodePtr node){
	if(node->Kind()==nke_component){
		if(contains_arg_in_object(find_field_ident(get_field_map(),"_UNIT_"),node)){
			unit_list.push_back(node);
		}
	}

	return 0; //when it returns 1, the Traverse function stops the traversal
}


//produces recursiverly all the combinations of fields (operands) for this opcode
//field_node: input, the current field being produced
void produce_fields(char *opcode,NodePtr operands,NodeList::iterator field_node,std::map<int,int> *instruction){
	int field,format;
	NodePtr var;
	NodeList::iterator k,temp,i;
	ResTab *rt;
	int computed;

	_ASSERT(operands);

	if(field_node==operands->Children()->end())
	{
		if(strcmp(opcode,"CMPP")==0)
		{ printf(" "); }
		//reached the end of the operands
		
		//find the units which support this opcode
		for(i=unit_list.begin();i!=unit_list.end();i++){
			if(supports_opcode(*i,opcode)){
				(*instruction)[find_field_ident(get_field_map(),"_UNIT_")]=get_ident(*i);
				//we have a complete instruction produced at this point
				if(debugGenInstr) print_instruction(opcode,instruction);

				format=find_format(instruction);

				rt=get_rt(opcode,instruction,format,computed,0);

				if(rt!=NULL){
					countInstructions++;
					if(dbEnabled==1){
						add_rt_to_db(opcode,instruction,rt);
					}
					if(debugGenInstr) print_res_tab(rt);
				}
			}
		}
	}
	else
	{

		if(debugGenInstr) printf("field_node = %s\n", (*field_node)->Name());

		//produce all the possible values for the current operand (field)
		field=find_field_ident(get_field_map(),(*field_node)->Name());
		_ASSERT(field!=-1);
		//save the current field into temp
		temp=field_node;
		//increment temp, to be used in the recursive call
		temp++;

		//find the var group pointed by this field
		var=(*field_node)->Child(nke_identifier)->Symbol()->GetFirstDef();
		_ASSERT(var);

		// Added by AVS on 03/16/2002 to fix generation of
		// reservation table due to change in EXPRESSION IR.
		
		// What we are doing here is collecting the register files
		// of the operands.
		// For example : 
		// For the format _UNIT_ _SOURCE_1_ _SOURCE_2_ _DEST_
		// It should add GPRFile, GPRFile, GPRFile with the three operands.
		//

		// find the reg_list node
		NodePtr reg_list_node = var->Child(nke_reg_list);
		_ASSERT(reg_list_node);

		//for all the children of reg_list node
		for(k=reg_list_node->Children()->begin();k!=reg_list_node->Children()->end();k++)
		{
			//field can be any of these objects
			(*instruction)[field]=get_ident(*k);

			//proceed to the next field (operand)
			produce_fields(opcode,operands,temp,instruction);
		}
	}
}


//traverses all fields in the format, and assigns all possible values to each
//defs: input, the nodes defining the opcode
void produce_all_instrs(NodeList * defs){
	NodePtr opers;
	NodeList::iterator i;
	std::map<int,int> instruction;
	int found=0;
	char *name;
	
	//for all the defs of this opcode
	for(i=defs->begin();i!=defs->end();i++){
		if((*i)->Kind()==nke_opcode){
			//look in this definition for an OPERANDS attribute
			opers=(*i)->Child(nke_operands);
			if(strcmp((*i)->Name(),"MPY")==0){
				printf(" ");
			}
			if(opers!=NULL){
				produce_fields((*i)->Name(),opers,opers->Children()->begin(),&instruction);
				found=1;
			}
			name=(*i)->Name();
		}
	}
	if(!found){
		//printf("Opcode %s is missing operands declaration.\n",name);
		//_ASSERT(0);
	}
}


//traverses the symbol table
int traverse_opcode(SymbolPtr sym){
	NodeList *defs;
	
	_ASSERT(sym);
	defs=sym->GetDefList();
	//if this guy is an opcode, produce all the instructions
	if(defs->begin()!=defs->end() && (*(defs->begin()))->Kind()==nke_opcode)
		produce_all_instrs(defs);

	return 0; //when it returns 1, the Traverse function stops the traversal
}



//traverses the instruction set description in EXPRESSION 
//and produces all the possible instructions
//node: input, the whole EXPRESSION IR
void gen_instrs(void){
	//find the list of all the units in the IR
	unit_list.clear();
	ir->Traverse(&traverse_units);

	//produce the instructions
	st.Traverse(&traverse_opcode);

	//at this point the dababase with RTs is complete
	printf("\nTotal number of instructions: %d",countInstructions);
}



