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
/*: FuncSim.cpp
 *
 * File:  		FuncSim.cpp
 * Created:		Sat March 11, 2000
 * Author:		Radu Cornea
 * Email:		radu@ics.uci.edu, ilp@ics.uci.edu
 *
 * Functional Simulator main file
 */ 

#include "FuncSim.h"
#include "machine.h"
#include "translator.h"
#include "call.h"
#include "syscall.h"
#include "MyString.h"

#include "STLIncludes.h"
#include <time.h>
#include <assert.h>

/* instruction pointers */
md_addr_t	reg_PC, reg_NPC;

/* register file and memory */
regfile_t	regs;
mem_t		mem;

/* pointers to current structures */
md_prog_t	sim_program;
md_addr_t	crt_routine;
md_instr_t		*instr;
md_oper_t		*oper;

/* execution cycles */
unsigned long	cycles;

int				operNo;

/* end of simulation? */
bool sim_finished;

extern unsigned long lockstep_clock_count;

/* time counting variables */
clock_t my_start1,my_stop1;
double  my_duration=0;

/* start counting */
void my_start_time(void){
	//start counting time
	my_start1 = clock();
}

/* reset time to 90 */
void my_reset_time(void)
{
	my_duration = 0;
}

/* pause time counter */
void my_pause_time(void){
	my_stop1=clock();
	my_duration +=(double)(my_stop1 - my_start1);
}

/* display counted time */
void my_print_time(void){
	printf( "\n***TIME*** : %5.2f secs\n", my_duration / (double)CLOCKS_PER_SEC );
}

/* display counted speed */
void my_print_speed(void){
	printf( "\n***SPEED*** : %10.2f cycles/sec\n", cycles / (my_duration / (double)CLOCKS_PER_SEC) );
}

extern MyString globalProgramName;
extern void initializeMemory(long, char *, char *);
extern md_addr_t translateProgramFromDump();

/* main simulator function */
unsigned long funcSimulateProgram(Program *curP, char *mesg)	// mesg = "SIMULATING")
{
	/* allocate memory for mem */
	mem = (mem_t) malloc(MD_MEM_SIZE * sizeof byte_t);

	assert(mem);

	/* translate initial program to funcSim internal representation */
	int startNo;
	long pgmstrtaddr = 0;
	MyString tmpFileName = globalProgramName.getStr();
	tmpFileName.concat(mesg);
    initializeMemory(pgmstrtaddr, "both", tmpFileName.getStr());
	printf("Translating program...\n");
	// This routine translates into func sim from IR dump
	startNo = translateProgramFromDump();
	// startNo= translateProgram(curP);

	/* some initializations */
	call_stack_sp = 0;
	saved_reg_stack_sp = 0;
	cycles = 0;
	sim_finished = 0;

	SET_CPC(startNo);
	SET_NPC(startNo + 1 );

	lockstep_clock_count = 0;

	printf("\n**********************************************\n");
	printf("Start functional simulation : %s\n", mesg);

	my_start_time();

	/* loop through the program */
	while (!FINISHED()) {
		/* get instruction at current PC */
		instr = &sim_program[CPC];

		/* get default target */
		SET_NPC(instr->next);
	
		for (operNo = 0; operNo < instr->oper_no; operNo++) {
			/* get current operation */
			oper = &instr->oper[operNo];

			/* execute the operation */
			switch (oper->opcode) {
#define DEFINST(OP, NAME, FLAGS)							\
			case OP:										\
				DBG_OPER(oper, NAME, cycles);				\
				SYMCAT(OP,_IMPL);							\
				break;
#include "machine.def"
#undef DEFINST
			default:
				printf("Unknown opcode : %d\n", oper->opcode);
				return 0;
			}
		}

		/* go to next instruction */
		SET_CPC(NPC);
		cycles++;
	}

	/* print elapsed time and speed */
	my_pause_time();
	my_print_time();
	my_print_speed();

	printf ("\n End functional simulation : %s\n", mesg);
	printf("**********************************************\n");

	/* clean up memory */
	free(sim_program);

	/* clean up mem */
	free(mem);

	return cycles;
}
