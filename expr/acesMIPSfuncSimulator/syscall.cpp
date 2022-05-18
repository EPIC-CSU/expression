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
/*: syscall.cpp
 *
 * File:  		syscall.cpp
 * Created:		Sun May 21, 2000
 * Author:		Radu Cornea
 * Email:		radu@ics.uci.edu, ilp@ics.uci.edu
 *
 * Syscall helpers
 */ 

#include "syscall.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <io.h>

long fs_heapPointer = FS_HEAP_START_ADDRESS;

void print_syscall(char *syscall_name)
{
	printf("SYSCALL : %s\n", syscall_name);
}

int my_moveOverDecimals(char * & str)
{
	if (str == NULL) return 0;

	char intstr[80];
	int i = 0;

	while(1)
	{
		if ((str[i] >= '0') && (str[i] <= '9'))
		{
			intstr[i] = str[i];
			i++;
		}
		else
			break;
	}

	intstr[i] = '\0';
	str += i;
	if (i > 0)
	{
		return (atoi(intstr));
	}
	return 0;
}

int sys_fopen(reg_t *regs,
			   mem_t mem,
			   md_oper_t *oper,
			   int use_fp)
{
	int r4 = REG_A(PARAM_REG(0));
	int r5 = REG_A(PARAM_REG(1));
	char *s1 = MEM_POINTER_C(REG_A(PARAM_REG(0)));
	char *s2 = MEM_POINTER_C(REG_A(PARAM_REG(1)));
	system("dir");
	FILE * f = fopen(MEM_POINTER_C(REG_A(PARAM_REG(0))), MEM_POINTER_C(REG_A(PARAM_REG(1))));
	return 0;
}

int sys_printf(reg_t *regs,
			   mem_t mem,
			   md_oper_t *oper,
			   int use_fp)
{
	char *p;
	int i = 1; // 0 is the format

	int ival = -1;
	char cval = '\0';
	double dval = -1.0;
	char *sval = NULL;

	int retcount = 0;
	int tempcount;
	int mode = PS_NONE;

	char *fmt;
	FILE *fp;

	if (use_fp) {
		fp = (FILE *) /*MEM_POINTER_C(*/REG_A(PARAMS[0].reg)/*)*/;
		fmt = MEM_POINTER_C(REG_A(PARAMS[1].reg));
		i++;
	} else {
		fp = stdout;
		fmt = MEM_POINTER_C(REG_A(PARAMS[0].reg));
	}

	if (fp == NULL)
	{
		// error condition
		return -1;
	}

	for (p = fmt; *p; p++)
	{
		if (*p == '%')
		{
			mode = PS_ARG;
		}
		else if (*p == '\\')
		{
			mode = PS_SPECIAL;
		}
		else
		{
			putc(*p, fp);
			retcount++;
			continue;
		}
		int precisionNum;
		if (mode == PS_ARG)
		{
			++p;
			if (*p == '.')
			{
				++p;
				precisionNum = my_moveOverDecimals(p);
			}

			switch (*p)
			{
			case 'c' : 
				cval = (char) REG_I(PARAMS[i].reg);

				if ((tempcount = fprintf(fp, "%c", cval)) > 0)
					retcount += tempcount;
				break;
			case 'd' :
			case 'i' :
				ival = (int) REG_I(PARAMS[i].reg);
			
				if ((tempcount = fprintf(fp, "%d", ival)) > 0)
					retcount += tempcount;
				i++;
				break;
			case 'f' :
			case 'g' :
				dval = (double) REG_D(PARAMS[i].reg);
				
				if ((tempcount = fprintf(fp, "%f", dval)) > 0)
					retcount += tempcount;
				i++;
				break;
			case 'e' :
				dval = (double) REG_D(PARAMS[i].reg);
				
				if ((tempcount = fprintf(fp, "%e", dval)) > 0)
					retcount += tempcount;
				i++;
				break;
			case 's' :
				sval = MEM_POINTER_C(REG_A(PARAMS[i].reg));

				if ((tempcount = fprintf(fp, "%s", sval)) > 0)
					retcount += tempcount;
				i++;
				break;
			case 'x' :
				dval = (double) REG_D(PARAMS[i].reg);

				if ((tempcount = fprintf(fp, "%x", dval)) > 0)
					retcount += tempcount;
				i++;
				break;
			case '.' :
			default:
				putc(*p, fp);
				retcount++;
			}
		}
		else if (mode == PS_SPECIAL)
		{
			++p;
			switch (*p)
			{
			case 'n' :
				if ((tempcount = fprintf(fp, "\n")) > 0)
					retcount += tempcount;
				break;
			case 't' :
				if ((tempcount = fprintf(fp, "\t")) > 0)
					retcount += tempcount;
				break;
			default:
				putc(*p, fp);
				retcount++;
			}
		}
	}
	return retcount;
}

int sys_scanf(reg_t *regs,
			  mem_t mem,
			  md_oper_t *oper,
			  int use_fp)
{
	char *p;
	int i = 1;	// 0 is the format

	int ival;
	char cval;
	double dval;
	char *sval;

	int retcount = 0;
	int tempcount;

	int mode = PS_NONE;

		char *fmt;
	FILE *fp;

	if (use_fp) {
		fp = (FILE *) /*MEM_POINTER_C(*/REG_A(PARAMS[0].reg)/*)*/;
		fmt = MEM_POINTER_C(REG_A(PARAMS[1].reg));
		i++;
	} else {
		fp = stdin;
		fmt = MEM_POINTER_C(REG_A(PARAMS[0].reg));
	}

	if (fp == NULL)
	{
		// error condition
		return -1;
	}

	for (p = fmt; *p; p++)
	{
		if (*p == '%')
		{
			mode = PS_ARG;
		}
		else if (*p == '\\')
		{
			mode = PS_SPECIAL;
		}
		else if (*p == ' ') // i.e. whitespace
		{
			// ignore
			getc(fp);
			mode = PS_NONE;
			continue;
		}
		else
		{
			getc(fp);
			mode = PS_NONE;
			continue;
		}
		int precisionNum;
		if (mode == PS_ARG)
		{
			++p;
			if (*p == '.')
			{
				++p;
				precisionNum = my_moveOverDecimals(p);
			}
			switch (*p)
			{
			case 'd' :
			case 'i' :
				if (((tempcount = fscanf(fp, "%d", &ival)) != EOF) && (tempcount > 0))
				{
					MEM_WRITE_I(REG_A(PARAMS[i].reg), ival);
					i++;
					retcount += tempcount;
				}
				break;
			case 'f' :
			case 'g' :
				if (((tempcount = fscanf(fp, "%f", &dval)) != EOF) && (tempcount > 0))
				{
					MEM_WRITE_D(REG_A(PARAMS[i].reg), dval);
					i++;
					retcount += tempcount;
				}
				break;
			case 'e' :
				if (((tempcount = fscanf(fp, "%e", &dval)) != EOF) && (tempcount > 0))
				{
					MEM_WRITE_D(REG_A(PARAMS[i].reg), dval);
					i++;
					retcount += tempcount;
				}
				break;
			case 's' :
				sval = MEM_POINTER_C(REG_A(PARAMS[i].reg));

				if (((tempcount = fscanf(fp, "%s", sval)) != EOF) && (tempcount > 0))
				{
					i++;
					retcount += tempcount;
				}
				break;
			case 'c' :
				if (((tempcount = fscanf(fp, "%c", &cval)) != EOF) && (tempcount > 0))
				{
					MEM_WRITE_I(REG_A(PARAMS[i].reg), cval);
					i++;
					retcount += tempcount;
				}
				break;
			default:
				getc(fp);
				retcount++;
			}
		}
		else if (mode == PS_SPECIAL)
		{
			++p;
			switch (*p)
			{
			case 'n' :
				if ((tempcount = fscanf(fp, "\n")) > 0)
					retcount += tempcount;
				break;
			case 't' :
				if ((tempcount = fscanf(fp, "\t")) > 0)
					retcount += tempcount;
				break;
			default:
				getc(fp);
				retcount++;
			}
		}
	}
	return retcount;
}



void sys_call(reg_t *regs,
			  mem_t mem,
			  md_oper_t *oper)
{
	int sys_code;

	if (PAR_NO == 0)
		return;
	
	sys_code = SRC1.targ;

	switch (sys_code) {

/*
	case SYS_strcmp:
															
		SET_REG_I(RD,											
			  strcmp(MEM_POINTER_C(REG_A(PARAM_REG(0))),	
			  	     MEM_POINTER_C(REG_A(PARAM_REG(1)))));	
		break;

	case SYS_strncmp:												
		SET_REG_I(RD,											
			  strncmp(MEM_POINTER_C(REG_A(PARAM_REG(0))),	
			  	      MEM_POINTER_C(REG_A(PARAM_REG(1))),	
					  REG_I(PARAM_REG(2))));				
		break;

	case SYS_strcpy:
		strcpy(MEM_POINTER_C(REG_A(PARAM_REG(0))),			
	 				MEM_POINTER_C(REG_A(PARAM_REG(1))));			
		SET_REG_A(RD, REG_A(PARAM_REG(0)));					
		break;

	case SYS_bcopy:
		memcpy(MEM_POINTER_C(REG_A(PARAM_REG(1))),
			   MEM_POINTER_C(REG_A(PARAM_REG(0))),
			   REG_I(PARAM_REG(2)));
		break;

	case SYS_strcat:
		strcat(MEM_POINTER_C(REG_A(PARAM_REG(0))),	
		 	      MEM_POINTER_C(REG_A(PARAM_REG(1))));	
		SET_REG_A(RD, REG_A(PARAM_REG(0)));					

		break;

	case SYS_strlen:
		SET_REG_I(RD, strlen(MEM_POINTER_C(REG_A(PARAM_REG(0)))));
		break;

	case SYS_atoi:
		SET_REG_I(RD, atoi(MEM_POINTER_C(REG_A(PARAM_REG(0)))));
		break;

	case SYS_atof:
		SET_REG_D(RD, atof(MEM_POINTER_C(REG_A(PARAM_REG(0)))));
		break;

	case SYS_atol:
		SET_REG_I(RD, atoi(MEM_POINTER_C(REG_A(PARAM_REG(0)))));
		break;

	case SYS_ceil:
		SET_REG_D(RD, ceil(REG_D(PARAM_REG(0))));
		break;

	case SYS_floor:
		SET_REG_D(RD, floor(REG_D(PARAM_REG(0))));
		break;

	case SYS_printf:
		SET_REG_I(RD, sys_printf(regs, mem, oper, 0));								
		break;
*/


#define DEFSYSCALL(SYSCALL_CODE, SYSCALL_NAME)				\
		case SYSCALL_CODE:									\
			print_syscall(SYSCALL_NAME);					\
			SYMCAT(SYSCALL_CODE,_IMPL);						\
			break;
#include "syscall.def"
#undef DEFSYSCALL

		default:
			printf("Invalid/unimplemented system call\n");
			exit(1);
	}

}
