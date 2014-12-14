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
 *
 * Stats addition by L0ki. Overhauled by FB.
 *
 */

#include "..\client.h"

#include "hook.h"
#include "superhud.h"
#include "stats.h"
#include "util.h"
#include <time.h>
#include "help.h"

#pragma warning (disable:4244) // float to int loss of data

/*Global*/
CStats gStats;

DECLARE_COMMAND_2(gStats, ToggleStats);

bool CStats::Pre_HUD_Init (void)
{
	HOOK_COMMAND_2("bstats", ToggleStats);
	gHelp.addCommand("bstats", "bstats display\nDisplays kills, deaths and k/d ratio.\nUsage: bstats display\nbstats say\nAnnounces kills, deaths and k/d ratio.\nUsage: bstats say");

	return true;
}

void CStats::Post_HUD_Init (void)
{
}

bool CStats::Pre_HUD_Redraw (float flTime, int intermission)
{
	return true;
}

void CStats::Post_HUD_Redraw (float flTime, int intermission)
{
	if (bShowStatsOn)
	{
		int barR, barG, barB;
		if((gSuperHUD.HUDColor.x >= 0) && (gSuperHUD.HUDColor.y >= 0) && (gSuperHUD.HUDColor.z >= 0))
		{
			barR = gSuperHUD.HUDColor.x;
			barG = gSuperHUD.HUDColor.y;
			barB = gSuperHUD.HUDColor.z;	
		}
		else
		{
			barR = 240;	
			barG = 179;
			barB = 16;
		}
		int istatsy = gSuperHUD.m_scrinfo.iHeight*0.64;
		int istatsx = gSuperHUD.m_scrinfo.iWidth*0.02;
		char buf[1024];
		sprintf(buf,"[stats: %i/%i (%0.2f)]",iKills,iDeaths,fRatioKills);
		gSuperHUD.DrawHudString(istatsx, istatsy, barR, barG, barB, buf);
		//gSuperHUD.WriteHUDChannel(HC_STATS_STATS, 1, buf);
		/*if(iDeaths != 0)
		{
			sprintf(buf,"ratio: %0.2f",fRatioKills);
		}
		else
		{
			sprintf(buf,"perfect %.1f ratio",fRatioKills);
		}
		gSuperHUD.WriteHUDChannel(HC_STATS_RATIO, 1, buf);*/
	}
}

bool CStats::Pre_HUD_Frame(double time)
{
	return true;
}

void CStats::Post_HUD_Frame(double time)
{
	if((fLastTime-10) > gEngfuncs.GetClientTime())
	{
		iKills = 0;
		iDeaths = 0;
		fRatioKills = 0;
	}
	fLastTime = gEngfuncs.GetClientTime();
}

void CStats::hook_scoreinfo(int cl, int frags, int deaths, int team)
{
	cl_entity_t *pLocal = gEngfuncs.GetLocalPlayer();
	if(cl == pLocal->index && bShowStatsOn)
	{
		iKills = frags;
		iDeaths = deaths;
		if(iDeaths != 0)
			fRatioKills = (float)frags / (float)deaths;
		else
			fRatioKills = (float)frags;	
	}
}

// FB's edited cmds

void CStats::Cmd_ToggleStats (void)
{
	if(gEngfuncs.Cmd_Argc() < 2)
	{
		gEngfuncs.pfnConsolePrint("[sparky] valid bstats commands: display say\n");
		return;
	}

	if(!strcmp(gEngfuncs.Cmd_Argv(1), "display"))
	{
		bShowStatsOn = bShowStatsOn ? 0 : 1;
		
		if(bShowStatsOn)
			gSuperHUD.WriteHUDChannel(HC_STATS_OPTION, 2, "stats display ON");
		else
			gSuperHUD.WriteHUDChannel(HC_STATS_OPTION, 2, "stats display OFF");
	}

	if(!strcmp(gEngfuncs.Cmd_Argv(1), "say"))
	{
		char buf[1024];
		char *temp;
		sprintf(buf,"say I have: %i kill", iKills);
		temp = buf;
		if(iKills != 1)
			strcat(temp,"s");
		sprintf(buf, "%s and %i death", temp, iDeaths);
		temp = buf;
		if (iDeaths != 1)
			strcat(temp,"s");
		sprintf(buf,"%s (",temp);
		temp = buf;
		if (iDeaths == 0)
			sprintf(buf,"%sPerfect %.2f ratio)  [sparky Stats]",temp,fRatioKills);
		else
			sprintf(buf,"%sK/D: %.2f)  [sparky Stats]", temp, fRatioKills);
		gEngfuncs.pfnClientCmd(buf);
	}
}

CStats::CStats (void)
{
	bShowStatsOn = 0;
	iKills = 0;
	iDeaths = 0;
	fRatioKills = 0;
	fLastTime = 0;
}
