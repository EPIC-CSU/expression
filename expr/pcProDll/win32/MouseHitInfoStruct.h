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
#if !defined(AFX_MOUSEHITINFOSTRUCT_H__6E562C31_28C6_11D2_B74D_00C04FB17504__INCLUDED_)
#define AFX_MOUSEHITINFOSTRUCT_H__6E562C31_28C6_11D2_B74D_00C04FB17504__INCLUDED_

//WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING 
//WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING 
//WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING 

//NOS: quite a few things depend on the order of this enum
//NOS: if you must only add things at the end of the list

typedef enum 
{
 MHI_INVALID,
 MHI_INSTR,
 MHI_BASICBLOCK, 
 MHI_SIMPLE_HTG_NODE, 
 MHI_IF_HTG_NODE, 
 MHI_LOOP_HTG_NODE, 
 MHI_COMPOUND_HTG_NODE,
 MHI_SIM_COMPOUND_UNIT,
 MHI_SIM_UNIT,
 MHI_SIM_PORT,
 MHI_SIM_CONNECTION,
 MHI_SIM_STORAGE,
 MHI_SIM_LATCH,
 MHI_SIM_PIPELINESTAGE,
 MHI_SIM_BUS
} 
 MHI_HIT_TYPE;

//WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING 
//WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING 
//WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING 

typedef struct
{
 MHI_HIT_TYPE type;
 void* node;
}
 MOUSEHITINFO;

#endif // !defined(AFX_MOUSEHITINFOSTRUCT_H__6E562C31_28C6_11D2_B74D_00C04FB17504__INCLUDED_)
