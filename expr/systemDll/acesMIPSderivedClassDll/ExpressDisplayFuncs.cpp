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
 *file  : ExpressDisplayFuncs.cpp                            *
 *created : Jan 08, 99                                       *
 *authors : Ashok Halambi                                    *
 *last modified :                                            *
 *                                                           *
 *compiled with : VC++                                       *
 *for more info :                                            *
 *************************************************************/

#include "ExpressIncludes.h"
#include "ExecSemantics.h"

// extern Storage *toyRegFile;

void printAReg(int regNum, char *regText, int ssaNum)
{
	int uniqID = globalSymbolTable->getID(regNum, regText, ssaNum);

	AddrOperand addr(_REG_, uniqID);

	// SAH 05/21/2000 
	//
//	toyRegFile->print(cout, addr); 
}

void printRegName(int addr)
{
	cout << globalSymbolTable->getRegName(addr);
}

// SAH 05/21/2000
//
// The next two functions are not being used in Elf.
// Before using them, make sure that printAReg is correct.
//
void printARegByNum(int regNum, int rf, int ssaNum)
{
	char regText[10];
	if( !doneRA )
	{
		switch(rf){
		case 0:
			strcpy(regText,"gR");
			break;
		case 1:
			strcpy(regText,"gf");
			break;
		case 2:
			strcpy(regText,"gsp");
			break;
		}
	}
	else
	{
		assert("Target Registers are not yet specified !\n");
		switch(rf){
		case 0:
			strcpy(regText,"R");
			break;
		case 1:
			strcpy(regText,"f");
			break;
		case 2:
			strcpy(regText,"sp");
			break;
		}
	}
	printAReg(regNum,regText,ssaNum);
}

void getRegFromUserAndPrint()
{
	int regNum, ssaNum;
	char regText[10];

	cout << "RegNum: ";
	cin >> regNum;

	cout << "RegText: ";
	cin >> regText;

	cout << "SSANum: ";
	cin >> ssaNum;

	printAReg(regNum, regText, ssaNum);
}
