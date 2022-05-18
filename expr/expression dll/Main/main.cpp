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

#ifndef __UNIX__
	#include <io.h>
#else
	#define __declspec(dllexport) 
#endif

#include <stdlib.h>
#include <stdio.h>
#include <iostream.h>


#include "parser.h"
#include "forwrefs.h"
#include "SimulatorGeneratorSupportFuncs.h"
#include "PredicationInfoGen\PredicationInfoGen.h"

extern FILE * yyin, * yyout;

//prototypes
void generate_timings(NodePtr node);


/////////////////////////////////// File redirection ///////////////////////////////

FILE *f=NULL;


void redirect_input(char *file){
	if((f=fopen(file,"r"))==NULL){
      printf( "Can't open file '%s'\n", file);
      abort();
	}
	if(-1==_dup2(_fileno(f),_fileno(stdin))){
		printf( "Can't dup2 file '%s' on stdin\n", file);
		abort();
	}
}


void close_files(void){
	if(f!=NULL)
		fclose(f);
}


void version(void){
	printf("\nEXPRESSION compiler v1.00, May 2003.");
}

/////////////////////////////////// command line options ////////////////////////////

char *filename=NULL;
int printIR=0;
int printPI=0;
int printST=0;
int genRT=0;
int genRegInfo=0;
int genTIM=0;
int genSim=0;
int dbEnabled=0;
int genASM=0;
int genDUMP=0;
int mdes=0;
char *mdes_file_name=NULL;
char *field_map_file=NULL;
char *instr_format_file=NULL;
char *pred_info_file=NULL;

void command_line(int argc, char **argv){
	int i;
	for(i=0;i<argc;i++){
		if(strcmp(argv[i],"-pIR")==0){
			printIR=1;
		}
		else if(strcmp(argv[i],"-pPI")==0){
			printPI=1;
		}
		else if(strcmp(argv[i],"-pST")==0){
			printST=1;
		}		
		else if(strcmp(argv[i],"-RT")==0){
			genRT=1;
		}
		else if(strcmp(argv[i],"-RI")==0){
			genRegInfo=1;
		}
		else if(strcmp(argv[i],"-TIM")==0){
			genTIM=1;
		}
		else if(strcmp(argv[i],"-SIM")==0){
			genSim=1;
		}
		else if(strcmp(argv[i],"-DUMP")==0){
			genDUMP=1;
		}
		else if(strcmp(argv[i],"-ASM")==0){
			genASM=1;
		}
		else if(strcmp(argv[i],"-DB")==0){
			dbEnabled=1;
		}
		else if(strcmp(argv[i],"-REM")==0){
			dbEnabled=2;
		}
		else if(strcmp(argv[i],"-MDES")==0){
			mdes=1;
		}
		else if (strcmp(argv[i], "-MDESFILENAME") == 0){
			mdes_file_name=strdup(argv[i+1]);
			i++;
		}
		else if (strcmp(argv[i], "-FMFILENAME") == 0){
			field_map_file=strdup(argv[i+1]);
			i++;
		}
		else if (strcmp(argv[i], "-IFFILENAME") == 0){
			instr_format_file=strdup(argv[i+1]);
			i++;
		}
		else if(argv[i][0]=='-'){
			char buf[2000];
			sprintf(buf,"Unrecognized command line option %s.",argv[i]);
			compiler_error(buf);
		}
		else{
			filename=strdup(argv[i]);
		}
	}
	if(filename==NULL){
		printf("\nUsage: expression <file name> [options]\n");
		exit(-1);
	}
}


/////////////////////////////////// Main ////////////////////////////////////////////

extern void generateSimulator(NodePtr);
extern void generateRT(NodePtr node);
extern void genMdes(NodePtr node);
extern void predicationInfoGen(NodePtr node);
extern void regInfoGen(NodePtr node);
extern int printdumpIRFile(Symtab st, NodePtr node);
extern int printOperandsMappingFile(Symtab st, NodePtr node);


NodePtr forwRefNodes;


//This function used to be the main in the expression project
//Now it is invoked either by the main form the expression console,
//or by the compiler, in order to initialize the expression data
//structures. 
//Once this has been called, on the fly reservation tables generation can be called. 
__declspec( dllexport ) int expression_main(int argc, char **argv)
{
	MyString mystr;

	version();
	
	command_line(argc,argv);


/*	if((yyin=fopen(filename,"r"))==NULL){
		printf( "Can't open file '%s'\n",filename);
		abort();
	}
*/	
	redirect_input(filename);
	yyparse(); //ir contains the whole EXPRESSION description
	close_files();
	


	if(ir!=NULL){ // if no syntax error
		
		printf("\n%s: 0 error(s), 0 warning(s).\n",filename);

		//update forward references to symbols (check if they are declared)
		forwrefs(ir);
		st.patchForwRefs();

		if(printIR) ir->Print();

		if(printST) st.Print();

		//generate simulator
		if(genSim) generateSimulator(ir);

		//generate reservation tables
		if(genRT) generateRT(ir);

		if(genTIM) generate_timings(ir);

		if(mdes) genMdes(ir);

		if(printPI) predicationInfoGen(ir);

		if (genRegInfo)  regInfoGen(ir);

		if(genASM) printOperandsMappingFile(st, ir);

		if(genDUMP) printdumpIRFile(st, ir);

		printf("\n");

		return 0;
	}
	else{
		report_errors(filename);
		return -1;
	}
}
