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
 *file  : Unit.cc                                            *
 *created : 11th nov,97.                                     *
 *authors : Wei Yu Tang,vgee                                 *
 *last modified :                                            *
 *                                                           *
 *contents:contains class methods for Unit class & other     *
 *         classes derived from it.                          *
 *compiled with : g++                                        *
 *for more info :                                            *
 *************************************************************/

#include "DerivedOpExecSemantics.h"

DoNOP nopFunc;

DoICONSTANT iconstantFunc;
DoDCONSTANT dconstantFunc;
DoFCONSTANT fconstantFunc;
DoASSIGN assignFunc;
DoIASSIGN iassignFunc;
DoFASSIGN fassignFunc;
DoDASSIGN dassignFunc;

DoIADD iaddFunc;
DoDADD daddFunc;
DoFADD faddFunc;
DoISUB isubFunc;
DoDSUB dsubFunc;
DoFSUB fsubFunc;
DoIMUL imulFunc;
DoMAC macFunc;
DoIMULU imuluFunc;
DoDMUL dmulFunc;
DoFMUL fmulFunc;
DoIDIV idivFunc;
DoIDIVU idivuFunc;
DoDDIV ddivFunc;
DoFDIV fdivFunc;
DoIREM iremFunc;

DoIEQ ieqFunc;
DoDEQ deqFunc;
DoFEQ feqFunc;
DoINE ineFunc;
DoDNE dneFunc;
DoFNE fneFunc;
DoIGE igeFunc;
DoIGE igeuFunc;
DoDGE dgeFunc;
DoFGE fgeFunc;
DoILE ileFunc;
DoILE ileuFunc;
DoDLE dleFunc;
DoFLE fleFunc;
DoILT iltFunc;
DoILT iltuFunc;
DoDLT dltFunc;
DoFLT fltFunc;
DoIGT igtFunc;
DoIGT igtuFunc;
DoDGT dgtFunc;
DoFGT fgtFunc;

DoILSH ilshFunc;
DoIASH iashFunc;
DoILAND ilandFunc;
DoILOR ilorFunc;

DoIAND iandFunc;
DoIOR iorFunc;
DoINOR inorFunc;
DoIXOR ixorFunc;

DoIVLOAD ivloadFunc;
DoDVLOAD dvloadFunc;
DoFVLOAD fvloadFunc;
DoHIVLOAD hivloadFunc;
DoHIVLOAD hivloaduFunc;
DoQIVLOAD qivloadFunc;
DoQIVLOADU qivloaduFunc;
DoIVSTORE ivstoreFunc;
DoDVSTORE dvstoreFunc;
DoFVSTORE fvstoreFunc;
DoHIVSTORE hivstoreFunc;
DoHIVSTORE hivstoreuFunc;
DoQIVSTORE qivstoreFunc;
DoQIVSTORE qivstoreuFunc;

DoIF ifFunc;
DoIF ifftFunc;
DoGOTO gotoFunc;
DoIGOTO igotoFunc;
DoCALL callFunc;
DoRETURN returnFunc;

// DoDUMP dumpFunc;

// Radu - May 21, 2000
//DoPRINTF printfFunc;
DoSQRT sqrtFunc;
DoEXP expFunc;

DoCVTDI cvtdiFunc;
DoCVTSI cvtsiFunc;
DoCVTSD cvtsdFunc;
DoCVTDS cvtdsFunc;

DoTRUNCID truncidFunc;
DoTRUNCIS truncisFunc;

DoMTC mtc1Func;
DoDMTC dmtc1Func;
DoMFC mfc1Func;
DoDMFC dmfc1Func;

DoFNEG fnegFunc;
DoDNEG dnegFunc;

DoIABS iabsFunc;
DoDABS dabsFunc;

DoILLSH illshFunc;
DoIRLSH irlshFunc;
DoILASH ilashFunc;
DoIRASH irashFunc;

DoIMUL_MIPS imulMIPSFunc;
DoIDIV_MIPS idivMIPSFunc;
DoMFHI mfhiFunc;
DoMFLO mfloFunc;

DoASSERT assertFunc;