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
// Module: ARCHITECTURE
/*************************************************************
 *file  : ArchClassIncludes.h                                *
 *authors : ahalambi,vgee                                    *
 *last modified :			                                 *
 *                                                           *
 *contents: certain important enums and other #defines which *
 *will be used throughout the Architecture description.      *
 *compiled with : g++                                        *
 *for more info :                                            *
 *************************************************************/
#ifndef __ARCHCLASSINCLUDES_H__
#define __ARCHCLASSINCLUDES_H__

#include "ArchGlobalIncludes.h"
#include "ArchSTLIncludes.h"

//#define numOfDataExecuteUnits 4
//#define numOfControlExecuteUnits 1

#define MAX_LEN 10

#define CREATE_ITER(p, q) (p = q.begin())

#define HAS_MORE_ELEMENTS(p, q) (p != q.end())

#define CURRENT_ELEMENT(p) (*p)

#define NEXT_ELEMENT(p) (p++)

#define UNIT_ELEM(q) (_units[q])
#define LATCH_ELEM(q) (_latches[q])
#define CONNECT_ELEM(q) (_connections[q])
#define PORT_ELEM(q) (_ports[q])
#define STORAGE_ELEM(q) (_storage[q])

#define SUB_LATCH_ELEM(p, q) ((SimpleLatch<p> *)(LATCH_ELEM(q)))
#define COMP_LATCH_ELEM(q) ((CompoundLatch *)(LATCH_ELEM(q)))

// #define COMP_LATCH_ELEM1(m, n, q) ((COMP_LATCH_ELEM(m, n, q))->getLatch1())
// #define COMP_LATCH_ELEM2(m, n, q) ((COMP_LATCH_ELEM(m, n, q))->getLatch2())

// #define COMP_LATCH_SUB_ELEM(p, m, n, q) COMP_LATCH_ELEM ## p (m, n, q)

#define COMP_LATCH_SUB_ELEM(q) (COMP_LATCH_ELEM(q))

#define REGFILE_CONNECT_ELEM(q) ((RegisterConnection *)CONNECT_ELEM(q))
#define MEMORY_CONNECT_ELEM(q) ((MemoryConnection *)CONNECT_ELEM(q))



enum ConnectionType {_CONNECTION_, _REGISTERCONNECTION_, _MEMORYCONNECTION_};
enum PortType { _PORT_, _UNIT_PORT_, _READ_, _READWRITE_, _WRITE_};
enum LatchType {_IN_L_, _OUT_L_, _OTHER_L_};
 
enum BaseTypeEnum { _BASETYPE_, _VAL_, _PCTYPE_, _PGMADDR_, _INSTRSTR_, _IRPTR_, _ADDR_, _LATCHADDR_, 
		    _ADDROPERAND_, _ADDRDATAUNION_, _PREDICATEADDRDATAUNION_, _DERIVEDADDRDATA_, _LATCHDATA_, 
		    _ONEOPERANDDATA_, _TWOOPERANDDATA_, _THREEOPERANDDATA_, _LISTOPERANDDATA_,
			_PREDICATEOPERANDDATA_, _PREDICATELISTOPERANDDATA_, _VALARRAY_,
			_IA64OPERANDDATA_, _PREDICATEOPERAND_, _OPCODEQUALIFIER_
                  }; 
enum StorageClassType { _STORAGE_,_MEM_, _REGISTER_,_LATCH_,_SIMPLELATCH_, _COMPOUNDLATCH_};

enum ClassType   {  _UNIT_, _SIMPLE_UNIT_, _COMPOUND_UNIT_,
		    _FETCH_UNIT_, _DECODE_UNIT_, _EXECUTE_UNIT_,
		    _ALU_UNIT_, _FLOATALU_UNIT_, _BRANCH_UNIT_,
		    _LOADSTORE_UNIT_, _OPREAD_UNIT_, _BROPREAD_UNIT_,
		    _ALUEXECUTE_UNIT_, _FLOATALUEXECUTE_UNIT_,
		    _BRANCHEXECUTE_UNIT_, _LOADSTOREEXECUTE_UNIT_,
		    _WRITEBACKSUB_UNIT_, _BRANCHWRITEBACKSUB_UNIT_,
		    _WRITEBACK_UNIT_, _BRANCHWRITEBACK_UNIT_, _ARCH_UNIT_,
			_COMPOUND_FETCH_UNIT_, _COMPOUND_DECODE_UNIT_, 
			_PROG_ADDR_GEN_UNIT_, _PROG_ADDR_SEND_UNIT_, 
			_PROG_ACCESS_WAIT_UNIT_, _PROG_FETCH_PKT_RECEIVE_UNIT_,
			_INSTR_DISPATCH_UNIT_, _INSTR_DECODE_UNIT_, _L_UNIT_E1_PHASE_UNIT_,
			_L_UNIT_, _M_UNIT_, _S_UNIT_, _D_UNIT_,
            _M_UNIT_E1_PHASE_UNIT_, _M_UNIT_E2_PHASE_UNIT_,_S_UNIT_E1_PHASE_UNIT_, 
			_D_UNIT_E1_PHASE_UNIT_, _D_UNIT_E2_PHASE_UNIT_, _MEM_CNTRL_UNIT_,
			_CONTROL_UNIT_, _MULTIPLY_UNIT_, _SHIFT_UNIT_, _ADDRINCREMENTER_UNIT_
		 };

/*enum OpCodeType {_IADD_, _IGE_, _ILE_, _IF_, _IASH_, _ISUB_, _ILSH_, 
		 _DMUL_, _DSUB_, _ILT_, _IVLOAD_, _IVSTORE_, _ICONSTANT_,
		 _NOP_
                };

  */ //Commented by Sumit G. 

enum AddressMode {_REG_,_DIRECTMEMORY_,_NONE_};   
enum FieldType { _ADDR_INT_, _ADDR_FLT_, _ADDR_DBL_, _ADDR_STRING_, _ADDR_MISC_};   
enum ArgType {_ADDRTYPE_,_VALTYPE_, _NOTYPE_};

#define CYCLES_0 0
#define CYCLES_1 1
#define CYCLES_2 2
#define CYCLES_3 3
#define PIPE_FULL 4
#define CYCLES_N_3 5
#define CYCLES_N_2 6
#define CYCLES_N_1 7
#define CYCLES_N_0 8
#define HAZARD_1 9
#define HAZARD_1_1 10
#define HAZARD_1_2 11
#define HAZARD_12_1 12
#define HAZARD_12_2 13
#define HAZARD_2 14
#define HAZARD_2_1 15

extern unsigned long clock_count;
#endif
