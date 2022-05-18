include ../Makefile.vars

SIMULATOR_OBJECTS =  sim_register_file.o  bitset.o \
        sim_cache.o sim_mem.o simulator.o simu_aux.o \
	Storage.o Latch.o SimMain.o

SIMULATOR_CXXSRCS = sim_register_file.cc bitset.cc\
	sim_cache.cc sim_mem.cc simulator.cc simu_aux.cc\
	Storage.cc SimMain.cc Latch.cc FlowMechanism.cc

Latch.o: $(SIMULATOR)Latch.h $(SIMULATOR)Latch.cc $(SYSTEM)ArchClassIncludes.h 
	$(CCC)  $(CCFLAGS)  $(SIMULATOR)Latch.cc  

Storage.o: $(SIMULATOR)Storage.h $(SIMULATOR)Storage.cc
	$(CCC)  $(CCFLAGS)  $(SIMULATOR)Storage.cc  $(MOTIFINCLUDE) 

SimMain.o: $(SIMULATOR)SimMain.cc $(SYSTEM)ArchClassIncludes.h\
	   $(GUI)simulatorWin.h
	$(CCC)  $(CCFLAGS)  $(SIMULATOR)SimMain.cc  

sim_register_file.o: $(SIMULATOR)sim_register_file.h $(SYSTEM)DataValue.h\
	  $(SIMULATOR)ltstr.h  $(SIMULATOR)sim_register_file.cc
	$(CCC)  $(CCFLAGS)  $(SIMULATOR)sim_register_file.cc  \
		$(MOTIFINCLUDE)	 

bitset.o: $(SIMULATOR)bitset.h $(SIMULATOR)bitset.cc
	$(CCC)  $(CCFLAGS)  $(SIMULATOR)bitset.cc  

sim_cache.o: $(SIMULATOR)sim_cache.h $(SIMULATOR)sim_cache.cc
	$(CCC)  $(CCFLAGS)  $(SIMULATOR)sim_cache.cc  \
		$(MOTIFINCLUDE)

sim_mem.o: $(SIMULATOR)sim_mem.h $(SIMULATOR)sim_mem.cc
	$(CCC)  $(CCFLAGS)  $(SIMULATOR)sim_mem.cc  

simulator.o: $(SIMULATOR)simulator.h $(SIMULATOR)simulator.cc
	$(CCC)  $(CCFLAGS)  $(SIMULATOR)simulator.cc   \
		$(MOTIFINCLUDE)

simu_aux.o: $(SIMULATOR)simulator.h $(SIMULATOR)simu_aux.cc
	$(CCC)  $(CCFLAGS)  $(SIMULATOR)simu_aux.cc  

FlowMechanism.o: $(SIMULATOR)FlowMechanism.h $(SIMULATOR)FlowMechanism.cc
	$(CCC)  $(CCFLAGS)  $(SIMULATOR)FlowMechanism.cc  

rm_sim:
	$(RM) $(LIBNAME) $(SIMULATOR_OBJECTS)

depend:
	makedepend $(INCLUDE_PATH) $(CXX_SRCS) $(C_SRCS)
