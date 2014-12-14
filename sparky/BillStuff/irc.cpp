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
#pragma warning (disable:4018) // float to int loss of data

#include "..\client.h"
#include "util.h"

#include "hook.h"
#include "superhud.h"
#include "playerinfo.h"
#include "irc.h"
#include "fb.h"
#include "rulesmanager.h"
#include "help.h"

/*Global*/
CIRC gIRC;

DECLARE_COMMAND_2(gIRC, IRCToggle);
DECLARE_COMMAND_2(gIRC, IRCInput);
DECLARE_COMMAND_2(gIRC, IRCAnnounceIRCToGame);
DECLARE_COMMAND_2(gIRC, IRCAnnounceGameToIRC);
DECLARE_COMMAND_2(gIRC, IRCGlobalSay);

bool CIRC::Pre_HUD_Init (void)
{
	HOOK_COMMAND_2("bircdisplay", IRCToggle);
	gHelp.addCommand("bircdisplay", "Toggles display of IRC info in the chat area (default) and in the console only.\nUsage: bircdisplay");
	HOOK_COMMAND_2("bircinput", IRCInput);
	gHelp.addCommand("bircinput", "Relays text to the IRC client.\nUsage: bircinput <text>");
	HOOK_COMMAND_2("birctogame", IRCAnnounceIRCToGame);
	gHelp.addCommand("birctogame", "Toggles relay of IRC events to TFC.\nUsage: birctogame");
	HOOK_COMMAND_2("bgametoirc", IRCAnnounceGameToIRC);
	gHelp.addCommand("bgametoirc", "Toggles relay of game events to the IRC client.\nUsage: bgametoirc");
	HOOK_COMMAND_2("bircglobalsay", IRCGlobalSay);
	gHelp.addCommand("bircglobalsay", "Toggles relay of all \"say\" messages to the IRC client.\nUsage: bircglobalsay");

	return true;
}

void CIRC::Post_HUD_Init (void)
{
	hIRCPlugin = LoadLibrary("sparkyutils\\plugin_irc.dll");
	if(hIRCPlugin)
	{
		bPluginLoaded = true;
		pfnIrcGetFirstUser = (char*(*)(void))GetProcAddress(hIRCPlugin, "IrcGetFirstUser");
		pfnIrcGetNextUser = (char*(*)(void))GetProcAddress(hIRCPlugin, "IrcGetNextUser");
		pfnIrcGetBottomLine = (char*(*)(void))GetProcAddress(hIRCPlugin, "IrcGetBottomLine");
		pfnIrcGetTopLine = (char*(*)(void))GetProcAddress(hIRCPlugin, "IrcGetTopLine");
		pfnIrcSubmit = (void(*)(char*))GetProcAddress(hIRCPlugin, "IrcSubmit");
		pfnIrcInitialize = (int(*)(void(*)(char*),void(*)(int)))GetProcAddress(hIRCPlugin, "IrcInitialize");
	}

	if(bPluginLoaded)
	{
		char szInitialName[32];
		sprintf(szInitialName, "/nick sparky%d", gEngfuncs.pfnRandomLong(100000, 999999));
		pfnIrcInitialize(gIRCEchoCallBack, gIRCNotify);
		gEngfuncs.pfnConsolePrint("[sparky] IRC plugin loaded\n");
		pfnIrcSubmit(szInitialName);
		//pfnIrcSubmit("/server irc.gamesnet.net");
	}
}


CIRC::CIRC(void)
{
	bPluginLoaded = false;
	bIRCDisplay = true;
	iNotification = 0;
	bIRCToGame = false;
	bGameToIRC = false;
	bGlobalSay = true;
}

CIRC::~CIRC(void)
{
	if(hIRCPlugin)
	{
		pfnIrcSubmit("/quit");
		FreeLibrary(hIRCPlugin);
	}
}


bool CIRC::Pre_HUD_Redraw (float flTime, int intermission)
{
	return true;
}

void CIRC::Post_HUD_Redraw (float flTime, int intermission)
{
	if(!bPluginLoaded) return;
	if(!bIRCDisplay) return;
	char buf[4096];
	int i = 2;
	sprintf(buf, "%s", pfnIrcGetTopLine());
	if(strlen(buf) < 3) return;
	if(buf[2] != '#') return;
	while((buf[i] != ' ') && (i < strlen(buf)))
	{
		i++;
	}
	buf[i] = '\0';
	gSuperHUD.WriteHUDChannel(HC_IRC_CHANNEL, 1, buf+2);
}

bool CIRC::Pre_Hook_ConsoleCommand (char *pszString)
{
	if(!bPluginLoaded) return true;

	if((pszString[0] == '.') && (strlen(pszString) > 1))
	{
		// Beg: Added by Mulchman
		// Check for %scb/r/g/y variables and shit
		char tmpbuf[256];
		if (strstr(pszString, "%scb"))
		{
			if(gFB.bScoreBlue)
			{
				sprintf(tmpbuf, "%d", gFB.teamscores[gFB.FindTeamIndex('b')].points);
				sprintf(pszString, "%s", StringSearchReplace("%scb", tmpbuf, pszString));
			}
			else
				sprintf(pszString, "%s", StringSearchReplace("%scb", "null", pszString));
		}
		if(strstr(pszString, "%scr"))
		{
			if(gFB.bScoreRed)
			{
				sprintf(tmpbuf, "%d", gFB.teamscores[gFB.FindTeamIndex('r')].points);
				sprintf(pszString, "%s", StringSearchReplace("%scr", tmpbuf, pszString));				
			}
			else
				sprintf(pszString, "%s", StringSearchReplace("%scr", "null", pszString));
		}
		if(strstr(pszString, "%scy"))
		{
			if(gFB.bScoreYellow)
			{
				sprintf(tmpbuf, "%d", gFB.teamscores[gFB.FindTeamIndex('y')].points);
				sprintf(pszString, "%s", StringSearchReplace("%scy", tmpbuf, pszString));
			}
			else
				sprintf(pszString, "%s", StringSearchReplace("%scy", "null", pszString));
		}
		if(strstr(pszString, "%scg"))
		{
			if(gFB.bScoreGreen)
			{
				sprintf(tmpbuf, "%d", gFB.teamscores[gFB.FindTeamIndex('g')].points);
				sprintf(pszString, "%s", StringSearchReplace("%scg", tmpbuf, pszString));
			}
			else
				sprintf(pszString, "%s", StringSearchReplace("%scg", "null", pszString));
		}
		if(strstr(pszString, "%t"))
		{
			gRulesManager.fLastTriggerTime = gEngfuncs.GetClientTime();
			char timeleftbuf[256];
			if(gRulesManager.iTimelimit)
				sprintf(timeleftbuf, "%02d:%02d", (int)((gRulesManager.iTimelimit - gEngfuncs.GetClientTime())/60), (int)fmod(gRulesManager.iTimelimit - gEngfuncs.GetClientTime(), 60));
			else
				sprintf(timeleftbuf, "00:00 (no limit)");
			sprintf(pszString, "%s", StringSearchReplace("%t", timeleftbuf, pszString));
		}
		// End: Added by Mulchman

		pfnIrcSubmit(&pszString[1]);
		return false;
	}
	return true;
}

void CIRC::Cmd_IRCInput(void)
{
	static char buf[1024];
	
	if(!bPluginLoaded) return;

	if(gEngfuncs.Cmd_Argc() > 1)
	{
		sprintf(buf, "\0");
		for(int i = 1; i < gEngfuncs.Cmd_Argc(); i++)
		{
			if(strlen(buf) + strlen(gEngfuncs.Cmd_Argv(i)) < 1000)
			{
				if(i > 1) strcat(buf, " ");
				strcat(buf, gEngfuncs.Cmd_Argv(i));
			}
		}
		strcat(buf, "\n");

		// Beg: Added by Mulchman
		// Check for %scb/r/g/y variables and shit
		char tmpbuf[256];
		if (strstr(buf, "%scb"))
		{
			if(gFB.bScoreBlue)
			{
				sprintf(tmpbuf, "%d", gFB.teamscores[gFB.FindTeamIndex('b')].points);
				sprintf(buf, "%s", StringSearchReplace("%scb", tmpbuf, buf));
			}
			else
				sprintf(buf, "%s", StringSearchReplace("%scb", "null", buf));
		}
		if(strstr(buf, "%scr"))
		{
			if(gFB.bScoreRed)
			{
				sprintf(tmpbuf, "%d", gFB.teamscores[gFB.FindTeamIndex('r')].points);
				sprintf(buf, "%s", StringSearchReplace("%scr", tmpbuf, buf));				
			}
			else
				sprintf(buf, "%s", StringSearchReplace("%scr", "null", buf));
		}
		if(strstr(buf, "%scy"))
		{
			if(gFB.bScoreYellow)
			{
				sprintf(tmpbuf, "%d", gFB.teamscores[gFB.FindTeamIndex('y')].points);
				sprintf(buf, "%s", StringSearchReplace("%scy", tmpbuf, buf));
			}
			else
				sprintf(buf, "%s", StringSearchReplace("%scy", "null", buf));
		}
		if(strstr(buf, "%scg"))
		{
			if(gFB.bScoreGreen)
			{
				sprintf(tmpbuf, "%d", gFB.teamscores[gFB.FindTeamIndex('g')].points);
				sprintf(buf, "%s", StringSearchReplace("%scg", tmpbuf, buf));
			}
			else
				sprintf(buf, "%s", StringSearchReplace("%scg", "null", buf));
		}
		if(strstr(buf, "%t"))
		{
			gRulesManager.fLastTriggerTime = gEngfuncs.GetClientTime();
			char timeleftbuf[256];
			if(gRulesManager.iTimelimit)
				sprintf(timeleftbuf, "%02d:%02d", (int)((gRulesManager.iTimelimit - gEngfuncs.GetClientTime())/60), (int)fmod(gRulesManager.iTimelimit - gEngfuncs.GetClientTime(), 60));
			else
				sprintf(timeleftbuf, "00:00 (no limit)");
			sprintf(buf, "%s", StringSearchReplace("%t", timeleftbuf, buf));
		}
		// End: Added by Mulchman

		pfnIrcSubmit(buf);
	}
}

void CIRC::Cmd_IRCToggle(void)
{
	if(bIRCDisplay)
	{
		gSuperHUD.WriteHUDChannel(HC_IRC_DISPLAY, 1, "IRC display - off");
		bIRCDisplay = false;
	}
	else
	{
		gSuperHUD.WriteHUDChannel(HC_IRC_DISPLAY, 1, "IRC display - on");
		bIRCDisplay = true;
	}
}

void CIRC::Cmd_IRCAnnounceIRCToGame(void)
{
	if(bIRCToGame)
	{
		gSuperHUD.WriteHUDChannel(HC_GAME_TO_IRC, 1, "IRC to game - off");
		bIRCToGame = false;
	}
	else
	{
		gSuperHUD.WriteHUDChannel(HC_GAME_TO_IRC, 1, "IRC to game - on");
		bIRCToGame = true;
	}
}

void CIRC::Cmd_IRCAnnounceGameToIRC(void)
{
	if(bGameToIRC)
	{
		gSuperHUD.WriteHUDChannel(HC_IRC_TO_GAME, 1, "game to IRC - off");
		bGameToIRC = false;
	}
	else
	{
		gSuperHUD.WriteHUDChannel(HC_IRC_TO_GAME, 1, "game to IRC - on");
		bGameToIRC = true;
	}
}

void CIRC::Cmd_IRCGlobalSay(void)
{
	if(bGlobalSay)
	{
		gSuperHUD.WriteHUDChannel(HC_IRC_GLOBALSAY, 1, "IRC to game - say_team");
		bGlobalSay = false;
	}
	else
	{
		gSuperHUD.WriteHUDChannel(HC_IRC_GLOBALSAY, 1, "IRC to game - say");
		bGlobalSay = true;
	}
}

void gIRCEchoCallBack(char *szText)
{
	char buf[8192], buf2[8192];	// better safe than sorry
	int i, j;
	if(strlen(szText) > 12)
		if(strstr(szText, "[sparkyIRC] ")) return;
	sprintf(buf, "[sparkyIRC] ");
	j = strlen(buf);
	//sprintf(buf, "%s\n", szText);
	for(i = 0; i < strlen(szText); i++)
	{
		if(szText[i] == '&')
		{
			i++;	// skip next char too
			continue;
		}
		if(szText[i] == 0x03)
		{
			continue;
		}
		buf[j] = szText[i];
		j++;
	}
	buf[j] = '\0';
	strcat(buf, "\n");
	if(!gIRC.bIRCDisplay || gIRC.bIRCToGame)
	{
		if(gIRC.bIRCToGame)
		{
			char ccbuf[8192];
			if(gIRC.bGlobalSay)
				sprintf(ccbuf, "say %s", buf);
			else
				sprintf(ccbuf, "say_team %s", buf);
			gEngfuncs.pfnClientCmd(ccbuf);
		}
		gEngfuncs.pfnConsolePrint(buf);
	}
	else
	{
		sprintf(buf2, "%c%c%s", 0xff, 0x02, buf);
		hook_saytext("SayText", strlen(buf2), buf2);
	}
}

void gIRCNotify(int iNotification)
{
	gIRC.iNotification = iNotification;
	if(iNotification == 3)
		gEngfuncs.pfnClientCmd("btrigger_irc_connect");
}

bool CIRC::Pre_Hook_TextMsg (const char *pszString)
{
	return true;
}

void CIRC::Post_Hook_TextMsg (const char *pszString)
{
}

bool CIRC::Pre_Hook_SayText (const char *pszString)
{
	return true;
}

void CIRC::Post_Hook_SayText (const char *pszString)
{
	char buf[8192];
	if(!bGameToIRC) return;
	if(strlen(pszString) > 12)
		if(strstr(pszString, "[sparkyIRC] ")) return;
	sprintf(buf, "%c[sparkyIRC] %c%s", 0x02, 0x02, pszString);
	pfnIrcSubmit(buf);
}

bool CIRC::Pre_Hook_DeathMsg (int killer, int victim, const char *pszWeapon)
{
	return true;
}

void CIRC::Post_Hook_DeathMsg (int killer, int victim, const char *pszWeapon)
{
}
