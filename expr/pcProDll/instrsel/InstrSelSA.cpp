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
#include "InstrSelSAPrim.h"
#include "InstrSelIburg.h"
#include "LabelMoving.h"

vector <defNodePairType *> nodePairs;


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

extern int isPow2(int v);



NODEPTR_TYPE InstrSelSA(FORMAL_PARAMS){
	int val;
	switch(eruleno){
	
	case 5://IADD(def,def)		= 5 (2);
		setPerformedMut(1);
		return IS_NEW_TREE("addu",
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					IS_DUPLICATE_TREE(RIGHT_CHILD(p))
					);
	case 6://ISUB(def,def)		= 6 (2);
		setPerformedMut(1);
		return IS_NEW_TREE("subu",
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					IS_DUPLICATE_TREE(RIGHT_CHILD(p))
					);
	case 7://IVLOAD(indirect)	= 7 (2);
		setPerformedMut(1);
		return IS_NEW_TREE("lw",
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					IS_DUPLICATE_TREE(RIGHT_CHILD(p))
					);
	case 8://IVSTORE(def,indirect)	= 8 (2);
		setPerformedMut(1);
		return IS_NEW_TREE("sw",
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					IS_DUPLICATE_TREE(RIGHT_CHILD(p))
					);
	case 9://IASSIGN(def)		= 9 (1);
		setPerformedMut(1);
		return IS_NEW_TREE("move",
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					NULL
					);
	case 10://ILSH(def,const)		= 10 (1);
		setPerformedMut(1);
				val=GET_INT_ATTR(RIGHT_CHILD(p));
		setPerformedMut(1);
		if(val<0){ 
			SET_INT_ATTR(RIGHT_CHILD(p),-val);
			return IS_NEW_TREE("sll",
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					IS_DUPLICATE_TREE(RIGHT_CHILD(p))
					);
		}
		else return IS_NEW_TREE("srl",
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					IS_DUPLICATE_TREE(RIGHT_CHILD(p))
					);
		
	case 11://IASH(def,const)		= 11 (1);
		setPerformedMut(1);
		val=GET_INT_ATTR(RIGHT_CHILD(p));
		setPerformedMut(1);
		if(val<0){
			SET_INT_ATTR(RIGHT_CHILD(p),-val);
			return IS_NEW_TREE("sla",
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					IS_DUPLICATE_TREE(RIGHT_CHILD(p))
					);
		}
		else return IS_NEW_TREE("sra",
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					IS_DUPLICATE_TREE(RIGHT_CHILD(p))
					);
		break;
		
	case 12://DMUL(def,def)		= 12 (4);
		setPerformedMut(1);
		return IS_NEW_TREE("mul_d",  // "dmult" also possible
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					IS_DUPLICATE_TREE(RIGHT_CHILD(p))
					);
	case 13://DADD(def,def)		= 13 (3);
		setPerformedMut(1);
		return IS_NEW_TREE("add_d",
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					IS_DUPLICATE_TREE(RIGHT_CHILD(p))
					);
	case 14://DSUB(def,def)		= 14 (3);
		setPerformedMut(1);
		return IS_NEW_TREE("sub_d",
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					IS_DUPLICATE_TREE(RIGHT_CHILD(p))
					);
	case 15://ICONSTANT(const)	= 15 (1);
		setPerformedMut(1);
		return IS_NEW_TREE("la",  
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					NULL
					);
	case 16://ASSIGN(def)			= 16 (1);
		setPerformedMut(1);
		return IS_NEW_TREE("move",
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					NULL
					);
	case 17://IGE(def,def)		= 17 (1);
		setPerformedMut(1);
		return IS_NEW_TREE("sge",   
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					IS_DUPLICATE_TREE(RIGHT_CHILD(p))
					);
	case 18://ILT(def,def)		= 18 (1);
		setPerformedMut(1);
		return IS_NEW_TREE("slt",
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					IS_DUPLICATE_TREE(RIGHT_CHILD(p))
					);
	case 19://ILE(def,def)		= 19 (1);
		setPerformedMut(1);
		return IS_NEW_TREE("sle",   
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					IS_DUPLICATE_TREE(RIGHT_CHILD(p))
					);
	case 20://IGT(def,def)		= 20 (1);
		setPerformedMut(1);
		return IS_NEW_TREE("sgt",   
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					IS_DUPLICATE_TREE(RIGHT_CHILD(p))
					);
	case 21://DMTC1(def)			= 21 (1);
		setPerformedMut(1);
		return IS_NEW_TREE("move",   // not sure
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					NULL
					);
	case 22://TRUNCID(def)		= 22 (1);
		setPerformedMut(1);
		return IS_NEW_TREE("trunc_w_d",  
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					NULL
					);
	case 23://MFC1(def)			= 23 (1);
		setPerformedMut(1);
		return IS_NEW_TREE("mfc1",   
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					NULL
					);
		break;
	case 24://DCONSTANT(const_label)	= 24 (1);
		setPerformedMut(1);
		return IS_NEW_TREE("la",   
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					NULL
					);
	case 25://MTC1(def)			= 25 (1);
		setPerformedMut(1);
		return IS_NEW_TREE("mtc1",
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					NULL
					);
	case 26://CVTDI(def)			= 26 (1);
		setPerformedMut(1);
		return IS_NEW_TREE("cvt_d_w",
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					NULL
					);
	case 27://ILAND(def,def)			= 27 (1);
		setPerformedMut(1);
		return IS_NEW_TREE("and",
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					IS_DUPLICATE_TREE(RIGHT_CHILD(p))
					);
	case 28://DASSIGN(def)		= 28 (1);
		setPerformedMut(1);
		return IS_NEW_TREE("mov_d",    
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					NULL
					);
	case 29://DGE(def,def)		= 29 (1);
		setPerformedMut(1);
		return IS_NEW_TREE("c_ge_d",   
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					IS_DUPLICATE_TREE(RIGHT_CHILD(p))
					);
	case 30://DLT(def,def)		= 30 (1);
		setPerformedMut(1);
		return IS_NEW_TREE("c_lt_d",    
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					IS_DUPLICATE_TREE(RIGHT_CHILD(p))
					);
	case 31://DLE(def,def)		= 31 (1);
		setPerformedMut(1);
		return IS_NEW_TREE("c_le_d",   
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					IS_DUPLICATE_TREE(RIGHT_CHILD(p))
					);
	case 32://DGT(def,def)		= 32 (1);
		setPerformedMut(1);
		return IS_NEW_TREE("c_gt_d",   
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					IS_DUPLICATE_TREE(RIGHT_CHILD(p))
					);
	case 33://CVTSD(def)			= 33 (1);
		setPerformedMut(1);
		return IS_NEW_TREE("cvt_s_d", 
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					IS_DUPLICATE_TREE(RIGHT_CHILD(p))
					);
	case 34://CVTDS(def)			= 34 (1);
		setPerformedMut(1);
		return IS_NEW_TREE("cvt_d_s", 
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					IS_DUPLICATE_TREE(RIGHT_CHILD(p))
					);
	case 35://DMFC1(def)			= 35 (1);
		setPerformedMut(1);
		return IS_NEW_TREE("move",   // not sure
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					IS_DUPLICATE_TREE(RIGHT_CHILD(p))
					);
	case 36://DDIV(def,def)		= 36 (3);
		setPerformedMut(1);
		return IS_NEW_TREE("div_d", 
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					IS_DUPLICATE_TREE(RIGHT_CHILD(p))
					);
		/*
		val=GET_INT_ATTR(RIGHT_CHILD(p));
		if(isPow2(val)){
			SET_INT_ATTR(RIGHT_CHILD(p),(int)(log(val)/log(2)));
			return IS_NEW_TREE("SHR",
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					IS_DUPLICATE_TREE(RIGHT_CHILD(p))
					);
		}
		printf("Warning: DIV not supported for c6x\n");
		break;
		*/
	case 37://DEQ(def,def)		= 37 (2);
		setPerformedMut(1);
		return IS_NEW_TREE("seq",    // not sure
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					IS_DUPLICATE_TREE(RIGHT_CHILD(p))
					);
	case 38://FADD(def,def)		= 38 (3);
		setPerformedMut(1);
		return IS_NEW_TREE("add_s",
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					IS_DUPLICATE_TREE(RIGHT_CHILD(p))
					);
	case 39://FSUB(def,def)		= 39 (3);
		setPerformedMut(1);
		return IS_NEW_TREE("sub_s",
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					IS_DUPLICATE_TREE(RIGHT_CHILD(p))
					);
	case 40://FMUL(def,def)		= 40 (3);
		setPerformedMut(1);
		return IS_NEW_TREE("mul_s",
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					IS_DUPLICATE_TREE(RIGHT_CHILD(p))
					);
	case 41://FDIV(def,def)		= 41 (3);
		setPerformedMut(1);
		return IS_NEW_TREE("div_s",
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					IS_DUPLICATE_TREE(RIGHT_CHILD(p))
					);
		/*
		val=GET_INT_ATTR(RIGHT_CHILD(p));
		if(isPow2(val)){
			SET_INT_ATTR(RIGHT_CHILD(p),(int)(log(val)/log(2)));
			return IS_NEW_TREE("SHR",
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					IS_DUPLICATE_TREE(RIGHT_CHILD(p))
					);
		}
		printf("Warning: DIV not supported for c6x\n");
		break;
		*/
	case 42://expr:	DVLOAD(indirect)	= 42 (2);
		setPerformedMut(1);
		return IS_NEW_TREE("l_d",  
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					IS_DUPLICATE_TREE(RIGHT_CHILD(p))
					);
	case 43://def:	DVSTORE(def,indirect)	= 43 (2);
		setPerformedMut(1);
		return IS_NEW_TREE("s_d",
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					IS_DUPLICATE_TREE(RIGHT_CHILD(p))
					);
	case 44://expr:	FVLOAD(indirect)	= 44 (2);
		setPerformedMut(1);
		return IS_NEW_TREE("l_s",   
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					IS_DUPLICATE_TREE(RIGHT_CHILD(p))
					);
	case 45://def:	FVSTORE(def,indirect)	= 45 (2);
		setPerformedMut(1);
		return IS_NEW_TREE("s_s", 
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					IS_DUPLICATE_TREE(RIGHT_CHILD(p))
					);
	case 46://ICONSTANT(const)	= 46 (1);
		setPerformedMut(1);
		return IS_NEW_TREE("li",   
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					NULL
					);
	case 47://IRASH(def,const)		= 47 (1);
		setPerformedMut(1);
		return IS_NEW_TREE("sra",
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					IS_DUPLICATE_TREE(RIGHT_CHILD(p))
					);
	case 48://ILASH(def,const)		= 48 (1);
		setPerformedMut(1);
		return IS_NEW_TREE("sla",
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					IS_DUPLICATE_TREE(RIGHT_CHILD(p))
					);
	case 49://IRLSH(def,const)		= 49 (1);
		setPerformedMut(1);
		return IS_NEW_TREE("srl",
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					IS_DUPLICATE_TREE(RIGHT_CHILD(p))
					);
	case 50://ILLSH(def,const)		= 50 (1);
		setPerformedMut(1);
		return IS_NEW_TREE("sll",
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					IS_DUPLICATE_TREE(RIGHT_CHILD(p))
					);
	case 51://FCONSTANT(const_label)	= 51 (1);
		setPerformedMut(1);
		return IS_NEW_TREE("la",   
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					NULL
					);
	case 52://FCONSTANT(float_const)	= 52 (1);
		setPerformedMut(1);
		return IS_NEW_TREE("li_s",   
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					NULL
					);
	case 53://FASSIGN(def)		= 53 (1);
		setPerformedMut(1);
		return IS_NEW_TREE("mov_s",    
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					NULL
					);
	case 54://INE(def,def)		= 54 (1);
		setPerformedMut(1);
		return IS_NEW_TREE("sne",   // not sure
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					IS_DUPLICATE_TREE(RIGHT_CHILD(p))
					);
	case 55://TRUNCIS(def)		= 55 (1);
		setPerformedMut(1);
		return IS_NEW_TREE("trunc_w_s",  
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					NULL
					);
	case 56://CVTSI(def)			= 56 (1);
		setPerformedMut(1);
		return IS_NEW_TREE("cvt_s_w",
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					NULL
					);
	case 57://DCONSTANT(float_const)	= 57 (1);
		setPerformedMut(1);
		return IS_NEW_TREE("li_d",   
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					NULL
					);
	case 58://IEQ(def,def)		= 58 (1);
		setPerformedMut(1);
		return IS_NEW_TREE("seq",   
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					IS_DUPLICATE_TREE(RIGHT_CHILD(p))
					);
	case 59://IOR(const,def)		= 59 (1);
		setPerformedMut(1);
		return IS_NEW_TREE("ori",   
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					IS_DUPLICATE_TREE(RIGHT_CHILD(p))
					);
	case 60://IAND(const,def)		= 60 (1);
		setPerformedMut(1);
		return IS_NEW_TREE("andi",   
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					IS_DUPLICATE_TREE(RIGHT_CHILD(p))
					);
	case 61://IXOR(def,def)		= 61 (1);
		setPerformedMut(1);
		return IS_NEW_TREE("andi",   
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					IS_DUPLICATE_TREE(RIGHT_CHILD(p))
					);
	case 62://IMUL(def,def)		= 62 (1);
		setPerformedMut(1);
		return IS_NEW_TREE("mult",   
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					IS_DUPLICATE_TREE(RIGHT_CHILD(p))
					);
	case 63://MFLO(def)		= 63 (1);
		setPerformedMut(1);
		return IS_NEW_TREE("mflo",   
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					NULL
					);
	case 64://QIVLOADU(indirect)	= 64 (2);
		setPerformedMut(1);
		return IS_NEW_TREE("lbu",
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					IS_DUPLICATE_TREE(RIGHT_CHILD(p))
					);
	case 65://QIVLOAD(indirect)	= 65 (2);
		setPerformedMut(1);
		return IS_NEW_TREE("lb",
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					IS_DUPLICATE_TREE(RIGHT_CHILD(p))
					);
	case 66://QIVSTORE(def,indirect)	= 66 (2);
		setPerformedMut(1);
		return IS_NEW_TREE("sb",
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					IS_DUPLICATE_TREE(RIGHT_CHILD(p))
					);
	case 67://QIVSTOREU(def,indirect)	= 67 (2);
		setPerformedMut(1);
		return IS_NEW_TREE("sbu",
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					IS_DUPLICATE_TREE(RIGHT_CHILD(p))
					);
	case 68://HIVLOADU(indirect)	= 68 (2);
		setPerformedMut(1);
		return IS_NEW_TREE("lhu",
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					IS_DUPLICATE_TREE(RIGHT_CHILD(p))
					);
	case 69://HIVLOAD(indirect)	= 69 (2);
		setPerformedMut(1);
		return IS_NEW_TREE("lh",
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					IS_DUPLICATE_TREE(RIGHT_CHILD(p))
					);
	case 70://HIVSTORE(def,indirect)	= 70 (2);
		setPerformedMut(1);
		return IS_NEW_TREE("sh",
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					IS_DUPLICATE_TREE(RIGHT_CHILD(p))
					);
	case 71://HIVSTOREU(def,indirect)	= 71 (2);
		setPerformedMut(1);
		return IS_NEW_TREE("shu",
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					IS_DUPLICATE_TREE(RIGHT_CHILD(p))
					);
	case 72://IGEU(def,def)		= 72 (1);
		setPerformedMut(1);
		return IS_NEW_TREE("sgeu",   
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					IS_DUPLICATE_TREE(RIGHT_CHILD(p))
					);
	case 73://ILTU(def,def)		= 73 (1);
		setPerformedMut(1);
		return IS_NEW_TREE("sltu",
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					IS_DUPLICATE_TREE(RIGHT_CHILD(p))
					);
	case 74://ILEU(def,def)		= 74 (1);
		setPerformedMut(1);
		return IS_NEW_TREE("sleu",   
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					IS_DUPLICATE_TREE(RIGHT_CHILD(p))
					);
	case 75://IGTU(def,def)		= 75 (1);
		setPerformedMut(1);
		return IS_NEW_TREE("sgtu",   
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					IS_DUPLICATE_TREE(RIGHT_CHILD(p))
					);
	case 76://IDIV(def,def)		= 76 (1);
		setPerformedMut(1);
		return IS_NEW_TREE("div",   
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					IS_DUPLICATE_TREE(RIGHT_CHILD(p))
					);
	case 77://MFHI(def)		= 77 (1);
		setPerformedMut(1);
		return IS_NEW_TREE("mfhi",   
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					NULL
					);
	case 78://IOR(def,def)		= 78 (1);
		setPerformedMut(1);
		return IS_NEW_TREE("or",   
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					IS_DUPLICATE_TREE(RIGHT_CHILD(p))
					);
	case 79://IAND(def,def)		= 79 (1);
		setPerformedMut(1);
		return IS_NEW_TREE("and",   
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					IS_DUPLICATE_TREE(RIGHT_CHILD(p))
					);
	case 80://IADD(const_label,def)		= 80 (2);
		setPerformedMut(1);
		return IS_NEW_TREE("la2",
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					IS_DUPLICATE_TREE(RIGHT_CHILD(p))
					);
	case 81://ISUB(const_label,def)		= 81 (2);
		setPerformedMut(1);
		return IS_NEW_TREE("la2",
					IS_DUPLICATE_TREE(LEFT_CHILD(p)),
					IS_DUPLICATE_TREE(RIGHT_CHILD(p))
					);
	case 83://DNEG(def)		= 83 (2);
		setPerformedMut(1);
		return IS_NEW_TREE("neg_d",  
			IS_DUPLICATE_TREE(LEFT_CHILD(p)),
			NULL
			);
	case 84://FNEG(def)		= 84 (2);
		setPerformedMut(1);
		return IS_NEW_TREE("neg_s",  
			IS_DUPLICATE_TREE(LEFT_CHILD(p)),
			NULL
			);
	case 85://FGE(def,def)		= 85 (1);
		setPerformedMut(1);
		return IS_NEW_TREE("c_ge_s",   
			IS_DUPLICATE_TREE(LEFT_CHILD(p)),
			IS_DUPLICATE_TREE(RIGHT_CHILD(p))
			);
	case 86://FLT(def,def)		= 86 (1);
		setPerformedMut(1);
		return IS_NEW_TREE("c_lt_s",    
			IS_DUPLICATE_TREE(LEFT_CHILD(p)),
			IS_DUPLICATE_TREE(RIGHT_CHILD(p))
			);
	case 87://FLE(def,def)		= 87 (1);
		setPerformedMut(1);
		return IS_NEW_TREE("c_le_s",   
			IS_DUPLICATE_TREE(LEFT_CHILD(p)),
			IS_DUPLICATE_TREE(RIGHT_CHILD(p))
			);
	case 88://FGT(def,def)		= 88 (1);
		setPerformedMut(1);
		return IS_NEW_TREE("c_gt_s",   
			IS_DUPLICATE_TREE(LEFT_CHILD(p)),
			IS_DUPLICATE_TREE(RIGHT_CHILD(p))
			);
	case 89://FEQ(def,def)		= 89 (1);
		setPerformedMut(1);
		return IS_NEW_TREE("c_eq_s",   
			IS_DUPLICATE_TREE(LEFT_CHILD(p)),
			IS_DUPLICATE_TREE(RIGHT_CHILD(p))
			);
	case 82:// NOP()
	default:
		break;
	}
	return IS_DUPLICATE_TREE(p);
}
