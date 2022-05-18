# Module Makefile template for ilp
# Descriptions of which files make up which libraries...
# $Id: Makefile,v 1.34 1998/04/07 20:13:53 wtang Exp $

GLOBAL_MODULE = GlobalIncludes.h FilePointer.h
GLOBAL_DEPS = $(GLOBAL_MODULE:%=../include/%)

EXCEPTION_MODULE = ArrayOutOfBoundsException.h CouldNotOpenFileException.h Exception.h Exceptions.h NoSuchElementException.h 
EXCEPTION_DEPS = $(EXCEPTION_MODULE:%=../include/%)

ARGUMENT_MODULE = ArgumentIncludes.h BaseArgProperty.h BaseArgument.h FConstArg.h IConstArg.h LabelArg.h LatchRegArg.h PseudoRegArg.h RealRegArg.h VirtualRegArg.h PredidArgProperty.h RegisterArg.h
ARGUMENT_DEPS = $(ARGUMENT_MODULE:%=../include/%)

OPERATION_MODULE = BaseOpProperty.h BaseOperation.h CallOp.h ComputeOp.h ControlOp.h DataOp.h FlowOp.h GotoOp.h HeaderOp.h IfOp.h JumpOp.h MemoryOp.h MiscOp.h NormalOp.h OperationIncludes.h PhiOp.h
OPERATION_DEPS = $(OPERATION_MODULE:%=../include/%)

INSTRUCTION_MODULE = BaseInstruction.h InstructionIncludes.h NormalInstr.h PhiInstr.h
INSTRUCTION_DEPS = $(INSTRUCTION_MODULE:%=../include/%)

CFG_MODULE = BasicBlockEdge.h BasicBlockNode.h BasicBlockPredecessors.h BinGraphDFS.h ControlFlowGraph.h BBIterator.h BasicBlockIncludes.h
CFG_DEPS = $(CFG_MODULE:%=../include/%)

HTG_MODULE = BaseHTGNode.h CallHTGNode.h IfHTGNode.h LoopHTGNode.h SimpleHTGNode.h
HTG_DEPS = $(HTG_MODULE:%=../include/%)

ROUTINE_MODULE = Loop.h LoopList.h Routine.h
ROUTINE_DEPS = $(ROUTINE_MODULE:%=../include/%)

OPSLOT_MODULE = BaseOpSlot.h ControlOpSlot.h DataOpSlot.h FlowOpSlot.h 
OPSLOT_DEPS = $(OPSLOT_MODULE:%=../include/%)

SYMTAB_MODULE = SymbolTable.h
SYMTAB_DEPS = $(SYMTAB_MODULE:%=../include/%)

ARCHITECTURE_MODULE = ArchitectureIncludes.h OpCode.h OpCodeTable.h RegisterFile.h RegisterFileList.h
ARCHITECTURE_DEPS = $(ARCHITECTURE_MODULE:%=../include/%)

ARRAY_MODULE = Array.h ArrayIterator.h
ARRAY_DEPS = $(ARRAY_MODULE:%=../include/%)

LINKEDLIST_MODULE = LinkedList.h LinkedListIterator.h MyLinkedList.h MyLinkedListIterator.h
LINKEDLIST_DEPS = $(LINKEDLIST_MODULE:%=../include/%)

STACK_MODULE = MyStack.h MyStackIterator.h Stack.h StackIterator.h
STACK_DEPS = $(STACK_MODULE:%=../include/%)

PROPERTY_MODULE = PropertyIncludes.h PropertyList.h
PROPERTY_DEPS = $(PROPERTY_MODULE:%=../include/%)

ARGPROPERTY_MODULE = BaseArgProperty.h PredidArgProperty.h SSAArgProperty.h UDProperty.h DUProperty.h CnstProperty.h
ARGPROPERTY_DEPS = $(ARGPROPERTY_MODULE:%=../include/%)

OPPROPERTY_MODULE = BaseOpProperty.h BranchProperty.h DeleteProperty.h
OPPROPERTY_DEPS = $(OPPROPERTY_MODULE:%=../include/%)

INSTRPROPERTY_MODULE = 
INSTRPROPERTY_DEPS = $(INSTRPROPERTY_MODULE:%=../include/%)

BBPROPERTY_MODULE = BaseBBProperty.h CompDefBBProperty.h LiveDeadBBProperty.h
BBPROPERTY_DEPS = $(BBPROPERTY_MODULE:%=../include/%)

HTG_MODULE = BaseHTGNode.h LoopHTGNode.h SimpleHTGNode.h IfHTGNode.h
HTG_DEPS = $(HTG_MODULE:%=../include/%)

COMPILERSTATUS_MODULE = CompilerRTStatus.h
COMPILERSTATUS_DEPS = $(COMPILERSTATUS_MODULE:%=../include/%)

# Composite modules:

SUPPORTDS_DEPS = $(ARRAY_DEPS) $(LINKEDLIST_DEPS) $(STACK_DEPS) $(GLOBAL_DEPS) $(EXCEPTION_DEPS) $(PROPERTY_DEPS)

ILIST_DEPS = $(ARGUMENT_DEPS) $(OPERATION_DEPS) $(INSTRUCTION_DEPS) $(OPSLOT_DEPS) $(SUPPORTDS_DEPS)

IR_DEPS = $(ILIST_DEPS) $(CFG_DEPS) $(HTG_DEPS) $(ROUTINE_DEPS) $(COMPILERSTATUS_DEPS)


# Source files which make up the library
SUPPORT_SRCS = BaseHTGNode.cc BaseOperation.cc BasicBlockNode.cc BasicBlockPredecessors.cc BuildHTG.cc BuildIR.cc BuildLoop.cc BuildSSA.cc CallOp.cc CnstProp.cc CompDefs.cc CompExecPossib.cc CompLiveDead.cc CompUDChain.cc CompilerRTStatus.cc ComputeOp.cc ControlFlowGraph.cc ControlOp.cc ControlOpSlot.cc CopyElim.cc CopyProp.cc DataOp.cc DataOpSlot.cc DeadCodeElimination.cc ErrorMesgs.cc FConstArg.cc FilePointer.cc FlowOp.cc FlowOpSlot.cc GotoOp.cc HeaderOp.cc IBSLoopProperty.cc IConstArg.cc IfHTGNode.cc IfOp.cc ImmediateArg.cc JumpOp.cc LabelArg.cc LatchRegArg.cc Loop.cc LoopHTGNode.cc LoopList.cc LoopManipulate.cc Main.cc MemoryOp.cc NormalInstr.cc NormalOp.cc OpCodeTable.cc PhiInstr.cc PhiOp.cc RealRegArg.cc RegisterAlloc.cc RegisterFileList.cc Routine.cc SimpleHTGNode.cc SymbolTable.cc ValueProp.cc VirtualRegArg.cc parser.cc lexer.cc

# Groups of .h files that belong to a "module". This is very useful if
# a .cc file is dependent on certain "modules".

# Object files to include in library
SUPPORT_OBJECTS = $(SUPPORT_SRCS:%.cc=%.o)

# Name of library.  Note that this should be same name as directory
# we are in for simplification with a prefix of lib (eg libsupport.a)
#LIBNAME = libsupport.a

$(SUPPORT)parser.cc: $(SUPPORT)parser.y
	bison -d -v -t -o$(SUPPORT)parser.cc $(SUPPORT)parser.y

$(SUPPORT)lexer.cc: $(SUPPORT)lexer.l $(SUPPORT)parser.y
	flex  -o$(SUPPORT)lexer.cc $(SUPPORT)lexer.l

BasicBlockNode.o: $(SUPPORT)BasicBlockNode.cc ../include/BasicBlockNode.h $(SUPPORTDS_DEPS)
	$(CCC) $(CCFLAGS) $(SUPPORT)BasicBlockNode.cc

BasicBlockPredecessors.o: $(SUPPORT)BasicBlockPredecessors.cc ../include/BasicBlockPredecessors.h $(SUPPORTDS_DEPS)
	$(CCC) $(CCFLAGS) $(SUPPORT)BasicBlockPredecessors.cc

BuildHTG.o: $(SUPPORT)BuildHTG.cc $(HTG_DEPS)
	$(CCC) $(CCFLAGS) $(SUPPORT)BuildHTG.cc

BuildIR.o: $(SUPPORT)BuildIR.cc $(IR_DEPS)
	$(CCC) $(CCFLAGS) $(SUPPORT)BuildIR.cc

BuildLoop.o: $(SUPPORT)BuildLoop.cc $(ROUTINE_DEPS) $(SUPPORTDS_DEPS)
	$(CCC) $(CCFLAGS) $(SUPPORT)BuildLoop.cc

BuildSSA.o: ../include/BuildSSA.h $(SUPPORT)BuildSSA.cc $(IR_DEPS) $(SUPPORTDS_DEPS)
	$(CCC) $(CCFLAGS) $(SUPPORT)BuildSSA.cc

CallOp.o: $(SUPPORT)CallOp.cc $(OPERATION_DEPS)
	$(CCC) $(CCFLAGS) $(SUPPORT)CallOp.cc

CompilerRTStatus.o: $(SUPPORT)CompilerRTStatus.cc $(SUPPORTDS_DEPS) $(COMPILERSTATUS_DEPS)
	$(CCC) $(CCFLAGS) $(SUPPORT)CompilerRTStatus.cc

ComputeOp.o: $(SUPPORT)ComputeOp.cc $(OPERATION_DEPS)
	$(CCC) $(CCFLAGS) $(SUPPORT)ComputeOp.cc

ControlFlowGraph.o: $(SUPPORT)ControlFlowGraph.cc $(CFG_DEPS) $(SUUPRTDS_DEPS)
	$(CCC) $(CCFLAGS) $(SUPPORT)ControlFlowGraph.cc

ControlOp.o: $(SUPPORT)ControlOp.cc $(OPERATION_DEPS)
	$(CCC) $(CCFLAGS) $(SUPPORT)ControlOp.cc

ControlOpSlot.o: $(SUPPORT)ControlOpSlot.cc ../include/ControlOpSlot.h $(OPERATION_DEPS)
	$(CCC) $(CCFLAGS) $(SUPPORT)ControlOpSlot.cc

DataOp.o: $(SUPPORT)DataOp.cc $(OPERATION_DEPS)
	$(CCC) $(CCFLAGS) $(SUPPORT)DataOp.cc

DataOpSlot.o: $(SUPPORT)DataOpSlot.cc ../include/DataOpSlot.h $(OPERATION_DEPS)
	$(CCC) $(CCFLAGS) $(SUPPORT)DataOpSlot.cc

ErrorMesgs.o: $(SUPPORT)ErrorMesgs.cc ../include/GlobalIncludes.h
	$(CCC) $(CCFLAGS) $(SUPPORT)ErrorMesgs.cc

FConstArg.o: $(SUPPORT)FConstArg.cc $(ARGUMENT_DEPS)
	$(CCC) $(CCFLAGS) $(SUPPORT)FConstArg.cc

FilePointer.o: $(SUPPORT)FilePointer.cc ../include/FilePointer.h
	$(CCC) $(CCFLAGS) $(SUPPORT)FilePointer.cc

FlowOp.o: $(SUPPORT)FlowOp.cc $(OPERATION_DEPS)
	$(CCC) $(CCFLAGS) $(SUPPORT)FlowOp.cc

FlowOpSlot.o: $(SUPPORT)FlowOpSlot.cc ../include/FlowOpSlot.h $(OPERATION_DEPS)
	$(CCC) $(CCFLAGS) $(SUPPORT)FlowOpSlot.cc

HeaderOp.o: $(SUPPORT)HeaderOp.cc $(OPERATION_DEPS)
	$(CCC) $(CCFLAGS) $(SUPPORT)HeaderOp.cc

GotoOp.o: $(SUPPORT)GotoOp.cc ../include/GotoOp.h $(OPERATION_DEPS)
	$(CCC) $(CCFLAGS) $(SUPPORT)GotoOp.cc

IBSLoopProperty.o: $(SUPPORT)IBSLoopProperty.cc $(PROPERTY_DEPS)
	$(CCC) $(CCFLAGS) $(SUPPORT)IBSLoopProperty.cc

IConstArg.o: $(SUPPORT)IConstArg.cc $(ARGUMENT_DEPS)
	$(CCC) $(CCFLAGS) $(SUPPORT)IConstArg.cc

IfOp.o: $(SUPPORT)IfOp.cc $(OPERATION_DEPS)
	$(CCC) $(CCFLAGS) $(SUPPORT)IfOp.cc

ImmediateArg.o: $(SUPPORT)ImmediateArg.cc $(ARGUMENT_DEPS)
	$(CCC) $(CCFLAGS) $(SUPPORT)ImmediateArg.cc

JumpOp.o: $(SUPPORT)JumpOp.cc $(OPERATION_DEPS)
	$(CCC) $(CCFLAGS) $(SUPPORT)JumpOp.cc

LabelArg.o: $(SUPPORT)LabelArg.cc $(ARGUMENT_DEPS)
	$(CCC) $(CCFLAGS) $(SUPPORT)LabelArg.cc

LatchRegArg.o: $(SUPPORT)LatchRegArg.cc $(ARGUMENT_DEPS)
	$(CCC) $(CCFLAGS) $(SUPPORT)LatchRegArg.cc

Loop.o: $(SUPPORT)Loop.cc $(ROUTINE_DEPS) $(SUPPORTDS_DEPS)
	$(CCC) $(CCFLAGS) $(SUPPORT)Loop.cc

LoopManipulate.o: $(SUPPORT)LoopManipulate.cc $(ROUTINE_DEPS) $(SUPPORTDS_DEPS)
	$(CCC) $(CCFLAGS) $(SUPPORT)LoopManipulate.cc

Main.o: $(SUPPORT)Main.cc $(SUPPORTDS_DEPS) $(INSTRUCTION_DEPS) $(ARCHITECTURE_DEPS) $(ROUTINE_DEPS) $(SYMTAB_DEPS) $(CFG_DEPS) $(COMPILERSTATUS_DEPS) $(IR_DEPS)
	$(CCC) $(CCFLAGS) $(SUPPORT)Main.cc -DDEBUG=0

MemoryOp.o: $(SUPPORT)MemoryOp.cc $(OPERATION_DEPS)
	$(CCC) $(CCFLAGS) $(SUPPORT)MemoryOp.cc

NoGUIMain.o: $(SUPPORT)NoGUIMain.cc $(ROUTINE_DEPS)
	$(CCC) $(CCFLAGS) $(SUPPORT)NoGUIMain.cc

NormalInstr.o: $(SUPPORT)NormalInstr.cc $(INSTRUCTION_DEPS) $(SUPPORTDS_DEPS)
	$(CCC) $(CCFLAGS) $(SUPPORT)NormalInstr.cc

NormalOp.o: $(SUPPORT)NormalOp.cc $(OPERATION_DEPS)
	$(CCC) $(CCFLAGS) $(SUPPORT)NormalOp.cc

OpCodeTable.o: $(SUPPORT)OpCodeTable.cc $(ARCHITECTURE_DEPS) $(SUPPORTDS_DEPS)
	$(CCC) $(CCFLAGS) $(SUPPORT)OpCodeTable.cc

PhiInstr.o: $(SUPPORT)PhiInstr.cc $(INSTRUCTION_DEPS) $(SUPPORTDS_DEPS)
	$(CCC) $(CCFLAGS) $(SUPPORT)PhiInstr.cc

PhiOp.o: $(SUPPORT)PhiOp.cc $(OPERATION_DEPS)
	$(CCC) $(CCFLAGS) $(SUPPORT)PhiOp.cc

RealRegArg.o: $(SUPPORT)RealRegArg.cc $(ARGUMENT_DEPS)
	$(CCC) $(CCFLAGS) $(SUPPORT)RealRegArg.cc

RegisterFileList.o: $(SUPPORT)RegisterFileList.cc $(ARCHITECTURE_DEPS) $(SUPPORTDS_DEPS)
	$(CCC) $(CCFLAGS) $(SUPPORT)RegisterFileList.cc

Routine.o: $(SUPPORT)Routine.cc $(ROUTINE_DEPS) $(SUPPORTDS_DEPS) $(COMPILERSTATUS_DEPS)
	$(CCC) $(CCFLAGS) $(SUPPORT)Routine.cc

SymbolTable.o: $(SUPPORT)SymbolTable.cc $(SUPPORTDS_DEPS) $(SYMTAB_DEPS)
	$(CCC) $(CCFLAGS) $(SUPPORT)SymbolTable.cc

VirtualRegArg.o: $(SUPPORT)VirtualRegArg.cc $(ARGUMENT_DEPS)
	$(CCC) $(CCFLAGS) $(SUPPORT)VirtualRegArg.cc

parser.o: $(SUPPORT)parser.cc $(IR_DEPS) $(SUPPORTDS_DEPS)
	$(CCC) $(CCFLAGS) $(SUPPORT)parser.cc

lexer.o: $(SUPPORT)lexer.cc
	$(CCC) $(CCFLAGS) $(SUPPORT)lexer.cc

LoopList.o: $(SUPPORT)LoopList.cc $(ROUTINE_DEPS) $(SUPPORTDS_DEPS)
	$(CCC) $(CCFLAGS) $(SUPPORT)LoopList.cc

CompDefs.o: $(SUPPORT)CompDefs.cc  $(IR_DEPS) $(SUPPORTDS_DEPS) $(BBPROPERTY_DEPS) 
	$(CCC) $(CCFLAGS) $(SUPPORT)CompDefs.cc -DDEBUG=0

CompUDChain.o: $(SUPPORT)CompUDChain.cc $(IR_DEPS) $(SUPPORTDS_DEPS) $(ARGPROPERTY_DEPS) $(SYMTAB_DEPS) $(BBPROPERTY_DEPS)
	$(CCC) $(CCFLAGS) $(SUPPORT)CompUDChain.cc  -DDEBUG=0

CompLiveDead.o: $(SUPPORT)CompLiveDead.cc $(IR_DEPS) $(SUPPORTDS_DEPS) $(BBPROPERTY_DEPS) $(SYMTAB_DEPS)
	$(CCC) $(CCFLAGS) $(SUPPORT)CompLiveDead.cc  -DDEBUG=0

CnstProp.o: $(SUPPORT)CnstProp.cc $(IR_DEPS) $(SUPPORTDS_DEPS) $(BBPROPERTY_DEPS) $(SYMTAB_DEPS) $(ARGPROPERTY_DEPS) $(OPPROPERTY_DEPS)
	$(CCC) $(CCFLAGS) $(SUPPORT)CnstProp.cc  -DDEBUG=0

DeadCodeElimination.o: $(SUPPORT)DeadCodeElimination.cc $(IR_DEPS) $(SUPPORTDS_DEPS) $(BBPROPERTY_DEPS)  $(OPPROPERTY_DEPS)
	$(CCC) $(CCFLAGS) $(SUPPORT)DeadCodeElimination.cc  -DDEBUG=0

ValueProp.o: $(SUPPORT)ValueProp.cc $(IR_DEPS) $(SUPPORTDS_DEPS)
	$(CCC) $(CCFLAGS) $(SUPPORT)ValueProp.cc  -DDEBUG=0

SimpleHTGNode.o: $(SUPPORT)SimpleHTGNode.cc $(IR_DEPS) $(HTG_DEPS)
	$(CCC) $(CCFLAGS) $(SUPPORT)SimpleHTGNode.cc  -DDEBUG=0

LoopHTGNode.o: $(SUPPORT)LoopHTGNode.cc $(IR_DEPS) $(HTG_DEPS)
	$(CCC) $(CCFLAGS) $(SUPPORT)LoopHTGNode.cc  -DDEBUG=0

IfHTGNode.o: $(SUPPORT)IfHTGNode.cc $(IR_DEPS) $(HTG_DEPS)
	$(CCC) $(CCFLAGS) $(SUPPORT)IfHTGNode.cc  -DDEBUG=0

BaseHTGNode.o: $(SUPPORT)BaseHTGNode.cc $(IR_DEPS) $(HTG_DEPS)
	$(CCC) $(CCFLAGS) $(SUPPORT)BaseHTGNode.cc  -DDEBUG=0

BaseOperation.o: $(SUPPORT)BaseOperation.cc $(IR_DEPS)
	$(CCC) $(CCFLAGS) $(SUPPORT)BaseOperation.cc  -DDEBUG=0

CompExecPossib.o: $(SUPPORT)CompExecPossib.cc $(IR_DEPS) $(SUPPORTDS_DEPS) $(BBPROPERTY_DEPS)
	$(CCC) $(CCFLAGS) $(SUPPORT)CompExecPossib.cc  -DDEBUG=0


RegisterAlloc.o: $(SUPPORT)RegisterAlloc.cc $(IR_DEPS) $(SUPPORTDS_DEPS) ../include/RegPressure.h ../include/RegOrder.h
	$(CCC) $(CCFLAGS) $(SUPPORT)RegisterAlloc.cc  -DDEBUG=0


CopyProp.o: $(SUPPORT)CopyProp.cc $(IR_DEPS) $(SUPPORTDS_DEPS)
	$(CCC) $(CCFLAGS) $(SUPPORT)CopyProp.cc  -DDEBUG=0

CopyElim.o: $(SUPPORT)CopyElim.cc $(IR_DEPS) $(SUPPORTDS_DEPS)
	$(CCC) $(CCFLAGS) $(SUPPORT)CopyElim.cc  -DDEBUG=0

#$(LIBNAME): $(OBJS)
#	$(AR) $(ARFLAGS) $@ $?
#	ranlib $@

rm_support:
	$(RM) $(LIBNAME) $(SUPPORT_OBJECTS)

depend:
	makedepend $(INCLUDE_PATH) $(CXX_SRCS) $(C_SRCS)
