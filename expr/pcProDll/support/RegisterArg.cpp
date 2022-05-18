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
#include "stdafx.h"

#include <iostream.h>
#include "RegisterArg.h"
#include "RAProperty.h"
#include "PredProperty.h"
#include "RegisterFile.h"
#include "RegisterFileList.h"


//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

extern void printIndent(FilePointer & fp, int indent);
extern void printRegNameAfterRA(RegisterArg *reg, ostream& out);
extern void printRegNameAfterRA(RegisterArg *reg);
extern void printRegNameAfterRAToString(RegisterArg *reg, char *str);

extern int doneSSA;
extern int doneRA;
extern RegisterFileList *globalRegFileList;  // global register file list

extern bool DumpingIR;
extern bool PrintingASM;

// for printing to a file : ASM gen
void RegisterArg::print(int type, ostream& out)
{
	if (type == 0)
	{
		//BaseArgument::print();
		
		PredProperty *pProp = (PredProperty *) getPtrToProperty(_PRED_);
		if (pProp) 
		{
			//if ((*pProp)->getIsNotInverted())
			cout << "~";
		}
		
		RAProperty *raProp = (RAProperty *) getPtrToProperty(_RA_);
		if (raProp) {
			printRegNameAfterRA(this, out);
			return;
		}
		
		out << _regText << _symVal;
		SSAArgProperty *ssaP = (SSAArgProperty *)getPtrToProperty(_SSA_);
		
		if (ssaP!=NULL)
			out<<"."<<ssaP->getSSANum();
		//		out << " ( " << getUniqID() << " ) ";
	}
	else
	{
		out << " " << getUniqID() << " ";
	}
}

// changed after the register allocation strategy is changed
void RegisterArg::printAsm(ostream& out)
{
	if( PrintingASM )
	{
		if( doneRA )
		{
			RAProperty *raProp = (RAProperty *) this->getPtrToProperty(_RA_);
			int regID = raProp->getRegID();
			RegisterFile& regFile = globalRegFileList->getRegFile(regID);
			
			
			// print the register prefix
			out << regFile.name();
			
			//RAProperty *raProp = (RAProperty *) getPtrToProperty(_RA_);
			if (raProp)
			{
				out << (regID - regFile.getID());
			} 
			else // register allocation not done yet
			{
				out << getUniqID() ;
			}
		}
	}

	else if( DumpingIR )
	{
		out << getUniqID();
	}
}


//used by print(indent)
void RegisterArg::print(int type)
{
	if (type == 0)
	{
		//BaseArgument::print();
		
		PredProperty *pProp = (PredProperty *) getPtrToProperty(_PRED_);
		if (pProp) 
		{
			//if ((*pProp)->getIsNotInverted())
			cout << "~";
		}
		
		RAProperty *raProp = (RAProperty *) getPtrToProperty(_RA_);
		if (raProp) 
		{
			printRegNameAfterRA(this);
			return;
		}
		
		cout << _regText << _symVal;
		SSAArgProperty *ssaP = (SSAArgProperty *)getPtrToProperty(_SSA_);
		
		if (ssaP!=NULL)
			cout<<"."<<ssaP->getSSANum();
		//		cout << " ( " << getUniqID() << " ) ";
	}
	else
	{
		cout << " " << getUniqID() << " ";
	}
}

//used by printC(indent)
void RegisterArg::printC(int type)
{
	if (type == 0)
    {
		cout << _regText << _symVal;
		SSAArgProperty *ssaP = (SSAArgProperty *)getPtrToProperty(_SSA_);
      if (ssaP!=NULL)
	cout<<"_"<<ssaP->getSSANum();
    }
  else
    {
      cout << " " << getUniqID() << " ";
    }
} // end of fcn printC


void RegisterArg::irToC(FilePointer & fp, int indent)
{
	printIndent(fp, indent);

	MyString s;
	printCToString(s);
	fprintf(fp, "%s", s.getStr());
}

extern int convertReg(int & a, BaseArgument *argPtr);

void RegisterArg::printCToString(MyString & s, char *tval)
{

	int uniqID;
	convertReg(uniqID, (BaseArgument *)this);

	char str[20];

	if (tval != NULL)
		sprintf(str, "%s, %d", tval, uniqID);
	else
		sprintf(str, "/* Error */");

	s.concat(str);
}

// used to create the symbol tables of all operands - returns char *
char *RegisterArg::createSymbol()
{
  char str[20];
  char *symbolName;
  symbolName = strdup(_regText);
  strcat(symbolName, strdup(itoa(_symVal, str, 10)));
  
  SSAArgProperty *ssaP = (SSAArgProperty *)getPtrToProperty(_SSA_);
  if (ssaP!=NULL)
    {
      strcat(symbolName, strdup("_"));
      strcat(symbolName, strdup(itoa(ssaP->getSSANum(), str, 10)));
    }
  return symbolName;
} // end of fcn char *RegisterArg::createSymbol()

void RegisterArg::printToString(char *buf)
{
 char str[20];

	if (!doneRA)
	{
		strcat(buf,_regText);
		strcat(buf,itoa(_symVal,str,10));
		
		SSAArgProperty *ssaP = (SSAArgProperty *)getPtrToProperty(_SSA_);
		if (ssaP!=NULL){
			strcat(buf,".");
			strcat(buf,itoa(ssaP->getSSANum(),str,10));
		}
	}
	else
	{
		printRegNameAfterRAToString(this, buf);
	}
}

void RegisterArg::printProp(ostream& out)
{
	BaseArgument::print(out);
	out<< _regText << _symVal;
	SSAArgProperty *ssaP = (SSAArgProperty *)getPtrToProperty(_SSA_);
	if (ssaP!=NULL)
		out<<"."<<ssaP->getSSANum();
}


//used by buildSSA (do not modify cause it's gonna break!!!!!!!!!!!!!!!!!!!!!!!!!!!!). 
void RegisterArg::print(ostream& out)
{
	//out << "Printing properties" << endl;
	//BaseArgument::print(out);
	//out << "Done printing properties" << endl;
	//out << flush;
	
	PredProperty *pProp = (PredProperty *) getPtrToProperty(_PRED_);
	if (pProp) 
	{
		//if ((*pProp)->getIsNotInverted())
		cout << "~";
	}
	
	RAProperty *raProp = (RAProperty *) getPtrToProperty(_RA_);
	if (raProp) {
		printRegNameAfterRA(this, out);
		return;
	}
	
	out<< _regText << _symVal;
	SSAArgProperty *ssaP = (SSAArgProperty *)getPtrToProperty(_SSA_);
	if (ssaP!=NULL)
		out<<"."<<ssaP->getSSANum();
	//	out << " ( " << getUniqID() << " ) ";
	
}

void RegisterArg::printC(ostream& out)
{
	out<< _regText << _symVal;
	SSAArgProperty *ssaP = (SSAArgProperty *)getPtrToProperty(_SSA_);
	if (ssaP!=NULL)
	  out<<"_"<<ssaP->getSSANum();
}

void RegisterArg::printProperties(ostream & out, PropNameList & pL)
{
	out << _regText << _symVal;
	SSAArgProperty *ssaP = (SSAArgProperty *)getPtrToProperty(_SSA_);
	if (ssaP!=NULL)
		out<<"."<<ssaP->getSSANum();
//		cout << " ( " << getUniqID() << " ) ";
	BaseArgument::printProperties(out, pL);
}

ostream & operator << (ostream & out, const RegisterArg & s)
{
	// once again, rewrite this after figuring out what are the
	// data members.
	
	out << s._regText << s._symVal;
	SSAArgProperty *ssaP = (SSAArgProperty *)(((RegisterArg &) s).getPtrToProperty(_SSA_));
	if (ssaP!=NULL)
		out<<"."<<ssaP->getSSANum();
	return out;
}
