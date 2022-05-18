# Expression ADL Toolkit
EXPRESSION Architecture Description Language (ADL) and retargetable compiler/simulator tool-kit 

EXPRESSION is an Architecture Description Language (ADL) as well as a retargetable compiler/simulator tool-kit for architectural design space exploration (DSE). A processor-memory architecture can be captured using the Graphical User Interface (GUI). The front-end of the tool-kit generates the EXPRESSION description for the processor, which in turn steers automatic generation of retargetable compiler and simulator.

The key features of our design methodology include:
	
- Ease of specification and modification of architecture from the GUI.	
- Mixed behavioral/structural representation supporting a natural, concise specification of the architecture.
- Explicit specification of the memory subsystem allowing novel memory organizations and hierarchies.
- Efficient specification of architectural resource constraints allowing extraction of detailed Reservation Tables (RTs) for compiler scheduling.


![EXPRESSION GUI](https://github.com/EPIC-CSU/expression/blob/main/expression-gui.jpg)


The complete EXPRESSION toolkit is now available as an Open Source Release in the public domain. The goal of this Open Source release is to promote usage of EXPRESSION ADL to specify processor-memory architectures and provide a reference point for further research into ADLs. 

## Overview

The EXPRESSION project aims to achieve exploration of Systems-On-Chip (SOCs) with programmable processor cores and novel memory hierarchies. Effective exploration of such SOCs is possible by considering the interaction between the processor architecture, the compiler and the target application. In this project we use EXPRESSION, an Architecture Description Language (ADL), to specify the processor-memory architecture. Further, we automatically generate EXPRESS, a highly optimizing, Instruction-Level-Parallelizing (ILP) compiler, and SIMPRESS, a cycle-accurate, structural simulator from EXPRESSION. EXPRESSION, EXPRESS and SIMPRESS are integrated under a visual environment, V-SAT (Visual Specification and Analysis Tool), to aid rapid Design Space Exploration (DSE).

**EXPRESSION** was designed with the dual goal of allowing processor description for fast DSE and for automatic generation of detailed/accurate simulation/compilation tools. The novel features of EXPRESSION include:

- Integration of the Instruction-Set and Structure to avoid redundancy in specification,
- Automatic generation of resource constraints (as reservation tables), and
- Constructs for explicitly specifying novel and traditional memories

**EXPRESS** was developed with the goal of providing a retargetable compiler platform for Embedded-System/System-on-Chip development. The EXPRESS retargetable compiler takes in C programs and produces a highly optimized (and parallel) target specific code using state-of-art Instruction-Level Parallelism (ILP) techniques. The compiler features an extensive set of integrated transformations to perform the traditional compiler tasks of code selection, instruction scheduling and register allocation and memory-aware optimizations.

**SIMPRESS** is a retargetable, cycle-accurate, structural simulator that can be used to evaluate the architecture, the application and the effectiveness of the compiler transformations. It features an extensive set of statistic collector agents that are used to gather information such as resource usage, hazard count, inner-loop execution time, etc.

**V-SAT** provides a visual environment to graphically specify the architecture and perform Architectural DSE in an intuitive manner. The EXPRESSION description of the processor can be automatically generated from the V-SAT specification.

**EXPRESSION** is an ADL supporting architectural design space exploration (DSE) for embedded Systems-on-Chip (SOC) and automatic generation of a retargetable compiler/simulator toolkit.

## System Requirements

The EXPRESSION toolkit is available as downloadable source code which needs to be compiled on a host machine before it can be executed. To run EXPRESSION, you will require a machine running Windows and Visual C++ installed on it. The EXPRESSION toolkit has been tested on the following systems:


OS: Microsoft Windows XP Professional, Windows 2000 Server
System Type: X86-based PC
Processor: x86 Family 15 Model 1 Stepping 2 Genuine Intel ~1 Ghz
Total Physical Memory: 512.00 MB
Total Virtual Memory: 1.72 GB
Page File Space: 1.22 GB

Development Platform: Visual C++ 6.0 Enterprise Edition

Additionally, you will also require access to a SUN Sparc workstation if you plan to compile your own C applications (see manual for details). This step can also be performed online, by clicking here to go to the web based compilation  page where you can upload your C program and get the requisite .defs and .procs files needed by the EXPRESSION toolkit after compilation.
