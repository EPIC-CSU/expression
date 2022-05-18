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
//: IRHelper.h
//
// File:  		IRHelper.h
// Author:		Radu Cornea
// Email:		radu@ics.uci.edu, ilp@ics.uci.edu
//
// Miscellaneous helper functions
// 

#ifndef __IRHELPER_H__
#define __IRHELPER_H__

#include "BaseArgument.h"
#include "SetList.h"

class IRHelper {

public:

	static int getIDOfArg(BaseArgument *arg);
	static BaseArgument * createArg(int argID);

	static BaseArgument * getSiblingArg(BaseArgument *arg);

	static int hasCallParamArg(NormalInstr *instr, int argID);

	static SetList<BaseArgument *> * getAllDefsInOper(BaseOperation *oper);
	static SetList<BaseArgument *> * getAllDefsInInstr(BaseInstruction *instr);
	static SetList<BaseArgument *> * getAllUsesInOper(BaseOperation *oper);
	static SetList<BaseArgument *> * getAllUsesInInstr(BaseInstruction *instr);

	static SetList<BaseArgument *> * getAllDelayedReadsInInstr(BaseInstruction *instr);
	static SetList<BaseArgument *> * getAllNotDelayedReadsInInstr(BaseInstruction *instr);

	static SetList<BaseArgument *> * getAllMoveOpUsesInInstr(BaseInstruction *instr);

	static BaseArgument * getDefOfArgInOper(BaseArgument *arg,
											 BaseOperation * op);
	static BaseArgument * getDefOfArgInInstr(BaseArgument *arg,
											 BaseInstruction * instr);

	static BaseArgument * getDefOfArgInOper(int argID,
											 BaseOperation * op);
	static BaseArgument * getDefOfArgInInstr(int argID,
											 BaseInstruction * instr);

	static SetList<BaseArgument *> * getUsesOfArgInOper(BaseArgument *arg,
															  BaseOperation *op);
	static SetList<BaseArgument *> * getUsesOfArgInInstr(BaseArgument *arg,
															  BaseInstruction *instr);

	static SetList<BaseArgument *> * getUsesOfArgInOper(int argID,
															  BaseOperation *op);
	static SetList<BaseArgument *> * getUsesOfArgInInstr(int argID,
															  BaseInstruction *instr);

	static BaseOperation * getOperationWithArgInInstr(BaseArgument * testArg, NormalInstr * instr);

	static void addArgsToLiveList(MyLinkedList<BaseArgument *> *argList, SetList<int>& liveList);
	static void delArgsFromLiveList(MyLinkedList<BaseArgument *> *argList, SetList<int>& liveList);

	static void changeRegWithSSA(BaseArgument *toChange, BaseArgument *orig);
};

#endif // __IRHELPER_H__

