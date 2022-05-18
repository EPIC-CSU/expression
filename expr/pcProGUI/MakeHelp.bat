@echo off
REM -- First make map file from Microsoft Visual C++ generated resource.h
echo // MAKEHELP.BAT generated Help Map file.  Used by PCPROGUI.HPJ. >"hlp\pcProGUI.hm"
echo. >>"hlp\pcProGUI.hm"
echo // Commands (ID_* and IDM_*) >>"hlp\pcProGUI.hm"
makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"hlp\pcProGUI.hm"
echo. >>"hlp\pcProGUI.hm"
echo // Prompts (IDP_*) >>"hlp\pcProGUI.hm"
makehm IDP_,HIDP_,0x30000 resource.h >>"hlp\pcProGUI.hm"
echo. >>"hlp\pcProGUI.hm"
echo // Resources (IDR_*) >>"hlp\pcProGUI.hm"
makehm IDR_,HIDR_,0x20000 resource.h >>"hlp\pcProGUI.hm"
echo. >>"hlp\pcProGUI.hm"
echo // Dialogs (IDD_*) >>"hlp\pcProGUI.hm"
makehm IDD_,HIDD_,0x20000 resource.h >>"hlp\pcProGUI.hm"
echo. >>"hlp\pcProGUI.hm"
echo // Frame Controls (IDW_*) >>"hlp\pcProGUI.hm"
makehm IDW_,HIDW_,0x50000 resource.h >>"hlp\pcProGUI.hm"
REM -- Make help for Project PCPROGUI


echo Building Win32 Help files
start /wait hcw /C /E /M "hlp\pcProGUI.hpj"
if errorlevel 1 goto :Error
if not exist "hlp\pcProGUI.hlp" goto :Error
if not exist "hlp\pcProGUI.cnt" goto :Error
echo.
if exist Debug\nul copy "hlp\pcProGUI.hlp" Debug
if exist Debug\nul copy "hlp\pcProGUI.cnt" Debug
if exist Release\nul copy "hlp\pcProGUI.hlp" Release
if exist Release\nul copy "hlp\pcProGUI.cnt" Release
echo.
goto :done

:Error
echo hlp\pcProGUI.hpj(1) : error: Problem encountered creating help file

:done
echo.
