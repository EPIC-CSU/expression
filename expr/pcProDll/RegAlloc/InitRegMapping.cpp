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
//: InitRegMapping.cpp
//
// File:  		InitRegMapping.cpp
// Author:		Radu Cornea
// Email:		radu@ics.uci.edu, ilp@ics.uci.edu
//
// Creates the register mapping
// 


#include "stdafx.h"
#include "RegisterMapping.h"


//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

static int debugRA = 0;
extern int doThumb;

RegisterMapping *globalRegisterMapping;

///// THE NEXT FUNCTION SHOULD BE REPLACED, IT IS JUST FOR TESTING!!!
enum Regtype {
	R0 = 0,	R1, R2, R3, R4, R5, R6, R7, R8, R9, R10, R11, R12, R13, R14, R15,
	R16, R17, R18, R19, R20, R21, R22, R23, R24, R25, R26, R27, R28, R29, R30, R31,
	CC0,
	SP0,
	F0, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15,
	F16, F17, F18, F19, F20, F21, F22, F23, F24, F25, F26, F27, F28, F29, F30, F31,
	FP0,
	TEMP0, TEMP1, TEMP2, TEMP3, TEMP4, TEMP5, TEMP6, TEMP7,
	TEMP8, TEMP9, TEMP10, TEMP11, TEMP12, TEMP13, TEMP14, TEMP15,
	TEMP16, TEMP17, TEMP18, TEMP19, TEMP20, TEMP21, TEMP22, TEMP23,
	TEMP24, TEMP25, TEMP26, TEMP27, TEMP28, TEMP29, TEMP30, TEMP31,
	CCTEMP0, HILOTEMP0, HILOTEMP1, SPTEMP0, FPTEMP0, 
	HILO0, HILO1,
	RISA0, RISA1, RISA2, RISA3, RISA4, RISA5, RISA6, RISA7,
	RISA8, RISA9, RISA10, RISA11, RISA12, RISA13, RISA14, RISA15,
	RISA16, RISA17, RISA18, RISA19, RISA20, RISA21, RISA22, RISA23,
	RISA24, RISA25, RISA26, RISA27, RISA28, RISA29, RISA30, RISA31,
	CCRISA0, HILORISA0, HILORISA1, SPRISA0, FPRISA0,
	_REGS_NO_
};

// This array contains a preffered order for coloring the registers, in terms of register indexes 
// (like: 0 for the first register in previous enum, 1 for next, ...)
// If all numbers from 0 to _REGS_NO_ are used, register names can be used instead of indexes
// ! Always keep FP register pairs in order, e.g. F0, F1, not F0, F5 or F2, F1
int regAllocPrefferedOrder[_REGS_NO_] = {
	R1, R2, R3,																		// general available
	R28, R29, R30, R31,
	R8, R9, R10, R11, R12, R13, R14, R15, R16, R17, R18, R19,
	R7, R6, R5, R4,
	R20, R21, R22, R23, R24, R25, R26, R27,
/*
	R16, R17, R18, R19, R28, R29, R30, R31, R20, R21, R22, R23, R24, R25, R26, R27, R28, R29, R30, R31,	// general available
	R15, R14, R13, R12, R11, R10, R9, R8, R7, R6, R5, R4,							// used as call parameters / return value
*/
	R0,																				// hardware 0
	CC0,																			// condition code
	SP0,																			// stack pointer
	F2, F3, F4, F5, F6, F7, F8, F9, F10, F11,									// general available
	F16, F17, F18, F19, F20, F21, F22, F23, F24, F25, F26, F27, F28, F29, F30, F31, // general available
	F14, F15, F12, F13,																// used as call parameters
	F0,	F1, 																		// used as return value
	FP0,																			// frame pointer
	TEMP0, TEMP1, TEMP2, TEMP3, TEMP4, TEMP5, TEMP6, TEMP7,							// general available
	TEMP8, TEMP9, TEMP10, TEMP11, TEMP12, TEMP13, TEMP14, TEMP15,					// general available
	TEMP16, TEMP17, TEMP18, TEMP19, TEMP20, TEMP21, TEMP22, TEMP23,					// general available
	TEMP24, TEMP25, TEMP26, TEMP27, TEMP28, TEMP29, TEMP30, TEMP31,					// general available
	CCTEMP0, HILOTEMP0, HILOTEMP1, SPTEMP0, FPTEMP0,
	HILO0, HILO1,   // hilo register
	RISA0, RISA1, RISA2, RISA3, RISA4, RISA5, RISA6, RISA7,							// risa available
	RISA8, RISA9, RISA10, RISA11, RISA12, RISA13, RISA14, RISA15,					// risa available
	RISA16, RISA17, RISA18, RISA19, RISA20, RISA21, RISA22, RISA23,					// risa available
	RISA24, RISA25, RISA26, RISA27, RISA28, RISA29, RISA30, RISA31,
	CCRISA0, HILORISA0, HILORISA1, SPRISA0, FPRISA0
};

void initRegisterMapping() {
	int i;
	int reg;

	globalRegisterMapping = new RegisterMapping();

	//// general register mappings
	// R0 -> R0
	RegisterList *r0_RegList = new RegisterList();
	reg = R0; r0_RegList->add(reg);
	globalRegisterMapping->setGeneralTargets(R0, r0_RegList);
	delete r0_RegList;

	// R1-31 -> R1-31
	RegisterList *r1_31_RegList = new RegisterList();
	for (i = R1; i <= R31; i++)
		r1_31_RegList->add(i);
	RegisterList *r1_31r_RegList = new RegisterList();
	for (i = R1; i <= R31; i++)
		r1_31r_RegList->add(i);
	globalRegisterMapping->setGeneralTargets(r1_31_RegList, r1_31r_RegList);
	delete r1_31_RegList;
	delete r1_31r_RegList;


	// SP0 -> SP0
	RegisterList *sp0_RegList = new RegisterList();

	for (i = R1; i <= R31; i++)
		sp0_RegList->add(i);

	reg = SP0; sp0_RegList->add(reg);
	globalRegisterMapping->setGeneralTargets(SP0, sp0_RegList);
	delete sp0_RegList;

	// FP0 -> FP0
	RegisterList *fp0_RegList = new RegisterList();
	for (i = R1; i <= R31; i++)
		fp0_RegList->add(i);

	reg = FP0; fp0_RegList->add(reg);
	globalRegisterMapping->setGeneralTargets(FP0, fp0_RegList);
	delete fp0_RegList;

			// SPTEMP0 -> SP0
	sp0_RegList = new RegisterList();
	reg = SP0; sp0_RegList->add(reg);
	globalRegisterMapping->setGeneralTargets(SPTEMP0, sp0_RegList);
	delete sp0_RegList;	

				// FPTEMP0 -> FP0
	fp0_RegList = new RegisterList();
	reg = FP0; fp0_RegList->add(reg);
	globalRegisterMapping->setGeneralTargets(FPTEMP0, fp0_RegList);
	delete fp0_RegList;	

		// HILOTEMP0 -> HILO0
	RegisterList *hilo0_RegList = new RegisterList();
	reg = HILO0; hilo0_RegList->add(reg);
	globalRegisterMapping->setGeneralTargets(HILOTEMP0, hilo0_RegList);
	delete hilo0_RegList;

	// HILOTEMP1 -> HILO1
	RegisterList *hilo1_RegList = new RegisterList();
	reg = HILO1; hilo1_RegList->add(reg);
	globalRegisterMapping->setGeneralTargets(HILOTEMP1, hilo1_RegList);
	delete hilo1_RegList;


	// CC0 -> R1-31, //CC0
	RegisterList *r1_31_cc0_RegList = new RegisterList();
	//for (i = R1; i <= CC0; i++)
	for (i = R1; i <= R31; i++)
		r1_31_cc0_RegList->add(i);
	globalRegisterMapping->setGeneralTargets(CC0, r1_31_cc0_RegList);
	delete r1_31_cc0_RegList;
/*
	if (doThumb)
	{
		// Modified by AVS and PB on 05/25/01
		//
		// Registers R10 to R17 have been identified as the registers
		// in the thumb set and all the temp's are to be mapped to R10-R17
		
		// temp0-31 -> R10-R17 
		RegisterList *temp0_31_RegList = new RegisterList();
		RegisterList *r10_17_RegList = new RegisterList();
		for (i = TEMP0; i <= TEMP31; i++)
			temp0_31_RegList->add(i);
		for (i = R10; i <= R17; i++)
			r10_17_RegList->add(i);
		globalRegisterMapping->setGeneralTargets(temp0_31_RegList, r10_17_RegList);
		globalRegisterMapping->setTargetsConditionCode(temp0_31_RegList, r10_17_RegList);
		delete temp0_31_RegList;
		delete r10_17_RegList;
	}
*/

		if (doThumb)
		{
		// Modified by AVS and PB and SAH on 05/16/01

			
			// SPRISA0 -> SP0
			RegisterList *sp0_RegList = new RegisterList();
			reg = SP0; sp0_RegList->add(reg);
			globalRegisterMapping->setGeneralTargets(SPRISA0, sp0_RegList);
			delete sp0_RegList;	
			
			// FPRISA0 -> FP0
			RegisterList *fp0_RegList = new RegisterList();
			reg = FP0; fp0_RegList->add(reg);
			globalRegisterMapping->setGeneralTargets(FPRISA0, fp0_RegList);
			delete fp0_RegList;	
			
			// HILORISA0 -> HILO0
			RegisterList *hilo0_RegList = new RegisterList();
			reg = HILO0; hilo0_RegList->add(reg);
			globalRegisterMapping->setGeneralTargets(HILORISA0, hilo0_RegList);
			delete hilo0_RegList;
			
			// HILORISA1 -> HILO1
			RegisterList *hilo1_RegList = new RegisterList();
			reg = HILO1; hilo1_RegList->add(reg);
			globalRegisterMapping->setGeneralTargets(HILORISA0, hilo1_RegList);
			delete hilo1_RegList;
			

			//
			// Registers R10 to R17 have been identified as the registers in the thumb
			// set and all the temp's are to be mapped to R10-17
			
			// RISA0-31 -> R10-17
			//
			RegisterList *risa0_31_RegList = new RegisterList();
			RegisterList *r10_17_RegList = new RegisterList();
			for (i = RISA0; i <= RISA31; i++)
				risa0_31_RegList->add(i);
			for (i = R10; i <= R17; i++)
				r10_17_RegList->add(i);
			globalRegisterMapping->setGeneralTargets(risa0_31_RegList, r10_17_RegList);
			delete risa0_31_RegList;
			delete r10_17_RegList;
			

		}

	// temp0-31 -> R1-31 
	RegisterList *temp0_31_RegList = new RegisterList();
	r1_31_RegList = new RegisterList();
	for (i = TEMP0; i <= TEMP31; i++)
		temp0_31_RegList->add(i);
	for (i = R1; i <= R31; i++)
		r1_31_RegList->add(i);
	globalRegisterMapping->setGeneralTargets(temp0_31_RegList, r1_31_RegList);
	delete temp0_31_RegList;
	delete r1_31_RegList;

	
	// F0-31 -> F0-31
	RegisterList *f0_31_RegList = new RegisterList();
	for (i = F0; i <= F31; i++)
		f0_31_RegList->add(i);
	globalRegisterMapping->setGeneralTargets(f0_31_RegList, f0_31_RegList);
	delete f0_31_RegList;

	// Modified by SAH on 06/01/01 to map HILO to any general purpose reg.
	//

	// HILO0 -> HILO0
	hilo0_RegList = new RegisterList();
	reg = HILO0; hilo0_RegList->add(reg);
	globalRegisterMapping->setGeneralTargets(HILO0, hilo0_RegList);
	delete hilo0_RegList;

	// HILO1 -> HILO1
	hilo1_RegList = new RegisterList();
	reg = HILO1; hilo1_RegList->add(reg);
	globalRegisterMapping->setGeneralTargets(HILO1, hilo1_RegList);
	delete hilo1_RegList;


	//// call ret register mappings
	// R4 -> R4
	RegisterList *r4_RegList = new RegisterList();
	reg = R4; r4_RegList->add(reg);
	globalRegisterMapping->setTargetsCallReturn(R4, r4_RegList);
	delete r4_RegList;

	if (doThumb)
	{
		//// call ret register mappings
		// temp4 -> R4
		RegisterList *tr4_RegList = new RegisterList();
		reg = R4; tr4_RegList->add(reg);
//		reg = R6; tr4_RegList->add(reg);
		globalRegisterMapping->setTargetsCallReturn(TEMP4, tr4_RegList);
		delete tr4_RegList;

		//// call ret register mappings
		// risa4 -> R4
		RegisterList *rr4_RegList = new RegisterList();
		reg = R4; rr4_RegList->add(reg);
//		reg = R6; rr4_RegList->add(reg);
		globalRegisterMapping->setTargetsCallReturn(RISA4, rr4_RegList);
		delete rr4_RegList;
	}


	// F0 -> F0
	RegisterList *f0_RegList = new RegisterList();
	reg = F0; f0_RegList->add(reg);
	globalRegisterMapping->setTargetsCallReturn(F0, f0_RegList);
	delete f0_RegList;
	RegisterList *f1_RegList = new RegisterList();
	reg = F1; f1_RegList->add(reg);
	globalRegisterMapping->setTargetsCallReturn(F1, f1_RegList);
	delete f1_RegList;


	//// call param register mappings
	// R4 -> R4
	RegisterList *r41_RegList = new RegisterList();
	reg = R4; r41_RegList->add(reg);
	globalRegisterMapping->setTargetsCallParam(R4, r41_RegList);
	delete r41_RegList;

	// R5 -> R5
	RegisterList *r5_RegList = new RegisterList();
	reg = R5; r5_RegList->add(reg);
	globalRegisterMapping->setTargetsCallParam(R5, r5_RegList);
	delete r5_RegList;

	// R6 -> R6
	RegisterList *r6_RegList = new RegisterList();
	reg = R6; r6_RegList->add(reg);
	globalRegisterMapping->setTargetsCallParam(R6, r6_RegList);
	delete r6_RegList;

	// R7 -> R7
	RegisterList *r7_RegList = new RegisterList();
	reg = R7; r7_RegList->add(reg);
	globalRegisterMapping->setTargetsCallParam(R7, r7_RegList);
	delete r7_RegList;

	// R8 -> R8
	RegisterList *r8_RegList = new RegisterList();
	reg = R8; r8_RegList->add(reg);
	globalRegisterMapping->setTargetsCallParam(R8, r8_RegList);
	delete r8_RegList;

	// R9 -> R9
	RegisterList *r9_RegList = new RegisterList();
	reg = R9; r9_RegList->add(reg);
	globalRegisterMapping->setTargetsCallParam(R9, r9_RegList);
	delete r9_RegList;

	// R10 -> R10
	RegisterList *r10_RegList = new RegisterList();
	reg = R10; r10_RegList->add(reg);
	globalRegisterMapping->setTargetsCallParam(R10, r10_RegList);
	delete r10_RegList;

	// R11 -> R11
	RegisterList *r11_RegList = new RegisterList();
	reg = R11; r11_RegList->add(reg);
	globalRegisterMapping->setTargetsCallParam(R11, r11_RegList);
	delete r11_RegList;
	
	// R12 -> R12
	RegisterList *r12_RegList = new RegisterList();
	reg = R12; r12_RegList->add(reg);
	globalRegisterMapping->setTargetsCallParam(R12, r12_RegList);
	delete r12_RegList;

	// R13 -> R13
	RegisterList *r13_RegList = new RegisterList();
	reg = R13; r13_RegList->add(reg);
	globalRegisterMapping->setTargetsCallParam(R13, r13_RegList);
	delete r13_RegList;

	// R14 -> R14
	RegisterList *r14_RegList = new RegisterList();
	reg = R14; r14_RegList->add(reg);
	globalRegisterMapping->setTargetsCallParam(R14, r14_RegList);
	delete r14_RegList;

	// R15 -> R15
	RegisterList *r15_RegList = new RegisterList();
	reg = R15; r15_RegList->add(reg);
	globalRegisterMapping->setTargetsCallParam(R15, r15_RegList);
	delete r15_RegList;


	if (doThumb)
	{
		//// call param register mappings
		// R4 -> R4
		RegisterList *tr41_RegList = new RegisterList();
		reg = R4; tr41_RegList->add(reg);
		globalRegisterMapping->setTargetsCallParam(TEMP4, tr41_RegList);
		delete tr41_RegList;

		// R5 -> R5
		RegisterList *tr5_RegList = new RegisterList();
		reg = R5; tr5_RegList->add(reg);
		globalRegisterMapping->setTargetsCallParam(TEMP5, tr5_RegList);
		delete tr5_RegList;

		// R6 -> R6
		RegisterList *tr6_RegList = new RegisterList();
		reg = R6; tr6_RegList->add(reg);
		globalRegisterMapping->setTargetsCallParam(TEMP6, tr6_RegList);
		delete tr6_RegList;

		// R7 -> R7
		RegisterList *tr7_RegList = new RegisterList();
		reg = R7; tr7_RegList->add(reg);
		globalRegisterMapping->setTargetsCallParam(TEMP7, tr7_RegList);
		delete tr7_RegList;

		// R8 -> R8
		RegisterList *tr8_RegList = new RegisterList();
		reg = R8; tr8_RegList->add(reg);
		globalRegisterMapping->setTargetsCallParam(TEMP8, tr8_RegList);
		delete tr8_RegList;

		// R9 -> R9
		RegisterList *tr9_RegList = new RegisterList();
		reg = R9; tr9_RegList->add(reg);
		globalRegisterMapping->setTargetsCallParam(TEMP9, tr9_RegList);
		delete tr9_RegList;

		// R10 -> R10
		RegisterList *tr10_RegList = new RegisterList();
		reg = R10; tr10_RegList->add(reg);
		globalRegisterMapping->setTargetsCallParam(TEMP10, tr10_RegList);
		delete tr10_RegList;

		// R11 -> R11
		RegisterList *tr11_RegList = new RegisterList();
		reg = R11; tr11_RegList->add(reg);
		globalRegisterMapping->setTargetsCallParam(TEMP11, tr11_RegList);
		delete tr11_RegList;

		/* NOTE: TEMP12 - TEMP15 have not been mapped to R12 - R15 */

		//// call param register mappings
		// R4 -> R4
		RegisterList *rr41_RegList = new RegisterList();
		reg = R4; rr41_RegList->add(reg);
		globalRegisterMapping->setTargetsCallParam(RISA4, rr41_RegList);
		delete rr41_RegList;

		// R5 -> R5
		RegisterList *rr5_RegList = new RegisterList();
		reg = R5; rr5_RegList->add(reg);
		globalRegisterMapping->setTargetsCallParam(RISA5, rr5_RegList);
		delete rr5_RegList;

		// R6 -> R6
		RegisterList *rr6_RegList = new RegisterList();
		reg = R6; rr6_RegList->add(reg);
		globalRegisterMapping->setTargetsCallParam(RISA6, rr6_RegList);
		delete rr6_RegList;

		// R7 -> R7
		RegisterList *rr7_RegList = new RegisterList();
		reg = R7; rr7_RegList->add(reg);
		globalRegisterMapping->setTargetsCallParam(RISA7, rr7_RegList);
		delete rr7_RegList;

		// R8 -> R8
		RegisterList *rr8_RegList = new RegisterList();
		reg = R8; rr8_RegList->add(reg);
		globalRegisterMapping->setTargetsCallParam(RISA8, rr8_RegList);
		delete rr8_RegList;

		/* NOTE: RISA9 - RISA15 have not been mapped to R9 - R15 */

	}

	// F12 -> F12
	RegisterList *f12_RegList = new RegisterList();
	reg = F12; f12_RegList->add(reg);
	globalRegisterMapping->setTargetsCallParam(F12, f12_RegList);
	delete f12_RegList;

	// F13 -> F13
	RegisterList *f13_RegList = new RegisterList();
	reg = F13; f13_RegList->add(reg);
	globalRegisterMapping->setTargetsCallParam(F13, f13_RegList);
	delete f13_RegList;

	// F14 -> F14
	RegisterList *f14_RegList = new RegisterList();
	reg = F14; f14_RegList->add(reg);
	globalRegisterMapping->setTargetsCallParam(F14, f14_RegList);
	delete f14_RegList;

	// F15 -> F15
	RegisterList *f15_RegList = new RegisterList();
	reg = F15; f15_RegList->add(reg);
	globalRegisterMapping->setTargetsCallParam(F15, f15_RegList);
	delete f15_RegList;


	//// condition code register mappings
	// CC0 -> CC0
	RegisterList *cc0_RegList = new RegisterList();
	reg = CC0; cc0_RegList->add(reg);
	globalRegisterMapping->setTargetsConditionCode(CC0, cc0_RegList);
	delete cc0_RegList;	

		//// condition code register mappings
	// CCRISA0 -> CC0
	RegisterList *ccrisa0_RegList = new RegisterList();
	reg = CC0; ccrisa0_RegList->add(reg);
	globalRegisterMapping->setTargetsConditionCode(CCRISA0, ccrisa0_RegList);
	delete ccrisa0_RegList;	


	
		// CCTEMP0 -> CC0
	cc0_RegList = new RegisterList();
	reg = CC0; cc0_RegList->add(reg);
	globalRegisterMapping->setTargetsConditionCode(CCTEMP0, cc0_RegList);
	delete cc0_RegList;	
	


	//// double float register mapping
	// F0, F2, ... , F30 -> F0, F2, ... , F30
	RegisterList *f0_30_RegList = new RegisterList();
	for (i = F0; i <= F30; i+=2)
		f0_30_RegList->add(i);
	globalRegisterMapping->setTargetsDoubleFloat(f0_30_RegList, f0_30_RegList);
	delete f0_30_RegList;

	// F1, F3, ... , F31 -> F1, F3, ... , F31
	RegisterList *f1_31_RegList = new RegisterList();
	for (i = F1; i <= F31; i+=2)
		f1_31_RegList->add(i);
	globalRegisterMapping->setTargetsDoubleFloat(f1_31_RegList, f1_31_RegList);
	delete f1_31_RegList;



	if (debugRA)
		globalRegisterMapping->print();
}
///// END TESTING
