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
#ifndef __MACHINE_H__
#define __MACHINE_H__

#include "host.h"


/* number of operations per instruction */
#define OPER_PER_INSTR		10

/* number of integer registers */
#define MD_NUM_IREGS		32

/* number of floating point registers */
#define MD_NUM_FREGS		32

/* number of control registers */
#define MD_NUM_CREGS		3

/* total number of registers, excluding PC and NPC */
#define MD_REGFILE_SIZE							\
  (/*int*/32 + /*fp*/32 + /*misc*/2 + /*tmp*/1 + /*mem*/1 + /*ctrl*/1 + /* generic*/229)

/* memory capacity */
#define MD_MEM_SIZE			256000000

/* stack/heap start */
#define FS_STACK_START_ADDRESS	MD_MEM_SIZE - 1
#define FS_HEAP_START_ADDRESS 10000004

/* argv/argc related */
#define FS_argCRegNum					4
#define FS_argVRegNum					5
#define FS_ARGV_START_LOCATION			100000
#define FS_ARGV_STRING_START_LOCATION	101000
#define FS_POINTER_INCREMENT			4

/* not applicable/available, usable in most definition contexts */
#define NA		0

enum md_opcode {
	OP_NA = 0,	/* N/A */
#define DEFINST(OP, NAME, FLAGS) OP,
#include "machine.def"
	OP_MAX		/* number of opcodes * NA */
#undef DEFINST
};

/* address type */
typedef	unsigned long	md_addr_t;

/* register type */
typedef union u_reg_t {
	char c;
	int i;
	long l;
	float f;
	double d;
	md_addr_t a;
} reg_t;

/* type agnostic register file */
typedef reg_t	regfile_t[MD_REGFILE_SIZE];


/* the next macros assume that the register state is kept in 'regs' */

/* general purpose registers */
#define REG_I(N)												\
	(DBG_MSG31("DEBUG: regs[%d].i => %d\n", N, (regs[N].i)),	\
	(regs[N].i))

#define SET_REG_I(N, EXPR)										\
	((regs[N].i = (EXPR)),										\
	DBG_MSG3("DEBUG: regs[%d].i <= %d\n", N, regs[N].i))


#define REG_A(N)												\
	(DBG_MSG31("DEBUG: regs[%d].a => %d\n", N, (regs[N].a)),		\
	(regs[N].a))


#define SET_REG_A(N, EXPR)										\
	((regs[N].a = (EXPR)),										\
	DBG_MSG3("DEBUG: regs[%d].a <= %d\n", N, regs[N].a))
	


/* floating point registers, L->word, F->single-prec, D->double-prec */
#define REG_F(N)												\
	(DBG_MSG31("DEBUG: regs[%d].f => %d\n", N, (regs[N].f)),		\
	(regs[N].f))

#define SET_REG_F(N, EXPR)										\
	((regs[N].f = (EXPR)),										\
	DBG_MSG3("DEBUG: regs[%d].f <= %g\n", N, regs[N].f))
	


#define REG_D(N)												\
	(DBG_MSG31("DEBUG: regs[%d].d => %d\n", N, (regs[N].d)),		\
	(regs[N].d))

#define SET_REG_D(N, EXPR)										\
	((regs[N].d = (EXPR)),										\
	DBG_MSG3("DEBUG: regs[%d].d <= %g\n", N, regs[N].d))
	


/* control registers accesors */

/* current program counter */
#define CPC					(reg_PC)
#define SET_CPC(EXPR)		(reg_PC = (EXPR))

/* next program counter */
#define NPC					(reg_NPC)
#define SET_NPC(EXPR)		(reg_NPC = (EXPR))


/* memory state */
typedef byte_t	*mem_t;

/* memory accessors macros */

#define MEM_POINTER(MEM, ADDR, TYPE)						\
	((TYPE *) (MEM + (ADDR)))

#define MEM_READ(MEM, ADDR, TYPE)							\
	(*MEM_POINTER(MEM, ADDR, TYPE))

#define MEM_WRITE(MEM, ADDR, TYPE, VAL)						\
	((*MEM_POINTER(MEM, ADDR, TYPE)) = (VAL))


/* fast memory accessor macros, typed versions */

/* pointers */
#define MEM_POINTER_C(ADDR)		MEM_POINTER(mem, ADDR, char)
#define MEM_POINTER_V(ADDR)		MEM_POINTER(mem, ADDR, void)

#define MEM_READ_C(ADDR)										\
	(DBG_MSG31("DEBUG: mem[%d].c => %d\n", ADDR,				\
			  MEM_READ(mem, ADDR, char)),						\
	 MEM_READ(mem, ADDR, char))

	
/* reads */
#define MEM_READ_I(ADDR)										\
	(DBG_MSG31("DEBUG: mem[%d].i => %d\n", ADDR,				\
			  MEM_READ(mem, ADDR, int)),						\
	 MEM_READ(mem, ADDR, int))
	

#define MEM_READ_H(ADDR)										\
	(DBG_MSG31("DEBUG: mem[%d].h => %d\n", ADDR,				\
			  MEM_READ(mem, ADDR, half_t)),						\
	 MEM_READ(mem, ADDR, half_t))
	

#define MEM_READ_W(ADDR)										\
	(DBG_MSG31("DEBUG: mem[%d].w => %d\n", ADDR,				\
			  MEM_READ(mem, ADDR, word_t)),						\
	 MEM_READ(mem, ADDR, word_t))
	

#define MEM_READ_F(ADDR)										\
	(DBG_MSG31("DEBUG: mem[%d].f => %f\n", ADDR,				\
			  MEM_READ(mem, ADDR, float)),						\
	 MEM_READ(mem, ADDR, float))
	

#define MEM_READ_D(ADDR)										\
	(DBG_MSG31("DEBUG: mem[%d].d => %f\n", ADDR,				\
			  MEM_READ(mem, ADDR, double)),						\
	 MEM_READ(mem, ADDR, double))
	

/* writes */
#define MEM_WRITE_C(ADDR, VAL)									\
	(MEM_WRITE(mem, ADDR, char, VAL),							\
	DBG_MSG3("DEBUG: mem[%d].c <= %d\n", ADDR,					\
			 MEM_READ(mem, ADDR, char)))
	 

#define MEM_WRITE_I(ADDR, VAL)									\
	(MEM_WRITE(mem, ADDR, int, VAL),							\
	DBG_MSG3("DEBUG: mem[%d].i <= %d\n", ADDR,					\
			 MEM_READ(mem, ADDR, int)))


#define MEM_WRITE_B(ADDR, VAL)									\
	(MEM_WRITE(mem, ADDR, byte_t, VAL),							\
	DBG_MSG3("DEBUG: mem[%d].b <= %d\n", ADDR,					\
			 MEM_READ(mem, ADDR, byte_t)))
	 

#define MEM_WRITE_H(ADDR, VAL)									\
	(MEM_WRITE(mem, ADDR, half_t, VAL),							\
	DBG_MSG3("DEBUG: mem[%d].h <= %d\n", ADDR,					\
			 MEM_READ(mem, ADDR, half_t)))
	 

#define MEM_WRITE_W(ADDR, VAL)									\
	(MEM_WRITE(mem, ADDR, word_t, VAL),							\
	DBG_MSG3("DEBUG: mem[%d].w <= %d\n", ADDR,					\
			 MEM_READ(mem, ADDR, word_t)))
	 

#define MEM_WRITE_F(ADDR, VAL)									\
	(MEM_WRITE(mem, ADDR, float, VAL),							\
	DBG_MSG3("DEBUG: mem[%d].f <= %g\n", ADDR,					\
			 MEM_READ(mem, ADDR, float)))
	 

#define MEM_WRITE_D(ADDR, VAL)									\
	(MEM_WRITE(mem, ADDR, double, VAL),							\
	DBG_MSG3("DEBUG: mem[%d].d <= %g\n", ADDR,					\
			 MEM_READ(mem, ADDR, double)))
	 

/* all the next macros assume we are executing instruction 'instr', 
 * operation 'oper', which are defined
 */

/* register specifiers */
#define RS1		(oper->src1.reg)
#define RS2		(oper->src2.reg)
#define RS3		(oper->src3.reg)
#define RD		(oper->dest.reg)

/* immediate field value */
#define IMM1_I		(oper->src1.ival)
#define IMM2_I		(oper->src2.ival)
#define IMM_DEST_I	(oper->dest.ival)

#define IMM1_D		(oper->src1.dval)
#define IMM2_D		(oper->src2.dval)
#define IMM_DEST_D	(oper->dest.dval)

#define IMM1_F		(oper->src1.fval)
#define IMM2_F		(oper->src2.fval)
#define IMM_DEST_F	(oper->dest.fval)

#define IMM1_A		(oper->src1.addr)
#define IMM2_A		(oper->src2.addr)
#define IMM_DEST_A	(oper->dest.addr)

#define OPFLAG_IMM1	1
#define OPFLAG_IMM2	2
#define HAS_IMM1()	(oper->flags & OPFLAG_IMM1)
#define HAS_IMM2()	(oper->flags & OPFLAG_IMM2)

/* target argument */
#define TARG1	(oper->src1.targ)
#define TARG2	(oper->src2.targ)

/* arguments */
#define SRC1	(oper->src1)
#define SRC2	(oper->src2)
#define DEST	(oper->dest)

/* parameter list */
#define PAR_NO	(oper->src2.plist->parno)
#define PARAMS	(oper->src2.plist->params)

#define PARAM_REG(I)	(PARAMS[I].reg)

/* forward declaration */
struct s_par_list;

/* instruction argument */
typedef union u_md_arg_t {
	long	ival;
	float	fval;      
	double	dval;    
	md_addr_t	addr;
	int		reg;
	md_addr_t	targ;
	struct s_par_list	*plist;
} md_arg_t;

/* parameter list */
typedef struct s_par_list {
	int			parno;
	md_arg_t	*params;
} md_plist_t;

/* operation format */
typedef struct {
	md_opcode	opcode;
	md_arg_t	dest;
	md_arg_t	src1;
	md_arg_t	src2;
	md_arg_t	src3;
	long		flags;
} md_oper_t;

/* instruction format */
typedef struct {
	md_oper_t	oper[OPER_PER_INSTR];
	int			oper_no;
	long		next;
} md_instr_t;

/* program type */
typedef md_instr_t *md_prog_t;

/* madros */
#ifdef DEBUG
#define DBG_OPER(OP, NAME, CLK)											\
	printf("\nPC=%d OPC=%s(%d) D=%d S1=%d S2=%d FLAGS=%d CLK=%ld\n",	\
		CPC, NAME, OP->opcode, OP->dest.ival,							\
		OP->src1.ival, OP->src2.ival, OP->flags, CLK)
#else
#define DBG_OPER(OP, NAME, CLK)
#endif

#define SET_FINISHED()									\
	(sim_finished = 1)

#define FINISHED()										\
	(sim_finished == 1)

/* forward declarations */
extern bool		sim_finished;
extern regfile_t	regs;
extern mem_t		mem;
extern md_oper_t	*oper;
extern md_addr_t	crt_routine;
extern md_prog_t	sim_program;

#endif	/* __MACHINE_H__ */