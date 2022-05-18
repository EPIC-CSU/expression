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
/*************************************************************
 *file  : SystemCallSupport.cpp                              *
 *created : Nov 22, 99                                       *
 *authors : Ashok Halambi                                    *
 *last modified :                                            *
 *                                                           *
 *compiled with : VC++                                       *
 *for more info :                                            *
 *************************************************************/

#include "ArchClassIncludes.h"
#include "DerivedClassIncludes.h"
#include "ExpressAddrData.h"

#include "SysCallExecSem.h"
#include "SimCall.h"

// This file contians code for executing system calls during simulation.
//

Vector<Val> sysCallInputArgs;
//
// This array contains the list of inputs for the system call.

Vector<Val> sysCallOutputArgs;
//
// This array holds the result (if any) of a system call.


Vector<SysSideEffect> sysCallSideEffects;
//
// This array holds the side effects (if any) of a system call.
// Example of a side effect is scanf, which modifies memory while returning a number indicating success.

Map<int, int> sysCallArgToTypeMap;
//
// This map holds the conversion type of each argument.
//
// Each argument is initially a register value. However, that value may, in turn,
// be expected to hold an address.
// For example, printf: 1st argument is actually a pointer to a char.

int numSysCallsImplemented = 0;

void resetSysCallArgs()
{
	sysCallInputArgs.clear();
	sysCallOutputArgs.clear();
	sysCallSideEffects.clear();
	sysCallArgToTypeMap.clear();
}

SysCallPtrObjMap systemCalls;
//
// This array holds the function objects of all the system calls that are defined.

Vector<MyString> systemCallsIndex;
//
// This array holds the names of all the system calls that are defined.
// The difference between this and the systemCalls array is that this is indexed
// by an integer and returns the name of the system call,
// while systemCalls is indexed by the name of the call and returns the function pointer.

void makeSystemCall(MyString & s)
{
	SysCallPtr sptr = systemCalls[s];

	if (sptr != NULL)
	{
		(*sptr)();
	}
}

void makeSystemCall(int index)
{
	SysCallPtr sptr = systemCalls[systemCallsIndex[index]];

	if (sptr != NULL)
	{
		(*sptr)();
	}
}

int checkSysCallAndSetOperands(MyString & s)
{
	SysCallPtr sptr = systemCalls[s];

	if (sptr != NULL)
	{
		return ((*sptr).checkAndSetOperands());
	}
	assert(0);
	return _SUCCESS_;
}

int isSystemCall(MyString & s)
{
	SysCallPtrObjMap::iterator i;

	if ((i = systemCalls.find(s)) != systemCalls.end())
		return _YES_;

	return _NO_;
}

int isSystemCallIndex(int index)
{
	if ((index >= 0) && (index < numSysCallsImplemented))
		return _YES_;

	return _NO_;
}

void addSystemCall(MyString & s, SysCallPtr & p)
{
	systemCalls[s] = p;
	systemCallsIndex.push_back(s);
	numSysCallsImplemented++;
}

MyString & getSystemCallName(int index)
{
	return systemCallsIndex[index];
}

void getSystemCallName(Val & a, MyString & s)
{
	if (a._valType == _INTERNAL_LABEL)
		s = a._as.label;

	if (a._valType == _INT)
		s = getSystemCallName(a._as.integer);
}

int getSystemCallNameAsInt(char *name)
{
	MyString s(name);

	// Now, iterate over the systemCallsIndex table and get the index.
	//
	for (int i = 0; i < numSysCallsImplemented; i++)
	{
		if (s == systemCallsIndex[i])
			return i;
	}
	return _INVALID_;
}



int myCheckAndSetForIO(char *fmt, int firstpos);

int Do_fopen::checkAndSetOperands()
{
	if (sysCallInputArgs.size() != 2)
	{
		// error condition. We expect to have 2 input args to strcmp
		//
		cout << "Panic: System call to fopen without appropriate (2) number of input args" << endl;
		assert(0);
	}

	// _SOURCE_1_ (0) and _SOURCE_2_ (1) are char pointers.
	//
	if (!isStringType(sysCallInputArgs[0]))
	{
		sysCallArgToTypeMap[0] = CHAR_POINTER;
		return _FAILURE_;
	}

	if (!isStringType(sysCallInputArgs[1]))
	{
		sysCallArgToTypeMap[1] = CHAR_POINTER;
		return _FAILURE_;
	}
	return _SUCCESS_;
}

int Do_fclose::checkAndSetOperands()
{
	if (sysCallInputArgs.size() != 1)
	{
		// error condition. We expect to have 1 input args to fclose
		//
		cout << "Panic: System call to fclose without appropriate (1) number of input args" << endl;
		assert(0);
	}

	return _SUCCESS_;
}

int Do_fseek::checkAndSetOperands()
{
	if (sysCallInputArgs.size() != 3)
	{
		// error condition. We expect to have 3 input args to fseek
		//
		cout << "Panic: System call to fseek without appropriate (3) number of input args" << endl;
		assert(0);
	}

	return _SUCCESS_;
}

int Do_rewind::checkAndSetOperands()
{
	if (sysCallInputArgs.size() != 1)
	{
		// error condition. We expect to have 1 input args to rewind
		//
		cout << "Panic: System call to rewind without appropriate (1) number of input args" << endl;
		assert(0);
	}

	return _SUCCESS_;
}

int Do_printf::checkAndSetOperands()
{
	if (sysCallInputArgs.size() < 1) // we expect atleast 1 argument.
	{
		// error condition.
		//
		cout << "Panic: Call to printf without appropriate (>=1) number of input args" << endl;
		assert(0);
	}

	// _SOURCE_1_ is a char pointer.
	//
	if (!isStringType(sysCallInputArgs[0]))
	{
		sysCallArgToTypeMap[0] = CHAR_POINTER;
		return _FAILURE_;
	}
	else //	if (isStringType(sysCallInputArgs[0])
		return (myCheckAndSetForIO(getStringValue(sysCallInputArgs[0]), 1));

	return _SUCCESS_;
}

// TBD. This method is not yet complete.
//
int Do_scanf::checkAndSetOperands()
{
	if (sysCallInputArgs.size() < 1) // we expect atleast 1 argument.
	{
		// error condition.
		//
		cout << "Panic: Call to scanf without appropriate (>=1) number of input args" << endl;
		assert(0);
	}

	return _SUCCESS_;
}

int Do_fprintf::checkAndSetOperands()
{
	if (sysCallInputArgs.size() < 2) // we expect atleast 2 arguments.
	{
		// error condition.
		//
		cout << "Panic: Call to printf without appropriate (>=2) number of input args" << endl;
		assert(0);
	}

	// _SOURCE_2_ is a char pointer.
	//
	if (!isStringType(sysCallInputArgs[1]))
	{
		sysCallArgToTypeMap[1] = CHAR_POINTER;
		return _FAILURE_;
	}
	else
		return (myCheckAndSetForIO(getStringValue(sysCallInputArgs[1]), 2));

	return _SUCCESS_;
}

// TBD. This method is not yet complete.
//
int Do_sscanf::checkAndSetOperands()
{
	if (sysCallInputArgs.size() < 2) // we expect atleast 2 arguments.
	{
		// error condition.
		//
		cout << "Panic: Call to sscanf without appropriate (>=2) number of input args" << endl;
		assert(0);
	}

	// _SOURCE_1_ is a char pointer.
	//
	if (!isStringType(sysCallInputArgs[0]))
	{
		sysCallArgToTypeMap[0] = CHAR_POINTER;
		return _FAILURE_;
	}

	// _SOURCE_2_ is a char pointer.
	//
	if (!isStringType(sysCallInputArgs[1]))
	{
		sysCallArgToTypeMap[1] = CHAR_POINTER;
		return _FAILURE_;
	}

	return _SUCCESS_;
}

int Do_fscanf::checkAndSetOperands()
{
	if (sysCallInputArgs.size() < 2) // we expect atleast 2 arguments.
	{
		// error condition.
		//
		cout << "Panic: Call to fscanf without appropriate (>=2) number of input args" << endl;
		assert(0);
	}

	// _SOURCE_2_ is a char pointer.
	//
	if (!isStringType(sysCallInputArgs[1]))
	{
		sysCallArgToTypeMap[1] = CHAR_POINTER;
		return _FAILURE_;
	}

	return _SUCCESS_;
}

int Do_read::checkAndSetOperands()
{
	if (sysCallInputArgs.size() != 3) // we expect exactly 3 arguments.
	{
		// error condition.
		//
		cout << "Panic: Call to read without appropriate (==3) number of input args" << endl;
		assert(0);
	}

	return _SUCCESS_;
}

int Do_fread::checkAndSetOperands()
{
	if (sysCallInputArgs.size() != 4) // we expect exactly 4 arguments.
	{
		// error condition.
		//
		cout << "Panic: Call to fread without appropriate (==3) number of input args" << endl;
		assert(0);
	}

	return _SUCCESS_;
}

int Do_fwrite::checkAndSetOperands()
{
	if (sysCallInputArgs.size() != 4) // we expect exactly 4 arguments.
	{
		// error condition.
		//
		cout << "Panic: Call to fwrite without appropriate (==4) number of input args" << endl;
		assert(0);
	}

	// _SOURCE_1_ is actually a void pointer.
	// However, we store it as a char pointer.
	//
	if (!isStringType(sysCallInputArgs[0]))
	{
		sysCallArgToTypeMap[0] = VOID_POINTER;
		return _FAILURE_;
	}

	return _SUCCESS_;
}

int Do_fputs::checkAndSetOperands()
{
	if (sysCallInputArgs.size() < 2) // we expect atleast 2 arguments.
	{
		// error condition.
		//
		cout << "Panic: Call to fputs without appropriate (>=2) number of input args" << endl;
		assert(0);
	}

	// _SOURCE_2_ is a char pointer.
	//
	if (!isStringType(sysCallInputArgs[1]))
	{
		sysCallArgToTypeMap[1] = CHAR_POINTER;
		return _FAILURE_;
	}

	return _SUCCESS_;
}

int Do_puts::checkAndSetOperands()
{
	if (sysCallInputArgs.size() != 1) // we expect 1 argument.
	{
		// error condition.
		//
		cout << "Panic: Call to puts without appropriate (=1) number of input args" << endl;
		assert(0);
	}

	// _SOURCE_1_ is a char pointer.
	//
	if (!isStringType(sysCallInputArgs[0]))
	{
		sysCallArgToTypeMap[0] = CHAR_POINTER;
		return _FAILURE_;
	}

	return _SUCCESS_;
}

int Do_getc::checkAndSetOperands()
{
	if (sysCallInputArgs.size() != 1) // we expect exactly one argument.
	{
		// error condition.
		//
		cout << "Panic: Call to getc without appropriate (1) number of input args" << endl;
		assert(0);
	}

	return _SUCCESS_;
}

int Do_open::checkAndSetOperands()
{
	if (sysCallInputArgs.size() < 2) // we expect atleast 2 arguments.
	{
		// error condition.
		//
		cout << "Panic: Call to open without appropriate (>=2) number of input args" << endl;
		assert(0);
	}

	// _SOURCE_1_ is a char pointer.
	//
	if (!isStringType(sysCallInputArgs[0]))
	{
		sysCallArgToTypeMap[0] = CHAR_POINTER;
		return _FAILURE_;
	}

	return _SUCCESS_;
}

int Do_close::checkAndSetOperands()
{
	if (sysCallInputArgs.size() != 1) // we expect atleast 1 argument.
	{
		// error condition.
		//
		cout << "Panic: Call to close without appropriate (==1) number of input args" << endl;
		assert(0);
	}

	return _SUCCESS_;
}

int Do__flsbuf::checkAndSetOperands()
{
	if (sysCallInputArgs.size() != 2) // we expect exactly one argument.
	{
		// error condition.
		//
		cout << "Panic: Call to _flsbuf without appropriate (2) number of input args" << endl;
		assert(0);
	}

	return _SUCCESS_;
}

int Do__filbuf::checkAndSetOperands()
{
	if (sysCallInputArgs.size() != 1) // we expect exactly one argument.
	{
		// error condition.
		//
		cout << "Panic: Call to _filbuf without appropriate (1) number of input args" << endl;
		assert(0);
	}

	return _SUCCESS_;
}

int Do_strcmp::checkAndSetOperands()
{
	// _SOURCE_1_ (0) and _SOURCE_2_ (1) are char pointers.
	//
	if (!isStringType(sysCallInputArgs[0]))
	{
		sysCallArgToTypeMap[0] = CHAR_POINTER;
		return _FAILURE_;
	}

	if (!isStringType(sysCallInputArgs[1]))
	{
		sysCallArgToTypeMap[1] = CHAR_POINTER;
		return _FAILURE_;
	}

	return _SUCCESS_;
}

int Do_strcpy::checkAndSetOperands()
{
	// _SOURCE_1_ (0) is a char pointer that gets written into
	// and _SOURCE_2_ (1) is an input char pointer.
	// So, check to see if only _SOURCE_2_ is a char pointer.
	//

	if (!isStringType(sysCallInputArgs[1]))
	{
		sysCallArgToTypeMap[1] = CHAR_POINTER;
		return _FAILURE_;
	}

	return _SUCCESS_;
}

int Do_bcopy::checkAndSetOperands()
{
	// _SOURCE_1_ (0) is a void pointer that gets written into
	// and _SOURCE_2_ (1) is an input void pointer.
	// So, check to see if only _SOURCE_2_ is a char pointer.
	//

	if (!isStringType(sysCallInputArgs[1]))
	{
		sysCallArgToTypeMap[1] = VOID_POINTER;
		return _FAILURE_;
	}

	return _SUCCESS_;
}

int Do_atoi::checkAndSetOperands()
{
	if (sysCallInputArgs.size() != 1)
	{
		// error condition. We expect to have 1 input args to atoi
		//
		cout << "Panic: System call to atoi without appropriate (1) number of input args" << endl;
		assert(0);
	}

	// _SOURCE_1_ is a char pointer.
	//
	if (!isStringType(sysCallInputArgs[0]))
	{
		sysCallArgToTypeMap[0] = CHAR_POINTER;
		return _FAILURE_;
	}

	return _SUCCESS_;
}

int Do_atof::checkAndSetOperands()
{
	if (sysCallInputArgs.size() != 1)
	{
		// error condition. We expect to have 1 input args to atof
		//
		cout << "Panic: System call to atof without appropriate (1) number of input args" << endl;
		assert(0);
	}

	// _SOURCE_1_ is a char pointer.
	//
	if (!isStringType(sysCallInputArgs[0]))
	{
		sysCallArgToTypeMap[0] = CHAR_POINTER;
		return _FAILURE_;
	}

	return _SUCCESS_;
}

int Do_atol::checkAndSetOperands()
{
	if (sysCallInputArgs.size() != 1)
	{
		// error condition. We expect to have 1 input args to atol
		//
		cout << "Panic: System call to atol without appropriate (1) number of input args" << endl;
		assert(0);
	}

	// _SOURCE_1_ is a char pointer.
	//
	if (!isStringType(sysCallInputArgs[0]))
	{
		sysCallArgToTypeMap[0] = CHAR_POINTER;
		return _FAILURE_;
	}

	return _SUCCESS_;
}

int Do_ceil::checkAndSetOperands()
{
	if (sysCallInputArgs.size() != 1)
	{
		// error condition. We expect to have 1 input args to ceil
		//
		cout << "Panic: System call to ceil without appropriate (1) number of input args" << endl;
		assert(0);
	}

	return _SUCCESS_;
}

int Do_floor::checkAndSetOperands()
{
	if (sysCallInputArgs.size() != 1)
	{
		// error condition. We expect to have 1 input args to floor
		//
		cout << "Panic: System call to floor without appropriate (1) number of input args" << endl;
		assert(0);
	}

	return _SUCCESS_;
}

int Do_log10::checkAndSetOperands()
{
	if (sysCallInputArgs.size() != 1)
	{
		// error condition. We expect to have 1 input args to log10
		//
		cout << "Panic: System call to log10 without appropriate (1) number of input args" << endl;
		assert(0);
	}

	return _SUCCESS_;
}

int Do_malloc::checkAndSetOperands()
{
	if (sysCallInputArgs.size() != 1)
	{
		// error condition. We expect to have 1 input args to malloc
		//
		cout << "Panic: System call to malloc without appropriate (1) number of input args" << endl;
		assert(0);
	}

	return _SUCCESS_;
}

int Do_free::checkAndSetOperands()
{
	if (sysCallInputArgs.size() != 1)
	{
		// error condition. We expect to have 1 input args to free
		//
		cout << "Panic: System call to free without appropriate (1) number of input args" << endl;
		assert(0);
	}

	return _SUCCESS_;
}

int Do_perror::checkAndSetOperands()
{
	if (sysCallInputArgs.size() != 1)
	{
		// error condition. We expect to have 1 input args to strcmp
		//
		cout << "Panic: System call to perror without appropriate (1) number of input args" << endl;
		assert(0);
	}

	// _SOURCE_1_ is a char pointer.
	//
	if (!isStringType(sysCallInputArgs[0]))
	{
		sysCallArgToTypeMap[0] = CHAR_POINTER;
		return _FAILURE_;
	}

	return _SUCCESS_;
}

int Do_sin::checkAndSetOperands()
{
	if (sysCallInputArgs.size() != 1) // we expect atleast 1 argument.
	{
		// error condition.
		//
		cout << "Panic: Call to sin without appropriate (==1) number of input args" << endl;
		assert(0);
	}

	return _SUCCESS_;
}

int Do_cos::checkAndSetOperands()
{
	if (sysCallInputArgs.size() != 1) // we expect atleast 1 argument.
	{
		// error condition.
		//
		cout << "Panic: Call to cos without appropriate (==1) number of input args" << endl;
		assert(0);
	}

	return _SUCCESS_;
}

int hasSideEffects(MyString & s)
{
	if (s == "_fscanf") return _YES_;
	if (s == "_sscanf") return _YES_;
	if (s == "_read") return _YES_;

	if (s == "_strcpy") return _YES_;

	return _NO_;
}

/********************************************************************************/
/*****************  Memory management support funcs.  ***************************/
/*****************   malloc, calloc, free             ***************************/
/********************************************************************************/

/********  SAH, Tue Dec 07, 99   ************************************************/

// We assume that heapPointer is first initialized to the beginning of the heap.

// Several assumptions/restrictions:

// 1) The heap grows downwards.
//
// 2) Malloc allocates a contiguous block.
//
// 3) Free does nothing. (I.e. once a block is allocated it is lost for ever)
//
// 4) Restriction 3 is not a problem as long as we are working with virtual memory (i.e. no size limit).
//
// 5) ToDo: Implement a small memory management block that keeps track of allocated and freed memory.

long heapPointer = HEAP_START_ADDRESS;

long myMalloc(int size)
{
	long retVal = heapPointer;

	heapPointer += size;

	return retVal;
}

int myFree(long addr)
{
	// as of now, free does nothing.

	return 1;
}

/********************************************************************************/
/*****************  Print/Scan support funcs.	            *********************/
/*****************   printf, scanf, fprintf, sscanf, etc.   *********************/
/********************************************************************************/

/********  SAH, Wed Dec 29, 99   ************************************************/

const int PS_NONE = 12345;
const int PS_ARG = 12346;
const int PS_SPECIAL = 12347;

int myCheckAndSetForIO(char *fmt, int firstpos)
{
	if (fmt == NULL) return _SUCCESS_;

	char *p;
	int i = firstpos;

	for (p = fmt; *p; p++)
	{
		if (*p != '%')
		{
			continue;
		}
		switch (*++p)
		{
		case 's' :
			if (!isStringType(sysCallInputArgs[i]))
			{
				if (isAddrType(sysCallInputArgs[i]))
				{
					sysCallArgToTypeMap[i] = CHAR_POINTER;
					return _FAILURE_;
				}
			}
			break;
		default:
			break;
		}
	}

	return _SUCCESS_;
}

// This function, traverses the string while the characters range from 0 -9.
// The moment it encounters a non-decimal character, it stops.
//
int moveOverDecimals(char * & str)
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

// SAH, Wed Dec 29 99:
// Note: Currently, we do very minimal error checking.
//
int myPrint(FILE *fp, char *fmt, int firstpos)
{
	if ((fp == NULL) || (fmt == NULL))
	{
		// error condition
		return -1;
	}

	// The rest of the arguments are assumed to be in sysCallArgPtr[firstpos], [firstpos+1], ...

	char *p;
	int i = firstpos;

	int ival = -1;
	double dval = -1.0;
	char *sval = NULL;

	int retcount = 0;
	int tempcount;
	int mode = PS_NONE;

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
				precisionNum = moveOverDecimals(p);
			}

			switch (*p)
			{
			case 'd' :
				if (isIntegerType(sysCallInputArgs[i]))
					ival = getIntegerValue(sysCallInputArgs[i]);
				if ((tempcount = fprintf(fp, "%d", ival)) > 0)
					retcount += tempcount;
				i++;
				break;
			case 'f' :
			case 'g' :
				if (isFloatType(sysCallInputArgs[i]))
					dval = getFloatValue(sysCallInputArgs[i]);
				else if (isIntegerType(sysCallInputArgs[i]))
					dval = getIntegerValue(sysCallInputArgs[i]);
				if ((tempcount = fprintf(fp, "%f", dval)) > 0)
					retcount += tempcount;
				i++;
				break;
			case 's' :
				if (isStringType(sysCallInputArgs[i]))
					sval = getStringValue(sysCallInputArgs[i]);
				if ((tempcount = fprintf(fp, "%s", sval)) > 0)
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

// scans return a value and also have a side effect.
//
// They store a value into memory (from a string or a file).
//
// So, sysCallOutputArgs contains the return value
// and sysCallSideEffects contains the modified values, and the address locations.
//
int myScan(FILE *fp, char *fmt, int firstpos)
{
	if ((fp == NULL) || (fmt == NULL))
	{
		// error condition
		return -1;
	}

	// The rest of the arguments are assumed to be in sysCallArgPtr[firstpos], [firstpos+1], ...

	char *p;
	int i = firstpos;

	int ival;
	double dval;
	char sval[100];

	int retcount = 0;
	int tempcount;

	int mode = PS_NONE;

	SysSideEffect ts;

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
			getc(fp);
			continue;
		}
		int precisionNum;
		if (mode == PS_ARG)
		{
			++p;
			if (*p == '.')
			{
				++p;
				precisionNum = moveOverDecimals(p);
			}
			switch (*p)
			{
			case 'd' :
				if (((tempcount = fscanf(fp, "%d", &ival)) != EOF) && (tempcount > 0))
				{
					// now fill the side effect array.
					//
					if (isAddrType(sysCallInputArgs[i]))
					{
						AddrOperand addr(_DIRECTMEMORY_, getIntegerValue(sysCallInputArgs[i]));
						Val a(ival);
						ts.addr = addr;
						ts.val = a;
						sysCallSideEffects.push_back(ts);

						i++;
					}
					retcount += tempcount;
				}
				break;
			case 'f' :
			case 'g' :
				if (((tempcount = fscanf(fp, "%f", &dval)) != EOF) && (tempcount > 0))
				{
					// now fill the side effect array.
					//
					if (isAddrType(sysCallInputArgs[i]))
					{
						AddrOperand addr(_DIRECTMEMORY_, getIntegerValue(sysCallInputArgs[i]));
						Val a(dval);
						ts.addr = addr;
						ts.val = a;
						sysCallSideEffects.push_back(ts);

						i++;
					}
					retcount += tempcount;
				}
				break;
			case 's' :
				if (((tempcount = fscanf(fp, "%s", sval)) != EOF) && (tempcount > 0))
				{
					// now fill the side effect array.
					//
					if (isAddrType(sysCallInputArgs[i]))
					{
						AddrOperand addr(_DIRECTMEMORY_, getIntegerValue(sysCallInputArgs[i]));
						Val a(sval);
						ts.addr = addr;
						ts.val = a;
						sysCallSideEffects.push_back(ts);

						i++;
					}
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

int myScan(char *buf, char *fmt, int firstpos)
{
	if ((buf == NULL) || (fmt == NULL))
	{
		// error condition
		return -1;
	}

	// The rest of the arguments are assumed to be in sysCallArgPtr[firstpos], [firstpos+1], ...

	char *p;
	int i = firstpos;

	int ival;
	double dval;
	char sval[100];

	int retcount = 0;
	int tempcount;

	int mode = PS_NONE;

	SysSideEffect ts;

	char *tmp;

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
			buf++;
			continue;
		}
		int precisionNum;
		if (mode == PS_ARG)
		{
			++p;
			if (*p == '.')
			{
				++p;
				precisionNum = moveOverDecimals(p);
			}
			switch (*p)
			{
			case 'd' :
				if (((tempcount = sscanf(buf, "%d", &ival)) != EOF) && (tempcount > 0))
				{
					// now fill the side effect array.
					//
					if (isAddrType(sysCallInputArgs[i]))
					{
						AddrOperand addr(_DIRECTMEMORY_, getIntegerValue(sysCallInputArgs[i]));
						Val a(ival);
						ts.addr = addr;
						ts.val = a;
						sysCallSideEffects.push_back(ts);

						i++;
					}
					retcount++;

					// Finally advance the buffer to point to the next valid char.
					//
					strtol(buf, &tmp, 10);
					buf = tmp;
				}
				break;
			case 'f' :
			case 'g' :
				if (((tempcount = sscanf(buf, "%f", &dval)) != EOF) && (tempcount > 0))
				{
					// now fill the side effect array.
					//
					if (isAddrType(sysCallInputArgs[i]))
					{
						AddrOperand addr(_DIRECTMEMORY_, getIntegerValue(sysCallInputArgs[i]));
						Val a(dval);
						ts.addr = addr;
						ts.val = a;
						sysCallSideEffects.push_back(ts);

						i++;
					}
					retcount++;

					// Finally advance the buffer to point to the next valid char.
					//
					strtod(buf, &tmp);
					buf = tmp;
				}
				break;
			case 's' :
				if (((tempcount = sscanf(buf, "%s", sval)) != EOF) && (tempcount > 0))
				{
					// now fill the side effect array.
					//
					if (isAddrType(sysCallInputArgs[i]))
					{
						AddrOperand addr(_DIRECTMEMORY_, getIntegerValue(sysCallInputArgs[i]));
						Val a(sval);
						ts.addr = addr;
						ts.val = a;
						sysCallSideEffects.push_back(ts);

						i++;
					}
					retcount++;

					// Finally advance the buffer to point to the next valid char.
					//
					buf += strlen(sval);
				}
				break;
			default:
				buf++;
			}
		}
		else if (mode == PS_SPECIAL)
		{
			++p;
			switch (*p)
			{
			case 'n' :
				if ((tempcount = sscanf(buf, "\n")) > 0)
					buf++;
				break;
			default:
				buf++;
			}
		}
	}
	return retcount;
}

void initSystemCallsTable()
{
	MyString s;

	SysCallPtr p;

	s = "_strcmp";
	p = new Do_strcmp();
	addSystemCall(s, p);

	s = "_strcpy";
	p = new Do_strcpy();
	addSystemCall(s, p);

	s = "_bcopy";
	p = new Do_strcpy();
	addSystemCall(s, p);

	s = "_atoi";
	p = new Do_atoi();
	addSystemCall(s, p);

	s = "_atof";
	p = new Do_atof();
	addSystemCall(s, p);

	s = "_atol";
	p = new Do_atol();
	addSystemCall(s, p);

	s = "_ceil";
	p = new Do_ceil();
	addSystemCall(s, p);

	s = "_floor";
	p = new Do_floor();
	addSystemCall(s, p);

	s = "_log10";
	p = new Do_log10();
	addSystemCall(s, p);

	s = "_fopen";
	p = new Do_fopen();
	addSystemCall(s, p);

	s = "_fclose";
	p = new Do_fclose();
	addSystemCall(s, p);

	s = "_fseek";
	p = new Do_fseek();
	addSystemCall(s, p);

	s = "_rewind";
	p = new Do_rewind();
	addSystemCall(s, p);

	s = "_printf";
	p = new Do_printf();
	addSystemCall(s, p);

	s = "_scanf";
	p = new Do_scanf();
	addSystemCall(s, p);

	s = "_fprintf";
	p = new Do_fprintf();
	addSystemCall(s, p);

	s = "_sscanf";
	p = new Do_sscanf();
	addSystemCall(s, p);

	s = "_fscanf";
	p = new Do_fscanf();
	addSystemCall(s, p);

	s = "_read";
	p = new Do_read();
	addSystemCall(s, p);

	s = "_fread";
	p = new Do_fread();
	addSystemCall(s, p);

	s = "_fwrite";
	p = new Do_fwrite();
	addSystemCall(s, p);

	s = "_fputs";
	p = new Do_fputs();
	addSystemCall(s, p);

	s = "_puts";
	p = new Do_puts();
	addSystemCall(s, p);

	s = "_getc";
	p = new Do_getc();
	addSystemCall(s, p);

	s = "_open";
	p = new Do_open();
	addSystemCall(s, p);

	s = "_close";
	p = new Do_close();
	addSystemCall(s, p);

	s = "__flsbuf";
	p = new Do__flsbuf();
	addSystemCall(s, p);

	s = "__filbuf";
	p = new Do__filbuf();
	addSystemCall(s, p);

	s = "_exit";
	p = new Do_exit();
	addSystemCall(s, p);

	s = "_perror";
	p = new Do_perror();
	addSystemCall(s, p);

	s = "_malloc";
	p = new Do_malloc();
	addSystemCall(s, p);

	s = "_free";
	p = new Do_free();
	addSystemCall(s, p);

	s = "_sin";
	p = new Do_sin();
	addSystemCall(s, p);

	s = "_cos";
	p = new Do_cos();
	addSystemCall(s, p);
}