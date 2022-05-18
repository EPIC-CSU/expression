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
#ifndef _TREEPARSE_H_
#define _TREEPARSE_H_

#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include "globalincludes.h"
#ifndef WIN32
#include <stl.h>
#include <multimap.h>
#include <algo.h>
#include <strstream.h>
#else //WIN32
#include <map>
#include <set>
#include <deque>
#include <vector>
#include <algorithm>
#include <strstrea.h>
#include <string.h>
#include <ctype.h>

// Note: find could not be #define'd to std:: find because find is also used as a member function
// (of set).
// Later on, #define Find to std::find and replace all the occurences of find (and not .find) with
// Find.

// #define find std::find

#endif // end of #ifdef WIN32

#define _REG_ 0
#define _IMM_ 1
#define _LAB_ 2


typedef std::vector<int> ArrayOfIntType;

const int _ADD_OPERATOR_ = 456;
const int _SUB_OPERATOR_ = 567;
const int _EQ_OPERATOR_ = 678;
const int _LT_OPERATOR_ = 789;

const int _ID_SINGLE_ = 987;
const int _ID_EXPRESSION_ = 876;

const int _DEST_LIST_ = 2222;

//int ISelcurrentOp, ISeltempOp;

typedef struct{
      int regOrImm;   // equals _REG_ or _IMM_ or _LAB_
	  int num;   // src num or dst num
	  int oprtr; // EQ or LT
	  int val;
} opndType;

class Oprnd{
	int _regOrImm;   // can be reg, imm or lab
	int _oprtr;
	int _val;  // max value of the immediate
public:
	Oprnd(int regOrImm, int oprtr, int val)
	{
		_regOrImm = regOrImm;  
		_oprtr = oprtr;
		_val = val;
	}

	// copy constructor
	Oprnd(const Oprnd &x)
	{
		_regOrImm = x._regOrImm;
		_oprtr = x._oprtr;
		_val = x._val;
	}

	int isRegType() { return ((_regOrImm == _REG_) ? _YES_ : _NO_); }
	int isImmType() { return ((_regOrImm == _IMM_) ? _YES_ : _NO_); }
	int isLabType() { return ((_regOrImm == _LAB_) ? _YES_ : _NO_); }

	int getType() { return _regOrImm; }

	int getOperator() { return _oprtr; }
	int getVal() { return _val; }
};

const int _DONT_CARE_IDENT_ = 0;

// identifier can be :
// <number> or
// <number> <operator> <number>
// <operator> : '+', '-'
//
// Note: The second case can occur only in TARGET or RHS rules
//
typedef struct{
	int idType;
    int ident1;   
	int ident2;   
	int oprtr; // operator
} identType;

class ArgIdent
{
private:
	int _idType; // Whether it is _ID_SINGLE_ or _ID_EXPRESSION_
	int _ident1;
	int _ident2;
	int _operator;

public:
	ArgIdent(int idT = _INVALID_, int id1 = _INVALID_, int id2 = _INVALID_, int op = _INVALID_) : _idType(idT), _ident1(id1), _ident2(id2), _operator(op) { }
	ArgIdent(const ArgIdent & b) : _idType(b._idType), _ident1(b._ident1), _ident2(b._ident2), _operator(b._operator) { }
	~ArgIdent() { }

	void setIDType(int typ) { _idType = typ; }
	int getIDType() { return _idType; }

	// default : set _ident1
	//
	void setIdent(int id) { _ident1 = id; }

	void setIdent1(int id1) { _ident1 = id1; }
	void setIdent2(int id2) { _ident2 = id2; }

	void setOperator(int op) { _operator = op; }

	// default : return _ident1
	//
	int getIdent() { return _ident1; }

	int getIdent1() { return _ident1; }
	int getIdent2() { return _ident2; }

	int getOperator() { return _operator; }
};

class ISelArg{
	ArgIdent _ident;   // identifies this argument by a number
	Oprnd _operand;
	int _num;

public:
	
	ISelArg(int num, Oprnd opnd, ArgIdent & ident) :
	  _ident(ident), _num(num), _operand(opnd)
	  {
	  }

	// copy constructor
	  ISelArg(const ISelArg &x) : _ident(x._ident), _operand(x._operand), _num(x._num) { }

	int getIdent()
	{
		return _ident.getIdent();
	}

	ArgIdent & getArgIdent()
	{
		return _ident;
	}

	Oprnd & getOperand()
	{
		return _operand;
	}

	int getArgNum()
	{
		return _num;
	}

	void setIdent(int ident)
	{
		_ident.setIdent(ident);
	}

	void setNum(int num)
	{
		_num = num;
	}
	  
};

typedef std::vector<ISelArg> ISelArgListType;

// for general opcodes
class ISelOp{

	int _opcode;   
	ISelArgListType *_dstList;
	ISelArgListType *_srcList;

public:
	ISelOp()
	{
	}

	// copy constructor
	ISelOp(const ISelOp &x)
	{
		_opcode = x._opcode;
		_dstList = x._dstList;
		_srcList = x._srcList;
	}

	int getOpcode()
	{
		return _opcode;
	}

	void setDstList(ISelArgListType *dstList)
	{
		_dstList = dstList;
		return;
	}

	void setSrcList(ISelArgListType *srcList)
	{
		_srcList = srcList;
		return;
	}

	void addToSrcList(ISelArg srcArg)
	{
		_srcList->push_back(srcArg);
	}

	void addToDstList(ISelArg dstArg)
	{
		_dstList->push_back(dstArg);
	}

	void addToList(ISelArg arg, int dstOrSrc)
	{
		((dstOrSrc == _DEST_LIST_) ? _dstList->push_back(arg) : _srcList->push_back(arg));
	}

	ISelArgListType *getDstList()
	{
		return _dstList;
	}

	ISelArgListType *getSrcList()
	{
		return _srcList;
	}

	ISelArgListType *getList(int dstOrSrc)
	{
		return ((dstOrSrc == _DEST_LIST_) ? _dstList : _srcList);
	}

//	void addOpcode(char *opcode);

};



// list of operations classified as generic operation
typedef std::vector<ISelOp*> ISelOpListType;

typedef std::vector<ISelOpListType*> ISelRuleType;

extern ISelRuleType genericRules;
extern ISelRuleType targetRules;

// generic opcode mapping to number of rules
typedef std::map<int, ArrayOfIntType> OpcToRuleNoType;

extern OpcToRuleNoType ISelOpcToRuleNo;

extern int numOps(ISelOpListType *rule);

// returns the opcode of the n'th operation in the rule.
// (A rule is a list of ops).
//
// By default, returns the opcode of the first operation in the rule.
//
extern int getOpcode(ISelOpListType *rule, int opPos = 0);

// returns the destination list of the n'th operation in the rule.
// (A rule is a list of ops).
//
// By default, returns the destination list of the first operation in the rule.
//
extern ISelArgListType *destList(ISelOpListType *rule, int opPos = 0);

// returns the source list of the n'th operation in the rule.
// (A rule is a list of ops).
//
// By default, returns the source list of the first operation in the rule.
//
extern ISelArgListType *sourceList(ISelOpListType *rule, int opPos = 0);

//extern ArrayOfIntType & getListOfRuleNums(OpCode & opc, int type = 0);

//extern int getOpcodeIndex(char *opcode);

extern int existsCurrentOp(int currOp);

extern ArrayOfIntType & getRuleNumList(int currOp);

extern void initInstrSel();

//extern int sizeOfMinSizeRule(BaseOperation *op, int size);

extern int getRootTreePos(ISelOpListType *rule);

extern int hasDefInRuleTree(ISelArg & arg, ISelOpListType *rule);

//extern int matchesOpInRule(BaseOperation *op, int pos, ISelOpListType *rule);


/**************************************************************************/
// Specific to Optimizations.cpp and OptimizationsSupport.cpp
/**************************************************************************/

extern ISelRuleType lhsRules;
extern ISelRuleType rhsRules;

extern OpcToRuleNoType OptOpcToRuleNo;

#endif