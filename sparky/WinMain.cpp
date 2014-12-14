/*
* HL Loader
* Copyright (c) goldfinder (goldfinder@ifrance.com) 2002
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/


#include <windows.h>
#include "Client.h"
#include "BillStuff\mempatcher.h"
#include "RedirectFunction.h"
#include "BillStuff\hook.h"
#include "BillStuff\soundpatch.h"
#include "BillStuff\superhud.h"
#include "BillStuff\glhook.h"

bool bCanPatchClient1 = false;
bool bCanPatchClient2 = true;
static void *stackret;

void PatchConsole(void);

FARPROC ( WINAPI *pGetProcAddress )( HMODULE hModule, LPCSTR lpProcName );
void ( *pExportFunc )( DllFuncs_t * );

void PatchClientFunc( DllFuncs_t *ClientFunc );

void newExportFunc( DllFuncs_t *DllFuncs )
{
	pExportFunc( DllFuncs );
	PatchClientFunc( DllFuncs );

	bCanPatchClient2 = false;
}


void DeCrypt( BYTE *bBuffer, int iSize )
{
	BYTE bXorByte = 0x57;

	for( int i = 0x44; i < iSize; i++ )
	{
		bBuffer[i] = bBuffer[i] ^ bXorByte;
		bXorByte += bBuffer[i] + 0x57;
	}
}


void Crypt( BYTE *bBuffer, int iSize )
{
	BYTE bXorByte = 0x57;
	BYTE bDecoded;

	for( int i = 0x44; i < iSize; i++ )
	{
		bDecoded = bBuffer[i] ^ bXorByte;
		bXorByte += bBuffer[i] + 0x57;
		bBuffer[i] = bDecoded;
	}
}

void SoundPatch(const DWORD a,const DWORD b,const DWORD c,const char*const sample,const float*const origin,DWORD f,const DWORD g,const DWORD h)
{
	if(gSoundPatch.bInitialized)
		gSoundPatch.RegisterSound(sample, origin);
}

void PatchSounds(void)
{
	/*

	static bool applied = false;
	MODULEENTRY32 modentry;
	CMemoryPatcher patcher;
	DWORD *addr;
	char *signature = "\x83\xEC\x48\xA1\x00\x00\x00\x00\x53";
	char *mask = "\xff\xff\xff\xff\x00\x00\x00\x00\xff";


	if( applied )   { return; }

	if(!patcher.Init()) return;
	patcher.FindModuleInProcess("hl.exe", GetCurrentProcessId(), &modentry);
	//addr = (DWORD*) patcher.FindPatternWithMask(GetCurrentProcessId(), signature, mask, strlen(signature),  (DWORD) modentry.modBaseAddr, modentry.modBaseSize);
	*/

	MODULEENTRY32 modentry;
	CMemoryPatcher patcher;
	if(!patcher.Init()) return;
	if(!patcher.FindModuleInProcess("hl.exe", GetCurrentProcessId(), &modentry))
		patcher.FindModuleInProcess("cstrike.exe", GetCurrentProcessId(), &modentry);

	static char patch2[8] = {
		(char)0x68, (char)0x00, (char)0x00, (char)0x00, // push addr; ret; nop; nop;
		(char)0x00, (char)0xC3, (char)0x90, (char)0x90  // 
	};
	static DWORD patch_size = sizeof(patch2);
	static BYTE* offset2 = (BYTE*)stackret - 1000000 + 0xB0818;
	static DWORD reloc_addr1=0;
	static BYTE buf[1024];

	//Protect(offset2,sizeof(patch2));
	offset2 = (BYTE*) patcher.FindPatternWithMask(GetCurrentProcessId(),(BYTE*)"\x83\xEC\x48\xA1\x00\x00\x00\x00\x53", (BYTE*)"\xff\xff\xff\xff\x00\x00\x00\x00\xff", 9, (DWORD) modentry.modBaseAddr, modentry.modBaseSize);
	if(!offset2) return;
	//patcher.ReadProcess(GetCurrentProcessId(),offset2+4,(BYTE*)reloc_addr1,sizeof(DWORD));
	patcher.ReadProcess(GetCurrentProcessId(),offset2,buf,9*sizeof(BYTE));
	reloc_addr1 = *(DWORD*)(offset2+4);
	/*
	if(!datacompare((BYTE*)buf,(BYTE*)"\x83\xEC\x48\xA1\x00\x00\x00\x00\x53","xxxx????x")) 
	{
		return;
	}
	system("echo datacompare succeeded >> soundpatch.txt");
	*/
	__asm{
			jmp label_continue;
		label_detour:
			// 8 args on stack
			call SoundPatch;  

			// exec caller instructions & jump back
			sub esp,0x48;
			
			mov  eax, offset2;
			add  eax, patch_size;
			push eax;
			
			mov  eax,reloc_addr1;
			mov  eax,[eax];

			ret;

		label_continue:
			mov dword ptr[patch2+1], offset label_detour;
		};

	memcpy(offset2, patch2, sizeof(patch2));
	//Protect(offset2,-1);
//	applied = 1;
	return;
}

static bool datacompare(BYTE* data1, BYTE* data2, char* mask)
{
	for(;*mask;++mask,++data1,++data2)
	{
		if(*mask=='x' && *data1!=*data2 ) return false;
	}
	return (*mask)==0;
}

void Protect(void* addr,int size)
{
	static DWORD realsize;
	static DWORD old;
	if (size!=-1)
	{
    realsize = size;
	VirtualProtect(addr,realsize,PAGE_EXECUTE_READWRITE,&old);
	}
	else 
	{   
		DWORD dummy;
		VirtualProtect(addr,realsize,old,&dummy);
	}
}

void patch(void)
{
	static bool done = false;
	if(done) return;
	PatchSounds();
	done = true;
}

HANDLE WINAPI newCreateFile( LPCTSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile )
{
	if( strstr( lpFileName, "client.dll" ) )
	{
		bCanPatchClient1 = true;
		PatchConsole();
		__asm pop stackret;
		__asm push stackret;
		patch();
	}

	return( CreateFile( lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile ) );
}


BOOL WINAPI newReadFile( HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped )
{
	BOOL retval = ReadFile( hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped );

	if( bCanPatchClient1 && bCanPatchClient2 ) // We patch one time, because there is a CRC check after
	{
		if( *( WORD * )lpBuffer == IMAGE_DOS_SIGNATURE ) // We don't patch if client.dll is a regular dll
		{
			bCanPatchClient1 = false;
			return( retval );
		}
		DeCrypt( ( BYTE * )lpBuffer, ( int )*lpNumberOfBytesRead );
		*( ( DWORD * )&pExportFunc ) = ( *( ( DWORD * )( ( DWORD )lpBuffer + 0x4C ) ) ^ 0x7A32BC85 );
		*( ( DWORD * )( ( DWORD )lpBuffer + 0x4C ) ) = ( ( DWORD )&newExportFunc ^ 0x7A32BC85 );
		Crypt( ( BYTE * )lpBuffer, ( int )*lpNumberOfBytesRead );
		bCanPatchClient1 = false;
	}

	return( retval );
}

// Beg: added by Mulchman
// OutputDebugStringA is a function we are hooking
// to get the messages that the game is sending 
// to the console
VOID WINAPI newOutputDebugStringA( const char *pszString )
{
	// Pre Hook
	if(!gMainHook.Pre_Hook_ConsoleOutput(pszString))
		return;

	// Now run the real OutputDebugStringA
	OutputDebugStringA( pszString );

	// Post Hook
	gMainHook.Post_Hook_ConsoleOutput(pszString);
}
// End: added by Mulchman

void PatchClientFunc( DllFuncs_t *ClientFunc )
{
	gDllFuncs.Initialize = ClientFunc->Initialize;
	ClientFunc->Initialize = &Initialize;

	gDllFuncs.HUD_Init = ClientFunc->HUD_Init;
	ClientFunc->HUD_Init = &HUD_Init;

	gDllFuncs.HUD_VidInit = ClientFunc->HUD_VidInit;
	ClientFunc->HUD_VidInit = &HUD_VidInit;

	gDllFuncs.HUD_Redraw = ClientFunc->HUD_Redraw;
	ClientFunc->HUD_Redraw = &HUD_Redraw;

	gDllFuncs.HUD_UpdateClientData = ClientFunc->HUD_UpdateClientData;
	ClientFunc->HUD_UpdateClientData = &HUD_UpdateClientData;

	gDllFuncs.HUD_Reset = ClientFunc->HUD_Reset;
	ClientFunc->HUD_Reset = &HUD_Reset;

	gDllFuncs.HUD_PlayerMove = ClientFunc->HUD_PlayerMove;
	ClientFunc->HUD_PlayerMove = &HUD_PlayerMove;

	gDllFuncs.HUD_PlayerMoveInit = ClientFunc->HUD_PlayerMoveInit;
	ClientFunc->HUD_PlayerMoveInit = &HUD_PlayerMoveInit;

	gDllFuncs.HUD_PlayerMoveTexture = ClientFunc->HUD_PlayerMoveTexture;
	ClientFunc->HUD_PlayerMoveTexture = &HUD_PlayerMoveTexture;

	gDllFuncs.IN_ActivateMouse = ClientFunc->IN_ActivateMouse;
	ClientFunc->IN_ActivateMouse = &IN_ActivateMouse;

	gDllFuncs.IN_DeactivateMouse = ClientFunc->IN_DeactivateMouse;
	ClientFunc->IN_DeactivateMouse = &IN_DeactivateMouse;

	gDllFuncs.IN_MouseEvent = ClientFunc->IN_MouseEvent;
	ClientFunc->IN_MouseEvent =&IN_MouseEvent;

	gDllFuncs.IN_ClearStates = ClientFunc->IN_ClearStates;
	ClientFunc->IN_ClearStates = &IN_ClearStates;

	gDllFuncs.IN_Accumulate = ClientFunc->IN_Accumulate;
	ClientFunc->IN_Accumulate = &IN_Accumulate;

	gDllFuncs.CL_CreateMove = ClientFunc->CL_CreateMove;
	ClientFunc->CL_CreateMove = &CL_CreateMove;

	gDllFuncs.CL_IsThirdPerson = ClientFunc->CL_IsThirdPerson;
	ClientFunc->CL_IsThirdPerson = &CL_IsThirdPerson;

	gDllFuncs.CL_CameraOffset = ClientFunc->CL_CameraOffset;
	ClientFunc->CL_CameraOffset = ClientFunc->CL_CameraOffset;

	gDllFuncs.KB_Find = ClientFunc->KB_Find;
	ClientFunc->KB_Find = &KB_Find;

	gDllFuncs.CAM_Think = ClientFunc->CAM_Think;
	ClientFunc->CAM_Think = &CAM_Think;

	gDllFuncs.V_CalcRefdef = ClientFunc->V_CalcRefdef;
	ClientFunc->V_CalcRefdef = &V_CalcRefdef;

	gDllFuncs.HUD_AddEntity = ClientFunc->HUD_AddEntity;
	ClientFunc->HUD_AddEntity = &HUD_AddEntity;

	gDllFuncs.HUD_CreateEntities = ClientFunc->HUD_CreateEntities;
	ClientFunc->HUD_CreateEntities = &HUD_CreateEntities;

	gDllFuncs.HUD_DrawNormalTriangles = ClientFunc->HUD_DrawNormalTriangles;
	ClientFunc->HUD_DrawNormalTriangles = &HUD_DrawNormalTriangles;

	gDllFuncs.HUD_DrawTransparentTriangles = ClientFunc->HUD_DrawTransparentTriangles;
	ClientFunc->HUD_DrawTransparentTriangles = &HUD_DrawTransparentTriangles;

	gDllFuncs.HUD_StudioEvent = ClientFunc->HUD_StudioEvent;
	ClientFunc->HUD_StudioEvent = &HUD_StudioEvent;

	gDllFuncs.HUD_PostRunCmd = ClientFunc->HUD_PostRunCmd;
	ClientFunc->HUD_PostRunCmd = &HUD_PostRunCmd;

	gDllFuncs.HUD_Shutdown = ClientFunc->HUD_Shutdown;
	ClientFunc->HUD_Shutdown = &HUD_Shutdown;

	gDllFuncs.HUD_TxferLocalOverrides = ClientFunc->HUD_TxferLocalOverrides;
	ClientFunc->HUD_TxferLocalOverrides = &HUD_TxferLocalOverrides;

	gDllFuncs.HUD_ProcessPlayerState = ClientFunc->HUD_ProcessPlayerState;
	ClientFunc->HUD_ProcessPlayerState = &HUD_ProcessPlayerState;

	gDllFuncs.HUD_TxferPredictionData = ClientFunc->HUD_TxferPredictionData;
	ClientFunc->HUD_TxferPredictionData = &HUD_TxferPredictionData;

	gDllFuncs.Demo_ReadBuffer = ClientFunc->Demo_ReadBuffer;
	ClientFunc->Demo_ReadBuffer = &Demo_ReadBuffer;

	gDllFuncs.HUD_ConnectionlessPacket = ClientFunc->HUD_ConnectionlessPacket;
	ClientFunc->HUD_ConnectionlessPacket = &HUD_ConnectionlessPacket;

	gDllFuncs.HUD_GetHullBounds = ClientFunc->HUD_GetHullBounds;
	ClientFunc->HUD_GetHullBounds = &HUD_GetHullBounds;

	gDllFuncs.HUD_Frame = ClientFunc->HUD_Frame;
	ClientFunc->HUD_Frame = &HUD_Frame;

	gDllFuncs.HUD_Key_Event = ClientFunc->HUD_Key_Event;
	ClientFunc->HUD_Key_Event = &HUD_Key_Event;

	gDllFuncs.HUD_TempEntUpdate = ClientFunc->HUD_TempEntUpdate;
	ClientFunc->HUD_TempEntUpdate = &HUD_TempEntUpdate;

	gDllFuncs.HUD_GetUserEntity = ClientFunc->HUD_GetUserEntity;
	ClientFunc->HUD_GetUserEntity = &HUD_GetUserEntity;

	gDllFuncs.HUD_VoiceStatus = ClientFunc->HUD_VoiceStatus;
	ClientFunc->HUD_VoiceStatus = &HUD_VoiceStatus;
}

FARPROC WINAPI newGetProcAddress( HMODULE hModule, LPCSTR lpProcName )
{
	if( HIWORD( lpProcName ) )
	{
		if( !lstrcmp( lpProcName, "GetProcAddress" ) )
			return( ( FARPROC )&newGetProcAddress );

		if( !lstrcmp( lpProcName, "CreateFileA" ) )
			return( ( FARPROC )&newCreateFile );

		if( !lstrcmp( lpProcName, "ReadFile" ) )
			return( ( FARPROC )&newReadFile );

		if( !lstrcmp( lpProcName, "Initialize" ) )
		{
			gDllFuncs.Initialize = ( int( * )( cl_enginefunc_t *, int ) )pGetProcAddress( hModule, lpProcName );
			return( ( FARPROC )&Initialize );
		}

		if( !lstrcmp( lpProcName, "HUD_Init" ) )
		{
			gDllFuncs.HUD_Init = ( int( * )( void ) )pGetProcAddress( hModule, lpProcName );
			return( ( FARPROC )&HUD_Init );
		}

		if( !lstrcmp( lpProcName, "HUD_VidInit" ) )
		{
			gDllFuncs.HUD_VidInit = ( int( * )( void ) )pGetProcAddress( hModule, lpProcName );
			return( ( FARPROC )&HUD_VidInit );
		}

		if( !lstrcmp( lpProcName, "HUD_Redraw" ) )
		{
			gDllFuncs.HUD_Redraw = ( int( * )( float, int ) )pGetProcAddress( hModule, lpProcName );
			return( ( FARPROC )&HUD_Redraw );
		}

		if( !lstrcmp( lpProcName, "HUD_UpdateClientData" ) )
		{
			gDllFuncs.HUD_UpdateClientData = ( int( * )( client_data_t *, float ) )pGetProcAddress( hModule, lpProcName );
			return( ( FARPROC )&HUD_UpdateClientData );
		}

		if( !lstrcmp( lpProcName, "HUD_Reset" ) )
		{
			gDllFuncs.HUD_Reset = ( int( * )( void ) )pGetProcAddress( hModule, lpProcName );
			return( ( FARPROC )&HUD_Reset );
		}

		if( !lstrcmp( lpProcName, "HUD_PlayerMove" ) )
		{
			gDllFuncs.HUD_PlayerMove = ( void( * )( struct playermove_s *, int ) )pGetProcAddress( hModule, lpProcName );
			return( ( FARPROC )&HUD_PlayerMove );
		}

		if( !lstrcmp( lpProcName, "HUD_PlayerMoveInit" ) )
		{
			gDllFuncs.HUD_PlayerMoveInit = ( void( * )( struct playermove_s * ) )pGetProcAddress( hModule, lpProcName );
			return( ( FARPROC )&HUD_PlayerMoveInit );
		}

		if( !lstrcmp( lpProcName, "HUD_PlayerMoveTexture" ) )
		{
			gDllFuncs.HUD_PlayerMoveTexture = ( char( * )( char * ) )pGetProcAddress( hModule, lpProcName );
			return( ( FARPROC )&HUD_PlayerMoveTexture );
		}

		if( !lstrcmp( lpProcName, "IN_ActivateMouse" ) )
		{
			gDllFuncs.IN_ActivateMouse = ( void( * )( void ) )pGetProcAddress( hModule, lpProcName );
			return( ( FARPROC )&IN_ActivateMouse );
		}

		if( !lstrcmp( lpProcName, "IN_DeactivateMouse" ) )
		{
			gDllFuncs.IN_DeactivateMouse = ( void( * )( void ) )pGetProcAddress( hModule, lpProcName );
			return( ( FARPROC )&IN_DeactivateMouse );
		}

		if( !lstrcmp( lpProcName, "IN_MouseEvent" ) )
		{
			gDllFuncs.IN_MouseEvent = ( void( * )( int mstate ) )pGetProcAddress( hModule, lpProcName );
			return( ( FARPROC )&IN_MouseEvent );
		}

		if( !lstrcmp( lpProcName, "IN_ClearStates" ) )
		{
			gDllFuncs.IN_ClearStates = ( void( * )( void ) )pGetProcAddress( hModule, lpProcName );
			return( ( FARPROC )&IN_ClearStates );
		}

		if( !lstrcmp( lpProcName, "IN_Accumulate" ) )
		{
			gDllFuncs.IN_Accumulate = ( void( * )( void ) )pGetProcAddress( hModule, lpProcName );
			return( ( FARPROC )&IN_Accumulate );
		}

		if( !lstrcmp( lpProcName, "CL_CreateMove" ) )
		{
			gDllFuncs.CL_CreateMove = ( void( * )( float, struct usercmd_s *, int ) )pGetProcAddress( hModule, lpProcName );
			return( ( FARPROC )&CL_CreateMove );
		}

		if( !lstrcmp( lpProcName, "CL_IsThirdPerson" ) )
		{
			gDllFuncs.CL_IsThirdPerson = ( int( * )( void ) )pGetProcAddress( hModule, lpProcName );
			return( ( FARPROC )&CL_IsThirdPerson );
		}

		if( !lstrcmp( lpProcName, "CL_CameraOffset" ) )
		{
			gDllFuncs.CL_CameraOffset = ( void( * )( float * ) )pGetProcAddress( hModule, lpProcName );
			return( ( FARPROC )&CL_CameraOffset );
		}

		if( !lstrcmp( lpProcName, "KB_Find" ) )
		{
			gDllFuncs.KB_Find = ( struct kbutton_s *( * )( const char *name ) )pGetProcAddress( hModule, lpProcName );
			return( ( FARPROC )&KB_Find );
		}

		if( !lstrcmp( lpProcName, "CAM_Think" ) )
		{
			gDllFuncs.CAM_Think = ( void( * )( void ) )pGetProcAddress( hModule, lpProcName );
			return( ( FARPROC )&CAM_Think );
		}

		if( !lstrcmp( lpProcName, "V_CalcRefdef" ) )
		{
			gDllFuncs.V_CalcRefdef = ( void( * )( struct ref_params_s *pparams ) )pGetProcAddress( hModule, lpProcName );
			return( ( FARPROC )&V_CalcRefdef );
		}

		if( !lstrcmp( lpProcName, "HUD_AddEntity" ) )
		{
			gDllFuncs.HUD_AddEntity = ( int( * )( int, struct cl_entity_s *, const char * ) )pGetProcAddress( hModule, lpProcName );
			return( ( FARPROC )&HUD_AddEntity );
		}

		if( !lstrcmp( lpProcName, "HUD_CreateEntities" ) )
		{
			gDllFuncs.HUD_CreateEntities = ( void( * )( void ) )pGetProcAddress( hModule, lpProcName );
			return( ( FARPROC )&HUD_CreateEntities );
		}

		if( !lstrcmp( lpProcName, "HUD_DrawNormalTriangles" ) )
		{
			gDllFuncs.HUD_DrawNormalTriangles = ( void( * )( void ) )pGetProcAddress( hModule, lpProcName );
			return( ( FARPROC )&HUD_DrawNormalTriangles );
		}

		if( !lstrcmp( lpProcName, "HUD_DrawTransparentTriangles" ) )
		{
			gDllFuncs.HUD_DrawTransparentTriangles = ( void( * )( void ) )pGetProcAddress( hModule, lpProcName );
			return( ( FARPROC )&HUD_DrawTransparentTriangles );
		}

		if( !lstrcmp( lpProcName, "HUD_StudioEvent" ) )
		{
			gDllFuncs.HUD_StudioEvent = ( void( * )( const struct mstudioevent_s *, const struct cl_entity_s * ) )pGetProcAddress( hModule, lpProcName );
			return( ( FARPROC )&HUD_StudioEvent );
		}

		if( !lstrcmp( lpProcName, "HUD_PostRunCmd" ) )
		{
			gDllFuncs.HUD_PostRunCmd = ( void( * )( struct local_state_s *from, struct local_state_s *to, struct usercmd_s *cmd, int runfuncs, double time, unsigned int random_seed ) )pGetProcAddress( hModule, lpProcName );
			return( ( FARPROC )&HUD_PostRunCmd );
		}

		if( !lstrcmp( lpProcName, "HUD_Shutdown" ) )
		{
			gDllFuncs.HUD_Shutdown = ( void( * )( void ) )pGetProcAddress( hModule, lpProcName );
			return( ( FARPROC )&HUD_Shutdown );
		}

		if( !lstrcmp( lpProcName, "HUD_TxferLocalOverrides" ) )
		{
			gDllFuncs.HUD_TxferLocalOverrides = ( void( * )( struct entity_state_s *, const struct clientdata_s * ) )pGetProcAddress( hModule, lpProcName );
			return( ( FARPROC )&HUD_TxferLocalOverrides );
		}

		if( !lstrcmp( lpProcName, "HUD_ProcessPlayerState" ) )
		{
			gDllFuncs.HUD_ProcessPlayerState = ( void( * )( struct entity_state_s *, const struct entity_state_s * ) )pGetProcAddress( hModule, lpProcName );
			return( ( FARPROC )&HUD_ProcessPlayerState );
		}

		if( !lstrcmp( lpProcName, "HUD_TxferPredictionData" ) )
		{
			gDllFuncs.HUD_TxferPredictionData = ( void( * )( struct entity_state_s *, const struct entity_state_s *, struct clientdata_s *, const struct clientdata_s *, struct weapon_data_s *, const struct weapon_data_s * ) )pGetProcAddress( hModule, lpProcName );
			return( ( FARPROC )&HUD_TxferPredictionData );
		}

		if( !lstrcmp( lpProcName, "Demo_ReadBuffer" ) )
		{
			gDllFuncs.Demo_ReadBuffer = ( void( * )( int, unsigned char * ) )pGetProcAddress( hModule, lpProcName );
			return( ( FARPROC )&Demo_ReadBuffer );
		}

		if( !lstrcmp( lpProcName, "HUD_ConnectionlessPacket" ) )
		{
			gDllFuncs.HUD_ConnectionlessPacket = ( int( * )( struct netadr_s *net_from, const char *args, char *response_buffer, int *response_buffer_size ) )pGetProcAddress( hModule, lpProcName );
			return( ( FARPROC )&HUD_ConnectionlessPacket );
		}

		if( !lstrcmp( lpProcName, "HUD_GetHullBounds" ) )
		{
			gDllFuncs.HUD_GetHullBounds = ( int( * )( int hullnumber, float *mins, float *maxs ) )pGetProcAddress( hModule, lpProcName );
			return( ( FARPROC )&HUD_GetHullBounds );
		}

		if( !lstrcmp( lpProcName, "HUD_Frame" ) )
		{
			gDllFuncs.HUD_Frame = ( void( * )( double time ) )pGetProcAddress( hModule, lpProcName );
			return( ( FARPROC )&HUD_Frame );
		}

		if( !lstrcmp( lpProcName, "HUD_Key_Event" ) )
		{
			gDllFuncs.HUD_Key_Event = ( int( * )( int, int, const char * ) )pGetProcAddress( hModule, lpProcName );
			return( ( FARPROC )&HUD_Key_Event );
		}

		if( !lstrcmp( lpProcName, "HUD_TempEntUpdate" ) )
		{
			gDllFuncs.HUD_TempEntUpdate = ( void( * )( double, double, double, struct tempent_s **, struct tempent_s **, int( *Callback_AddVisibleEntity )( struct cl_entity_s *pEntity ), void( *Callback_TempEntPlaySound )( struct tempent_s *pTemp, float damp ) ) )pGetProcAddress( hModule, lpProcName );
			return( ( FARPROC )&HUD_TempEntUpdate );
		}

		if( !lstrcmp( lpProcName, "HUD_GetUserEntity" ) )
		{
			gDllFuncs.HUD_GetUserEntity = ( struct cl_entity_s *( * )( int index ) )pGetProcAddress( hModule, lpProcName );
			return( ( FARPROC )&HUD_GetUserEntity );
		}

		if( !lstrcmp( lpProcName, "HUD_VoiceStatus" ) )
		{
			gDllFuncs.HUD_VoiceStatus = ( int( * )( int entindex, qboolean bTalking ) )pGetProcAddress( hModule, lpProcName );
			return( ( FARPROC )&HUD_VoiceStatus );
		}
		if( !lstrcmp( lpProcName, "OutputDebugStringA" ) ) // added by Mulchman
		{
			return( ( FARPROC )&newOutputDebugStringA );
		}
		// added by ruiner
		
		if( !lstrcmp( lpProcName, "glEnable" ) )
		{
			*( ( DWORD * )&pglEnable ) = ( DWORD )pGetProcAddress( hModule, lpProcName );
			return( ( FARPROC )&hook_glEnable );
		}

		if( !lstrcmp( lpProcName, "glBegin" ) )
		{
			*( ( DWORD * )&pglBegin ) = ( DWORD )pGetProcAddress( hModule, lpProcName );
			return( ( FARPROC )&hook_glBegin );
		}

		// END ADDITION
	}
	return( pGetProcAddress( hModule, lpProcName ) );
}


bool WINAPI DllMain( HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved )
{
	switch( fdwReason ) 
	{ 
		case DLL_PROCESS_ATTACH :
		{
			pGetProcAddress = ( FARPROC( WINAPI * )( HMODULE, LPCSTR ) )InterceptDllCall( GetModuleHandle( NULL ), "Kernel32.dll", "GetProcAddress", ( DWORD )&newGetProcAddress );
			break;
		}
		case DLL_PROCESS_DETACH :
		{
			break;
		}
		case DLL_THREAD_ATTACH :
		{
			break;
		}
		case DLL_THREAD_DETACH :
		{
			break;
		}
	}
	return( true );
}

static BYTE* consolepatchoffset;

void hookCommandParser( char* cmdString, int size )
{
	char* pos       = cmdString;
	int   charsLeft = size;

	bool passThroughBlock = false;

	// skip whitespace
	while(charsLeft>0 && (*pos==' '||*pos=='\t')){pos++;charsLeft--;}
	if(charsLeft<=0){ return; }

	// extract command
	char  command[512];
	char* commandPos = command;
	int   commandCharsLeft = 500;

	while(charsLeft>0 && *pos>'\0' && commandCharsLeft>0)
	{
		*commandPos = tolower(*pos);
		commandPos++;
		pos++;
		charsLeft--;
		commandCharsLeft--;
	}
	*commandPos = 0;
	if(!*command) return;

	/*
	if(!strcmp(command, "unbindall"))
	{
		for(int i=0;i<size;i++){ cmdString[i]=' ';}
		gEngfuncs.pfnConsolePrint("[sparky] unbindall prevented\n");
	}

	if(!strcmp(command, "quit") || !strcmp(command, "exit"))
	{
		for(int i=0;i<size;i++){ cmdString[i]=' ';}
		gEngfuncs.pfnConsolePrint("[sparky] quit/exit prevented\n");
	}

	if(!strcmp(command, "name"))
	{
		for(int i=0;i<size;i++){ cmdString[i]=' ';}
		gEngfuncs.pfnConsolePrint("[sparky] name prevented\n");
	}
	
	if(!strcmp(command, "+gren1") || !strcmp(command, "+gren2"))
	{
		//for(int i=0;i<size;i++){ cmdString[i]=' ';}
		char buf[1024];
		sprintf(buf, "+gren1/2");
		gSuperHUD.WriteHUDChannel(HC_IN_GAME_COLOR, 2, buf);
	}*/

	// Call the pre hook, wipe command if necessary
	if(!gMainHook.Pre_Hook_ConsoleCommand(command))
	{
		for(int i=0;i<size;i++){ cmdString[i]=' ';}
	}
}

__declspec(naked) void JumpGate_CommandParser()
{
	__asm{
                pushad;
                push edi;
                push ebp;
                call hookCommandParser;
                pop eax;
                pop eax;
                popad;
                
                // caller code
                push edi;
                lea eax, dword ptr [esp+0x14];
                push ebp;
                push eax;
                
                // jump back
                mov eax, consolepatchoffset;
                add eax, 0x07;
                jmp eax;
	}
}

void PatchConsole(void)
{
	static bool patched = 0;
	if(patched) return;
	patched = 1;
	MODULEENTRY32 modentry;
	CMemoryPatcher patcher;
	if(!patcher.Init()) return;
	if(!patcher.FindModuleInProcess("hl.exe", GetCurrentProcessId(), &modentry))
		patcher.FindModuleInProcess("cstrike.exe", GetCurrentProcessId(), &modentry);

	consolepatchoffset = (BYTE*) patcher.FindPatternWithMask(GetCurrentProcessId(),(BYTE*)"\x57\x00\x00\x00\x00\x55\x50\xE8", (BYTE*)"\xff\x00\x00\x00\x00\xff\xff\xff", 8, (DWORD) modentry.modBaseAddr, modentry.modBaseSize);
	if(consolepatchoffset)
	{
//		char buf[1024];
//		sprintf(buf, "echo Found console signature at %x! >> pfft.txt", consolepatchoffset);
//		system(buf);
		*(consolepatchoffset+0) = 0xB8;
		*(DWORD*)(consolepatchoffset+1) = (DWORD)JumpGate_CommandParser;
		*(consolepatchoffset+5) = 0xFF;
		*(consolepatchoffset+6) = 0xE0;

	}
}

