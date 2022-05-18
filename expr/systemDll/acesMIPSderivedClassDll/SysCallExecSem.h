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
 *file  : SysCallExecSem.h                                   *
 *created : Nov 22, 99                                       *
 *authors : Ashok Halambi                                    *
 *last modified :                                            *
 *                                                           *
 *compiled with : VC++                                       *
 *for more info :                                            *
 *************************************************************/

#ifndef _SYSCALLEXECSEM_H_
#define _SYSCALLEXECSEM_H_

#include <io.h>

#include "DerivedClassIncludes.h"

#include "ExpressIncludes.h"
#include "MyString.h"

#include "SimCall.h"

extern Vector<Val> sysCallInputArgs;
extern Vector<Val> sysCallOutputArgs;
extern Vector<SysSideEffect> sysCallSideEffects;

const long HEAP_START_ADDRESS = 1000000004;

extern long myMalloc(int size);
extern int myFree(long addr);

const int _SUCCESS_ = 1;
const int _FAILURE_ = 0;

class BaseSysCallExecSem
{
public:
  virtual void operator()()
      {
		// Error: this function should never be called.
		cout << "Error: Base class method (BaseSysCallExecSem::operator()) should not be called" << endl;
      }

  virtual int checkAndSetOperands(){ return _SUCCESS_;}

  virtual int hasSideEffects() { return _NO_; }
};

class Do_fopen : public BaseSysCallExecSem
{
public:
	void operator()()
	{
		char *filename;
		char *mode;

		FILE *fPtr = NULL;

		// first convert the vals in sysCallInputArgs to char *filename, and char *mode

		filename = getStringValue(sysCallInputArgs[0]);
		mode = getStringValue(sysCallInputArgs[1]);

		if ((filename != NULL) && (mode != NULL))
			fPtr = fopen(filename, mode);
		else
		{
			// error condition.
			//
			cout << "Warning: FOPEN recieved either a NULL filename or a NULL mode (or both). " << endl;
		}

		// finally, store fptr as a val in sysCallOutputArgs

		Val a(fPtr);

		sysCallOutputArgs.push_back(a);
	}

	int checkAndSetOperands();
};

class Do_fclose : public BaseSysCallExecSem
{
public:
	void operator()()
	{
		FILE *fPtr;

		// first convert the val in sysCallInputArgs to FILE *filename

		fPtr = (sysCallInputArgs[0]).asFPTR();

		if (fPtr != NULL)
			fclose(fPtr);

	}

	int checkAndSetOperands();
};

class Do_fseek : public BaseSysCallExecSem
{
public:
	void operator()()
	{
		FILE *fPtr;

		// first convert the val in sysCallInputArgs to FILE *filename

		fPtr = (sysCallInputArgs[0]).asFPTR();

		long offset = getIntegerValue(sysCallInputArgs[1]);
		int origin = getIntegerValue(sysCallInputArgs[2]);

		int retval;

		if (fPtr != NULL)
			retval = fseek(fPtr, offset, origin);

		// finally, store retval as a val in sysCallOutputArgs

		Val a(retval);

		sysCallOutputArgs.push_back(a);
	}

	int checkAndSetOperands();
};

class Do_rewind : public BaseSysCallExecSem
{
public:
	void operator()()
	{
		FILE *fPtr;

		// first convert the val in sysCallInputArgs to FILE *filename

		fPtr = (sysCallInputArgs[0]).asFPTR();

		if (fPtr != NULL)
			rewind(fPtr);

	}

	int checkAndSetOperands();
};

extern int myPrint(FILE *fp, char *fmt, int firstpos);
extern int myScan(FILE *fp, char *fmt, int firstpos);
extern int myScan(char *buf, char *fmt, int firstpos);

class Do_printf : public BaseSysCallExecSem
{
public:
	void operator()()
	{
		// We expect the first argument to be the format string.
		//
		char *fmt = getStringValue(sysCallInputArgs[0]);

		// call myprint
		//
		if (myPrint(stdout, fmt, 1) == -1)
		{
			// error condition.
			//
			cout << "Panic: Error while printing (printf)" << endl;
			assert(0);
		}
	}

	int checkAndSetOperands();
};

class Do_scanf : public BaseSysCallExecSem
{
public:
	void operator()()
	{
		// We expect the first argument to be the format string.
		//
		char *fmt = getStringValue(sysCallInputArgs[0]);

		// call myprint
		//
		if (myScan(stdin, fmt, 1) == -1)
		{
			// error condition.
			//
			cout << "Panic: Error while scanning (scanf)" << endl;
			assert(0);
		}
	}

	int checkAndSetOperands();
};

class Do_fprintf : public BaseSysCallExecSem
{
public:
	void operator()()
	{
		// We expect the first argument to be the FILE *ptr or stdout our stderr.
		// stdout and stderr have been hardcoded to -9980 and -9960 respectively
		//
		FILE *fp = NULL;
		if ((sysCallInputArgs[0])._valType == _FILE_PTR)
		{
			fp = (sysCallInputArgs[0]).asFPTR();
		}
		else if (isIntegerType(sysCallInputArgs[0]))
		{
			int check = getIntegerValue(sysCallInputArgs[0]);

			if (check == STDOUT_HACK)
				fp = stdout;
			else if (check == STDERR_HACK)
				fp = stderr;
			else
			{
				// error condition.
				//
				cout << "Panic: In fprintf, expected FILE * argument, got integer." << endl;
				assert(0);
			}
		}

		char *fmt = getStringValue(sysCallInputArgs[1]);

		// call myprint
		//
		if (myPrint(fp, fmt, 2) == -1)
		{
			// error condition.
			//
			cout << "Panic: Error while printing (fprintf)" << endl;
			assert(0);
		}
	}

	int checkAndSetOperands();
};


// sscanf and fscanf return a value and also have a side effect.
//
// They store a value into memory (from a string and a file, respectively).
//
// So, sysCallOutputArgs contains the return value
// while sysCallSideEffect contains the modified values, and the address locations.
//
class Do_sscanf : public BaseSysCallExecSem
{
public:
	void operator()()
	{
		// We expect the first argument to be the string buffer.
		//
		char *buf = getStringValue(sysCallInputArgs[0]);

		// We expect the second argument to be the format string.
		//
		char *fmt = getStringValue(sysCallInputArgs[1]);

		// call myscan
		//
		if (myScan(buf, fmt, 2) == -1)
		{
			// error condition.
			//
			cout << "Panic: Error while scanning (sscanf)" << endl;
			assert(0);
		}
	}

	int checkAndSetOperands();
};

class Do_fscanf : public BaseSysCallExecSem
{
public:
	void operator()()
	{
		// We expect the first argument to be the file pointer.
		//
		FILE *fp = NULL;
		if ((sysCallInputArgs[0])._valType == _FILE_PTR)
		{
			fp = (sysCallInputArgs[0]).asFPTR();
		}

		// We expect the second argument to be the format string.
		//
		char *fmt = getStringValue(sysCallInputArgs[1]);

		// call myscan
		//
		if (myScan(fp, fmt, 2) == -1)
		{
			// error condition.
			//
			cout << "Panic: Error while scanning (fscanf)" << endl;
			assert(0);
		}
	}

	int checkAndSetOperands();
};

// Read returns success (or failure).
// However, it also has the side effect of modifying memory locations.
// This is stored in sysCallSideEffects.
//
class Do_read : public BaseSysCallExecSem
{
public:
	void operator()()
	{
		// We expect the first argument to be the file handle.
		//
		int fp = getIntegerValue(sysCallInputArgs[0]);

		// The second argument contains the location where the string (that is read from the file)
		// needs to be stored.

		// The 3rd argument specifies the maximum length of the buffer (in bytes).
		int size = getIntegerValue(sysCallInputArgs[2]);
		
		char *buf;
		buf = new char[size];

		// call read
		//
		int retval = _read(fp, buf, size);

		Val a(retval);
		sysCallOutputArgs.push_back(a);

		// now fill the side effect array.
		//
		if (isAddrType(sysCallInputArgs[1]))
		{
			SysSideEffect ts;
	
			AddrOperand addr(_DIRECTMEMORY_, getIntegerValue(sysCallInputArgs[1]));
			Val a(buf);
			ts.addr = addr;
			ts.val = a;
			sysCallSideEffects.push_back(ts);
		}
	}

	int checkAndSetOperands();
};

class Do_fread : public BaseSysCallExecSem
{
public:
	void operator()()
	{
		// The first argument contains the location where the string (that is read from the file)
		// needs to be stored.

		// We expect the second argument to be the element size.
		//
		size_t size = getIntegerValue(sysCallInputArgs[1]);

		// The 3rd argument specifies the number of elements to be read.
		int count = getIntegerValue(sysCallInputArgs[2]);

		// The 4th argument specifies the file handler.
		//
		FILE *fp =  (sysCallInputArgs[3]).asFPTR();

		char *buf;
		buf = new char[size*count];

		// call fread
		//
		size_t retval = fread(buf, size, count, fp);

		Val a(retval);
		sysCallOutputArgs.push_back(a);

		// now fill the side effect array.
		//
		if (isAddrType(sysCallInputArgs[1]))
		{
			SysSideEffect ts;
	
			AddrOperand addr(_DIRECTMEMORY_, getIntegerValue(sysCallInputArgs[0]));
			Val a(buf);
			ts.addr = addr;
			ts.val = a;
			sysCallSideEffects.push_back(ts);
		}
	}

	int checkAndSetOperands();
};

class Do_fwrite : public BaseSysCallExecSem
{
public:
	void operator()()
	{
		// The first argument contains the string
		//
		void *buf = (void *)(getStringValue(sysCallInputArgs[0]));

		// We expect the second argument to be the element size.
		//
		size_t size = getIntegerValue(sysCallInputArgs[1]);

		// The 3rd argument specifies the number of elements to be read.
		int count = getIntegerValue(sysCallInputArgs[2]);

		// The 4th argument specifies the file handler.
		//
		FILE *fp =  (sysCallInputArgs[3]).asFPTR();

		// call fwrite
		//
		size_t retval = fwrite(buf, size, count, fp);

		Val a(retval);
		sysCallOutputArgs.push_back(a);

	}

	int checkAndSetOperands();
};

// fputs writes a string into a file stream.
//
class Do_fputs : public BaseSysCallExecSem
{
public:
	void operator()()
	{

		// We expect the first argument to be the string.
		//
		char *fmt = getStringValue(sysCallInputArgs[0]);

		// We expect the second argument to be the file pointer.
		//
		FILE *fp = NULL;
		if ((sysCallInputArgs[1])._valType == _FILE_PTR)
		{
			fp = (sysCallInputArgs[0]).asFPTR();
		}

		// call fputs
		//
		int retval = fputs(fmt, fp);
	
		// store the result into sysCallOutputArgs
		//
		Val a(retval);
		sysCallOutputArgs.push_back(a);
	}

	int checkAndSetOperands();
};

// puts writes a string (and a newline) into stdout.
//
class Do_puts : public BaseSysCallExecSem
{
public:
	void operator()()
	{

		// We expect the first argument to be the string.
		//
		char *fmt = getStringValue(sysCallInputArgs[0]);

		// call puts
		//
		int retval = puts(fmt);
	
		// store the result into sysCallOutputArgs
		//
		Val a(retval);
		sysCallOutputArgs.push_back(a);
	}

	int checkAndSetOperands();
};

class Do_getc : public BaseSysCallExecSem
{
public:
	void operator()()
	{

		// getc takes one argument of type FILE *
		//
		FILE *fp = (sysCallInputArgs[0]).asFPTR();

		int ret = getc(fp);
//		printf("%c", ret);

		Val a(ret);
		sysCallOutputArgs.push_back(a);
	}

	int checkAndSetOperands();
};

class Do_open : public BaseSysCallExecSem
{
public:
	void operator()()
	{
		// We expect the first argument to be the file name.
		//
		char *fmt = getStringValue(sysCallInputArgs[0]);

		// We expect the second argument to be the flag specifier.
		//
		int arg2 = getIntegerValue(sysCallInputArgs[1]);
	
		// Check to see if the 3rd argument too is specified.
		//
		int arg3 = 0;
		if (sysCallInputArgs.size() == 3)
		{
			arg3 = getIntegerValue(sysCallInputArgs[2]);
		}
		// else the default value of arg3 is 0

		// call open
		//
		int retval = _open(fmt, arg2, arg3);
	
		// store the result into sysCallOutputArgs
		//
		Val a(retval);
		sysCallOutputArgs.push_back(a);
	}

	int checkAndSetOperands();
};

class Do_close : public BaseSysCallExecSem
{
public:
	void operator()()
	{
		// We expect the first argument to be the file handle.
		//
		int arg1 = getIntegerValue(sysCallInputArgs[0]);
	
		// call close
		//
		int retval = _close(arg1);
	
		// store the result into sysCallOutputArgs
		//
		Val a(retval);
		sysCallOutputArgs.push_back(a);
	}

	int checkAndSetOperands();
};

class Do__flsbuf : public BaseSysCallExecSem
{
public:
	void operator()()
	{
		// _flsbuf is declared in STDIO.H and defined in _FLSBUF.C in DevStudion\VC\crt\src
		//
		// _flsbuf takes one argument of type int and the other of type FILE * 
		//
		int ch = getIntegerValue(sysCallInputArgs[0]);

		FILE *fp = (sysCallInputArgs[1]).asFPTR();

		int ret = _flsbuf(ch, fp);

		Val a(ret);
		sysCallOutputArgs.push_back(a);
	}

	int checkAndSetOperands();
};

class Do__filbuf : public BaseSysCallExecSem
{
public:
	void operator()()
	{
		// _filbuf is declared in STDIO.H and defined in _FILBUF.C in DevStudion\VC\crt\src
		//
		// _filbuf takes one argument of type FILE *
		//
		FILE *fp = (sysCallInputArgs[0]).asFPTR();

		int ret = _filbuf(fp);

		Val a(ret);
		sysCallOutputArgs.push_back(a);
	}

	int checkAndSetOperands();
};

class Do_strcmp : public BaseSysCallExecSem
{
public:
	void operator()()
	{
		char *string1, *string2;

		int retVal;

		// first convert the vals in sysCallInputArgs to string1 and string2

		if (sysCallInputArgs.size() != 2)
		{
			// error condition. We expect to have 2 input args to strcmp
			//
			cout << "Panic: System call to strcmp without appropriate (2) number of input args" << endl;
			assert(0);
		}

		string1 = getStringValue(sysCallInputArgs[0]);
		string2 = getStringValue(sysCallInputArgs[1]);

		retVal = strcmp(string1, string2);

		// finally, store retVal as a val in sysCallOutputArgs

		Val a(retVal);

		sysCallOutputArgs.push_back(a);
	}

	int checkAndSetOperands();
};

class Do_strcpy : public BaseSysCallExecSem
{
public:
	void operator()()
	{
		char *string2, *string1;

		char *retVal;

		string2 = getStringValue(sysCallInputArgs[1]);

		//string1 = (char *)malloc(strlen(string2) + 1);
		string1 = new char[strlen(string2) + 1];

		retVal = strcpy(string1, string2);

		// finally, store retVal as a val in sysCallOutputArgs

		Val a(retVal);

		sysCallOutputArgs.push_back(a);

		// now fill the side effect array.
		//
		if (isAddrType(sysCallInputArgs[0]))
		{
			SysSideEffect ts;
	
			AddrOperand addr(_DIRECTMEMORY_, getIntegerValue(sysCallInputArgs[0]));
			Val a(retVal);
			ts.addr = addr;
			ts.val = a;
			sysCallSideEffects.push_back(ts);
		}
	}

	int checkAndSetOperands();
};

// Class bcopy is a funny one.
//
// WINDOWS does not have the exact library call.
// However, it has memcpy which is very similar to bcopy.
// So, we call memcpy.
// However, there are some differences:
// bcopy's input parameters are: char *, char *, int
// memcpy's input parameters are: void *, const void *, size_t (which is actually unsigned int)
//
// The biggest difference tho is that bcopy ensures correctness in case of overlapping strings,
// while memcpy does not. In case this does constitute a problem use memmove instead.
//
class Do_bcopy : public BaseSysCallExecSem
{
public:
	void operator()()
	{
		char *string2, *string1;

		char *retVal;

		// first convert the vals in sysCallInputArgs to string1 and string2

		string2 = getStringValue(sysCallInputArgs[1]);

		size_t count = getIntegerValue(sysCallInputArgs[2]);

		string1 = (char *)malloc(count + 1);

		retVal = (char *)memcpy((void *)string1, (void *)string2, count);

		// finally, store retVal as a val in sysCallOutputArgs

		Val a(retVal);

		sysCallOutputArgs.push_back(a);

		// now fill the side effect array.
		//
		if (isAddrType(sysCallInputArgs[0]))
		{
			SysSideEffect ts;
	
			AddrOperand addr(_DIRECTMEMORY_, getIntegerValue(sysCallInputArgs[0]));
			Val a(retVal);
			ts.addr = addr;
			ts.val = a;
			sysCallSideEffects.push_back(ts);
		}
	}

	int checkAndSetOperands();
};

class Do_atoi : public BaseSysCallExecSem
{
public:
	void operator()()
	{
		int retVal = 0;

		char *buf = getStringValue(sysCallInputArgs[0]);

		retVal = atoi(buf);

		Val a(retVal);

		sysCallOutputArgs.push_back(a);
	}

	int checkAndSetOperands();
};

class Do_atof : public BaseSysCallExecSem
{
public:
	void operator()()
	{
		double retVal = 0;

		char *buf = getStringValue(sysCallInputArgs[0]);

		retVal = atof(buf);

		Val a(retVal);

		sysCallOutputArgs.push_back(a);
	}

	int checkAndSetOperands();
};

class Do_atol : public BaseSysCallExecSem
{
public:
	void operator()()
	{
		long retVal = 0;

		char *buf = getStringValue(sysCallInputArgs[0]);

		retVal = atol(buf);

		Val a(retVal);

		sysCallOutputArgs.push_back(a);
	}

	int checkAndSetOperands();
};

class Do_ceil : public BaseSysCallExecSem
{
public:
	void operator()()
	{
		double retVal = 0;

		double x = getFloatValue(sysCallInputArgs[0]);

		retVal = ceil(x);

		Val a(retVal);

		sysCallOutputArgs.push_back(a);
	}

	int checkAndSetOperands();
};

class Do_floor : public BaseSysCallExecSem
{
public:
	void operator()()
	{
		double retVal = 0;

		double x = getFloatValue(sysCallInputArgs[0]);

		retVal = floor(x);

		Val a(retVal);

		sysCallOutputArgs.push_back(a);
	}

	int checkAndSetOperands();
};

class Do_log10 : public BaseSysCallExecSem
{
public:
	void operator()()
	{
		double retVal = 0;

		double x = getFloatValue(sysCallInputArgs[0]);

		retVal = log10(x);

		Val a(retVal);

		sysCallOutputArgs.push_back(a);
	}

	int checkAndSetOperands();
};

class Do_malloc : public BaseSysCallExecSem
{
public:
	void operator()()
	{
		int size = 0;
		long retVal = 0;

		size = getIntegerValue(sysCallInputArgs[0]);

		retVal = myMalloc(size);

		Val a(retVal);

		sysCallOutputArgs.push_back(a);
	}

	int checkAndSetOperands();
};

class Do_free : public BaseSysCallExecSem
{
public:
	void operator()()
	{
		long addr = 0;

		int retVal = 0;

		addr = getIntegerValue(sysCallInputArgs[0]);

		retVal = myFree(addr);

		Val a(retVal);

		sysCallOutputArgs.push_back(a);
	}

	int checkAndSetOperands();
};

class Do_exit : public BaseSysCallExecSem
{
public:
	void operator()()
	{
		int status = 0;

		// first convert the val in sysCallInputArgs to int status

//		exit(status);
	}

//	int checkAndSetOperands();
};

class Do_perror : public BaseSysCallExecSem
{
public:
	void operator()()
	{
		char *string1 = getStringValue(sysCallInputArgs[0]);

		// call perror
		//
		perror(string1);
	}

	int checkAndSetOperands();
};

#include <math.h>

class Do_sin : public BaseSysCallExecSem
{
public:
	void operator()()
	{
		double x = getFloatValue(sysCallInputArgs[0]);

		// call sin
		//
		x = sin(x);

		Val a(x);

		sysCallOutputArgs.push_back(a);
	}

	int checkAndSetOperands();
};

class Do_cos : public BaseSysCallExecSem
{
public:
	void operator()()
	{
		double x = getFloatValue(sysCallInputArgs[0]);

		// call sin
		//
		x = cos(x);

		Val a(x);

		sysCallOutputArgs.push_back(a);
	}

	int checkAndSetOperands();
};

typedef BaseSysCallExecSem *SysCallPtr;

typedef map<MyString, SysCallPtr> SysCallPtrObjMap;

#endif
