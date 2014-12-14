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

#include "hook.h"
#include "superhud.h"
#include "timers.h"
#include <time.h>
#include "rulesmanager.h"

/*Global*/
CTimers gTimers;

DECLARE_COMMAND_2(gTimers, StartUserTimer);
DECLARE_COMMAND_2(gTimers, ToggleTimers);

bool CTimers::Pre_HUD_Init (void)
{
	HOOK_COMMAND_2("bstarttimer", StartUserTimer);
	HOOK_COMMAND_2("btimers", ToggleTimers);

	return true;
}

void CTimers::Post_HUD_Init (void)
{
}

bool CTimers::Pre_HUD_Frame (double time)
{
	return true;
}

void CTimers::Post_HUD_Frame (double time)
{
	if(bNewMap)
	{
		bNewMap = 0;
	}
	if((fLastTime-10) > gEngfuncs.GetClientTime())
	{
		bNewMap = 1;
	}
	fLastTime = gEngfuncs.GetClientTime();

	// check for new map
	if(gEngfuncs.GetClientTime() < fRoundTimerStart)
	{
		bNewMap = 1;
		fRoundTimerStart = gEngfuncs.GetClientTime();
	}
	if(gEngfuncs.GetClientTime() < fUserTimerStart)
	{
		fUserTimerStart = gEngfuncs.GetClientTime();
	}

	// draw the timers
	char buf[1024];

	if(bTimersUser)
	{
		float time = gEngfuncs.GetClientTime();

		if(!bUserTimerStopped)
			fUserTimerValue = time - fUserTimerStart;

		//user timer write
		//sprintf(buf, "user - %.2f", fUserTimerValue);
		// thanks to xack for figuring out the format specifiers
		sprintf(buf, "user - %02d:%05.02f", (int)(fUserTimerValue/60), fmod(fUserTimerValue, 60));
		gSuperHUD.WriteHUDChannel(HC_USER_TIMER, 1, buf);
	}

	if(bTimersClock)
	{	
		char tbuf[64];
		_strtime(tbuf);
		//cronological time write
		sprintf(buf, "time - %s", tbuf);
		gSuperHUD.WriteHUDChannel(HC_CLOCK, 1, buf);
	}

	if(bTimersRound)
	{
		float time = gEngfuncs.GetClientTime();
		//round timer write
		sprintf(buf, "round - %02d:%02d", (int)((time - fRoundTimerStart)/60), (int)fmod(time - fRoundTimerStart, 60));
		gSuperHUD.WriteHUDChannel(51, 1, buf);
	}

	if(gRulesManager.bTimeleftOn)
	{
		if(gRulesManager.iTimelimit)
		{
			sprintf(buf, "timeleft - %02d:%02d", (int)((gRulesManager.iTimelimit - gEngfuncs.GetClientTime())/60), (int)fmod(gRulesManager.iTimelimit - gEngfuncs.GetClientTime(), 60));
			gSuperHUD.WriteHUDChannel(HC_ROUND_TIMER, 1, buf);
		}
		else
			gSuperHUD.WriteHUDChannel(HC_ROUND_TIMER, 1, "timeleft - no limit");
	}
}

bool CTimers::Pre_Hook_TextMsg (const char *pszString)
{
	return true;
}

void CTimers::Post_Hook_TextMsg (const char *pszString)
{
}

CTimers::CTimers(void)
{
	bTimersOn = 0;
	bUserTimerStopped = 1;
	fUserTimerStart = 9999;
	fUserTimerValue = 0;
	fRoundTimerStart = 9999;
}

void CTimers::Cmd_StartUserTimer (void)
{
	if(bUserTimerStopped)
	{
		bUserTimerStopped = 0;
		fUserTimerValue = 0;
		fUserTimerStart = gEngfuncs.GetClientTime();
	}
	else
	{
		bUserTimerStopped = 1;
	}

	gEngfuncs.pfnClientCmd("speak weapons/ax1;");
}

void CTimers::Cmd_ToggleTimers (void)
{
	if(gEngfuncs.Cmd_Argc() < 2)
	{
		bTimersOn = bTimersOn ? 0 : 1;

		if(!bTimersOn)
		{
			gSuperHUD.WriteHUDChannel(HC_IN_GAME_COLOR, 2, "timers OFF");
			bTimersUser = 0;
			bTimersClock = 0;
			bTimersRound = 0;
			gRulesManager.bTimeleftOn = 0;
		}
		else
		{
			gSuperHUD.WriteHUDChannel(HC_IN_GAME_COLOR, 2, "timers ON");
			bTimersUser = 1;
			bTimersClock = 1;
			bTimersRound = 1;
			gRulesManager.bTimeleftOn = 1;
		}
	
		gEngfuncs.pfnConsolePrint("[sparky] timers toggled, other commands: user, clock, round, timeleft\n");
	}

	if(!strcmp(gEngfuncs.Cmd_Argv(1), "user"))
	{
		bTimersUser = bTimersUser ? 0 : 1;

		if(bTimersUser)
			gSuperHUD.WriteHUDChannel(HC_IN_GAME_COLOR, 2, "timers (user) ON");
		else
			gSuperHUD.WriteHUDChannel(HC_IN_GAME_COLOR, 2, "timers (user) OFF");
	}

	if(!strcmp(gEngfuncs.Cmd_Argv(1), "clock"))
	{
		bTimersClock = bTimersClock ? 0 : 1;

		if(bTimersClock)
			gSuperHUD.WriteHUDChannel(HC_IN_GAME_COLOR, 2, "timers (clock) ON");
		else
			gSuperHUD.WriteHUDChannel(HC_IN_GAME_COLOR, 2, "timers (clock) OFF");
	}

	if(!strcmp(gEngfuncs.Cmd_Argv(1), "round"))
	{
		bTimersRound = bTimersRound ? 0 : 1;

		if(bTimersRound)
			gSuperHUD.WriteHUDChannel(HC_IN_GAME_COLOR, 2, "timers (round) ON");
		else
			gSuperHUD.WriteHUDChannel(HC_IN_GAME_COLOR, 2, "timers (round) OFF");
	}

	if(!strcmp(gEngfuncs.Cmd_Argv(1), "timeleft"))
	{
		gRulesManager.bTimeleftOn = gRulesManager.bTimeleftOn ? 0 : 1;

		if(gRulesManager.bTimeleftOn)
			gSuperHUD.WriteHUDChannel(HC_IN_GAME_COLOR, 2, "timers (timeleft) ON");
		else
			gSuperHUD.WriteHUDChannel(HC_IN_GAME_COLOR, 2, "timers (timeleft) OFF");
	}
}