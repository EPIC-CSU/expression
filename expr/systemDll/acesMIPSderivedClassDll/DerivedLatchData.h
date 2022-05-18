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
 *file  : DerivedLatchData.h                                 *
 *authors : Prabhat, Srikanth                                *
 *created : 3 May, 2000.                                     *
 *last modified :                                            *
 *                                                           *
 *contents : contains the Data classes which will reside in  *
 *           the latches.                                    *
 *compiled with : g++                                        *
 *for more info : contact Ashok at ahalambi@ics.uci.edu      *
 *caution : always grep for TODO to check for incompleteness *
 *************************************************************/

#ifndef _DERIVEDLATCHDATA_H_
#define _DERIVEDLATCHDATA_H_

#include "DerivedAddrData.h"
#include "ExpressAddrData.h"
#include "LatchData.h"

typedef OneOperandData<PgmAddr, BaseType> PCLatchData;
typedef OneOperandData<InstrStr, BaseType> InstStrLatchData;
typedef ListOperandData<AddrOperand, PredicateAddrDataUnion, MyInt> OperationLatchData;

class MyOperList
{
	public:
		vector<OperationLatchData> _operList;
		MyOperList(){}
		MyOperList(MyOperList & a)
		{
			vector<OperationLatchData>::const_iterator i1,i2;
			i1 = (a._operList).begin();
			i2 = (a._operList).end();
			while (i1 != i2)
			{
				_operList.push_back(*i1);
				i1++;
			}
		}

		~MyOperList(){}
		
		void operator=(const MyOperList & a)
		{

			vector<OperationLatchData>::const_iterator i1,i2;
			i1 = (a._operList).begin();
			i2 = (a._operList).end();
			while (i1 != i2)
			{
				_operList.push_back(*i1);
				i1++;
			}
		}

		void reset()
		{
			_operList.clear();
		}

		void print(ostream & out)
		{
			vector<OperationLatchData>::iterator i1,i2;
			i1 = (_operList).begin();
			i2 = (_operList).end();
			while (i1 != i2)
			{
				(*i1).print(out);
				out << endl;
				i1++;
			}
		}
};

typedef OneOperandData<MyOperList, BaseType> InstructionLatchData;

#endif


