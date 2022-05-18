# Microsoft Developer Studio Project File - Name="acesMIPS Base Class Lib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=acesMIPS Base Class Lib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "acesMIPS Base Class Lib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "acesMIPS Base Class Lib.mak" CFG="acesMIPS Base Class Lib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "acesMIPS Base Class Lib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "acesMIPS Base Class Lib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "acesMIPS Base Class Lib - Win32 Release"

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

!ELSEIF  "$(CFG)" == "acesMIPS Base Class Lib - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /GR /GX /ZI /Od /I "../../pcProDll/include" /I "../../pcProDll/disamb" /I "../../pcProDll/mutation" /I "../../pcProDll/instrsel" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "WIN32_GUI" /YX /FD /c
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

# Name "acesMIPS Base Class Lib - Win32 Release"
# Name "acesMIPS Base Class Lib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\NewBaseClassDll\AddrData.cpp
# End Source File
# Begin Source File

SOURCE=..\NewBaseClassDll\ArchStats.cpp
# End Source File
# Begin Source File

SOURCE=..\NewBaseClassDll\BaseSimObject.cpp
# End Source File
# Begin Source File

SOURCE=..\NewBaseClassDll\CompName.cpp
# End Source File
# Begin Source File

SOURCE=..\NewBaseClassDll\Connection.cpp
# End Source File
# Begin Source File

SOURCE=..\NewBaseClassDll\DataConversion.cpp
# End Source File
# Begin Source File

SOURCE=..\NewBaseClassDll\Latch.cpp
# End Source File
# Begin Source File

SOURCE=..\NewBaseClassDll\NewBaseClassDll.cpp
# End Source File
# Begin Source File

SOURCE=..\NewBaseClassDll\OpCodeList.cpp
# End Source File
# Begin Source File

SOURCE=..\NewBaseClassDll\Port.cpp
# End Source File
# Begin Source File

SOURCE=..\NewBaseClassDll\StatList.cpp
# End Source File
# Begin Source File

SOURCE=..\NewBaseClassDll\StdAfx.cpp
# End Source File
# Begin Source File

SOURCE=..\NewBaseClassDll\Storage.cpp
# End Source File
# Begin Source File

SOURCE=..\NewBaseClassDll\SupportFuncs.cpp
# End Source File
# Begin Source File

SOURCE=..\NewBaseClassDll\Unit.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\NewBaseClassDll\AddrData.h
# End Source File
# Begin Source File

SOURCE=..\NewBaseClassDll\ArchClassIncludes.h
# End Source File
# Begin Source File

SOURCE=..\NewBaseClassDll\ArchGlobalIncludes.h
# End Source File
# Begin Source File

SOURCE=..\NewBaseClassDll\ArchStats.h
# End Source File
# Begin Source File

SOURCE=..\NewBaseClassDll\ArchSTLIncludes.h
# End Source File
# Begin Source File

SOURCE=..\NewBaseClassDll\BaseOpExecSemantics.h
# End Source File
# Begin Source File

SOURCE=..\NewBaseClassDll\BaseSimObject.h
# End Source File
# Begin Source File

SOURCE=..\NewBaseClassDll\CompName.h
# End Source File
# Begin Source File

SOURCE=..\NewBaseClassDll\Connection.h
# End Source File
# Begin Source File

SOURCE=..\NewBaseClassDll\DataConversion.h
# End Source File
# Begin Source File

SOURCE=..\NewBaseClassDll\Latch.h
# End Source File
# Begin Source File

SOURCE=..\NewBaseClassDll\LatchData.h
# End Source File
# Begin Source File

SOURCE=..\NewBaseClassDll\NewBaseClassDll.h
# End Source File
# Begin Source File

SOURCE=..\NewBaseClassDll\OpCodeList.h
# End Source File
# Begin Source File

SOURCE=..\NewBaseClassDll\Port.h
# End Source File
# Begin Source File

SOURCE=..\NewBaseClassDll\resource.h
# End Source File
# Begin Source File

SOURCE=..\NewBaseClassDll\StatList.h
# End Source File
# Begin Source File

SOURCE=..\NewBaseClassDll\StdAfx.h
# End Source File
# Begin Source File

SOURCE=..\NewBaseClassDll\stdhdr.h
# End Source File
# Begin Source File

SOURCE=..\NewBaseClassDll\Storage.h
# End Source File
# Begin Source File

SOURCE=..\NewBaseClassDll\Unit.h
# End Source File
# End Group
# End Target
# End Project
