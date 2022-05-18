
#include "DerivedUnit.h"
#include "DerivedLatch.h"
#include "DerivedLatchData.h"
#include "DerivedPort.h"
#include "DerivedStorage.h"

#include "Instruction.h"
#include "DerivedOpExecSemantics.h"
#include "CompName.h"

#include "ControlTable.h"

#include "Connectivity.h"

#include "AssociativeCache.h"

#include "Dram.h"


#include "ArchStats.h"
#include "StatList.h"

Unit *toyArch;
MemoryModule *mainMemory;
Connectivity * connect;
int __L2_LINE_SIZE__;


MyString nameStr;
Storage * GPRFile;
Storage * FPRFile;
Storage * L1;
Storage * IL1;
Storage * L2;
Storage * ScratchPad;
Storage * MainMem;
Storage * Connect;

extern ArchStatsDB globalArchStatsDB;

extern MyString memConfigFile;

__declspec (dllexport) MemoryModule *icachePtr;

Simple_VLIW_Instr *simpleVLIWInstrTemplate;

// Need to store a list of Objects that contain simulator wide statistics
// For e.g. control unit with the hazard stall statistics.

__declspec (dllexport) vector<BaseSimObject *> globalStatsObjects;
typedef struct address_map_node {
int start;
int end;
int module_number;
address_map_node *next;
} addr_map_node;

addr_map_node *address_map;

void buildSystem(void)
{

// Initialize the memory and register (STORAGE) modules
	/********* Storage *****************************/
	GPRFile = new VirtualRegFile();
	nameStr = "GPRFile";
	addName(GPRFile, nameStr);
	FPRFile = new VirtualRegFile();
	nameStr = "FPRFile";
	addName(FPRFile, nameStr);

//INSTANTIATE UNITS


	/********* Units *****************************/
	toyArch = new ArchUnit();
	toyArch->init();
	nameStr = "ARCH";
	addName(toyArch, nameStr);

	Unit *cntrl;
	cntrl = new ControlUnit();
	cntrl->init();
	nameStr = "Control";
	addName(cntrl, nameStr);

	Unit * FETCH;
	FETCH=new FetchUnit(1,4,4);
	FETCH->init();

	cntrl->addUnit(FETCH);

	nameStr = "FETCH";
	addName(FETCH, nameStr);

	Unit * DECODE;
	DECODE=new DecodeUnit(12,4,2);
	DECODE->init();

	cntrl->addUnit(DECODE);

	nameStr = "DECODE";
	addName(DECODE, nameStr);

	Unit * ALU1_READ;
	ALU1_READ=new OpReadUnit(1,1,1);
	ALU1_READ->init();

	cntrl->addUnit(ALU1_READ);

	nameStr = "ALU1_READ";
	addName(ALU1_READ, nameStr);

	Unit * ALU2_READ;
	ALU2_READ=new OpReadUnit(1,1,1);
	ALU2_READ->init();

	cntrl->addUnit(ALU2_READ);

	nameStr = "ALU2_READ";
	addName(ALU2_READ, nameStr);

	Unit * FALU_READ;
	FALU_READ=new OpReadUnit(1,1,1);
	FALU_READ->init();

	cntrl->addUnit(FALU_READ);

	nameStr = "FALU_READ";
	addName(FALU_READ, nameStr);

	Unit * BR_READ;
	BR_READ=new OpReadUnit(1,1,1);
	BR_READ->init();

	cntrl->addUnit(BR_READ);

	nameStr = "BR_READ";
	addName(BR_READ, nameStr);

	Unit * LDST_READ;
	LDST_READ=new OpReadUnit(1,1,1);
	LDST_READ->init();

	cntrl->addUnit(LDST_READ);

	nameStr = "LDST_READ";
	addName(LDST_READ, nameStr);

	Unit * ALU1_EX;
	ALU1_EX=new ExecuteUnit(1,1,1);
	ALU1_EX->init();

	cntrl->addUnit(ALU1_EX);

	nameStr = "ALU1_EX";
	addName(ALU1_EX, nameStr);

	Unit * ALU2_EX;
	ALU2_EX=new ExecuteUnit(1,1,1);
	ALU2_EX->init();

	cntrl->addUnit(ALU2_EX);

	nameStr = "ALU2_EX";
	addName(ALU2_EX, nameStr);

	Unit * FALU_EX;
	FALU_EX=new ExecuteUnit(1,1,1);
	FALU_EX->init();

	cntrl->addUnit(FALU_EX);

	nameStr = "FALU_EX";
	addName(FALU_EX, nameStr);

	Unit * BR_EX;
	BR_EX=new BranchUnit(1,1,1);
	BR_EX->init();

	cntrl->addUnit(BR_EX);

	nameStr = "BR_EX";
	addName(BR_EX, nameStr);

	Unit * LDST_EX;
	LDST_EX=new LoadStoreUnit(1,1,1);
	LDST_EX->init();

	cntrl->addUnit(LDST_EX);

	nameStr = "LDST_EX";
	addName(LDST_EX, nameStr);

	Unit * WB;
	WB=new WriteBackUnit(5,1,1);
	WB->init();

	cntrl->addUnit(WB);

	nameStr = "WB";
	addName(WB, nameStr);

	toyArch->addUnit(cntrl);

//INSTANTIATE LATCHES


//INSTANTIATE STORAGE

	Latch * FetDecLatch = new InstStrLatch();
	nameStr = "FetDecLatch";
	addName(FetDecLatch, nameStr);

	Latch * pcLatch = new PCLatch();
	nameStr = "pcLatch";
	addName(pcLatch, nameStr);

	Latch * DecAlu1ReadLatch = new InstructionLatch();
	nameStr = "DecAlu1ReadLatch";
	addName(DecAlu1ReadLatch, nameStr);

	Latch * DecAlu2ReadLatch = new InstructionLatch();
	nameStr = "DecAlu2ReadLatch";
	addName(DecAlu2ReadLatch, nameStr);

	Latch * DecFaluReadLatch = new InstructionLatch();
	nameStr = "DecFaluReadLatch";
	addName(DecFaluReadLatch, nameStr);

	Latch * DecLdStReadLatch = new InstructionLatch();
	nameStr = "DecLdStReadLatch";
	addName(DecLdStReadLatch, nameStr);

	Latch * DecBrReadLatch = new InstructionLatch();
	nameStr = "DecBrReadLatch";
	addName(DecBrReadLatch, nameStr);

	Latch * Alu1ReadExLatch = new OperationLatch();
	nameStr = "Alu1ReadExLatch";
	addName(Alu1ReadExLatch, nameStr);

	Latch * Alu2ReadExLatch = new OperationLatch();
	nameStr = "Alu2ReadExLatch";
	addName(Alu2ReadExLatch, nameStr);

	Latch * FaluReadExLatch = new OperationLatch();
	nameStr = "FaluReadExLatch";
	addName(FaluReadExLatch, nameStr);

	Latch * BrReadBrExLatch = new OperationLatch();
	nameStr = "BrReadBrExLatch";
	addName(BrReadBrExLatch, nameStr);

	Latch * LdStReadExLatch = new OperationLatch();
	nameStr = "LdStReadExLatch";
	addName(LdStReadExLatch, nameStr);

	Latch * Alu1ExWbLatch = new OperationLatch();
	nameStr = "Alu1ExWbLatch";
	addName(Alu1ExWbLatch, nameStr);

	Latch * Alu2ExWbLatch = new OperationLatch();
	nameStr = "Alu2ExWbLatch";
	addName(Alu2ExWbLatch, nameStr);

	Latch * FaluExWbLatch = new OperationLatch();
	nameStr = "FaluExWbLatch";
	addName(FaluExWbLatch, nameStr);

	Latch * BrExWbLatch = new OperationLatch();
	nameStr = "BrExWbLatch";
	addName(BrExWbLatch, nameStr);

	Latch * pcLatch1 = new PCLatch();
	nameStr = "pcLatch1";
	addName(pcLatch1, nameStr);

	Latch * LdStExWbLatch = new OperationLatch();
	nameStr = "LdStExWbLatch";
	addName(LdStExWbLatch, nameStr);

	((ControlUnit *)cntrl)->addStorage(GPRFile);
	((ControlUnit *)cntrl)->addStorage(FPRFile);

//INSTANTIATE PORTS

	Port * Alu1ReadPort1 = new UnitPort(_READ_);
	nameStr = "Alu1ReadPort1";
	addName(Alu1ReadPort1, nameStr);

	Port * Alu1ReadPort2 = new UnitPort(_READ_);
	nameStr = "Alu1ReadPort2";
	addName(Alu1ReadPort2, nameStr);

	Port * Alu2ReadPort1 = new UnitPort(_READ_);
	nameStr = "Alu2ReadPort1";
	addName(Alu2ReadPort1, nameStr);

	Port * Alu2ReadPort2 = new UnitPort(_READ_);
	nameStr = "Alu2ReadPort2";
	addName(Alu2ReadPort2, nameStr);

	Port * FaluReadPort1 = new UnitPort(_READ_);
	nameStr = "FaluReadPort1";
	addName(FaluReadPort1, nameStr);

	Port * FaluReadPort2 = new UnitPort(_READ_);
	nameStr = "FaluReadPort2";
	addName(FaluReadPort2, nameStr);

	Port * BrReadPort1 = new UnitPort(_READ_);
	nameStr = "BrReadPort1";
	addName(BrReadPort1, nameStr);

	Port * BrReadPort2 = new UnitPort(_READ_);
	nameStr = "BrReadPort2";
	addName(BrReadPort2, nameStr);

	Port * LdStReadPort2 = new UnitPort(_READ_);
	nameStr = "LdStReadPort2";
	addName(LdStReadPort2, nameStr);

	Port * LdStReadPort3 = new UnitPort(_READ_);
	nameStr = "LdStReadPort3";
	addName(LdStReadPort3, nameStr);

	Port * LdStReadPort1 = new UnitPort(_READ_);
	nameStr = "LdStReadPort1";
	addName(LdStReadPort1, nameStr);

	Port * LdStReadWritePort = new UnitPort(_READWRITE_);
	nameStr = "LdStReadWritePort";
	addName(LdStReadWritePort, nameStr);

	Port * WbWritePort = new UnitPort(_WRITE_);
	nameStr = "WbWritePort";
	addName(WbWritePort, nameStr);

	Port * GprReadPort1 = new Port(_READ_);
	nameStr = "GprReadPort1";
	addName(GprReadPort1, nameStr);

	Port * GprReadPort2 = new Port(_READ_);
	nameStr = "GprReadPort2";
	addName(GprReadPort2, nameStr);

	Port * GprReadPort4 = new Port(_READ_);
	nameStr = "GprReadPort4";
	addName(GprReadPort4, nameStr);

	Port * GprReadPort5 = new Port(_READ_);
	nameStr = "GprReadPort5";
	addName(GprReadPort5, nameStr);

	Port * GprReadPort6 = new Port(_READ_);
	nameStr = "GprReadPort6";
	addName(GprReadPort6, nameStr);

	Port * GprReadPort7 = new Port(_READ_);
	nameStr = "GprReadPort7";
	addName(GprReadPort7, nameStr);

	Port * GprReadPort8 = new Port(_READ_);
	nameStr = "GprReadPort8";
	addName(GprReadPort8, nameStr);

	Port * GprReadPort9 = new Port(_READ_);
	nameStr = "GprReadPort9";
	addName(GprReadPort9, nameStr);

	Port * GprWritePort = new Port(_WRITE_);
	nameStr = "GprWritePort";
	addName(GprWritePort, nameStr);

	Port * GprReadPort3 = new Port(_READ_);
	nameStr = "GprReadPort3";
	addName(GprReadPort3, nameStr);

	Port * FprReadPort1 = new Port(_READ_);
	nameStr = "FprReadPort1";
	addName(FprReadPort1, nameStr);

	Port * FprReadPort2 = new Port(_READ_);
	nameStr = "FprReadPort2";
	addName(FprReadPort2, nameStr);

	Port * FprReadPort3 = new Port(_READ_);
	nameStr = "FprReadPort3";
	addName(FprReadPort3, nameStr);

	Port * FprReadPort4 = new Port(_READ_);
	nameStr = "FprReadPort4";
	addName(FprReadPort4, nameStr);

	Port * FprReadPort5 = new Port(_READ_);
	nameStr = "FprReadPort5";
	addName(FprReadPort5, nameStr);

	Port * FprWritePort = new Port(_WRITE_);
	nameStr = "FprWritePort";
	addName(FprWritePort, nameStr);

	Port * FprReadPort6 = new Port(_READ_);
	nameStr = "FprReadPort6";
	addName(FprReadPort6, nameStr);

	Port * FprReadPort7 = new Port(_READ_);
	nameStr = "FprReadPort7";
	addName(FprReadPort7, nameStr);

	Port * L1ReadWritePort = new Port(_READWRITE_);
	nameStr = "L1ReadWritePort";
	addName(L1ReadWritePort, nameStr);


//INSTANTIATE CONNECTIONS

	Connection * LdStMemCxn = new MemoryConnection();
	nameStr = "LdStMemCxn";
	addName(LdStMemCxn, nameStr);

	Connection * GprReadPort1Alu1ReadPort1Cxn = new RegisterConnection();
	nameStr = "GprReadPort1Alu1ReadPort1Cxn";
	addName(GprReadPort1Alu1ReadPort1Cxn, nameStr);

	Connection * GprReadPort2Alu1ReadPort2Cxn = new RegisterConnection();
	nameStr = "GprReadPort2Alu1ReadPort2Cxn";
	addName(GprReadPort2Alu1ReadPort2Cxn, nameStr);

	Connection * GprReadPort4Alu2ReadPort2Cxn = new RegisterConnection();
	nameStr = "GprReadPort4Alu2ReadPort2Cxn";
	addName(GprReadPort4Alu2ReadPort2Cxn, nameStr);

	Connection * GprReadPort5BrReadPort1Cxn = new RegisterConnection();
	nameStr = "GprReadPort5BrReadPort1Cxn";
	addName(GprReadPort5BrReadPort1Cxn, nameStr);

	Connection * GprReadPort6BrReadPort2Cxn = new RegisterConnection();
	nameStr = "GprReadPort6BrReadPort2Cxn";
	addName(GprReadPort6BrReadPort2Cxn, nameStr);

	Connection * GprReadPort7LdStReadPort1Cxn = new RegisterConnection();
	nameStr = "GprReadPort7LdStReadPort1Cxn";
	addName(GprReadPort7LdStReadPort1Cxn, nameStr);

	Connection * GprReadPort8LdStReadPort2Cxn = new RegisterConnection();
	nameStr = "GprReadPort8LdStReadPort2Cxn";
	addName(GprReadPort8LdStReadPort2Cxn, nameStr);

	Connection * GprReadPort9LdStReadPort3Cxn = new RegisterConnection();
	nameStr = "GprReadPort9LdStReadPort3Cxn";
	addName(GprReadPort9LdStReadPort3Cxn, nameStr);

	Connection * WbWritePortGprWritePortCxn = new RegisterConnection();
	nameStr = "WbWritePortGprWritePortCxn";
	addName(WbWritePortGprWritePortCxn, nameStr);

	Connection * GprReadPort3Alu2ReadPort1Cxn = new RegisterConnection();
	nameStr = "GprReadPort3Alu2ReadPort1Cxn";
	addName(GprReadPort3Alu2ReadPort1Cxn, nameStr);

	Connection * FprReadPort1Alu2ReadPort1Cxn = new RegisterConnection();
	nameStr = "FprReadPort1Alu2ReadPort1Cxn";
	addName(FprReadPort1Alu2ReadPort1Cxn, nameStr);

	Connection * FprReadPort2Alu2ReadPort2Cxn = new RegisterConnection();
	nameStr = "FprReadPort2Alu2ReadPort2Cxn";
	addName(FprReadPort2Alu2ReadPort2Cxn, nameStr);

	Connection * FprReadPort3FaluReadPort1Cxn = new RegisterConnection();
	nameStr = "FprReadPort3FaluReadPort1Cxn";
	addName(FprReadPort3FaluReadPort1Cxn, nameStr);

	Connection * FprReadPort4FaluReadPort2Cxn = new RegisterConnection();
	nameStr = "FprReadPort4FaluReadPort2Cxn";
	addName(FprReadPort4FaluReadPort2Cxn, nameStr);

	Connection * FprReadPort5LdStReadPort3Cxn = new RegisterConnection();
	nameStr = "FprReadPort5LdStReadPort3Cxn";
	addName(FprReadPort5LdStReadPort3Cxn, nameStr);

	Connection * WbWritePortFprWritePortCxn = new RegisterConnection();
	nameStr = "WbWritePortFprWritePortCxn";
	addName(WbWritePortFprWritePortCxn, nameStr);

	Connection * FprReadPort6ALU1ReadPort1 = new RegisterConnection();
	nameStr = "FprReadPort6ALU1ReadPort1";
	addName(FprReadPort6ALU1ReadPort1, nameStr);

	Connection * FprReadPort7ALU1ReadPort2Cxn = new RegisterConnection();
	nameStr = "FprReadPort7ALU1ReadPort2Cxn";
	addName(FprReadPort7ALU1ReadPort2Cxn, nameStr);


//ADD SUBCOMPONENTS TO COMPOUND COMPONENTS


//ADD SUBCOMPONENTS TO COMPOUND LATCHES


//ADD LATCHES TO COMPONENTS

	FETCH->addLatch(_OUT_L_,FetDecLatch);
	cntrl->addLatch(_OTHER_L_,FetDecLatch);
	FETCH->addLatch(_OTHER_L_,pcLatch);
	cntrl->addLatch(_OTHER_L_,pcLatch);

	DECODE->addLatch(_IN_L_,FetDecLatch);
	cntrl->addLatch(_OTHER_L_,FetDecLatch);
	DECODE->addLatch(_OUT_L_,DecAlu1ReadLatch);
	cntrl->addLatch(_OTHER_L_,DecAlu1ReadLatch);
	DECODE->addLatch(_OUT_L_,DecAlu2ReadLatch);
	cntrl->addLatch(_OTHER_L_,DecAlu2ReadLatch);
	DECODE->addLatch(_OUT_L_,DecFaluReadLatch);
	cntrl->addLatch(_OTHER_L_,DecFaluReadLatch);
	DECODE->addLatch(_OUT_L_,DecLdStReadLatch);
	cntrl->addLatch(_OTHER_L_,DecLdStReadLatch);
	DECODE->addLatch(_OUT_L_,DecBrReadLatch);
	cntrl->addLatch(_OTHER_L_,DecBrReadLatch);
	ALU1_READ->addLatch(_IN_L_,DecAlu1ReadLatch);
	cntrl->addLatch(_OTHER_L_,DecAlu1ReadLatch);
	ALU1_READ->addLatch(_OUT_L_,Alu1ReadExLatch);
	cntrl->addLatch(_OTHER_L_,Alu1ReadExLatch);
	ALU2_READ->addLatch(_IN_L_,DecAlu2ReadLatch);
	cntrl->addLatch(_OTHER_L_,DecAlu2ReadLatch);
	ALU2_READ->addLatch(_OUT_L_,Alu2ReadExLatch);
	cntrl->addLatch(_OTHER_L_,Alu2ReadExLatch);
	FALU_READ->addLatch(_IN_L_,DecFaluReadLatch);
	cntrl->addLatch(_OTHER_L_,DecFaluReadLatch);
	FALU_READ->addLatch(_OUT_L_,FaluReadExLatch);
	cntrl->addLatch(_OTHER_L_,FaluReadExLatch);
	BR_READ->addLatch(_IN_L_,DecBrReadLatch);
	cntrl->addLatch(_OTHER_L_,DecBrReadLatch);
	BR_READ->addLatch(_OUT_L_,BrReadBrExLatch);
	cntrl->addLatch(_OTHER_L_,BrReadBrExLatch);
	LDST_READ->addLatch(_IN_L_,DecLdStReadLatch);
	cntrl->addLatch(_OTHER_L_,DecLdStReadLatch);
	LDST_READ->addLatch(_OUT_L_,LdStReadExLatch);
	cntrl->addLatch(_OTHER_L_,LdStReadExLatch);
	ALU1_EX->addLatch(_IN_L_,Alu1ReadExLatch);
	cntrl->addLatch(_OTHER_L_,Alu1ReadExLatch);
	ALU1_EX->addLatch(_OUT_L_,Alu1ExWbLatch);
	cntrl->addLatch(_OTHER_L_,Alu1ExWbLatch);
	ALU2_EX->addLatch(_IN_L_,Alu2ReadExLatch);
	cntrl->addLatch(_OTHER_L_,Alu2ReadExLatch);
	ALU2_EX->addLatch(_OUT_L_,Alu2ExWbLatch);
	cntrl->addLatch(_OTHER_L_,Alu2ExWbLatch);
	FALU_EX->addLatch(_IN_L_,FaluReadExLatch);
	cntrl->addLatch(_OTHER_L_,FaluReadExLatch);
	FALU_EX->addLatch(_OUT_L_,FaluExWbLatch);
	cntrl->addLatch(_OTHER_L_,FaluExWbLatch);
	BR_EX->addLatch(_IN_L_,BrReadBrExLatch);
	cntrl->addLatch(_OTHER_L_,BrReadBrExLatch);
	BR_EX->addLatch(_OUT_L_,BrExWbLatch);
	cntrl->addLatch(_OTHER_L_,BrExWbLatch);
	BR_EX->addLatch(_OTHER_L_,pcLatch1);
	cntrl->addLatch(_OTHER_L_,pcLatch1);

	LDST_EX->addLatch(_IN_L_,LdStReadExLatch);
	cntrl->addLatch(_OTHER_L_,LdStReadExLatch);
	LDST_EX->addLatch(_OUT_L_,LdStExWbLatch);
	cntrl->addLatch(_OTHER_L_,LdStExWbLatch);
	WB->addLatch(_IN_L_,Alu1ExWbLatch);
	cntrl->addLatch(_OTHER_L_,Alu1ExWbLatch);
	WB->addLatch(_IN_L_,Alu2ExWbLatch);
	cntrl->addLatch(_OTHER_L_,Alu2ExWbLatch);
	WB->addLatch(_IN_L_,FaluExWbLatch);
	cntrl->addLatch(_OTHER_L_,FaluExWbLatch);
	WB->addLatch(_IN_L_,BrExWbLatch);
	cntrl->addLatch(_OTHER_L_,BrExWbLatch);
	WB->addLatch(_IN_L_,LdStExWbLatch);
	cntrl->addLatch(_OTHER_L_,LdStExWbLatch);

//ADD PORTS TO CONNECTIONS


//ADD CONNECTIONS TO PORTS


//ADD PORTS TO COMPONENTS

	ALU1_READ->addPort(Alu1ReadPort1);
	ALU1_READ->addPort(Alu1ReadPort2);

	ALU2_READ->addPort(Alu2ReadPort1);
	ALU2_READ->addPort(Alu2ReadPort2);

	FALU_READ->addPort(FaluReadPort1);
	FALU_READ->addPort(FaluReadPort2);

	BR_READ->addPort(BrReadPort1);
	BR_READ->addPort(BrReadPort2);

	LDST_READ->addPort(LdStReadPort2);
	LDST_READ->addPort(LdStReadPort3);
	LDST_READ->addPort(LdStReadPort1);

	LDST_EX->addPort(LdStReadWritePort);

	WB->addPort(WbWritePort);


//ADD CONNECTIONS TO COMPONENTS


//ADD MAPPING TO OPCODES



	ALU1_READ->addOpCode("dmfc1",dmfc1Func,1);
	ALU1_READ->addOpCode("DMFC1",dmfc1Func,1);
	ALU1_READ->addOpCode("dmtc1",dmtc1Func,1);
	ALU1_READ->addOpCode("DMTC1",dmtc1Func,1);
	ALU1_READ->addOpCode("cvt_s_w",cvtsiFunc,1);
	ALU1_READ->addOpCode("CVTSI",cvtsiFunc,1);
	ALU1_READ->addOpCode("xor",ixorFunc,1);
	ALU1_READ->addOpCode("IXOR",ixorFunc,1);
	ALU1_READ->addOpCode("and",iandFunc,1);
	ALU1_READ->addOpCode("IAND",iandFunc,1);
	ALU1_READ->addOpCode("cvt_s_d",cvtsdFunc,1);
	ALU1_READ->addOpCode("CVTSD",cvtsdFunc,1);
	ALU1_READ->addOpCode("cvt_d_s",cvtdsFunc,1);
	ALU1_READ->addOpCode("CVTDS",cvtdsFunc,1);
	ALU1_READ->addOpCode("cvt_d_w",cvtdiFunc,1);
	ALU1_READ->addOpCode("CVTDI",cvtdiFunc,1);
	ALU1_READ->addOpCode("trunc_w_s",truncisFunc,1);
	ALU1_READ->addOpCode("TRUNCIS",truncisFunc,1);
	ALU1_READ->addOpCode("trunc_w_d",truncidFunc,1);
	ALU1_READ->addOpCode("TRUNCID",truncidFunc,1);
	ALU1_READ->addOpCode("mfhi",mfhiFunc,1);
	ALU1_READ->addOpCode("MFHI",mfhiFunc,1);
	ALU1_READ->addOpCode("mflo",mfloFunc,1);
	ALU1_READ->addOpCode("MFLO",mfloFunc,1);
	ALU1_READ->addOpCode("mfc1",mfc1Func,1);
	ALU1_READ->addOpCode("MFC1",mfc1Func,1);
	ALU1_READ->addOpCode("mtc1",mtc1Func,1);
	ALU1_READ->addOpCode("MTC1",mtc1Func,1);
	ALU1_READ->addOpCode("sgtu",igtFunc,1);
	ALU1_READ->addOpCode("IGTU",igtFunc,1);
	ALU1_READ->addOpCode("sleu",ileFunc,1);
	ALU1_READ->addOpCode("ILEU",ileFunc,1);
	ALU1_READ->addOpCode("sltu",iltFunc,1);
	ALU1_READ->addOpCode("ILTU",iltFunc,1);
	ALU1_READ->addOpCode("li",iconstantFunc,1);
	ALU1_READ->addOpCode("ICONSTANT",iconstantFunc,1);
	ALU1_READ->addOpCode("div",idivMIPSFunc,1);
	ALU1_READ->addOpCode("IDIV",idivMIPSFunc,1);
	ALU1_READ->addOpCode("mult",imulMIPSFunc,1);
	ALU1_READ->addOpCode("IMUL",imulMIPSFunc,1);
	ALU1_READ->addOpCode("and",iandFunc,1);
	ALU1_READ->addOpCode("IAND",iandFunc,1);
	ALU1_READ->addOpCode("or",iorFunc,1);
	ALU1_READ->addOpCode("IOR",iorFunc,1);
	ALU1_READ->addOpCode("xori",ixorFunc,1);
	ALU1_READ->addOpCode("IXOR",ixorFunc,1);
	ALU1_READ->addOpCode("andi",iandFunc,1);
	ALU1_READ->addOpCode("IAND",iandFunc,1);
	ALU1_READ->addOpCode("ori",iorFunc,1);
	ALU1_READ->addOpCode("IOR",iorFunc,1);
	ALU1_READ->addOpCode("li_s",fconstantFunc,1);
	ALU1_READ->addOpCode("FCONSTANT",fconstantFunc,1);
	ALU1_READ->addOpCode("li_d",dconstantFunc,1);
	ALU1_READ->addOpCode("DCONSTANT",dconstantFunc,1);
	ALU1_READ->addOpCode("sgeu",igeFunc,1);
	ALU1_READ->addOpCode("IGEU",igeFunc,1);
	ALU1_READ->addOpCode("sne",ineFunc,1);
	ALU1_READ->addOpCode("INE",ineFunc,1);
	ALU1_READ->addOpCode("seq",ieqFunc,1);
	ALU1_READ->addOpCode("IEQ",ieqFunc,1);
	ALU1_READ->addOpCode("sgt",igtFunc,1);
	ALU1_READ->addOpCode("IGT",igtFunc,1);
	ALU1_READ->addOpCode("sle",ileFunc,1);
	ALU1_READ->addOpCode("ILE",ileFunc,1);
	ALU1_READ->addOpCode("slt",iltFunc,1);
	ALU1_READ->addOpCode("ILT",iltFunc,1);
	ALU1_READ->addOpCode("sge",igeFunc,1);
	ALU1_READ->addOpCode("IGE",igeFunc,1);
	ALU1_READ->addOpCode("sla",ilashFunc,1);
	ALU1_READ->addOpCode("ILASH",ilashFunc,1);
	ALU1_READ->addOpCode("sll",illshFunc,1);
	ALU1_READ->addOpCode("ILLSH",illshFunc,1);
	ALU1_READ->addOpCode("sra",irashFunc,1);
	ALU1_READ->addOpCode("IRASH",irashFunc,1);
	ALU1_READ->addOpCode("srl",irlshFunc,1);
	ALU1_READ->addOpCode("IRLSH",irlshFunc,1);
	ALU1_READ->addOpCode("move",iassignFunc,1);
	ALU1_READ->addOpCode("IASSIGN",iassignFunc,1);
	ALU1_READ->addOpCode("subu",isubFunc,1);
	ALU1_READ->addOpCode("ISUB",isubFunc,1);
	ALU1_READ->addOpCode("nop",nopFunc,1);
	ALU1_READ->addOpCode("NOP",nopFunc,1);
	ALU1_READ->addOpCode("addu",iaddFunc,1);
	ALU1_READ->addOpCode("IADD",iaddFunc,1);

	ALU2_READ->addOpCode("dmfc1",dmfc1Func,1);
	ALU2_READ->addOpCode("DMFC1",dmfc1Func,1);
	ALU2_READ->addOpCode("dmtc1",dmtc1Func,1);
	ALU2_READ->addOpCode("DMTC1",dmtc1Func,1);
	ALU2_READ->addOpCode("cvt_s_w",cvtsiFunc,1);
	ALU2_READ->addOpCode("CVTSI",cvtsiFunc,1);
	ALU2_READ->addOpCode("xor",ixorFunc,1);
	ALU2_READ->addOpCode("IXOR",ixorFunc,1);
	ALU2_READ->addOpCode("and",iandFunc,1);
	ALU2_READ->addOpCode("IAND",iandFunc,1);
	ALU2_READ->addOpCode("cvt_s_d",cvtsdFunc,1);
	ALU2_READ->addOpCode("CVTSD",cvtsdFunc,1);
	ALU2_READ->addOpCode("cvt_d_s",cvtdsFunc,1);
	ALU2_READ->addOpCode("CVTDS",cvtdsFunc,1);
	ALU2_READ->addOpCode("cvt_d_w",cvtdiFunc,1);
	ALU2_READ->addOpCode("CVTDI",cvtdiFunc,1);
	ALU2_READ->addOpCode("trunc_w_s",truncisFunc,1);
	ALU2_READ->addOpCode("TRUNCIS",truncisFunc,1);
	ALU2_READ->addOpCode("trunc_w_d",truncidFunc,1);
	ALU2_READ->addOpCode("TRUNCID",truncidFunc,1);
	ALU2_READ->addOpCode("mfhi",mfhiFunc,1);
	ALU2_READ->addOpCode("MFHI",mfhiFunc,1);
	ALU2_READ->addOpCode("mflo",mfloFunc,1);
	ALU2_READ->addOpCode("MFLO",mfloFunc,1);
	ALU2_READ->addOpCode("mfc1",mfc1Func,1);
	ALU2_READ->addOpCode("MFC1",mfc1Func,1);
	ALU2_READ->addOpCode("mtc1",mtc1Func,1);
	ALU2_READ->addOpCode("MTC1",mtc1Func,1);
	ALU2_READ->addOpCode("sgtu",igtFunc,1);
	ALU2_READ->addOpCode("IGTU",igtFunc,1);
	ALU2_READ->addOpCode("sleu",ileFunc,1);
	ALU2_READ->addOpCode("ILEU",ileFunc,1);
	ALU2_READ->addOpCode("sltu",iltFunc,1);
	ALU2_READ->addOpCode("ILTU",iltFunc,1);
	ALU2_READ->addOpCode("li",iconstantFunc,1);
	ALU2_READ->addOpCode("ICONSTANT",iconstantFunc,1);
	ALU2_READ->addOpCode("div",idivMIPSFunc,1);
	ALU2_READ->addOpCode("IDIV",idivMIPSFunc,1);
	ALU2_READ->addOpCode("mult",imulMIPSFunc,1);
	ALU2_READ->addOpCode("IMUL",imulMIPSFunc,1);
	ALU2_READ->addOpCode("and",iandFunc,1);
	ALU2_READ->addOpCode("IAND",iandFunc,1);
	ALU2_READ->addOpCode("or",iorFunc,1);
	ALU2_READ->addOpCode("IOR",iorFunc,1);
	ALU2_READ->addOpCode("xori",ixorFunc,1);
	ALU2_READ->addOpCode("IXOR",ixorFunc,1);
	ALU2_READ->addOpCode("andi",iandFunc,1);
	ALU2_READ->addOpCode("IAND",iandFunc,1);
	ALU2_READ->addOpCode("ori",iorFunc,1);
	ALU2_READ->addOpCode("IOR",iorFunc,1);
	ALU2_READ->addOpCode("li_s",fconstantFunc,1);
	ALU2_READ->addOpCode("FCONSTANT",fconstantFunc,1);
	ALU2_READ->addOpCode("li_d",dconstantFunc,1);
	ALU2_READ->addOpCode("DCONSTANT",dconstantFunc,1);
	ALU2_READ->addOpCode("sgeu",igeFunc,1);
	ALU2_READ->addOpCode("IGEU",igeFunc,1);
	ALU2_READ->addOpCode("sne",ineFunc,1);
	ALU2_READ->addOpCode("INE",ineFunc,1);
	ALU2_READ->addOpCode("seq",ieqFunc,1);
	ALU2_READ->addOpCode("IEQ",ieqFunc,1);
	ALU2_READ->addOpCode("sgt",igtFunc,1);
	ALU2_READ->addOpCode("IGT",igtFunc,1);
	ALU2_READ->addOpCode("sle",ileFunc,1);
	ALU2_READ->addOpCode("ILE",ileFunc,1);
	ALU2_READ->addOpCode("slt",iltFunc,1);
	ALU2_READ->addOpCode("ILT",iltFunc,1);
	ALU2_READ->addOpCode("sge",igeFunc,1);
	ALU2_READ->addOpCode("IGE",igeFunc,1);
	ALU2_READ->addOpCode("sla",ilashFunc,1);
	ALU2_READ->addOpCode("ILASH",ilashFunc,1);
	ALU2_READ->addOpCode("sll",illshFunc,1);
	ALU2_READ->addOpCode("ILLSH",illshFunc,1);
	ALU2_READ->addOpCode("sra",irashFunc,1);
	ALU2_READ->addOpCode("IRASH",irashFunc,1);
	ALU2_READ->addOpCode("srl",irlshFunc,1);
	ALU2_READ->addOpCode("IRLSH",irlshFunc,1);
	ALU2_READ->addOpCode("move",iassignFunc,1);
	ALU2_READ->addOpCode("IASSIGN",iassignFunc,1);
	ALU2_READ->addOpCode("subu",isubFunc,1);
	ALU2_READ->addOpCode("ISUB",isubFunc,1);
	ALU2_READ->addOpCode("nop",nopFunc,1);
	ALU2_READ->addOpCode("NOP",nopFunc,1);
	ALU2_READ->addOpCode("addu",iaddFunc,1);
	ALU2_READ->addOpCode("IADD",iaddFunc,1);

	FALU_READ->addOpCode("mul_d",dmulFunc,1);
	FALU_READ->addOpCode("DMUL",dmulFunc,1);
	FALU_READ->addOpCode("add_d",daddFunc,1);
	FALU_READ->addOpCode("DADD",daddFunc,1);
	FALU_READ->addOpCode("sub_d",dsubFunc,1);
	FALU_READ->addOpCode("DSUB",dsubFunc,1);
	FALU_READ->addOpCode("div_d",ddivFunc,1);
	FALU_READ->addOpCode("DDIV",ddivFunc,1);
	FALU_READ->addOpCode("sub_s",fsubFunc,1);
	FALU_READ->addOpCode("FSUB",fsubFunc,1);
	FALU_READ->addOpCode("add_s",faddFunc,1);
	FALU_READ->addOpCode("FADD",faddFunc,1);
	FALU_READ->addOpCode("mul_s",fmulFunc,1);
	FALU_READ->addOpCode("FMUL",fmulFunc,1);
	FALU_READ->addOpCode("div_s",fdivFunc,1);
	FALU_READ->addOpCode("FDIV",fdivFunc,1);
	FALU_READ->addOpCode("mov_d",dassignFunc,1);
	FALU_READ->addOpCode("DASSIGN",dassignFunc,1);
	FALU_READ->addOpCode("mov_s",fassignFunc,1);
	FALU_READ->addOpCode("FASSIGN",fassignFunc,1);
	FALU_READ->addOpCode("c_le_d",dleFunc,1);
	FALU_READ->addOpCode("DLE",dleFunc,1);
	FALU_READ->addOpCode("c_lt_d",dltFunc,1);
	FALU_READ->addOpCode("DLT",dltFunc,1);
	FALU_READ->addOpCode("c_ge_d",dgeFunc,1);
	FALU_READ->addOpCode("DGE",dgeFunc,1);
	FALU_READ->addOpCode("c_gt_d",dgtFunc,1);
	FALU_READ->addOpCode("DGT",dgtFunc,1);
	FALU_READ->addOpCode("c_eq_d",deqFunc,1);
	FALU_READ->addOpCode("DEQ",deqFunc,1);
	FALU_READ->addOpCode("c_le_s",fleFunc,1);
	FALU_READ->addOpCode("FLE",fleFunc,1);
	FALU_READ->addOpCode("c_lt_s",fltFunc,1);
	FALU_READ->addOpCode("FLT",fltFunc,1);
	FALU_READ->addOpCode("c_ge_s",fgeFunc,1);
	FALU_READ->addOpCode("FGE",fgeFunc,1);
	FALU_READ->addOpCode("c_gt_s",fgtFunc,1);
	FALU_READ->addOpCode("FGT",fgtFunc,1);
	FALU_READ->addOpCode("c_eq_s",feqFunc,1);
	FALU_READ->addOpCode("FEQ",feqFunc,1);
	FALU_READ->addOpCode("li_d",dconstantFunc,1);
	FALU_READ->addOpCode("DCONSTANT",dconstantFunc,1);
	FALU_READ->addOpCode("li_s",fconstantFunc,1);
	FALU_READ->addOpCode("FCONSTANT",fconstantFunc,1);

	BR_READ->addOpCode("jr",returnFunc,1);
	BR_READ->addOpCode("RETURN",returnFunc,1);
	BR_READ->addOpCode("j",gotoFunc,1);
	BR_READ->addOpCode("IGOTO",gotoFunc,1);
	BR_READ->addOpCode("bnez",ifFunc,1);
	BR_READ->addOpCode("IF",ifFunc,1);
	BR_READ->addOpCode("bc1t",ifFunc,1);
	BR_READ->addOpCode("IFFT",ifFunc,1);
	BR_READ->addOpCode("jal",callFunc,1);
	BR_READ->addOpCode("CALL",callFunc,1);

	LDST_READ->addOpCode("la",iconstantFunc,1);
	LDST_READ->addOpCode("ICONSTANT",iconstantFunc,1);
	LDST_READ->addOpCode("lw",ivloadFunc,1);
	LDST_READ->addOpCode("IVLOAD",ivloadFunc,1);
	LDST_READ->addOpCode("sw",ivstoreFunc,1);
	LDST_READ->addOpCode("IVSTORE",ivstoreFunc,1);
	LDST_READ->addOpCode("lbu",qivloaduFunc,1);
	LDST_READ->addOpCode("QIVLOADU",qivloaduFunc,1);
	LDST_READ->addOpCode("lb",qivloadFunc,1);
	LDST_READ->addOpCode("QIVLOAD",qivloadFunc,1);
	LDST_READ->addOpCode("sbu",qivstoreuFunc,1);
	LDST_READ->addOpCode("QIVSTOREU",qivstoreuFunc,1);
	LDST_READ->addOpCode("sb",qivstoreFunc,1);
	LDST_READ->addOpCode("QIVSTORE",qivstoreFunc,1);
	LDST_READ->addOpCode("lhu",hivloadFunc,1);
	LDST_READ->addOpCode("HIVLOADU",hivloadFunc,1);
	LDST_READ->addOpCode("lh",hivloadFunc,1);
	LDST_READ->addOpCode("HIVLOAD",hivloadFunc,1);
	LDST_READ->addOpCode("shu",hivstoreuFunc,1);
	LDST_READ->addOpCode("HIVSTOREU",hivstoreuFunc,1);
	LDST_READ->addOpCode("sh",hivstoreFunc,1);
	LDST_READ->addOpCode("HIVSTORE",hivstoreFunc,1);
	LDST_READ->addOpCode("s_s",fvstoreFunc,1);
	LDST_READ->addOpCode("FVSTORE",fvstoreFunc,1);
	LDST_READ->addOpCode("l_s",fvloadFunc,1);
	LDST_READ->addOpCode("FVLOAD",fvloadFunc,1);
	LDST_READ->addOpCode("s_d",dvstoreFunc,1);
	LDST_READ->addOpCode("DVSTORE",dvstoreFunc,1);
	LDST_READ->addOpCode("l_d",dvloadFunc,1);
	LDST_READ->addOpCode("DVLOAD",dvloadFunc,1);

	ALU1_EX->addOpCode("dmfc1",dmfc1Func,1);
	ALU1_EX->addOpCode("DMFC1",dmfc1Func,1);
	ALU1_EX->addOpCode("dmtc1",dmtc1Func,1);
	ALU1_EX->addOpCode("DMTC1",dmtc1Func,1);
	ALU1_EX->addOpCode("cvt_s_w",cvtsiFunc,1);
	ALU1_EX->addOpCode("CVTSI",cvtsiFunc,1);
	ALU1_EX->addOpCode("xor",ixorFunc,1);
	ALU1_EX->addOpCode("IXOR",ixorFunc,1);
	ALU1_EX->addOpCode("and",iandFunc,1);
	ALU1_EX->addOpCode("IAND",iandFunc,1);
	ALU1_EX->addOpCode("cvt_s_d",cvtsdFunc,1);
	ALU1_EX->addOpCode("CVTSD",cvtsdFunc,1);
	ALU1_EX->addOpCode("cvt_d_s",cvtdsFunc,1);
	ALU1_EX->addOpCode("CVTDS",cvtdsFunc,1);
	ALU1_EX->addOpCode("cvt_d_w",cvtdiFunc,1);
	ALU1_EX->addOpCode("CVTDI",cvtdiFunc,1);
	ALU1_EX->addOpCode("trunc_w_s",truncisFunc,1);
	ALU1_EX->addOpCode("TRUNCIS",truncisFunc,1);
	ALU1_EX->addOpCode("trunc_w_d",truncidFunc,1);
	ALU1_EX->addOpCode("TRUNCID",truncidFunc,1);
	ALU1_EX->addOpCode("mfhi",mfhiFunc,1);
	ALU1_EX->addOpCode("MFHI",mfhiFunc,1);
	ALU1_EX->addOpCode("mflo",mfloFunc,1);
	ALU1_EX->addOpCode("MFLO",mfloFunc,1);
	ALU1_EX->addOpCode("mfc1",mfc1Func,1);
	ALU1_EX->addOpCode("MFC1",mfc1Func,1);
	ALU1_EX->addOpCode("mtc1",mtc1Func,1);
	ALU1_EX->addOpCode("MTC1",mtc1Func,1);
	ALU1_EX->addOpCode("sgtu",igtFunc,1);
	ALU1_EX->addOpCode("IGTU",igtFunc,1);
	ALU1_EX->addOpCode("sleu",ileFunc,1);
	ALU1_EX->addOpCode("ILEU",ileFunc,1);
	ALU1_EX->addOpCode("sltu",iltFunc,1);
	ALU1_EX->addOpCode("ILTU",iltFunc,1);
	ALU1_EX->addOpCode("li",iconstantFunc,1);
	ALU1_EX->addOpCode("ICONSTANT",iconstantFunc,1);
	ALU1_EX->addOpCode("div",idivMIPSFunc,1);
	ALU1_EX->addOpCode("IDIV",idivMIPSFunc,1);
	ALU1_EX->addOpCode("mult",imulMIPSFunc,1);
	ALU1_EX->addOpCode("IMUL",imulMIPSFunc,1);
	ALU1_EX->addOpCode("and",iandFunc,1);
	ALU1_EX->addOpCode("IAND",iandFunc,1);
	ALU1_EX->addOpCode("or",iorFunc,1);
	ALU1_EX->addOpCode("IOR",iorFunc,1);
	ALU1_EX->addOpCode("xori",ixorFunc,1);
	ALU1_EX->addOpCode("IXOR",ixorFunc,1);
	ALU1_EX->addOpCode("andi",iandFunc,1);
	ALU1_EX->addOpCode("IAND",iandFunc,1);
	ALU1_EX->addOpCode("ori",iorFunc,1);
	ALU1_EX->addOpCode("IOR",iorFunc,1);
	ALU1_EX->addOpCode("li_s",fconstantFunc,1);
	ALU1_EX->addOpCode("FCONSTANT",fconstantFunc,1);
	ALU1_EX->addOpCode("li_d",dconstantFunc,1);
	ALU1_EX->addOpCode("DCONSTANT",dconstantFunc,1);
	ALU1_EX->addOpCode("sgeu",igeFunc,1);
	ALU1_EX->addOpCode("IGEU",igeFunc,1);
	ALU1_EX->addOpCode("sne",ineFunc,1);
	ALU1_EX->addOpCode("INE",ineFunc,1);
	ALU1_EX->addOpCode("seq",ieqFunc,1);
	ALU1_EX->addOpCode("IEQ",ieqFunc,1);
	ALU1_EX->addOpCode("sgt",igtFunc,1);
	ALU1_EX->addOpCode("IGT",igtFunc,1);
	ALU1_EX->addOpCode("sle",ileFunc,1);
	ALU1_EX->addOpCode("ILE",ileFunc,1);
	ALU1_EX->addOpCode("slt",iltFunc,1);
	ALU1_EX->addOpCode("ILT",iltFunc,1);
	ALU1_EX->addOpCode("sge",igeFunc,1);
	ALU1_EX->addOpCode("IGE",igeFunc,1);
	ALU1_EX->addOpCode("sla",ilashFunc,1);
	ALU1_EX->addOpCode("ILASH",ilashFunc,1);
	ALU1_EX->addOpCode("sll",illshFunc,1);
	ALU1_EX->addOpCode("ILLSH",illshFunc,1);
	ALU1_EX->addOpCode("sra",irashFunc,1);
	ALU1_EX->addOpCode("IRASH",irashFunc,1);
	ALU1_EX->addOpCode("srl",irlshFunc,1);
	ALU1_EX->addOpCode("IRLSH",irlshFunc,1);
	ALU1_EX->addOpCode("move",iassignFunc,1);
	ALU1_EX->addOpCode("IASSIGN",iassignFunc,1);
	ALU1_EX->addOpCode("subu",isubFunc,1);
	ALU1_EX->addOpCode("ISUB",isubFunc,1);
	ALU1_EX->addOpCode("nop",nopFunc,1);
	ALU1_EX->addOpCode("NOP",nopFunc,1);
	ALU1_EX->addOpCode("addu",iaddFunc,1);
	ALU1_EX->addOpCode("IADD",iaddFunc,1);

	ALU2_EX->addOpCode("dmfc1",dmfc1Func,1);
	ALU2_EX->addOpCode("DMFC1",dmfc1Func,1);
	ALU2_EX->addOpCode("dmtc1",dmtc1Func,1);
	ALU2_EX->addOpCode("DMTC1",dmtc1Func,1);
	ALU2_EX->addOpCode("cvt_s_w",cvtsiFunc,1);
	ALU2_EX->addOpCode("CVTSI",cvtsiFunc,1);
	ALU2_EX->addOpCode("xor",ixorFunc,1);
	ALU2_EX->addOpCode("IXOR",ixorFunc,1);
	ALU2_EX->addOpCode("and",iandFunc,1);
	ALU2_EX->addOpCode("IAND",iandFunc,1);
	ALU2_EX->addOpCode("cvt_s_d",cvtsdFunc,1);
	ALU2_EX->addOpCode("CVTSD",cvtsdFunc,1);
	ALU2_EX->addOpCode("cvt_d_s",cvtdsFunc,1);
	ALU2_EX->addOpCode("CVTDS",cvtdsFunc,1);
	ALU2_EX->addOpCode("cvt_d_w",cvtdiFunc,1);
	ALU2_EX->addOpCode("CVTDI",cvtdiFunc,1);
	ALU2_EX->addOpCode("trunc_w_s",truncisFunc,1);
	ALU2_EX->addOpCode("TRUNCIS",truncisFunc,1);
	ALU2_EX->addOpCode("trunc_w_d",truncidFunc,1);
	ALU2_EX->addOpCode("TRUNCID",truncidFunc,1);
	ALU2_EX->addOpCode("mfhi",mfhiFunc,1);
	ALU2_EX->addOpCode("MFHI",mfhiFunc,1);
	ALU2_EX->addOpCode("mflo",mfloFunc,1);
	ALU2_EX->addOpCode("MFLO",mfloFunc,1);
	ALU2_EX->addOpCode("mfc1",mfc1Func,1);
	ALU2_EX->addOpCode("MFC1",mfc1Func,1);
	ALU2_EX->addOpCode("mtc1",mtc1Func,1);
	ALU2_EX->addOpCode("MTC1",mtc1Func,1);
	ALU2_EX->addOpCode("sgtu",igtFunc,1);
	ALU2_EX->addOpCode("IGTU",igtFunc,1);
	ALU2_EX->addOpCode("sleu",ileFunc,1);
	ALU2_EX->addOpCode("ILEU",ileFunc,1);
	ALU2_EX->addOpCode("sltu",iltFunc,1);
	ALU2_EX->addOpCode("ILTU",iltFunc,1);
	ALU2_EX->addOpCode("li",iconstantFunc,1);
	ALU2_EX->addOpCode("ICONSTANT",iconstantFunc,1);
	ALU2_EX->addOpCode("div",idivMIPSFunc,1);
	ALU2_EX->addOpCode("IDIV",idivMIPSFunc,1);
	ALU2_EX->addOpCode("mult",imulMIPSFunc,1);
	ALU2_EX->addOpCode("IMUL",imulMIPSFunc,1);
	ALU2_EX->addOpCode("and",iandFunc,1);
	ALU2_EX->addOpCode("IAND",iandFunc,1);
	ALU2_EX->addOpCode("or",iorFunc,1);
	ALU2_EX->addOpCode("IOR",iorFunc,1);
	ALU2_EX->addOpCode("xori",ixorFunc,1);
	ALU2_EX->addOpCode("IXOR",ixorFunc,1);
	ALU2_EX->addOpCode("andi",iandFunc,1);
	ALU2_EX->addOpCode("IAND",iandFunc,1);
	ALU2_EX->addOpCode("ori",iorFunc,1);
	ALU2_EX->addOpCode("IOR",iorFunc,1);
	ALU2_EX->addOpCode("li_s",fconstantFunc,1);
	ALU2_EX->addOpCode("FCONSTANT",fconstantFunc,1);
	ALU2_EX->addOpCode("li_d",dconstantFunc,1);
	ALU2_EX->addOpCode("DCONSTANT",dconstantFunc,1);
	ALU2_EX->addOpCode("sgeu",igeFunc,1);
	ALU2_EX->addOpCode("IGEU",igeFunc,1);
	ALU2_EX->addOpCode("sne",ineFunc,1);
	ALU2_EX->addOpCode("INE",ineFunc,1);
	ALU2_EX->addOpCode("seq",ieqFunc,1);
	ALU2_EX->addOpCode("IEQ",ieqFunc,1);
	ALU2_EX->addOpCode("sgt",igtFunc,1);
	ALU2_EX->addOpCode("IGT",igtFunc,1);
	ALU2_EX->addOpCode("sle",ileFunc,1);
	ALU2_EX->addOpCode("ILE",ileFunc,1);
	ALU2_EX->addOpCode("slt",iltFunc,1);
	ALU2_EX->addOpCode("ILT",iltFunc,1);
	ALU2_EX->addOpCode("sge",igeFunc,1);
	ALU2_EX->addOpCode("IGE",igeFunc,1);
	ALU2_EX->addOpCode("sla",ilashFunc,1);
	ALU2_EX->addOpCode("ILASH",ilashFunc,1);
	ALU2_EX->addOpCode("sll",illshFunc,1);
	ALU2_EX->addOpCode("ILLSH",illshFunc,1);
	ALU2_EX->addOpCode("sra",irashFunc,1);
	ALU2_EX->addOpCode("IRASH",irashFunc,1);
	ALU2_EX->addOpCode("srl",irlshFunc,1);
	ALU2_EX->addOpCode("IRLSH",irlshFunc,1);
	ALU2_EX->addOpCode("move",iassignFunc,1);
	ALU2_EX->addOpCode("IASSIGN",iassignFunc,1);
	ALU2_EX->addOpCode("subu",isubFunc,1);
	ALU2_EX->addOpCode("ISUB",isubFunc,1);
	ALU2_EX->addOpCode("nop",nopFunc,1);
	ALU2_EX->addOpCode("NOP",nopFunc,1);
	ALU2_EX->addOpCode("addu",iaddFunc,1);
	ALU2_EX->addOpCode("IADD",iaddFunc,1);

	FALU_EX->addOpCode("mul_d",dmulFunc,1);
	FALU_EX->addOpCode("DMUL",dmulFunc,1);
	FALU_EX->addOpCode("add_d",daddFunc,1);
	FALU_EX->addOpCode("DADD",daddFunc,1);
	FALU_EX->addOpCode("sub_d",dsubFunc,1);
	FALU_EX->addOpCode("DSUB",dsubFunc,1);
	FALU_EX->addOpCode("div_d",ddivFunc,1);
	FALU_EX->addOpCode("DDIV",ddivFunc,1);
	FALU_EX->addOpCode("sub_s",fsubFunc,1);
	FALU_EX->addOpCode("FSUB",fsubFunc,1);
	FALU_EX->addOpCode("add_s",faddFunc,1);
	FALU_EX->addOpCode("FADD",faddFunc,1);
	FALU_EX->addOpCode("mul_s",fmulFunc,1);
	FALU_EX->addOpCode("FMUL",fmulFunc,1);
	FALU_EX->addOpCode("div_s",fdivFunc,1);
	FALU_EX->addOpCode("FDIV",fdivFunc,1);
	FALU_EX->addOpCode("mov_d",dassignFunc,1);
	FALU_EX->addOpCode("DASSIGN",dassignFunc,1);
	FALU_EX->addOpCode("mov_s",fassignFunc,1);
	FALU_EX->addOpCode("FASSIGN",fassignFunc,1);
	FALU_EX->addOpCode("c_le_d",dleFunc,1);
	FALU_EX->addOpCode("DLE",dleFunc,1);
	FALU_EX->addOpCode("c_lt_d",dltFunc,1);
	FALU_EX->addOpCode("DLT",dltFunc,1);
	FALU_EX->addOpCode("c_ge_d",dgeFunc,1);
	FALU_EX->addOpCode("DGE",dgeFunc,1);
	FALU_EX->addOpCode("c_gt_d",dgtFunc,1);
	FALU_EX->addOpCode("DGT",dgtFunc,1);
	FALU_EX->addOpCode("c_eq_d",deqFunc,1);
	FALU_EX->addOpCode("DEQ",deqFunc,1);
	FALU_EX->addOpCode("c_le_s",fleFunc,1);
	FALU_EX->addOpCode("FLE",fleFunc,1);
	FALU_EX->addOpCode("c_lt_s",fltFunc,1);
	FALU_EX->addOpCode("FLT",fltFunc,1);
	FALU_EX->addOpCode("c_ge_s",fgeFunc,1);
	FALU_EX->addOpCode("FGE",fgeFunc,1);
	FALU_EX->addOpCode("c_gt_s",fgtFunc,1);
	FALU_EX->addOpCode("FGT",fgtFunc,1);
	FALU_EX->addOpCode("c_eq_s",feqFunc,1);
	FALU_EX->addOpCode("FEQ",feqFunc,1);
	FALU_EX->addOpCode("li_d",dconstantFunc,1);
	FALU_EX->addOpCode("DCONSTANT",dconstantFunc,1);
	FALU_EX->addOpCode("li_s",fconstantFunc,1);
	FALU_EX->addOpCode("FCONSTANT",fconstantFunc,1);

	BR_EX->addOpCode("jr",returnFunc,1);
	BR_EX->addOpCode("RETURN",returnFunc,1);
	BR_EX->addOpCode("j",gotoFunc,1);
	BR_EX->addOpCode("IGOTO",gotoFunc,1);
	BR_EX->addOpCode("bnez",ifFunc,1);
	BR_EX->addOpCode("IF",ifFunc,1);
	BR_EX->addOpCode("bc1t",ifFunc,1);
	BR_EX->addOpCode("IFFT",ifFunc,1);
	BR_EX->addOpCode("jal",callFunc,1);
	BR_EX->addOpCode("CALL",callFunc,1);
	// BR_EX->addOpCode("DEBUG_PRINT",debug_printFunc);

	LDST_EX->addOpCode("la",iconstantFunc,1);
	LDST_EX->addOpCode("ICONSTANT",iconstantFunc,1);
	LDST_EX->addOpCode("lw",ivloadFunc,1);
	LDST_EX->addOpCode("IVLOAD",ivloadFunc,1);
	LDST_EX->addOpCode("sw",ivstoreFunc,1);
	LDST_EX->addOpCode("IVSTORE",ivstoreFunc,1);
	LDST_EX->addOpCode("lbu",qivloaduFunc,1);
	LDST_EX->addOpCode("QIVLOADU",qivloaduFunc,1);
	LDST_EX->addOpCode("lb",qivloadFunc,1);
	LDST_EX->addOpCode("QIVLOAD",qivloadFunc,1);
	LDST_EX->addOpCode("sbu",qivstoreuFunc,1);
	LDST_EX->addOpCode("QIVSTOREU",qivstoreuFunc,1);
	LDST_EX->addOpCode("sb",qivstoreFunc,1);
	LDST_EX->addOpCode("QIVSTORE",qivstoreFunc,1);
	LDST_EX->addOpCode("lhu",hivloadFunc,1);
	LDST_EX->addOpCode("HIVLOADU",hivloadFunc,1);
	LDST_EX->addOpCode("lh",hivloadFunc,1);
	LDST_EX->addOpCode("HIVLOAD",hivloadFunc,1);
	LDST_EX->addOpCode("shu",hivstoreuFunc,1);
	LDST_EX->addOpCode("HIVSTOREU",hivstoreuFunc,1);
	LDST_EX->addOpCode("sh",hivstoreFunc,1);
	LDST_EX->addOpCode("HIVSTORE",hivstoreFunc,1);
	LDST_EX->addOpCode("s_s",fvstoreFunc,1);
	LDST_EX->addOpCode("FVSTORE",fvstoreFunc,1);
	LDST_EX->addOpCode("l_s",fvloadFunc,1);
	LDST_EX->addOpCode("FVLOAD",fvloadFunc,1);
	LDST_EX->addOpCode("s_d",dvstoreFunc,1);
	LDST_EX->addOpCode("DVSTORE",dvstoreFunc,1);
	LDST_EX->addOpCode("l_d",dvloadFunc,1);
	LDST_EX->addOpCode("DVLOAD",dvloadFunc,1);



//ADD DATA TRANSFER PATHS

	Alu1ReadPort1->addConnection(FprReadPort6ALU1ReadPort1);
	FprReadPort6ALU1ReadPort1->addPort(FprReadPort6);
	FprReadPort6->addPtrToStorage(FPRFile);

	Alu1ReadPort2->addConnection(FprReadPort7ALU1ReadPort2Cxn);
	FprReadPort7ALU1ReadPort2Cxn->addPort(FprReadPort7);
	FprReadPort7->addPtrToStorage(FPRFile);

	Alu2ReadPort1->addConnection(FprReadPort1Alu2ReadPort1Cxn);
	FprReadPort1Alu2ReadPort1Cxn->addPort(FprReadPort1);
	FprReadPort1->addPtrToStorage(FPRFile);

	Alu2ReadPort2->addConnection(FprReadPort2Alu2ReadPort2Cxn);
	FprReadPort2Alu2ReadPort2Cxn->addPort(FprReadPort2);
	FprReadPort2->addPtrToStorage(FPRFile);

	FaluReadPort1->addConnection(FprReadPort3FaluReadPort1Cxn);
	FprReadPort3FaluReadPort1Cxn->addPort(FprReadPort3);
	FprReadPort3->addPtrToStorage(FPRFile);

	FaluReadPort2->addConnection(FprReadPort4FaluReadPort2Cxn);
	FprReadPort4FaluReadPort2Cxn->addPort(FprReadPort4);
	FprReadPort4->addPtrToStorage(FPRFile);

	LdStReadPort3->addConnection(FprReadPort5LdStReadPort3Cxn);
	FprReadPort5LdStReadPort3Cxn->addPort(FprReadPort5);
	FprReadPort5->addPtrToStorage(FPRFile);

	Alu1ReadPort1->addConnection(GprReadPort1Alu1ReadPort1Cxn);
	GprReadPort1Alu1ReadPort1Cxn->addPort(GprReadPort1);
	GprReadPort1->addPtrToStorage(GPRFile);

	Alu1ReadPort2->addConnection(GprReadPort2Alu1ReadPort2Cxn);
	GprReadPort2Alu1ReadPort2Cxn->addPort(GprReadPort2);
	GprReadPort2->addPtrToStorage(GPRFile);

	Alu2ReadPort1->addConnection(GprReadPort3Alu2ReadPort1Cxn);
	GprReadPort3Alu2ReadPort1Cxn->addPort(GprReadPort3);
	GprReadPort3->addPtrToStorage(GPRFile);

	Alu2ReadPort2->addConnection(GprReadPort4Alu2ReadPort2Cxn);
	GprReadPort4Alu2ReadPort2Cxn->addPort(GprReadPort4);
	GprReadPort4->addPtrToStorage(GPRFile);

	BrReadPort1->addConnection(GprReadPort5BrReadPort1Cxn);
	GprReadPort5BrReadPort1Cxn->addPort(GprReadPort5);
	GprReadPort5->addPtrToStorage(GPRFile);

	BrReadPort2->addConnection(GprReadPort6BrReadPort2Cxn);
	GprReadPort6BrReadPort2Cxn->addPort(GprReadPort6);
	GprReadPort6->addPtrToStorage(GPRFile);

	LdStReadPort1->addConnection(GprReadPort7LdStReadPort1Cxn);
	GprReadPort7LdStReadPort1Cxn->addPort(GprReadPort7);
	GprReadPort7->addPtrToStorage(GPRFile);

	LdStReadPort2->addConnection(GprReadPort8LdStReadPort2Cxn);
	GprReadPort8LdStReadPort2Cxn->addPort(GprReadPort8);
	GprReadPort8->addPtrToStorage(GPRFile);

	LdStReadPort3->addConnection(GprReadPort9LdStReadPort3Cxn);
	GprReadPort9LdStReadPort3Cxn->addPort(GprReadPort9);
	GprReadPort9->addPtrToStorage(GPRFile);

	LdStReadWritePort->addConnection(LdStMemCxn);
	LdStMemCxn->addPort(L1ReadWritePort);
	L1ReadWritePort->addPtrToStorage(L1);

	WbWritePort->addConnection(WbWritePortFprWritePortCxn);
	WbWritePortFprWritePortCxn->addPort(FprWritePort);
	FprWritePort->addPtrToStorage(FPRFile);

	WbWritePort->addConnection(WbWritePortGprWritePortCxn);
	WbWritePortGprWritePortCxn->addPort(GprWritePort);
	GprWritePort->addPtrToStorage(GPRFile);


//SETUP THE VLIW INSTRUCTION TEMPLATE

#define SLOT_1 0
#define SLOT_2 1
#define SLOT_3 2
#define SLOT_4 3

	simpleVLIWInstrTemplate = new Simple_VLIW_Instr();
	simpleVLIWInstrTemplate->addSlot(DATA_OP_SLOT,ALU1_EX,SLOT_1);
	simpleVLIWInstrTemplate->addSlot(DATA_OP_SLOT,ALU2_EX,SLOT_2);
	simpleVLIWInstrTemplate->addSlot(DATA_OP_SLOT,FALU_EX,SLOT_3);
	simpleVLIWInstrTemplate->addSlot(DATA_OP_SLOT,LDST_EX,SLOT_4);
	simpleVLIWInstrTemplate->addSlot(CONTROL_OP_SLOT,BR_EX,SLOT_1);

	numDataOpsPerInstr = 4;
	numControlOpsPerInstr = NUM_CONTROL_OPS_PER_INSTR;
	numFlowOpsPerInstr = NUM_FLOW_OPS_PER_INSTR;

	globalControlTable.addUnitToTable(FETCH,0,0);

	globalControlTable.addUnitToTable(DECODE,1,0);

	globalControlTable.addUnitToTable(ALU1_READ,2,0);
	globalControlTable.addUnitToTable(ALU2_READ,2,1);
	globalControlTable.addUnitToTable(FALU_READ,2,2);
	globalControlTable.addUnitToTable(BR_READ,2,3);
	globalControlTable.addUnitToTable(LDST_READ,2,4);

	globalControlTable.addUnitToTable(ALU1_EX,3,0);
	globalControlTable.addUnitToTable(ALU2_EX,3,1);
	globalControlTable.addUnitToTable(FALU_EX,3,2);
	globalControlTable.addUnitToTable(BR_EX,3,3);
	globalControlTable.addUnitToTable(LDST_EX,3,4);

	globalControlTable.addUnitToTable(WB,4,0);

	globalNetList.addUnit(FETCH, 0, 0);
	globalNetList.addUnit(DECODE, 0, 0);
	globalNetList.addUnit(ALU1_READ, 0, 0);
	globalNetList.addUnit(ALU2_READ, 0, 0);
	globalNetList.addUnit(FALU_READ, 0, 0);
	globalNetList.addUnit(BR_READ, 0, 0);
	globalNetList.addUnit(LDST_READ, 0, 0);
	globalNetList.addUnit(ALU1_EX, 0, 0);
	globalNetList.addUnit(ALU2_EX, 0, 0);
	globalNetList.addUnit(FALU_EX, 0, 0);
	globalNetList.addUnit(BR_EX, 0, 0);
	globalNetList.addUnit(LDST_EX, 0, 0);
	globalNetList.addUnit(WB, 0, 0);

	globalNetList.setPredecessor(DECODE,FETCH);
	globalNetList.setPredecessor(ALU1_READ,DECODE);
	globalNetList.setPredecessor(ALU2_READ,DECODE);
	globalNetList.setPredecessor(FALU_READ,DECODE);
	globalNetList.setPredecessor(BR_READ,DECODE);
	globalNetList.setPredecessor(LDST_READ,DECODE);
	globalNetList.setPredecessor(ALU1_EX,ALU1_READ);
	globalNetList.setPredecessor(ALU2_EX,ALU2_READ);
	globalNetList.setPredecessor(FALU_EX,FALU_READ);
	globalNetList.setPredecessor(BR_EX,BR_READ);
	globalNetList.setPredecessor(LDST_EX,LDST_READ);
	globalNetList.setPredecessor(WB,ALU1_EX);
	globalNetList.setPredecessor(WB,ALU2_EX);
	globalNetList.setPredecessor(WB,FALU_EX);
	globalNetList.setPredecessor(WB,BR_EX);
	globalNetList.setPredecessor(WB,LDST_EX);

	globalNetList.setSuccessor(FETCH,DECODE);
	globalNetList.setSuccessor(DECODE,ALU1_READ);
	globalNetList.setSuccessor(DECODE,ALU2_READ);
	globalNetList.setSuccessor(DECODE,FALU_READ);
	globalNetList.setSuccessor(DECODE,LDST_READ);
	globalNetList.setSuccessor(DECODE,BR_READ);
	globalNetList.setSuccessor(ALU1_READ,ALU1_EX);
	globalNetList.setSuccessor(ALU2_READ,ALU2_EX);
	globalNetList.setSuccessor(FALU_READ,FALU_EX);
	globalNetList.setSuccessor(BR_READ,BR_EX);
	globalNetList.setSuccessor(LDST_READ,LDST_EX);
	globalNetList.setSuccessor(ALU1_EX,WB);
	globalNetList.setSuccessor(ALU2_EX,WB);
	globalNetList.setSuccessor(FALU_EX,WB);
	globalNetList.setSuccessor(BR_EX,WB);
	globalNetList.setSuccessor(LDST_EX,WB);

createForwardingTable();

globalControlTable.initGlobalFUList();
globalControlTable.createFUandRFConnTable();


}


void Connectivity::initialize()
{

  // open the memory config file to read the following:
  // 1. memory modules and their parameters. (between $begin_mem_modules and $end_mem_modules)
  //    syntax: module_number(int) module_type(string) module_parameters(par1:par2:par3)
  // 2. connectivity between modules (between $begin_connectivity and $end_connectivity)
  //    syntax: module_from(int) module_to(int)
  // 3. Address mapping (between $begin_address_map and $end_address_map )
  //    syntax: module_number start_address(int) end_address(int) 


	FILE *fp;
	char line[80];

	fp = fopen(memConfigFile.getStr(),"r");

	while (fgets(line,80,fp) != NULL) {
		if (!strcmp(line,"BEGIN_MEM_MODULES\n")) break;
	}
	
	// instatiate the memory modules described in the config file
	while (fgets(line,80,fp) != NULL) {

		if (!strcmp(line,"\n")) continue;

		if (!strcmp(line,"END_MEM_MODULES\n")) break;

		int module_number;
		char module_type[20], module_parameters[40];
		sscanf(line,"%d%s%s",&module_number,module_type,module_parameters);
		n_modules++;
		if (!strcmp(module_type,"DCACHE") || !strcmp(module_type,"dcache")) {
			// syntax - lines:words_per_line:no_of_ways:word_size:latency
			int lines = atoi(strtok(module_parameters,":"));
			int words = atoi(strtok(NULL,":"));
			int ways = atoi(strtok(NULL,":"));
			int wordsize = atoi(strtok(NULL,":"));
			int latency = atoi(strtok(NULL,":"));
		
			MemoryModules[module_number]=(MemoryModule*)new AssociativeCache(lines, words, ways, wordsize, latency);
		
		} else
			if (!strcmp(module_type,"ICACHE") || !strcmp(module_type,"icache")) {
			// syntax - lines:words_per_line:no_of_ways:word_size:latency
			int lines = atoi(strtok(module_parameters,":"));
			int words = atoi(strtok(NULL,":"));
			int ways = atoi(strtok(NULL,":"));
			int wordsize = atoi(strtok(NULL,":"));
			int latency = atoi(strtok(NULL,":"));
		
			MemoryModules[module_number]=(MemoryModule*)new AssociativeCache(lines, words, ways, wordsize, latency);
			icachePtr = MemoryModules[module_number];
		
		} else
			if (!strcmp(module_type,"SRAM") || !strcmp(module_type,"sram")) {
				// syntax - size:latency
				int size = atoi(strtok(module_parameters,":"));
				int latency = atoi(strtok(NULL,":"));

				MemoryModules[module_number]=(MemoryModule*)new Sram(latency);
			
			} else 
				if (!strcmp(module_type,"STREAM_BUFFER") || !strcmp(module_type,"stream_buffer")) {
					// syntax - lines:words_per_line:no_of_ways:word_size:latency
					int lines = atoi(strtok(module_parameters,":"));
					int words = atoi(strtok(NULL,":"));
					int ways = atoi(strtok(NULL,":"));
					int wordsize = atoi(strtok(NULL,":"));
					int latency = atoi(strtok(NULL,":"));

					MemoryModules[module_number]=(MemoryModule*)new StreamBuffer(lines, words, ways, wordsize, latency);
				} else
					if (!strcmp(module_type,"DRAM") || !strcmp(module_type,"dram")) {
					// syntax - size:latency
					int size = atoi(strtok(module_parameters,":"));
					int latency = atoi(strtok(NULL,":"));

					MemoryModules[module_number]=(MemoryModule*)new Dram(latency);
					mainMemory = MemoryModules[module_number];

			
				} else {
					cout << "Panic: Invalid memory module " << module_type  << " specified " << endl;
					assert(0);
				}
	}

	while (fgets(line,80,fp) != NULL) {
		if (!strcmp(line,"BEGIN_CONNECTIVITY\n")) break;
	}
	
	// instatiate the memory modules described in the config file
	while (fgets(line,80,fp) != NULL) {

		if (!strcmp(line,"\n")) continue;

		if (!strcmp(line,"END_CONNECTIVITY\n")) break;
		
		int from, to;
		sscanf(line,"%d%d",&from,&to);
		addConnection(from,to);
	}

	while (fgets(line,80,fp) != NULL) {
		if (!strcmp(line,"BEGIN_MEMORY_MAP\n")) break;
	}


	address_map = NULL;
	addr_map_node *tmp;
	
	while (fgets(line,80,fp) != NULL) {

		if (!strcmp(line,"\n")) continue;

		if (!strcmp(line,"END_MEMORY_MAP\n")) break;
		
		// Syntax: module_number(int), start_address(int), end_address (int)
		int module_number, address_start, address_end;

		sscanf(line,"%d%d%d",&module_number,&address_start,&address_end);
		tmp = (addr_map_node *)malloc(sizeof(addr_map_node));
		tmp->start = address_start;
		tmp->end = address_end;
		tmp->module_number = module_number;
		tmp->next = address_map;
		address_map = tmp;
	}

	fclose(fp);
	// Temporarily hardcoding this variable...
	__L2_LINE_SIZE__ = 16; // Used in DRAM.h for power calculations.

}


//implements the mapping between the address range and the memory modules
//returns the memory module corresponding to an address
MemoryModule* Connectivity::get_module_for_address(long addr)
{

  int memory_module = -1;
  addr_map_node *tmp = address_map;
  while (tmp != NULL) {
  	if ((addr >= tmp->start) && (addr <= tmp->end)) {
		memory_module = tmp->module_number;
		break;
	}
	tmp = tmp->next;
  }

  if (memory_module < 0) {
	  cout << " Address location "<<addr<<" out of range (not found in address map) " << endl;
  }	  
  return MemoryModules[memory_module];

}


extern void initializeMemory(Storage *);
extern void initializeStackPointer(Storage *);

void Connectivity::initializeMemorySubsystem()
{
	//initializeMemory(toyMem);
	for (int i = 0; i < n_modules; i++)
	{
		MemoryModules[i]->init();
	}
}

void initializeMemory(long strtAddr, char *ldType, char *ldFileName)
{
	//initializeMemory(toyMem);
	mainMemory->initmem(strtAddr, ldType, ldFileName);
	//initializeStackPointer(IntRegFile);
}

extern void initializeArgc(Storage *regFile);
extern void initializeArgv(Storage *regFile, Storage *toyMem);




struct slot_type{
	int nr;
	SlotType type;
};

//the physical to virtual slot mappings
map<int,set<slot_type*>*> slot_mappings;

//reads the slot mapping file
void read_slot_mappings(void){
  char line[1000],word[1000];
  int n;
  FILE *f;
  int nr,current_phys_slot;
  slot_type *s;

  f=fopen("SlotMappings.txt","r");

  if(f==NULL){
    printf("Could not open SlotMappings.txt\n");
    exit(1);
  }

  while(!feof(f)){
    fgets(line,1000,f);
    if(line[0]!='#'){
      n=sscanf(line,"%s %d\n",word,&nr);
      if(n==2){
		  if(strcmp(word,"physical_slot")==0){
			  current_phys_slot=nr;
			  slot_mappings[current_phys_slot]=new set<slot_type*>();
		  }
		  else if(strcmp(word,"data")==0){
			  //this is a data virtual slot
			  s=new slot_type();
			  s->nr=nr;
			  s->type=DATA_OP_SLOT;
			  slot_mappings[current_phys_slot]->insert(s);
		  }
		  else if(strcmp(word,"control")==0){
			  //this is a control slot
			  s=new slot_type();
			  s->nr=nr;
			  s->type=CONTROL_OP_SLOT;
			  slot_mappings[current_phys_slot]->insert(s);
		  }
		  else _ASSERT(0);
      }
    }
  }
  fclose(f);
}

int isDelayedRead(BaseArgument *arg, BaseOperation *op)
{
	return false;
}
