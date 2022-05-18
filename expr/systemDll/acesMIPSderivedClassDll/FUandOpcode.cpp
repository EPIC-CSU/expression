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
 *file  : FUandOpcode.cpp                                  *
 *created : 11 Jan,99                                        *
 *authors : Ashok Halambi                                    *
 *last modified :                                            *
 *                                                           *
 *compiled with : VC++                                       *
 *for more info :                                            *
 *************************************************************/

#include "ArchSTLIncludes.h"
#include "DerivedClassIncludes.h"

#include "../../pcProDll/include/FilePointer.h"

#ifdef __UNIX__
#include "../../expressionDll/ArchModel/FUandOpcodeDS.h"
#else
#include "../../expression dll/ArchModel/FUandOpcodeDS.h"
#endif

FUandOpcodeDS globalFUandOpcodeDS;

void initFUandOpcodeDS(char *fileName)
{
   FilePointer fp(fileName, "r");
   char buffer[80], fuName[80], opName[80];
   char *bufPtr;
   int opLatency;

   int inFU = 0;
   int fuIndex;

   try
   {
	   while (fgets(buffer, 80, fp) != NULL)
	   {
		   if (buffer[0] == '(')
		   {
			   inFU = 1;
			   bufPtr = buffer;
			   bufPtr++;
			   sscanf(bufPtr, "%s\n", fuName);
			   fuIndex = globalFUandOpcodeDS.addFU(fuName);
		   }
		   else if (buffer[0] == ')')
		   {
			   inFU = 0;
		   }
		   else
		   {
			   if (inFU == 0)
			   {
				   // error condition.
				   //
				   cout << " The file: " << fileName << " is not in the proper format" << endl;
				   assert(0);
			   }
			   sscanf(buffer, "\t%s\t%d\n", opName, &opLatency);
			   globalFUandOpcodeDS.addOpcodeAndLatency(fuIndex, opName, opLatency);
		   }
	   }
   }
   catch (CouldNotOpenFileException e)
   {
	   cout << "Panic: Could Not Open File" << fileName << endl;
	   assert(0);
   }
}

void printFUandOpcodeDS(ostream & out)
{
	globalFUandOpcodeDS.print(out);
}
