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

#ifndef WIN32
#include <stdio.h>
#include <stdlib.h>
#endif

#include <math.h>

#include "NormalInstr.h"
#include "UDDU.h"

#include "TreeBuilding.h"
#include "IburgToIR.h"
#include "SemActPrimitives.h"
#include "../DebugSwitches.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

//In all the semantic action routines, there is passed a parameter p, which represents the root
//of the subtree on which the mutation is to be performed. 

int isPow2(int v){
	switch(v){
		case 2:
		case 4:
		case 8:
		case 16:
		case 32:
		case 64:
		case 128:
		case 256:
		case 512:
			return 1;
		default:
			return 0;
	}
	return 0;
}


NODEPTR_TYPE emitShift1(FORMAL_PARAMS){
	NODEPTR_TYPE r;
	int val=GET_INT_ATTR(RIGHT_CHILD(p));
	if(isPow2(val)){
		r=NEW_TREE("ILSH",
					DUPLICATE_TREE(LEFT_CHILD(p)),
					DUPLICATE_TREE(RIGHT_CHILD(p)));
		SET_INT_ATTR(RIGHT_CHILD(r),(int)(log(val)/log(2)));
		setPerformedMut(1);
		MultToShift++;
	}
	else
		r=DUPLICATE_TREE(p);
	if(val==0 || val==1 || val==3 || val==5){
		if(debugMut) cout << "Small constant in MULT1" << endl;
	}
	return r;
}

NODEPTR_TYPE emitShift2(FORMAL_PARAMS){
	NODEPTR_TYPE r;
	int val=GET_INT_ATTR(LEFT_CHILD(p));
	if(isPow2(val)){
		r=NEW_TREE("ILSH",
					DUPLICATE_TREE(RIGHT_CHILD(p)),
					DUPLICATE_TREE(LEFT_CHILD(p)));
		SET_INT_ATTR(RIGHT_CHILD(r),(int)(log(val)/log(2)));
		setPerformedMut(1);
		MultToShift++;
	}
	else
		r=DUPLICATE_TREE(p);
	if(val==0 || val==1 || val==3 || val==5){
		if(debugMut) cout << "Small constant in MULT1" << endl;
	}
	return r;
}

//Mutation 1: MAC

NODEPTR_TYPE emitMAC1(FORMAL_PARAMS){
	ADDOMITTEDOPS(RIGHT_CHILD(LEFT_CHILD(p)));
	return NEW_TREE("DADD",
					DUPLICATE_TREE(RIGHT_CHILD(LEFT_CHILD(p))),
					DUPLICATE_TREE(RIGHT_CHILD(p)));
}

NODEPTR_TYPE emitMAC2(FORMAL_PARAMS){
	ADDOMITTEDOPS(RIGHT_CHILD(RIGHT_CHILD(p)));
	return NEW_TREE("DADD",
					DUPLICATE_TREE(LEFT_CHILD(p)),
					DUPLICATE_TREE(RIGHT_CHILD(RIGHT_CHILD(p))));
}


//Mutation 2: Constant folding through copies

NODEPTR_TYPE constantFold(FORMAL_PARAMS){
	ADDOMITTEDOPS(p);
	ConstFold++;
	setPerformedMut(1);
	return DUPLICATE_TREE(LEFT_CHILD(RIGHT_CHILD(p)));
}

NODEPTR_TYPE copyPropagAssign(FORMAL_PARAMS){
	ADDOMITTEDOPS(p);
	CopyProp++;
	setPerformedMut(1);
	return DUPLICATE_TREE(LEFT_CHILD(RIGHT_CHILD(p)));
}

NODEPTR_TYPE constantFoldIADD(FORMAL_PARAMS){
	NODEPTR_TYPE r;
	ADDOMITTEDOPS(p);
	setPerformedMut(1);
	ConstFold++;
	r=NEW_TREE("ICONSTANT",
					NEW_TREE("INT",0,0),
					0);
	SET_INT_ATTR(LEFT_CHILD(r),GET_INT_ATTR(LEFT_CHILD(p)) + GET_INT_ATTR(RIGHT_CHILD(p)));
	return r;
}

NODEPTR_TYPE constantFoldISUB(FORMAL_PARAMS){
	NODEPTR_TYPE r;
	ADDOMITTEDOPS(p);
	setPerformedMut(1);
	ConstFold++;
	r=NEW_TREE("ICONSTANT",
					NEW_TREE("INT",0,0),
					0);
	SET_INT_ATTR(LEFT_CHILD(r),GET_INT_ATTR(LEFT_CHILD(p)) - GET_INT_ATTR(RIGHT_CHILD(p)));
	return r;
}

NODEPTR_TYPE constantFoldDMUL(FORMAL_PARAMS){
	NODEPTR_TYPE r;
	ADDOMITTEDOPS(p);
	setPerformedMut(1);
	ConstFold++;
	r=NEW_TREE("ICONSTANT",
					NEW_TREE("INT",0,0),
					0);
	SET_INT_ATTR(LEFT_CHILD(r),GET_INT_ATTR(LEFT_CHILD(p)) * GET_INT_ATTR(RIGHT_CHILD(p)));
	return r;
}

NODEPTR_TYPE THRCF1(FORMAL_PARAMS){
	NODEPTR_TYPE r,q,s;
	ADDOMITTEDOPS(LEFT_CHILD(p));
	THRCF++;
	s=RIGHT_CHILD(LEFT_CHILD(p));
	q=NEW_TREE("INT",0,0);
	SET_INT_ATTR(q,GET_INT_ATTR(RIGHT_CHILD(p)) + GET_INT_ATTR(RIGHT_CHILD(s)));
	r=NEW_TREE("IADD",
					DUPLICATE_TREE(LEFT_CHILD(s)),
					q);
	setPerformedMut(1);
	return r;
}

NODEPTR_TYPE THRCF2(FORMAL_PARAMS){
	NODEPTR_TYPE r,q,s;
	ADDOMITTEDOPS(RIGHT_CHILD(p));
	THRCF++;
	s=RIGHT_CHILD(RIGHT_CHILD(p));
	q=NEW_TREE("INT",0,0);
	SET_INT_ATTR(q,GET_INT_ATTR(LEFT_CHILD(p)) + GET_INT_ATTR(RIGHT_CHILD(s)));
	r=NEW_TREE("IADD",
					DUPLICATE_TREE(LEFT_CHILD(s)),
					q);
	setPerformedMut(1);
	return r;
}

NODEPTR_TYPE THRCF3(FORMAL_PARAMS){
	NODEPTR_TYPE r,q,s;
	ADDOMITTEDOPS(LEFT_CHILD(p));
	THRCF++;
	s=RIGHT_CHILD(LEFT_CHILD(p));
	q=NEW_TREE("INT",0,0);
	SET_INT_ATTR(q,GET_INT_ATTR(RIGHT_CHILD(p)) + GET_INT_ATTR(LEFT_CHILD(s)));
	r=NEW_TREE("IADD",
					DUPLICATE_TREE(RIGHT_CHILD(s)),
					q);
	setPerformedMut(1);
	return r;
}

NODEPTR_TYPE THRCF4(FORMAL_PARAMS){
	NODEPTR_TYPE r,q,s;
	ADDOMITTEDOPS(RIGHT_CHILD(p));
	THRCF++;
	s=RIGHT_CHILD(RIGHT_CHILD(p));
	q=NEW_TREE("INT",0,0);
	SET_INT_ATTR(q,GET_INT_ATTR(LEFT_CHILD(p)) + GET_INT_ATTR(LEFT_CHILD(s)));
	r=NEW_TREE("IADD",
					DUPLICATE_TREE(RIGHT_CHILD(s)),
					q);
	setPerformedMut(1);
	return r;
}

NODEPTR_TYPE DCONST_CF(FORMAL_PARAMS){
	ADDOMITTEDOPS(p);
	setPerformedMut(1);
	ConstFold++;
	return DUPLICATE_TREE(LEFT_CHILD(RIGHT_CHILD(p)));
}


NODEPTR_TYPE IADD_THR1(FORMAL_PARAMS){
	NODEPTR_TYPE a1,a2,a3,a4,a5;
	BaseOperation *op1,*op2;
	char *oper=get_term_str(OP_LABEL(RIGHT_CHILD(p)));
	int i1,i2,i3,i4;
	a1=p;
	a2=LEFT_CHILD(RIGHT_CHILD(a1));
	a3=LEFT_CHILD(RIGHT_CHILD(a2));

	op1=GetIburgToOpMap(a2);
	op2=GetIburgToOpMap(a3);

	i1=INSTR_NO(RIGHT_CHILD(RIGHT_CHILD(a1)));
	i2=INSTR_NO(a1);
	i3=INSTR_NO(RIGHT_CHILD(RIGHT_CHILD(a2)));
	i4=INSTR_NO(a2);

	if(i1 < i2 - 2){
		if(i3 < i4 - 1){
			ADDOMITTEDOPS(a2);

			a5=NEW_TREE("DEF",
						NEW_TREE(NEW_TEMP(),0,0),
						NEW_TREE(oper,
							DUPLICATE_TREE(RIGHT_CHILD(RIGHT_CHILD(a2))),
							DUPLICATE_TREE(RIGHT_CHILD(RIGHT_CHILD(a1)))));
			a4=NEW_TREE("DEF",
						DUPLICATE_TREE(LEFT_CHILD(a1)),
						NEW_TREE(oper,
							DUPLICATE_TREE(a3),
							a5));

			THR++;

			if(op1!=NULL){
				AddIburgToOpMap(a4,op1);
				DeleteIburgToOpMap(a2);
			}
			if(op2!=NULL){
				AddIburgToOpMap(a5,op2);
			}
			setPerformedMut(1);
			return a4;
		}
	}

	return DUPLICATE_TREE(p);
}

NODEPTR_TYPE IADD_THR2(FORMAL_PARAMS){
	NODEPTR_TYPE a1,a2,a3,a4,a5;
	BaseOperation *op1,*op2;
	char *oper=get_term_str(OP_LABEL(RIGHT_CHILD(p)));
	int i1,i2,i3,i4;
	a1=p;
	a2=RIGHT_CHILD(RIGHT_CHILD(a1));
	a3=RIGHT_CHILD(RIGHT_CHILD(a2));

	op1=GetIburgToOpMap(a2);
	op2=GetIburgToOpMap(a3);

	i1=INSTR_NO(LEFT_CHILD(RIGHT_CHILD(a1)));
	i2=INSTR_NO(a1);
	i3=INSTR_NO(LEFT_CHILD(RIGHT_CHILD(a2)));
	i4=INSTR_NO(a2);

	if(i1 < i2 - 2){
		if(i3 < i4 - 1){
			THR++;
			
			ADDOMITTEDOPS(a2);

			a5=NEW_TREE("DEF",
						NEW_TREE(NEW_TEMP(),0,0),
						NEW_TREE(oper,
							DUPLICATE_TREE(LEFT_CHILD(RIGHT_CHILD(a1))),
							DUPLICATE_TREE(LEFT_CHILD(RIGHT_CHILD(a2)))));
			a4=NEW_TREE("DEF",
						DUPLICATE_TREE(LEFT_CHILD(a1)),
						NEW_TREE(oper,
							a5,
							DUPLICATE_TREE(a3)));

			if(op1!=NULL){
				AddIburgToOpMap(a4,op1);
				DeleteIburgToOpMap(a2);
			}
			if(op2!=NULL){
				AddIburgToOpMap(a5,op2);
			}
			setPerformedMut(1);
			return a4;
		}
	}

	return DUPLICATE_TREE(p);
}


NODEPTR_TYPE IADD_THR3(FORMAL_PARAMS){
	NODEPTR_TYPE a1,a2,a3,a4,a5;
	BaseOperation *op1,*op2;
	char *oper=get_term_str(OP_LABEL(RIGHT_CHILD(p)));
	int i1,i2,i3,i4;
	a1=p;
	a2=LEFT_CHILD(RIGHT_CHILD(a1));
	a3=RIGHT_CHILD(RIGHT_CHILD(a2));

	op1=GetIburgToOpMap(a2);
	op2=GetIburgToOpMap(a3);

	i1=INSTR_NO(RIGHT_CHILD(RIGHT_CHILD(a1)));
	i2=INSTR_NO(a1);
	i3=INSTR_NO(LEFT_CHILD(RIGHT_CHILD(a2)));
	i4=INSTR_NO(a2);

	if(i1 < i2 - 2){
		if(i3 < i4 - 1){
			ADDOMITTEDOPS(a2);

			a5=NEW_TREE("DEF",
						NEW_TREE(NEW_TEMP(),0,0),
						NEW_TREE(oper,
							DUPLICATE_TREE(LEFT_CHILD(RIGHT_CHILD(a2))),
							DUPLICATE_TREE(RIGHT_CHILD(RIGHT_CHILD(a1)))));
			a4=NEW_TREE("DEF",
						DUPLICATE_TREE(LEFT_CHILD(a1)),
						NEW_TREE(oper,
							DUPLICATE_TREE(a3),
							a5));

			THR++;

			if(op1!=NULL){
				AddIburgToOpMap(a4,op1);
				DeleteIburgToOpMap(a2);
			}
			if(op2!=NULL){
				AddIburgToOpMap(a5,op2);
			}
			setPerformedMut(1);
			return a4;
		}
	}

	return DUPLICATE_TREE(p);
}

NODEPTR_TYPE IADD_THR4(FORMAL_PARAMS){
	NODEPTR_TYPE a1,a2,a3,a4,a5;
	BaseOperation *op1,*op2;
	char *oper=get_term_str(OP_LABEL(RIGHT_CHILD(p)));
	int i1,i2,i3,i4;
	a1=p;
	a2=RIGHT_CHILD(RIGHT_CHILD(a1));
	a3=LEFT_CHILD(RIGHT_CHILD(a2));

	op1=GetIburgToOpMap(a2);
	op2=GetIburgToOpMap(a3);

	i1=INSTR_NO(LEFT_CHILD(RIGHT_CHILD(a1)));
	i2=INSTR_NO(a1);
	i3=INSTR_NO(RIGHT_CHILD(RIGHT_CHILD(a2)));
	i4=INSTR_NO(a2);

	if(i1 < i2 - 2){
		if(i3 < i4 - 1){
			THR++;
			
			ADDOMITTEDOPS(a2);

			a5=NEW_TREE("DEF",
						NEW_TREE(NEW_TEMP(),0,0),
						NEW_TREE(oper,
							DUPLICATE_TREE(LEFT_CHILD(RIGHT_CHILD(a1))),
							DUPLICATE_TREE(RIGHT_CHILD(RIGHT_CHILD(a2)))));
			a4=NEW_TREE("DEF",
						DUPLICATE_TREE(LEFT_CHILD(a1)),
						NEW_TREE(oper,
							a5,
							DUPLICATE_TREE(a3)));

			if(op1!=NULL){
				AddIburgToOpMap(a4,op1);
				DeleteIburgToOpMap(a2);
			}
			if(op2!=NULL){
				AddIburgToOpMap(a5,op2);
			}
			setPerformedMut(1);
			return a4;
		}
	}

	return DUPLICATE_TREE(p);
}





NODEPTR_TYPE DMUL_THR1(FORMAL_PARAMS){
	NODEPTR_TYPE a1,a2,a3,a4,a5;
	BaseOperation *op1,*op2;
	int i1,i2,i3,i4;
	a1=p;
	a2=LEFT_CHILD(RIGHT_CHILD(a1));
	a3=LEFT_CHILD(RIGHT_CHILD(a2));

	op1=GetIburgToOpMap(a2);
	op2=GetIburgToOpMap(a3);

	i1=INSTR_NO(RIGHT_CHILD(RIGHT_CHILD(a1)));
	i2=INSTR_NO(a1);
	i3=INSTR_NO(RIGHT_CHILD(RIGHT_CHILD(a2)));
	i4=INSTR_NO(a2);

	if(i1 < i2 - 2){
		if(i3 < i4 - 1){
			ADDOMITTEDOPS(a2);

			a5=NEW_TREE("DEF",
						NEW_TREE(NEW_TEMP(),0,0),
						NEW_TREE("DMUL",
							DUPLICATE_TREE(RIGHT_CHILD(RIGHT_CHILD(a2))),
							DUPLICATE_TREE(RIGHT_CHILD(RIGHT_CHILD(a1)))));
			a4=NEW_TREE("DEF",
						DUPLICATE_TREE(LEFT_CHILD(a1)),
						NEW_TREE("DMUL",
							DUPLICATE_TREE(a3),
							a5));

			THR++;

			if(op1!=NULL){
				AddIburgToOpMap(a4,op1);
				DeleteIburgToOpMap(a2);
			}
			if(op2!=NULL){
				AddIburgToOpMap(a5,op2);
			}
			setPerformedMut(1);
			return a4;
		}
	}

	return DUPLICATE_TREE(p);
}


NODEPTR_TYPE SemanticAction(FORMAL_PARAMS){
	if(debugMut) cout << flush;
	switch(eruleno){
	case 201://expr:	DADD(DEF(reg,DMUL(def,def)),def)	= 201 (4);
		if(debugMut) cout << "  //Mutation 1: MAC";
		//setPerformedMut(1);
		//return emitMAC1(ACTUAL_PARAMS);
		break;
	case 202://expr:	DADD(def,DEF(reg,DMUL(def,def)))	= 202 (4);
		if(debugMut) cout << "  //Mutation 1: MAC";
		//setPerformedMut(1);
		//return emitMAC2(ACTUAL_PARAMS);
		break;


	case 204://def:	DEF(reg,ICONSTANT(const))			= 204 (0);
		if(debugMut) cout << "  //Mutation 2: Constant folding through copies";
		return constantFold(ACTUAL_PARAMS);


	case 205://def:	DEF(reg,IASSIGN(def))				= 205 (0);
	case 224://def:	DEF(reg,ASSIGN(def))				= 224 (0);
		if(debugMut) cout << "  //Mutation 3: Copy propagation & copy removal";
		return copyPropagAssign(ACTUAL_PARAMS);

	case 207://expr:	IADD(INT, INT)					= 207 (0);
		if(debugMut) cout << "  //Mutation 7: More constant folding";
		return constantFoldIADD(ACTUAL_PARAMS);
	case 208://expr:	ISUB(INT, INT)					= 208 (0);
		if(debugMut) cout << "  //Mutation 7: More constant folding";
		return constantFoldISUB(ACTUAL_PARAMS);
	case 209://expr:	DMUL(INT, INT)					= 209 (0);
		if(debugMut) cout << "  //Mutation 7: More constant folding";
		return constantFoldDMUL(ACTUAL_PARAMS);


	case 220://expr:	DMUL(reg, INT)					= 220 (2);
		if(debugMut) cout << "  //Mutation: MUL => SHIFT";
		return emitShift1(ACTUAL_PARAMS);
	case 221://expr:	DMUL(INT, reg)					= 221 (2);
		if(debugMut) cout << "  //Mutation: MUL => SHIFT";
		return emitShift2(ACTUAL_PARAMS);

	case 211://expr:	DMUL(DEF(reg,DMUL(def,INT)),INT)= 211 (4);
	case 212://expr:	DMUL(INT,DEF(reg,DMUL(def,INT)))= 212 (4);
	case 213://expr:	DMUL(DEF(reg,DMUL(INT,def)),INT)= 213 (4);
	case 214://expr:	DMUL(INT,DEF(reg,DMUL(INT,def)))= 214 (4);
		if(debugMut) cout << "  //Mutation 7: Constant folding with THR";
		break;	

	case 225://def:	DEF(reg,IADD(DEF(reg,IADD(DEF(reg,expr),def)),def))	= 225 (3);
	case 239://def:	DEF(reg,DADD(DEF(reg,DADD(DEF(reg,expr),def)),def))	= 239 (3);
		if(debugMut) cout << "  //Mutation: Incremental THR1 on 3 ADDS";
		return IADD_THR1(ACTUAL_PARAMS);
	case 226://def:	DEF(reg,IADD(def,DEF(reg,IADD(def,DEF(reg,expr)))))	= 226 (3);
	case 240://def:	DEF(reg,DADD(def,DEF(reg,DADD(def,DEF(reg,expr)))))	= 240 (3);
		if(debugMut) cout << "  //Mutation: Incremental THR2 on 3 ADDS";
		return IADD_THR2(ACTUAL_PARAMS);
	case 235://def:	DEF(reg,IADD(DEF(reg,IADD(def,DEF(reg,expr))),def))	= 235 (3);
	case 241://def:	DEF(reg,DADD(DEF(reg,DADD(def,DEF(reg,expr))),def))	= 241 (3);
		if(debugMut) cout << "  //Mutation: Incremental THR3 on 3 ADDS";
		return IADD_THR3(ACTUAL_PARAMS);
	case 236://def:	DEF(reg,IADD(def,DEF(reg,IADD(DEF(reg,expr),def))))	= 236 (3);
	case 242://def:	DEF(reg,DADD(def,DEF(reg,DADD(DEF(reg,expr),def))))	= 242 (3);
		if(debugMut) cout << "  //Mutation: Incremental THR4 on 3 ADDS";
		return IADD_THR4(ACTUAL_PARAMS);


	case 227://expr:	IADD(DEF(reg,IADD(def,INT)),INT)= 227 (2);
		if(debugMut) cout << "  //Mutation 7: Constant folding with THR";
		return THRCF1(ACTUAL_PARAMS);
	case 228://expr:	IADD(INT,DEF(reg,IADD(def,INT)))= 228 (2);
		if(debugMut) cout << "  //Mutation 7: Constant folding with THR";
		return THRCF2(ACTUAL_PARAMS);
	case 229://expr:	IADD(DEF(reg,IADD(INT,def)),INT)= 229 (2);
		if(debugMut) cout << "  //Mutation 7: Constant folding with THR";
		return THRCF3(ACTUAL_PARAMS);
	case 230://expr:	IADD(INT,DEF(reg,IADD(INT,def)))= 230 (2);
		if(debugMut) cout << "  //Mutation 7: Constant folding with THR";
		return THRCF4(ACTUAL_PARAMS);


	case 223://def:	DEF(reg,DCONSTANT(const))			= 223 (0);
		if(debugMut) cout << "  //Mutation: Constant folding on DCONSTANT";
		return DCONST_CF(ACTUAL_PARAMS);


	case 231://def:	DEF(reg,DMUL(DEF(reg,DMUL(DEF(reg,expr),def)),def))	= 231 (3);
		if(debugMut) cout << "  //Mutation: Incremental THR4 on 3 DMULS";
		return DMUL_THR1(ACTUAL_PARAMS);
	case 232://def:	DEF(reg,DMUL(def,DEF(reg,DMUL(def,DEF(reg,expr)))))	= 232 (3);
		//0 times
		break;
	case 233://def:	DEF(reg,ISUB(DEF(reg,ISUB(DEF(reg,expr),def)),def))	= 233 (3);
	case 234://def:	DEF(reg,ISUB(def,DEF(reg,ISUB(def,DEF(reg,expr)))))	= 234 (3);
		//0 times
		break;

	case 237://expr:	DMUL(DEF(reg,DADD(def,def)),def)	= 237 (5);
	case 238://expr:	DMUL(def,DEF(reg,DADD(def,def)))	= 238 (5);
		//18 times
		if(debugMut) cout << "  //Mutation 8: Distributivity";
		//TempCount++;
		break;
	
	default:
		break;
	}
	return DUPLICATE_TREE(p);
}
