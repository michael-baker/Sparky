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


#include <memory.h>
#include "Client.h"
#include "BillStuff\hook.h"
extern "C"
{
#include "pm_shared\pm_shared.h"
}


DllFuncs_t gDllFuncs;
cl_enginefunc_t gHookEngfuncs;
cl_enginefunc_t gEngfuncs;
efx_api_t gHookEfxAPI;


void CAM_Think( void )
{
	gDllFuncs.CAM_Think( );
}


void CL_CameraOffset( float *ofs )
{
	gDllFuncs.CL_CameraOffset( ofs );
}


void CL_CreateMove( float frametime, struct usercmd_s *cmd, int active )
{
	// Run Pre hook, abort if necessary
	if(!gMainHook.Pre_CL_CreateMove(frametime, cmd, active))
		return;

	gDllFuncs.CL_CreateMove( frametime, cmd, active );

	// Run Post hook
	gMainHook.Post_CL_CreateMove(frametime, cmd, active);
}


int CL_IsThirdPerson( void )
{
	int retval = 0;

	retval = gDllFuncs.CL_IsThirdPerson( );
	return( retval );
}


void Demo_ReadBuffer( int size, unsigned char *buffer )
{
	// Run Pre hook, abort if necessary
	if(!gMainHook.Pre_Demo_ReadBuffer(size, buffer))
		return;

	gDllFuncs.Demo_ReadBuffer( size, buffer );

	// Run Post hook
	gMainHook.Post_Demo_ReadBuffer(size, buffer);
}


int HUD_AddEntity( int type, struct cl_entity_s *ent, const char *modelname )
{
	int retval = 0;

	// Run Pre hook, abort if necessary
	if(!gMainHook.Pre_HUD_AddEntity(type, ent, modelname))
		return NULL;

	retval = gDllFuncs.HUD_AddEntity( type, ent, modelname );

	// Run Post hook
	gMainHook.Post_HUD_AddEntity(type, ent, modelname);

	return( retval );
}


int HUD_ConnectionlessPacket( struct netadr_s *net_from, const char *args, char *response_buffer, int *response_buffer_size )
{
	int retval = 0;

	// Run Pre hook, abort if necessary
	if(!gMainHook.Pre_HUD_ConnectionlessPacket(net_from, args, response_buffer, response_buffer_size))
		return NULL;

	retval = gDllFuncs.HUD_ConnectionlessPacket( net_from, args, response_buffer, response_buffer_size );

	// Run Post hook
	gMainHook.Post_HUD_ConnectionlessPacket(net_from, args, response_buffer, response_buffer_size);

	return( retval );
}


void HUD_CreateEntities( void )
{
	// Run Pre hook, abort if necessary
	if(!gMainHook.Pre_HUD_CreateEntities())
		return;

	gDllFuncs.HUD_CreateEntities( );

	// Run Post hook
	gMainHook.Post_HUD_CreateEntities();

}


void HUD_DrawNormalTriangles( void )
{
	// Run Pre hook, abort if necessary
	if(!gMainHook.Pre_HUD_DrawNormalTriangles())
		return;

	gDllFuncs.HUD_DrawNormalTriangles( );

	// Run Post hook
	gMainHook.Post_HUD_DrawNormalTriangles();
}


void HUD_DrawTransparentTriangles( void )
{
	// Run Pre hook, abort if necessary
	if(!gMainHook.Pre_HUD_DrawTransparentTriangles())
		return;

	gDllFuncs.HUD_DrawTransparentTriangles( );

	// Run Post hook
	gMainHook.Post_HUD_DrawTransparentTriangles();
}


void HUD_Frame( double time )
{
	// Run Pre hook, abort if necessary
	if(!gMainHook.Pre_HUD_Frame(time))
		return;

	gDllFuncs.HUD_Frame( time );

	// Run Post hook
	gMainHook.Post_HUD_Frame(time);
}


int HUD_GetHullBounds( int hullnumber, float *mins, float *maxs )
{
	// Run Pre hook, abort if necessary
	if(!gMainHook.Pre_HUD_GetHullBounds(hullnumber, mins, maxs))
		return NULL;

	int retval = 0;

	retval = gDllFuncs.HUD_GetHullBounds( hullnumber, mins, maxs );

	// Run Post hook
	gMainHook.Post_HUD_GetHullBounds(hullnumber, mins, maxs);

	return( retval );
}


struct cl_entity_s *HUD_GetUserEntity( int index )
{
	// Run Pre hook, abort if necessary
	if(!gMainHook.Pre_HUD_GetUserEntity(index))
		return NULL;

	struct cl_entity_s *retval = NULL;

	retval = gDllFuncs.HUD_GetUserEntity( index );

	// Run Post hook
	gMainHook.Post_HUD_GetUserEntity(index);

	return( retval );
}


int HUD_Init( void )
{
	// Run Pre hook, abort if necessary
	if(!gMainHook.Pre_HUD_Init())
		return NULL;

	int retval = 0;

	retval = gDllFuncs.HUD_Init( );

	// Run Post hook
	gMainHook.Post_HUD_Init();
	
	return( retval );
}


int HUD_Key_Event( int eventcode, int keynum, const char *pszCurrentBinding )
{
	// Run Pre hook, abort if necessary
	if(!gMainHook.Pre_HUD_Key_Event(eventcode, keynum, pszCurrentBinding))
		return NULL;

	int retval = 0;
 
	retval = gDllFuncs.HUD_Key_Event( eventcode, keynum, pszCurrentBinding );

	// Run Post hook
	gMainHook.Post_HUD_Key_Event(eventcode, keynum, pszCurrentBinding);

	return( retval );
}


void HUD_PlayerMove( struct playermove_s *ppmove, int server )
{
	// Run Pre hook, abort if necessary
	if(!gMainHook.Pre_HUD_PlayerMove(ppmove, server))
		return;
	
	gDllFuncs.HUD_PlayerMove( ppmove, server );

	// Run Post hook
	gMainHook.Post_HUD_PlayerMove(ppmove, server);
}


void HUD_PlayerMoveInit( struct playermove_s *ppmove )
{
	// Run Pre hook, abort if necessary
	if(!gMainHook.Pre_HUD_PlayerMoveInit(ppmove))
		return;

	gDllFuncs.HUD_PlayerMoveInit( ppmove );

	// Run Post hook
	gMainHook.Post_HUD_PlayerMoveInit(ppmove);
}


char HUD_PlayerMoveTexture( char *name )
{
	//*** HOOKS HERE REMOVED - FUNCTION IS RUN INTERNALLY IN ORDER TO IMPLEMENT BHOP FIX ***
	// Run Pre hook, abort if necessary
	/*
	if(!gMainHook.Pre_HUD_PlayerMoveTexture(name))
		return NULL;

	char retval = NULL;

	retval = gDllFuncs.HUD_PlayerMoveTexture( name );

	// Run Post hook
	gMainHook.Post_HUD_PlayerMoveTexture(name);

	return( retval );
	*/
	return PM_FindTextureType( name );
}


void HUD_PostRunCmd( struct local_state_s *from, struct local_state_s *to, struct usercmd_s *cmd, int runfuncs, double time, unsigned int random_seed )
{
	// Run Pre hook, abort if necessary
	if(!gMainHook.Pre_HUD_PostRunCmd(from, to, cmd, runfuncs, time, random_seed))
		return;

	gDllFuncs.HUD_PostRunCmd( from, to, cmd, runfuncs, time, random_seed );

	// Run Post hook
	gMainHook.Post_HUD_PostRunCmd(from, to, cmd, runfuncs, time, random_seed);
}


void HUD_ProcessPlayerState( struct entity_state_s *dst, const struct entity_state_s *src )
{
	// Run Pre hook, abort if necessary
	if(!gMainHook.Pre_HUD_ProcessPlayerState(dst, src))
		return;

	gDllFuncs.HUD_ProcessPlayerState( dst, src );

	// Run Post hook
	gMainHook.Post_HUD_ProcessPlayerState(dst, src);
}


int HUD_Redraw( float flTime, int intermission )
{
	// Run Pre hook, abort if necessary
	if(!gMainHook.Pre_HUD_Redraw(flTime, intermission))
		return NULL;

	int retval = 0;

	retval = gDllFuncs.HUD_Redraw( flTime, intermission );

	// Run Post hook
	gMainHook.Post_HUD_Redraw(flTime, intermission);

	return( retval );
}


int HUD_Reset( void )
{
	// Run Pre hook, abort if necessary
	if(!gMainHook.Pre_HUD_Reset())
		return NULL;

	int retval = 0;

	retval = gDllFuncs.HUD_Reset( );

	// Run Post hook
	gMainHook.Post_HUD_Reset();

	return( retval );
}


void HUD_Shutdown( void )
{
	// Run Pre hook, abort if necessary
	if(!gMainHook.Pre_HUD_Shutdown())
		return;

	gDllFuncs.HUD_Shutdown( );

	// Run Post hook
	gMainHook.Post_HUD_Shutdown();
}


void HUD_StudioEvent( const struct mstudioevent_s *event, const struct cl_entity_s *entity )
{
	// Run Pre hook, abort if necessary
	if(!gMainHook.Pre_HUD_StudioEvent(event, entity))
		return;

	gDllFuncs.HUD_StudioEvent( event, entity );

	// Run Post hook
	gMainHook.Post_HUD_StudioEvent(event, entity);
}


void HUD_TempEntUpdate(	double frametime, double client_time, double cl_gravity, TEMPENTITY **ppTempEntFree, TEMPENTITY **ppTempEntActive, int( *Callback_AddVisibleEntity )( cl_entity_t *pEntity ), void( *Callback_TempEntPlaySound )( TEMPENTITY *pTemp, float damp ) )
{
	// Run Pre hook, abort if necessary
	if(!gMainHook.Pre_HUD_TempEntUpdate(frametime, client_time, cl_gravity, ppTempEntFree, ppTempEntActive, Callback_AddVisibleEntity, Callback_TempEntPlaySound))
		return;

	gDllFuncs.HUD_TempEntUpdate( frametime, client_time, cl_gravity, ppTempEntFree, ppTempEntActive, Callback_AddVisibleEntity, Callback_TempEntPlaySound );

	// Run Post hook
	gMainHook.Post_HUD_TempEntUpdate(frametime, client_time, cl_gravity, ppTempEntFree, ppTempEntActive, Callback_AddVisibleEntity, Callback_TempEntPlaySound);
}


void HUD_TxferLocalOverrides( struct entity_state_s *state, const struct clientdata_s *client )
{
	// Run Pre hook, abort if necessary
	if(!gMainHook.Pre_HUD_TxferLocalOverrides(state, client))
		return;

	gDllFuncs.HUD_TxferLocalOverrides( state, client );

	// Run Post hook
	gMainHook.Post_HUD_TxferLocalOverrides(state, client);
}


void HUD_TxferPredictionData( struct entity_state_s *ps, const struct entity_state_s *pps, struct clientdata_s *pcd, const struct clientdata_s *ppcd, struct weapon_data_s *wd, const struct weapon_data_s *pwd )
{
	// Run Pre hook, abort if necessary
	if(!gMainHook.Pre_HUD_TxferPredictionData(ps, pps, pcd, ppcd, wd, pwd))
		return;

	gDllFuncs.HUD_TxferPredictionData( ps, pps, pcd, ppcd, wd, pwd );

	// Run Post hook
	gMainHook.Post_HUD_TxferPredictionData(ps, pps, pcd, ppcd, wd, pwd);
}


int HUD_UpdateClientData( client_data_t *cdata, float flTime )
{
	int retval = 0;

	// Run Pre hook, abort if necessary
	if(!gMainHook.Pre_HUD_UpdateClientData(cdata, flTime))
		return NULL;

	retval = gDllFuncs.HUD_UpdateClientData( cdata, flTime );

	// Run Post hook
	gMainHook.Post_HUD_UpdateClientData(cdata, flTime);

	return( retval );
}


int HUD_VidInit( void )
{
	int retval = 0;

	// Run Pre hook, abort if necessary
	if(!gMainHook.Pre_HUD_VidInit())
		return NULL;

	retval = gDllFuncs.HUD_VidInit( );

	// Run Post hook
	gMainHook.Post_HUD_VidInit();

	return( retval );
}


int HUD_VoiceStatus( int entindex, qboolean bTalking )
{
	// Run Pre hook, abort if necessary
	if(!gMainHook.Pre_HUD_VoiceStatus(entindex, bTalking))
		return NULL;

	int retval = 0;

	retval = gDllFuncs.HUD_VoiceStatus( entindex, bTalking );

	// Run Post hook
	gMainHook.Post_HUD_VoiceStatus(entindex, bTalking);

	return( retval );
}

void IN_Accumulate( void )
{
	// Run Pre hook, abort if necessary
	if(!gMainHook.Pre_IN_Accumulate())
		return;

	gDllFuncs.IN_Accumulate( );

	// Run Post hook
	gMainHook.Post_IN_Accumulate();
}


void IN_ActivateMouse( void )
{
	// Run Pre hook, abort if necessary
	if(!gMainHook.Pre_IN_ActivateMouse())
		return;

	gDllFuncs.IN_ActivateMouse( );

	// Run Post hook
	gMainHook.Post_IN_ActivateMouse();
}


void IN_ClearStates( void )
{
	// Run Pre hook, abort if necessary
	if(!gMainHook.Pre_IN_ClearStates())
		return;

	gDllFuncs.IN_ClearStates( );

	// Run Post hook
	gMainHook.Post_IN_ClearStates();
}


void IN_DeactivateMouse( void )
{
	// Run Pre hook, abort if necessary
	if(!gMainHook.Pre_IN_DeactivateMouse())
		return;

	gDllFuncs.IN_DeactivateMouse( );

	// Run Post hook
	gMainHook.Post_IN_DeactivateMouse();
}


void IN_MouseEvent( int mstate )
{
	// Run Pre hook, abort if necessary
	if(!gMainHook.Pre_IN_MouseEvent(mstate))
		return;

	gDllFuncs.IN_MouseEvent( mstate );

	// Run Post hook
	gMainHook.Post_IN_MouseEvent(mstate);
}


int Initialize( cl_enginefunc_t *pEnginefuncs, int iVersion )
{
	int retval = 0;

	memcpy( &gEngfuncs, pEnginefuncs, sizeof( cl_enginefunc_t ) );

	InitializeEngineHook();

	retval = gDllFuncs.Initialize( &gHookEngfuncs, iVersion );
	return( retval );
}


struct kbutton_s *KB_Find( const char *name )
{
	struct kbutton_s *retval = NULL;

	retval = gDllFuncs.KB_Find( name );
	return( retval );
}


void V_CalcRefdef( struct ref_params_s *pparams )
{
	// Run Pre hook, abort if necessary
	if(!gMainHook.Pre_V_CalcRefdef(pparams))
		return;

	gDllFuncs.V_CalcRefdef( pparams );

	// Run Post hook
	gMainHook.Post_V_CalcRefdef(pparams);
}