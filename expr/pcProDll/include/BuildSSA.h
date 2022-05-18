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
//#include "Routine.h"
#include "BaseOperation.h"
#include "DataOp.h"
#include "DataOpSlot.h"
#include "NormalInstr.h"
#include "BaseArgument.h"
#include "ComputeOp.h"
#include "IfOp.h"
#include "MemoryOp.h"
#include "CallOp.h"
#include "JumpOp.h"
#include "PhiInstr.h"
#include "PhiOp.h"
#include "RegisterArg.h"
#include "RegisterFileList.h"
#include "SSAArgProperty.h"
#include "STLIncludes.h"

#include "SymbolTable.h"
#include "ltstr.h"


//The first argument is of type: RegisterArg *, since there is limit on symbol
//length of the assembler, have to use void * here, and do explicit conversion
//in the program
typedef set<void *, less<void *> > _argSet;

typedef set<void *, less<void *> > _instrSet;

//the first argument is of type: BaseInstruction *
//the second argument is of type: BasicBlockNode *
typedef map<void *, void *, less<void *> > _instrBBMap;

//the first argument is the name, such as R11.5
//the second is the definition instruction  set of the argument
typedef map<char *, _instrSet , ltstr > _defInstrMap;

//the first argument is of type: RegisterArg *, which is the definition point
// of an argument. the second argument is the use points of the argument
typedef map<char *, _argSet, ltstr > _sameNameMap;


typedef set<int, less<int> > intSet;
typedef deque<int> intStack;

class DTnode {
  int idom;
  intSet children;
public:
  DTnode() {
    idom = 0;
  }

  ~DTnode() {
  }

  void setIDOM(int i){
    idom = i;

        //NOS
        //return 0;
  }

  void addChild(int i){
    children.insert(i);
  }

  int getIDOM() {
    return idom;
  }

  intSet& getChildren() {
    return children;
  }
};

extern SymbolTable *globalSymbolTable;
extern RegisterFileList *globalRegFileList;

