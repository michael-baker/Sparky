/*  ------------------------------------------------------------------------
	  glhook.cpp by ruiner based on mstr code
		- modified v1.42 - added ability to use wallhack while spectator
		- added v1.41 - 04/09/03
	------------------------------------------------------------------------ */

#include "..\client.h"
#include "hook.h"
#include "superhud.h"
#include <gl\gl.h>
#include "glhook.h"
#include "help.h"

#pragma comment (lib, "opengl32.lib") 

/*Global*/
CGLHook gGLHook;

DECLARE_COMMAND_2(gGLHook, ToggleWallHack);
DECLARE_COMMAND_2(gGLHook, ToggleFogHack)

bool CGLHook::Pre_HUD_Init (void)
{
	HOOK_COMMAND_2("bwallhack", ToggleWallHack);
	gHelp.addCommand("bwallhack", "Toggles wallhack (only while playing a demo).\nUsage: bwallhack");
	HOOK_COMMAND_2("bfoghack", ToggleFogHack);
	gHelp.addCommand("bfoghack", "Toggles foghack (only while playing a demo).\nUsage: bfoghack");
	bFogHack = false;
	bWallHack = false;
	bSpectator = false;
	bPlayingDemo = false;
	return true;
}

void CGLHook::Cmd_ToggleWallHack (void)
{
	bWallHack = bWallHack ? 0 : 1;

	if(bWallHack)
		gSuperHUD.WriteHUDChannel(HC_IN_GAME_COLOR, 2, "wallhack ON");
	else
		gSuperHUD.WriteHUDChannel(HC_IN_GAME_COLOR, 2, "wallhack OFF");
}

void CGLHook::Cmd_ToggleFogHack (void)
{
	bFogHack = bFogHack ? 0 : 1;

	if(bFogHack)
		gSuperHUD.WriteHUDChannel(HC_IN_GAME_COLOR, 2, "foghack ON");
	else
		gSuperHUD.WriteHUDChannel(HC_IN_GAME_COLOR, 2, "foghack OFF");
}

bool CGLHook::Pre_HUD_AddEntity (int type, struct cl_entity_s *ent, const char *modelname)
{
/*	if(!strstr(modelname,"flare1.spr")) // i want to make a nice opengl flare to replace hl flare :D
	{
		return false;
	} */
	return true;
}

bool CGLHook::Pre_HUD_Redraw (float flTime, int intermission)
{
	cl_entity_s *pLocal = gEngfuncs.GetLocalPlayer();

	if(pLocal->curstate.playerclass == 0)
		bSpectator = true;
	else
		bSpectator = false;

	bPlayingDemo = gEngfuncs.pDemoAPI->IsPlayingback();

	return true;
}

void( APIENTRY *pglEnable )(int arg1) = 0;
void APIENTRY hook_glEnable (int arg1)
{

	if ((arg1 == GL_FOG) && ((gGLHook.bPlayingDemo || gGLHook.bSpectator)))
	{ // no-fog hax
		return;
	}
	pglEnable(arg1);
}

void ( APIENTRY *pglBegin )(int arg1) = 0;
void APIENTRY hook_glBegin (int arg1)
{
	if ((gGLHook.bWallHack) && ((gGLHook.bPlayingDemo || gGLHook.bSpectator))) { // wall hax
		if((arg1 == GL_TRIANGLE_STRIP) || (arg1 == GL_TRIANGLE_FAN)) glDisable(GL_DEPTH_TEST);
		else if(arg1 != GL_QUADS) glEnable(GL_DEPTH_TEST);
	}

	pglBegin(arg1);	
}

