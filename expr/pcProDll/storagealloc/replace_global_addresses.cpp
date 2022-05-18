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
#include <map>
#include <vector>
#endif

#ifdef __UNIX__
#include <ctype.h>
#endif

#include "stdafx.h"

#ifdef WIN32
#include "defs_node.h"
#include "symbols.h"
#include "symtabs.h"
#endif

#include "IConstArg.h"
#include "LabelArg.h"
#include "ComputeOp.h"
#include "CallOp.h"
#include "MemoryOp.h"
#include "Routine.h"
#include "Program.h"
#include "BaseOperation.h"


#ifdef __UNIX__
#include "defs_node.h"
#include "symbols.h"
#include "symtabs.h"
#endif

#ifdef map
#undef map
#endif 
#define map std::map

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

//prototypes
void allocate_all_symbols(void);
void print_mem_alloc(void);
long int get_memory_assig(char *name);
void size_all_symbols(void);
defs_node *get_node(char *name);
defs_node *get_global(defs_node *node);

int debugMemAlloc=0;


char * remove_and_compute_hi_lo(char *argname,int &hi,int &lo){
	char *p;
	hi=lo=0;
	p=argname;
	if(*p=='_'){
		p++;
		if(*p++=='g'){
			if(*p++=='c' && *p++=='c'){
				if(*p=='h' && *(p+1)=='i' && *(p+2)=='_'){
					//_gcchi_
					hi=1;
					return p+3;
				}
				if(*p=='l' && *(p+1)=='o' && *(p+2)=='_'){
					//_gcclo_
					lo=1;
					return p+3;
				}
			}
		}
	}
	return argname;
}

char * remove_expression(char *argname,int &expr){
	char *p;
	char expr_buf[1000],*ep;
	char *r;
	int i=0;
	int minus=0;
	expr=0;
	for(p=argname;*p!='+' && *p!='-' && *p!='\0';p++,i++){
	}
	r=strdup(argname);
	if(*p=='+' || *p=='-'){
		if(*p=='-') minus=1;
		ep=expr_buf;
		for(p++;isdigit(*p) && *p!='\0';p++){
			*ep++=*p;
		}
		*ep='\0';
		if(*p!='\0'){
			_ASSERT(0);
		}
		expr=atoi(expr_buf);
		if(minus) expr=-expr;
		r[i]='\0';
	}
	return r;
}

unsigned long compute_hilo(unsigned long val,int hi,int lo){
	unsigned long val_hilo;
	if(hi){//if it is a hi make 0 in the low part of the address
		val_hilo=val & 0xFFFF0000;
	}
	else if(lo){ //if it is a lo, make 0 in the high part of the address
		val_hilo=val & 0x0000FFFF;
	}
	else
		val_hilo=val;
	return val_hilo;
}

//creates an IConstArg instead of the symbol, with it's address
IConstArg *create_iconst_arg_from_symbol(BaseArgument *arg, MyString & filename){
	unsigned long val, val_hilo;
	IConstArg *int_arg;
	char buffer[1000];
	char *argname;
	int hi=0, lo=0;
	int expr=0;

	if (arg != NULL){
		if ( arg->isLabelArg()==_YES_){
			argname=((LabelArg*)arg)->lName();
			//this is a symbol which has to be replaced with the memory location

			//if this is a %hi or %lo argument (from the gcc-mips)
			//remove the _%hi_ from the begining of the argname, and put in hi, lo the info wheather its a hi or lo
			argname=remove_and_compute_hi_lo(argname,hi,lo);

			//if this is a label + an expression or - an expression, need to remove the expression, and then
			//add it or subtract it from the address
			argname=remove_expression(argname,expr);

			//first add the file name to the name of the label (if it is a local)
			//and try to find it as a local
			sprintf(buffer,"%s_%s",filename.getStr(),argname);

			val=get_memory_assig(buffer);

			if(val!=-1){
				//if found a local
				//if this label argument was defined in the DEFS file
				val_hilo=compute_hilo(val, hi,lo);
				int_arg=new IConstArg(val_hilo+expr);
				//argname was strduped in remove_expression, need to delete it
				delete argname;
				return int_arg;
			}
			else{
				//try to find it as a global (no file name appended)
				val=get_memory_assig(argname);
				if(val!=-1){
					//if found a global
					//if this label argument was defined in the DEFS file
					val_hilo=compute_hilo(val, hi,lo);
					int_arg=new IConstArg(val_hilo+expr);
					//argname was strduped in remove_expression, need to delete it
					delete argname;
					return int_arg;
				}
			}
			//argname was strduped in remove_expression, need to delete it
			delete argname;
			//did not find the symbol
			//_ASSERT(0);
		}
	}
	return NULL;
}


// argType is one of : _DEST_LIST_ or _SRC_LIST_
//
void replace_symbol_arg(BaseOperation *curDataOp, int pos, int argType, MyString & filename){
	BaseArgument *arg;
	IConstArg * int_arg;

	arg = curDataOp->ptrToOperand(pos, argType);

	if(arg!=NULL && arg->isLabelArg()==_YES_){
		//create the integer IConst argument for this symbol (label) argument
		int_arg=create_iconst_arg_from_symbol(arg, filename);
		if(int_arg!=NULL){
			//if this label argument was defined in the DEFS file
			//replace the symbol Label argument with the IConst integer argument
			curDataOp->addOperand(int_arg, pos, argType);
		}
	}
}

extern Program *curProgram;

//traverses the comiler IR, and replaces the symbols with their mem location
void replace_symbols_with_mem_location(void){
	MyLinkedListIterator<BaseInstruction *> * iter;
	BaseInstruction *curInstr;
	ArrayIterator<DataOpSlot> *dataSlotIterator;
	ArrayIterator<ControlOpSlot> *cntrlSlotIterator;
	DataOpSlot *curDataSlot;
	ControlOpSlot *curCntrlSlot;
	BaseOperation *curDataOp, *curCntrlOp;
	
	Routine *cRout;
	MyLinkedListIterator<Routine *> *i;
	
	MyString str;
	for(i = curProgram->routineListIteratorForw(); i->hasMoreElements(); i->nextElement())
	{
		cRout = i->currentElement();
		
		// get the filename for this routine.
		//
		cRout->getFileName(str);
		
		for(iter=cRout->instrListIteratorForw();iter->hasMoreElements();iter->nextElement()){
			curInstr = iter->currentElement();
			if (curInstr->isNormalInstr()){
				for(dataSlotIterator = ((NormalInstr *)curInstr)->getDataOpSlotIter();
				dataSlotIterator->hasMoreElements();
				dataSlotIterator->nextElement())
				{
					curDataSlot = & dataSlotIterator->currentElement();
					curDataOp = curDataSlot->getPtrToOperation();
					if(curDataOp!=NULL){

						if(debugMemAlloc){
							curDataOp->shortPrint(cout);
							cout << endl;
						}

					/*
					if (curDataOp->isComputeOp()) 
					{
					replace_symbol_arg(((ComputeOp*)curDataOp),_DEST_, str);
					replace_symbol_arg(((ComputeOp*)curDataOp),_SOURCE_1_, str);
					replace_symbol_arg(((ComputeOp*)curDataOp),_SOURCE_2_, str);
					}
					else if (curDataOp->isMemoryOp()) {
					if (((MemoryOp *)curDataOp)->isLoad()) {
					replace_symbol_arg(((MemoryOp*)curDataOp),_DEST_, str);
					replace_symbol_arg(((MemoryOp*)curDataOp),_SOURCE_1_, str);
					replace_symbol_arg(((MemoryOp*)curDataOp),_SOURCE_2_, str);
					}
					else {
					replace_symbol_arg(((MemoryOp*)curDataOp),_DEST_, str);
					replace_symbol_arg(((MemoryOp*)curDataOp),_SOURCE_1_, str);
					replace_symbol_arg(((MemoryOp*)curDataOp),_SOURCE_2_, str);
					}
					}
					else if (curDataOp->isCallOp()) {
					replace_symbol_arg(((ComputeOp*)curDataOp),_DEST_, str);
					}
					else 
					_ASSERT(0);
						*/
						
						// Changed by SAH : Fri, 18th August, 2000
						//
						// MULTIPLE_DESTS_AND_SOURCES
						//

						// DEBUG code.
						// cout << "*****  ";
						// curDataOp->shortPrint(cout);
						// cout << "  *****" << endl;

						ArgList & a = curDataOp->destOperandList();
						ArgListIter ai;
						int i = 0;
						for (ai = a.begin(); ai != a.end(); ai++)
						{
							if ((*ai) != NULL)
							{
								replace_symbol_arg(curDataOp, i, _DEST_LIST_, str);
							}
							i++;
						}
						i = 0;
						ArgList & b = curDataOp->sourceOperandList();
						for (ai = b.begin(); ai != b.end(); ai++)
						{
							if ((*ai) != NULL)
							{
								replace_symbol_arg(curDataOp, i, _SRC_LIST_, str);
							}
							i++;
						}
						//
						// end change for MULTIPLE_DESTS_AND_SOURCES
// DEBUG Code
						if(debugMemAlloc){
							curDataOp->shortPrint(cout);
							cout << endl;
						}
					}
				}
				delete dataSlotIterator;
				
				for(cntrlSlotIterator = ((NormalInstr *)curInstr)->getControlOpSlotIter();
				cntrlSlotIterator->hasMoreElements();
				cntrlSlotIterator->nextElement())
				{
					curCntrlSlot = & cntrlSlotIterator ->currentElement();
					curCntrlOp = curCntrlSlot ->getPtrToOperation();
					if(curCntrlOp!=NULL){
						
// DEBUG Code
						if(debugMemAlloc){
							curCntrlOp->shortPrint(cout);
							cout << endl;
						}


						// Changed by SAH : Fri, 18th August, 2000
						//
						// MULTIPLE_DESTS_AND_SOURCES
						//
						ArgList & a = curCntrlOp->destOperandList();
						ArgListIter ai;
						int i = 0;
						for (ai = a.begin(); ai != a.end(); ai++)
						{
							if ((*ai) != NULL)
							{
								replace_symbol_arg(curCntrlOp, i, _DEST_LIST_, str);
							}
							i++;
						}
						i = 0;
						ArgList & b = curCntrlOp->sourceOperandList();
						for (ai = b.begin(); ai != b.end(); ai++)
						{
							if ((*ai) != NULL)
							{
								replace_symbol_arg(curCntrlOp, i, _SRC_LIST_, str);
							}
							i++;
						}
						//
						// end change for MULTIPLE_DESTS_AND_SOURCES
// DEBUG Code
						if(debugMemAlloc){
							curCntrlOp->shortPrint(cout);
							cout << endl;
						}
					}
				}
				delete cntrlSlotIterator;
			}
		}
	}
	delete i;
}



void global_memory_alloc(void){
	//determine the size of all symbols
	size_all_symbols();
	//allocate memory for globals
	allocate_all_symbols();
	//print out the memory allocation
	print_mem_alloc();
	//traverse the compiler IR, and replace the symbols with their base address
	replace_symbols_with_mem_location();
}

void append_file_name_arg(ComputeOp *curDataOp,int pos, MyString & filename){
	BaseArgument *arg;
	char *argname;
	defs_node *node,*global;
	char buffer[1000];

	arg = curDataOp->ptrToOperand(pos);

	if(arg!=NULL && arg->isLabelArg()==_YES_){
		//append the file name to this arg
		argname=((LabelArg*)arg)->lName();
		sprintf(buffer,"%s_%s",filename.getStr(),argname);
		node=get_node(buffer);
		if(node != NULL){
			global=get_global(node);
			if(global!=NULL && atoi(global->Name())==0){
				//if it is local and static
				//need to append the file name to its name, to distinguish it from other locals in other files
				((LabelArg*)arg)->initLabel(buffer);
			}
		}
	}
}

void append_file_name_arg(MemoryOp *curDataOp,int pos, MyString & filename){
	BaseArgument *arg = NULL;
	char *argname;
	defs_node *node,*global;
	char buffer[1000];

	int store_with_label_offset = 0;

	if (curDataOp->hasOperand(pos))
		arg = curDataOp->ptrToOperand(pos);

	// Changed by SAH on : Tue, Jul 11 2000
	//
	// If it is a store op, we need to behave differently.
	// 
	if ((curDataOp->isStore()) && (pos == _DEST_))
	{
		// then get the source 3 argument (store op has no "DEST"). Source 3 is the same as offset.
		//
		arg = curDataOp->getOffset();
		store_with_label_offset = 1;
	}

	if(arg!=NULL && arg->isLabelArg()==_YES_){
		//append the file name to this arg
		argname=((LabelArg*)arg)->lName();
		sprintf(buffer,"%s_%s",filename.getStr(),argname);
		node=get_node(buffer);
		//_ASSERT(node);
		if(node!=NULL){
			global=get_global(node);
			if(global!=NULL && atoi(global->Name())==0){
				//if it is local and static
				//need to append the file name to its name, to distinguish it from other locals in other files
				((LabelArg*)arg)->initLabel(buffer);
			}
		}
	}
}

void append_file_name_arg(BaseOperation *curDataOp, int pos, int argType, MyString & filename){
	BaseArgument *arg = NULL;
	char *argname;
	defs_node *node,*global;
	char buffer[1000];

	arg = curDataOp->ptrToOperand(pos, argType);

	int store_with_label_offset = 0;

	if (curDataOp->hasOperand(pos))
		arg = curDataOp->ptrToOperand(pos);

	// Changed by SAH on : Tue, Jul 11 2000
	//

	if(arg!=NULL && arg->isLabelArg()==_YES_){
		//append the file name to this arg
		argname=((LabelArg*)arg)->lName();
		sprintf(buffer,"%s_%s",filename.getStr(),argname);
		node=get_node(buffer);
		//_ASSERT(node);
		if(node!=NULL){
			global=get_global(node);
			if(global!=NULL && atoi(global->Name())==0){
				//if it is local and static
				//need to append the file name to its name, to distinguish it from other locals in other files
				((LabelArg*)arg)->initLabel(buffer);
			}
		}
	}
}

//traverses the comiler IR, and replaces the symbols with their mem location
void append_file_names_to_symbols(void){
	MyLinkedListIterator<BaseInstruction *> * iter;
	BaseInstruction *curInstr;
	ArrayIterator<DataOpSlot> *dataSlotIterator;
	ArrayIterator<ControlOpSlot> *cntrlSlotIterator;
	DataOpSlot *curDataSlot;
	ControlOpSlot *curCntrlSlot;
	BaseOperation *curDataOp, *curCntrlOp;
	
	Routine *cRout;
	MyLinkedListIterator<Routine *> *i;

 // Remember to change this function as mentioned below.

/******
******* Comment by SAH on 7th Sept, 2000.
******* Need to change this to incorporate MULTIPLE_DESTS_AND_SOURCES
*******/
	MyString str;
	for(i = curProgram->routineListIteratorForw(); i->hasMoreElements(); i->nextElement())
	{
		cRout = i->currentElement();
		
		// get the filename for this routine.
		//
		cRout->getFileName(str);
		
		for(iter=cRout->instrListIteratorForw();iter->hasMoreElements();iter->nextElement()){
			curInstr = iter->currentElement();
			if (curInstr->isNormalInstr()){
				for(dataSlotIterator = ((NormalInstr *)curInstr)->getDataOpSlotIter();
					dataSlotIterator->hasMoreElements();
					dataSlotIterator->nextElement())
				{
					curDataSlot = & dataSlotIterator->currentElement();
					curDataOp = curDataSlot->getPtrToOperation();
					if(curDataOp!=NULL){
					
					  /*
					  if (curDataOp->isComputeOp()) 
					  {
					  append_file_name_arg(((ComputeOp*)curDataOp),_DEST_, str);
					  append_file_name_arg(((ComputeOp*)curDataOp),_SOURCE_1_, str);
					  append_file_name_arg(((ComputeOp*)curDataOp),_SOURCE_2_, str);
					  }
					  else if (curDataOp->isMemoryOp()) {
					  if (((MemoryOp *)curDataOp)->isLoad()) {
					  append_file_name_arg(((MemoryOp*)curDataOp),_DEST_, str);
					  append_file_name_arg(((MemoryOp*)curDataOp),_SOURCE_1_, str);
					  append_file_name_arg(((MemoryOp*)curDataOp),_SOURCE_2_, str);
					  }
					  else {
					  append_file_name_arg(((MemoryOp*)curDataOp),_DEST_, str);
					  append_file_name_arg(((MemoryOp*)curDataOp),_SOURCE_1_, str);
					  append_file_name_arg(((MemoryOp*)curDataOp),_SOURCE_2_, str);
					  }
					  }
					  else if (curDataOp->isCallOp()) {
					  append_file_name_arg(((ComputeOp*)curDataOp),_DEST_, str);
					  }
					  else 
					  _ASSERT(0);
					  */

					ArgList & a = curDataOp->destOperandList();
						ArgListIter ai;
						int i = 0;
						for (ai = a.begin(); ai != a.end(); ai++)
						{
							if ((*ai) != NULL)
							{
								append_file_name_arg(curDataOp, i, _DEST_LIST_, str);
							}
							i++;
						}
						i = 0;
						ArgList & b = curDataOp->sourceOperandList();
						for (ai = b.begin(); ai != b.end(); ai++)
						{
							if ((*ai) != NULL)
							{
								append_file_name_arg(curDataOp, i, _SRC_LIST_, str);
							}
							i++;
						}
						//
						// end change for MULTIPLE_DESTS_AND_SOURCES
					}
				}
				delete dataSlotIterator;
				
				for(cntrlSlotIterator = ((NormalInstr *)curInstr)->getControlOpSlotIter();
				cntrlSlotIterator->hasMoreElements();
				cntrlSlotIterator->nextElement())
				{
					curCntrlSlot = & cntrlSlotIterator ->currentElement();
					curCntrlOp = curCntrlSlot ->getPtrToOperation();
					if(curCntrlOp!=NULL){
						
						// Changed by SAH : Fri, 18th August, 2000
						//
						// MULTIPLE_DESTS_AND_SOURCES
						//
						ArgList & a = curCntrlOp->destOperandList();
						ArgListIter ai;
						int i = 0;
						for (ai = a.begin(); ai != a.end(); ai++)
						{
							if ((*ai) != NULL)
							{
								append_file_name_arg(curCntrlOp, i, _DEST_LIST_, str);
							}
							i++;
						}
						i = 0;
						ArgList & b = curCntrlOp->sourceOperandList();
						for (ai = b.begin(); ai != b.end(); ai++)
						{
							if ((*ai) != NULL)
							{
								append_file_name_arg(curCntrlOp, i, _SRC_LIST_, str);
							}
							i++;
						}
						//
						// end change for MULTIPLE_DESTS_AND_SOURCES
					}
				}
				delete cntrlSlotIterator;
			}
		}
	}
	delete i;

}

void append_defs_file_names(void){
	append_file_names_to_symbols();
}
