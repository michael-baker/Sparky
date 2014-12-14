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
 * winamp3 support added by ruiner for use with the Winamp 2x Plugin Manager for Winamp 3
 * Winamp 2x Plugin Manager for Winamp3 is Copyright 2001 - 2002 Nicholas Schweitzer
 * plugin download location: http://www.winamp.com/components3/detail.jhtml?componentId=118230
 *
 */

/*  ------------------------------------------------------------------------
	  winamp.cpp by billdoor
	    - updated v1.33 - winamp3 support (via plugin, see header) by ruiner
	    - updated v1.3 - bwinamp (display/volup/voldown/spam) by loki
		- added v1.0 - 01/12/03
	------------------------------------------------------------------------ */

#include "..\client.h"

#include "hook.h"
#include "superhud.h"
#include "winamp.h"
#include "util.h"
#include <time.h>
#include "..\winampfrontend.h"
#include "help.h"

#pragma warning (disable:4244) // float to int loss of data

/*Global*/
CWinamp gWinamp;

DECLARE_COMMAND_2(gWinamp, Winamp);
DECLARE_COMMAND_2(gWinamp, ToggleMicWinampMute);

bool CWinamp::Pre_HUD_Init (void)
{
	HOOK_COMMAND_2("bwinamp", Winamp);
	gHelp.addCommand("bwinamp", "Winamp must be running to function.\nUsage: bwinamp play / stop / pause / prev / next / display / spam / volup / voldown / shuffle\nNo parameters will display the current track.\nbwinamp display - toggles status display\nbwinamp spam - announces current track");
	HOOK_COMMAND_2("bmicwinampmute", ToggleMicWinampMute);
	gHelp.addCommand("bmicwinampmute", "Pauses your current winamp song while you or someone else is talking on voicecom and then continues the song when the talking is done.\nUsage: bmicwinampmute");

	return true;
}

void CWinamp::Post_HUD_Init (void)
{
}

bool CWinamp::Pre_Hook_SayText (const char *pszString)
{
	return true;
}

void CWinamp::Post_Hook_SayText (const char *pszString)
{
	const char *p;
	if(strlen(pszString) > 5)
	{
		p = pszString + strlen(pszString) - 5;
		if(!strcmp(p, "!wut\n"))
		{
			char buf[4096];
			HWND hwWinamp = FindWindow("Winamp v1.x",NULL);
			if(hwWinamp)
			{
				if(fLastTriggerTime > gEngfuncs.GetClientTime())
					fLastTriggerTime = gEngfuncs.GetClientTime();
				if((gEngfuncs.GetClientTime() - fLastTriggerTime) > 2)
				{
					fLastTriggerTime = gEngfuncs.GetClientTime();
					sprintf(buf, "say \"[sparky] %s\";", GetCurrentTrack(hwWinamp));
					gEngfuncs.pfnClientCmd(buf);
				}
			}
		}
	}
}

bool CWinamp::Pre_HUD_Redraw (float flTime, int intermission)
{
	return true;
}
void CWinamp::Post_HUD_Redraw (float flTime, int intermission)
{	
	HWND hwWinamp = FindWindow("Winamp v1.x",NULL);
	if ((bShowWinampDisp) && (hwWinamp) && (gSuperHUD.iSuperHUDLevel))
	{
		curpos = (::SendMessage(hwWinamp,WM_WA_IPC,0,IPC_GETOUTPUTTIME)) / 1000;
		songlen = (::SendMessage(hwWinamp,WM_WA_IPC,1,IPC_GETOUTPUTTIME));
		playstatus = ::SendMessage(hwWinamp,WM_WA_IPC,0,IPC_ISPLAYING);

		char buf[1024];
		char status[10];

		if(playstatus == 0)
			sprintf(status,"%s", "stopped");
		else if (playstatus == 1)
			sprintf(status,"%s", "playing");
		else if(playstatus == 3)
			sprintf(status, "%s", "paused");
		else
			sprintf(status,"%s", "unknown");

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
		int iwinampy = gSuperHUD.m_scrinfo.iHeight*0.68;
		int iwinampx = gSuperHUD.m_scrinfo.iWidth*0.02;
		int iwinampy2 = iwinampy + gSuperHUD.m_scrinfo.iCharHeight - 3;

		HWND hwWinamp3 = FindWindow("STUDIO",NULL);
		if(!hwWinamp3) {
			sprintf(buf, "[status: %s]",status);
			gSuperHUD.DrawHudString(iwinampx, iwinampy2, barR, barG, barB, buf);
			sprintf(buf, "[%s  %i:%.2i/%i:%.2i]", GetCurrentTrack(hwWinamp), (curpos / 60), (curpos % 60), (songlen / 60), (songlen % 60));
		} else {
			char tmpbuf[1024];
			strcpy(tmpbuf, GetCurrentTrack(hwWinamp3));
			if(strlen(tmpbuf)){
				tmpbuf[strlen(tmpbuf)-1] = ' ';
				sprintf(buf, "[%s%i:%.2i)]", tmpbuf, (curpos / 60), (curpos % 60));
			} else {
				sprintf(buf, "[nothing is being played]", tmpbuf, (curpos / 60), (curpos % 60));
			}
		}
		gSuperHUD.DrawHudString(iwinampx, iwinampy, barR, barG, barB, buf);
	}
}

void CWinamp::Cmd_Winamp(void)
{
	HWND hwWinamp = FindWindow("Winamp v1.x",NULL);
	if(!hwWinamp)
	{
		gEngfuncs.pfnConsolePrint("[sparky] winamp is not currently running\n");
		return;
	}

	if(gEngfuncs.Cmd_Argc() < 2)
	{
		char buf[4096];
		gEngfuncs.pfnConsolePrint("[sparky] valid bwinamp commands: play stop pause next prev display volup voldown spam shuffle\n");
		sprintf(buf, "[sparky] currently playing: %s\n", GetCurrentTrack(hwWinamp));
		gEngfuncs.pfnConsolePrint(buf);
		return;
	}

	if(!strcmp(gEngfuncs.Cmd_Argv(1), "play"))
	{
		::SendMessage(hwWinamp, WM_COMMAND, WINAMP_BUTTON2, 0);
		bWinampPlaying = 1;
	}
	if(!strcmp(gEngfuncs.Cmd_Argv(1), "stop"))
	{
		::SendMessage(hwWinamp, WM_COMMAND, WINAMP_BUTTON4, 0);
		bWinampPlaying = 0;
	}
	if(!strcmp(gEngfuncs.Cmd_Argv(1), "pause"))
	{
		::SendMessage(hwWinamp, WM_COMMAND, WINAMP_BUTTON3, 0);
		bWinampPlaying = 0;
	}
	if(!strcmp(gEngfuncs.Cmd_Argv(1), "next"))
		::SendMessage(hwWinamp, WM_COMMAND, WINAMP_BUTTON5, 0);
	if(!strcmp(gEngfuncs.Cmd_Argv(1), "prev"))
		::SendMessage(hwWinamp, WM_COMMAND, WINAMP_BUTTON1, 0);
	if(!strcmp(gEngfuncs.Cmd_Argv(1), "previous"))
		::SendMessage(hwWinamp, WM_COMMAND, WINAMP_BUTTON1, 0);
	if(!strcmp(gEngfuncs.Cmd_Argv(1), "volup"))
	{
		for (int i=0; i<5; i++)
			::SendMessage(hwWinamp, WM_COMMAND, WINAMP_VOLUMEUP, 0);
	}
	if(!strcmp(gEngfuncs.Cmd_Argv(1), "voldown"))
	{
		for (int i=0; i<5; i++)
			::SendMessage(hwWinamp, WM_COMMAND, WINAMP_VOLUMEDOWN, 0);
	}
	if(!strcmp(gEngfuncs.Cmd_Argv(1), "display"))
		bShowWinampDisp = bShowWinampDisp ? 0 : 1;
	if(!strcmp(gEngfuncs.Cmd_Argv(1), "spam"))
	{
		char buf[4096];
		HWND hwWinamp = FindWindow("Winamp v1.x",NULL);
		if(hwWinamp)
		{
			if(fLastTriggerTime > gEngfuncs.GetClientTime())
				fLastTriggerTime = gEngfuncs.GetClientTime();
			if((gEngfuncs.GetClientTime() - fLastTriggerTime) > 2)
			{
				fLastTriggerTime = gEngfuncs.GetClientTime();
				sprintf(buf, "say \"%s [sparky]\";", GetCurrentTrack(hwWinamp));
				gEngfuncs.pfnClientCmd(buf);
			}
		}
	}
	if(!strcmp(gEngfuncs.Cmd_Argv(1), "shuffle"))
	{
		int ShuffleVal = ::SendMessage(hwWinamp, WM_WA_IPC, 0, IPC_GET_SHUFFLE);

		if (ShuffleVal == 1)
		{
			::SendMessage(hwWinamp, WM_WA_IPC, 0, IPC_SET_SHUFFLE);
			gEngfuncs.pfnConsolePrint("[sparky] winamp shuffle : off\n");
		}
		else
		{
			::SendMessage(hwWinamp, WM_WA_IPC, 1, IPC_SET_SHUFFLE);
			gEngfuncs.pfnConsolePrint("[sparky] winamp shuffle : on\n");
		}
	}
}

char *CWinamp::GetCurrentTrack(HWND hwWinamp)
{
	static char buf[4096];
	char this_title[2048];
	char *p;
	HWND hwWinamp3 = FindWindow("STUDIO",NULL);
	if(hwWinamp3){
		GetWindowText(hwWinamp3,this_title,sizeof(this_title));
		sprintf(buf, "%s", this_title);
		return buf;
	} else {
		GetWindowText(hwWinamp,this_title,sizeof(this_title));
	}
	p = this_title+strlen(this_title)-8;
	while (p >= this_title)
	{
		if (!strnicmp(p,"- Winamp",8)) break;
		p--;
	}
	if (p >= this_title) p--;
	while (p >= this_title && *p == ' ') p--;
	*++p=0;

	sprintf(buf, "%s", this_title);
	return buf;
}
	
CWinamp::CWinamp(void)
{
	fLastTriggerTime = 0;
	bShowWinampDisp = 0;
	bMicWinampMuteOn = 0;
	bWinampPlaying = 1;
}

bool CWinamp::Pre_HUD_VoiceStatus (int entindex, qboolean bTalking)
{
	if ((bMicWinampMuteOn) && (bTalking) && (bWinampPlaying))
	{
		HWND hwWinamp = FindWindow("Winamp v1.x",NULL);

		if(hwWinamp)
		{
			::SendMessage(hwWinamp, WM_COMMAND, WINAMP_BUTTON3, 0);
			bWinampPlaying = 0;
		}
	}

	return true;
}

void CWinamp::Post_HUD_VoiceStatus (int entindex, qboolean bTalking)
{
	if ((bMicWinampMuteOn) && (!bTalking) && (!bWinampPlaying))
	{
		HWND hwWinamp = FindWindow("Winamp v1.x",NULL);

		if(hwWinamp)
		{
			::SendMessage(hwWinamp, WM_COMMAND, WINAMP_BUTTON2, 0);
			bWinampPlaying = 1;
		}
	}
}

void CWinamp::Cmd_ToggleMicWinampMute (void)
{
	bMicWinampMuteOn = bMicWinampMuteOn ? 0 : 1;

	if (bMicWinampMuteOn)
		gEngfuncs.pfnConsolePrint("[sparky] Mic Winamp Mute ON\n");
	else
		gEngfuncs.pfnConsolePrint("[sparky] Mic Winamp Mute OFF\n");
}

