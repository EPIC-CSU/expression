# Microsoft Developer Studio Project File - Name="acesMIPS Derived Class Lib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=acesMIPS Derived Class Lib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "acesMIPS Derived Class Lib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "acesMIPS Derived Class Lib.mak" CFG="acesMIPS Derived Class Lib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "acesMIPS Derived Class Lib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "acesMIPS Derived Class Lib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "acesMIPS Derived Class Lib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "acesMIPS Derived Class Lib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /GR /GX /Z7 /Od /I "../../pcProDll/include" /I "../../pcProDll/disamb" /I "../../pcProDll/mutation" /I "../../systemDll/acesMIPSsimulatorFuncDll" /I "../../systemDll/NewBaseClassDll" /I "../../systemDll/Memory Subsystem" /I "../../pcProDll/regalloc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "WIN32_GUI" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "acesMIPS Derived Class Lib - Win32 Release"
# Name "acesMIPS Derived Class Lib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Memory Subsystem Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\Memory Subsystem\Connectivity.cpp"
# End Source File
# Begin Source File

SOURCE="..\Memory Subsystem\MemData.cpp"
# End Source File
# Begin Source File

SOURCE="..\Memory Subsystem\MemHierarchy.cpp"
# End Source File
# Begin Source File

SOURCE="..\Memory Subsystem\MemorySubsystem.cpp"
# End Source File
# End Group
# Begin Source File

SOURCE=..\acesMIPSderivedClassDll\ControlTable.cpp
# End Source File
# Begin Source File

SOURCE=..\acesMIPSderivedClassDll\DerivedAddrData.cpp
# End Source File
# Begin Source File

SOURCE=..\acesMIPSderivedClassDll\DerivedOpExecSemantics.cpp
# End Source File
# Begin Source File

SOURCE=..\acesMIPSderivedClassDll\DerivedPort.cpp
# End Source File
# Begin Source File

SOURCE=..\acesMIPSderivedClassDll\DerivedUnit.cpp
# End Source File
# Begin Source File

SOURCE=..\acesMIPSderivedClassDll\DynamicScheduler.cpp
# End Source File
# Begin Source File

SOURCE=..\acesMIPSderivedClassDll\ExecSemantics.cpp
# End Source File
# Begin Source File

SOURCE=..\acesMIPSderivedClassDll\ExecSemSupportFuncs.cpp
# End Source File
# Begin Source File

SOURCE=..\acesMIPSderivedClassDll\ExpressCallSupport.cpp
# End Source File
# Begin Source File

SOURCE=..\acesMIPSderivedClassDll\ExpressDisplayFuncs.cpp
# End Source File
# Begin Source File

SOURCE=..\acesMIPSderivedClassDll\FUandOpcode.cpp
# End Source File
# Begin Source File

SOURCE=..\acesMIPSderivedClassDll\IssueUnit.cpp
# End Source File
# Begin Source File

SOURCE=..\acesMIPSderivedClassDll\PowersimSupportFuncs.cpp
# End Source File
# Begin Source File

SOURCE=..\acesMIPSderivedClassDll\SystemCallSupport.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "Memory Subsystem Header"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\Memory Subsystem\AssociativeCache.h"
# End Source File
# Begin Source File

SOURCE="..\Memory Subsystem\Cache.h"
# End Source File
# Begin Source File

SOURCE="..\Memory Subsystem\Cacti.h"
# End Source File
# Begin Source File

SOURCE="..\Memory Subsystem\Connectivity.h"
# End Source File
# Begin Source File

SOURCE="..\Memory Subsystem\DirectCache.h"
# End Source File
# Begin Source File

SOURCE="..\Memory Subsystem\Dram.h"
# End Source File
# Begin Source File

SOURCE="..\Memory Subsystem\MemData.h"
# End Source File
# Begin Source File

SOURCE="..\Memory Subsystem\MemDefines.h"
# End Source File
# Begin Source File

SOURCE="..\Memory Subsystem\MemoryModule.h"
# End Source File
# Begin Source File

SOURCE="..\Memory Subsystem\MemorySubsystem.h"
# End Source File
# Begin Source File

SOURCE="..\Memory Subsystem\power.h"
# End Source File
# Begin Source File

SOURCE="..\Memory Subsystem\SmartDram.h"
# End Source File
# Begin Source File

SOURCE="..\Memory Subsystem\Sram.h"
# End Source File
# Begin Source File

SOURCE="..\Memory Subsystem\StreamBuffer.h"
# End Source File
# End Group
# Begin Source File

SOURCE=..\acesMIPSderivedClassDll\AddrQueueInsn.h
# End Source File
# Begin Source File

SOURCE=..\acesMIPSderivedClassDll\Cache.h
# End Source File
# Begin Source File

SOURCE=..\acesMIPSderivedClassDll\ControlTable.h
# End Source File
# Begin Source File

SOURCE=..\acesMIPSderivedClassDll\DataHazardDetector.h
# End Source File
# Begin Source File

SOURCE=..\acesMIPSderivedClassDll\DerivedAddrData.h
# End Source File
# Begin Source File

SOURCE=..\acesMIPSderivedClassDll\DerivedClassIncludes.h
# End Source File
# Begin Source File

SOURCE=..\acesMIPSderivedClassDll\DerivedDataTransSem.h
# End Source File
# Begin Source File

SOURCE=..\acesMIPSderivedClassDll\DerivedLatch.h
# End Source File
# Begin Source File

SOURCE=..\acesMIPSderivedClassDll\DerivedLatchData.h
# End Source File
# Begin Source File

SOURCE=..\acesMIPSderivedClassDll\DerivedOpExecSemantics.h
# End Source File
# Begin Source File

SOURCE=..\acesMIPSderivedClassDll\DerivedPort.h
# End Source File
# Begin Source File

SOURCE=..\acesMIPSderivedClassDll\DerivedStorage.h
# End Source File
# Begin Source File

SOURCE=..\acesMIPSderivedClassDll\DerivedUnit.h
# End Source File
# Begin Source File

SOURCE=..\acesMIPSderivedClassDll\DynamicScheduler.h
# End Source File
# Begin Source File

SOURCE=..\acesMIPSderivedClassDll\ExecSemantics.h
# End Source File
# Begin Source File

SOURCE=..\acesMIPSderivedClassDll\ExecSemSupportFuncs.h
# End Source File
# Begin Source File

SOURCE=..\acesMIPSderivedClassDll\ExpressAddrData.h
# End Source File
# Begin Source File

SOURCE=..\acesMIPSderivedClassDll\ExpressIncludes.h
# End Source File
# Begin Source File

SOURCE=..\acesMIPSderivedClassDll\IssueUnit.h
# End Source File
# Begin Source File

SOURCE=..\acesMIPSderivedClassDll\PowersimSupportFuncs.h
# End Source File
# Begin Source File

SOURCE=..\acesMIPSderivedClassDll\ReservationStation.h
# End Source File
# Begin Source File

SOURCE=..\acesMIPSderivedClassDll\SimCall.h
# End Source File
# Begin Source File

SOURCE=..\acesMIPSderivedClassDll\SysCallExecSem.h
# End Source File
# End Group
# End Target
# End Project
