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
//////////////////////////////////////////////////////////////////////
// CONSOLE main.cpp                                                ///
//////////////////////////////////////////////////////////////////////


//#define _MT

#include <iostream.h>
#include "../pcProGUI/ProgramParams.h"

#ifdef __UNIX__
	#define __declspec(dllimport)
	#define __stdcall
#else
	#include <windows.h>
	#include <process.h>


	void UpdateConsoleCOUT(char* str, int streamcount);
	void UpdateConsoleCERR(char* str, int streamcount);

typedef struct
{
 HANDLE handle;
 FILETIME creationTime;
 FILETIME exitTime;
 FILETIME kernelTime;
 FILETIME userTime;
}
 THREADINFO;

__declspec( dllimport ) THREADINFO threadInfo;

//unsigned int __stdcall pcProConsoleMain(void* _cp);

void myBuild(char *fileName);
void myInit1();
void myInit2();
extern int strcasecmp(char *s1, char *s2);
#endif  // end of else of #ifdef __UNIX__

unsigned int __stdcall pcProMain(void* _cp);

int main(int argc, char **argv)
{
#ifdef __UNIX__
 ProgramParams pp;

 pp.argc=argc;
 pp.argv=argv;

 //pp.cout=new my_ostream(&UpdateConsoleCOUT);
 //pp.cerr=new my_ostream(&UpdateConsoleCERR);

	pcProMain(&pp);
#else
 //int res;
 ProgramParams pp;

 HANDLE hO=GetStdHandle(STD_OUTPUT_HANDLE);
 SetConsoleTextAttribute(hO,FOREGROUND_GREEN|FOREGROUND_INTENSITY);  

 pp.argc=argc;
 pp.argv=argv;
// pp.cout=new my_ostream(&UpdateConsoleCOUT);
 //pp.cerr=new my_ostream(&UpdateConsoleCERR);

 cout << "Starting EXPRESS..." << endl;
 
/* //create a "gentle" thread
 unsigned thrdaddr;
 //threadInfo.handle=(HANDLE)_beginthreadex(NULL, 0, pcProConsoleMain, &pp, CREATE_SUSPENDED, &thrdaddr);
 threadInfo.handle=(HANDLE)_beginthreadex(NULL, 0, pcProMain, &pp, CREATE_SUSPENDED, &thrdaddr);
 SetThreadPriority(threadInfo.handle,THREAD_PRIORITY_BELOW_NORMAL); //THREAD_PRIORITY_IDLE);
 //SetThreadPriority(threadInfo.handle,THREAD_PRIORITY_ABOVE_NORMAL); //THREAD_PRIORITY_IDLE);
 ResumeThread(threadInfo.handle); 
 WaitForSingleObject(threadInfo.handle,INFINITE); */

 pcProMain(&pp); /* this was added and the gentle thread is commented..by MnM */
  
 cout << endl << endl << "Leaving EXPRESS..." << endl;

#endif	// #ifdef __UNIX__
 return 0;


}
