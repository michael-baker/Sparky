/*  ------------------------------------------------------------------------
	  addons.h by Mulchman
		- Cleaned up some tiny bugs (v1.44)
	    - Error prevention and more functions added (v1.43)
		- Generic addons loader and manager (v1.42)
	------------------------------------------------------------------------ */

#ifndef ADDONS_H
#define ADDONS_H

#include <string>
#include <vector>

using namespace std;

struct CAddonEntry
{
	string szAddonName;	// the filename

	char *pszAddonTitle;
	char *pszAddonAuthor;
	char *pszAddonVersion;
	char *pszAddonCommand;
	char *pszAddonDescription;

	bool bAddonRunning;

	HMODULE hAddonModule;

	bool ( *pfnNeedsEnginefuncs )( void );
	void ( *pfnPassGameInfo )( cl_enginefunc_t *pEngfuncs, CSuperHUD *pSuperHUD, CPlayerinfo *pPlayerinfo );

	char* ( *pfnGetAddonTitle )( void );
	char* ( *pfnGetAddonAuthor )( void );
	char* ( *pfnGetAddonVersion )( void );
	char* ( *pfnGetAddonCommand )( void );
	char* ( *pfnGetAddonDescription )( void );
	
	void ( *pfnPost_HUD_Init )( void );
	void ( *pfnPost_HUD_Frame )( double time );
	void ( *pfnPost_CL_CreateMove )( float frametime, struct usercmd_s *cmd, int active );
	void ( *pfnPost_HUD_PlayerMove )( struct playermove_s *ppmove, int server );
	bool ( *pfnPre_HUD_Key_Event )( int eventcode, int keynum, const char *pszCurrentBinding );
	void ( *pfnPost_HUD_Key_Event )( int eventcode, int keynum, const char *pszCurrentBinding );
	bool ( *pfnPre_HUD_Redraw )( float flTime, int intermission );
	void ( *pfnPost_HUD_Redraw )( float flTime, int intermission );	
	void ( *pfnPost_HUD_VoiceStatus )( int entindex, qboolean bTalking );
	void ( *pfnPost_Hook_TextMsg )( const char *pszString );
	bool ( *pfnPre_Hook_SayText )( const char *pszString );
	void ( *pfnPost_Hook_SayText )( const char *pszString );
	void ( *pfnPost_Hook_HudText )( const char *pszString );
	void ( *pfnPost_Hook_DeathMsg )( int killer, int victim, const char *pszWeapon );
	void ( *pfnPost_Hook_ConsoleOutput )( const char *pszString );
	void ( *pfnPost_Hook_AmmoX )( int iIndex, int iCount );
	bool ( *pfnPre_Hook_ConsoleCommand )( char *pszString );
};

class CAddons : CBaseHook
{
	public:
	
	virtual void Post_HUD_Init (void);
	virtual void Post_HUD_Frame (double time);
	virtual void Post_CL_CreateMove (float frametime, struct usercmd_s *cmd, int active);
	virtual void Post_HUD_PlayerMove (struct playermove_s *ppmove, int server);
	virtual bool Pre_HUD_Key_Event (int eventcode, int keynum, const char *pszCurrentBinding);
	virtual void Post_HUD_Key_Event (int eventcode, int keynum, const char *pszCurrentBinding);
	virtual bool Pre_HUD_Redraw (float flTime, int intermission);
	virtual void Post_HUD_Redraw (float flTime, int intermission);
	virtual void Post_HUD_VoiceStatus (int entindex, qboolean bTalking);	
	virtual void Post_Hook_TextMsg (const char *pszString);	
	virtual bool Pre_Hook_SayText (const char *pszString);
	virtual void Post_Hook_SayText (const char *pszString);
	virtual void Post_Hook_HudText (const char *pszString);	
	virtual void Post_Hook_DeathMsg (int killer, int victim, const char *pszWeapon);
	virtual void Post_Hook_ConsoleOutput (const char *pszString);	
	virtual void Post_Hook_AmmoX (int iIndex, int iCount);
	virtual bool Pre_Hook_ConsoleCommand (char *pszString);

	CAddons (void);
	~CAddons (void);

	vector<CAddonEntry> vAddons;

};

extern CAddons gAddons;

#endif
