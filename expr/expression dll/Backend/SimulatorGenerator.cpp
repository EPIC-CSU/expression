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
 *file  : simulatorGenerator.cpp                             *
 *authors : Asheesh Khare                                    *
 *last modified :                                            *
 *                                                           *
 *modified by : Sudeep Pasricha	(10/24/02)					 *
 *contents:Support functions for simulator generation        *
 *comments:-                                                 *
 *compiled with : g++                                        *
 *for more info :                                            *
 *************************************************************/

#include "symtabs.h"
#include "SimulatorGeneratorSupportFuncs.h"

NodeList Componentlist,Unitlist, Latchlist, Storagelist, Portlist, Connectionlist, Dtpathlist;
SymbolList Symbollist;
NodePtr ArchRootNode,PipelineRootNode,InstrSectionRootNode, StorageRootNode, OpMapRootNode;
extern Symtab st;
FILE * fp;

void generateSimulator(NodePtr ir)
{

	char fname[] = "../../systemDll/acesMIPSbuildsystemDll/acesMIPSbuildSystem.cpp"; 
	if((fp=fopen(fname,"w"))==NULL)
	{
		printf( "Can't open file '%s'\n", fname);
		abort();
	}	
	
	// get root of the storage section 
    ir->Traverse(&getStorageRootNode);

	// Generate necessary functions for memory subsystem
	StorageRootNode->Traverse(&CreateComponentList);

	addGenericOpcodesFromFile();

	CreateOpMap();
	// Now printInitialDecls dynamically declares Storage
	// type elements instead of hardcoding them - Sudeep
	printInitialDecls(StorageRootNode);	
		
	generateMemorySubsystem(StorageRootNode);

	// get root of he architecture section 
	ir->Traverse(&getArchRootNode);
	
	// the following call generates Componentlist as well as a map 
	// ComponenetMap containing "element node,parent node" entry
	ArchRootNode->Traverse(&CreateComponentList);

	//printComponentMap();

	fprintf(fp,"\n//INSTANTIATE UNITS\n\n");
	// the following call also generates unitlist
	instantiateUnits(Componentlist);

	fprintf(fp,"\n//INSTANTIATE LATCHES\n\n");
	// the following call also generates latchlist 
	instantiateLatches(Componentlist);

	fprintf(fp,"\n//INSTANTIATE STORAGE\n\n");
	// the following call also generates storagelist
	instantiateStorage(Componentlist);

	addStorageToCntrl(StorageRootNode);

	fprintf(fp,"\n//INSTANTIATE PORTS\n\n");
	// the following call also generates portlist
	instantiatePorts(Componentlist);

	fprintf(fp,"\n//INSTANTIATE CONNECTIONS\n\n");
	// the following call also generates connectionlist
	instantiateConnections(Componentlist);
	
	fprintf(fp,"\n//ADD SUBCOMPONENTS TO COMPOUND COMPONENTS\n\n");
	addSubCompsToUnits(Unitlist);

	fprintf(fp,"\n//ADD SUBCOMPONENTS TO COMPOUND LATCHES\n\n");
	addSubCompsToLatches(Storagelist);

	fprintf(fp,"\n//ADD LATCHES TO COMPONENTS\n\n");
	addLatchesToUnits(Unitlist);

	fprintf(fp,"\n//ADD PORTS TO CONNECTIONS\n\n");
	//addPortsToUnits(Connectionlist);

	fprintf(fp,"\n//ADD CONNECTIONS TO PORTS\n\n");
	//addConnectionsToUnits(Portlist);
	//addConnectionsToUnits(Componentlist);

	fprintf(fp,"\n//ADD PORTS TO COMPONENTS\n\n");
	addPortsToUnits(Unitlist);

	fprintf(fp,"\n//ADD CONNECTIONS TO COMPONENTS\n\n");
	addConnectionsToUnits(Unitlist);

	//fprintf(fp,"\n//ADD ADDDATATRANS STATEMENTS (hardcoded)\n");
	//addDataTransStatements(Unitlist);

	//fprintf(fp,"\n\t// ADD BRANCH PREDICTION STATEMENTS (hardcoded)\n");
	//addBranchPredictionStatements(); 

	// get root of the Opcode/Operand mapping section 
	ir->Traverse(&getOpMapRootNode);

	fprintf(fp,"\n//ADD MAPPING TO OPCODES\n\n");
	createOpcodeMapping();

	// Removed (10/22) - Sudeep
	// fprintf(fp,"\n//ADD OPCODES TO COMPONENTS\n\n");
	 addOpCodesToUnits(Unitlist);

	// get root of the Pipeline section 
	ir->Traverse(&getPipelineRootNode);

	// the following call generates DTpathlist as well as a map 
	// DTMap containing "element node,parent node" entry
	PipelineRootNode->Traverse(&CollectDTPaths);

	// get Symbollist that is needed for the IsAdjacentToUnit function
	st.Traverse(&getSymbolList);

	fprintf(fp,"\n//ADD DATA TRANSFER PATHS\n\n");
	addDTPaths(Dtpathlist, Componentlist);

	// create mem.config
	creatememconfig(StorageRootNode);

	ir->Traverse(&getInstrSectionRootNode);

	//ConnectivityMatrix.printGraph();
	fprintf(fp,"\n//SETUP THE VLIW INSTRUCTION TEMPLATE\n\n");
	createInstructionTemplate();

	createPipelineInfo();

	printEndOfFunction(Unitlist);

	//addGenericOpcodesFromFile();

	addTargetOpcodesFromTreeMapping(Unitlist);

	//printEndOfFunction2();

	// get root of the storage section 
    //ir->Traverse(&getStorageRootNode);

	// Generate necessary functions for memory subsystem
	//StorageRootNode->Traverse(&CreateComponentList);
		
	//generateMemorySubsystem(StorageRootNode);

	//addOpCodesToUnits(Unitlist);

	fclose(fp);
}