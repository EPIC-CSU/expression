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
//$Id$
//
// File:  		SimSupportFuncs.cpp
// Created:		Thu Dec 23, 99
// Last modified: 	Thu Dec 23, 99
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module:

#include "ArchGlobalIncludes.h"
#include "ArchSTLIncludes.h"

#include "MyString.h"
#include "FilePointer.h"

int argC;
Vector<MyString> argV;

extern void initStorageWithCmdLineArgs();

void readCommandLineArgs(MyString & fName)
{
	// Note: ArgC is set ot 1 becos we currently assume that the 
	// first argV parameter (i.e. the name of the executable)
	// is not read in the command line args.
	//
	// If this condition were to change, argC should be set to 0
	//
	argC = 1;
	argV.clear();

	char line[100];
	if (!fName.isEmpty()) 
	{
		FilePointer fp;
		
		try
		{
			fp.open(fName.getStr(), "r");
		}
		catch (CouldNotOpenFileException e)
		{
			cout << e;
			assert(0);
		}
		if (fgets(line, 100, fp) == NULL)
		{
			// error condition.
			//
			cout << "Warning: CommandLineArgument File was empty" << endl;
			fp.close();
			return;
		}

		MyString buf;
		buf = strtok(line, " \n");

		while (!buf.isEmpty())
		{
			argC++;
			argV.push_back(buf);

			buf = strtok(NULL, " \n");
		}
	}

//	initStorageWithCmdLineArgs();
}
