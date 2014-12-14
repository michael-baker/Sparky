/*  
 * Half-Life client-side generic hook engine.
 * Copyright (c) Vasily Pupkin (clientbot@narod.ru) 2000-2001
 * Copyright (c) Tim Yunaev (tim@lviv.gu.net) 2000-2001
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
 *
 * NOTE:
 * GNU license doesn't cover Engine directory.
 * Content of Engine directory is copyrighted (c) 1999, 2000, by Valve LLC
 * and it is licensed under Valve's proprietary license (see original HL SDK).
 *
 * $Id: menu.h,v 1.2 2001/07/16 11:23:10 vasily Exp $
 */

#include "..\client.h"
#include "mempatcher.h"
#include <windows.h>

#include "hook.h"
#include "util.h"
#include "superhud.h"
#include "wonid.h"
#include "playerinfo.h"
#include "timers.h"
#include "logging.h"
#include "winamp.h"
#include "triggers.h"
#include "location.h"
#include "irc.h"
#include "soundpatch.h"
#include "fb.h" // added by FB
#include "stats.h" // added by FB, stats by L0ki
#include "commandpoint.h" // added by FB, commandpoint scoreboard by FryGuy
#include "rulesmanager.h" // added by FB, rulesmanager from FryGuy
#include "laserstatus.h" // added by FB, blaserstatus by ruiner
#include "flagstatus.h" // added by FB, bflagstatus by ruiner
#include "speedos.h" // added by FB
#include "kselookup.h" // added by FB, bkselookup by ruiner
#include "autorecord.h" // added by FB, autorecord by Zoltarc
#include "menu.h" // added by FB (v1.4), menu by equinox and 8ball
#include "RTFM.h" // added by Mulchman (v1.4)
#include "retry.h" // added by Mulchman (v1.4)
#include "spammeter.h" // added by Ruiner (v1.41)
#include "glhook.h" // added by Ruiner (v1.41)
#include "help.h" // added by Fr33 (v1.42)
#include "logo.h" // added by Ruiner (v1.42)
#include "addons.h" // added by Mulchman (v1.42)

#include "..\pm_shared\pm_shared.h"

/*Global*/
CBaseHook gMainHook;

bool CBaseHook::Pre_CL_CreateMove(float frametime, struct usercmd_s *cmd, int active)
{
	return true;
}

void CBaseHook::Post_CL_CreateMove(float frametime, struct usercmd_s *cmd, int active)
{
	gAddons.Post_CL_CreateMove(frametime, cmd, active);
}

bool CBaseHook::Pre_HUD_Shutdown (void)
{
	return true;
}

void CBaseHook::Post_HUD_Shutdown (void)
{
}

bool CBaseHook::Pre_HUD_CreateEntities (void)
{
	return true;
}

void CBaseHook::Post_HUD_CreateEntities (void)
{
}

bool CBaseHook::Pre_HUD_DrawNormalTriangles (void)
{
	return true;
}

void CBaseHook::Post_HUD_DrawNormalTriangles (void)
{
}

bool CBaseHook::Pre_HUD_DrawTransparentTriangles (void)
{
	return true;
}

void CBaseHook::Post_HUD_DrawTransparentTriangles (void)
{
}

bool CBaseHook::Pre_IN_ActivateMouse (void)
{
	return true;
}

void CBaseHook::Post_IN_ActivateMouse (void)
{
}

bool CBaseHook::Pre_IN_DeactivateMouse (void)
{
	return true;
}

void CBaseHook::Post_IN_DeactivateMouse (void)
{
}

bool CBaseHook::Pre_IN_Accumulate (void)
{
	return true;
}

void CBaseHook::Post_IN_Accumulate (void)
{
}

bool CBaseHook::Pre_IN_ClearStates (void)
{
	return true;
}

void CBaseHook::Post_IN_ClearStates (void)
{
}

bool CBaseHook::Pre_HUD_Reset (void)
{
	return true;
}

void CBaseHook::Post_HUD_Reset (void)
{
}

bool CBaseHook::Pre_HUD_VidInit (void)
{
	return true;
}

void CBaseHook::Post_HUD_VidInit (void)
{
}

bool CBaseHook::Pre_HUD_Init (void)
{
	if(!gSuperHUD.Pre_HUD_Init())
		return false;

	if(!gWonid.Pre_HUD_Init())
		return false;

	if(!gTimers.Pre_HUD_Init())
		return false;

	if(!gLogging.Pre_HUD_Init())
		return false;

	if(!gWinamp.Pre_HUD_Init())
		return false;

	if(!gTriggers.Pre_HUD_Init())
		return false;

	if(!gLocation.Pre_HUD_Init())
		return false;

	if(!gIRC.Pre_HUD_Init())
		return false;

	if(!gSoundPatch.Pre_HUD_Init())
		return false;

	if(!gFB.Pre_HUD_Init())
		return false;

	if(!gStats.Pre_HUD_Init())
		return false;

	if(!gCommandPoint.Pre_HUD_Init())
		return false;

	if(!gRulesManager.Pre_HUD_Init())
		return false;

	if(!gLaserStatus.Pre_HUD_Init())
		return false;

	if(!gFlagStatus.Pre_HUD_Init())
		return false;

	if(!gSpeedos.Pre_HUD_Init())
		return false;

	if(!gAutorecord.Pre_HUD_Init())
		return false;

	if(!gSparkyMenu.Pre_HUD_Init())
		return false;

	if(!gRetry.Pre_HUD_Init())
		return false;

	if(!gRTFM.Pre_HUD_Init())
		return false;

	if(!gSpamMeter.Pre_HUD_Init())
		return false;

	if(!gLogo.Pre_HUD_Init())
		return false;

	if(!gHelp.Pre_HUD_Init())
		return false;

	if(!gGLHook.Pre_HUD_Init())
		return false;

	return true;
}

void CBaseHook::Post_HUD_Init (void)
{
	sprintf(gLogo.strVersion,"1.44");

	char buf[1024];
	sprintf(buf, "[sparky] v%s loaded\n", gLogo.strVersion);
	gEngfuncs.pfnConsolePrint(buf);

	gSuperHUD.Post_HUD_Init();
	gTimers.Post_HUD_Init();
	gLogging.Post_HUD_Init();
	gWinamp.Post_HUD_Init();
	gTriggers.Post_HUD_Init();
	gLocation.Post_HUD_Init();
	gIRC.Post_HUD_Init();
	gSoundPatch.Post_HUD_Init();
	gStats.Post_HUD_Init();
	gKSELookup.Post_HUD_Init();
	gSpeedos.Post_HUD_Init();
	gWonid.Post_HUD_Init();
	gSpamMeter.Post_HUD_Init();
	gAddons.Post_HUD_Init();
	gHelp.Post_HUD_Init();
}

bool CBaseHook::Pre_IN_MouseEvent (int mstate)
{
	return true;
}

void CBaseHook::Post_IN_MouseEvent (int mstate)
{
}

bool CBaseHook::Pre_HUD_Frame (double time)
{
	if(!gTimers.Pre_HUD_Frame(time))
		return false;

	if(!gLogging.Pre_HUD_Frame(time))
		return false;

	if(!gTriggers.Pre_HUD_Frame(time))
		return false;

	if(!gLocation.Pre_HUD_Frame(time))
		return false;

	if(!gStats.Pre_HUD_Frame(time))
		return false;

	if(!gRulesManager.Pre_HUD_Frame(time))
		return false;

	return true;
}

void CBaseHook::Post_HUD_Frame (double time)
{
	gWonid.Post_HUD_Frame(time);
	gTimers.Post_HUD_Frame(time);
	gLogging.Post_HUD_Frame(time);
	gTriggers.Post_HUD_Frame(time);
	gLocation.Post_HUD_Frame(time);
	gFB.Post_HUD_Frame(time);
	gStats.Post_HUD_Frame(time);
	gCommandPoint.Post_HUD_Frame(time);
	gRulesManager.Post_HUD_Frame(time);
	gLaserStatus.Post_HUD_Frame(time);
	gFlagStatus.Post_HUD_Frame(time);
	gAddons.Post_HUD_Frame(time);
}

bool CBaseHook::Pre_HUD_PlayerMoveInit (struct playermove_s *ppmove)
{
	if(!gSuperHUD.Pre_HUD_PlayerMoveInit(ppmove))
		return false;

	return true;
}

void CBaseHook::Post_HUD_PlayerMoveInit (struct playermove_s *ppmove)
{
	gSuperHUD.Post_HUD_PlayerMoveInit(ppmove);	
}

bool CBaseHook::Pre_V_CalcRefdef (struct ref_params_s *pparams)
{
	return true;
}

void CBaseHook::Post_V_CalcRefdef (struct ref_params_s *pparams)
{
}

bool CBaseHook::Pre_HUD_PlayerMoveTexture (char *name)
{
	return true;
}

void CBaseHook::Post_HUD_PlayerMoveTexture (char *name)
{
}

bool CBaseHook::Pre_HUD_GetUserEntity (int index)
{
	return true;
}

void CBaseHook::Post_HUD_GetUserEntity (int index)
{
}

bool CBaseHook::Pre_Demo_ReadBuffer (int size, unsigned char * buffer)
{
	return true;
}

void CBaseHook::Post_Demo_ReadBuffer (int size, unsigned char * buffer)
{
}

bool CBaseHook::Pre_HUD_StudioEvent (const struct mstudioevent_s *event, const struct cl_entity_s *entity)
{
	return true;
}

void CBaseHook::Post_HUD_StudioEvent (const struct mstudioevent_s *event, const struct cl_entity_s *entity)
{
}

bool CBaseHook::Pre_HUD_TxferLocalOverrides (struct entity_state_s *state, const struct clientdata_s *client)
{
	return true;
}

void CBaseHook::Post_HUD_TxferLocalOverrides (struct entity_state_s *state, const struct clientdata_s *client)
{
}

bool CBaseHook::Pre_HUD_ProcessPlayerState (struct entity_state_s *dst, const struct entity_state_s *src)
{
	return true;
}

void CBaseHook::Post_HUD_ProcessPlayerState (struct entity_state_s *dst, const struct entity_state_s *src)
{
}

bool CBaseHook::Pre_HUD_TxferPredictionData (struct entity_state_s *ps, const struct entity_state_s *pps, struct clientdata_s *pcd, const struct clientdata_s *ppcd, struct weapon_data_s *wd, const struct weapon_data_s *pwd)
{
	return true;
}

void CBaseHook::Post_HUD_TxferPredictionData (struct entity_state_s *ps, const struct entity_state_s *pps, struct clientdata_s *pcd, const struct clientdata_s *ppcd, struct weapon_data_s *wd, const struct weapon_data_s *pwd)
{
}

bool CBaseHook::Pre_HUD_TempEntUpdate (
		double frametime,   			// Simulation time
		double client_time, 			// Absolute time on client
		double cl_gravity,  			// True gravity on client
		TEMPENTITY **ppTempEntFree,   	// List of freed temporary ents
		TEMPENTITY **ppTempEntActive, 	// List 
		int		( *Callback_AddVisibleEntity )( cl_entity_t *pEntity ),
		void	( *Callback_TempEntPlaySound )( TEMPENTITY *pTemp, float damp ) )
{
	return true;
}

void CBaseHook::Post_HUD_TempEntUpdate (
		double frametime,   			// Simulation time
		double client_time, 			// Absolute time on client
		double cl_gravity,  			// True gravity on client
		TEMPENTITY **ppTempEntFree,   	// List of freed temporary ents
		TEMPENTITY **ppTempEntActive, 	// List 
		int		( *Callback_AddVisibleEntity )( cl_entity_t *pEntity ),
		void	( *Callback_TempEntPlaySound )( TEMPENTITY *pTemp, float damp ) )
{
}

bool CBaseHook::Pre_HUD_PostRunCmd (struct local_state_s *from, struct local_state_s *to, struct usercmd_s *cmd, int runfuncs, double time, unsigned int random_seed)
{
	return true;
}

void CBaseHook::Post_HUD_PostRunCmd (struct local_state_s *from, struct local_state_s *to, struct usercmd_s *cmd, int runfuncs, double time, unsigned int random_seed)
{
}

bool CBaseHook::Pre_HUD_PlayerMove (struct playermove_s *ppmove, int server)
{
	if(!gPlayerinfo.Pre_HUD_PlayerMove(ppmove, server))
		return false;

	if(!gSuperHUD.Pre_HUD_PlayerMove(ppmove, server))
		return false;

	return true;
}

void CBaseHook::Post_HUD_PlayerMove (struct playermove_s *ppmove, int server)
{	
	gPlayerinfo.Post_HUD_PlayerMove(ppmove, server);
	gSuperHUD.Post_HUD_PlayerMove(ppmove, server);
	gAddons.Post_HUD_PlayerMove(ppmove, server);
}

bool CBaseHook::Pre_HUD_Key_Event (int eventcode, int keynum, const char *pszCurrentBinding)
{
	if(!gSparkyMenu.Pre_HUD_Key_Event(eventcode, keynum, pszCurrentBinding))
		return false;

	if(!gAddons.Pre_HUD_Key_Event(eventcode, keynum, pszCurrentBinding))
		return false;

	return true;
}

void CBaseHook::Post_HUD_Key_Event (int eventcode, int keynum, const char *pszCurrentBinding)
{
	gWonid.Post_HUD_Key_Event(eventcode, keynum);
	gAddons.Post_HUD_Key_Event(eventcode, keynum, pszCurrentBinding);
}

bool CBaseHook::Pre_HUD_AddEntity (int type, struct cl_entity_s *ent, const char *modelname)
{
	if(!gSuperHUD.Pre_HUD_AddEntity(type, ent, modelname))
		return false;

	if(!gSpamMeter.Pre_HUD_AddEntity(type, ent, modelname))
		return false;

	if(!gGLHook.Pre_HUD_AddEntity(type, ent, modelname))
		return false;

	return true;
}

void CBaseHook::Post_HUD_AddEntity (int type, struct cl_entity_s *ent, const char *modelname)
{
	gSuperHUD.Post_HUD_AddEntity(type, ent, modelname);
	gFB.Post_HUD_AddEntity(type, ent, modelname);
}

bool CBaseHook::Pre_HUD_Redraw (float flTime, int intermission)
{
	if(!gSuperHUD.Pre_HUD_Redraw(flTime, intermission))
		return false;

	if(!gIRC.Pre_HUD_Redraw(flTime, intermission))
		return false;
	
	if(!gSoundPatch.Pre_HUD_Redraw(flTime, intermission))
		return false;

	if(!gStats.Pre_HUD_Redraw(flTime, intermission))
		return false;

	if(!gWinamp.Pre_HUD_Redraw(flTime, intermission))
		return false;

	if(!gFB.Pre_HUD_Redraw(flTime, intermission))
		return false;

	if(!gRTFM.Pre_HUD_Redraw(flTime, intermission))
		return false;

	if(!gAddons.Pre_HUD_Redraw(flTime, intermission))
		return false;

	if(!gGLHook.Pre_HUD_Redraw(flTime, intermission))
		return false;

	return true;
}

void CBaseHook::Post_HUD_Redraw (float flTime, int intermission)
{
	gSuperHUD.Post_HUD_Redraw(flTime, intermission);
	gIRC.Post_HUD_Redraw(flTime, intermission);
	gSoundPatch.Post_HUD_Redraw(flTime, intermission);
	gStats.Post_HUD_Redraw(flTime, intermission);
	gWinamp.Post_HUD_Redraw(flTime, intermission);
	gFB.Post_HUD_Redraw(flTime, intermission);
	gCommandPoint.Post_HUD_Redraw(flTime, intermission);
	gLaserStatus.Post_HUD_Redraw(flTime, intermission);
	gFlagStatus.Post_HUD_Redraw(flTime, intermission);
	gSpeedos.Post_HUD_Redraw(flTime, intermission);
	gWonid.Post_HUD_Redraw(flTime, intermission);
	gSparkyMenu.Post_HUD_Redraw(flTime, intermission);
	gRTFM.Post_HUD_Redraw(flTime, intermission);
	gSpamMeter.Post_HUD_Redraw(flTime, intermission);
	gAddons.Post_HUD_Redraw(flTime, intermission);

	// below is always last
	
	gLogo.Post_HUD_Redraw(flTime, intermission);	
}

bool CBaseHook::Pre_HUD_UpdateClientData (client_data_t *cdata, float flTime)
{
	return true;
}

void CBaseHook::Post_HUD_UpdateClientData (client_data_t *cdata, float flTime)
{
}

bool CBaseHook::Pre_HUD_ConnectionlessPacket (struct netadr_s *net_from, const char *args, char *response_buffer, int *response_buffer_size)
{
	return true;
}

void CBaseHook::Post_HUD_ConnectionlessPacket (struct netadr_s *net_from, const char *args, char *response_buffer, int *response_buffer_size)
{
}

bool CBaseHook::Pre_HUD_GetHullBounds (int hullnumber, float *mins, float *maxs)
{
	return true;
}

void CBaseHook::Post_HUD_GetHullBounds (int hullnumber, float *mins, float *maxs)
{
}

bool CBaseHook::Pre_HUD_GetStudioModelInterface (int version, r_studio_interface_t **ppinterface, engine_studio_api_t *pstudio)
{
	return true;
}

void CBaseHook::Post_HUD_GetStudioModelInterface (int version, r_studio_interface_t **ppinterface, engine_studio_api_t *pstudio)
{
}

bool CBaseHook::Pre_HUD_DirectorEvent (unsigned char command, unsigned int firstObject, unsigned int secondObject, unsigned int flags)
{
	return true;
}

void CBaseHook::Post_HUD_DirectorEvent (unsigned char command, unsigned int firstObject, unsigned int secondObject, unsigned int flags)
{
}

bool CBaseHook::Pre_HUD_VoiceStatus (int entindex, qboolean bTalking)
{
	if(!gWinamp.Pre_HUD_VoiceStatus(entindex, bTalking))
		return false;

	return true;
}

void CBaseHook::Post_HUD_VoiceStatus (int entindex, qboolean bTalking)
{
	gWinamp.Post_HUD_VoiceStatus(entindex, bTalking);
	gAddons.Post_HUD_VoiceStatus(entindex, bTalking);
}

bool CBaseHook::Pre_Initialize (cl_enginefunc_t *pEnginefuncs, int iVersion)
{
	return true;
}

void CBaseHook::Post_Initialize (cl_enginefunc_t *pEnginefuncs, int iVersion)
{
}

bool CBaseHook::Pre_Hook_TextMsg (const char *pszString)
{
	if(!gWonid.Pre_Hook_TextMsg(pszString))
		return false;

	/*if(!gTimers.Pre_Hook_TextMsg(pszString))
		return false;*/

	if(!gLogging.Pre_Hook_TextMsg(pszString))
		return false;

	if(!gTriggers.Pre_Hook_TextMsg(pszString))
		return false;

	if(!gIRC.Pre_Hook_TextMsg(pszString))
		return false;

	if(!gFB.Pre_Hook_TextMsg(pszString))
		return false;

	if(!gLocation.Pre_Hook_TextMsg(pszString))
		return false;

	return true;
}

void CBaseHook::Post_Hook_TextMsg (const char *pszString)
{
	/*gTimers.Post_Hook_TextMsg(pszString);*/
	gLogging.Post_Hook_TextMsg(pszString);
	gTriggers.Post_Hook_TextMsg(pszString);
	gIRC.Post_Hook_TextMsg(pszString);
	gFB.Post_Hook_TextMsg(pszString);
	gCommandPoint.Post_Hook_TextMsg(pszString);
	gRulesManager.Post_Hook_TextMsg(pszString);
	gAutorecord.Post_Hook_TextMsg(pszString);
	gLocation.Post_Hook_TextMsg(pszString);
	gAddons.Post_Hook_TextMsg(pszString);
}

bool CBaseHook::Pre_Hook_SayText (const char *pszString)
{
	if(!gLogging.Pre_Hook_SayText(pszString))
		return false;

	if(!gWinamp.Pre_Hook_SayText(pszString))
		return false;

	if(!gTriggers.Pre_Hook_SayText(pszString))
		return false;

	if(!gIRC.Pre_Hook_SayText(pszString))
		return false;

	return true;
}

void CBaseHook::Post_Hook_SayText (const char *pszString)
{
	gLogging.Post_Hook_SayText(pszString);
	gWinamp.Post_Hook_SayText(pszString);
	gTriggers.Post_Hook_SayText(pszString);
	gIRC.Post_Hook_SayText(pszString);
	gAddons.Post_Hook_SayText(pszString);
}

//added by FB
bool CBaseHook::Pre_Hook_HudText (const char *pszString)
{
	return true;
}

//added by FB
void CBaseHook::Post_Hook_HudText (const char *pszString)
{
	gLogging.Post_Hook_HudText(pszString);
	gFB.Post_Hook_HudText(pszString);
	gTriggers.Post_Hook_HudText(pszString);
	gCommandPoint.Post_Hook_HudText(pszString);
	gLaserStatus.Post_Hook_HudText(pszString);
	gFlagStatus.Post_Hook_HudText(pszString);
	gAddons.Post_Hook_HudText(pszString);
}

bool CBaseHook::Pre_Hook_DeathMsg (int killer, int victim, const char *pszWeapon)
{
	if(!gLogging.Pre_Hook_DeathMsg(killer, victim, pszWeapon))
		return false;

	if(!gTriggers.Pre_Hook_DeathMsg(killer, victim, pszWeapon))
		return false;

	if(!gIRC.Pre_Hook_DeathMsg(killer, victim, pszWeapon))
		return false;

	return true;
}

void CBaseHook::Post_Hook_DeathMsg (int killer, int victim, const char *pszWeapon)
{
	gLogging.Post_Hook_DeathMsg(killer, victim, pszWeapon);
	gTriggers.Post_Hook_DeathMsg(killer, victim, pszWeapon);
	gIRC.Post_Hook_DeathMsg(killer, victim, pszWeapon);
	gAddons.Post_Hook_DeathMsg(killer, victim, pszWeapon);
}

bool CBaseHook::Pre_Hook_ConsoleCommand (char *pszString)
{

	if(!gLocation.Pre_Hook_ConsoleCommand(pszString))
		return false;

	if(!gIRC.Pre_Hook_ConsoleCommand(pszString))
		return false;

	if(!gFB.Pre_Hook_ConsoleCommand(pszString))
		return false;
	
	if(!gRulesManager.Pre_Hook_ConsoleCommand(pszString))
		return false;

	if(!gFlagStatus.Pre_Hook_ConsoleCommand(pszString))
		return false;

	if(!gTriggers.Pre_Hook_ConsoleCommand(pszString))
		return false;

	if(!gAutorecord.Pre_Hook_ConsoleCommand(pszString))
		return false;

	if(!gAddons.Pre_Hook_ConsoleCommand(pszString))
		return false;

	return true;
}

bool CBaseHook::Pre_Hook_ConsoleOutput (const char *pszString)
{
	if(!gRetry.Pre_Hook_ConsoleOutput(pszString))
		return false;

	return true;
}

void CBaseHook::Post_Hook_ConsoleOutput (const char *pszString)
{
	gRetry.Post_Hook_ConsoleOutput(pszString);
	gAddons.Post_Hook_ConsoleOutput(pszString);
}

CBaseHook::CBaseHook (void)
{
}

bool CBaseHook::Pre_Hook_AmmoX (int iIndex, int iCount)
{
	return true;
}

void CBaseHook::Post_Hook_AmmoX (int iIndex, int iCount)
{
	gAddons.Post_Hook_AmmoX(iIndex, iCount);
}

bool CBaseHook::Pre_Hook_Feign (int iIsFeigning)
{
	return true;
}

void CBaseHook::Post_Hook_Feign (int iIsFeigning)
{
	gTriggers.Post_Hook_Feign(iIsFeigning);
}

bool CBaseHook::Pre_Hook_Detpack (int iIsSettingDetpack)
{
	return true;
}

void CBaseHook::Post_Hook_Detpack (int iIsSettingDetpack)
{
	gTriggers.Post_Hook_Detpack(iIsSettingDetpack);
}


