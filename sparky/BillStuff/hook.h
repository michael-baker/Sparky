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


#ifndef HOOK_H
#define HOOK_H

#define HOOK_COMMAND_2(x, y) 		gEngfuncs.pfnAddCommand (x, __CmdFunc_##y );
#define DECLARE_COMMAND_2(y, x) 	static void __CmdFunc_##x( void )	\
								{ 									\
									##y.Cmd_##x( ); 			\
								}


class CBaseHook
{
	public:

	virtual bool Pre_CL_CreateMove(float frametime, struct usercmd_s *cmd, int active);
	virtual void Post_CL_CreateMove(float frametime, struct usercmd_s *cmd, int active);
	virtual bool Pre_HUD_Shutdown (void);
	virtual void Post_HUD_Shutdown (void);
	virtual bool Pre_HUD_CreateEntities (void);
	virtual void Post_HUD_CreateEntities (void);
	virtual bool Pre_HUD_DrawNormalTriangles (void);
	virtual void Post_HUD_DrawNormalTriangles (void);
	virtual bool Pre_HUD_DrawTransparentTriangles (void);
	virtual void Post_HUD_DrawTransparentTriangles (void);
	virtual bool Pre_IN_ActivateMouse (void);
	virtual void Post_IN_ActivateMouse (void);
	virtual bool Pre_IN_DeactivateMouse (void);
	virtual void Post_IN_DeactivateMouse (void);
	virtual bool Pre_IN_Accumulate (void);
	virtual void Post_IN_Accumulate (void);
	virtual bool Pre_IN_ClearStates (void);
	virtual void Post_IN_ClearStates (void);
	virtual bool Pre_HUD_Reset (void);
	virtual void Post_HUD_Reset (void);
	virtual bool Pre_HUD_VidInit (void);
	virtual void Post_HUD_VidInit (void);
	virtual bool Pre_HUD_Init (void);
	virtual void Post_HUD_Init (void);
	virtual bool Pre_IN_MouseEvent (int mstate);
	virtual void Post_IN_MouseEvent (int mstate);
	virtual bool Pre_HUD_Frame (double time);
	virtual void Post_HUD_Frame (double time);
	virtual bool Pre_HUD_PlayerMoveInit (struct playermove_s *ppmove);
	virtual void Post_HUD_PlayerMoveInit (struct playermove_s *ppmove);
	virtual bool Pre_V_CalcRefdef (struct ref_params_s *pparams);
	virtual void Post_V_CalcRefdef (struct ref_params_s *pparams);
	virtual bool Pre_HUD_PlayerMoveTexture (char *name);
	virtual void Post_HUD_PlayerMoveTexture (char *name);
	virtual bool Pre_HUD_GetUserEntity (int index);
	virtual void Post_HUD_GetUserEntity (int index);
	virtual bool Pre_Demo_ReadBuffer (int size, unsigned char * buffer);
	virtual void Post_Demo_ReadBuffer (int size, unsigned char * buffer);
	virtual bool Pre_HUD_StudioEvent (const struct mstudioevent_s *event, const struct cl_entity_s *entity);
	virtual void Post_HUD_StudioEvent (const struct mstudioevent_s *event, const struct cl_entity_s *entity);
	virtual bool Pre_HUD_TxferLocalOverrides (struct entity_state_s *state, const struct clientdata_s *client);
	virtual void Post_HUD_TxferLocalOverrides (struct entity_state_s *state, const struct clientdata_s *client);
	virtual bool Pre_HUD_ProcessPlayerState (struct entity_state_s *dst, const struct entity_state_s *src);
	virtual void Post_HUD_ProcessPlayerState (struct entity_state_s *dst, const struct entity_state_s *src);
	virtual bool Pre_HUD_TxferPredictionData (struct entity_state_s *ps, const struct entity_state_s *pps, struct clientdata_s *pcd, const struct clientdata_s *ppcd, struct weapon_data_s *wd, const struct weapon_data_s *pwd);
	virtual void Post_HUD_TxferPredictionData (struct entity_state_s *ps, const struct entity_state_s *pps, struct clientdata_s *pcd, const struct clientdata_s *ppcd, struct weapon_data_s *wd, const struct weapon_data_s *pwd);
	virtual bool Pre_HUD_TempEntUpdate (
		double frametime,   			// Simulation time
		double client_time, 			// Absolute time on client
		double cl_gravity,  			// True gravity on client
		TEMPENTITY **ppTempEntFree,   	// List of freed temporary ents
		TEMPENTITY **ppTempEntActive, 	// List 
		int		( *Callback_AddVisibleEntity )( cl_entity_t *pEntity ),
		void	( *Callback_TempEntPlaySound )( TEMPENTITY *pTemp, float damp ) );
	virtual void Post_HUD_TempEntUpdate (
		double frametime,   			// Simulation time
		double client_time, 			// Absolute time on client
		double cl_gravity,  			// True gravity on client
		TEMPENTITY **ppTempEntFree,   	// List of freed temporary ents
		TEMPENTITY **ppTempEntActive, 	// List 
		int		( *Callback_AddVisibleEntity )( cl_entity_t *pEntity ),
		void	( *Callback_TempEntPlaySound )( TEMPENTITY *pTemp, float damp ) );
	virtual bool Pre_HUD_PostRunCmd (struct local_state_s *from, struct local_state_s *to, struct usercmd_s *cmd, int runfuncs, double time, unsigned int random_seed);
	virtual void Post_HUD_PostRunCmd (struct local_state_s *from, struct local_state_s *to, struct usercmd_s *cmd, int runfuncs, double time, unsigned int random_seed);
	virtual bool Pre_HUD_PlayerMove (struct playermove_s *ppmove, int server);
	virtual void Post_HUD_PlayerMove (struct playermove_s *ppmove, int server);
	virtual bool Pre_HUD_Key_Event (int eventcode, int keynum, const char *pszCurrentBinding);
	virtual void Post_HUD_Key_Event (int eventcode, int keynum, const char *pszCurrentBinding);
	virtual bool Pre_HUD_AddEntity (int type, struct cl_entity_s *ent, const char *modelname);
	virtual void Post_HUD_AddEntity (int type, struct cl_entity_s *ent, const char *modelname);
	virtual bool Pre_HUD_Redraw (float flTime, int intermission);
	virtual void Post_HUD_Redraw (float flTime, int intermission);
	virtual bool Pre_HUD_UpdateClientData (client_data_t *cdata, float flTime);
	virtual void Post_HUD_UpdateClientData (client_data_t *cdata, float flTime);
	virtual bool Pre_HUD_ConnectionlessPacket (struct netadr_s *net_from, const char *args, char *response_buffer, int *response_buffer_size);
	virtual void Post_HUD_ConnectionlessPacket (struct netadr_s *net_from, const char *args, char *response_buffer, int *response_buffer_size);
	virtual bool Pre_HUD_GetHullBounds (int hullnumber, float *mins, float *maxs);
	virtual void Post_HUD_GetHullBounds (int hullnumber, float *mins, float *maxs);
	virtual bool Pre_HUD_GetStudioModelInterface (int version, r_studio_interface_t **ppinterface, engine_studio_api_t *pstudio);
	virtual void Post_HUD_GetStudioModelInterface (int version, r_studio_interface_t **ppinterface, engine_studio_api_t *pstudio);
	virtual bool Pre_HUD_DirectorEvent (unsigned char command, unsigned int firstObject, unsigned int secondObject, unsigned int flags);
	virtual void Post_HUD_DirectorEvent (unsigned char command, unsigned int firstObject, unsigned int secondObject, unsigned int flags);
	virtual bool Pre_HUD_VoiceStatus (int entindex, qboolean bTalking);
	virtual void Post_HUD_VoiceStatus (int entindex, qboolean bTalking);
	virtual bool Pre_Initialize (cl_enginefunc_t *pEnginefuncs, int iVersion);
	virtual void Post_Initialize (cl_enginefunc_t *pEnginefuncs, int iVersion);

	virtual bool Pre_Hook_TextMsg (const char *pszString);
	virtual void Post_Hook_TextMsg (const char *pszString);
	virtual bool Pre_Hook_SayText (const char *pszString);
	virtual void Post_Hook_SayText (const char *pszString);
	virtual bool Pre_Hook_HudText (const char *pszString);
	virtual void Post_Hook_HudText (const char *pszString);
	virtual bool Pre_Hook_DeathMsg (int killer, int victim, const char *pszWeapon);
	virtual void Post_Hook_DeathMsg (int killer, int victim, const char *pszWeapon);

	virtual bool Pre_Hook_ConsoleCommand (char *pszString);
	
	virtual bool Pre_Hook_ConsoleOutput (const char *pszString);
	virtual void Post_Hook_ConsoleOutput (const char *pszString);
	virtual bool Pre_Hook_AmmoX (int iIndex, int iCount);
	virtual void Post_Hook_AmmoX (int iIndex, int iCount);
	virtual bool Pre_Hook_Feign (int iIsFeigning);
	virtual void Post_Hook_Feign (int iIsFeigning);
	virtual bool Pre_Hook_Detpack (int iIsSettingDetpack);
	virtual void Post_Hook_Detpack (int iIsSettingDetpack);

	CBaseHook (void);
};

extern CBaseHook gMainHook;


#endif