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
#ifndef __CALL_H__
#define __CALL_H__

#include "machine.h"
#include "STLIncludes.h"

/* program calling stack */
struct s_call_stack {
	md_addr_t	pc;
	md_addr_t	routine;
};
extern int saved_reg_stack_sp;

/* return address call stack */
struct s_call_stack call_stack[];
extern int			call_stack_sp;

/* registers for passing parameters (currently R4 and R5) */
extern int	param_regs[];

/* calls */

typedef struct s_saved_regs {
	int			saved_reg_no;
	int			*saved_regs;
} md_saved_regs_t;

/* mapping between calls (index of instr.) and saved registers */
extern map<int, md_saved_regs_t*> indexToSavedRegsMap;

/* macros definition for call purposes */
#define SPEC_CALL(OPER)									\
	spec_call(OPER)

#define DO_CALL_STUFF(TARG)								\
	do_call_stuff(TARG)

#define DO_RET_STUFF()									\
	do_ret_stuff()

#define PUSH_PC(PC)										\
	(call_stack[call_stack_sp].pc = PC,					\
	call_stack[call_stack_sp++].routine = crt_routine)

#define POP_PC()										\
	(crt_routine = call_stack[--call_stack_sp].routine,	\
	 call_stack[call_stack_sp].pc)

#define PUSH_REG(REG)									\
	saved_reg_stack[saved_reg_stack_sp++] = REG

#define POP_REG()										\
	saved_reg_stack[--saved_reg_stack_sp]

#define END_OF_PROGRAM()								\
	(call_stack_sp == 0)

#define IS_SYSCALL(ADDR)								\
	(ADDR < 100)


/* forward declarations */
void spec_call(md_oper_t *oper);
void do_call_stuff(md_addr_t addr);
void do_ret_stuff();


#endif	/* __CALL_H__ */