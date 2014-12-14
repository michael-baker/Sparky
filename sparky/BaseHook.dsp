# Microsoft Developer Studio Project File - Name="BaseHook" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=BaseHook - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "BaseHook.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "BaseHook.mak" CFG="BaseHook - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "BaseHook - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "BaseHook - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "BaseHook - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "BASEHOOK_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "BASEHOOK_EXPORTS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"Release/sparky.dll"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy release\sparky.dll c:\sierra\half-life\sparkyutils\sparky.dll	upx -9 c:\sierra\half-life\sparkyutils\sparky.dll
# End Special Build Tool

!ELSEIF  "$(CFG)" == "BaseHook - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "BASEHOOK_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "BASEHOOK_EXPORTS" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"Debug/sparky.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "BaseHook - Win32 Release"
# Name "BaseHook - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "BillStuff Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\BillStuff\addons.cpp
# End Source File
# Begin Source File

SOURCE=.\BillStuff\autorecord.cpp
# End Source File
# Begin Source File

SOURCE=.\BillStuff\commandpoint.cpp
# End Source File
# Begin Source File

SOURCE=.\BillStuff\fb.cpp
# End Source File
# Begin Source File

SOURCE=.\BillStuff\flagstatus.cpp
# End Source File
# Begin Source File

SOURCE=.\BillStuff\glhook.cpp
# End Source File
# Begin Source File

SOURCE=.\BillStuff\help.cpp
# End Source File
# Begin Source File

SOURCE=.\BillStuff\hook.cpp
# End Source File
# Begin Source File

SOURCE=.\BillStuff\irc.cpp
# End Source File
# Begin Source File

SOURCE=.\BillStuff\kselookup.cpp
# End Source File
# Begin Source File

SOURCE=.\BillStuff\laserstatus.cpp
# End Source File
# Begin Source File

SOURCE=.\BillStuff\location.cpp
# End Source File
# Begin Source File

SOURCE=.\BillStuff\logging.cpp
# End Source File
# Begin Source File

SOURCE=.\BillStuff\logo.cpp
# End Source File
# Begin Source File

SOURCE=.\BillStuff\mempatcher.cpp
# End Source File
# Begin Source File

SOURCE=.\BillStuff\menu.cpp
# End Source File
# Begin Source File

SOURCE=.\BillStuff\playerinfo.cpp
# End Source File
# Begin Source File

SOURCE=.\pm_shared\pm_math.c
# End Source File
# Begin Source File

SOURCE=.\pm_shared\pm_shared.c
# End Source File
# Begin Source File

SOURCE=.\BillStuff\retry.cpp
# End Source File
# Begin Source File

SOURCE=.\BillStuff\RTFM.cpp
# End Source File
# Begin Source File

SOURCE=.\BillStuff\rulesmanager.cpp
# End Source File
# Begin Source File

SOURCE=.\BillStuff\soundpatch.cpp
# End Source File
# Begin Source File

SOURCE=.\BillStuff\spammeter.cpp
# End Source File
# Begin Source File

SOURCE=.\BillStuff\speedos.cpp
# End Source File
# Begin Source File

SOURCE=.\BillStuff\stats.cpp
# End Source File
# Begin Source File

SOURCE=.\BillStuff\superhud.cpp
# End Source File
# Begin Source File

SOURCE=.\BillStuff\timers.cpp
# End Source File
# Begin Source File

SOURCE=.\BillStuff\triggers.cpp
# End Source File
# Begin Source File

SOURCE=.\BillStuff\util.cpp
# End Source File
# Begin Source File

SOURCE=.\BillStuff\winamp.cpp
# End Source File
# Begin Source File

SOURCE=.\BillStuff\wonid.cpp
# End Source File
# End Group
# Begin Group "engine source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Engine\parsemsg.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\Client.cpp
# End Source File
# Begin Source File

SOURCE=.\enginehook.cpp
# End Source File
# Begin Source File

SOURCE=.\hookmessages.cpp
# End Source File
# Begin Source File

SOURCE=.\RedirectFunction.cpp
# End Source File
# Begin Source File

SOURCE=".\Engine\valve-util.cpp"
# End Source File
# Begin Source File

SOURCE=.\WinMain.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "BillStuff Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\BillStuff\addons.h
# End Source File
# Begin Source File

SOURCE=.\BillStuff\autorecord.h
# End Source File
# Begin Source File

SOURCE=.\BillStuff\commandpoint.h
# End Source File
# Begin Source File

SOURCE=.\BillStuff\fb.h
# End Source File
# Begin Source File

SOURCE=.\BillStuff\flagstatus.h
# End Source File
# Begin Source File

SOURCE=.\BillStuff\foxbotarea.h
# End Source File
# Begin Source File

SOURCE=.\BillStuff\glhook.h
# End Source File
# Begin Source File

SOURCE=.\BillStuff\help.h
# End Source File
# Begin Source File

SOURCE=.\BillStuff\hook.h
# End Source File
# Begin Source File

SOURCE=.\BillStuff\irc.h
# End Source File
# Begin Source File

SOURCE=.\BillStuff\kselookup.h
# End Source File
# Begin Source File

SOURCE=.\BillStuff\laserstatus.h
# End Source File
# Begin Source File

SOURCE=.\BillStuff\location.h
# End Source File
# Begin Source File

SOURCE=.\BillStuff\logging.h
# End Source File
# Begin Source File

SOURCE=.\BillStuff\logo.h
# End Source File
# Begin Source File

SOURCE=.\BillStuff\mempatcher.h
# End Source File
# Begin Source File

SOURCE=.\BillStuff\menu.h
# End Source File
# Begin Source File

SOURCE=.\BillStuff\playerinfo.h
# End Source File
# Begin Source File

SOURCE=.\pm_shared\pm_shared.h
# End Source File
# Begin Source File

SOURCE=.\BillStuff\retry.h
# End Source File
# Begin Source File

SOURCE=.\BillStuff\RTFM.h
# End Source File
# Begin Source File

SOURCE=.\BillStuff\rulesmanager.h
# End Source File
# Begin Source File

SOURCE=.\BillStuff\soundpatch.h
# End Source File
# Begin Source File

SOURCE=.\BillStuff\spammeter.h
# End Source File
# Begin Source File

SOURCE=.\BillStuff\speedos.h
# End Source File
# Begin Source File

SOURCE=.\BillStuff\stats.h
# End Source File
# Begin Source File

SOURCE=.\BillStuff\superhud.h
# End Source File
# Begin Source File

SOURCE=.\BillStuff\timers.h
# End Source File
# Begin Source File

SOURCE=.\BillStuff\triggers.h
# End Source File
# Begin Source File

SOURCE=.\BillStuff\util.h
# End Source File
# Begin Source File

SOURCE=.\BillStuff\winamp.h
# End Source File
# Begin Source File

SOURCE=.\BillStuff\wonid.h
# End Source File
# End Group
# Begin Group "engine headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Engine\beamdef.h
# End Source File
# Begin Source File

SOURCE=.\Engine\cdll_dll.h
# End Source File
# Begin Source File

SOURCE=.\Engine\cdll_engine_dll.h
# End Source File
# Begin Source File

SOURCE=.\Engine\cdll_int.h
# End Source File
# Begin Source File

SOURCE=.\Engine\cl_dll.h
# End Source File
# Begin Source File

SOURCE=.\Engine\cl_entity.h
# End Source File
# Begin Source File

SOURCE=.\Engine\cl_util.h
# End Source File
# Begin Source File

SOURCE=.\Engine\com_model.h
# End Source File
# Begin Source File

SOURCE=.\Engine\const.h
# End Source File
# Begin Source File

SOURCE=.\Engine\crc.h
# End Source File
# Begin Source File

SOURCE=.\Engine\custom.h
# End Source File
# Begin Source File

SOURCE=.\Engine\customentity.h
# End Source File
# Begin Source File

SOURCE=.\Engine\cvardef.h
# End Source File
# Begin Source File

SOURCE=.\Engine\demo_api.h
# End Source File
# Begin Source File

SOURCE=.\Engine\director_cmds.h
# End Source File
# Begin Source File

SOURCE=.\Engine\dlight.h
# End Source File
# Begin Source File

SOURCE=.\Engine\edict.h
# End Source File
# Begin Source File

SOURCE=.\Engine\eiface.h
# End Source File
# Begin Source File

SOURCE=.\Engine\entity_state.h
# End Source File
# Begin Source File

SOURCE=.\Engine\entity_types.h
# End Source File
# Begin Source File

SOURCE=.\Engine\event_api.h
# End Source File
# Begin Source File

SOURCE=.\Engine\event_args.h
# End Source File
# Begin Source File

SOURCE=.\Engine\event_flags.h
# End Source File
# Begin Source File

SOURCE=.\common\hltv.h
# End Source File
# Begin Source File

SOURCE=.\Engine\in_buttons.h
# End Source File
# Begin Source File

SOURCE=.\Engine\ivoicetweak.h
# End Source File
# Begin Source File

SOURCE=.\Engine\net_api.h
# End Source File
# Begin Source File

SOURCE=.\Engine\netadr.h
# End Source File
# Begin Source File

SOURCE=.\Engine\parsemsg.h
# End Source File
# Begin Source File

SOURCE=.\Engine\particledef.h
# End Source File
# Begin Source File

SOURCE=.\Engine\pm_defs.h
# End Source File
# Begin Source File

SOURCE=.\Engine\pm_info.h
# End Source File
# Begin Source File

SOURCE=.\Engine\pmtrace.h
# End Source File
# Begin Source File

SOURCE=.\Engine\progdefs.h
# End Source File
# Begin Source File

SOURCE=.\Engine\progs.h
# End Source File
# Begin Source File

SOURCE=.\Engine\r_efx.h
# End Source File
# Begin Source File

SOURCE=.\Engine\r_studioint.h
# End Source File
# Begin Source File

SOURCE=.\Engine\ref_params.h
# End Source File
# Begin Source File

SOURCE=.\Engine\screenfade.h
# End Source File
# Begin Source File

SOURCE=.\Engine\studio.h
# End Source File
# Begin Source File

SOURCE=.\Engine\studio_event.h
# End Source File
# Begin Source File

SOURCE=.\Engine\triangleapi.h
# End Source File
# Begin Source File

SOURCE=.\Engine\usercmd.h
# End Source File
# Begin Source File

SOURCE=.\Engine\util_vector.h
# End Source File
# Begin Source File

SOURCE=.\Engine\weaponinfo.h
# End Source File
# Begin Source File

SOURCE=.\Engine\wrect.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\client.h
# End Source File
# Begin Source File

SOURCE=.\Engine.h
# End Source File
# Begin Source File

SOURCE=.\enginehook.h
# End Source File
# Begin Source File

SOURCE=.\hookmessages.h
# End Source File
# Begin Source File

SOURCE=.\RedirectFunction.h
# End Source File
# Begin Source File

SOURCE=".\Engine\valve-util.h"
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
