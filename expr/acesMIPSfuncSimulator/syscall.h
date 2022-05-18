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
#ifndef __SYSCALL_H__
#define __SYSCALL_H__


#include "machine.h"

/* constants used by syscalls */
const int FS_STDOUT_HACK = -9980;
const int FS_STDERR_HACK = -9960;
const int FS_MALLOC_ERROR_BUFFER_SIZE = 2;

/* constants used by sys_printf and sys_scanf */
const int PS_NONE = 12345;
const int PS_ARG = 12346;
const int PS_SPECIAL = 12347;

/* system call (and library call) codes */
enum md_syscall_code {
	SYS_NA = 0,		/* N/A */
#define DEFSYSCALL(SYSCALL_CODE, SYSCALL_NAME) SYSCALL_CODE,
#include "syscall.def"
	SYS_MAX			/* number of syscalls */
#undef DEFSYSCALL
};

/* macros */
#define MAKE_SYSCALL_NAME(EXPR_NAME)					\
	SYMCAT(SYS,EXPR_NAME)

#define SYS_CALL(OPER)									\
	sys_call(regs, mem, OPER)

/* forward declarations */
void sys_call(reg_t *regs,
			  mem_t mem,
			  md_oper_t *oper);


#endif	/* __SYSCALL_H__ */