#include "client.h"
#include "billstuff\hook.h"
#include "billstuff\superhud.h"

#pragma warning (disable:4244) // float to int loss of data

// pain sprite stored so its color isnt changed
HSPRITE gPainSprite;
HSPRITE gDot;
HSPRITE gSPRLogo;

// Efx API hooks (incomplete, only some are hooked)
// note that these hooks are virtually USELESS because the cleint.dll doesn't use them
// messages from the server to display an effect are sent straight to the engine and dont part through here

/*
	BEAM		*( *R_BeamEnts )				( int startEnt, int endEnt, int modelIndex, float life, float width, float amplitude, float brightness, float speed, int startFrame, float framerate, float r, float g, float b );
	BEAM		*( *R_BeamFollow )				( int startEnt, int modelIndex, float life, float width, float r, float g, float b, float brightness );
*/

BEAM *hookR_BeamFollow( int startEnt, int modelIndex, float life, float width, float r, float g, float b, float brightness )
{
	BEAM *retval;
	//retval = gEngfuncs.pEfxAPI->R_BeamFollow(startEnt, modelIndex, life, width, r, g, b, brightness );
	retval = gEngfuncs.pEfxAPI->R_BeamFollow(startEnt, modelIndex, life, width, gEngfuncs.pfnRandomLong(0,255), gEngfuncs.pfnRandomLong(0,255), gEngfuncs.pfnRandomLong(0,255), brightness );
	return retval;
}

// sprite handlers
HSPRITE hookSPR_Load( const char *szPicName )
{
	HSPRITE retval = gEngfuncs.pfnSPR_Load(szPicName);
	if(!strcmp(szPicName, "sprites/640_pain.spr"))
	{
		gPainSprite = retval;
	}
	if(!strcmp(szPicName, "sprites/dot.spr"))
	{
		gDot = retval;
	}
	if(!strcmp(szPicName, "../sparkyutils/logo.spr"))
	{
		gSPRLogo = retval;
	}
	return retval;
};

int hookSPR_Frames( HSPRITE hPic )
{
	int retval = gEngfuncs.pfnSPR_Frames(hPic);
	return retval;
}

int hookSPR_Height( HSPRITE hPic, int frame )
{
	int retval = gEngfuncs.pfnSPR_Height(hPic, frame);
	return retval;
}

int hookSPR_Width( HSPRITE hPic, int frame )
{
	int retval = gEngfuncs.pfnSPR_Width(hPic, frame);
	return retval;
}

void hookSPR_Set(HSPRITE hPic, int r, int g, int b )
{
	// skip pain sprite & sparky logo
	if((hPic == gPainSprite) || (hPic == gSPRLogo))
	{
		gEngfuncs.pfnSPR_Set(hPic, r, g, b);
		return;
	}
	// skip red sprites (low health, red flag)
	if((r>(g*2)) && (b==0))
	{
		gEngfuncs.pfnSPR_Set(hPic, r, g, b);
		return;
	}
	// skip blue sprites (blue flag)
	if((b>(g*2)) && (r==0))
	{
		gEngfuncs.pfnSPR_Set(hPic, r, g, b);
		return;
	}
	// normal client DLL uses the color to apply fading to updated hud items, so calculate brightness first
	// then reapply it once the color is altered
	Vector HUDColor = gSuperHUD.HUDColor;
	float fBrightness = (((float)(r + g + b))/(3*255));
	fBrightness *= gSuperHUD.fHUDBrightness;
	if(gSuperHUD.fHUDBrightness > -1)	// not rendered in white (coloured sprites)
	{
		r = HUDColor.x * fBrightness;
		if(r > 255) r = 255;
		g = HUDColor.y * fBrightness;
		if(g > 255) g = 255;
		b = HUDColor.z * fBrightness;
		if(b > 255) b = 255;
	}
	gEngfuncs.pfnSPR_Set(hPic, r, g, b);
}

void hookSPR_Draw( int frame, int x, int y, const wrect_t *prc )
{
	gEngfuncs.pfnSPR_Draw(frame, x, y, prc);
}

void hookSPR_DrawHoles( int frame, int x, int y, const wrect_t *prc )
{
	gEngfuncs.pfnSPR_DrawHoles(frame, x, y, prc);
}

void hookSPR_DrawAdditive( int frame, int x, int y, const wrect_t *prc )
{
	gEngfuncs.pfnSPR_DrawAdditive(frame, x, y, prc);
}

void hookSPR_EnableScissor( int x, int y, int width, int height )
{
	gEngfuncs.pfnSPR_EnableScissor(x, y, width, height);
}

void hookSPR_DisableScissor( void )
{
}

client_sprite_t *hookSPR_GetList( char *psz, int *piCount )
{
	client_sprite_t *retval;
	retval = gEngfuncs.pfnSPR_GetList(psz, piCount);
	return retval;
}

	// screen handlers
void hookFillRGBA( int x, int y, int width, int height, int r, int g, int b, int a )
{
	Vector HUDColor = gSuperHUD.HUDColor;
	//float fBrightness = (((float)(r + g + b))/(3*255)) * gSuperHUD.fHUDBrightness;
	if(gSuperHUD.fHUDBrightness > -1)
	{
		r = HUDColor.x;// * fBrightness;
		//if(r > 255) r = 255;
		g = HUDColor.y;// * fBrightness;
		//if(g > 255) g = 255;
		b = HUDColor.z;// * fBrightness;
		//if(b > 255) b = 255;
	}
	gEngfuncs.pfnFillRGBA(x,y,width,height,r,g,b,a);
}

int	hookGetScreenInfo( SCREENINFO *pscrinfo )
{
	int retval = 0;
	retval = gEngfuncs.pfnGetScreenInfo(pscrinfo);
	return retval;
}

void hookSetCrosshair( HSPRITE hspr, wrect_t rc, int r, int g, int b )
{
	/*
	Vector HUDColor = gSuperHUD.HUDColor;
	if(gSuperHUD.fHUDBrightness > -1)
	{
		r = HUDColor.x;// * fBrightness;
		g = HUDColor.y;// * fBrightness;
		b = HUDColor.z;// * fBrightness;
	}
	*/
	gEngfuncs.pfnSetCrosshair(hspr, rc, r, g, b);
}

// cvar handlers
struct cvar_s *hookRegisterVariable( char *szName, char *szValue, int flags )
{
	struct cvar_s *varptr;
	varptr = gEngfuncs.pfnRegisterVariable (szName, szValue, flags);
	return varptr;
}

float hookGetCvarFloat( char *szName )
{
	float retval;
	retval = gEngfuncs.pfnGetCvarFloat(szName);
	return retval;
}

char *hookGetCvarString( char *szName )
{
	char *retval;
	retval = gEngfuncs.pfnGetCvarString(szName);
	return retval;
}

	// command handlers
int hookAddCommand( char *cmd_name, void (*function)(void) )
{
	int retval;
	/*
	char buf[1024];
	sprintf(buf, "echo [%s] >> hookedconsolecommands.txt", cmd_name);
	system(buf);
	*/
	retval = gEngfuncs.pfnAddCommand(cmd_name, function);
	return retval;
}

int hookHookUserMsg( char *szMsgName, pfnUserMsgHook pfn )
{
	int retval;
	/*
	char buf[1024];
	sprintf(buf, "echo [%s] >> hookedusermessages.txt", szMsgName);
	system(buf);
	*/

	pfnUserMsgHook hook = CheckAddHook (szMsgName, pfn);

	if ( hook )
		pfn = hook;

	retval = gEngfuncs.pfnHookUserMsg(szMsgName, pfn);
	return retval;
}

int hookServerCmd( char *szCmdString )
{
	int retval;
	retval = gEngfuncs.pfnServerCmd(szCmdString);
	return retval;
}

int hookClientCmd( char *szCmdString )
{
	int retval;
	retval = gEngfuncs.pfnClientCmd(szCmdString);
	return retval;
}

void hookGetPlayerInfo( int ent_num, hud_player_info_t *pinfo )
{
	gEngfuncs.pfnGetPlayerInfo(ent_num, pinfo);
}


	// sound handlers
void hookPlaySoundByName( char *szSound, float volume )
{
	gEngfuncs.pfnPlaySoundByName(szSound, volume);
}

void hookPlaySoundByIndex( int iSound, float volume )
{
	gEngfuncs.pfnPlaySoundByIndex(iSound, volume);
}

	// vector helpers
void hookAngleVectors( const float * vecAngles, float * forward, float * right, float * up )
{
	gEngfuncs.pfnAngleVectors(vecAngles, forward, right, up);
}

	// text message system
client_textmessage_t *hookTextMessageGet( const char *pName )
{
	client_textmessage_t *retval;
	retval = gEngfuncs.pfnTextMessageGet(pName);
	return retval;
}

int hookDrawCharacter( int x, int y, int number, int r, int g, int b )
{
	int retval;
	retval = gEngfuncs.pfnDrawCharacter(x, y, number, r, g, b);
	return retval;
}

int hookDrawConsoleString( int x, int y, char *string )
{
	int retval;
	retval = gEngfuncs.pfnDrawConsoleString(x, y, string);
	return retval;
}

void hookDrawSetTextColor( float r, float g, float b )
{
	gEngfuncs.pfnDrawSetTextColor(r, g, b);
}

void hookDrawConsoleStringLen(  const char *string, int *length, int *height )
{
	gEngfuncs.pfnDrawConsoleStringLen(string, length, height);
}

void hookConsolePrint( const char *string )
{
	gEngfuncs.pfnConsolePrint(string);
}

void hookCenterPrint( const char *string )
{
	gEngfuncs.pfnCenterPrint(string);
}



// Added for user input processing
int hookGetWindowCenterX( void )
{
	int retval;
	retval = gEngfuncs.GetWindowCenterX();
	return retval;
}

int hookGetWindowCenterY( void )
{
	int retval;
	retval = gEngfuncs.GetWindowCenterY();
	return retval;
}

void hookGetViewAngles( float *ang)
{
	gEngfuncs.GetViewAngles(ang);
}


void hookSetViewAngles( float *ang)
{
	gEngfuncs.SetViewAngles(ang);
}

int hookGetMaxClients( void )
{
	int retval;
	retval = gEngfuncs.GetMaxClients();
	return retval;
}

void hookCvar_SetValue( char *cvar, float value )
{
	gEngfuncs.Cvar_SetValue(cvar, value);
}

int hookCmd_Argc(void)
{
	int retval;
	retval = gEngfuncs.Cmd_Argc();
	return retval;
}

char *hookCmd_Argv( int arg )
{
	char *retval;
	retval = gEngfuncs.Cmd_Argv(arg);
	return retval;
}

void hookCon_Printf( char *fmt, ... )
{
	va_list va_alist;
	char buf [8192];

	va_start (va_alist, fmt);
	_vsnprintf (buf, sizeof(buf)-1, fmt, va_alist);
	va_end (va_alist);
	buf[sizeof(buf)-1] = '\0';

	gEngfuncs.Con_Printf ("%s", buf);
}

void hookCon_DPrintf( char *fmt, ... )
{
	va_list va_alist;
	char buf [8192];

	va_start (va_alist, fmt);
	_vsnprintf (buf, sizeof(buf)-1, fmt, va_alist);
	va_end (va_alist);
	buf[sizeof(buf)-1] = '\0';

	gEngfuncs.Con_DPrintf ("%s", buf);
}

void hookCon_NPrintf( int pos, char *fmt, ... )
{
	va_list va_alist;
	char buf [8192];

	va_start (va_alist, fmt);
	_vsnprintf (buf, sizeof(buf)-1, fmt, va_alist);
	va_end (va_alist);
	buf[sizeof(buf)-1] = '\0';

	gEngfuncs.Con_NPrintf (pos, "%s", buf);
}

void hookCon_NXPrintf( struct con_nprint_s *info, char *fmt, ... )
{
	va_list va_alist;
	char buf [8192];

	va_start (va_alist, fmt);
	_vsnprintf (buf, sizeof(buf)-1, fmt, va_alist);
	va_end (va_alist);
	buf[sizeof(buf)-1] = '\0';

	gEngfuncs.Con_NXPrintf (info, "%s", buf);
}

const char *hookPhysInfo_ValueForKey( const char *key )
{
	const char *retval;
	retval = gEngfuncs.PhysInfo_ValueForKey(key);
	return retval;
}

const char *hookServerInfo_ValueForKey( const char *key )
{
	const char *retval;
	retval = gEngfuncs.ServerInfo_ValueForKey(key);
	return retval;
}

float hookGetClientMaxspeed( void )
{
	float retval;
	retval = gEngfuncs.GetClientMaxspeed();
	return retval;
}

int hookCheckParm( char *parm, char **ppnext )
{
	int retval;
	retval = gEngfuncs.CheckParm(parm, ppnext);
	return retval;
}

void hookKey_Event( int key, int down )
{
	gEngfuncs.Key_Event(key, down);
}

void hookGetMousePosition( int *mx, int *my )
{
	gEngfuncs.GetMousePosition(mx, my);
}

int hookIsNoClipping( void )
{
	int retval;
	retval = gEngfuncs.IsNoClipping();
	return retval;
}

struct cl_entity_s *hookGetLocalPlayer( void )
{
	struct cl_entity_s *retval;
	retval = gEngfuncs.GetLocalPlayer();
	return retval;
}

struct cl_entity_s *hookGetViewModel( void )
{
	struct cl_entity_s *retval;
	retval = gEngfuncs.GetViewModel();
	return retval;
}

struct cl_entity_s *hookGetEntityByIndex( int idx )
{
	struct cl_entity_s *retval;
	retval = gEngfuncs.GetEntityByIndex(idx);
	return retval;
}

float hookGetClientTime( void )
{
	float retval;
	retval = gEngfuncs.GetClientTime();
	return retval;
}

void hookV_CalcShake( void )
{
	gEngfuncs.V_CalcShake();
}

void hookV_ApplyShake( float *origin, float *angles, float factor )
{
	gEngfuncs.V_ApplyShake(origin, angles, factor);
}

int hookPM_PointContents( float *point, int *truecontents )
{
	int retval;
	retval = gEngfuncs.PM_PointContents(point, truecontents);
	return retval;
}

int hookPM_WaterEntity( float *p )
{
	int retval;
	retval = gEngfuncs.PM_WaterEntity(p);
	return retval;
}

struct pmtrace_s *hookPM_TraceLine( float *start, float *end, int flags, int usehull, int ignore_pe )
{
	pmtrace_s *retval;
	retval = gEngfuncs.PM_TraceLine(start, end, flags, usehull, ignore_pe);
	return retval;
}

struct model_s *hookCL_LoadModel( const char *modelname, int *index )
{
	model_s *retval;
	retval = gEngfuncs.CL_LoadModel(modelname, index);
	return retval;
}

int hookCL_CreateVisibleEntity( int type, struct cl_entity_s *ent )
{
	int retval;
	retval = gEngfuncs.CL_CreateVisibleEntity(type, ent);
	return retval;
}

const struct model_s *hookGetSpritePointer( HSPRITE hSprite )
{
	const model_s *retval;
	retval = gEngfuncs.GetSpritePointer(hSprite);
	return retval;
}

void hookPlaySoundByNameAtLocation( char *szSound, float volume, float *origin )
{
	gEngfuncs.pfnPlaySoundByNameAtLocation(szSound, volume, origin);
}

unsigned short hookPrecacheEvent( int type, const char* psz )
{
	unsigned short retval;
	retval = gEngfuncs.pfnPrecacheEvent(type, psz);
	return retval;
}

void hookPlaybackEvent( int flags, const struct edict_s *pInvoker, unsigned short eventindex, float delay, float *origin, float *angles, float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2 )
{
	gEngfuncs.pfnPlaybackEvent(flags, pInvoker, eventindex, delay, origin, angles, fparam1, fparam2, iparam1, iparam2, bparam1, bparam2);
}

void hookWeaponAnim( int iAnim, int body )
{
	gEngfuncs.pfnWeaponAnim(iAnim, body);
}

float hookRandomFloat( float flLow, float flHigh )
{
	float retval;
	retval = gEngfuncs.pfnRandomFloat(flLow, flHigh);
	return retval;
}

long hookRandomLong( long lLow, long lHigh )
{
	long retval;
	retval = gEngfuncs.pfnRandomLong(lLow, lHigh);
	return retval;
}

void hookHookEvent( char *name, void ( *pfnEvent )( struct event_args_s *args ) )
{
	gEngfuncs.pfnHookEvent(name, pfnEvent);
}

int hookCon_IsVisible()
{
	int retval;
	retval = gEngfuncs.Con_IsVisible();
	return retval;
}

const char *hookGetGameDirectory( void )
{
	const char *retval;
	retval = gEngfuncs.pfnGetGameDirectory();
	return retval;
}

struct cvar_s *hookGetCvarPointer( const char *szName )
{
	cvar_s *retval;
	retval = gEngfuncs.pfnGetCvarPointer(szName);
	return retval;
}

const char *hookKey_LookupBinding( const char *pBinding )
{
	const char *retval;
	retval = gEngfuncs.Key_LookupBinding(pBinding);
	return retval;
}

const char *hookGetLevelName( void )
{
	const char *retval;
	retval = gEngfuncs.pfnGetLevelName();
	return retval;
}

void hookGetScreenFade( struct screenfade_s *fade )
{
	gEngfuncs.pfnGetScreenFade(fade);
}

void hookSetScreenFade( struct screenfade_s *fade )
{
	gEngfuncs.pfnSetScreenFade(fade);
}

void *hookVGui_GetPanel( )
{
	return gEngfuncs.VGui_GetPanel();
}

void hookVGui_ViewportPaintBackground(int extents[4])
{
	gEngfuncs.VGui_ViewportPaintBackground(extents);
}

byte *hookCOM_LoadFile( char *path, int usehunk, int *pLength )
{
	byte *retval;
	retval = gEngfuncs.COM_LoadFile(path, usehunk, pLength);
	return retval;
}

char *hookCOM_ParseFile( char *data, char *token )
{
	char *retval;
	retval = gEngfuncs.COM_ParseFile(data, token);
	return retval;
}

void hookCOM_FreeFile( void *buffer )
{
	gEngfuncs.COM_FreeFile(buffer);
}

/*
	struct triangleapi_s		*pTriAPI;
	struct efx_api_s			*pEfxAPI;
	struct event_api_s			*pEventAPI;
	struct demo_api_s			*pDemoAPI;
	struct net_api_s			*pNetAPI;
	struct IVoiceTweak_s		*pVoiceTweak;
*/

	// returns 1 if the client is a spectator only (connected to a proxy), 0 otherwise or 2 if in dev_overview mode
int hookIsSpectateOnly( void )
{
	int retval;
	retval = gEngfuncs.IsSpectateOnly();
	return retval;
}

struct model_s *hookLoadMapSprite( const char *filename )
{
	model_s *retval;
	retval = gEngfuncs.LoadMapSprite(filename);
	return retval;
}

	// file search functions
void hookCOM_AddAppDirectoryToSearchPath( const char *pszBaseDir, const char *appName )
{
	gEngfuncs.COM_AddAppDirectoryToSearchPath(pszBaseDir, appName);
}

int hookCOM_ExpandFilename( const char *fileName, char *nameOutBuffer, int nameOutBufferSize )
{
	int retval;
	retval = gEngfuncs.COM_ExpandFilename(fileName, nameOutBuffer, nameOutBufferSize);
	return retval;
}

	// User info
	// playerNum is in the range (1, MaxClients)
	// returns NULL if player doesn't exit
	// returns "" if no value is set
const char *hookPlayerInfo_ValueForKey( int playerNum, const char *key )
{
	const char *retval;
	retval = gEngfuncs.PlayerInfo_ValueForKey(playerNum, key);
	return retval;
}

void hookPlayerInfo_SetValueForKey( const char *key, const char *value )
{
	gEngfuncs.PlayerInfo_SetValueForKey(key, value);
}

	// Gets a unique ID for the specified player. This is the same even if you see the player on a different server.
	// iPlayer is an entity index, so client 0 would use iPlayer=1.
	// Returns false if there is no player on the server in the specified slot.
qboolean hookGetPlayerUniqueID(int iPlayer, char playerID[16])
{
	qboolean retval;
	retval = gEngfuncs.GetPlayerUniqueID(iPlayer, playerID);
	return retval;
}

	// TrackerID access
int hookGetTrackerIDForPlayer(int playerSlot)
{
	int retval;
	retval = gEngfuncs.GetTrackerIDForPlayer(playerSlot);
	return retval;
}

int hookGetPlayerForTrackerID(int trackerID)
{
	int retval;
	retval = gEngfuncs.GetPlayerForTrackerID(trackerID);
	return retval;
}
	// Same as pfnServerCmd, but the message goes in the unreliable stream so it can't clog the net stream
	// (but it might not get there).
int hookServerCmdUnreliable( char *szCmdString )
{
	int retval;
	retval = gEngfuncs.pfnServerCmdUnreliable(szCmdString);
	return retval;
}

void InitializeEngineHook()
{
	memcpy(&gHookEngfuncs, &gEngfuncs, sizeof(cl_enginefunc_t));
	gHookEngfuncs.pfnSPR_Load = hookSPR_Load;
	gHookEngfuncs.pfnSPR_Frames = hookSPR_Frames;
	gHookEngfuncs.pfnSPR_Height = hookSPR_Height;
	gHookEngfuncs.pfnSPR_Width = hookSPR_Width;
	gHookEngfuncs.pfnSPR_Set = hookSPR_Set;
	gHookEngfuncs.pfnSPR_Draw = hookSPR_Draw;
	gHookEngfuncs.pfnSPR_DrawHoles = hookSPR_DrawHoles;
	gHookEngfuncs.pfnSPR_DrawAdditive = hookSPR_DrawAdditive;

	gHookEngfuncs.pfnSPR_EnableScissor = hookSPR_EnableScissor;
	gHookEngfuncs.pfnSPR_DisableScissor = hookSPR_DisableScissor;
	gHookEngfuncs.pfnSPR_GetList = hookSPR_GetList;

	gHookEngfuncs.pfnFillRGBA = hookFillRGBA;
	gHookEngfuncs.pfnGetScreenInfo = hookGetScreenInfo;
	gHookEngfuncs.pfnSetCrosshair = hookSetCrosshair;

	gHookEngfuncs.pfnRegisterVariable = hookRegisterVariable;
	gHookEngfuncs.pfnGetCvarFloat = hookGetCvarFloat;
	gHookEngfuncs.pfnGetCvarString = hookGetCvarString;
	gHookEngfuncs.pfnAddCommand = hookAddCommand;
	gHookEngfuncs.pfnHookUserMsg = hookHookUserMsg;
	gHookEngfuncs.pfnServerCmd = hookServerCmd;
	gHookEngfuncs.pfnClientCmd = hookClientCmd;
	gHookEngfuncs.pfnGetPlayerInfo = hookGetPlayerInfo;
	gHookEngfuncs.pfnPlaySoundByName = hookPlaySoundByName;
	gHookEngfuncs.pfnPlaySoundByIndex = hookPlaySoundByIndex;

	gHookEngfuncs.pfnAngleVectors = hookAngleVectors;
	gHookEngfuncs.pfnTextMessageGet = hookTextMessageGet;
	gHookEngfuncs.pfnDrawCharacter = hookDrawCharacter;
	gHookEngfuncs.pfnDrawConsoleString = hookDrawConsoleString;
	gHookEngfuncs.pfnDrawSetTextColor = hookDrawSetTextColor;
	gHookEngfuncs.pfnDrawConsoleStringLen = hookDrawConsoleStringLen;
	gHookEngfuncs.pfnConsolePrint = hookConsolePrint;
	gHookEngfuncs.pfnCenterPrint = hookCenterPrint;

	gHookEngfuncs.GetWindowCenterX = hookGetWindowCenterX;
	gHookEngfuncs.GetWindowCenterY = hookGetWindowCenterY;
	gHookEngfuncs.GetViewAngles = hookGetViewAngles;
	gHookEngfuncs.SetViewAngles = hookSetViewAngles;
	gHookEngfuncs.GetMaxClients = hookGetMaxClients;
	gHookEngfuncs.Cvar_SetValue = hookCvar_SetValue;

	gHookEngfuncs.Cmd_Argc = hookCmd_Argc;
	gHookEngfuncs.Cmd_Argv = hookCmd_Argv;
	gHookEngfuncs.Con_Printf = hookCon_Printf;
	gHookEngfuncs.Con_DPrintf = hookCon_DPrintf;
	gHookEngfuncs.Con_NPrintf = hookCon_NPrintf;
	gHookEngfuncs.Con_NXPrintf = hookCon_NXPrintf;

	gHookEngfuncs.PhysInfo_ValueForKey = hookPhysInfo_ValueForKey;
	gHookEngfuncs.ServerInfo_ValueForKey = hookServerInfo_ValueForKey;
	gHookEngfuncs.GetClientMaxspeed = hookGetClientMaxspeed;
	gHookEngfuncs.CheckParm = hookCheckParm;
	gHookEngfuncs.Key_Event = hookKey_Event;
	gHookEngfuncs.GetMousePosition = hookGetMousePosition;
	gHookEngfuncs.IsNoClipping = hookIsNoClipping;

	gHookEngfuncs.GetLocalPlayer = hookGetLocalPlayer;
	gHookEngfuncs.GetViewModel = hookGetViewModel;
	gHookEngfuncs.GetEntityByIndex = hookGetEntityByIndex;
	gHookEngfuncs.GetClientTime = hookGetClientTime;
	gHookEngfuncs.V_CalcShake = hookV_CalcShake;

	gHookEngfuncs.V_ApplyShake = hookV_ApplyShake;
	gHookEngfuncs.PM_PointContents = hookPM_PointContents;
	gHookEngfuncs.PM_WaterEntity = hookPM_WaterEntity;
	gHookEngfuncs.PM_TraceLine = hookPM_TraceLine;

	gHookEngfuncs.CL_LoadModel = hookCL_LoadModel;
	gHookEngfuncs.CL_CreateVisibleEntity = hookCL_CreateVisibleEntity;
	gHookEngfuncs.GetSpritePointer = hookGetSpritePointer;
	gHookEngfuncs.pfnPlaySoundByNameAtLocation = hookPlaySoundByNameAtLocation;

	gHookEngfuncs.pfnPrecacheEvent = hookPrecacheEvent;
	gHookEngfuncs.pfnPlaybackEvent = hookPlaybackEvent;
	gHookEngfuncs.pfnWeaponAnim = hookWeaponAnim;
	gHookEngfuncs.pfnRandomFloat = hookRandomFloat;
	gHookEngfuncs.pfnRandomLong = hookRandomLong;
	gHookEngfuncs.pfnHookEvent = hookHookEvent;

	gHookEngfuncs.Con_IsVisible = hookCon_IsVisible;
	gHookEngfuncs.pfnGetGameDirectory = hookGetGameDirectory;
	gHookEngfuncs.pfnGetCvarPointer = hookGetCvarPointer;
	gHookEngfuncs.Key_LookupBinding = hookKey_LookupBinding;

	gHookEngfuncs.pfnGetLevelName = hookGetLevelName;
	gHookEngfuncs.pfnGetScreenFade = hookGetScreenFade;
	gHookEngfuncs.pfnSetScreenFade = hookSetScreenFade;
	gHookEngfuncs.VGui_GetPanel = hookVGui_GetPanel;

	gHookEngfuncs.VGui_ViewportPaintBackground = hookVGui_ViewportPaintBackground;
	gHookEngfuncs.COM_LoadFile = hookCOM_LoadFile;
	gHookEngfuncs.COM_ParseFile = hookCOM_ParseFile;
	gHookEngfuncs.COM_FreeFile = hookCOM_FreeFile;

	gHookEngfuncs.pTriAPI = gEngfuncs.pTriAPI;
	gHookEngfuncs.pDemoAPI = gEngfuncs.pDemoAPI;
	gHookEngfuncs.pEfxAPI = gEngfuncs.pEfxAPI;
	gHookEngfuncs.pEventAPI = gEngfuncs.pEventAPI;
	gHookEngfuncs.pNetAPI = gEngfuncs.pNetAPI;

	gHookEngfuncs.pVoiceTweak = gEngfuncs.pVoiceTweak;
	gHookEngfuncs.IsSpectateOnly = hookIsSpectateOnly;
	gHookEngfuncs.LoadMapSprite = hookLoadMapSprite;
	gHookEngfuncs.COM_AddAppDirectoryToSearchPath = hookCOM_AddAppDirectoryToSearchPath;
	gHookEngfuncs.COM_ExpandFilename = hookCOM_ExpandFilename;
	gHookEngfuncs.PlayerInfo_ValueForKey = hookPlayerInfo_ValueForKey;
	gHookEngfuncs.PlayerInfo_SetValueForKey = hookPlayerInfo_SetValueForKey;
	gHookEngfuncs.GetPlayerUniqueID = hookGetPlayerUniqueID;
	gHookEngfuncs.GetTrackerIDForPlayer = hookGetTrackerIDForPlayer;
	gHookEngfuncs.GetPlayerForTrackerID = hookGetPlayerForTrackerID;
	gHookEngfuncs.pfnServerCmdUnreliable = hookServerCmdUnreliable;

	// add whatever EfxAPI hooks are used
	memcpy(&gHookEfxAPI, gEngfuncs.pEfxAPI, sizeof(efx_api_t));
	gHookEngfuncs.pEfxAPI = &gHookEfxAPI;
	gHookEfxAPI.R_BeamFollow = hookR_BeamFollow;
}