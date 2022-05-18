include ../Makefile.vars

DISAMB_SRCS = PartofProperty.cc disamb.cc error.cc DisambRoutine.cc \
		DisambNormalInstr.cc DisambComputeOp.cc DisambIConstArg.cc \
		DisambMemoryOp.cc DisambRealRegArg.cc \
		DisambRegisterArg.cc gcd.cc disamb_parser.cc \
		exprs.cc expr_utils.cc \
		BasicSet.cc intMatrix.cc polyhedron.cc myvector.cc auxiliary.cc \
		poly.cc dependency.cc

DISAMB_OBJECTS = $(DISAMB_SRCS:%.cc=%.o)

PartofProperty.o: $(DIS)PartofProperty.cc $(DIS)PartofProperty.h
	$(CCC) $(CCFLAGS) $(DIS)PartofProperty.cc

DISAMB_DEPS = $(DIS)disamb.h $(DIS)error.h $(DIS)charString.h $(DIS)PartofProperty.h 
INTERNAL_DISAMB_DEPS = $(DISAMB_DEPS) $(DIS)exprs.h $(DIS)BasicSet.h $(DIS)intMatrix.h \
			$(DIS)poly.h $(DIS)gcd.h $(DIS)disamb_parser.h $(DIS)types.h \
			$(DIS)auxiliary.h $(DIS)expr_utils.h $(DIS)polyhedron.h \
			$(DIS)std_types.h $(DIS)myvector.h

disamb.o: $(DIS)disamb.cc $(INTERNAL_DISAMB_DEPS) $(ILIST_DEPS)
	$(CCC) $(CCFLAGS) $(DIS)disamb.cc

error.o: $(DIS)error.cc $(INTERNAL_DISAMB_DEPS)
	$(CCC) $(CCFLAGS) $(DIS)error.cc

DisambRoutine.o: $(DIS)DisambRoutine.cc $(ROUTINE_DEPS)
	$(CCC) $(CCFLAGS) $(DIS)DisambRoutine.cc

DisambNormalInstr.o: $(DIS)DisambNormalInstr.cc $(INSTRUCTION_DEPS)
	$(CCC) $(CCFLAGS) $(DIS)DisambNormalInstr.cc

DisambComputeOp.o: $(DIS)DisambComputeOp.cc $(INTERNAL_DISAMB_DEPS) $(OPERATION_DEPS) $(ARCHITECTURE_DEPS)
	$(CCC) $(CCFLAGS) $(DIS)DisambComputeOp.cc

DisambIConstArg.o: $(DIS)DisambIConstArg.cc $(INTERNAL_DISAMB_DEPS) $(ARGUMENT_DEPS) $(OPERATION_DEPS)
	$(CCC) $(CCFLAGS) $(DIS)DisambIConstArg.cc

DisambMemoryOp.o: $(DIS)DisambMemoryOp.cc $(OPERATION_DEPS) $(INTERNAL_DISAMB_DEPS)
	$(CCC) $(CCFLAGS) $(DIS)DisambMemoryOp.cc

DisambRealRegArg.o: $(DIS)DisambRealRegArg.cc $(INTERNAL_DISAMB_DEPS) $(OPERATION_DEPS) $(ARGUMENT_DEPS) $(PROPERTY_DEPS)
	$(CCC) $(CCFLAGS) $(DIS)DisambRealRegArg.cc

DisambRegisterArg.o: $(DIS)DisambRegisterArg.cc $(INTERNAL_DISAMB_DEPS) $(OPERATION_DEPS) $(ARGUMENT_DEPS)
	$(CCC) $(CCFLAGS) $(DIS)DisambRegisterArg.cc


gcd.o: $(DIS)gcd.cc $(INTERNAL_DISAMB_DEPS)
	$(CCC) $(CCFLAGS) $(DIS)gcd.cc


disamb_parser.o: $(DIS)disamb_parser.cc $(INTERNAL_DISAMB_DEPS)
	$(CCC) $(CCFLAGS) $(DIS)disamb_parser.cc

exprs.o: $(DIS)exprs.cc $(INTERNAL_DISAMB_DEPS)
	$(CCC) $(CCFLAGS) $(DIS)exprs.cc

expr_utils.o: $(DIS)expr_utils.cc $(INTERNAL_DISAMB_DEPS)
	$(CCC) $(CCFLAGS) $(DIS)expr_utils.cc

BasicSet.o: $(DIS)BasicSet.cc $(INTERNAL_DISAMB_DEPS)
	$(CCC) $(CCFLAGS) $(DIS)BasicSet.cc

intMatrix.o: $(DIS)intMatrix.cc $(INTERNAL_DISAMB_DEPS)
	$(CCC) $(CCFLAGS) $(DIS)intMatrix.cc

polyhedron.o: $(DIS)polyhedron.cc $(INTERNAL_DISAMB_DEPS)
	$(CCC) $(CCFLAGS) $(DIS)polyhedron.cc

myvector.o: $(DIS)myvector.cc $(INTERNAL_DISAMB_DEPS)
	$(CCC) $(CCFLAGS) $(DIS)myvector.cc

auxiliary.o: $(DIS)auxiliary.cc $(INTERNAL_DISAMB_DEPS)
	$(CCC) $(CCFLAGS) $(DIS)auxiliary.cc

poly.o: $(DIS)poly.cc $(INTERNAL_DISAMB_DEPS)
	$(CCC) $(CCFLAGS) $(DIS)poly.cc

dependency.o: $(DIS)dependency.cc $(INTERNAL_DISAMB_DEPS)
	$(CCC) $(CCFLAGS) $(DIS)dependency.cc

rm_disamb:
	$(RM) $(LIBNAME) $(DISAMB_OBJECTS)
