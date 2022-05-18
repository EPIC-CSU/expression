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

#include "symtabs.h"
#include "node.h"
#include "GenerateRT.h"

#ifdef __UNIX__
	#include <stdio.h>
#else
	#include "crtdbg.h"
#endif

#include <set>

#define PORT_GEN
#define CONNECTION_GEN

//prototypes
int get_ident(NodePtr node);
char *get_name(int ident);
extern std::map<int,std::list<ResTab*>*> global_traces;
std::map<int,std::list<int>*> * get_instr_formats(void);
extern char *mdes_file_name;
struct CompleteInstrType{
	char *opcode;
	std::map<int,int> *fields;
	ResTab *rt;
};
extern std::map<long,std::list<CompleteInstrType*>*> rt_data_base;
std::map<char *,int> *get_field_map(void);
int find_field_ident(std::map<char *,int> *global_field_map,char *name);
extern Symtab st;
int is_storage_type(NodePtr node);
int is_unit_type(NodePtr node);
int is_port_type(NodePtr node);
int is_connection_type(NodePtr node);



//debug the genMdes pass
int debugMDES=1;

//ATTENTION: at most one of these 2 guys have to be 1 at the same time
//if both are 0, it takes for each opgroup the first RT option
//if 1, the intersection of RTs is generated for each opgroup
int genRTintersection=0;
//if 1, the union of RTs is generated for each opgroup
int genRTunion=1;

//the file we output the MDES to 
FILE *mdes_file;

void open_mdes_file(char *name){
	_ASSERT(name);
	mdes_file=fopen(name,"w");
	_ASSERT(mdes_file);
}

void close_mdes_file(void){
	fclose(mdes_file);
}


void print_mdes(char *buf){
	if(debugMDES){
		printf(buf);
	}
	_ASSERT(mdes_file);
	fprintf(mdes_file,"%s",buf);
}


//generates the resource section of the MDES description, from the global_traces, 
//and the instr formats
void genResourceSection(void){
	std::list<ResTab*>::iterator i;
	std::map<int,std::list<int>*>::iterator l;
	std::set<int> resource_list;
	std::set<int>::iterator m;
	std::map<int,std::list<int>*> * instr_formats;
	int k,format;
	ResTab *rt;
	NodeList::iterator j;
	char buf[2000];

	instr_formats=get_instr_formats();

	_ASSERT(instr_formats);

	//for each instruction format 
	for(l=instr_formats->begin();l!=instr_formats->end();l++){
		format=(*l).first;
		//for each trace for this format
		for(i=global_traces[format]->begin();i!=global_traces[format]->end();i++){
			rt=*i;
			_ASSERT(rt);
			//for each stage in this reservation table (trace)
			for(k=0;k<PIPELENGTH;k++){
				_ASSERT((*rt)[k]);
				//for each resource in this stage
				for(j=(*rt)[k]->begin();j!=(*rt)[k]->end();j++){
					_ASSERT(*j);
					//add this resource to the resource_list
					resource_list.insert(get_ident(*j));
				}
			}
		}
	}
	//output the MDES description
	print_mdes("\nSECTION Resource\n{");
	for(m=resource_list.begin();m!=resource_list.end();m++){
		sprintf(buf,"\n\t%s();",get_name(*m));
		print_mdes(buf);
	}
	print_mdes("\n}");
}


//generates the resource usage section of the MDES description, from the global_traces, 
//and the instr formats
void genResourceUsageSection(std::map<int,std::set<int>*> *resource_usage_map){
	std::map<long,std::list<CompleteInstrType*>*>::iterator i;
	std::list<CompleteInstrType*>::iterator l;
	CompleteInstrType* instr;
	std::map<int,std::set<int>*>::iterator m;
	std::set<int>::iterator n;
	int k;
	ResTab *rt;
	NodeList::iterator j;
	char buf[2000];
	int unit;

	//for each opcode in the hash table of the RT data-base
	for(i=rt_data_base.begin();i!=rt_data_base.end();i++){
		_ASSERT((*i).second);
		//for each fully qualified operation in the list for this opcode
		for(l=(*i).second->begin();l!=(*i).second->end();l++){
			instr=(*l);
			_ASSERT(instr);
			rt=instr->rt;
			_ASSERT(rt);
			//for each stage in this reservation table (trace)
			for(k=0;k<PIPELENGTH;k++){
				_ASSERT((*rt)[k]);
				//for each resource in this stage
				for(j=(*rt)[k]->begin();j!=(*rt)[k]->end();j++){
					_ASSERT(*j);
					unit=get_ident(*j);
					//add this resource to the resource_usage_map
					if(resource_usage_map->find(unit)==resource_usage_map->end()){
						//if this is the first insertion of this unit
						//initialize the list of ints
						(*resource_usage_map)[unit]=new std::set<int>();
					}
					(*resource_usage_map)[unit]->insert(k);
				}
			}
		}
	}
	//output the MDES description
	print_mdes("\nSECTION Resource_Usage\n{");
	//for each resource in the resource_usage_map
	for(m=resource_usage_map->begin();m!=resource_usage_map->end();m++){
		//for each stage it is used in
		for(n=(*m).second->begin();n!=(*m).second->end();n++){
			//create a resource usage for this resource and this stage
			sprintf(buf,"\n\tRU_%s_t%d( use(%s) time(%d));",get_name((*m).first),*n,get_name((*m).first),*n);
			print_mdes(buf);
		}
	}
	print_mdes("\n}");
}

//generates the Resource Unit section of the MDES description
//no_output: input, if 1 it does not generate output in the MDES file
void genResourceUnitSection(std::map<ResTab*,int> *resource_unit_map,std::map<int,std::list<char *>*> *reservation_tables_map,int no_output){
	std::map<long,std::list<CompleteInstrType*>*>::iterator i;
	std::list<CompleteInstrType*>::iterator l;
	CompleteInstrType* instr;
	int k;
	ResTab *rt;
	NodeList::iterator j;
	char buf[2000];
	int unit;
	int rt_cnt=0;

	if(!no_output) print_mdes("\nSECTION Resource_Unit\n{");

	//for each opcode in the hash table of the RT data-base
	for(i=rt_data_base.begin();i!=rt_data_base.end();i++){
		_ASSERT((*i).second);
		//for each fully qualified operation in the list for this opcode
		for(l=(*i).second->begin();l!=(*i).second->end();l++){
			instr=(*l);
			_ASSERT(instr);
			rt=instr->rt;
			_ASSERT(rt);

			sprintf(buf,"\n\tRUNIT_%d(",rt_cnt);
			if(!no_output) print_mdes(buf);
			//add this rt to the reservation table map, to remember the indexing of the RTs
			(*resource_unit_map)[rt]=rt_cnt;

			//for each stage in this reservation table (trace)
			for(k=0;k<PIPELENGTH;k++){
				_ASSERT((*rt)[k]);
				//for each resource in this stage
				for(j=(*rt)[k]->begin();j!=(*rt)[k]->end();j++){
					_ASSERT(*j);
					unit=get_ident(*j);
					//don't add the storages into the RTs, since Trimaran shouldn't find conflicts
					//on them
					//not generating port/connection resource conflicts, since Trimaran
					//does not capture this information
					//Not generating FETCH/ISSUE, since Trimaran does not support CAPACITY
					//different than 1
					if((is_unit_type(*j)) && k>1)
					{
						//add this resource usage to this reservation table
						sprintf(buf,"RU_%s_t%d ",get_name(unit),k);
						if(!no_output) print_mdes(buf);
						if(reservation_tables_map->find(rt_cnt)==reservation_tables_map->end())
						{
							(*reservation_tables_map)[rt_cnt]=new std::list<char*>();
						}
						(*reservation_tables_map)[rt_cnt]->push_back(strdup(buf));
					}
#ifdef PORT_GEN
					if ((is_port_type(*j)) && k>1)
					{ //ATTENTION!!!!!!!!!!!!!!!!!!!!!!!!!!!!
						//add this resource usage to this reservation table
						sprintf(buf,"RU_%s_t%d ",get_name(unit),k);
						if(!no_output) print_mdes(buf);
						if(reservation_tables_map->find(rt_cnt)==reservation_tables_map->end())
						{
							(*reservation_tables_map)[rt_cnt]=new std::list<char*>();
						}
						(*reservation_tables_map)[rt_cnt]->push_back(strdup(buf));
					}
#endif
#ifdef CONNECTION_GEN
					if ((is_connection_type(*j)) && k>1)
					{ //ATTENTION!!!!!!!!!!!!!!!!!!!!!!!!!!!!
						//add this resource usage to this reservation table
						sprintf(buf,"RU_%s_t%d ",get_name(unit),k);
						if(!no_output) print_mdes(buf);
						if(reservation_tables_map->find(rt_cnt)==reservation_tables_map->end())
						{
							(*reservation_tables_map)[rt_cnt]=new std::list<char*>();
						}
						(*reservation_tables_map)[rt_cnt]->push_back(strdup(buf));
					}
#endif
		}
			}
			//increment the number of reservation tables
			rt_cnt++;
			if(!no_output) print_mdes(");");
		}
	}
	if(!no_output) print_mdes("\n}");
}

//returns the list of options for a table option if found
//otherwise returns NULL
std::set<int>* find_table_option(std::map<char *,std::set<int>*> *table_option_map,char *name){
	std::map<char *,std::set<int>*>::iterator i;

	_ASSERT(table_option_map);
	_ASSERT(name);

	for(i=table_option_map->begin();i!=table_option_map->end();i++){
		_ASSERT((*i).second);
		if(strcmp((*i).first,name)==0)
			return (*i).second;
	}
	return NULL;
}

//generates the Table Option section of the MDES description on a per opcode basis
void genTableOptionPerOpcode(std::map<ResTab*,int> *resource_unit_map,std::map<char *,std::set<int>*> *table_option_map){
	std::map<long,std::list<CompleteInstrType*>*>::iterator i;
	char buf[2000];
	std::list<CompleteInstrType*>::iterator j;
	CompleteInstrType* instr;
	char *unitname;
	int unit,ru_idx;
	std::set<int> *options;
	std::map<char *,std::set<int>*>::iterator k;
	std::set<int>::iterator l;

	//for each opcode in the hash table of the RT data-base
	for(i=rt_data_base.begin();i!=rt_data_base.end();i++){
		_ASSERT((*i).second);
		//for each fully qualified operation in the list for this opcode
		for(j=(*i).second->begin();j!=(*i).second->end();j++){
			instr=(*j);
			_ASSERT(instr);
			unit=find_field_ident(get_field_map(),"_UNIT_");
			unitname=get_name((*(instr->fields))[unit]);
			_ASSERT(resource_unit_map->find(instr->rt)!=resource_unit_map->end());
			ru_idx=(*resource_unit_map)[instr->rt];
			sprintf(buf,"%s_%s",instr->opcode,unitname);
			if((options=find_table_option(table_option_map,buf))==NULL){
				options=(*table_option_map)[strdup(buf)]=new std::set<int>();
			}
			options->insert(ru_idx);
		}
	}

	print_mdes("\nSECTION Table_Option\n{");

	//for each table option
	for(k=table_option_map->begin();k!=table_option_map->end();k++){
		_ASSERT((*k).second);
		//print out the table option header
		sprintf(buf,"\n\tTO_%s(one_of(",(*k).first);
		print_mdes(buf);
		//for each option in this table option
		for(l=(*k).second->begin();l!=(*k).second->end();l++){
			sprintf(buf,"RUNIT_%d ",*l);
			print_mdes(buf);
		}
		print_mdes("));");
	}

	print_mdes("\n}");
}


//generates the Table Option section of the MDES description on a per Operation group basis
//no_output: input, if 1 it does not generate output in the MDES file
void genTableOptionPerGroup(std::map<ResTab*,int> *resource_unit_map,std::map<char *,std::set<int>*> *table_option_map,int no_output){
	std::map<long,std::list<CompleteInstrType*>*>::iterator i;
	char buf[2000];
	std::list<CompleteInstrType*>::iterator j;
	CompleteInstrType* instr;
	char *unitname;
	int unit,ru_idx;
	std::set<int> *options;
	std::map<char *,std::set<int>*>::iterator k;
	std::set<int>::iterator l;
	SymbolPtr opcode_sym;
	NodeList * opcode_def_list;
	NodeList::iterator m;

	//for each opcode in the hash table of the RT data-base
	for(i=rt_data_base.begin();i!=rt_data_base.end();i++){
		_ASSERT((*i).second);
		//for each fully qualified operation in the list for this opcode
		for(j=(*i).second->begin();j!=(*i).second->end();j++){
			instr=(*j);
			_ASSERT(instr);
			unit=find_field_ident(get_field_map(),"_UNIT_");
			unitname=get_name((*(instr->fields))[unit]);
			_ASSERT(resource_unit_map->find(instr->rt)!=resource_unit_map->end());
			ru_idx=(*resource_unit_map)[instr->rt];
			//find the opgroup this opcode belongs to 
			opcode_sym=st.FindSymbol(instr->opcode);
			opcode_def_list=opcode_sym->GetDefList();
			//for each definition node for this opcode
			for(m=opcode_def_list->begin();m!=opcode_def_list->end();m++){
				//if the parent of this definition is an opcode group
				if((*m)->Parent()->Kind()==nke_op_group && 
					strcmp((*m)->Parent()->Name(),"_all_ops_")!=0){
					//add the table option for this opgroup
					sprintf(buf,"%s_%s",(*m)->Parent()->Name(),unitname);
					if((options=find_table_option(table_option_map,buf))==NULL){
						options=(*table_option_map)[strdup(buf)]=new std::set<int>();
					}
					options->insert(ru_idx);
				}
			}
		}
	}

	if(!no_output){
		print_mdes("\nSECTION Table_Option\n{");
		
		//for each table option
		for(k=table_option_map->begin();k!=table_option_map->end();k++){
			_ASSERT((*k).second);
			//print out the table option header
			sprintf(buf,"\n\tTO_%s(one_of(",(*k).first);
			print_mdes(buf);
			//for each option in this table option
			for(l=(*k).second->begin();l!=(*k).second->end();l++){
				sprintf(buf,"RUNIT_%d ",*l);
				print_mdes(buf);
			}
			print_mdes("));");
		}
		
		print_mdes("\n}");
	}
}

//generates the reservation table section of the MDES description
void genReservationTableWithOptions(std::map<char *,std::set<int>*> *table_option_map){
	std::map<char *,std::set<int>*>::iterator k;
	char buf[2000];

	print_mdes("\nSECTION Reservation_Table\n{");

	//for each table option
	for(k=table_option_map->begin();k!=table_option_map->end();k++){
		_ASSERT((*k).second);
		//print out the RT and the corresponding table option
		sprintf(buf,"\n\tRT_%s(TO_%s);",(*k).first,(*k).first);
		print_mdes(buf);
	}

	print_mdes("\n}");
}


//returns 1 if it finds ru in this rt
int find_ru_in_rt(char *ru,std::list<char *> *rt){
	std::list<char *>::iterator i;

	for(i=rt->begin();i!=rt->end();i++){
		if(strcmp(ru,*i)==0){
			return 1;
		}
	}
	return 0;
}


//generates the reservation table section of the MDES description
//generates the flat version, with no RUNIT and Table_Option sections
void genReservationTableFlat(std::map<char *,std::set<int>*> *table_option_map,std::map<int,std::list<char *>*> *reservation_tables_map){
	std::map<char *,std::set<int>*>::iterator k;
	char buf[2000];
	std::list<char *>::iterator i;
	int rt;
	std::set<int>* options;
	std::list<char *> res_tab;
	std::set<int>::iterator j;
	std::list<char *>::iterator l,temp_l;

	print_mdes("\nSECTION Reservation_Table\n{\n\tRT_null(use());");

	//for each table option
	for(k=table_option_map->begin();k!=table_option_map->end();k++){
		options=(*k).second;
		_ASSERT(options);
		//print out the RT and the corresponding table option
		sprintf(buf,"\n\tRT_%s(use(",(*k).first);
		print_mdes(buf);

		//find one of the option we want to represent for this whole group
		//rt is the reservation table for this group

		//we can generate the intersection here instead (optimistic)
		res_tab.clear();

		//if we want to generate the intersection of the RTs in a set of options
		if(genRTintersection){
			//first create the union of all these RTs

			//for each RT option for this opgroup
			for(j=options->begin();j!=options->end();j++){
				//rt is the reservation table number
				rt=*j;

				//for each RU in this RT option
				for(i=(*reservation_tables_map)[rt]->begin();i!=(*reservation_tables_map)[rt]->end();i++){
					_ASSERT(*i);
					//if it's not already inside
					if(!find_ru_in_rt(*i,&res_tab)){
						//add this RU to the res_tab
						res_tab.push_back(strdup(*i));
					}
				}
			}

			//next, subtract from the uion all the RUs missing from every RT option

			//for each RT option for this opgroup
			for(j=options->begin();j!=options->end();j++){
				//rt is the reservation table number
				rt=*j;
				//for each RU in the res_tab
				for(l=res_tab.begin();l!=res_tab.end();){
					//if this RU is not in the RT option 
					if(!find_ru_in_rt(*l,(*reservation_tables_map)[rt])){
						//extract it from res_tab
						temp_l=l;
						l++;
						res_tab.erase(temp_l);
					}
					else{
						l++;
					}
				}
			}
		}
		else if(genRTunion){
			//for each RT option for this opgroup
			for(j=options->begin();j!=options->end();j++){
				//rt is the reservation table number
				rt=*j;

				//for each RU in this RT option
				for(i=(*reservation_tables_map)[rt]->begin();i!=(*reservation_tables_map)[rt]->end();i++){
					_ASSERT(*i);
					if(!find_ru_in_rt(*i,&res_tab)){
						//add this RU to the res_tab
						res_tab.push_back(strdup(*i));
					}
				}
			}
		}
		else{//just use the first RT for this group
			rt=*(options->begin());

			//for each resource usage in this reservation table
			for(i=(*reservation_tables_map)[rt]->begin();i!=(*reservation_tables_map)[rt]->end();i++){
				_ASSERT(*i);
				//add this RU to the res_tab
				res_tab.push_back(strdup(*i));
			}
		}
		
		//for each RU in this res_tab
		for(i=res_tab.begin();i!=res_tab.end();i++){
			_ASSERT(*i);
			sprintf(buf,"%s",*i);
			print_mdes(buf);
		}

		print_mdes("));");
	}

	print_mdes("\n}");
}


//generates the MDES description from the reservation tables information
void genMdes(NodePtr node){
	std::map<int,std::set<int>*> resource_usage_map;
	std::map<ResTab*,int> resource_unit_map;
	std::map<char *,std::set<int>*> table_option_map;
	std::map<int,std::list<char *>*> reservation_tables_map;

	open_mdes_file(mdes_file_name);

	genResourceSection();

	genResourceUsageSection(&resource_usage_map);

	genResourceUnitSection(&resource_unit_map,&reservation_tables_map,1);

	genTableOptionPerGroup(&resource_unit_map,&table_option_map,1);

	genReservationTableFlat(&table_option_map,&reservation_tables_map);

	close_mdes_file();
}


/*
//generates the reservation table section of the MDES description
void genReservationTable(std::map<ResTab*,int> *resource_unit_map){
	std::map<long,std::list<CompleteInstrType*>*>::iterator i;
	char buf[2000];
	std::list<CompleteInstrType*>::iterator j;
	CompleteInstrType* instr;
	char *unitname;
	int unit,ru_idx;

	print_mdes("\nSECTION Reservation_Table\n{");

	for(i=rt_data_base.begin();i!=rt_data_base.end();i++){
		_ASSERT((*i).second);
		for(j=(*i).second->begin();j!=(*i).second->end();j++){
			instr=(*j);
			_ASSERT(instr);
			unit=find_field_ident(get_field_map(),"_UNIT_");
			unitname=get_name((*(instr->fields))[unit]);
			_ASSERT(resource_unit_map->find(instr->rt)!=resource_unit_map->end());
			ru_idx=(*resource_unit_map)[instr->rt];
			sprintf(buf,"\n\tRT_%s_%s(RUNIT_%d);",instr->opcode,unitname,ru_idx);
			print_mdes(buf);
		}
	}

	print_mdes("\n}");
}
*/

/*
//generates the Resource Unit section of the MDES description
//no_output: input, if 1 it does not generate output in the MDES file
void genResourceUnitSection(std::map<ResTab*,int> *resource_unit_map,std::map<int,std::list<char *>*> *reservation_tables_map,int no_output){
	std::list<ResTab*>::iterator i;
	std::map<int,std::list<int>*>::iterator l;
	std::map<int,std::list<int>*> * instr_formats;
	int k,format;
	ResTab *rt;
	NodeList::iterator j;
	char buf[2000];
	int unit;
	int rt_cnt=0;

	instr_formats=get_instr_formats();

	_ASSERT(instr_formats);

	if(!no_output) print_mdes("\nSECTION Resource_Unit\n{");

	//for each instruction format 
	for(l=instr_formats->begin();l!=instr_formats->end();l++){
		format=(*l).first;
		//for each trace for this format
		for(i=global_traces[format]->begin();i!=global_traces[format]->end();i++){
			rt=*i;
			_ASSERT(rt);

			sprintf(buf,"\n\tRUNIT_%d(",rt_cnt);
			if(!no_output) print_mdes(buf);
			//add this rt to the reservation table map, to remember the indexing of the RTs
			(*resource_unit_map)[rt]=rt_cnt;

			//for each stage in this reservation table (trace)
			for(k=0;k<PIPELENGTH;k++){
				_ASSERT((*rt)[k]);
				//for each resource in this stage
				for(j=(*rt)[k]->begin();j!=(*rt)[k]->end();j++){
					_ASSERT(*j);
					unit=get_ident(*j);
					//don't add the storages into the RTs, since Trimaran shouldn't find conflicts
					//on them
					//not generating port/connection resource conflicts, since Trimaran
					//does not capture this information
					//Not generating FETCH/ISSUE, since Trimaran does not support CAPACITY
					//different than 1
					if((is_unit_type(*j) || is_port_type(*j)
						|| is_connection_type(*j)) && k>1){ //ATTENTION!!!!!!!!!!!!!!!!!!!!!!!!!!!!

						//add this resource usage to this reservation table
						sprintf(buf,"RU_%s_t%d ",get_name(unit),k);
						if(!no_output) print_mdes(buf);
						if(reservation_tables_map->find(rt_cnt)==reservation_tables_map->end()){
							(*reservation_tables_map)[rt_cnt]=new std::list<char*>();
						}
						(*reservation_tables_map)[rt_cnt]->push_back(strdup(buf));
					}
				}
			}
			//increment the number of reservation tables
			rt_cnt++;
			if(!no_output) print_mdes(");");
		}
	}
	if(!no_output) print_mdes("\n}");
}

*/
