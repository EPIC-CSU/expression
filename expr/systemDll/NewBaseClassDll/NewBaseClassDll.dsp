# Microsoft Developer Studio Project File - Name="NewBaseClassDll" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=NewBaseClassDll - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "NewBaseClassDll.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "NewBaseClassDll.mak" CFG="NewBaseClassDll - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "NewBaseClassDll - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "NewBaseClassDll - Win32 Debug" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "NewBaseClassDll - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386

!ELSEIF  "$(CFG)" == "NewBaseClassDll - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "NewBaseClassDll - Win32 Release"
# Name "NewBaseClassDll - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter ""
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

SOURCE=..\NewBaseClassDll\Latch.cpp
# End Source File
# Begin Source File

SOURCE=..\NewBaseClassDll\OpCodeList.cpp
# End Source File
# Begin Source File

SOURCE=..\NewBaseClassDll\Port.cpp
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

# PROP Default_Filter ""
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

SOURCE=..\NewBaseClassDll\Latch.h
# End Source File
# Begin Source File

SOURCE=..\NewBaseClassDll\LatchData.h
# End Source File
# Begin Source File

SOURCE=..\NewBaseClassDll\OpCodeList.h
# End Source File
# Begin Source File

SOURCE=..\NewBaseClassDll\Port.h
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
