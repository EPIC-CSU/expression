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
/*: call.cpp
 *
 * File:  		call.cpp
 * Created:		Sun May 21, 2000
 * Author:		Radu Cornea
 * Email:		radu@ics.uci.edu, ilp@ics.uci.edu
 *
 * Functions for handling calls
 */ 

#include "call.h"
#include "machine.h"
#include <stdio.h>

/* registers for passing parameters (currently R4 and R5) */
int	param_regs[2];

/* stack for saving registers on a call */
reg_t		saved_reg_stack[1024];
int			saved_reg_stack_sp;

/* mapping between calls (index of instr.) and saved registers */
map<int, md_saved_regs_t*> indexToSavedRegsMap;

/* return address call stack */
struct s_call_stack call_stack[256];
int			call_stack_sp;

/* handler for special calls (DEBUG_PRINT currently) */
void spec_call(md_oper_t *oper)
{
	printf("#-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-#\n");
	printf("DEBUG_PRINT: ");
	
	/* print all parameters as integers */
	static int i;
	for (i = 0; i < PAR_NO; i++)
	{
		printf("%d :: ", REG_I(PARAMS[i].reg));
	}
	
	printf("\n#-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-#\n");
}

/* save registers before the call */
void do_call_stuff(md_addr_t addr)
{
	/* save registers */
	md_saved_regs_t *saved = indexToSavedRegsMap[addr];
	int reg_no = saved->saved_reg_no;
	int *reg_i = saved->saved_regs;

	int i;
	for (i = 0; i < reg_no; i++) {
		saved_reg_stack[saved_reg_stack_sp++].i = regs[reg_i[i]].i;
	}

	/* save arguments in R4[.1] and R5[.1] */
	for (i = 0; (i < PAR_NO) && (i < 2); i++) {
		SET_REG_I(param_regs[i], REG_I(PARAMS[i].ival));
	}

	crt_routine = addr;
}

/* restore registers after the call */
void do_ret_stuff()
{		
	/* restore registers */
	/* crt_routine still points to the called routine */
	md_saved_regs_t *saved = indexToSavedRegsMap[crt_routine];
	int reg_no = saved->saved_reg_no;
	int *reg_i = saved->saved_regs;

	for (int i = reg_no - 1; i >= 0; i--) {
		regs[reg_i[i]].i = saved_reg_stack[--saved_reg_stack_sp].i;
	}
}
