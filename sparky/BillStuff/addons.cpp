/*  ------------------------------------------------------------------------
	  addons.h by Mulchman
		- Cleaned up some tiny bugs (v1.44)
	    - Error prevention and more functions added (v1.43)
		- Generic addons loader and manager (v1.42)
	------------------------------------------------------------------------ */

#include "..\client.h"

#include "hook.h"
#include "playerinfo.h"
#include "superhud.h"
#include "addons.h"

/*Global*/
CAddons gAddons;

void CAddons::Post_HUD_Init (void)
{
	gEngfuncs.pfnConsolePrint( "[sparky] Loading addons..." );

	char strCurDir[2048] = {0};
	GetCurrentDirectory( sizeof( strCurDir ), strCurDir );
	strcat( strCurDir, "\\sparkyutils\\addons\\*.dll" );

	/* Beg: Find all addon DLLs */
	vAddons.clear( );
	CAddonEntry caeTemp;
	HANDLE hFound;
	WIN32_FIND_DATA ffdFoundData;

	hFound = FindFirstFile( strCurDir, &ffdFoundData );
	while (( (HANDLE)(-1) != hFound ) && ( GetLastError( ) != ERROR_NO_MORE_FILES ))
	{
		caeTemp.szAddonName = ffdFoundData.cFileName;
		vAddons.push_back( caeTemp );
		FindNextFile( hFound, &ffdFoundData );
	}
	/* End: Find all addon DLLs */

	/* Beg: Load all addon DLLs */
	if ( !vAddons.empty( ) )
	{
		for ( int i = 0; i < vAddons.size( ); i++ )
		{
			char strModuleName[1024];
			sprintf( strModuleName, "sparkyutils\\addons\\%s", vAddons[i].szAddonName.c_str( ) );

			// Load the library
			vAddons[i].hAddonModule = LoadLibrary( strModuleName );

			// Get the addresses of our exported functions
			if ( ! ( vAddons[i].pfnNeedsEnginefuncs = ( bool( * )( void ) )GetProcAddress( vAddons[i].hAddonModule, "NeedsEnginefuncs" ) ) )
				vAddons[i].pfnNeedsEnginefuncs = NULL;
			if ( ! ( vAddons[i].pfnGetAddonTitle = ( char*( * )( void ) )GetProcAddress( vAddons[i].hAddonModule, "GetAddonTitle" ) ) )
				vAddons[i].pfnGetAddonTitle = NULL;			
			if ( ! ( vAddons[i].pfnGetAddonAuthor = ( char*( * )( void ) )GetProcAddress( vAddons[i].hAddonModule, "GetAddonAuthor" ) ) )
				vAddons[i].pfnGetAddonAuthor = NULL;			
			if ( ! ( vAddons[i].pfnGetAddonVersion = ( char*( * )( void ) )GetProcAddress( vAddons[i].hAddonModule, "GetAddonVersion" ) ) )
				vAddons[i].pfnGetAddonVersion = NULL;
			if ( ! ( vAddons[i].pfnGetAddonCommand = ( char*( * )( void ) )GetProcAddress( vAddons[i].hAddonModule, "GetAddonCommand" ) ) )
				vAddons[i].pfnGetAddonCommand = NULL;
			if ( ! ( vAddons[i].pfnGetAddonDescription = ( char*( * )( void ) )GetProcAddress( vAddons[i].hAddonModule, "GetAddonDescription" ) ) )
				vAddons[i].pfnGetAddonDescription = NULL;
			
			if ( ! ( vAddons[i].pfnPost_HUD_Init = ( void( * )( void ) )GetProcAddress( vAddons[i].hAddonModule, "Post_HUD_Init" ) ) )
				vAddons[i].pfnPost_HUD_Init = NULL;			
			if ( ! ( vAddons[i].pfnPost_HUD_Frame = ( void( * )( double ) )GetProcAddress( vAddons[i].hAddonModule, "Post_HUD_Frame" ) ) )
				vAddons[i].pfnPost_HUD_Frame = NULL;			
			if ( ! ( vAddons[i].pfnPost_CL_CreateMove = ( void( * )( float, struct usercmd_s *, int ) )GetProcAddress( vAddons[i].hAddonModule, "Post_CL_CreateMove" ) ) )
				vAddons[i].pfnPost_CL_CreateMove = NULL;
			if ( ! ( vAddons[i].pfnPost_HUD_PlayerMove = ( void( * )( struct playermove_s *, int ) )GetProcAddress( vAddons[i].hAddonModule, "Post_HUD_PlayerMove" ) ) )
				vAddons[i].pfnPost_HUD_PlayerMove = NULL;
			if ( ! ( vAddons[i].pfnPre_HUD_Key_Event = ( bool( * )( int, int, const char * ) )GetProcAddress( vAddons[i].hAddonModule, "Pre_HUD_Key_Event" ) ) )
				vAddons[i].pfnPre_HUD_Key_Event = NULL;
			if ( ! ( vAddons[i].pfnPost_HUD_Key_Event = ( void( * )( int, int, const char * ) )GetProcAddress( vAddons[i].hAddonModule, "Post_HUD_Key_Event" ) ) )
				vAddons[i].pfnPost_HUD_Key_Event = NULL;
			if ( ! ( vAddons[i].pfnPre_HUD_Redraw = ( bool( * )( float, int ) )GetProcAddress( vAddons[i].hAddonModule, "Pre_HUD_Redraw" ) ) )
				vAddons[i].pfnPre_HUD_Redraw = NULL;
			if ( ! ( vAddons[i].pfnPost_HUD_Redraw = ( void( * )( float, int ) )GetProcAddress( vAddons[i].hAddonModule, "Post_HUD_Redraw" ) ) )
				vAddons[i].pfnPost_HUD_Redraw = NULL;
			if ( ! ( vAddons[i].pfnPost_HUD_VoiceStatus = ( void( * )( int, qboolean ) )GetProcAddress( vAddons[i].hAddonModule, "Post_HUD_VoiceStatus" ) ) )
				vAddons[i].pfnPost_HUD_VoiceStatus = NULL;
			if ( ! ( vAddons[i].pfnPost_Hook_TextMsg = ( void( * )( const char * ) )GetProcAddress( vAddons[i].hAddonModule, "Post_Hook_TextMsg" ) ) )
				vAddons[i].pfnPost_Hook_TextMsg = NULL;
			if ( ! ( vAddons[i].pfnPre_Hook_SayText = ( bool( * )( const char * ) )GetProcAddress( vAddons[i].hAddonModule, "Pre_Hook_SayText" ) ) )
				vAddons[i].pfnPre_Hook_SayText = NULL;
			if ( ! ( vAddons[i].pfnPost_Hook_SayText = ( void( * )( const char * ) )GetProcAddress( vAddons[i].hAddonModule, "Post_Hook_SayText" ) ) )
				vAddons[i].pfnPost_Hook_SayText = NULL;			
			if ( ! ( vAddons[i].pfnPost_Hook_HudText = ( void( * )( const char * ) )GetProcAddress( vAddons[i].hAddonModule, "Post_Hook_HudText" ) ) )
				vAddons[i].pfnPost_Hook_HudText = NULL;
			if ( ! ( vAddons[i].pfnPost_Hook_DeathMsg = ( void( * )( int, int, const char * ) )GetProcAddress( vAddons[i].hAddonModule, "Post_Hook_DeathMsg" ) ) )
				vAddons[i].pfnPost_Hook_DeathMsg = NULL;
			if ( ! ( vAddons[i].pfnPost_Hook_ConsoleOutput = ( void( * )( const char * ) )GetProcAddress( vAddons[i].hAddonModule, "Post_Hook_ConsoleOutput" ) ) )
				vAddons[i].pfnPost_Hook_ConsoleOutput = NULL;
			if ( ! ( vAddons[i].pfnPost_Hook_AmmoX = ( void( * )( int, int ) )GetProcAddress( vAddons[i].hAddonModule, "Post_Hook_AmmoX" ) ) )
				vAddons[i].pfnPost_Hook_AmmoX = NULL;
			if ( ! ( vAddons[i].pfnPre_Hook_ConsoleCommand = ( bool( * )( char * ) )GetProcAddress( vAddons[i].hAddonModule, "Pre_Hook_ConsoleCommand" ) ) )
				vAddons[i].pfnPre_Hook_ConsoleCommand = NULL;
			
			// Using our exported functions to set up stuff
			if ( vAddons[i].pfnNeedsEnginefuncs != NULL )
			{
				if ( vAddons[i].pfnNeedsEnginefuncs( ) )
				{
					// Only do this if the dll needs to use enginefuncs
					if ( ! ( vAddons[i].pfnPassGameInfo = ( void( * )( cl_enginefunc_t *, CSuperHUD *, CPlayerinfo * ) )GetProcAddress( vAddons[i].hAddonModule, "PassGameInfo" ) ) )
						vAddons[i].pfnPassGameInfo = NULL;

					/// Pass some enginefunc stuff to the dll
					if ( vAddons[i].pfnPassGameInfo != NULL )
						vAddons[i].pfnPassGameInfo( &gEngfuncs, &gSuperHUD, &gPlayerinfo );
				}
			}

			// Added in error checking so we don't try to access stuff that doesn't exist!
			if ( vAddons[i].pfnGetAddonTitle != NULL )
				vAddons[i].pszAddonTitle = vAddons[i].pfnGetAddonTitle( );
			else
				vAddons[i].pszAddonTitle = "No title";
			if ( vAddons[i].pfnGetAddonAuthor != NULL )
				vAddons[i].pszAddonAuthor = vAddons[i].pfnGetAddonAuthor( );
			else
				vAddons[i].pszAddonAuthor = "No author";
			if ( vAddons[i].pfnGetAddonVersion != NULL )
				vAddons[i].pszAddonVersion = vAddons[i].pfnGetAddonVersion( );
			else
				vAddons[i].pszAddonVersion = "No version";
			if ( vAddons[i].pfnGetAddonCommand != NULL )
				vAddons[i].pszAddonCommand = vAddons[i].pfnGetAddonCommand( );
			else
				vAddons[i].pszAddonCommand = "No command - this is bad";
			if ( vAddons[i].pfnGetAddonDescription != NULL )
				vAddons[i].pszAddonDescription = vAddons[i].pfnGetAddonDescription( );
			else
				vAddons[i].pszAddonDescription = "No description";
			vAddons[i].bAddonRunning = false;			
		}
	}
	/* End: Load all addon DLLs */

	gEngfuncs.pfnConsolePrint( "done\n" );

	/* Beg: Check the sparky_addons.cfg */
	FILE *fpInfile = 0;
	if ( fpInfile = fopen( "sparkyutils\\sparky_addons.cfg", "r" ) )
	{
		char strLine[1024] = {0};

		while ( fgets( strLine, 1024, fpInfile ) )
		{
			if ( strlen(strLine) >= 3 )
			{
				if (( strLine[0] != '/' ) && ( strLine[1] != '/' ))
					gEngfuncs.pfnClientCmd( strLine );
			}
		}

		fclose( fpInfile );
	}
	/* End: Check the sparky_addons.cfg */
}

void CAddons::Post_HUD_Frame (double time)
{
	if ( !vAddons.empty( ) )
	{
		for ( int i = 0; i < vAddons.size( ); i++ )
		{
			if ( vAddons[i].bAddonRunning )
			{
				if ( vAddons[i].pfnPost_HUD_Frame != NULL )
					vAddons[i].pfnPost_HUD_Frame( time );
			}
		}
	}
}

void CAddons::Post_CL_CreateMove (float frametime, struct usercmd_s *cmd, int active)
{
	if ( !vAddons.empty( ) )
	{
		for ( int i = 0; i < vAddons.size( ); i++ )
		{
			if ( vAddons[i].bAddonRunning )
			{
				if ( vAddons[i].pfnPost_CL_CreateMove != NULL )
					vAddons[i].pfnPost_CL_CreateMove( frametime, cmd, active );
			}
		}
	}
}

void CAddons::Post_HUD_PlayerMove (struct playermove_s *ppmove, int server)
{
	if ( !vAddons.empty( ) )
	{
		for ( int i = 0; i < vAddons.size( ); i++ )
		{
			if ( vAddons[i].bAddonRunning )
			{
				if ( vAddons[i].pfnPost_HUD_PlayerMove != NULL )
					vAddons[i].pfnPost_HUD_PlayerMove( ppmove, server );
			}
		}
	}
}

bool CAddons::Pre_HUD_Key_Event (int eventcode, int keynum, const char *pszCurrentBinding)
{
	if ( !vAddons.empty( ) )
	{
		for ( int i = 0; i < vAddons.size( ); i++ )
		{
			if ( vAddons[i].bAddonRunning )
			{
				if ( vAddons[i].pfnPre_HUD_Key_Event != NULL )
				{
					if ( !vAddons[i].pfnPre_HUD_Key_Event( eventcode, keynum, pszCurrentBinding ))
						return false;
				}
			}
		}
	}

	return true;
}

void CAddons::Post_HUD_Key_Event (int eventcode, int keynum, const char *pszCurrentBinding)
{
	if ( !vAddons.empty( ) )
	{
		for ( int i = 0; i < vAddons.size( ); i++ )
		{
			if ( vAddons[i].bAddonRunning )
			{
				if ( vAddons[i].pfnPost_HUD_Key_Event != NULL )
					vAddons[i].pfnPost_HUD_Key_Event( eventcode, keynum, pszCurrentBinding );
			}
		}
	}
}

bool CAddons::Pre_HUD_Redraw (float flTime, int intermission)
{
	if ( !vAddons.empty( ) )
	{
		for ( int i = 0; i < vAddons.size( ); i++ )
		{
			if ( vAddons[i].bAddonRunning )
			{
				if ( vAddons[i].pfnPre_HUD_Redraw != NULL )
					if(!vAddons[i].pfnPre_HUD_Redraw( flTime, intermission ))
						return false;
			}
		}
	}
	return true;
}

void CAddons::Post_HUD_Redraw (float flTime, int intermission)
{
	if ( !vAddons.empty( ) )
	{
		for ( int i = 0; i < vAddons.size( ); i++ )
		{
			if ( vAddons[i].bAddonRunning )
			{
				if ( vAddons[i].pfnPost_HUD_Redraw != NULL )
					vAddons[i].pfnPost_HUD_Redraw( flTime, intermission );
			}
		}
	}
}

void CAddons::Post_HUD_VoiceStatus (int entindex, qboolean bTalking)
{
	if ( !vAddons.empty( ) )
	{
		for ( int i = 0; i < vAddons.size( ); i++ )
		{
			if ( vAddons[i].bAddonRunning )
			{
				if ( vAddons[i].pfnPost_HUD_VoiceStatus != NULL )
					vAddons[i].pfnPost_HUD_VoiceStatus( entindex, bTalking );
			}
		}
	}
}

void CAddons::Post_Hook_TextMsg (const char *pszString)
{
	if ( !vAddons.empty( ) )
	{
		for ( int i = 0; i < vAddons.size( ); i++ )
		{
			if ( vAddons[i].bAddonRunning )
			{
				if ( vAddons[i].pfnPost_Hook_TextMsg != NULL )
					vAddons[i].pfnPost_Hook_TextMsg( pszString );
			}
		}
	}
}

bool CAddons::Pre_Hook_SayText (const char *pszString)
{
	if ( !vAddons.empty( ) )
	{
		for ( int i = 0; i < vAddons.size( ); i++ )
		{
			if ( vAddons[i].bAddonRunning )
			{
				if ( vAddons[i].pfnPre_Hook_SayText != NULL )
				{
					if ( !vAddons[i].pfnPre_Hook_SayText( pszString ) )
						return false;
				}
			}
		}
	}

	return true;
}

void CAddons::Post_Hook_SayText (const char *pszString)
{
	if ( !vAddons.empty( ) )
	{
		for ( int i = 0; i < vAddons.size( ); i++ )
		{
			if ( vAddons[i].bAddonRunning )
			{
				if ( vAddons[i].pfnPost_Hook_SayText != NULL )
					vAddons[i].pfnPost_Hook_SayText( pszString );
			}
		}
	}
}

void CAddons::Post_Hook_HudText (const char *pszString)
{
	if ( !vAddons.empty( ) )
	{
		for ( int i = 0; i < vAddons.size( ); i++ )
		{
			if ( vAddons[i].bAddonRunning )
			{
				if ( vAddons[i].pfnPost_Hook_HudText != NULL )
					vAddons[i].pfnPost_Hook_HudText( pszString );
			}
		}
	}
}

void CAddons::Post_Hook_DeathMsg (int killer, int victim, const char *pszWeapon)
{
	if ( !vAddons.empty( ) )
	{
		for ( int i = 0; i < vAddons.size( ); i++ )
		{
			if ( vAddons[i].bAddonRunning )
			{
				if ( vAddons[i].pfnPost_Hook_DeathMsg != NULL )
					vAddons[i].pfnPost_Hook_DeathMsg( killer, victim, pszWeapon );
			}
		}
	}
}

void CAddons::Post_Hook_ConsoleOutput (const char *pszString)
{
	if ( !vAddons.empty( ) )
	{
		for ( int i = 0; i < vAddons.size( ); i++ )
		{
			if ( vAddons[i].bAddonRunning )
			{
				if ( vAddons[i].pfnPost_Hook_ConsoleOutput != NULL )
					vAddons[i].pfnPost_Hook_ConsoleOutput( pszString );
			}
		}
	}
}

void CAddons::Post_Hook_AmmoX (int iIndex, int iCount)
{
	if ( !vAddons.empty( ) )
	{
		for ( int i = 0; i < vAddons.size( ); i++ )
		{
			if ( vAddons[i].bAddonRunning )
			{
				if ( vAddons[i].pfnPost_Hook_AmmoX != NULL )
					vAddons[i].pfnPost_Hook_AmmoX( iIndex, iCount );
			}
		}
	}
}

bool CAddons::Pre_Hook_ConsoleCommand (char *pszString)
{
	// Check if a addon is being turned on / off
	if ( !vAddons.empty( ) )
	{
		for ( int i = 0; i < vAddons.size( ); i++ )
		{			
			if ( strcmp( pszString, vAddons[i].pszAddonCommand ) == 0 )
			{
				vAddons[i].bAddonRunning = !vAddons[i].bAddonRunning;

				char buf[1024];
				
				if ( vAddons[i].bAddonRunning )
				{
					sprintf( buf, "[sparky] [addon] Loading (%s): \"%s\" by: %s, v%s: %s\n", vAddons[i].szAddonName.c_str( ), vAddons[i].pszAddonTitle, vAddons[i].pszAddonAuthor, vAddons[i].pszAddonVersion, vAddons[i].pszAddonDescription );
					gEngfuncs.pfnConsolePrint( buf );

					// Let the DLL init whatever it needs to!
					if ( vAddons[i].pfnPost_HUD_Init != NULL )
						vAddons[i].pfnPost_HUD_Init( );
				}
				else
				{
					sprintf( buf, "[sparky] [addon] Unloading (%s): \"%s\"\n", vAddons[i].szAddonName.c_str( ), vAddons[i].pszAddonTitle );
					gEngfuncs.pfnConsolePrint( buf );
				}
			}
		}
	}

	// Send a command to the addon
	if ( !vAddons.empty( ) )
	{
		for ( int i = 0; i < vAddons.size( ); i++ )
		{
			if ( vAddons[i].bAddonRunning )
			{
				if ( vAddons[i].pfnPre_Hook_ConsoleCommand != NULL )
				{
					if ( !vAddons[i].pfnPre_Hook_ConsoleCommand( pszString ) )
						return false;
				}
			}
		}
	}

	return true;
}

CAddons::CAddons (void)
{
}

CAddons::~CAddons (void)
{
	// Free our libraries
	if ( !vAddons.empty( ) )
	{
		for ( int i = 0; i < vAddons.size( ); i++ )
		{
			FreeLibrary( vAddons[i].hAddonModule );
		}
	}
}
