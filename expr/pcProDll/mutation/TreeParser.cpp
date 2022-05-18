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

#include <assert.h>

#ifndef WIN32
#include <stdio.h>
#include <stdlib.h>
#endif

#define MY_TRACE

#include "TreeBuilding.h"


//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 


static void burm_trace(NODEPTR_TYPE p, int eruleno, int cost, int bestcost) {
#ifdef MY_TRACE
	extern char *burm_string[];

	fprintf(stderr, "0x%p matched %s with cost %d vs. %d\n", p,
		burm_string[eruleno], cost, bestcost);
#endif
}
#include <limits.h>
#include <stdlib.h>
#ifndef STATE_TYPE
#define STATE_TYPE int
#endif
#ifndef ALLOC
#define ALLOC(n) malloc(n)
#endif
#ifndef burm_assert
#define burm_assert(x,y) if (!(x)) { y; abort(); }
#endif

#define burm_def_NT 1
#define burm_reg_NT 2
#define burm_expr_NT 3
#define burm_const_NT 4
#define burm_rf1_NT 5
#define burm_rf2_NT 6
#define burm_rf3_NT 7
int burm_max_nt = 7;

char *burm_ntname[] = {
	0,
	"def",
	"reg",
	"expr",
	"const",
	"rf1",
	"rf2",
	"rf3",
	0
};

struct burm_state {
	int op;
	struct burm_state *left, *right;
	short cost[8];
	struct {
		unsigned burm_def:5;
		unsigned burm_reg:2;
		unsigned burm_expr:6;
		unsigned burm_const:2;
		unsigned burm_rf1:4;
		unsigned burm_rf2:4;
		unsigned burm_rf3:6;
	} rule;
};

static short burm_nts_0[] = { burm_reg_NT, burm_expr_NT, 0 };
static short burm_nts_1[] = { burm_reg_NT, 0 };
static short burm_nts_2[] = { burm_const_NT, 0 };
static short burm_nts_3[] = { burm_def_NT, burm_def_NT, 0 };
static short burm_nts_4[] = { burm_def_NT, 0 };
static short burm_nts_5[] = { burm_def_NT, burm_const_NT, 0 };
static short burm_nts_6[] = { burm_rf1_NT, 0 };
static short burm_nts_7[] = { burm_rf2_NT, 0 };
static short burm_nts_8[] = { 0 };
static short burm_nts_9[] = { burm_rf3_NT, 0 };
static short burm_nts_10[] = { burm_reg_NT, burm_def_NT, burm_def_NT, burm_def_NT, 0 };
static short burm_nts_11[] = { burm_def_NT, burm_reg_NT, burm_def_NT, burm_def_NT, 0 };
static short burm_nts_12[] = { burm_reg_NT, burm_const_NT, 0 };
static short burm_nts_13[] = { burm_reg_NT, burm_def_NT, 0 };
static short burm_nts_14[] = { burm_reg_NT, burm_reg_NT, burm_reg_NT, burm_expr_NT, burm_def_NT, burm_def_NT, 0 };
static short burm_nts_15[] = { burm_reg_NT, burm_def_NT, burm_reg_NT, burm_def_NT, burm_reg_NT, burm_expr_NT, 0 };
static short burm_nts_16[] = { burm_reg_NT, burm_reg_NT, burm_def_NT, burm_reg_NT, burm_expr_NT, burm_def_NT, 0 };
static short burm_nts_17[] = { burm_reg_NT, burm_def_NT, burm_reg_NT, burm_reg_NT, burm_expr_NT, burm_def_NT, 0 };

short *burm_nts[] = {
	0,	/* 0 */
	burm_nts_0,	/* 1 */
	burm_nts_1,	/* 2 */
	burm_nts_2,	/* 3 */
	0,	/* 4 */
	burm_nts_3,	/* 5 */
	burm_nts_3,	/* 6 */
	burm_nts_3,	/* 7 */
	burm_nts_3,	/* 8 */
	burm_nts_4,	/* 9 */
	burm_nts_5,	/* 10 */
	burm_nts_5,	/* 11 */
	burm_nts_3,	/* 12 */
	burm_nts_3,	/* 13 */
	burm_nts_3,	/* 14 */
	burm_nts_2,	/* 15 */
	burm_nts_4,	/* 16 */
	burm_nts_3,	/* 17 */
	burm_nts_3,	/* 18 */
	burm_nts_3,	/* 19 */
	burm_nts_3,	/* 20 */
	burm_nts_4,	/* 21 */
	burm_nts_4,	/* 22 */
	burm_nts_4,	/* 23 */
	burm_nts_2,	/* 24 */
	burm_nts_4,	/* 25 */
	burm_nts_4,	/* 26 */
	burm_nts_3,	/* 27 */
	burm_nts_4,	/* 28 */
	burm_nts_3,	/* 29 */
	burm_nts_3,	/* 30 */
	burm_nts_3,	/* 31 */
	burm_nts_3,	/* 32 */
	burm_nts_4,	/* 33 */
	burm_nts_4,	/* 34 */
	burm_nts_4,	/* 35 */
	burm_nts_3,	/* 36 */
	burm_nts_3,	/* 37 */
	burm_nts_3,	/* 38 */
	burm_nts_3,	/* 39 */
	burm_nts_3,	/* 40 */
	burm_nts_3,	/* 41 */
	burm_nts_3,	/* 42 */
	burm_nts_3,	/* 43 */
	burm_nts_3,	/* 44 */
	burm_nts_3,	/* 45 */
	burm_nts_2,	/* 46 */
	burm_nts_4,	/* 47 */
	burm_nts_4,	/* 48 */
	burm_nts_3,	/* 49 */
	0,	/* 50 */
	0,	/* 51 */
	0,	/* 52 */
	0,	/* 53 */
	0,	/* 54 */
	0,	/* 55 */
	0,	/* 56 */
	0,	/* 57 */
	0,	/* 58 */
	0,	/* 59 */
	0,	/* 60 */
	0,	/* 61 */
	0,	/* 62 */
	0,	/* 63 */
	0,	/* 64 */
	0,	/* 65 */
	0,	/* 66 */
	0,	/* 67 */
	0,	/* 68 */
	0,	/* 69 */
	0,	/* 70 */
	0,	/* 71 */
	0,	/* 72 */
	0,	/* 73 */
	0,	/* 74 */
	0,	/* 75 */
	0,	/* 76 */
	0,	/* 77 */
	0,	/* 78 */
	0,	/* 79 */
	0,	/* 80 */
	0,	/* 81 */
	0,	/* 82 */
	0,	/* 83 */
	0,	/* 84 */
	0,	/* 85 */
	0,	/* 86 */
	0,	/* 87 */
	0,	/* 88 */
	0,	/* 89 */
	0,	/* 90 */
	0,	/* 91 */
	0,	/* 92 */
	0,	/* 93 */
	0,	/* 94 */
	0,	/* 95 */
	0,	/* 96 */
	0,	/* 97 */
	0,	/* 98 */
	0,	/* 99 */
	0,	/* 100 */
	0,	/* 101 */
	0,	/* 102 */
	0,	/* 103 */
	0,	/* 104 */
	0,	/* 105 */
	0,	/* 106 */
	0,	/* 107 */
	0,	/* 108 */
	0,	/* 109 */
	0,	/* 110 */
	0,	/* 111 */
	0,	/* 112 */
	0,	/* 113 */
	burm_nts_6,	/* 114 */
	burm_nts_7,	/* 115 */
	burm_nts_8,	/* 116 */
	burm_nts_8,	/* 117 */
	burm_nts_8,	/* 118 */
	burm_nts_8,	/* 119 */
	burm_nts_8,	/* 120 */
	burm_nts_8,	/* 121 */
	burm_nts_8,	/* 122 */
	burm_nts_8,	/* 123 */
	burm_nts_8,	/* 124 */
	burm_nts_8,	/* 125 */
	burm_nts_8,	/* 126 */
	burm_nts_8,	/* 127 */
	burm_nts_8,	/* 128 */
	burm_nts_8,	/* 129 */
	burm_nts_8,	/* 130 */
	burm_nts_8,	/* 131 */
	burm_nts_8,	/* 132 */
	burm_nts_8,	/* 133 */
	burm_nts_8,	/* 134 */
	burm_nts_8,	/* 135 */
	burm_nts_8,	/* 136 */
	burm_nts_8,	/* 137 */
	burm_nts_8,	/* 138 */
	burm_nts_8,	/* 139 */
	burm_nts_8,	/* 140 */
	burm_nts_8,	/* 141 */
	burm_nts_9,	/* 142 */
	burm_nts_8,	/* 143 */
	burm_nts_8,	/* 144 */
	burm_nts_8,	/* 145 */
	burm_nts_8,	/* 146 */
	burm_nts_8,	/* 147 */
	burm_nts_8,	/* 148 */
	burm_nts_8,	/* 149 */
	burm_nts_8,	/* 150 */
	burm_nts_8,	/* 151 */
	burm_nts_8,	/* 152 */
	burm_nts_8,	/* 153 */
	burm_nts_8,	/* 154 */
	burm_nts_8,	/* 155 */
	burm_nts_8,	/* 156 */
	burm_nts_8,	/* 157 */
	burm_nts_8,	/* 158 */
	burm_nts_8,	/* 159 */
	burm_nts_8,	/* 160 */
	burm_nts_8,	/* 161 */
	burm_nts_8,	/* 162 */
	burm_nts_8,	/* 163 */
	burm_nts_8,	/* 164 */
	burm_nts_8,	/* 165 */
	burm_nts_8,	/* 166 */
	burm_nts_8,	/* 167 */
	burm_nts_8,	/* 168 */
	burm_nts_8,	/* 169 */
	burm_nts_8,	/* 170 */
	burm_nts_8,	/* 171 */
	burm_nts_8,	/* 172 */
	burm_nts_8,	/* 173 */
	burm_nts_8,	/* 174 */
	burm_nts_8,	/* 175 */
	burm_nts_8,	/* 176 */
	burm_nts_8,	/* 177 */
	burm_nts_8,	/* 178 */
	burm_nts_8,	/* 179 */
	burm_nts_8,	/* 180 */
	burm_nts_8,	/* 181 */
	burm_nts_8,	/* 182 */
	burm_nts_8,	/* 183 */
	burm_nts_8,	/* 184 */
	burm_nts_8,	/* 185 */
	burm_nts_8,	/* 186 */
	burm_nts_8,	/* 187 */
	burm_nts_8,	/* 188 */
	burm_nts_8,	/* 189 */
	burm_nts_8,	/* 190 */
	burm_nts_8,	/* 191 */
	burm_nts_8,	/* 192 */
	0,	/* 193 */
	0,	/* 194 */
	0,	/* 195 */
	0,	/* 196 */
	0,	/* 197 */
	0,	/* 198 */
	0,	/* 199 */
	0,	/* 200 */
	burm_nts_10,	/* 201 */
	burm_nts_11,	/* 202 */
	burm_nts_2,	/* 203 */
	burm_nts_12,	/* 204 */
	burm_nts_13,	/* 205 */
	0,	/* 206 */
	burm_nts_8,	/* 207 */
	burm_nts_8,	/* 208 */
	burm_nts_8,	/* 209 */
	0,	/* 210 */
	burm_nts_13,	/* 211 */
	burm_nts_13,	/* 212 */
	burm_nts_13,	/* 213 */
	burm_nts_13,	/* 214 */
	0,	/* 215 */
	0,	/* 216 */
	0,	/* 217 */
	0,	/* 218 */
	0,	/* 219 */
	burm_nts_1,	/* 220 */
	burm_nts_1,	/* 221 */
	0,	/* 222 */
	burm_nts_12,	/* 223 */
	burm_nts_13,	/* 224 */
	burm_nts_14,	/* 225 */
	burm_nts_15,	/* 226 */
	burm_nts_13,	/* 227 */
	burm_nts_13,	/* 228 */
	burm_nts_13,	/* 229 */
	burm_nts_13,	/* 230 */
	burm_nts_14,	/* 231 */
	burm_nts_15,	/* 232 */
	burm_nts_14,	/* 233 */
	burm_nts_15,	/* 234 */
	burm_nts_16,	/* 235 */
	burm_nts_17,	/* 236 */
	burm_nts_10,	/* 237 */
	burm_nts_11,	/* 238 */
	burm_nts_14,	/* 239 */
	burm_nts_15,	/* 240 */
	burm_nts_16,	/* 241 */
	burm_nts_17,	/* 242 */
};

char burm_arity[] = {
	0,	/* 0 */
	2,	/* 1=IADD */
	2,	/* 2=IGE */
	2,	/* 3=IGT */
	2,	/* 4=ILE */
	2,	/* 5=IASH */
	2,	/* 6=ISUB */
	2,	/* 7=ILSH */
	2,	/* 8=ILT */
	0,	/* 9=NOP */
	1,	/* 10=ICONSTANT */
	1,	/* 11=IASSIGN */
	0,	/* 12=IF */
	0,	/* 13=IGOTO */
	2,	/* 14=IVLOAD */
	2,	/* 15=DVLOAD */
	2,	/* 16=IVSTORE */
	2,	/* 17=DVSTORE */
	1,	/* 18=ASSIGN */
	1,	/* 19=DCONSTANT */
	2,	/* 20=DADD */
	0,	/* 21=FASSIGN */
	2,	/* 22=DMUL */
	2,	/* 23=DSUB */
	1,	/* 24=DMTC1 */
	0,	/* 25=GOTO */
	1,	/* 26=TRUNCID */
	1,	/* 27=MFC1 */
	1,	/* 28=MTC1 */
	1,	/* 29=CVTDI */
	2,	/* 30=ILAND */
	1,	/* 31=DASSIGN */
	2,	/* 32=DGE */
	2,	/* 33=DLT */
	2,	/* 34=DLE */
	2,	/* 35=DGT */
	1,	/* 36=CVTSD */
	1,	/* 37=CVTDS */
	1,	/* 38=DMFC1 */
	2,	/* 39=DDIV */
	2,	/* 40=DEQ */
	2,	/* 41=FADD */
	2,	/* 42=FSUB */
	2,	/* 43=FMUL */
	2,	/* 44=FDIV */
	2,	/* 45=FVLOAD */
	2,	/* 46=FVSTORE */
	1,	/* 47=FCONSTANT */
	1,	/* 48=CVTSI */
	1,	/* 49=TRUNCIS */
	2,	/* 50=INE */
	0,	/* 51 */
	0,	/* 52 */
	0,	/* 53 */
	0,	/* 54 */
	0,	/* 55 */
	0,	/* 56 */
	0,	/* 57 */
	0,	/* 58 */
	0,	/* 59 */
	0,	/* 60 */
	0,	/* 61 */
	0,	/* 62 */
	0,	/* 63 */
	0,	/* 64 */
	0,	/* 65 */
	0,	/* 66 */
	0,	/* 67 */
	0,	/* 68 */
	0,	/* 69 */
	0,	/* 70 */
	0,	/* 71 */
	0,	/* 72 */
	0,	/* 73 */
	0,	/* 74 */
	0,	/* 75 */
	0,	/* 76 */
	0,	/* 77 */
	0,	/* 78 */
	0,	/* 79 */
	0,	/* 80 */
	0,	/* 81 */
	0,	/* 82 */
	0,	/* 83 */
	0,	/* 84 */
	0,	/* 85 */
	0,	/* 86 */
	0,	/* 87 */
	0,	/* 88 */
	0,	/* 89 */
	0,	/* 90 */
	0,	/* 91 */
	0,	/* 92 */
	0,	/* 93 */
	0,	/* 94 */
	0,	/* 95 */
	0,	/* 96 */
	0,	/* 97 */
	0,	/* 98 */
	0,	/* 99 */
	0,	/* 100 */
	0,	/* 101=R0 */
	0,	/* 102=R1 */
	0,	/* 103=R2 */
	0,	/* 104=R3 */
	0,	/* 105=R4 */
	0,	/* 106=R5 */
	0,	/* 107=R6 */
	0,	/* 108=R7 */
	0,	/* 109=R8 */
	0,	/* 110=R9 */
	0,	/* 111=R10 */
	0,	/* 112=R11 */
	0,	/* 113=R12 */
	0,	/* 114=R13 */
	0,	/* 115=R14 */
	0,	/* 116=R15 */
	0,	/* 117=R16 */
	0,	/* 118=R17 */
	0,	/* 119=R18 */
	0,	/* 120=R19 */
	0,	/* 121=R20 */
	0,	/* 122=R21 */
	0,	/* 123=R22 */
	0,	/* 124=R23 */
	0,	/* 125=R24 */
	0,	/* 126=R25 */
	0,	/* 127=sp0 */
	0,	/* 128=cc0 */
	0,	/* 129=f0 */
	0,	/* 130=f1 */
	0,	/* 131=f2 */
	0,	/* 132=f3 */
	0,	/* 133=f4 */
	0,	/* 134=f5 */
	0,	/* 135=f6 */
	0,	/* 136=f7 */
	0,	/* 137=f8 */
	0,	/* 138=f9 */
	0,	/* 139=f10 */
	0,	/* 140=f11 */
	0,	/* 141=f12 */
	0,	/* 142=f13 */
	0,	/* 143=f14 */
	0,	/* 144=f15 */
	0,	/* 145=f16 */
	0,	/* 146=f17 */
	0,	/* 147=f18 */
	0,	/* 148=f19 */
	0,	/* 149=f20 */
	0,	/* 150=f21 */
	0,	/* 151=f22 */
	0,	/* 152=f23 */
	0,	/* 153=f24 */
	0,	/* 154=f25 */
	0,	/* 155=f26 */
	0,	/* 156=f27 */
	0,	/* 157=f28 */
	0,	/* 158=f29 */
	0,	/* 159=f30 */
	0,	/* 160=f31 */
	0,	/* 161=fp0 */
	0,	/* 162=temp0 */
	0,	/* 163=temp1 */
	0,	/* 164=temp2 */
	0,	/* 165=temp3 */
	0,	/* 166=temp4 */
	0,	/* 167=temp5 */
	0,	/* 168=temp6 */
	0,	/* 169=temp7 */
	0,	/* 170=temp8 */
	0,	/* 171=temp9 */
	0,	/* 172=temp10 */
	0,	/* 173=temp11 */
	0,	/* 174=temp12 */
	0,	/* 175 */
	0,	/* 176 */
	0,	/* 177 */
	0,	/* 178 */
	0,	/* 179 */
	0,	/* 180 */
	0,	/* 181 */
	0,	/* 182 */
	0,	/* 183 */
	0,	/* 184 */
	0,	/* 185 */
	0,	/* 186 */
	0,	/* 187 */
	0,	/* 188 */
	0,	/* 189 */
	0,	/* 190 */
	0,	/* 191 */
	0,	/* 192 */
	0,	/* 193 */
	0,	/* 194 */
	0,	/* 195 */
	0,	/* 196 */
	0,	/* 197 */
	0,	/* 198 */
	0,	/* 199 */
	0,	/* 200 */
	2,	/* 201=DEF */
	0,	/* 202=INT */
	0,	/* 203=IND */
	0,	/* 204=SRC2 */
	0,	/* 205=FLOAT */
	0,	/* 206 */
	0,	/* 207 */
	0,	/* 208 */
	0,	/* 209 */
	0,	/* 210 */
	0,	/* 211 */
	0,	/* 212 */
	0,	/* 213 */
	0,	/* 214 */
	0,	/* 215 */
	0,	/* 216 */
	0,	/* 217 */
	0,	/* 218 */
	0,	/* 219 */
	0,	/* 220 */
	0,	/* 221 */
	0,	/* 222 */
	0,	/* 223 */
	0,	/* 224 */
	0,	/* 225 */
	0,	/* 226 */
	0,	/* 227 */
	0,	/* 228 */
	0,	/* 229 */
	0,	/* 230 */
	0,	/* 231 */
	0,	/* 232 */
	0,	/* 233 */
	0,	/* 234 */
	0,	/* 235 */
	0,	/* 236 */
	0,	/* 237 */
	0,	/* 238 */
	0,	/* 239 */
	0,	/* 240 */
	0,	/* 241 */
	0,	/* 242 */
	0,	/* 243 */
	0,	/* 244 */
	0,	/* 245 */
	0,	/* 246 */
	0,	/* 247 */
	0,	/* 248 */
	0,	/* 249 */
	0,	/* 250 */
	0,	/* 251 */
	0,	/* 252 */
	0,	/* 253 */
	0,	/* 254 */
	0,	/* 255 */
	0,	/* 256 */
	0,	/* 257 */
	0,	/* 258 */
	0,	/* 259 */
	0,	/* 260 */
	0,	/* 261 */
	0,	/* 262 */
	0,	/* 263 */
	0,	/* 264 */
	0,	/* 265 */
	0,	/* 266 */
	0,	/* 267 */
	0,	/* 268 */
	0,	/* 269 */
	0,	/* 270 */
	0,	/* 271 */
	0,	/* 272 */
	0,	/* 273 */
	0,	/* 274 */
	0,	/* 275 */
	0,	/* 276 */
	0,	/* 277 */
	0,	/* 278 */
	0,	/* 279 */
	0,	/* 280 */
	0,	/* 281 */
	0,	/* 282 */
	0,	/* 283 */
	0,	/* 284 */
	0,	/* 285 */
	0,	/* 286 */
	0,	/* 287 */
	0,	/* 288 */
	0,	/* 289 */
	0,	/* 290 */
	0,	/* 291 */
	0,	/* 292 */
	0,	/* 293 */
	0,	/* 294 */
	0,	/* 295 */
	0,	/* 296 */
	0,	/* 297 */
	0,	/* 298 */
	0,	/* 299 */
	0,	/* 300 */
	0,	/* 301 */
	0,	/* 302 */
	0,	/* 303 */
	0,	/* 304 */
	0,	/* 305 */
	0,	/* 306 */
	0,	/* 307 */
	0,	/* 308 */
	0,	/* 309 */
	0,	/* 310 */
	0,	/* 311 */
	0,	/* 312 */
	0,	/* 313 */
	0,	/* 314 */
	0,	/* 315 */
	0,	/* 316 */
	0,	/* 317 */
	0,	/* 318 */
	0,	/* 319 */
	0,	/* 320 */
	0,	/* 321 */
	0,	/* 322 */
	0,	/* 323 */
	0,	/* 324 */
	0,	/* 325 */
	0,	/* 326 */
	0,	/* 327 */
	0,	/* 328 */
	0,	/* 329 */
	0,	/* 330 */
	0,	/* 331 */
	0,	/* 332 */
	0,	/* 333 */
	0,	/* 334 */
	0,	/* 335 */
	0,	/* 336 */
	0,	/* 337 */
	0,	/* 338 */
	0,	/* 339 */
	0,	/* 340 */
	0,	/* 341 */
	0,	/* 342 */
	0,	/* 343 */
	0,	/* 344 */
	0,	/* 345 */
	0,	/* 346 */
	0,	/* 347 */
	0,	/* 348 */
	0,	/* 349 */
	0,	/* 350 */
	0,	/* 351 */
	0,	/* 352 */
	0,	/* 353 */
	0,	/* 354 */
	0,	/* 355 */
	0,	/* 356 */
	0,	/* 357 */
	0,	/* 358 */
	0,	/* 359 */
	0,	/* 360 */
	0,	/* 361 */
	0,	/* 362 */
	0,	/* 363 */
	0,	/* 364 */
	0,	/* 365 */
	0,	/* 366 */
	0,	/* 367 */
	0,	/* 368 */
	0,	/* 369 */
	0,	/* 370 */
	0,	/* 371 */
	0,	/* 372 */
	0,	/* 373 */
	0,	/* 374 */
	0,	/* 375 */
	0,	/* 376 */
	0,	/* 377 */
	0,	/* 378 */
	0,	/* 379 */
	0,	/* 380 */
	0,	/* 381 */
	0,	/* 382 */
	0,	/* 383 */
	0,	/* 384 */
	0,	/* 385 */
	0,	/* 386 */
	0,	/* 387 */
	0,	/* 388 */
	0,	/* 389 */
	0,	/* 390 */
	0,	/* 391 */
	0,	/* 392 */
	0,	/* 393 */
	0,	/* 394 */
	0,	/* 395 */
	0,	/* 396 */
	0,	/* 397 */
	0,	/* 398 */
	0,	/* 399 */
	0,	/* 400=NONE */
	0,	/* 401=ALU1 */
	0,	/* 402=ALU2 */
};

char *burm_opname[] = {
	/* 0 */	0,
	/* 1 */	"IADD",
	/* 2 */	"IGE",
	/* 3 */	"IGT",
	/* 4 */	"ILE",
	/* 5 */	"IASH",
	/* 6 */	"ISUB",
	/* 7 */	"ILSH",
	/* 8 */	"ILT",
	/* 9 */	"NOP",
	/* 10 */	"ICONSTANT",
	/* 11 */	"IASSIGN",
	/* 12 */	"IF",
	/* 13 */	"IGOTO",
	/* 14 */	"IVLOAD",
	/* 15 */	"DVLOAD",
	/* 16 */	"IVSTORE",
	/* 17 */	"DVSTORE",
	/* 18 */	"ASSIGN",
	/* 19 */	"DCONSTANT",
	/* 20 */	"DADD",
	/* 21 */	"FASSIGN",
	/* 22 */	"DMUL",
	/* 23 */	"DSUB",
	/* 24 */	"DMTC1",
	/* 25 */	"GOTO",
	/* 26 */	"TRUNCID",
	/* 27 */	"MFC1",
	/* 28 */	"MTC1",
	/* 29 */	"CVTDI",
	/* 30 */	"ILAND",
	/* 31 */	"DASSIGN",
	/* 32 */	"DGE",
	/* 33 */	"DLT",
	/* 34 */	"DLE",
	/* 35 */	"DGT",
	/* 36 */	"CVTSD",
	/* 37 */	"CVTDS",
	/* 38 */	"DMFC1",
	/* 39 */	"DDIV",
	/* 40 */	"DEQ",
	/* 41 */	"FADD",
	/* 42 */	"FSUB",
	/* 43 */	"FMUL",
	/* 44 */	"FDIV",
	/* 45 */	"FVLOAD",
	/* 46 */	"FVSTORE",
	/* 47 */	"FCONSTANT",
	/* 48 */	"CVTSI",
	/* 49 */	"TRUNCIS",
	/* 50 */	"INE",
	/* 51 */	0,
	/* 52 */	0,
	/* 53 */	0,
	/* 54 */	0,
	/* 55 */	0,
	/* 56 */	0,
	/* 57 */	0,
	/* 58 */	0,
	/* 59 */	0,
	/* 60 */	0,
	/* 61 */	0,
	/* 62 */	0,
	/* 63 */	0,
	/* 64 */	0,
	/* 65 */	0,
	/* 66 */	0,
	/* 67 */	0,
	/* 68 */	0,
	/* 69 */	0,
	/* 70 */	0,
	/* 71 */	0,
	/* 72 */	0,
	/* 73 */	0,
	/* 74 */	0,
	/* 75 */	0,
	/* 76 */	0,
	/* 77 */	0,
	/* 78 */	0,
	/* 79 */	0,
	/* 80 */	0,
	/* 81 */	0,
	/* 82 */	0,
	/* 83 */	0,
	/* 84 */	0,
	/* 85 */	0,
	/* 86 */	0,
	/* 87 */	0,
	/* 88 */	0,
	/* 89 */	0,
	/* 90 */	0,
	/* 91 */	0,
	/* 92 */	0,
	/* 93 */	0,
	/* 94 */	0,
	/* 95 */	0,
	/* 96 */	0,
	/* 97 */	0,
	/* 98 */	0,
	/* 99 */	0,
	/* 100 */	0,
	/* 101 */	"R0",
	/* 102 */	"R1",
	/* 103 */	"R2",
	/* 104 */	"R3",
	/* 105 */	"R4",
	/* 106 */	"R5",
	/* 107 */	"R6",
	/* 108 */	"R7",
	/* 109 */	"R8",
	/* 110 */	"R9",
	/* 111 */	"R10",
	/* 112 */	"R11",
	/* 113 */	"R12",
	/* 114 */	"R13",
	/* 115 */	"R14",
	/* 116 */	"R15",
	/* 117 */	"R16",
	/* 118 */	"R17",
	/* 119 */	"R18",
	/* 120 */	"R19",
	/* 121 */	"R20",
	/* 122 */	"R21",
	/* 123 */	"R22",
	/* 124 */	"R23",
	/* 125 */	"R24",
	/* 126 */	"R25",
	/* 127 */	"sp0",
	/* 128 */	"cc0",
	/* 129 */	"f0",
	/* 130 */	"f1",
	/* 131 */	"f2",
	/* 132 */	"f3",
	/* 133 */	"f4",
	/* 134 */	"f5",
	/* 135 */	"f6",
	/* 136 */	"f7",
	/* 137 */	"f8",
	/* 138 */	"f9",
	/* 139 */	"f10",
	/* 140 */	"f11",
	/* 141 */	"f12",
	/* 142 */	"f13",
	/* 143 */	"f14",
	/* 144 */	"f15",
	/* 145 */	"f16",
	/* 146 */	"f17",
	/* 147 */	"f18",
	/* 148 */	"f19",
	/* 149 */	"f20",
	/* 150 */	"f21",
	/* 151 */	"f22",
	/* 152 */	"f23",
	/* 153 */	"f24",
	/* 154 */	"f25",
	/* 155 */	"f26",
	/* 156 */	"f27",
	/* 157 */	"f28",
	/* 158 */	"f29",
	/* 159 */	"f30",
	/* 160 */	"f31",
	/* 161 */	"fp0",
	/* 162 */	"temp0",
	/* 163 */	"temp1",
	/* 164 */	"temp2",
	/* 165 */	"temp3",
	/* 166 */	"temp4",
	/* 167 */	"temp5",
	/* 168 */	"temp6",
	/* 169 */	"temp7",
	/* 170 */	"temp8",
	/* 171 */	"temp9",
	/* 172 */	"temp10",
	/* 173 */	"temp11",
	/* 174 */	"temp12",
	/* 175 */	0,
	/* 176 */	0,
	/* 177 */	0,
	/* 178 */	0,
	/* 179 */	0,
	/* 180 */	0,
	/* 181 */	0,
	/* 182 */	0,
	/* 183 */	0,
	/* 184 */	0,
	/* 185 */	0,
	/* 186 */	0,
	/* 187 */	0,
	/* 188 */	0,
	/* 189 */	0,
	/* 190 */	0,
	/* 191 */	0,
	/* 192 */	0,
	/* 193 */	0,
	/* 194 */	0,
	/* 195 */	0,
	/* 196 */	0,
	/* 197 */	0,
	/* 198 */	0,
	/* 199 */	0,
	/* 200 */	0,
	/* 201 */	"DEF",
	/* 202 */	"INT",
	/* 203 */	"IND",
	/* 204 */	"SRC2",
	/* 205 */	"FLOAT",
	/* 206 */	0,
	/* 207 */	0,
	/* 208 */	0,
	/* 209 */	0,
	/* 210 */	0,
	/* 211 */	0,
	/* 212 */	0,
	/* 213 */	0,
	/* 214 */	0,
	/* 215 */	0,
	/* 216 */	0,
	/* 217 */	0,
	/* 218 */	0,
	/* 219 */	0,
	/* 220 */	0,
	/* 221 */	0,
	/* 222 */	0,
	/* 223 */	0,
	/* 224 */	0,
	/* 225 */	0,
	/* 226 */	0,
	/* 227 */	0,
	/* 228 */	0,
	/* 229 */	0,
	/* 230 */	0,
	/* 231 */	0,
	/* 232 */	0,
	/* 233 */	0,
	/* 234 */	0,
	/* 235 */	0,
	/* 236 */	0,
	/* 237 */	0,
	/* 238 */	0,
	/* 239 */	0,
	/* 240 */	0,
	/* 241 */	0,
	/* 242 */	0,
	/* 243 */	0,
	/* 244 */	0,
	/* 245 */	0,
	/* 246 */	0,
	/* 247 */	0,
	/* 248 */	0,
	/* 249 */	0,
	/* 250 */	0,
	/* 251 */	0,
	/* 252 */	0,
	/* 253 */	0,
	/* 254 */	0,
	/* 255 */	0,
	/* 256 */	0,
	/* 257 */	0,
	/* 258 */	0,
	/* 259 */	0,
	/* 260 */	0,
	/* 261 */	0,
	/* 262 */	0,
	/* 263 */	0,
	/* 264 */	0,
	/* 265 */	0,
	/* 266 */	0,
	/* 267 */	0,
	/* 268 */	0,
	/* 269 */	0,
	/* 270 */	0,
	/* 271 */	0,
	/* 272 */	0,
	/* 273 */	0,
	/* 274 */	0,
	/* 275 */	0,
	/* 276 */	0,
	/* 277 */	0,
	/* 278 */	0,
	/* 279 */	0,
	/* 280 */	0,
	/* 281 */	0,
	/* 282 */	0,
	/* 283 */	0,
	/* 284 */	0,
	/* 285 */	0,
	/* 286 */	0,
	/* 287 */	0,
	/* 288 */	0,
	/* 289 */	0,
	/* 290 */	0,
	/* 291 */	0,
	/* 292 */	0,
	/* 293 */	0,
	/* 294 */	0,
	/* 295 */	0,
	/* 296 */	0,
	/* 297 */	0,
	/* 298 */	0,
	/* 299 */	0,
	/* 300 */	0,
	/* 301 */	0,
	/* 302 */	0,
	/* 303 */	0,
	/* 304 */	0,
	/* 305 */	0,
	/* 306 */	0,
	/* 307 */	0,
	/* 308 */	0,
	/* 309 */	0,
	/* 310 */	0,
	/* 311 */	0,
	/* 312 */	0,
	/* 313 */	0,
	/* 314 */	0,
	/* 315 */	0,
	/* 316 */	0,
	/* 317 */	0,
	/* 318 */	0,
	/* 319 */	0,
	/* 320 */	0,
	/* 321 */	0,
	/* 322 */	0,
	/* 323 */	0,
	/* 324 */	0,
	/* 325 */	0,
	/* 326 */	0,
	/* 327 */	0,
	/* 328 */	0,
	/* 329 */	0,
	/* 330 */	0,
	/* 331 */	0,
	/* 332 */	0,
	/* 333 */	0,
	/* 334 */	0,
	/* 335 */	0,
	/* 336 */	0,
	/* 337 */	0,
	/* 338 */	0,
	/* 339 */	0,
	/* 340 */	0,
	/* 341 */	0,
	/* 342 */	0,
	/* 343 */	0,
	/* 344 */	0,
	/* 345 */	0,
	/* 346 */	0,
	/* 347 */	0,
	/* 348 */	0,
	/* 349 */	0,
	/* 350 */	0,
	/* 351 */	0,
	/* 352 */	0,
	/* 353 */	0,
	/* 354 */	0,
	/* 355 */	0,
	/* 356 */	0,
	/* 357 */	0,
	/* 358 */	0,
	/* 359 */	0,
	/* 360 */	0,
	/* 361 */	0,
	/* 362 */	0,
	/* 363 */	0,
	/* 364 */	0,
	/* 365 */	0,
	/* 366 */	0,
	/* 367 */	0,
	/* 368 */	0,
	/* 369 */	0,
	/* 370 */	0,
	/* 371 */	0,
	/* 372 */	0,
	/* 373 */	0,
	/* 374 */	0,
	/* 375 */	0,
	/* 376 */	0,
	/* 377 */	0,
	/* 378 */	0,
	/* 379 */	0,
	/* 380 */	0,
	/* 381 */	0,
	/* 382 */	0,
	/* 383 */	0,
	/* 384 */	0,
	/* 385 */	0,
	/* 386 */	0,
	/* 387 */	0,
	/* 388 */	0,
	/* 389 */	0,
	/* 390 */	0,
	/* 391 */	0,
	/* 392 */	0,
	/* 393 */	0,
	/* 394 */	0,
	/* 395 */	0,
	/* 396 */	0,
	/* 397 */	0,
	/* 398 */	0,
	/* 399 */	0,
	/* 400 */	"NONE",
	/* 401 */	"ALU1",
	/* 402 */	"ALU2",
};

short burm_cost[][4] = {
	{ 0 },	/* 0 */
	{ 0 },	/* 1 = def: DEF(reg,expr) */
	{ 0 },	/* 2 = def: reg */
	{ 0 },	/* 3 = def: const */
	{ 0 },	/* 4 */
	{ 2 },	/* 5 = expr: IADD(def,def) */
	{ 2 },	/* 6 = expr: ISUB(def,def) */
	{ 2 },	/* 7 = expr: IVLOAD(def,def) */
	{ 2 },	/* 8 = expr: IVSTORE(def,def) */
	{ 1 },	/* 9 = expr: IASSIGN(def) */
	{ 1 },	/* 10 = expr: ILSH(def,const) */
	{ 1 },	/* 11 = expr: IASH(def,const) */
	{ 4 },	/* 12 = expr: DMUL(def,def) */
	{ 3 },	/* 13 = expr: DADD(def,def) */
	{ 3 },	/* 14 = expr: DSUB(def,def) */
	{ 1 },	/* 15 = expr: ICONSTANT(const) */
	{ 1 },	/* 16 = expr: ASSIGN(def) */
	{ 1 },	/* 17 = expr: IGE(def,def) */
	{ 1 },	/* 18 = expr: ILT(def,def) */
	{ 1 },	/* 19 = expr: ILE(def,def) */
	{ 1 },	/* 20 = expr: IGT(def,def) */
	{ 1 },	/* 21 = expr: DMTC1(def) */
	{ 1 },	/* 22 = expr: TRUNCID(def) */
	{ 1 },	/* 23 = expr: MFC1(def) */
	{ 1 },	/* 24 = expr: DCONSTANT(const) */
	{ 1 },	/* 25 = expr: MTC1(def) */
	{ 1 },	/* 26 = expr: CVTDI(def) */
	{ 1 },	/* 27 = expr: ILAND(def,def) */
	{ 1 },	/* 28 = expr: DASSIGN(def) */
	{ 1 },	/* 29 = expr: DGE(def,def) */
	{ 1 },	/* 30 = expr: DLT(def,def) */
	{ 1 },	/* 31 = expr: DLE(def,def) */
	{ 1 },	/* 32 = expr: DGT(def,def) */
	{ 1 },	/* 33 = expr: CVTSD(def) */
	{ 1 },	/* 34 = expr: CVTDS(def) */
	{ 1 },	/* 35 = expr: DMFC1(def) */
	{ 3 },	/* 36 = expr: DDIV(def,def) */
	{ 2 },	/* 37 = expr: DEQ(def,def) */
	{ 3 },	/* 38 = expr: FADD(def,def) */
	{ 3 },	/* 39 = expr: FSUB(def,def) */
	{ 3 },	/* 40 = expr: FMUL(def,def) */
	{ 3 },	/* 41 = expr: FDIV(def,def) */
	{ 2 },	/* 42 = expr: DVLOAD(def,def) */
	{ 2 },	/* 43 = expr: DVSTORE(def,def) */
	{ 2 },	/* 44 = expr: FVLOAD(def,def) */
	{ 2 },	/* 45 = expr: FVSTORE(def,def) */
	{ 1 },	/* 46 = expr: FCONSTANT(const) */
	{ 1 },	/* 47 = expr: CVTSI(def) */
	{ 1 },	/* 48 = expr: TRUNCIS(def) */
	{ 1 },	/* 49 = expr: INE(def,def) */
	{ 0 },	/* 50 */
	{ 0 },	/* 51 */
	{ 0 },	/* 52 */
	{ 0 },	/* 53 */
	{ 0 },	/* 54 */
	{ 0 },	/* 55 */
	{ 0 },	/* 56 */
	{ 0 },	/* 57 */
	{ 0 },	/* 58 */
	{ 0 },	/* 59 */
	{ 0 },	/* 60 */
	{ 0 },	/* 61 */
	{ 0 },	/* 62 */
	{ 0 },	/* 63 */
	{ 0 },	/* 64 */
	{ 0 },	/* 65 */
	{ 0 },	/* 66 */
	{ 0 },	/* 67 */
	{ 0 },	/* 68 */
	{ 0 },	/* 69 */
	{ 0 },	/* 70 */
	{ 0 },	/* 71 */
	{ 0 },	/* 72 */
	{ 0 },	/* 73 */
	{ 0 },	/* 74 */
	{ 0 },	/* 75 */
	{ 0 },	/* 76 */
	{ 0 },	/* 77 */
	{ 0 },	/* 78 */
	{ 0 },	/* 79 */
	{ 0 },	/* 80 */
	{ 0 },	/* 81 */
	{ 0 },	/* 82 */
	{ 0 },	/* 83 */
	{ 0 },	/* 84 */
	{ 0 },	/* 85 */
	{ 0 },	/* 86 */
	{ 0 },	/* 87 */
	{ 0 },	/* 88 */
	{ 0 },	/* 89 */
	{ 0 },	/* 90 */
	{ 0 },	/* 91 */
	{ 0 },	/* 92 */
	{ 0 },	/* 93 */
	{ 0 },	/* 94 */
	{ 0 },	/* 95 */
	{ 0 },	/* 96 */
	{ 0 },	/* 97 */
	{ 0 },	/* 98 */
	{ 0 },	/* 99 */
	{ 0 },	/* 100 */
	{ 0 },	/* 101 */
	{ 0 },	/* 102 */
	{ 0 },	/* 103 */
	{ 0 },	/* 104 */
	{ 0 },	/* 105 */
	{ 0 },	/* 106 */
	{ 0 },	/* 107 */
	{ 0 },	/* 108 */
	{ 0 },	/* 109 */
	{ 0 },	/* 110 */
	{ 0 },	/* 111 */
	{ 0 },	/* 112 */
	{ 0 },	/* 113 */
	{ 0 },	/* 114 = reg: rf1 */
	{ 0 },	/* 115 = reg: rf2 */
	{ 0 },	/* 116 = rf1: R0 */
	{ 0 },	/* 117 = rf1: R1 */
	{ 0 },	/* 118 = rf1: R2 */
	{ 0 },	/* 119 = rf1: R3 */
	{ 0 },	/* 120 = rf1: R4 */
	{ 0 },	/* 121 = rf1: R5 */
	{ 0 },	/* 122 = rf1: R6 */
	{ 0 },	/* 123 = rf1: R7 */
	{ 0 },	/* 124 = rf2: R8 */
	{ 0 },	/* 125 = rf2: R9 */
	{ 0 },	/* 126 = rf2: R10 */
	{ 0 },	/* 127 = rf2: R11 */
	{ 0 },	/* 128 = rf2: R12 */
	{ 0 },	/* 129 = rf2: R13 */
	{ 0 },	/* 130 = rf2: R14 */
	{ 0 },	/* 131 = rf2: R15 */
	{ 0 },	/* 132 = rf3: R16 */
	{ 0 },	/* 133 = rf3: R17 */
	{ 0 },	/* 134 = rf3: R18 */
	{ 0 },	/* 135 = rf3: R19 */
	{ 0 },	/* 136 = rf3: R20 */
	{ 0 },	/* 137 = rf3: R21 */
	{ 0 },	/* 138 = rf3: R22 */
	{ 0 },	/* 139 = rf3: R23 */
	{ 0 },	/* 140 = rf3: R24 */
	{ 0 },	/* 141 = rf3: R25 */
	{ 0 },	/* 142 = reg: rf3 */
	{ 0 },	/* 143 = rf3: sp0 */
	{ 0 },	/* 144 = rf3: cc0 */
	{ 0 },	/* 145 = rf3: f0 */
	{ 0 },	/* 146 = rf3: f1 */
	{ 0 },	/* 147 = rf3: f2 */
	{ 0 },	/* 148 = rf3: f3 */
	{ 0 },	/* 149 = rf3: f4 */
	{ 0 },	/* 150 = rf3: f5 */
	{ 0 },	/* 151 = rf3: f6 */
	{ 0 },	/* 152 = rf3: f7 */
	{ 0 },	/* 153 = rf3: f8 */
	{ 0 },	/* 154 = rf3: f9 */
	{ 0 },	/* 155 = rf3: f10 */
	{ 0 },	/* 156 = rf3: f11 */
	{ 0 },	/* 157 = rf3: f12 */
	{ 0 },	/* 158 = rf3: f13 */
	{ 0 },	/* 159 = rf3: f14 */
	{ 0 },	/* 160 = rf3: f15 */
	{ 0 },	/* 161 = rf3: f16 */
	{ 0 },	/* 162 = rf3: f17 */
	{ 0 },	/* 163 = rf3: f18 */
	{ 0 },	/* 164 = rf3: f19 */
	{ 0 },	/* 165 = rf3: f20 */
	{ 0 },	/* 166 = rf3: f21 */
	{ 0 },	/* 167 = rf3: f22 */
	{ 0 },	/* 168 = rf3: f23 */
	{ 0 },	/* 169 = rf3: f24 */
	{ 0 },	/* 170 = rf3: f25 */
	{ 0 },	/* 171 = rf3: f26 */
	{ 0 },	/* 172 = rf3: f27 */
	{ 0 },	/* 173 = rf3: f28 */
	{ 0 },	/* 174 = rf3: f29 */
	{ 0 },	/* 175 = rf3: f30 */
	{ 0 },	/* 176 = rf3: f31 */
	{ 0 },	/* 177 = rf3: fp0 */
	{ 0 },	/* 178 = const: INT */
	{ 0 },	/* 179 = rf3: temp0 */
	{ 0 },	/* 180 = rf3: temp1 */
	{ 0 },	/* 181 = rf3: temp2 */
	{ 0 },	/* 182 = rf3: temp3 */
	{ 0 },	/* 183 = rf3: temp4 */
	{ 0 },	/* 184 = rf3: temp5 */
	{ 0 },	/* 185 = rf3: temp6 */
	{ 0 },	/* 186 = rf3: temp7 */
	{ 0 },	/* 187 = rf3: temp8 */
	{ 0 },	/* 188 = rf3: temp9 */
	{ 0 },	/* 189 = rf3: temp10 */
	{ 0 },	/* 190 = rf3: temp11 */
	{ 0 },	/* 191 = rf3: temp12 */
	{ 0 },	/* 192 = const: FLOAT */
	{ 0 },	/* 193 */
	{ 0 },	/* 194 */
	{ 0 },	/* 195 */
	{ 0 },	/* 196 */
	{ 0 },	/* 197 */
	{ 0 },	/* 198 */
	{ 0 },	/* 199 */
	{ 0 },	/* 200 */
	{ 4 },	/* 201 = expr: DADD(DEF(reg,DMUL(def,def)),def) */
	{ 4 },	/* 202 = expr: DADD(def,DEF(reg,DMUL(def,def))) */
	{ 0 },	/* 203 = expr: IASSIGN(const) */
	{ 0 },	/* 204 = def: DEF(reg,ICONSTANT(const)) */
	{ 0 },	/* 205 = def: DEF(reg,IASSIGN(def)) */
	{ 0 },	/* 206 */
	{ 0 },	/* 207 = expr: IADD(INT,INT) */
	{ 0 },	/* 208 = expr: ISUB(INT,INT) */
	{ 0 },	/* 209 = expr: DMUL(INT,INT) */
	{ 0 },	/* 210 */
	{ 4 },	/* 211 = expr: DMUL(DEF(reg,DMUL(def,INT)),INT) */
	{ 4 },	/* 212 = expr: DMUL(INT,DEF(reg,DMUL(def,INT))) */
	{ 4 },	/* 213 = expr: DMUL(DEF(reg,DMUL(INT,def)),INT) */
	{ 4 },	/* 214 = expr: DMUL(INT,DEF(reg,DMUL(INT,def))) */
	{ 0 },	/* 215 */
	{ 0 },	/* 216 */
	{ 0 },	/* 217 */
	{ 0 },	/* 218 */
	{ 0 },	/* 219 */
	{ 2 },	/* 220 = expr: DMUL(reg,INT) */
	{ 2 },	/* 221 = expr: DMUL(INT,reg) */
	{ 0 },	/* 222 */
	{ 0 },	/* 223 = def: DEF(reg,DCONSTANT(const)) */
	{ 0 },	/* 224 = def: DEF(reg,ASSIGN(def)) */
	{ 3 },	/* 225 = def: DEF(reg,IADD(DEF(reg,IADD(DEF(reg,expr),def)),def)) */
	{ 3 },	/* 226 = def: DEF(reg,IADD(def,DEF(reg,IADD(def,DEF(reg,expr))))) */
	{ 2 },	/* 227 = expr: IADD(DEF(reg,IADD(def,INT)),INT) */
	{ 2 },	/* 228 = expr: IADD(INT,DEF(reg,IADD(def,INT))) */
	{ 2 },	/* 229 = expr: IADD(DEF(reg,IADD(INT,def)),INT) */
	{ 2 },	/* 230 = expr: IADD(INT,DEF(reg,IADD(INT,def))) */
	{ 3 },	/* 231 = def: DEF(reg,DMUL(DEF(reg,DMUL(DEF(reg,expr),def)),def)) */
	{ 3 },	/* 232 = def: DEF(reg,DMUL(def,DEF(reg,DMUL(def,DEF(reg,expr))))) */
	{ 3 },	/* 233 = def: DEF(reg,ISUB(DEF(reg,ISUB(DEF(reg,expr),def)),def)) */
	{ 3 },	/* 234 = def: DEF(reg,ISUB(def,DEF(reg,ISUB(def,DEF(reg,expr))))) */
	{ 3 },	/* 235 = def: DEF(reg,IADD(DEF(reg,IADD(def,DEF(reg,expr))),def)) */
	{ 3 },	/* 236 = def: DEF(reg,IADD(def,DEF(reg,IADD(DEF(reg,expr),def)))) */
	{ 5 },	/* 237 = expr: DMUL(DEF(reg,DADD(def,def)),def) */
	{ 5 },	/* 238 = expr: DMUL(def,DEF(reg,DADD(def,def))) */
	{ 3 },	/* 239 = def: DEF(reg,DADD(DEF(reg,DADD(DEF(reg,expr),def)),def)) */
	{ 3 },	/* 240 = def: DEF(reg,DADD(def,DEF(reg,DADD(def,DEF(reg,expr))))) */
	{ 3 },	/* 241 = def: DEF(reg,DADD(DEF(reg,DADD(def,DEF(reg,expr))),def)) */
	{ 3 },	/* 242 = def: DEF(reg,DADD(def,DEF(reg,DADD(DEF(reg,expr),def)))) */
};

char *burm_string[] = {
	/* 0 */	0,
	/* 1 */	"def: DEF(reg,expr)",
	/* 2 */	"def: reg",
	/* 3 */	"def: const",
	/* 4 */	0,
	/* 5 */	"expr: IADD(def,def)",
	/* 6 */	"expr: ISUB(def,def)",
	/* 7 */	"expr: IVLOAD(def,def)",
	/* 8 */	"expr: IVSTORE(def,def)",
	/* 9 */	"expr: IASSIGN(def)",
	/* 10 */	"expr: ILSH(def,const)",
	/* 11 */	"expr: IASH(def,const)",
	/* 12 */	"expr: DMUL(def,def)",
	/* 13 */	"expr: DADD(def,def)",
	/* 14 */	"expr: DSUB(def,def)",
	/* 15 */	"expr: ICONSTANT(const)",
	/* 16 */	"expr: ASSIGN(def)",
	/* 17 */	"expr: IGE(def,def)",
	/* 18 */	"expr: ILT(def,def)",
	/* 19 */	"expr: ILE(def,def)",
	/* 20 */	"expr: IGT(def,def)",
	/* 21 */	"expr: DMTC1(def)",
	/* 22 */	"expr: TRUNCID(def)",
	/* 23 */	"expr: MFC1(def)",
	/* 24 */	"expr: DCONSTANT(const)",
	/* 25 */	"expr: MTC1(def)",
	/* 26 */	"expr: CVTDI(def)",
	/* 27 */	"expr: ILAND(def,def)",
	/* 28 */	"expr: DASSIGN(def)",
	/* 29 */	"expr: DGE(def,def)",
	/* 30 */	"expr: DLT(def,def)",
	/* 31 */	"expr: DLE(def,def)",
	/* 32 */	"expr: DGT(def,def)",
	/* 33 */	"expr: CVTSD(def)",
	/* 34 */	"expr: CVTDS(def)",
	/* 35 */	"expr: DMFC1(def)",
	/* 36 */	"expr: DDIV(def,def)",
	/* 37 */	"expr: DEQ(def,def)",
	/* 38 */	"expr: FADD(def,def)",
	/* 39 */	"expr: FSUB(def,def)",
	/* 40 */	"expr: FMUL(def,def)",
	/* 41 */	"expr: FDIV(def,def)",
	/* 42 */	"expr: DVLOAD(def,def)",
	/* 43 */	"expr: DVSTORE(def,def)",
	/* 44 */	"expr: FVLOAD(def,def)",
	/* 45 */	"expr: FVSTORE(def,def)",
	/* 46 */	"expr: FCONSTANT(const)",
	/* 47 */	"expr: CVTSI(def)",
	/* 48 */	"expr: TRUNCIS(def)",
	/* 49 */	"expr: INE(def,def)",
	/* 50 */	0,
	/* 51 */	0,
	/* 52 */	0,
	/* 53 */	0,
	/* 54 */	0,
	/* 55 */	0,
	/* 56 */	0,
	/* 57 */	0,
	/* 58 */	0,
	/* 59 */	0,
	/* 60 */	0,
	/* 61 */	0,
	/* 62 */	0,
	/* 63 */	0,
	/* 64 */	0,
	/* 65 */	0,
	/* 66 */	0,
	/* 67 */	0,
	/* 68 */	0,
	/* 69 */	0,
	/* 70 */	0,
	/* 71 */	0,
	/* 72 */	0,
	/* 73 */	0,
	/* 74 */	0,
	/* 75 */	0,
	/* 76 */	0,
	/* 77 */	0,
	/* 78 */	0,
	/* 79 */	0,
	/* 80 */	0,
	/* 81 */	0,
	/* 82 */	0,
	/* 83 */	0,
	/* 84 */	0,
	/* 85 */	0,
	/* 86 */	0,
	/* 87 */	0,
	/* 88 */	0,
	/* 89 */	0,
	/* 90 */	0,
	/* 91 */	0,
	/* 92 */	0,
	/* 93 */	0,
	/* 94 */	0,
	/* 95 */	0,
	/* 96 */	0,
	/* 97 */	0,
	/* 98 */	0,
	/* 99 */	0,
	/* 100 */	0,
	/* 101 */	0,
	/* 102 */	0,
	/* 103 */	0,
	/* 104 */	0,
	/* 105 */	0,
	/* 106 */	0,
	/* 107 */	0,
	/* 108 */	0,
	/* 109 */	0,
	/* 110 */	0,
	/* 111 */	0,
	/* 112 */	0,
	/* 113 */	0,
	/* 114 */	"reg: rf1",
	/* 115 */	"reg: rf2",
	/* 116 */	"rf1: R0",
	/* 117 */	"rf1: R1",
	/* 118 */	"rf1: R2",
	/* 119 */	"rf1: R3",
	/* 120 */	"rf1: R4",
	/* 121 */	"rf1: R5",
	/* 122 */	"rf1: R6",
	/* 123 */	"rf1: R7",
	/* 124 */	"rf2: R8",
	/* 125 */	"rf2: R9",
	/* 126 */	"rf2: R10",
	/* 127 */	"rf2: R11",
	/* 128 */	"rf2: R12",
	/* 129 */	"rf2: R13",
	/* 130 */	"rf2: R14",
	/* 131 */	"rf2: R15",
	/* 132 */	"rf3: R16",
	/* 133 */	"rf3: R17",
	/* 134 */	"rf3: R18",
	/* 135 */	"rf3: R19",
	/* 136 */	"rf3: R20",
	/* 137 */	"rf3: R21",
	/* 138 */	"rf3: R22",
	/* 139 */	"rf3: R23",
	/* 140 */	"rf3: R24",
	/* 141 */	"rf3: R25",
	/* 142 */	"reg: rf3",
	/* 143 */	"rf3: sp0",
	/* 144 */	"rf3: cc0",
	/* 145 */	"rf3: f0",
	/* 146 */	"rf3: f1",
	/* 147 */	"rf3: f2",
	/* 148 */	"rf3: f3",
	/* 149 */	"rf3: f4",
	/* 150 */	"rf3: f5",
	/* 151 */	"rf3: f6",
	/* 152 */	"rf3: f7",
	/* 153 */	"rf3: f8",
	/* 154 */	"rf3: f9",
	/* 155 */	"rf3: f10",
	/* 156 */	"rf3: f11",
	/* 157 */	"rf3: f12",
	/* 158 */	"rf3: f13",
	/* 159 */	"rf3: f14",
	/* 160 */	"rf3: f15",
	/* 161 */	"rf3: f16",
	/* 162 */	"rf3: f17",
	/* 163 */	"rf3: f18",
	/* 164 */	"rf3: f19",
	/* 165 */	"rf3: f20",
	/* 166 */	"rf3: f21",
	/* 167 */	"rf3: f22",
	/* 168 */	"rf3: f23",
	/* 169 */	"rf3: f24",
	/* 170 */	"rf3: f25",
	/* 171 */	"rf3: f26",
	/* 172 */	"rf3: f27",
	/* 173 */	"rf3: f28",
	/* 174 */	"rf3: f29",
	/* 175 */	"rf3: f30",
	/* 176 */	"rf3: f31",
	/* 177 */	"rf3: fp0",
	/* 178 */	"const: INT",
	/* 179 */	"rf3: temp0",
	/* 180 */	"rf3: temp1",
	/* 181 */	"rf3: temp2",
	/* 182 */	"rf3: temp3",
	/* 183 */	"rf3: temp4",
	/* 184 */	"rf3: temp5",
	/* 185 */	"rf3: temp6",
	/* 186 */	"rf3: temp7",
	/* 187 */	"rf3: temp8",
	/* 188 */	"rf3: temp9",
	/* 189 */	"rf3: temp10",
	/* 190 */	"rf3: temp11",
	/* 191 */	"rf3: temp12",
	/* 192 */	"const: FLOAT",
	/* 193 */	0,
	/* 194 */	0,
	/* 195 */	0,
	/* 196 */	0,
	/* 197 */	0,
	/* 198 */	0,
	/* 199 */	0,
	/* 200 */	0,
	/* 201 */	"expr: DADD(DEF(reg,DMUL(def,def)),def)",
	/* 202 */	"expr: DADD(def,DEF(reg,DMUL(def,def)))",
	/* 203 */	"expr: IASSIGN(const)",
	/* 204 */	"def: DEF(reg,ICONSTANT(const))",
	/* 205 */	"def: DEF(reg,IASSIGN(def))",
	/* 206 */	0,
	/* 207 */	"expr: IADD(INT,INT)",
	/* 208 */	"expr: ISUB(INT,INT)",
	/* 209 */	"expr: DMUL(INT,INT)",
	/* 210 */	0,
	/* 211 */	"expr: DMUL(DEF(reg,DMUL(def,INT)),INT)",
	/* 212 */	"expr: DMUL(INT,DEF(reg,DMUL(def,INT)))",
	/* 213 */	"expr: DMUL(DEF(reg,DMUL(INT,def)),INT)",
	/* 214 */	"expr: DMUL(INT,DEF(reg,DMUL(INT,def)))",
	/* 215 */	0,
	/* 216 */	0,
	/* 217 */	0,
	/* 218 */	0,
	/* 219 */	0,
	/* 220 */	"expr: DMUL(reg,INT)",
	/* 221 */	"expr: DMUL(INT,reg)",
	/* 222 */	0,
	/* 223 */	"def: DEF(reg,DCONSTANT(const))",
	/* 224 */	"def: DEF(reg,ASSIGN(def))",
	/* 225 */	"def: DEF(reg,IADD(DEF(reg,IADD(DEF(reg,expr),def)),def))",
	/* 226 */	"def: DEF(reg,IADD(def,DEF(reg,IADD(def,DEF(reg,expr)))))",
	/* 227 */	"expr: IADD(DEF(reg,IADD(def,INT)),INT)",
	/* 228 */	"expr: IADD(INT,DEF(reg,IADD(def,INT)))",
	/* 229 */	"expr: IADD(DEF(reg,IADD(INT,def)),INT)",
	/* 230 */	"expr: IADD(INT,DEF(reg,IADD(INT,def)))",
	/* 231 */	"def: DEF(reg,DMUL(DEF(reg,DMUL(DEF(reg,expr),def)),def))",
	/* 232 */	"def: DEF(reg,DMUL(def,DEF(reg,DMUL(def,DEF(reg,expr)))))",
	/* 233 */	"def: DEF(reg,ISUB(DEF(reg,ISUB(DEF(reg,expr),def)),def))",
	/* 234 */	"def: DEF(reg,ISUB(def,DEF(reg,ISUB(def,DEF(reg,expr)))))",
	/* 235 */	"def: DEF(reg,IADD(DEF(reg,IADD(def,DEF(reg,expr))),def))",
	/* 236 */	"def: DEF(reg,IADD(def,DEF(reg,IADD(DEF(reg,expr),def))))",
	/* 237 */	"expr: DMUL(DEF(reg,DADD(def,def)),def)",
	/* 238 */	"expr: DMUL(def,DEF(reg,DADD(def,def)))",
	/* 239 */	"def: DEF(reg,DADD(DEF(reg,DADD(DEF(reg,expr),def)),def))",
	/* 240 */	"def: DEF(reg,DADD(def,DEF(reg,DADD(def,DEF(reg,expr)))))",
	/* 241 */	"def: DEF(reg,DADD(DEF(reg,DADD(def,DEF(reg,expr))),def))",
	/* 242 */	"def: DEF(reg,DADD(def,DEF(reg,DADD(DEF(reg,expr),def))))",
};

static short burm_decode_def[] = {
	0,
	1,
	2,
	3,
	204,
	205,
	223,
	224,
	225,
	226,
	231,
	232,
	233,
	234,
	235,
	236,
	239,
	240,
	241,
	242,
};

static short burm_decode_reg[] = {
	0,
	114,
	115,
	142,
};

static short burm_decode_expr[] = {
	0,
	5,
	6,
	7,
	8,
	9,
	10,
	11,
	12,
	13,
	14,
	15,
	16,
	17,
	18,
	19,
	20,
	21,
	22,
	23,
	24,
	25,
	26,
	27,
	28,
	29,
	30,
	31,
	32,
	33,
	34,
	35,
	36,
	37,
	38,
	39,
	40,
	41,
	42,
	43,
	44,
	45,
	46,
	47,
	48,
	49,
	201,
	202,
	203,
	207,
	208,
	209,
	211,
	212,
	213,
	214,
	220,
	221,
	227,
	228,
	229,
	230,
	237,
	238,
};

static short burm_decode_const[] = {
	0,
	178,
	192,
};

static short burm_decode_rf1[] = {
	0,
	116,
	117,
	118,
	119,
	120,
	121,
	122,
	123,
};

static short burm_decode_rf2[] = {
	0,
	124,
	125,
	126,
	127,
	128,
	129,
	130,
	131,
};

static short burm_decode_rf3[] = {
	0,
	132,
	133,
	134,
	135,
	136,
	137,
	138,
	139,
	140,
	141,
	143,
	144,
	145,
	146,
	147,
	148,
	149,
	150,
	151,
	152,
	153,
	154,
	155,
	156,
	157,
	158,
	159,
	160,
	161,
	162,
	163,
	164,
	165,
	166,
	167,
	168,
	169,
	170,
	171,
	172,
	173,
	174,
	175,
	176,
	177,
	179,
	180,
	181,
	182,
	183,
	184,
	185,
	186,
	187,
	188,
	189,
	190,
	191,
};

int burm_rule(STATE_TYPE state, int goalnt) {
	burm_assert(goalnt >= 1 && goalnt <= 7, PANIC("Bad goal nonterminal %d in burm_rule\n", goalnt));
	if (!state)
		return 0;
	switch (goalnt) {
	case burm_def_NT:	return burm_decode_def[((struct burm_state *)state)->rule.burm_def];
	case burm_reg_NT:	return burm_decode_reg[((struct burm_state *)state)->rule.burm_reg];
	case burm_expr_NT:	return burm_decode_expr[((struct burm_state *)state)->rule.burm_expr];
	case burm_const_NT:	return burm_decode_const[((struct burm_state *)state)->rule.burm_const];
	case burm_rf1_NT:	return burm_decode_rf1[((struct burm_state *)state)->rule.burm_rf1];
	case burm_rf2_NT:	return burm_decode_rf2[((struct burm_state *)state)->rule.burm_rf2];
	case burm_rf3_NT:	return burm_decode_rf3[((struct burm_state *)state)->rule.burm_rf3];
	default:
		burm_assert(0, PANIC("Bad goal nonterminal %d in burm_rule\n", goalnt));
	}
	return 0;
}

static void burm_closure_reg(struct burm_state *, int);
static void burm_closure_const(struct burm_state *, int);
static void burm_closure_rf1(struct burm_state *, int);
static void burm_closure_rf2(struct burm_state *, int);
static void burm_closure_rf3(struct burm_state *, int);

static void burm_closure_reg(struct burm_state *p, int c) {
	if (c + 0 < p->cost[burm_def_NT]) {
		p->cost[burm_def_NT] = c + 0;
		p->rule.burm_def = 2;
	}
}

static void burm_closure_const(struct burm_state *p, int c) {
	if (c + 0 < p->cost[burm_def_NT]) {
		p->cost[burm_def_NT] = c + 0;
		p->rule.burm_def = 3;
	}
}

static void burm_closure_rf1(struct burm_state *p, int c) {
	if (c + 0 < p->cost[burm_reg_NT]) {
		p->cost[burm_reg_NT] = c + 0;
		p->rule.burm_reg = 1;
		burm_closure_reg(p, c + 0);
	}
}

static void burm_closure_rf2(struct burm_state *p, int c) {
	if (c + 0 < p->cost[burm_reg_NT]) {
		p->cost[burm_reg_NT] = c + 0;
		p->rule.burm_reg = 2;
		burm_closure_reg(p, c + 0);
	}
}

static void burm_closure_rf3(struct burm_state *p, int c) {
	if (c + 0 < p->cost[burm_reg_NT]) {
		p->cost[burm_reg_NT] = c + 0;
		p->rule.burm_reg = 3;
		burm_closure_reg(p, c + 0);
	}
}

STATE_TYPE burm_state(int op, STATE_TYPE left, STATE_TYPE right) {
	int c;
	struct burm_state *p, *l = (struct burm_state *)left,
		*r = (struct burm_state *)right;

	assert(sizeof (STATE_TYPE) >= sizeof (void *));
	if (burm_arity[op] > 0) {
		p = (struct burm_state *)ALLOC(sizeof *p);
		burm_assert(p, PANIC("ALLOC returned NULL in burm_state\n"));
		p->op = op;
		p->left = l;
		p->right = r;
		p->rule.burm_def = 0;
		p->cost[1] =
		p->cost[2] =
		p->cost[3] =
		p->cost[4] =
		p->cost[5] =
		p->cost[6] =
		p->cost[7] =
			32767;
	}
	switch (op) {
	case 1: /* IADD */
		assert(l && r);
		if (	/* expr: IADD(INT,DEF(reg,IADD(INT,def))) */
			l->op == 202 && /* INT */
			r->op == 201 && /* DEF */
			r->right->op == 1 && /* IADD */
			r->right->left->op == 202 /* INT */
		) {
			c = r->left->cost[burm_reg_NT] + r->right->right->cost[burm_def_NT] + 2;
			if (c + 0 < p->cost[burm_expr_NT]) {
				p->cost[burm_expr_NT] = c + 0;
				p->rule.burm_expr = 61;
			}
		}
		if (	/* expr: IADD(DEF(reg,IADD(INT,def)),INT) */
			l->op == 201 && /* DEF */
			l->right->op == 1 && /* IADD */
			l->right->left->op == 202 && /* INT */
			r->op == 202 /* INT */
		) {
			c = l->left->cost[burm_reg_NT] + l->right->right->cost[burm_def_NT] + 2;
			if (c + 0 < p->cost[burm_expr_NT]) {
				p->cost[burm_expr_NT] = c + 0;
				p->rule.burm_expr = 60;
			}
		}
		if (	/* expr: IADD(INT,DEF(reg,IADD(def,INT))) */
			l->op == 202 && /* INT */
			r->op == 201 && /* DEF */
			r->right->op == 1 && /* IADD */
			r->right->right->op == 202 /* INT */
		) {
			c = r->left->cost[burm_reg_NT] + r->right->left->cost[burm_def_NT] + 2;
			if (c + 0 < p->cost[burm_expr_NT]) {
				p->cost[burm_expr_NT] = c + 0;
				p->rule.burm_expr = 59;
			}
		}
		if (	/* expr: IADD(DEF(reg,IADD(def,INT)),INT) */
			l->op == 201 && /* DEF */
			l->right->op == 1 && /* IADD */
			l->right->right->op == 202 && /* INT */
			r->op == 202 /* INT */
		) {
			c = l->left->cost[burm_reg_NT] + l->right->left->cost[burm_def_NT] + 2;
			if (c + 0 < p->cost[burm_expr_NT]) {
				p->cost[burm_expr_NT] = c + 0;
				p->rule.burm_expr = 58;
			}
		}
		if (	/* expr: IADD(INT,INT) */
			l->op == 202 && /* INT */
			r->op == 202 /* INT */
		) {
			c = 0;
			if (c + 0 < p->cost[burm_expr_NT]) {
				p->cost[burm_expr_NT] = c + 0;
				p->rule.burm_expr = 49;
			}
		}
		{	/* expr: IADD(def,def) */
			c = l->cost[burm_def_NT] + r->cost[burm_def_NT] + 2;
			if (c + 0 < p->cost[burm_expr_NT]) {
				p->cost[burm_expr_NT] = c + 0;
				p->rule.burm_expr = 1;
			}
		}
		break;
	case 2: /* IGE */
		assert(l && r);
		{	/* expr: IGE(def,def) */
			c = l->cost[burm_def_NT] + r->cost[burm_def_NT] + 1;
			if (c + 0 < p->cost[burm_expr_NT]) {
				p->cost[burm_expr_NT] = c + 0;
				p->rule.burm_expr = 13;
			}
		}
		break;
	case 3: /* IGT */
		assert(l && r);
		{	/* expr: IGT(def,def) */
			c = l->cost[burm_def_NT] + r->cost[burm_def_NT] + 1;
			if (c + 0 < p->cost[burm_expr_NT]) {
				p->cost[burm_expr_NT] = c + 0;
				p->rule.burm_expr = 16;
			}
		}
		break;
	case 4: /* ILE */
		assert(l && r);
		{	/* expr: ILE(def,def) */
			c = l->cost[burm_def_NT] + r->cost[burm_def_NT] + 1;
			if (c + 0 < p->cost[burm_expr_NT]) {
				p->cost[burm_expr_NT] = c + 0;
				p->rule.burm_expr = 15;
			}
		}
		break;
	case 5: /* IASH */
		assert(l && r);
		{	/* expr: IASH(def,const) */
			c = l->cost[burm_def_NT] + r->cost[burm_const_NT] + 1;
			if (c + 0 < p->cost[burm_expr_NT]) {
				p->cost[burm_expr_NT] = c + 0;
				p->rule.burm_expr = 7;
			}
		}
		break;
	case 6: /* ISUB */
		assert(l && r);
		if (	/* expr: ISUB(INT,INT) */
			l->op == 202 && /* INT */
			r->op == 202 /* INT */
		) {
			c = 0;
			if (c + 0 < p->cost[burm_expr_NT]) {
				p->cost[burm_expr_NT] = c + 0;
				p->rule.burm_expr = 50;
			}
		}
		{	/* expr: ISUB(def,def) */
			c = l->cost[burm_def_NT] + r->cost[burm_def_NT] + 2;
			if (c + 0 < p->cost[burm_expr_NT]) {
				p->cost[burm_expr_NT] = c + 0;
				p->rule.burm_expr = 2;
			}
		}
		break;
	case 7: /* ILSH */
		assert(l && r);
		{	/* expr: ILSH(def,const) */
			c = l->cost[burm_def_NT] + r->cost[burm_const_NT] + 1;
			if (c + 0 < p->cost[burm_expr_NT]) {
				p->cost[burm_expr_NT] = c + 0;
				p->rule.burm_expr = 6;
			}
		}
		break;
	case 8: /* ILT */
		assert(l && r);
		{	/* expr: ILT(def,def) */
			c = l->cost[burm_def_NT] + r->cost[burm_def_NT] + 1;
			if (c + 0 < p->cost[burm_expr_NT]) {
				p->cost[burm_expr_NT] = c + 0;
				p->rule.burm_expr = 14;
			}
		}
		break;
	case 9: /* NOP */
		{
			static struct burm_state z = { 9, 0, 0,
				{	0,
					32767,
					32767,
					32767,
					32767,
					32767,
					32767,
					32767,
				},{
					0,
					0,
					0,
					0,
					0,
					0,
					0,
				}
			};
			return (STATE_TYPE)&z;
		}
	case 10: /* ICONSTANT */
		assert(l);
		{	/* expr: ICONSTANT(const) */
			c = l->cost[burm_const_NT] + 1;
			if (c + 0 < p->cost[burm_expr_NT]) {
				p->cost[burm_expr_NT] = c + 0;
				p->rule.burm_expr = 11;
			}
		}
		break;
	case 11: /* IASSIGN */
		assert(l);
		{	/* expr: IASSIGN(const) */
			c = l->cost[burm_const_NT] + 0;
			if (c + 0 < p->cost[burm_expr_NT]) {
				p->cost[burm_expr_NT] = c + 0;
				p->rule.burm_expr = 48;
			}
		}
		{	/* expr: IASSIGN(def) */
			c = l->cost[burm_def_NT] + 1;
			if (c + 0 < p->cost[burm_expr_NT]) {
				p->cost[burm_expr_NT] = c + 0;
				p->rule.burm_expr = 5;
			}
		}
		break;
	case 12: /* IF */
		{
			static struct burm_state z = { 12, 0, 0,
				{	0,
					32767,
					32767,
					32767,
					32767,
					32767,
					32767,
					32767,
				},{
					0,
					0,
					0,
					0,
					0,
					0,
					0,
				}
			};
			return (STATE_TYPE)&z;
		}
	case 13: /* IGOTO */
		{
			static struct burm_state z = { 13, 0, 0,
				{	0,
					32767,
					32767,
					32767,
					32767,
					32767,
					32767,
					32767,
				},{
					0,
					0,
					0,
					0,
					0,
					0,
					0,
				}
			};
			return (STATE_TYPE)&z;
		}
	case 14: /* IVLOAD */
		assert(l && r);
		{	/* expr: IVLOAD(def,def) */
			c = l->cost[burm_def_NT] + r->cost[burm_def_NT] + 2;
			if (c + 0 < p->cost[burm_expr_NT]) {
				p->cost[burm_expr_NT] = c + 0;
				p->rule.burm_expr = 3;
			}
		}
		break;
	case 15: /* DVLOAD */
		assert(l && r);
		{	/* expr: DVLOAD(def,def) */
			c = l->cost[burm_def_NT] + r->cost[burm_def_NT] + 2;
			if (c + 0 < p->cost[burm_expr_NT]) {
				p->cost[burm_expr_NT] = c + 0;
				p->rule.burm_expr = 38;
			}
		}
		break;
	case 16: /* IVSTORE */
		assert(l && r);
		{	/* expr: IVSTORE(def,def) */
			c = l->cost[burm_def_NT] + r->cost[burm_def_NT] + 2;
			if (c + 0 < p->cost[burm_expr_NT]) {
				p->cost[burm_expr_NT] = c + 0;
				p->rule.burm_expr = 4;
			}
		}
		break;
	case 17: /* DVSTORE */
		assert(l && r);
		{	/* expr: DVSTORE(def,def) */
			c = l->cost[burm_def_NT] + r->cost[burm_def_NT] + 2;
			if (c + 0 < p->cost[burm_expr_NT]) {
				p->cost[burm_expr_NT] = c + 0;
				p->rule.burm_expr = 39;
			}
		}
		break;
	case 18: /* ASSIGN */
		assert(l);
		{	/* expr: ASSIGN(def) */
			c = l->cost[burm_def_NT] + 1;
			if (c + 0 < p->cost[burm_expr_NT]) {
				p->cost[burm_expr_NT] = c + 0;
				p->rule.burm_expr = 12;
			}
		}
		break;
	case 19: /* DCONSTANT */
		assert(l);
		{	/* expr: DCONSTANT(const) */
			c = l->cost[burm_const_NT] + 1;
			if (c + 0 < p->cost[burm_expr_NT]) {
				p->cost[burm_expr_NT] = c + 0;
				p->rule.burm_expr = 20;
			}
		}
		break;
	case 20: /* DADD */
		assert(l && r);
		if (	/* expr: DADD(def,DEF(reg,DMUL(def,def))) */
			r->op == 201 && /* DEF */
			r->right->op == 22 /* DMUL */
		) {
			c = l->cost[burm_def_NT] + r->left->cost[burm_reg_NT] + r->right->left->cost[burm_def_NT] + r->right->right->cost[burm_def_NT] + 4;
			if (c + 0 < p->cost[burm_expr_NT]) {
				p->cost[burm_expr_NT] = c + 0;
				p->rule.burm_expr = 47;
			}
		}
		if (	/* expr: DADD(DEF(reg,DMUL(def,def)),def) */
			l->op == 201 && /* DEF */
			l->right->op == 22 /* DMUL */
		) {
			c = l->left->cost[burm_reg_NT] + l->right->left->cost[burm_def_NT] + l->right->right->cost[burm_def_NT] + r->cost[burm_def_NT] + 4;
			if (c + 0 < p->cost[burm_expr_NT]) {
				p->cost[burm_expr_NT] = c + 0;
				p->rule.burm_expr = 46;
			}
		}
		{	/* expr: DADD(def,def) */
			c = l->cost[burm_def_NT] + r->cost[burm_def_NT] + 3;
			if (c + 0 < p->cost[burm_expr_NT]) {
				p->cost[burm_expr_NT] = c + 0;
				p->rule.burm_expr = 9;
			}
		}
		break;
	case 21: /* FASSIGN */
		{
			static struct burm_state z = { 21, 0, 0,
				{	0,
					32767,
					32767,
					32767,
					32767,
					32767,
					32767,
					32767,
				},{
					0,
					0,
					0,
					0,
					0,
					0,
					0,
				}
			};
			return (STATE_TYPE)&z;
		}
	case 22: /* DMUL */
		assert(l && r);
		if (	/* expr: DMUL(def,DEF(reg,DADD(def,def))) */
			r->op == 201 && /* DEF */
			r->right->op == 20 /* DADD */
		) {
			c = l->cost[burm_def_NT] + r->left->cost[burm_reg_NT] + r->right->left->cost[burm_def_NT] + r->right->right->cost[burm_def_NT] + 5;
			if (c + 0 < p->cost[burm_expr_NT]) {
				p->cost[burm_expr_NT] = c + 0;
				p->rule.burm_expr = 63;
			}
		}
		if (	/* expr: DMUL(DEF(reg,DADD(def,def)),def) */
			l->op == 201 && /* DEF */
			l->right->op == 20 /* DADD */
		) {
			c = l->left->cost[burm_reg_NT] + l->right->left->cost[burm_def_NT] + l->right->right->cost[burm_def_NT] + r->cost[burm_def_NT] + 5;
			if (c + 0 < p->cost[burm_expr_NT]) {
				p->cost[burm_expr_NT] = c + 0;
				p->rule.burm_expr = 62;
			}
		}
		if (	/* expr: DMUL(INT,reg) */
			l->op == 202 /* INT */
		) {
			c = r->cost[burm_reg_NT] + 2;
			if (c + 0 < p->cost[burm_expr_NT]) {
				p->cost[burm_expr_NT] = c + 0;
				p->rule.burm_expr = 57;
			}
		}
		if (	/* expr: DMUL(reg,INT) */
			r->op == 202 /* INT */
		) {
			c = l->cost[burm_reg_NT] + 2;
			if (c + 0 < p->cost[burm_expr_NT]) {
				p->cost[burm_expr_NT] = c + 0;
				p->rule.burm_expr = 56;
			}
		}
		if (	/* expr: DMUL(INT,DEF(reg,DMUL(INT,def))) */
			l->op == 202 && /* INT */
			r->op == 201 && /* DEF */
			r->right->op == 22 && /* DMUL */
			r->right->left->op == 202 /* INT */
		) {
			c = r->left->cost[burm_reg_NT] + r->right->right->cost[burm_def_NT] + 4;
			if (c + 0 < p->cost[burm_expr_NT]) {
				p->cost[burm_expr_NT] = c + 0;
				p->rule.burm_expr = 55;
			}
		}
		if (	/* expr: DMUL(DEF(reg,DMUL(INT,def)),INT) */
			l->op == 201 && /* DEF */
			l->right->op == 22 && /* DMUL */
			l->right->left->op == 202 && /* INT */
			r->op == 202 /* INT */
		) {
			c = l->left->cost[burm_reg_NT] + l->right->right->cost[burm_def_NT] + 4;
			if (c + 0 < p->cost[burm_expr_NT]) {
				p->cost[burm_expr_NT] = c + 0;
				p->rule.burm_expr = 54;
			}
		}
		if (	/* expr: DMUL(INT,DEF(reg,DMUL(def,INT))) */
			l->op == 202 && /* INT */
			r->op == 201 && /* DEF */
			r->right->op == 22 && /* DMUL */
			r->right->right->op == 202 /* INT */
		) {
			c = r->left->cost[burm_reg_NT] + r->right->left->cost[burm_def_NT] + 4;
			if (c + 0 < p->cost[burm_expr_NT]) {
				p->cost[burm_expr_NT] = c + 0;
				p->rule.burm_expr = 53;
			}
		}
		if (	/* expr: DMUL(DEF(reg,DMUL(def,INT)),INT) */
			l->op == 201 && /* DEF */
			l->right->op == 22 && /* DMUL */
			l->right->right->op == 202 && /* INT */
			r->op == 202 /* INT */
		) {
			c = l->left->cost[burm_reg_NT] + l->right->left->cost[burm_def_NT] + 4;
			if (c + 0 < p->cost[burm_expr_NT]) {
				p->cost[burm_expr_NT] = c + 0;
				p->rule.burm_expr = 52;
			}
		}
		if (	/* expr: DMUL(INT,INT) */
			l->op == 202 && /* INT */
			r->op == 202 /* INT */
		) {
			c = 0;
			if (c + 0 < p->cost[burm_expr_NT]) {
				p->cost[burm_expr_NT] = c + 0;
				p->rule.burm_expr = 51;
			}
		}
		{	/* expr: DMUL(def,def) */
			c = l->cost[burm_def_NT] + r->cost[burm_def_NT] + 4;
			if (c + 0 < p->cost[burm_expr_NT]) {
				p->cost[burm_expr_NT] = c + 0;
				p->rule.burm_expr = 8;
			}
		}
		break;
	case 23: /* DSUB */
		assert(l && r);
		{	/* expr: DSUB(def,def) */
			c = l->cost[burm_def_NT] + r->cost[burm_def_NT] + 3;
			if (c + 0 < p->cost[burm_expr_NT]) {
				p->cost[burm_expr_NT] = c + 0;
				p->rule.burm_expr = 10;
			}
		}
		break;
	case 24: /* DMTC1 */
		assert(l);
		{	/* expr: DMTC1(def) */
			c = l->cost[burm_def_NT] + 1;
			if (c + 0 < p->cost[burm_expr_NT]) {
				p->cost[burm_expr_NT] = c + 0;
				p->rule.burm_expr = 17;
			}
		}
		break;
	case 25: /* GOTO */
		{
			static struct burm_state z = { 25, 0, 0,
				{	0,
					32767,
					32767,
					32767,
					32767,
					32767,
					32767,
					32767,
				},{
					0,
					0,
					0,
					0,
					0,
					0,
					0,
				}
			};
			return (STATE_TYPE)&z;
		}
	case 26: /* TRUNCID */
		assert(l);
		{	/* expr: TRUNCID(def) */
			c = l->cost[burm_def_NT] + 1;
			if (c + 0 < p->cost[burm_expr_NT]) {
				p->cost[burm_expr_NT] = c + 0;
				p->rule.burm_expr = 18;
			}
		}
		break;
	case 27: /* MFC1 */
		assert(l);
		{	/* expr: MFC1(def) */
			c = l->cost[burm_def_NT] + 1;
			if (c + 0 < p->cost[burm_expr_NT]) {
				p->cost[burm_expr_NT] = c + 0;
				p->rule.burm_expr = 19;
			}
		}
		break;
	case 28: /* MTC1 */
		assert(l);
		{	/* expr: MTC1(def) */
			c = l->cost[burm_def_NT] + 1;
			if (c + 0 < p->cost[burm_expr_NT]) {
				p->cost[burm_expr_NT] = c + 0;
				p->rule.burm_expr = 21;
			}
		}
		break;
	case 29: /* CVTDI */
		assert(l);
		{	/* expr: CVTDI(def) */
			c = l->cost[burm_def_NT] + 1;
			if (c + 0 < p->cost[burm_expr_NT]) {
				p->cost[burm_expr_NT] = c + 0;
				p->rule.burm_expr = 22;
			}
		}
		break;
	case 30: /* ILAND */
		assert(l && r);
		{	/* expr: ILAND(def,def) */
			c = l->cost[burm_def_NT] + r->cost[burm_def_NT] + 1;
			if (c + 0 < p->cost[burm_expr_NT]) {
				p->cost[burm_expr_NT] = c + 0;
				p->rule.burm_expr = 23;
			}
		}
		break;
	case 31: /* DASSIGN */
		assert(l);
		{	/* expr: DASSIGN(def) */
			c = l->cost[burm_def_NT] + 1;
			if (c + 0 < p->cost[burm_expr_NT]) {
				p->cost[burm_expr_NT] = c + 0;
				p->rule.burm_expr = 24;
			}
		}
		break;
	case 32: /* DGE */
		assert(l && r);
		{	/* expr: DGE(def,def) */
			c = l->cost[burm_def_NT] + r->cost[burm_def_NT] + 1;
			if (c + 0 < p->cost[burm_expr_NT]) {
				p->cost[burm_expr_NT] = c + 0;
				p->rule.burm_expr = 25;
			}
		}
		break;
	case 33: /* DLT */
		assert(l && r);
		{	/* expr: DLT(def,def) */
			c = l->cost[burm_def_NT] + r->cost[burm_def_NT] + 1;
			if (c + 0 < p->cost[burm_expr_NT]) {
				p->cost[burm_expr_NT] = c + 0;
				p->rule.burm_expr = 26;
			}
		}
		break;
	case 34: /* DLE */
		assert(l && r);
		{	/* expr: DLE(def,def) */
			c = l->cost[burm_def_NT] + r->cost[burm_def_NT] + 1;
			if (c + 0 < p->cost[burm_expr_NT]) {
				p->cost[burm_expr_NT] = c + 0;
				p->rule.burm_expr = 27;
			}
		}
		break;
	case 35: /* DGT */
		assert(l && r);
		{	/* expr: DGT(def,def) */
			c = l->cost[burm_def_NT] + r->cost[burm_def_NT] + 1;
			if (c + 0 < p->cost[burm_expr_NT]) {
				p->cost[burm_expr_NT] = c + 0;
				p->rule.burm_expr = 28;
			}
		}
		break;
	case 36: /* CVTSD */
		assert(l);
		{	/* expr: CVTSD(def) */
			c = l->cost[burm_def_NT] + 1;
			if (c + 0 < p->cost[burm_expr_NT]) {
				p->cost[burm_expr_NT] = c + 0;
				p->rule.burm_expr = 29;
			}
		}
		break;
	case 37: /* CVTDS */
		assert(l);
		{	/* expr: CVTDS(def) */
			c = l->cost[burm_def_NT] + 1;
			if (c + 0 < p->cost[burm_expr_NT]) {
				p->cost[burm_expr_NT] = c + 0;
				p->rule.burm_expr = 30;
			}
		}
		break;
	case 38: /* DMFC1 */
		assert(l);
		{	/* expr: DMFC1(def) */
			c = l->cost[burm_def_NT] + 1;
			if (c + 0 < p->cost[burm_expr_NT]) {
				p->cost[burm_expr_NT] = c + 0;
				p->rule.burm_expr = 31;
			}
		}
		break;
	case 39: /* DDIV */
		assert(l && r);
		{	/* expr: DDIV(def,def) */
			c = l->cost[burm_def_NT] + r->cost[burm_def_NT] + 3;
			if (c + 0 < p->cost[burm_expr_NT]) {
				p->cost[burm_expr_NT] = c + 0;
				p->rule.burm_expr = 32;
			}
		}
		break;
	case 40: /* DEQ */
		assert(l && r);
		{	/* expr: DEQ(def,def) */
			c = l->cost[burm_def_NT] + r->cost[burm_def_NT] + 2;
			if (c + 0 < p->cost[burm_expr_NT]) {
				p->cost[burm_expr_NT] = c + 0;
				p->rule.burm_expr = 33;
			}
		}
		break;
	case 41: /* FADD */
		assert(l && r);
		{	/* expr: FADD(def,def) */
			c = l->cost[burm_def_NT] + r->cost[burm_def_NT] + 3;
			if (c + 0 < p->cost[burm_expr_NT]) {
				p->cost[burm_expr_NT] = c + 0;
				p->rule.burm_expr = 34;
			}
		}
		break;
	case 42: /* FSUB */
		assert(l && r);
		{	/* expr: FSUB(def,def) */
			c = l->cost[burm_def_NT] + r->cost[burm_def_NT] + 3;
			if (c + 0 < p->cost[burm_expr_NT]) {
				p->cost[burm_expr_NT] = c + 0;
				p->rule.burm_expr = 35;
			}
		}
		break;
	case 43: /* FMUL */
		assert(l && r);
		{	/* expr: FMUL(def,def) */
			c = l->cost[burm_def_NT] + r->cost[burm_def_NT] + 3;
			if (c + 0 < p->cost[burm_expr_NT]) {
				p->cost[burm_expr_NT] = c + 0;
				p->rule.burm_expr = 36;
			}
		}
		break;
	case 44: /* FDIV */
		assert(l && r);
		{	/* expr: FDIV(def,def) */
			c = l->cost[burm_def_NT] + r->cost[burm_def_NT] + 3;
			if (c + 0 < p->cost[burm_expr_NT]) {
				p->cost[burm_expr_NT] = c + 0;
				p->rule.burm_expr = 37;
			}
		}
		break;
	case 45: /* FVLOAD */
		assert(l && r);
		{	/* expr: FVLOAD(def,def) */
			c = l->cost[burm_def_NT] + r->cost[burm_def_NT] + 2;
			if (c + 0 < p->cost[burm_expr_NT]) {
				p->cost[burm_expr_NT] = c + 0;
				p->rule.burm_expr = 40;
			}
		}
		break;
	case 46: /* FVSTORE */
		assert(l && r);
		{	/* expr: FVSTORE(def,def) */
			c = l->cost[burm_def_NT] + r->cost[burm_def_NT] + 2;
			if (c + 0 < p->cost[burm_expr_NT]) {
				p->cost[burm_expr_NT] = c + 0;
				p->rule.burm_expr = 41;
			}
		}
		break;
	case 47: /* FCONSTANT */
		assert(l);
		{	/* expr: FCONSTANT(const) */
			c = l->cost[burm_const_NT] + 1;
			if (c + 0 < p->cost[burm_expr_NT]) {
				p->cost[burm_expr_NT] = c + 0;
				p->rule.burm_expr = 42;
			}
		}
		break;
	case 48: /* CVTSI */
		assert(l);
		{	/* expr: CVTSI(def) */
			c = l->cost[burm_def_NT] + 1;
			if (c + 0 < p->cost[burm_expr_NT]) {
				p->cost[burm_expr_NT] = c + 0;
				p->rule.burm_expr = 43;
			}
		}
		break;
	case 49: /* TRUNCIS */
		assert(l);
		{	/* expr: TRUNCIS(def) */
			c = l->cost[burm_def_NT] + 1;
			if (c + 0 < p->cost[burm_expr_NT]) {
				p->cost[burm_expr_NT] = c + 0;
				p->rule.burm_expr = 44;
			}
		}
		break;
	case 50: /* INE */
		assert(l && r);
		{	/* expr: INE(def,def) */
			c = l->cost[burm_def_NT] + r->cost[burm_def_NT] + 1;
			if (c + 0 < p->cost[burm_expr_NT]) {
				p->cost[burm_expr_NT] = c + 0;
				p->rule.burm_expr = 45;
			}
		}
		break;
	case 101: /* R0 */
		{
			static struct burm_state z = { 101, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf1 */
					32767,
					32767,
					0,	/* rf1: R0 */
					32767,
					32767,
				},{
					2,	/* def: reg */
					1,	/* reg: rf1 */
					0,
					0,
					1,	/* rf1: R0 */
					0,
					0,
				}
			};
			return (STATE_TYPE)&z;
		}
	case 102: /* R1 */
		{
			static struct burm_state z = { 102, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf1 */
					32767,
					32767,
					0,	/* rf1: R1 */
					32767,
					32767,
				},{
					2,	/* def: reg */
					1,	/* reg: rf1 */
					0,
					0,
					2,	/* rf1: R1 */
					0,
					0,
				}
			};
			return (STATE_TYPE)&z;
		}
	case 103: /* R2 */
		{
			static struct burm_state z = { 103, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf1 */
					32767,
					32767,
					0,	/* rf1: R2 */
					32767,
					32767,
				},{
					2,	/* def: reg */
					1,	/* reg: rf1 */
					0,
					0,
					3,	/* rf1: R2 */
					0,
					0,
				}
			};
			return (STATE_TYPE)&z;
		}
	case 104: /* R3 */
		{
			static struct burm_state z = { 104, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf1 */
					32767,
					32767,
					0,	/* rf1: R3 */
					32767,
					32767,
				},{
					2,	/* def: reg */
					1,	/* reg: rf1 */
					0,
					0,
					4,	/* rf1: R3 */
					0,
					0,
				}
			};
			return (STATE_TYPE)&z;
		}
	case 105: /* R4 */
		{
			static struct burm_state z = { 105, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf1 */
					32767,
					32767,
					0,	/* rf1: R4 */
					32767,
					32767,
				},{
					2,	/* def: reg */
					1,	/* reg: rf1 */
					0,
					0,
					5,	/* rf1: R4 */
					0,
					0,
				}
			};
			return (STATE_TYPE)&z;
		}
	case 106: /* R5 */
		{
			static struct burm_state z = { 106, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf1 */
					32767,
					32767,
					0,	/* rf1: R5 */
					32767,
					32767,
				},{
					2,	/* def: reg */
					1,	/* reg: rf1 */
					0,
					0,
					6,	/* rf1: R5 */
					0,
					0,
				}
			};
			return (STATE_TYPE)&z;
		}
	case 107: /* R6 */
		{
			static struct burm_state z = { 107, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf1 */
					32767,
					32767,
					0,	/* rf1: R6 */
					32767,
					32767,
				},{
					2,	/* def: reg */
					1,	/* reg: rf1 */
					0,
					0,
					7,	/* rf1: R6 */
					0,
					0,
				}
			};
			return (STATE_TYPE)&z;
		}
	case 108: /* R7 */
		{
			static struct burm_state z = { 108, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf1 */
					32767,
					32767,
					0,	/* rf1: R7 */
					32767,
					32767,
				},{
					2,	/* def: reg */
					1,	/* reg: rf1 */
					0,
					0,
					8,	/* rf1: R7 */
					0,
					0,
				}
			};
			return (STATE_TYPE)&z;
		}
	case 109: /* R8 */
		{
			static struct burm_state z = { 109, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf2 */
					32767,
					32767,
					32767,
					0,	/* rf2: R8 */
					32767,
				},{
					2,	/* def: reg */
					2,	/* reg: rf2 */
					0,
					0,
					0,
					1,	/* rf2: R8 */
					0,
				}
			};
			return (STATE_TYPE)&z;
		}
	case 110: /* R9 */
		{
			static struct burm_state z = { 110, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf2 */
					32767,
					32767,
					32767,
					0,	/* rf2: R9 */
					32767,
				},{
					2,	/* def: reg */
					2,	/* reg: rf2 */
					0,
					0,
					0,
					2,	/* rf2: R9 */
					0,
				}
			};
			return (STATE_TYPE)&z;
		}
	case 111: /* R10 */
		{
			static struct burm_state z = { 111, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf2 */
					32767,
					32767,
					32767,
					0,	/* rf2: R10 */
					32767,
				},{
					2,	/* def: reg */
					2,	/* reg: rf2 */
					0,
					0,
					0,
					3,	/* rf2: R10 */
					0,
				}
			};
			return (STATE_TYPE)&z;
		}
	case 112: /* R11 */
		{
			static struct burm_state z = { 112, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf2 */
					32767,
					32767,
					32767,
					0,	/* rf2: R11 */
					32767,
				},{
					2,	/* def: reg */
					2,	/* reg: rf2 */
					0,
					0,
					0,
					4,	/* rf2: R11 */
					0,
				}
			};
			return (STATE_TYPE)&z;
		}
	case 113: /* R12 */
		{
			static struct burm_state z = { 113, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf2 */
					32767,
					32767,
					32767,
					0,	/* rf2: R12 */
					32767,
				},{
					2,	/* def: reg */
					2,	/* reg: rf2 */
					0,
					0,
					0,
					5,	/* rf2: R12 */
					0,
				}
			};
			return (STATE_TYPE)&z;
		}
	case 114: /* R13 */
		{
			static struct burm_state z = { 114, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf2 */
					32767,
					32767,
					32767,
					0,	/* rf2: R13 */
					32767,
				},{
					2,	/* def: reg */
					2,	/* reg: rf2 */
					0,
					0,
					0,
					6,	/* rf2: R13 */
					0,
				}
			};
			return (STATE_TYPE)&z;
		}
	case 115: /* R14 */
		{
			static struct burm_state z = { 115, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf2 */
					32767,
					32767,
					32767,
					0,	/* rf2: R14 */
					32767,
				},{
					2,	/* def: reg */
					2,	/* reg: rf2 */
					0,
					0,
					0,
					7,	/* rf2: R14 */
					0,
				}
			};
			return (STATE_TYPE)&z;
		}
	case 116: /* R15 */
		{
			static struct burm_state z = { 116, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf2 */
					32767,
					32767,
					32767,
					0,	/* rf2: R15 */
					32767,
				},{
					2,	/* def: reg */
					2,	/* reg: rf2 */
					0,
					0,
					0,
					8,	/* rf2: R15 */
					0,
				}
			};
			return (STATE_TYPE)&z;
		}
	case 117: /* R16 */
		{
			static struct burm_state z = { 117, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf3 */
					32767,
					32767,
					32767,
					32767,
					0,	/* rf3: R16 */
				},{
					2,	/* def: reg */
					3,	/* reg: rf3 */
					0,
					0,
					0,
					0,
					1,	/* rf3: R16 */
				}
			};
			return (STATE_TYPE)&z;
		}
	case 118: /* R17 */
		{
			static struct burm_state z = { 118, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf3 */
					32767,
					32767,
					32767,
					32767,
					0,	/* rf3: R17 */
				},{
					2,	/* def: reg */
					3,	/* reg: rf3 */
					0,
					0,
					0,
					0,
					2,	/* rf3: R17 */
				}
			};
			return (STATE_TYPE)&z;
		}
	case 119: /* R18 */
		{
			static struct burm_state z = { 119, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf3 */
					32767,
					32767,
					32767,
					32767,
					0,	/* rf3: R18 */
				},{
					2,	/* def: reg */
					3,	/* reg: rf3 */
					0,
					0,
					0,
					0,
					3,	/* rf3: R18 */
				}
			};
			return (STATE_TYPE)&z;
		}
	case 120: /* R19 */
		{
			static struct burm_state z = { 120, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf3 */
					32767,
					32767,
					32767,
					32767,
					0,	/* rf3: R19 */
				},{
					2,	/* def: reg */
					3,	/* reg: rf3 */
					0,
					0,
					0,
					0,
					4,	/* rf3: R19 */
				}
			};
			return (STATE_TYPE)&z;
		}
	case 121: /* R20 */
		{
			static struct burm_state z = { 121, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf3 */
					32767,
					32767,
					32767,
					32767,
					0,	/* rf3: R20 */
				},{
					2,	/* def: reg */
					3,	/* reg: rf3 */
					0,
					0,
					0,
					0,
					5,	/* rf3: R20 */
				}
			};
			return (STATE_TYPE)&z;
		}
	case 122: /* R21 */
		{
			static struct burm_state z = { 122, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf3 */
					32767,
					32767,
					32767,
					32767,
					0,	/* rf3: R21 */
				},{
					2,	/* def: reg */
					3,	/* reg: rf3 */
					0,
					0,
					0,
					0,
					6,	/* rf3: R21 */
				}
			};
			return (STATE_TYPE)&z;
		}
	case 123: /* R22 */
		{
			static struct burm_state z = { 123, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf3 */
					32767,
					32767,
					32767,
					32767,
					0,	/* rf3: R22 */
				},{
					2,	/* def: reg */
					3,	/* reg: rf3 */
					0,
					0,
					0,
					0,
					7,	/* rf3: R22 */
				}
			};
			return (STATE_TYPE)&z;
		}
	case 124: /* R23 */
		{
			static struct burm_state z = { 124, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf3 */
					32767,
					32767,
					32767,
					32767,
					0,	/* rf3: R23 */
				},{
					2,	/* def: reg */
					3,	/* reg: rf3 */
					0,
					0,
					0,
					0,
					8,	/* rf3: R23 */
				}
			};
			return (STATE_TYPE)&z;
		}
	case 125: /* R24 */
		{
			static struct burm_state z = { 125, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf3 */
					32767,
					32767,
					32767,
					32767,
					0,	/* rf3: R24 */
				},{
					2,	/* def: reg */
					3,	/* reg: rf3 */
					0,
					0,
					0,
					0,
					9,	/* rf3: R24 */
				}
			};
			return (STATE_TYPE)&z;
		}
	case 126: /* R25 */
		{
			static struct burm_state z = { 126, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf3 */
					32767,
					32767,
					32767,
					32767,
					0,	/* rf3: R25 */
				},{
					2,	/* def: reg */
					3,	/* reg: rf3 */
					0,
					0,
					0,
					0,
					10,	/* rf3: R25 */
				}
			};
			return (STATE_TYPE)&z;
		}
	case 127: /* sp0 */
		{
			static struct burm_state z = { 127, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf3 */
					32767,
					32767,
					32767,
					32767,
					0,	/* rf3: sp0 */
				},{
					2,	/* def: reg */
					3,	/* reg: rf3 */
					0,
					0,
					0,
					0,
					11,	/* rf3: sp0 */
				}
			};
			return (STATE_TYPE)&z;
		}
	case 128: /* cc0 */
		{
			static struct burm_state z = { 128, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf3 */
					32767,
					32767,
					32767,
					32767,
					0,	/* rf3: cc0 */
				},{
					2,	/* def: reg */
					3,	/* reg: rf3 */
					0,
					0,
					0,
					0,
					12,	/* rf3: cc0 */
				}
			};
			return (STATE_TYPE)&z;
		}
	case 129: /* f0 */
		{
			static struct burm_state z = { 129, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf3 */
					32767,
					32767,
					32767,
					32767,
					0,	/* rf3: f0 */
				},{
					2,	/* def: reg */
					3,	/* reg: rf3 */
					0,
					0,
					0,
					0,
					13,	/* rf3: f0 */
				}
			};
			return (STATE_TYPE)&z;
		}
	case 130: /* f1 */
		{
			static struct burm_state z = { 130, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf3 */
					32767,
					32767,
					32767,
					32767,
					0,	/* rf3: f1 */
				},{
					2,	/* def: reg */
					3,	/* reg: rf3 */
					0,
					0,
					0,
					0,
					14,	/* rf3: f1 */
				}
			};
			return (STATE_TYPE)&z;
		}
	case 131: /* f2 */
		{
			static struct burm_state z = { 131, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf3 */
					32767,
					32767,
					32767,
					32767,
					0,	/* rf3: f2 */
				},{
					2,	/* def: reg */
					3,	/* reg: rf3 */
					0,
					0,
					0,
					0,
					15,	/* rf3: f2 */
				}
			};
			return (STATE_TYPE)&z;
		}
	case 132: /* f3 */
		{
			static struct burm_state z = { 132, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf3 */
					32767,
					32767,
					32767,
					32767,
					0,	/* rf3: f3 */
				},{
					2,	/* def: reg */
					3,	/* reg: rf3 */
					0,
					0,
					0,
					0,
					16,	/* rf3: f3 */
				}
			};
			return (STATE_TYPE)&z;
		}
	case 133: /* f4 */
		{
			static struct burm_state z = { 133, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf3 */
					32767,
					32767,
					32767,
					32767,
					0,	/* rf3: f4 */
				},{
					2,	/* def: reg */
					3,	/* reg: rf3 */
					0,
					0,
					0,
					0,
					17,	/* rf3: f4 */
				}
			};
			return (STATE_TYPE)&z;
		}
	case 134: /* f5 */
		{
			static struct burm_state z = { 134, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf3 */
					32767,
					32767,
					32767,
					32767,
					0,	/* rf3: f5 */
				},{
					2,	/* def: reg */
					3,	/* reg: rf3 */
					0,
					0,
					0,
					0,
					18,	/* rf3: f5 */
				}
			};
			return (STATE_TYPE)&z;
		}
	case 135: /* f6 */
		{
			static struct burm_state z = { 135, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf3 */
					32767,
					32767,
					32767,
					32767,
					0,	/* rf3: f6 */
				},{
					2,	/* def: reg */
					3,	/* reg: rf3 */
					0,
					0,
					0,
					0,
					19,	/* rf3: f6 */
				}
			};
			return (STATE_TYPE)&z;
		}
	case 136: /* f7 */
		{
			static struct burm_state z = { 136, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf3 */
					32767,
					32767,
					32767,
					32767,
					0,	/* rf3: f7 */
				},{
					2,	/* def: reg */
					3,	/* reg: rf3 */
					0,
					0,
					0,
					0,
					20,	/* rf3: f7 */
				}
			};
			return (STATE_TYPE)&z;
		}
	case 137: /* f8 */
		{
			static struct burm_state z = { 137, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf3 */
					32767,
					32767,
					32767,
					32767,
					0,	/* rf3: f8 */
				},{
					2,	/* def: reg */
					3,	/* reg: rf3 */
					0,
					0,
					0,
					0,
					21,	/* rf3: f8 */
				}
			};
			return (STATE_TYPE)&z;
		}
	case 138: /* f9 */
		{
			static struct burm_state z = { 138, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf3 */
					32767,
					32767,
					32767,
					32767,
					0,	/* rf3: f9 */
				},{
					2,	/* def: reg */
					3,	/* reg: rf3 */
					0,
					0,
					0,
					0,
					22,	/* rf3: f9 */
				}
			};
			return (STATE_TYPE)&z;
		}
	case 139: /* f10 */
		{
			static struct burm_state z = { 139, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf3 */
					32767,
					32767,
					32767,
					32767,
					0,	/* rf3: f10 */
				},{
					2,	/* def: reg */
					3,	/* reg: rf3 */
					0,
					0,
					0,
					0,
					23,	/* rf3: f10 */
				}
			};
			return (STATE_TYPE)&z;
		}
	case 140: /* f11 */
		{
			static struct burm_state z = { 140, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf3 */
					32767,
					32767,
					32767,
					32767,
					0,	/* rf3: f11 */
				},{
					2,	/* def: reg */
					3,	/* reg: rf3 */
					0,
					0,
					0,
					0,
					24,	/* rf3: f11 */
				}
			};
			return (STATE_TYPE)&z;
		}
	case 141: /* f12 */
		{
			static struct burm_state z = { 141, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf3 */
					32767,
					32767,
					32767,
					32767,
					0,	/* rf3: f12 */
				},{
					2,	/* def: reg */
					3,	/* reg: rf3 */
					0,
					0,
					0,
					0,
					25,	/* rf3: f12 */
				}
			};
			return (STATE_TYPE)&z;
		}
	case 142: /* f13 */
		{
			static struct burm_state z = { 142, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf3 */
					32767,
					32767,
					32767,
					32767,
					0,	/* rf3: f13 */
				},{
					2,	/* def: reg */
					3,	/* reg: rf3 */
					0,
					0,
					0,
					0,
					26,	/* rf3: f13 */
				}
			};
			return (STATE_TYPE)&z;
		}
	case 143: /* f14 */
		{
			static struct burm_state z = { 143, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf3 */
					32767,
					32767,
					32767,
					32767,
					0,	/* rf3: f14 */
				},{
					2,	/* def: reg */
					3,	/* reg: rf3 */
					0,
					0,
					0,
					0,
					27,	/* rf3: f14 */
				}
			};
			return (STATE_TYPE)&z;
		}
	case 144: /* f15 */
		{
			static struct burm_state z = { 144, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf3 */
					32767,
					32767,
					32767,
					32767,
					0,	/* rf3: f15 */
				},{
					2,	/* def: reg */
					3,	/* reg: rf3 */
					0,
					0,
					0,
					0,
					28,	/* rf3: f15 */
				}
			};
			return (STATE_TYPE)&z;
		}
	case 145: /* f16 */
		{
			static struct burm_state z = { 145, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf3 */
					32767,
					32767,
					32767,
					32767,
					0,	/* rf3: f16 */
				},{
					2,	/* def: reg */
					3,	/* reg: rf3 */
					0,
					0,
					0,
					0,
					29,	/* rf3: f16 */
				}
			};
			return (STATE_TYPE)&z;
		}
	case 146: /* f17 */
		{
			static struct burm_state z = { 146, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf3 */
					32767,
					32767,
					32767,
					32767,
					0,	/* rf3: f17 */
				},{
					2,	/* def: reg */
					3,	/* reg: rf3 */
					0,
					0,
					0,
					0,
					30,	/* rf3: f17 */
				}
			};
			return (STATE_TYPE)&z;
		}
	case 147: /* f18 */
		{
			static struct burm_state z = { 147, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf3 */
					32767,
					32767,
					32767,
					32767,
					0,	/* rf3: f18 */
				},{
					2,	/* def: reg */
					3,	/* reg: rf3 */
					0,
					0,
					0,
					0,
					31,	/* rf3: f18 */
				}
			};
			return (STATE_TYPE)&z;
		}
	case 148: /* f19 */
		{
			static struct burm_state z = { 148, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf3 */
					32767,
					32767,
					32767,
					32767,
					0,	/* rf3: f19 */
				},{
					2,	/* def: reg */
					3,	/* reg: rf3 */
					0,
					0,
					0,
					0,
					32,	/* rf3: f19 */
				}
			};
			return (STATE_TYPE)&z;
		}
	case 149: /* f20 */
		{
			static struct burm_state z = { 149, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf3 */
					32767,
					32767,
					32767,
					32767,
					0,	/* rf3: f20 */
				},{
					2,	/* def: reg */
					3,	/* reg: rf3 */
					0,
					0,
					0,
					0,
					33,	/* rf3: f20 */
				}
			};
			return (STATE_TYPE)&z;
		}
	case 150: /* f21 */
		{
			static struct burm_state z = { 150, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf3 */
					32767,
					32767,
					32767,
					32767,
					0,	/* rf3: f21 */
				},{
					2,	/* def: reg */
					3,	/* reg: rf3 */
					0,
					0,
					0,
					0,
					34,	/* rf3: f21 */
				}
			};
			return (STATE_TYPE)&z;
		}
	case 151: /* f22 */
		{
			static struct burm_state z = { 151, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf3 */
					32767,
					32767,
					32767,
					32767,
					0,	/* rf3: f22 */
				},{
					2,	/* def: reg */
					3,	/* reg: rf3 */
					0,
					0,
					0,
					0,
					35,	/* rf3: f22 */
				}
			};
			return (STATE_TYPE)&z;
		}
	case 152: /* f23 */
		{
			static struct burm_state z = { 152, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf3 */
					32767,
					32767,
					32767,
					32767,
					0,	/* rf3: f23 */
				},{
					2,	/* def: reg */
					3,	/* reg: rf3 */
					0,
					0,
					0,
					0,
					36,	/* rf3: f23 */
				}
			};
			return (STATE_TYPE)&z;
		}
	case 153: /* f24 */
		{
			static struct burm_state z = { 153, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf3 */
					32767,
					32767,
					32767,
					32767,
					0,	/* rf3: f24 */
				},{
					2,	/* def: reg */
					3,	/* reg: rf3 */
					0,
					0,
					0,
					0,
					37,	/* rf3: f24 */
				}
			};
			return (STATE_TYPE)&z;
		}
	case 154: /* f25 */
		{
			static struct burm_state z = { 154, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf3 */
					32767,
					32767,
					32767,
					32767,
					0,	/* rf3: f25 */
				},{
					2,	/* def: reg */
					3,	/* reg: rf3 */
					0,
					0,
					0,
					0,
					38,	/* rf3: f25 */
				}
			};
			return (STATE_TYPE)&z;
		}
	case 155: /* f26 */
		{
			static struct burm_state z = { 155, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf3 */
					32767,
					32767,
					32767,
					32767,
					0,	/* rf3: f26 */
				},{
					2,	/* def: reg */
					3,	/* reg: rf3 */
					0,
					0,
					0,
					0,
					39,	/* rf3: f26 */
				}
			};
			return (STATE_TYPE)&z;
		}
	case 156: /* f27 */
		{
			static struct burm_state z = { 156, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf3 */
					32767,
					32767,
					32767,
					32767,
					0,	/* rf3: f27 */
				},{
					2,	/* def: reg */
					3,	/* reg: rf3 */
					0,
					0,
					0,
					0,
					40,	/* rf3: f27 */
				}
			};
			return (STATE_TYPE)&z;
		}
	case 157: /* f28 */
		{
			static struct burm_state z = { 157, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf3 */
					32767,
					32767,
					32767,
					32767,
					0,	/* rf3: f28 */
				},{
					2,	/* def: reg */
					3,	/* reg: rf3 */
					0,
					0,
					0,
					0,
					41,	/* rf3: f28 */
				}
			};
			return (STATE_TYPE)&z;
		}
	case 158: /* f29 */
		{
			static struct burm_state z = { 158, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf3 */
					32767,
					32767,
					32767,
					32767,
					0,	/* rf3: f29 */
				},{
					2,	/* def: reg */
					3,	/* reg: rf3 */
					0,
					0,
					0,
					0,
					42,	/* rf3: f29 */
				}
			};
			return (STATE_TYPE)&z;
		}
	case 159: /* f30 */
		{
			static struct burm_state z = { 159, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf3 */
					32767,
					32767,
					32767,
					32767,
					0,	/* rf3: f30 */
				},{
					2,	/* def: reg */
					3,	/* reg: rf3 */
					0,
					0,
					0,
					0,
					43,	/* rf3: f30 */
				}
			};
			return (STATE_TYPE)&z;
		}
	case 160: /* f31 */
		{
			static struct burm_state z = { 160, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf3 */
					32767,
					32767,
					32767,
					32767,
					0,	/* rf3: f31 */
				},{
					2,	/* def: reg */
					3,	/* reg: rf3 */
					0,
					0,
					0,
					0,
					44,	/* rf3: f31 */
				}
			};
			return (STATE_TYPE)&z;
		}
	case 161: /* fp0 */
		{
			static struct burm_state z = { 161, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf3 */
					32767,
					32767,
					32767,
					32767,
					0,	/* rf3: fp0 */
				},{
					2,	/* def: reg */
					3,	/* reg: rf3 */
					0,
					0,
					0,
					0,
					45,	/* rf3: fp0 */
				}
			};
			return (STATE_TYPE)&z;
		}
	case 162: /* temp0 */
		{
			static struct burm_state z = { 162, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf3 */
					32767,
					32767,
					32767,
					32767,
					0,	/* rf3: temp0 */
				},{
					2,	/* def: reg */
					3,	/* reg: rf3 */
					0,
					0,
					0,
					0,
					46,	/* rf3: temp0 */
				}
			};
			return (STATE_TYPE)&z;
		}
	case 163: /* temp1 */
		{
			static struct burm_state z = { 163, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf3 */
					32767,
					32767,
					32767,
					32767,
					0,	/* rf3: temp1 */
				},{
					2,	/* def: reg */
					3,	/* reg: rf3 */
					0,
					0,
					0,
					0,
					47,	/* rf3: temp1 */
				}
			};
			return (STATE_TYPE)&z;
		}
	case 164: /* temp2 */
		{
			static struct burm_state z = { 164, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf3 */
					32767,
					32767,
					32767,
					32767,
					0,	/* rf3: temp2 */
				},{
					2,	/* def: reg */
					3,	/* reg: rf3 */
					0,
					0,
					0,
					0,
					48,	/* rf3: temp2 */
				}
			};
			return (STATE_TYPE)&z;
		}
	case 165: /* temp3 */
		{
			static struct burm_state z = { 165, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf3 */
					32767,
					32767,
					32767,
					32767,
					0,	/* rf3: temp3 */
				},{
					2,	/* def: reg */
					3,	/* reg: rf3 */
					0,
					0,
					0,
					0,
					49,	/* rf3: temp3 */
				}
			};
			return (STATE_TYPE)&z;
		}
	case 166: /* temp4 */
		{
			static struct burm_state z = { 166, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf3 */
					32767,
					32767,
					32767,
					32767,
					0,	/* rf3: temp4 */
				},{
					2,	/* def: reg */
					3,	/* reg: rf3 */
					0,
					0,
					0,
					0,
					50,	/* rf3: temp4 */
				}
			};
			return (STATE_TYPE)&z;
		}
	case 167: /* temp5 */
		{
			static struct burm_state z = { 167, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf3 */
					32767,
					32767,
					32767,
					32767,
					0,	/* rf3: temp5 */
				},{
					2,	/* def: reg */
					3,	/* reg: rf3 */
					0,
					0,
					0,
					0,
					51,	/* rf3: temp5 */
				}
			};
			return (STATE_TYPE)&z;
		}
	case 168: /* temp6 */
		{
			static struct burm_state z = { 168, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf3 */
					32767,
					32767,
					32767,
					32767,
					0,	/* rf3: temp6 */
				},{
					2,	/* def: reg */
					3,	/* reg: rf3 */
					0,
					0,
					0,
					0,
					52,	/* rf3: temp6 */
				}
			};
			return (STATE_TYPE)&z;
		}
	case 169: /* temp7 */
		{
			static struct burm_state z = { 169, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf3 */
					32767,
					32767,
					32767,
					32767,
					0,	/* rf3: temp7 */
				},{
					2,	/* def: reg */
					3,	/* reg: rf3 */
					0,
					0,
					0,
					0,
					53,	/* rf3: temp7 */
				}
			};
			return (STATE_TYPE)&z;
		}
	case 170: /* temp8 */
		{
			static struct burm_state z = { 170, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf3 */
					32767,
					32767,
					32767,
					32767,
					0,	/* rf3: temp8 */
				},{
					2,	/* def: reg */
					3,	/* reg: rf3 */
					0,
					0,
					0,
					0,
					54,	/* rf3: temp8 */
				}
			};
			return (STATE_TYPE)&z;
		}
	case 171: /* temp9 */
		{
			static struct burm_state z = { 171, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf3 */
					32767,
					32767,
					32767,
					32767,
					0,	/* rf3: temp9 */
				},{
					2,	/* def: reg */
					3,	/* reg: rf3 */
					0,
					0,
					0,
					0,
					55,	/* rf3: temp9 */
				}
			};
			return (STATE_TYPE)&z;
		}
	case 172: /* temp10 */
		{
			static struct burm_state z = { 172, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf3 */
					32767,
					32767,
					32767,
					32767,
					0,	/* rf3: temp10 */
				},{
					2,	/* def: reg */
					3,	/* reg: rf3 */
					0,
					0,
					0,
					0,
					56,	/* rf3: temp10 */
				}
			};
			return (STATE_TYPE)&z;
		}
	case 173: /* temp11 */
		{
			static struct burm_state z = { 173, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf3 */
					32767,
					32767,
					32767,
					32767,
					0,	/* rf3: temp11 */
				},{
					2,	/* def: reg */
					3,	/* reg: rf3 */
					0,
					0,
					0,
					0,
					57,	/* rf3: temp11 */
				}
			};
			return (STATE_TYPE)&z;
		}
	case 174: /* temp12 */
		{
			static struct burm_state z = { 174, 0, 0,
				{	0,
					0,	/* def: reg */
					0,	/* reg: rf3 */
					32767,
					32767,
					32767,
					32767,
					0,	/* rf3: temp12 */
				},{
					2,	/* def: reg */
					3,	/* reg: rf3 */
					0,
					0,
					0,
					0,
					58,	/* rf3: temp12 */
				}
			};
			return (STATE_TYPE)&z;
		}
	case 201: /* DEF */
		assert(l && r);
		if (	/* def: DEF(reg,DADD(def,DEF(reg,DADD(DEF(reg,expr),def)))) */
			r->op == 20 && /* DADD */
			r->right->op == 201 && /* DEF */
			r->right->right->op == 20 && /* DADD */
			r->right->right->left->op == 201 /* DEF */
		) {
			c = l->cost[burm_reg_NT] + r->left->cost[burm_def_NT] + r->right->left->cost[burm_reg_NT] + r->right->right->left->left->cost[burm_reg_NT] + r->right->right->left->right->cost[burm_expr_NT] + r->right->right->right->cost[burm_def_NT] + 3;
			if (c + 0 < p->cost[burm_def_NT]) {
				p->cost[burm_def_NT] = c + 0;
				p->rule.burm_def = 19;
			}
		}
		if (	/* def: DEF(reg,DADD(DEF(reg,DADD(def,DEF(reg,expr))),def)) */
			r->op == 20 && /* DADD */
			r->left->op == 201 && /* DEF */
			r->left->right->op == 20 && /* DADD */
			r->left->right->right->op == 201 /* DEF */
		) {
			c = l->cost[burm_reg_NT] + r->left->left->cost[burm_reg_NT] + r->left->right->left->cost[burm_def_NT] + r->left->right->right->left->cost[burm_reg_NT] + r->left->right->right->right->cost[burm_expr_NT] + r->right->cost[burm_def_NT] + 3;
			if (c + 0 < p->cost[burm_def_NT]) {
				p->cost[burm_def_NT] = c + 0;
				p->rule.burm_def = 18;
			}
		}
		if (	/* def: DEF(reg,DADD(def,DEF(reg,DADD(def,DEF(reg,expr))))) */
			r->op == 20 && /* DADD */
			r->right->op == 201 && /* DEF */
			r->right->right->op == 20 && /* DADD */
			r->right->right->right->op == 201 /* DEF */
		) {
			c = l->cost[burm_reg_NT] + r->left->cost[burm_def_NT] + r->right->left->cost[burm_reg_NT] + r->right->right->left->cost[burm_def_NT] + r->right->right->right->left->cost[burm_reg_NT] + r->right->right->right->right->cost[burm_expr_NT] + 3;
			if (c + 0 < p->cost[burm_def_NT]) {
				p->cost[burm_def_NT] = c + 0;
				p->rule.burm_def = 17;
			}
		}
		if (	/* def: DEF(reg,DADD(DEF(reg,DADD(DEF(reg,expr),def)),def)) */
			r->op == 20 && /* DADD */
			r->left->op == 201 && /* DEF */
			r->left->right->op == 20 && /* DADD */
			r->left->right->left->op == 201 /* DEF */
		) {
			c = l->cost[burm_reg_NT] + r->left->left->cost[burm_reg_NT] + r->left->right->left->left->cost[burm_reg_NT] + r->left->right->left->right->cost[burm_expr_NT] + r->left->right->right->cost[burm_def_NT] + r->right->cost[burm_def_NT] + 3;
			if (c + 0 < p->cost[burm_def_NT]) {
				p->cost[burm_def_NT] = c + 0;
				p->rule.burm_def = 16;
			}
		}
		if (	/* def: DEF(reg,IADD(def,DEF(reg,IADD(DEF(reg,expr),def)))) */
			r->op == 1 && /* IADD */
			r->right->op == 201 && /* DEF */
			r->right->right->op == 1 && /* IADD */
			r->right->right->left->op == 201 /* DEF */
		) {
			c = l->cost[burm_reg_NT] + r->left->cost[burm_def_NT] + r->right->left->cost[burm_reg_NT] + r->right->right->left->left->cost[burm_reg_NT] + r->right->right->left->right->cost[burm_expr_NT] + r->right->right->right->cost[burm_def_NT] + 3;
			if (c + 0 < p->cost[burm_def_NT]) {
				p->cost[burm_def_NT] = c + 0;
				p->rule.burm_def = 15;
			}
		}
		if (	/* def: DEF(reg,IADD(DEF(reg,IADD(def,DEF(reg,expr))),def)) */
			r->op == 1 && /* IADD */
			r->left->op == 201 && /* DEF */
			r->left->right->op == 1 && /* IADD */
			r->left->right->right->op == 201 /* DEF */
		) {
			c = l->cost[burm_reg_NT] + r->left->left->cost[burm_reg_NT] + r->left->right->left->cost[burm_def_NT] + r->left->right->right->left->cost[burm_reg_NT] + r->left->right->right->right->cost[burm_expr_NT] + r->right->cost[burm_def_NT] + 3;
			if (c + 0 < p->cost[burm_def_NT]) {
				p->cost[burm_def_NT] = c + 0;
				p->rule.burm_def = 14;
			}
		}
		if (	/* def: DEF(reg,ISUB(def,DEF(reg,ISUB(def,DEF(reg,expr))))) */
			r->op == 6 && /* ISUB */
			r->right->op == 201 && /* DEF */
			r->right->right->op == 6 && /* ISUB */
			r->right->right->right->op == 201 /* DEF */
		) {
			c = l->cost[burm_reg_NT] + r->left->cost[burm_def_NT] + r->right->left->cost[burm_reg_NT] + r->right->right->left->cost[burm_def_NT] + r->right->right->right->left->cost[burm_reg_NT] + r->right->right->right->right->cost[burm_expr_NT] + 3;
			if (c + 0 < p->cost[burm_def_NT]) {
				p->cost[burm_def_NT] = c + 0;
				p->rule.burm_def = 13;
			}
		}
		if (	/* def: DEF(reg,ISUB(DEF(reg,ISUB(DEF(reg,expr),def)),def)) */
			r->op == 6 && /* ISUB */
			r->left->op == 201 && /* DEF */
			r->left->right->op == 6 && /* ISUB */
			r->left->right->left->op == 201 /* DEF */
		) {
			c = l->cost[burm_reg_NT] + r->left->left->cost[burm_reg_NT] + r->left->right->left->left->cost[burm_reg_NT] + r->left->right->left->right->cost[burm_expr_NT] + r->left->right->right->cost[burm_def_NT] + r->right->cost[burm_def_NT] + 3;
			if (c + 0 < p->cost[burm_def_NT]) {
				p->cost[burm_def_NT] = c + 0;
				p->rule.burm_def = 12;
			}
		}
		if (	/* def: DEF(reg,DMUL(def,DEF(reg,DMUL(def,DEF(reg,expr))))) */
			r->op == 22 && /* DMUL */
			r->right->op == 201 && /* DEF */
			r->right->right->op == 22 && /* DMUL */
			r->right->right->right->op == 201 /* DEF */
		) {
			c = l->cost[burm_reg_NT] + r->left->cost[burm_def_NT] + r->right->left->cost[burm_reg_NT] + r->right->right->left->cost[burm_def_NT] + r->right->right->right->left->cost[burm_reg_NT] + r->right->right->right->right->cost[burm_expr_NT] + 3;
			if (c + 0 < p->cost[burm_def_NT]) {
				p->cost[burm_def_NT] = c + 0;
				p->rule.burm_def = 11;
			}
		}
		if (	/* def: DEF(reg,DMUL(DEF(reg,DMUL(DEF(reg,expr),def)),def)) */
			r->op == 22 && /* DMUL */
			r->left->op == 201 && /* DEF */
			r->left->right->op == 22 && /* DMUL */
			r->left->right->left->op == 201 /* DEF */
		) {
			c = l->cost[burm_reg_NT] + r->left->left->cost[burm_reg_NT] + r->left->right->left->left->cost[burm_reg_NT] + r->left->right->left->right->cost[burm_expr_NT] + r->left->right->right->cost[burm_def_NT] + r->right->cost[burm_def_NT] + 3;
			if (c + 0 < p->cost[burm_def_NT]) {
				p->cost[burm_def_NT] = c + 0;
				p->rule.burm_def = 10;
			}
		}
		if (	/* def: DEF(reg,IADD(def,DEF(reg,IADD(def,DEF(reg,expr))))) */
			r->op == 1 && /* IADD */
			r->right->op == 201 && /* DEF */
			r->right->right->op == 1 && /* IADD */
			r->right->right->right->op == 201 /* DEF */
		) {
			c = l->cost[burm_reg_NT] + r->left->cost[burm_def_NT] + r->right->left->cost[burm_reg_NT] + r->right->right->left->cost[burm_def_NT] + r->right->right->right->left->cost[burm_reg_NT] + r->right->right->right->right->cost[burm_expr_NT] + 3;
			if (c + 0 < p->cost[burm_def_NT]) {
				p->cost[burm_def_NT] = c + 0;
				p->rule.burm_def = 9;
			}
		}
		if (	/* def: DEF(reg,IADD(DEF(reg,IADD(DEF(reg,expr),def)),def)) */
			r->op == 1 && /* IADD */
			r->left->op == 201 && /* DEF */
			r->left->right->op == 1 && /* IADD */
			r->left->right->left->op == 201 /* DEF */
		) {
			c = l->cost[burm_reg_NT] + r->left->left->cost[burm_reg_NT] + r->left->right->left->left->cost[burm_reg_NT] + r->left->right->left->right->cost[burm_expr_NT] + r->left->right->right->cost[burm_def_NT] + r->right->cost[burm_def_NT] + 3;
			if (c + 0 < p->cost[burm_def_NT]) {
				p->cost[burm_def_NT] = c + 0;
				p->rule.burm_def = 8;
			}
		}
		if (	/* def: DEF(reg,ASSIGN(def)) */
			r->op == 18 /* ASSIGN */
		) {
			c = l->cost[burm_reg_NT] + r->left->cost[burm_def_NT] + 0;
			if (c + 0 < p->cost[burm_def_NT]) {
				p->cost[burm_def_NT] = c + 0;
				p->rule.burm_def = 7;
			}
		}
		if (	/* def: DEF(reg,DCONSTANT(const)) */
			r->op == 19 /* DCONSTANT */
		) {
			c = l->cost[burm_reg_NT] + r->left->cost[burm_const_NT] + 0;
			if (c + 0 < p->cost[burm_def_NT]) {
				p->cost[burm_def_NT] = c + 0;
				p->rule.burm_def = 6;
			}
		}
		if (	/* def: DEF(reg,IASSIGN(def)) */
			r->op == 11 /* IASSIGN */
		) {
			c = l->cost[burm_reg_NT] + r->left->cost[burm_def_NT] + 0;
			if (c + 0 < p->cost[burm_def_NT]) {
				p->cost[burm_def_NT] = c + 0;
				p->rule.burm_def = 5;
			}
		}
		if (	/* def: DEF(reg,ICONSTANT(const)) */
			r->op == 10 /* ICONSTANT */
		) {
			c = l->cost[burm_reg_NT] + r->left->cost[burm_const_NT] + 0;
			if (c + 0 < p->cost[burm_def_NT]) {
				p->cost[burm_def_NT] = c + 0;
				p->rule.burm_def = 4;
			}
		}
		{	/* def: DEF(reg,expr) */
			c = l->cost[burm_reg_NT] + r->cost[burm_expr_NT] + 0;
			if (c + 0 < p->cost[burm_def_NT]) {
				p->cost[burm_def_NT] = c + 0;
				p->rule.burm_def = 1;
			}
		}
		break;
	case 202: /* INT */
		{
			static struct burm_state z = { 202, 0, 0,
				{	0,
					0,	/* def: const */
					32767,
					32767,
					0,	/* const: INT */
					32767,
					32767,
					32767,
				},{
					3,	/* def: const */
					0,
					0,
					1,	/* const: INT */
					0,
					0,
					0,
				}
			};
			return (STATE_TYPE)&z;
		}
	case 203: /* IND */
		{
			static struct burm_state z = { 203, 0, 0,
				{	0,
					32767,
					32767,
					32767,
					32767,
					32767,
					32767,
					32767,
				},{
					0,
					0,
					0,
					0,
					0,
					0,
					0,
				}
			};
			return (STATE_TYPE)&z;
		}
	case 204: /* SRC2 */
		{
			static struct burm_state z = { 204, 0, 0,
				{	0,
					32767,
					32767,
					32767,
					32767,
					32767,
					32767,
					32767,
				},{
					0,
					0,
					0,
					0,
					0,
					0,
					0,
				}
			};
			return (STATE_TYPE)&z;
		}
	case 205: /* FLOAT */
		{
			static struct burm_state z = { 205, 0, 0,
				{	0,
					0,	/* def: const */
					32767,
					32767,
					0,	/* const: FLOAT */
					32767,
					32767,
					32767,
				},{
					3,	/* def: const */
					0,
					0,
					2,	/* const: FLOAT */
					0,
					0,
					0,
				}
			};
			return (STATE_TYPE)&z;
		}
	case 400: /* NONE */
		{
			static struct burm_state z = { 400, 0, 0,
				{	0,
					32767,
					32767,
					32767,
					32767,
					32767,
					32767,
					32767,
				},{
					0,
					0,
					0,
					0,
					0,
					0,
					0,
				}
			};
			return (STATE_TYPE)&z;
		}
	case 401: /* ALU1 */
		{
			static struct burm_state z = { 401, 0, 0,
				{	0,
					32767,
					32767,
					32767,
					32767,
					32767,
					32767,
					32767,
				},{
					0,
					0,
					0,
					0,
					0,
					0,
					0,
				}
			};
			return (STATE_TYPE)&z;
		}
	case 402: /* ALU2 */
		{
			static struct burm_state z = { 402, 0, 0,
				{	0,
					32767,
					32767,
					32767,
					32767,
					32767,
					32767,
					32767,
				},{
					0,
					0,
					0,
					0,
					0,
					0,
					0,
				}
			};
			return (STATE_TYPE)&z;
		}
	default:
		burm_assert(0, PANIC("Bad operator %d in burm_state\n", op));
	}
	return (STATE_TYPE)p;
}

#ifdef STATE_LABEL
static void burm_label1(NODEPTR_TYPE p) {
	burm_assert(p, PANIC("NULL tree in burm_label\n"));
	switch (burm_arity[OP_LABEL(p)]) {
	case 0:
		STATE_LABEL(p) = burm_state(OP_LABEL(p), 0, 0);
		break;
	case 1:
		burm_label1(LEFT_CHILD(p));
		STATE_LABEL(p) = burm_state(OP_LABEL(p),
			STATE_LABEL(LEFT_CHILD(p)), 0);
		break;
	case 2:
		burm_label1(LEFT_CHILD(p));
		burm_label1(RIGHT_CHILD(p));
		STATE_LABEL(p) = burm_state(OP_LABEL(p),
			STATE_LABEL(LEFT_CHILD(p)),
			STATE_LABEL(RIGHT_CHILD(p)));
		break;
	}
}

STATE_TYPE burm_label(NODEPTR_TYPE p) {
	burm_label1(p);
	return ((struct burm_state *)STATE_LABEL(p))->rule.burm_def ? STATE_LABEL(p) : 0;
}

NODEPTR_TYPE *burm_kids(NODEPTR_TYPE p, int eruleno, NODEPTR_TYPE kids[]) {
	burm_assert(p, PANIC("NULL tree in burm_kids\n"));
	burm_assert(kids, PANIC("NULL kids in burm_kids\n"));
	switch (eruleno) {
	case 49: /* expr: INE(def,def) */
	case 45: /* expr: FVSTORE(def,def) */
	case 44: /* expr: FVLOAD(def,def) */
	case 43: /* expr: DVSTORE(def,def) */
	case 42: /* expr: DVLOAD(def,def) */
	case 41: /* expr: FDIV(def,def) */
	case 40: /* expr: FMUL(def,def) */
	case 39: /* expr: FSUB(def,def) */
	case 38: /* expr: FADD(def,def) */
	case 37: /* expr: DEQ(def,def) */
	case 36: /* expr: DDIV(def,def) */
	case 32: /* expr: DGT(def,def) */
	case 31: /* expr: DLE(def,def) */
	case 30: /* expr: DLT(def,def) */
	case 29: /* expr: DGE(def,def) */
	case 27: /* expr: ILAND(def,def) */
	case 20: /* expr: IGT(def,def) */
	case 19: /* expr: ILE(def,def) */
	case 18: /* expr: ILT(def,def) */
	case 17: /* expr: IGE(def,def) */
	case 14: /* expr: DSUB(def,def) */
	case 13: /* expr: DADD(def,def) */
	case 12: /* expr: DMUL(def,def) */
	case 11: /* expr: IASH(def,const) */
	case 10: /* expr: ILSH(def,const) */
	case 8: /* expr: IVSTORE(def,def) */
	case 7: /* expr: IVLOAD(def,def) */
	case 6: /* expr: ISUB(def,def) */
	case 5: /* expr: IADD(def,def) */
	case 1: /* def: DEF(reg,expr) */
		kids[0] = LEFT_CHILD(p);
		kids[1] = RIGHT_CHILD(p);
		break;
	case 142: /* reg: rf3 */
	case 115: /* reg: rf2 */
	case 114: /* reg: rf1 */
	case 3: /* def: const */
	case 2: /* def: reg */
		kids[0] = p;
		break;
	case 220: /* expr: DMUL(reg,INT) */
	case 203: /* expr: IASSIGN(const) */
	case 48: /* expr: TRUNCIS(def) */
	case 47: /* expr: CVTSI(def) */
	case 46: /* expr: FCONSTANT(const) */
	case 35: /* expr: DMFC1(def) */
	case 34: /* expr: CVTDS(def) */
	case 33: /* expr: CVTSD(def) */
	case 28: /* expr: DASSIGN(def) */
	case 26: /* expr: CVTDI(def) */
	case 25: /* expr: MTC1(def) */
	case 24: /* expr: DCONSTANT(const) */
	case 23: /* expr: MFC1(def) */
	case 22: /* expr: TRUNCID(def) */
	case 21: /* expr: DMTC1(def) */
	case 16: /* expr: ASSIGN(def) */
	case 15: /* expr: ICONSTANT(const) */
	case 9: /* expr: IASSIGN(def) */
		kids[0] = LEFT_CHILD(p);
		break;
	case 209: /* expr: DMUL(INT,INT) */
	case 208: /* expr: ISUB(INT,INT) */
	case 207: /* expr: IADD(INT,INT) */
	case 192: /* const: FLOAT */
	case 191: /* rf3: temp12 */
	case 190: /* rf3: temp11 */
	case 189: /* rf3: temp10 */
	case 188: /* rf3: temp9 */
	case 187: /* rf3: temp8 */
	case 186: /* rf3: temp7 */
	case 185: /* rf3: temp6 */
	case 184: /* rf3: temp5 */
	case 183: /* rf3: temp4 */
	case 182: /* rf3: temp3 */
	case 181: /* rf3: temp2 */
	case 180: /* rf3: temp1 */
	case 179: /* rf3: temp0 */
	case 178: /* const: INT */
	case 177: /* rf3: fp0 */
	case 176: /* rf3: f31 */
	case 175: /* rf3: f30 */
	case 174: /* rf3: f29 */
	case 173: /* rf3: f28 */
	case 172: /* rf3: f27 */
	case 171: /* rf3: f26 */
	case 170: /* rf3: f25 */
	case 169: /* rf3: f24 */
	case 168: /* rf3: f23 */
	case 167: /* rf3: f22 */
	case 166: /* rf3: f21 */
	case 165: /* rf3: f20 */
	case 164: /* rf3: f19 */
	case 163: /* rf3: f18 */
	case 162: /* rf3: f17 */
	case 161: /* rf3: f16 */
	case 160: /* rf3: f15 */
	case 159: /* rf3: f14 */
	case 158: /* rf3: f13 */
	case 157: /* rf3: f12 */
	case 156: /* rf3: f11 */
	case 155: /* rf3: f10 */
	case 154: /* rf3: f9 */
	case 153: /* rf3: f8 */
	case 152: /* rf3: f7 */
	case 151: /* rf3: f6 */
	case 150: /* rf3: f5 */
	case 149: /* rf3: f4 */
	case 148: /* rf3: f3 */
	case 147: /* rf3: f2 */
	case 146: /* rf3: f1 */
	case 145: /* rf3: f0 */
	case 144: /* rf3: cc0 */
	case 143: /* rf3: sp0 */
	case 141: /* rf3: R25 */
	case 140: /* rf3: R24 */
	case 139: /* rf3: R23 */
	case 138: /* rf3: R22 */
	case 137: /* rf3: R21 */
	case 136: /* rf3: R20 */
	case 135: /* rf3: R19 */
	case 134: /* rf3: R18 */
	case 133: /* rf3: R17 */
	case 132: /* rf3: R16 */
	case 131: /* rf2: R15 */
	case 130: /* rf2: R14 */
	case 129: /* rf2: R13 */
	case 128: /* rf2: R12 */
	case 127: /* rf2: R11 */
	case 126: /* rf2: R10 */
	case 125: /* rf2: R9 */
	case 124: /* rf2: R8 */
	case 123: /* rf1: R7 */
	case 122: /* rf1: R6 */
	case 121: /* rf1: R5 */
	case 120: /* rf1: R4 */
	case 119: /* rf1: R3 */
	case 118: /* rf1: R2 */
	case 117: /* rf1: R1 */
	case 116: /* rf1: R0 */
		break;
	case 237: /* expr: DMUL(DEF(reg,DADD(def,def)),def) */
	case 201: /* expr: DADD(DEF(reg,DMUL(def,def)),def) */
		kids[0] = LEFT_CHILD(LEFT_CHILD(p));
		kids[1] = LEFT_CHILD(RIGHT_CHILD(LEFT_CHILD(p)));
		kids[2] = RIGHT_CHILD(RIGHT_CHILD(LEFT_CHILD(p)));
		kids[3] = RIGHT_CHILD(p);
		break;
	case 238: /* expr: DMUL(def,DEF(reg,DADD(def,def))) */
	case 202: /* expr: DADD(def,DEF(reg,DMUL(def,def))) */
		kids[0] = LEFT_CHILD(p);
		kids[1] = LEFT_CHILD(RIGHT_CHILD(p));
		kids[2] = LEFT_CHILD(RIGHT_CHILD(RIGHT_CHILD(p)));
		kids[3] = RIGHT_CHILD(RIGHT_CHILD(RIGHT_CHILD(p)));
		break;
	case 224: /* def: DEF(reg,ASSIGN(def)) */
	case 223: /* def: DEF(reg,DCONSTANT(const)) */
	case 205: /* def: DEF(reg,IASSIGN(def)) */
	case 204: /* def: DEF(reg,ICONSTANT(const)) */
		kids[0] = LEFT_CHILD(p);
		kids[1] = LEFT_CHILD(RIGHT_CHILD(p));
		break;
	case 227: /* expr: IADD(DEF(reg,IADD(def,INT)),INT) */
	case 211: /* expr: DMUL(DEF(reg,DMUL(def,INT)),INT) */
		kids[0] = LEFT_CHILD(LEFT_CHILD(p));
		kids[1] = LEFT_CHILD(RIGHT_CHILD(LEFT_CHILD(p)));
		break;
	case 228: /* expr: IADD(INT,DEF(reg,IADD(def,INT))) */
	case 212: /* expr: DMUL(INT,DEF(reg,DMUL(def,INT))) */
		kids[0] = LEFT_CHILD(RIGHT_CHILD(p));
		kids[1] = LEFT_CHILD(RIGHT_CHILD(RIGHT_CHILD(p)));
		break;
	case 229: /* expr: IADD(DEF(reg,IADD(INT,def)),INT) */
	case 213: /* expr: DMUL(DEF(reg,DMUL(INT,def)),INT) */
		kids[0] = LEFT_CHILD(LEFT_CHILD(p));
		kids[1] = RIGHT_CHILD(RIGHT_CHILD(LEFT_CHILD(p)));
		break;
	case 230: /* expr: IADD(INT,DEF(reg,IADD(INT,def))) */
	case 214: /* expr: DMUL(INT,DEF(reg,DMUL(INT,def))) */
		kids[0] = LEFT_CHILD(RIGHT_CHILD(p));
		kids[1] = RIGHT_CHILD(RIGHT_CHILD(RIGHT_CHILD(p)));
		break;
	case 221: /* expr: DMUL(INT,reg) */
		kids[0] = RIGHT_CHILD(p);
		break;
	case 239: /* def: DEF(reg,DADD(DEF(reg,DADD(DEF(reg,expr),def)),def)) */
	case 233: /* def: DEF(reg,ISUB(DEF(reg,ISUB(DEF(reg,expr),def)),def)) */
	case 231: /* def: DEF(reg,DMUL(DEF(reg,DMUL(DEF(reg,expr),def)),def)) */
	case 225: /* def: DEF(reg,IADD(DEF(reg,IADD(DEF(reg,expr),def)),def)) */
		kids[0] = LEFT_CHILD(p);
		kids[1] = LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(p)));
		kids[2] = LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(LEFT_CHILD(RIGHT_CHILD(p)))));
		kids[3] = RIGHT_CHILD(LEFT_CHILD(RIGHT_CHILD(LEFT_CHILD(RIGHT_CHILD(p)))));
		kids[4] = RIGHT_CHILD(RIGHT_CHILD(LEFT_CHILD(RIGHT_CHILD(p))));
		kids[5] = RIGHT_CHILD(RIGHT_CHILD(p));
		break;
	case 240: /* def: DEF(reg,DADD(def,DEF(reg,DADD(def,DEF(reg,expr))))) */
	case 234: /* def: DEF(reg,ISUB(def,DEF(reg,ISUB(def,DEF(reg,expr))))) */
	case 232: /* def: DEF(reg,DMUL(def,DEF(reg,DMUL(def,DEF(reg,expr))))) */
	case 226: /* def: DEF(reg,IADD(def,DEF(reg,IADD(def,DEF(reg,expr))))) */
		kids[0] = LEFT_CHILD(p);
		kids[1] = LEFT_CHILD(RIGHT_CHILD(p));
		kids[2] = LEFT_CHILD(RIGHT_CHILD(RIGHT_CHILD(p)));
		kids[3] = LEFT_CHILD(RIGHT_CHILD(RIGHT_CHILD(RIGHT_CHILD(p))));
		kids[4] = LEFT_CHILD(RIGHT_CHILD(RIGHT_CHILD(RIGHT_CHILD(RIGHT_CHILD(p)))));
		kids[5] = RIGHT_CHILD(RIGHT_CHILD(RIGHT_CHILD(RIGHT_CHILD(RIGHT_CHILD(p)))));
		break;
	case 241: /* def: DEF(reg,DADD(DEF(reg,DADD(def,DEF(reg,expr))),def)) */
	case 235: /* def: DEF(reg,IADD(DEF(reg,IADD(def,DEF(reg,expr))),def)) */
		kids[0] = LEFT_CHILD(p);
		kids[1] = LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(p)));
		kids[2] = LEFT_CHILD(RIGHT_CHILD(LEFT_CHILD(RIGHT_CHILD(p))));
		kids[3] = LEFT_CHILD(RIGHT_CHILD(RIGHT_CHILD(LEFT_CHILD(RIGHT_CHILD(p)))));
		kids[4] = RIGHT_CHILD(RIGHT_CHILD(RIGHT_CHILD(LEFT_CHILD(RIGHT_CHILD(p)))));
		kids[5] = RIGHT_CHILD(RIGHT_CHILD(p));
		break;
	case 242: /* def: DEF(reg,DADD(def,DEF(reg,DADD(DEF(reg,expr),def)))) */
	case 236: /* def: DEF(reg,IADD(def,DEF(reg,IADD(DEF(reg,expr),def)))) */
		kids[0] = LEFT_CHILD(p);
		kids[1] = LEFT_CHILD(RIGHT_CHILD(p));
		kids[2] = LEFT_CHILD(RIGHT_CHILD(RIGHT_CHILD(p)));
		kids[3] = LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(RIGHT_CHILD(RIGHT_CHILD(p)))));
		kids[4] = RIGHT_CHILD(LEFT_CHILD(RIGHT_CHILD(RIGHT_CHILD(RIGHT_CHILD(p)))));
		kids[5] = RIGHT_CHILD(RIGHT_CHILD(RIGHT_CHILD(RIGHT_CHILD(p))));
		break;
	default:
		burm_assert(0, PANIC("Bad external rule number %d in burm_kids\n", eruleno));
	}
	return kids;
}

int burm_op_label(NODEPTR_TYPE p) {
	burm_assert(p, PANIC("NULL tree in burm_op_label\n"));
	return OP_LABEL(p);
}

STATE_TYPE burm_state_label(NODEPTR_TYPE p) {
	burm_assert(p, PANIC("NULL tree in burm_state_label\n"));
	return STATE_LABEL(p);
}

NODEPTR_TYPE burm_child(NODEPTR_TYPE p, int index) {
	burm_assert(p, PANIC("NULL tree in burm_child\n"));
	switch (index) {
	case 0:	return LEFT_CHILD(p);
	case 1:	return RIGHT_CHILD(p);
	}
	burm_assert(0, PANIC("Bad index %d in burm_child\n", index));
	return 0;
}

#endif
