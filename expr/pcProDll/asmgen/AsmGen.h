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

#include "MyString.h"

#ifndef _ASM_GEN_H_
#define _ASM_GEN_H_

#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include <vector>

#ifdef vector
#undef vector
#endif 
#define vector std::vector

#include <map>

#ifdef map
#undef map
#endif 
#define map std::map

/*
class targetInstruction;

typedef vector<targetInstruction> targetInstructionList;

class targetInstruction{
	MyString Label;
	MyString printable;
	MyString format;
public:
	
	void print();
	void setLabel(MyString);
	void setFormat(MyString formatStr);
};

*/

typedef map <MyString, MyString> stringToStringMap;
typedef vector <MyString> stringVector;

class formatStructure{
	stringToStringMap srcRegMapping;  // Mappings to immediate or register
	stringToStringMap dstRegMapping;
	MyString formatString;     // Printing format
	stringVector order;           // Operand ordering
public:

	formatStructure(){
	}  

	~formatStructure(){
	}
	formatStructure(const formatStructure &fs):srcRegMapping(fs.srcRegMapping),
		dstRegMapping(fs.dstRegMapping), formatString(fs.formatString), order(fs.order)
	{}   // Copy constructor

	void setRegMapping(MyString );
	void setFormatString(MyString);
	void setOrder(MyString);
	stringToStringMap getSrcRegMapping();
	stringToStringMap getDstRegMapping();
	MyString getFormatString();
	stringVector getOperandOrder();
};





#endif


