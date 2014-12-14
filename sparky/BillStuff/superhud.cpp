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

#pragma warning (disable:4244) // float to int loss of data

#include "..\client.h"
#include "util.h"

#include "hook.h"
#include "superhud.h"
#include "playerinfo.h"
#include "mempatcher.h"
#include "rulesmanager.h"
#include "addons.h"
#include "help.h"

extern "C"
{
#include "..\pm_shared\pm_shared.h"
}

/*Global*/
CSuperHUD gSuperHUD;

DECLARE_COMMAND_2(gSuperHUD, SuperHUDLevel);
DECLARE_COMMAND_2(gSuperHUD, Gamma);
DECLARE_COMMAND_2(gSuperHUD, PredictHop);
DECLARE_COMMAND_2(gSuperHUD, SetHUDColor);
DECLARE_COMMAND_2(gSuperHUD, ToggleColors);

bool CSuperHUD::Pre_HUD_Init (void)
{
	m_scrinfo.iSize = sizeof(m_scrinfo);
	gEngfuncs.pfnGetScreenInfo (&m_scrinfo);
	HOOK_COMMAND_2("bsuperhudlevel", SuperHUDLevel);
	gHelp.addCommand("bsuperhudlevel", "Toggles display of sparky HUD.\nUsage: bsuperhudlevel <0/1>");
	HOOK_COMMAND_2("bgamma", Gamma);
	gHelp.addCommand("bgamma", "Alters graphics card gamma level.\nUsage: bgamma <0.5 - 3>\n0.5 - darkest\n1 - normal\n3 - brightest\nNo parameters reports current gamma level"); 
	HOOK_COMMAND_2("bpredicthop", PredictHop);
	gHelp.addCommand("bpredicthop", "Toggles prediction of slowdown when exceeding speed limit on bunny hop servers.\nUsage: bpredicthop");
	HOOK_COMMAND_2("bsethudcolor", SetHUDColor);
	gHelp.addCommand("bsethudcolor", "Determines color and opacity of HUD.\nUsage: bsethudcolor <r> <g> <b> <brightness>\nIf any value is -1, the HUD color will be reset");
	HOOK_COMMAND_2("btogglecolors", ToggleColors);
	gHelp.addCommand("btogglecolors", "Toggles the use of hud colors in the rendering of various world entities.\nUsage: btogglecolors");

	return true;
}

void CSuperHUD::Post_HUD_Init (void)
{
	/*
	if(bHopOffsetFound) return;
	// test to patch bhop limit
	MODULEENTRY32 modentry;
	CMemoryPatcher patcher;
	if(!patcher.Init()) return;
	if(!patcher.FindModuleInProcess("client.dll", GetCurrentProcessId(), &modentry))
		return;
	bHopOffset = (BYTE*) patcher.FindPattern(GetCurrentProcessId(),(BYTE*)"\x51\x8B\x0D\xFC\x8A\x0B\x27\xD9\x81\xF4", 10, (DWORD) modentry.modBaseAddr, modentry.modBaseSize);
	if(bHopOffset)
	{
//		sprintf(buf, "echo found bhop code %x >> pfft.txt", offset);
//		system(buf);
		bHopOffsetFound = 1;
	}
	*/
	CopyFile("sparkyutils\\sparky.cfg", "tfc\\sparky.cfg", false);
	gEngfuncs.pfnClientCmd("exec sparky.cfg");
	/*
	if(CopyFile("tfc\\commandmenu.txt", "tfc\\commandmenu.backup", true))
	{
		gEngfuncs.pfnConsolePrint("[sparky] ------------------------------\n");
		gEngfuncs.pfnConsolePrint("[sparky] replacing commandmenu\n");
		gEngfuncs.pfnConsolePrint("[sparky] to restore previous menu:\n");
		gEngfuncs.pfnConsolePrint("[sparky] copy commandmenu.backup\n");
		gEngfuncs.pfnConsolePrint("[sparky] to commandmenu.txt\n");
		gEngfuncs.pfnConsolePrint("[sparky] this process only occurs\n");
		gEngfuncs.pfnConsolePrint("[sparky] if the backup does not exist\n");
		gEngfuncs.pfnConsolePrint("[sparky] ------------------------------\n");
		
		CopyFile("sparkyut.ils\\commandmenu.txt", "tfc\\commandmenu.txt", false);
	}
	*/
}

bool CSuperHUD::Pre_HUD_PlayerMove (struct playermove_s *ppmove, int server)
{
	if(gEngfuncs.GetLocalPlayer()->curstate.spectator) return true;
	if(bPredictHop)
	{
		PM_Move2(ppmove, server);
	}
	else
	{
		//PM_Move(ppmove, server);
		return true;
	}
	gPlayerinfo.Post_HUD_PlayerMove(ppmove, server);
	gAddons.Post_HUD_PlayerMove(ppmove, server);
	return false;
}

void CSuperHUD::Post_HUD_PlayerMove (struct playermove_s *ppmove, int server)
{
}

bool CSuperHUD::Pre_HUD_PlayerMoveInit (struct playermove_s *ppmove)
{
	PM_Init(ppmove);
	return true;
}

/*
bool CSuperHUD::Pre_HUD_PlayerMoveTexture (char *name)
{
	return false;
}

void CSuperHUD::Post_HUD_PlayerMoveTexture (char *name)
{
}
*/

void CSuperHUD::Post_HUD_PlayerMoveInit (struct playermove_s *ppmove)
{
}

CSuperHUD::CSuperHUD(void)
{
	iSuperHUDLevel = 1;
	bUsedGamma = 0;
	bPredictHop = 0;
	bHopOffset = 0;
	bHopOffsetFound = 0;
	HUDColor = Vector(-1, -1, -1);
	fHUDBrightness = -1;
	bHUDColorInGame = 0;
	for(int i = 0; i < 1024; i++)
		bRendered[i] = 0;
}

CSuperHUD::~CSuperHUD(void)
{
	if(bUsedGamma)
	{
		// Code taken from RadeonTweaker
	    int i;
	    short gamma_ramp[768];
	    HDC dc;
	    for (i = 0; i < 768; i++)
	        gamma_ramp[i] = (short)(((i & 255) / 256.0) * 65536);
		dc = GetDC(NULL);
		SetDeviceGammaRamp(dc, gamma_ramp);
	    ReleaseDC(NULL, dc);
	}
}

void CSuperHUD::Cmd_Gamma(void)
{
	float level;
	if(gEngfuncs.Cmd_Argc() > 1)
	{
		level = atof(gEngfuncs.Cmd_Argv(1));
		if((level >= 0.5) && (level <= 3))
		{
			// Code taken from RadeonTweaker
		    int i;
		    short gamma_ramp[768];
		    HDC dc;
			
			bUsedGamma = 1;
		    for (i = 0; i < 768; i++)
		        gamma_ramp[i] = (short)(pow(((i & 255) / 256.0), 1 / level) * 65536);
		    dc = GetDC(NULL);
		    SetDeviceGammaRamp(dc, gamma_ramp);
		    ReleaseDC(NULL, dc);
		}
	}
	/*
	// this stops working if you use double instead of gdouble :(
	else
	{
		char buf[1024];
		// Display current gamma
	    short gamma_ramp[768];
	    float sum = 0, gamma;
	    HDC dc;
	    dc = GetDC(NULL);
	    GetDeviceGammaRamp(dc, gamma_ramp);
	    ReleaseDC(NULL, dc);
	    sum =  (127 & 255) / 256.0;
	    gamma = log(sum) / log( ((float)gamma_ramp[127] / 65536.0) );
		sprintf(buf, "[sparky] gamma level is %f\n", gamma);
		gEngfuncs.pfnConsolePrint(buf);
	}
	*/
}

void CSuperHUD::Cmd_SuperHUDLevel(void)
{
	int level;
	if(gEngfuncs.Cmd_Argc() > 1)
	{
		level = atoi(gEngfuncs.Cmd_Argv(1));
		if((level >= 0) && (level <= 3))
		{
			char buf[64];
			iSuperHUDLevel = level;
			sprintf(buf, "HUD level %d", level);
			gSuperHUD.WriteHUDChannel(HC_HUDLEVEL, 1, buf);
		}
	}
}

void CSuperHUD::Cmd_PredictHop(void)
{
	/*
	if(!bHopOffsetFound) return;
	CMemoryPatcher patcher;
	MODULEENTRY32 modentry;
	if(!patcher.Init()) return;
	if(!patcher.FindModuleInProcess("client.dll", GetCurrentProcessId(), &modentry))
		return;
	*/
	if(bPredictHop == 0)
	{
		bPredictHop = 1;
		//patcher.WriteProcessByte(GetCurrentProcessId(), bHopOffset, 0xc3);
		gSuperHUD.WriteHUDChannel(HC_IN_GAME_COLOR, 1, "bhop fix - on");
		return;
	}

	if(bPredictHop == 1)
	{
		bPredictHop = 0;
		//patcher.WriteProcessByte(GetCurrentProcessId(), bHopOffset, 0x51);
		gSuperHUD.WriteHUDChannel(HC_IN_GAME_COLOR, 1, "bhop fix - off");
		return;
	}
}

void CSuperHUD::Cmd_SetHUDColor(void)
{
	if(gEngfuncs.Cmd_Argc() > 4)
	{
		HUDColor.x = atoi(gEngfuncs.Cmd_Argv(1));
		if(HUDColor.x > 255) HUDColor.x = 255;
		HUDColor.y = atoi(gEngfuncs.Cmd_Argv(2));
		if(HUDColor.y > 255) HUDColor.y = 255;
		HUDColor.z = atoi(gEngfuncs.Cmd_Argv(3));
		if(HUDColor.z > 255) HUDColor.z = 255;
		fHUDBrightness = atof(gEngfuncs.Cmd_Argv(4));
		if((HUDColor.x < 0) || (HUDColor.y < 0) || (HUDColor.z < 0) || (fHUDBrightness < 0))
		{
			fHUDBrightness = -1;
		}
	}
}

void CSuperHUD::Cmd_ToggleColors(void)
{
	if(bHUDColorInGame)
	{
		bHUDColorInGame = false;
		gSuperHUD.WriteHUDChannel(HC_IN_GAME_COLOR, 1, "hud colors in game - off");
	}
	else
	{
		bHUDColorInGame = true;
		gSuperHUD.WriteHUDChannel(HC_IN_GAME_COLOR, 1, "hud colors in game - on");
	}
}

bool CSuperHUD::Pre_HUD_Redraw (float flTime, int intermission)
{
	return true;
}

void CSuperHUD::Post_HUD_Redraw (float flTime, int intermission)
{
	int i;

	// If HUD is off, don't do anything
	if(iSuperHUDLevel < 1) return;

	// Draw floating text
	FloatingText.UpdateFloatingText();
	FloatingText3d.UpdateFloatingText();

	// Display the HUD channels
	for(i = 0; i < 64; i++)
	{
		Channel[i].DisplayHUDChannel(i);
	}

	if(iSuperHUDLevel > 1)
	{
		DrawHUDLabels();
	}

	for(i = 0; i < 1024; i++)
		bRendered[i] = 0;
	
	/* TEMPORARY STUFF */
	/*
	static Vector vecOldAngles[33], vecOldOrigin[33];
	static bool bInit;
	if(!bInit)
	{
		for(i = 0; i < 33; i++)
		{
			vecOldAngles[i] = Vector(0,0,0);
			vecOldOrigin[i] = Vector(0,0,0);
		}
		bInit = true;
	}

	for(i = 1; i < 33; i++)
	{
		cl_entity_t *ent;
		ent = gEngfuncs.GetEntityByIndex(i);
		if(ent != NULL)
		{
			if(ent->player && ent->curstate.solid)
			{
				int beamindex = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/laserbeam.spr");
				vec3_t vecEnd, up, right, forward, EntViewOrg, vecTarget;
				VectorCopy(ent->origin,EntViewOrg);
				if (ent->curstate.usehull == 0)
					EntViewOrg[2] += 26;
				else if (ent->curstate.usehull == 1)
					EntViewOrg[2] += 12;
				gEngfuncs.pfnAngleVectors (Vector(ent->angles.x*(8.9f/2.6f), ent->angles.y, ent->angles.z), forward, right, up);
				forward[2] = -forward[2];
				vec3_t vecBegin(EntViewOrg);
				vecEnd = vecBegin + forward * 4096;
				//gEngfuncs.pEfxAPI->R_BeamPoints(vecBegin,vecEnd,beamindex,0.001f,0.9f,0,32,2,0,0,0,127,255);
				pmtrace_t *vis;
		        vis = gEngfuncs.PM_TraceLine((float *)&vecBegin, (float *)&vecEnd, PM_TRACELINE_ANYVISIBLE, 2, -1);
				vecTarget = vis->endpos;
				float ScrPos[2];
				if(CalcScreen(vecTarget, ScrPos))
				{
					gEngfuncs.pfnDrawSetTextColor(0, 127, 255);
					gEngfuncs.pfnDrawConsoleString(ScrPos[0], ScrPos[1], "+");
				}
				if(CalcScreen(ent->origin, ScrPos))
				{
					gEngfuncs.pfnDrawSetTextColor(255, 127, 0);
					gEngfuncs.pfnDrawConsoleString(ScrPos[0], ScrPos[1], "o");
				}
				char buf[1024];
				hud_player_info_t pinfo;
				gEngfuncs.pfnGetPlayerInfo(i, &pinfo);
			}
		}
	}
	*/
}

void CSuperHUD::WriteHUDChannel(int iChannel, float fPeriod, char *WriteText)
{
	if((iChannel < 0) || (iChannel > 63)) return;

	Channel[iChannel].WriteHUDChannel(fPeriod, WriteText);
}

void CSuperHUD::DrawHUDLabels (void)
{
}


bool CSuperHUD::Pre_HUD_AddEntity (int type, struct cl_entity_s *ent, const char *modelname)
{
	return true;
}

void CSuperHUD::Post_HUD_AddEntity (int type, struct cl_entity_s *ent, const char *modelname)
{
	bRendered[ent->index] = 1;

	if(type == ET_BEAM)
	{
		// OMG OMG RAWFLEZ pulsating beams!
		/*
		ent->curstate.rendercolor.r *= (3+sin(gEngfuncs.GetClientTime()*3))/4;
		ent->curstate.rendercolor.g *= (3+sin(gEngfuncs.GetClientTime()*3))/4;
		ent->curstate.rendercolor.b *= (3+sin(gEngfuncs.GetClientTime()*3))/4;
		*/
		// phase lasers in and out of hud color
		// please note whoring of c/c++ operators
		if(bHUDColorInGame && (fHUDBrightness > 0))
		{
			float phase = (1+sin(gEngfuncs.GetClientTime()*2))/2;
			ent->curstate.rendercolor.r *= phase;
			ent->curstate.rendercolor.r += HUDColor.x * (1-phase);
			ent->curstate.rendercolor.g *= phase;
			ent->curstate.rendercolor.g += HUDColor.y * (1-phase);
			ent->curstate.rendercolor.b *= phase;
			ent->curstate.rendercolor.b += HUDColor.z * (1-phase);
		}
	}

	if(type == ET_TEMPENTITY)
	{
		// set explosions/flames to HUD color
		if(bHUDColorInGame && (fHUDBrightness > 0))
		{
			ent->curstate.rendercolor.r = HUDColor.x;
			ent->curstate.rendercolor.g = HUDColor.y;
			ent->curstate.rendercolor.b = HUDColor.z;
		}
	}

	/*
	if(ent->player && ent->curstate.solid)
	{
		int beamindex = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/laserbeam.spr");
		vec3_t vecEnd, up, right, forward, EntViewOrg;
		VectorCopy(ent->origin,EntViewOrg);
		if (ent->curstate.usehull == 0)
			EntViewOrg[2] += 26;
		else if (ent->curstate.usehull == 1)
			EntViewOrg[2] += 12;
		gEngfuncs.pfnAngleVectors (Vector(ent->angles.x*(8.9f/2.6f), ent->angles.y, ent->angles.z), forward, right, up);
		forward[2] = -forward[2];
		vec3_t vecBegin(EntViewOrg);
		vecEnd = vecBegin + forward * 64;
		gEngfuncs.pEfxAPI->R_BeamPoints(vecBegin,vecEnd,beamindex,0.001f,0.9f,0,32,2,0,0,0,127, 255);
	}
	*/
}




// HUD Channel stuff

void CHUDChannel::WriteHUDChannel (float fPeriod, char *WriteText)
{
	if(strlen(WriteText) < 1) return;

	sprintf(Text, "[%s]", WriteText);
	fStartTime = gEngfuncs.GetClientTime();
	fEndTime = fStartTime + fPeriod;
}

void CHUDChannel::DisplayHUDChannel (int iSlot)
{
	// (int)((iTimelimit - gEngfuncs.GetClientTime())/60)
	int ix;
	int iy;
	
	// Check that the times aren't messed
	if(fStartTime > gEngfuncs.GetClientTime())
	{
		fStartTime = 0;
		fEndTime = 0;
		return;
	}

	if((fEndTime > gEngfuncs.GetClientTime()) && (strlen(Text) > 0))
	{
		if(iSlot < 32)
		{
			ix = 16;
			iy = 128 + iSlot*16;
		}
		else
		{
			ix = (gSuperHUD.m_scrinfo.iWidth - 16) - (gSuperHUD.m_scrinfo.charWidths['0']*16);
			iy = 128 + (iSlot-32)*16;
		}

		if(iSlot == HC_ROUND_TIMER) {
			if(((int)((gRulesManager.iTimelimit - gEngfuncs.GetClientTime())/60) < 1) && (gRulesManager.iTimelimit > 0)) {
				gSuperHUD.DrawHudString(ix, iy, 255, 15, 9, Text);
			} else {
				if(gSuperHUD.fHUDBrightness > -1)
					gSuperHUD.DrawHudString(ix, iy, (gSuperHUD.HUDColor.x*(fEndTime - gEngfuncs.GetClientTime()))/(fEndTime-fStartTime), (gSuperHUD.HUDColor.y*(fEndTime - gEngfuncs.GetClientTime()))/(fEndTime-fStartTime), (gSuperHUD.HUDColor.z*(fEndTime - gEngfuncs.GetClientTime()))/(fEndTime-fStartTime), Text);
				else
					gSuperHUD.DrawHudString(ix, iy, (127*(fEndTime - gEngfuncs.GetClientTime()))/(fEndTime-fStartTime), (255*(fEndTime - gEngfuncs.GetClientTime()))/(fEndTime-fStartTime), (127*(fEndTime - gEngfuncs.GetClientTime()))/(fEndTime-fStartTime), Text);
			}
		} else {
			if(gSuperHUD.fHUDBrightness > -1)
			{
				gSuperHUD.DrawHudString(ix, iy, (gSuperHUD.HUDColor.x*(fEndTime - gEngfuncs.GetClientTime()))/(fEndTime-fStartTime), (gSuperHUD.HUDColor.y*(fEndTime - gEngfuncs.GetClientTime()))/(fEndTime-fStartTime), (gSuperHUD.HUDColor.z*(fEndTime - gEngfuncs.GetClientTime()))/(fEndTime-fStartTime), Text);
			}
			else
			{
				gSuperHUD.DrawHudString(ix, iy, (127*(fEndTime - gEngfuncs.GetClientTime()))/(fEndTime-fStartTime), (255*(fEndTime - gEngfuncs.GetClientTime()))/(fEndTime-fStartTime), (127*(fEndTime - gEngfuncs.GetClientTime()))/(fEndTime-fStartTime), Text);
			}
		}
	}
	else
	{
		fStartTime = 0;
		fEndTime = 0;
	}
}

CHUDChannel::CHUDChannel (void)
{
	sprintf(Text, "\0");
	fStartTime = 0;
	fEndTime = 0;
}

int CSuperHUD::DrawHudString (int x, int y, int r, int g, int b, const char *fmt, ... )
{
	va_list va_alist;
	char buf[256];

	va_start (va_alist, fmt);
	_vsnprintf (buf, sizeof(buf), fmt, va_alist);
	va_end (va_alist);
	
	// draw the string until we hit the null character or a newline character
	for ( char * p = buf; *p; p++ )
	{
		int next = x + m_scrinfo.charWidths[*p]; // variable-width fonts look cool

		gEngfuncs.pfnDrawCharacter (x, y, *p, r, g, b);
		x = next;
	}

	return x;
}

CFloatingText::CFloatingText(void)
{
	int i;
	for(i = 0; i < FLOATING_TEXT_SLOTS; i++)
	{
		FloatText[i].bFree = 1;
	}
}

void CFloatingText::AddFloatingText(char *szText, int ix, int iy, Vector Color, float fEndTime, bool bConsole)
{
	// Find a free text slot
	int i, iFreeSlot;
	bool bFoundOne = 0;
	for(i = 0; i < FLOATING_TEXT_SLOTS; i++)
	{
		if(FloatText[i].bFree)
		{
			bFoundOne = 1;
			iFreeSlot = i;
		}
	}
	if(!bFoundOne || (ix < 0) || (iy < 0) || (ix >= gSuperHUD.m_scrinfo.iWidth) || (iy >= gSuperHUD.m_scrinfo.iWidth)) return;

	i = iFreeSlot;
	sprintf(FloatText[i].szText, "%s", szText);
	FloatText[i].bFree = 0;
	FloatText[i].fEndTime = fEndTime;
	FloatText[i].ix = ix;
	FloatText[i].iy = iy;
	FloatText[i].Color = Color;
	FloatText[i].bConsole = bConsole;
}

void CFloatingText::UpdateFloatingText(void)
{
	// remove timed-out texts and draw all others to screen
	int i;
	float time = gEngfuncs.GetClientTime();
	for(i = 0; i < FLOATING_TEXT_SLOTS; i++)
	{
		if(!FloatText[i].bFree)
		{
			if(FloatText[i].fEndTime < time)
			{
				FloatText[i].bFree = 1;
			}
			else
			{
				if(FloatText[i].fEndTime - time < 1)
				{
					if(FloatText[i].bConsole)
					{
						gEngfuncs.pfnDrawSetTextColor(FloatText[i].Color.x, FloatText[i].Color.y, FloatText[i].Color.z);
						gEngfuncs.pfnDrawConsoleString(FloatText[i].ix, FloatText[i].iy, FloatText[i].szText);
					}
					else
					{
						gSuperHUD.DrawHudString (FloatText[i].ix, FloatText[i].iy - gSuperHUD.m_scrinfo.iCharHeight/2, FloatText[i].Color.x * (FloatText[i].fEndTime - time), FloatText[i].Color.y * (FloatText[i].fEndTime - time), FloatText[i].Color.z * (FloatText[i].fEndTime - time), FloatText[i].szText);
					}
				}
				else
				{
					if(FloatText[i].bConsole)
					{
						gEngfuncs.pfnDrawSetTextColor(FloatText[i].Color.x, FloatText[i].Color.y, FloatText[i].Color.z);
						gEngfuncs.pfnDrawConsoleString(FloatText[i].ix, FloatText[i].iy, FloatText[i].szText);
					}
					else
					{
						gSuperHUD.DrawHudString (FloatText[i].ix, FloatText[i].iy - gSuperHUD.m_scrinfo.iCharHeight/2, FloatText[i].Color.x, FloatText[i].Color.y, FloatText[i].Color.z, FloatText[i].szText);
					}
				}
			}
		}
	}
}

CFloatingText3d::CFloatingText3d(void)
{
	int i;
	for(i = 0; i < FLOATING_TEXT_SLOTS; i++)
	{
		FloatText[i].bFree = 1;
	}
}

void CFloatingText3d::AddFloatingText(char *szText, Vector Origin, Vector Color, float fEndTime, bool bConsole)
{
	// Find a free text slot
	int i, iFreeSlot;
	bool bFoundOne = 0;
	for(i = 0; i < FLOATING_TEXT_SLOTS; i++)
	{
		if(FloatText[i].bFree)
		{
			bFoundOne = 1;
			iFreeSlot = i;
		}
	}
	if(!bFoundOne) return;

	i = iFreeSlot;
	sprintf(FloatText[i].szText, "%s", szText);
	FloatText[i].bFree = 0;
	FloatText[i].fEndTime = fEndTime;
	FloatText[i].Origin = Origin;
	FloatText[i].Color = Color;
	FloatText[i].bConsole = bConsole;
}

void CFloatingText3d::UpdateFloatingText(void)
{
	// remove timed-out texts and draw all others to screen
	float ScrPos[2];
	int i;
	float time = gEngfuncs.GetClientTime();
	for(i = 0; i < FLOATING_TEXT_SLOTS; i++)
	{
		if(!FloatText[i].bFree)
		{
			if(FloatText[i].fEndTime < time)
			{
				FloatText[i].bFree = 1;
			}
			else
			{
				if(CalcScreen(FloatText[i].Origin, ScrPos))
				{
					if(FloatText[i].fEndTime - time < 1)
					{
						if(FloatText[i].bConsole)
						{
							gEngfuncs.pfnDrawSetTextColor(FloatText[i].Color.x, FloatText[i].Color.y, FloatText[i].Color.z);
							gEngfuncs.pfnDrawConsoleString(ScrPos[0], ScrPos[1], FloatText[i].szText);
						}
						else
						{
							gSuperHUD.DrawHudString (ScrPos[0], ScrPos[1] - gSuperHUD.m_scrinfo.iCharHeight/2, FloatText[i].Color.x * (FloatText[i].fEndTime - time), FloatText[i].Color.y * (FloatText[i].fEndTime - time), FloatText[i].Color.z * (FloatText[i].fEndTime - time), FloatText[i].szText);
						}
					}
					else
					{
						if(FloatText[i].bConsole)
						{
							gEngfuncs.pfnDrawSetTextColor(FloatText[i].Color.x, FloatText[i].Color.y, FloatText[i].Color.z);
							gEngfuncs.pfnDrawConsoleString(ScrPos[0], ScrPos[1], FloatText[i].szText);
						}
						else
						{
							gSuperHUD.DrawHudString (ScrPos[0], ScrPos[1] - gSuperHUD.m_scrinfo.iCharHeight/2, FloatText[i].Color.x, FloatText[i].Color.y, FloatText[i].Color.z, FloatText[i].szText);
						}
					}
				}
			}
		}
	}
}