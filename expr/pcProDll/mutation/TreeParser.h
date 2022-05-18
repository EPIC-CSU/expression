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

//Increment this number every time you add a new terminal
#define NUM_TERMS 118
//ATTENTION!!!!!!!!!!!!!!!!!!!!!!!!!!!Don't forget to increment this number

enum {BRG_IADD=1, IGE=2, IGT=3, ILE=4, IASH=5, ISUB=6, ILSH=7, ILT=8, NOP=9, ICONSTANT=10, IASSIGN=11, IF=12, 
	IGOTO=13, IVLOAD=14, DVLOAD=15, IVSTORE=16, DVSTORE=17, ASSIGN=18, DCONSTANT=19, DADD=20, FASSIGN=21, DMUL=22, 
	DSUB=23, DMTC1=24, GOTO=25, TRUNCID=26, MFC1=27, MTC1=28, CVTDI=29, ILAND=30, DASSIGN=31, 
	DGE=32, DLT=33, DLE=34, DGT=35, CVTSD=36, CVTDS=37, DMFC1=38, DDIV=39, DEQ=40, 
	FADD=41, FSUB=42, FMUL=43, FDIV=44,
	R0=101, R1=102, R2=103, R3=104, R4=105, R5=106, R6=107, R7=108, R8=109, R9=110, R10=111, R11=112, R12=113, 
	R13=114, R14=115, R15=116, 
	R16=117, R17=118, R18=119, R19=120, R20=121, R21=122, R22=123, R23=124, R24=125, R25=126,
	SP0=127, CC0=128, F0=129, F1=130, F2=131, F3=132, F4=133, F5=134, F6=135, F7=136, F8=137,
	F9=138, F10=139, F11=140, F12=141, F13=142, F14=143, F15=144, 
	F16=145, F17=146, F18=147, F19=148, F20=149, F21=150, F22=151, F23=152, F24=153,
	F25=154, F26=155, F27=156, F28=157, F29=158, F30=159, F31=160, 
	FP0=161, 
	temp0=162, temp1=163, temp2=164, temp3=165, temp4=166, temp5=167, temp6=168, temp7=169, 
	temp8=170, temp9=171, temp10=172,
	DEF=201, BRG_INT=202, BRG_IND=203, SRC2=204,
	NONE=400, ALU1=401, ALU2=402};


const TERM_MAP_TYPE term_map[]={
	{BRG_IADD,"IADD"}, 
	{IGE,"IGE"}, 
	{IGT,"IGT"}, 
	{ILE,"ILE"}, 
	{IASH,"IASH"}, 
	{ISUB,"ISUB"}, 
	{ILSH,"ILSH"}, 
	{ILT,"ILT"}, 
	{NOP,"NOP"}, 
	{ICONSTANT,"ICONSTANT"}, 
	{IASSIGN,"IASSIGN"}, 
	{IF,"IF"}, 
	{IGOTO,"IGOTO"}, 
	{IVLOAD,"IVLOAD"}, 
	{DVLOAD,"DVLOAD"}, 
	{IVSTORE,"IVSTORE"}, 
	{DVSTORE,"DVSTORE"}, 
	{ASSIGN,"ASSIGN"}, 
	{DCONSTANT,"DCONSTANT"}, 
	{DADD,"DADD"}, 
	{FASSIGN,"FASSIGN"}, 
	{DMUL,"DMUL"}, 
	{DSUB,"DSUB"}, 
	{DMTC1,"DMTC1"}, 
	{GOTO,"GOTO"}, 
	{TRUNCID,"TRUNCID"},
	{MFC1,"MFC1"},
	{MTC1,"MTC1"},
	{CVTDI,"CVTDI"},
	{ILAND,"ILAND"},
	{DASSIGN,"DASSIGN"},
	{DGE,"DGE"},
	{DLT,"DLT"},
	{DLE,"DLE"},
	{DGT,"DGT"},
	{CVTSD,"CVTSD"},
	{CVTDS,"CVTDS"},
	{DMFC1,"DMFC1"},
	{DDIV,"DDIV"},
	{DEQ,"DEQ"},
	{FADD,"FADD"},
	{FSUB,"FSUB"},
	{FMUL,"FMUL"},
	{FDIV,"FDIV"},
	{R0,"R0"}, 
	{R1,"R1"}, 
	{R2,"R2"}, 
	{R3,"R3"}, 
	{R4,"R4"}, 
	{R5,"R5"}, 
	{R6,"R6"}, 
	{R7,"R7"}, 
	{R8,"R8"}, 
	{R9,"R9"}, 
	{R10,"R10"}, 
	{R11,"R11"}, 
	{R12,"R12"}, 
	{R13,"R13"}, 
	{R14,"R14"}, 
	{R15,"R15"}, 
	{R16,"R16"},
	{R17,"R17"},
	{R18,"R18"},
	{R19,"R19"},
	{R20,"R20"},
	{R21,"R21"},
	{R22,"R22"},
	{R23,"R23"},
	{R24,"R24"},
	{R25,"R25"},
	{SP0,"sp0"},
	{CC0,"cc0"},
	{F0,"f0"},
	{F1,"f1"},
	{F2,"f2"},
	{F3,"f3"},
	{F4,"f4"},
	{F5,"f5"},
	{F6,"f6"},
	{F7,"f7"},
	{F8,"f8"},
	{F9,"f9"},
	{F10,"f10"},
	{F11,"f11"},
	{F12,"f12"},
	{F13,"f13"},
	{F14,"f14"},
	{F15,"f15"},
	{F16,"f16"},
	{F17,"f17"},
	{F18,"f18"},
	{F19,"f19"},
	{F20,"f20"},
	{F21,"f21"},
	{F22,"f22"},
	{F23,"f23"},
	{F24,"f24"},
	{F25,"f25"},
	{F26,"f26"},
	{F27,"f27"},
	{F28,"f28"},
	{F29,"f29"},
	{F30,"f30"},
	{F31,"f31"},
	{FP0,"fp0"},
	{temp0,"temp0"},
	{temp1,"temp1"},
	{temp2,"temp2"},
	{temp3,"temp3"},
	{temp4,"temp4"},
	{temp5,"temp5"},
	{temp6,"temp6"},
	{temp7,"temp7"},
	{temp8,"temp8"},
	{temp9,"temp9"},
	{temp10,"temp10"},
	{DEF,"DEF"},
	{BRG_INT,"BRG_INT"},
	{BRG_IND,"IND"},
	{SRC2,"SRC2"},
	{NONE,"NONE"},
	{ALU1,"ALU1"},
	{ALU2,"ALU2"}
};




