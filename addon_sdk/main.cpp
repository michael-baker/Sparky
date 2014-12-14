
/*
	Basic Addon v1.1 Template by Mulchman
*/

// Includes
#include <windows.h>

// DllMain
bool WINAPI DllMain( HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved )
{
	switch ( dwReason )
	{
		case DLL_PROCESS_ATTACH:
			break;

		case DLL_PROCESS_DETACH:
			break;

		case DLL_THREAD_ATTACH:
			break;

		case DLL_THREAD_DETACH:
			break;
	}

	return  true;
}

// Our exports, functions that can be used outside of the DLL
extern "C"{	/* To avoid name mangling, silly C++... */

	// Functions that have to be here
	__declspec(dllexport) char *GetAddonTitle( )
	{
		char *pszTemp = "Addon Title";
		return pszTemp;
	}

	__declspec(dllexport) char *GetAddonAuthor( )
	{
		char *pszTemp = "Addon Author";
		return pszTemp;
	}

	__declspec(dllexport) char *GetAddonVersion( )
	{
		char *pszTemp = "1.0";
		return pszTemp;
	}

	__declspec(dllexport) char *GetAddonCommand( )
	{
		// Command to toggle your addon on / off
		char *pszTemp = "bmycommand";
		return pszTemp;
	}

	__declspec(dllexport) char *GetAddonDescription( )
	{
		char *pszTemp = "Short description of my addon!";
		return pszTemp;
	}

	// Functions from the engine, you may remove those you aren't using
	__declspec(dllexport) void Post_HUD_Init( )
	{
	}

	__declspec(dllexport) void Post_HUD_Frame( )
	{
	}

	__declspec(dllexport) bool Pre_HUD_Key_Event( int eventcode, int keynum, const char *pszCurrentBinding )
	{
		return true;
	}

	__declspec(dllexport) void Post_HUD_Key_Event( int eventcode, int keynum, const char *pszCurrentBinding )
	{
	}

	__declspec(dllexport) void Post_HUD_Redraw( float flTime, int intermission )
	{
	}

/*	__declspec(dllexport) void Post_HUD_VoiceStatus( int entindex, qboolean bTalking )
	{
	} */
	
	__declspec(dllexport) void Post_Hook_TextMsg( const char *pszString )
	{
	}
	
	__declspec(dllexport) void Post_Hook_SayText( const char *pszString )
	{
	}
	
	__declspec(dllexport) void Post_Hook_HudText( const char *pszString )
	{
	}
	
	__declspec(dllexport) void Post_Hook_DeathMsg( int killer, int victim, const char *pszWeapon )
	{
	}
	
	__declspec(dllexport) void Post_Hook_ConsoleOutput( const char *pszString )
	{
	}
	
	__declspec(dllexport) void Post_Hook_AmmoX( int iIndex, int iCount )
	{
	}

	__declspec(dllexport) bool Pre_Hook_ConsoleCommand( char *pszString )
	{
		return true;
	}

	// engine related stuff - important!
	
	__declspec(dllexport) bool NeedsEnginefuncs( )
	{
		// Return true if you need to use engine funcs
		// You will have to include the engine headers
		// and possibly more stuff from sparky!
		// Note: By returning "true" this makes your addon open source
		// since you are now using GPL source

		return false;
	}

	// Functions that have to be here or sparky will crash!
	// Leave this function commented out unless you are returning
	// "true" in "NeedsEnginefuncs".

	// Note: by using this function and the enginefuncs it makes your
	// addon opne source!!
	/*
	__declspec(dllexport) void PassGameInfo( cl_enginefunc_t *pEngfuncs, CSuperHUD *pSuperHUD, CPlayerinfo *pPlayerinfo )
	{
		gEngfuncs = *pEngfuncs;
		gSuperHUD = *pSuperHUD;
		gPlayerinfo = *pPlayerinfo;
	}
	*/
}
