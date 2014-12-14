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
#include "mirc.h"

/*Global*/
CmIRC gmIRC;

unsigned long	msg;
HWND	hwndIrcInput = NULL; 
LRESULT CALLBACK WndProc(HWND hwnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);

DECLARE_COMMAND_2(gmIRC, IRCToggle);
DECLARE_COMMAND_2(gmIRC, IRCInput);
DECLARE_COMMAND_2(gmIRC, IRCRawInput);
DECLARE_COMMAND_2(gmIRC, IRCAnnounceIRCToGame);
DECLARE_COMMAND_2(gmIRC, IRCAnnounceGameToIRC);
DECLARE_COMMAND_2(gmIRC, IRCGlobalSay);
DECLARE_COMMAND_2(gmIRC, IRCNextChannel);
DECLARE_COMMAND_2(gmIRC, IRCPrevChannel);
DECLARE_COMMAND_2(gmIRC, IRCactiveColor);
DECLARE_COMMAND_2(gmIRC, IRCinactiveColor);
DECLARE_COMMAND_2(gmIRC, IRCbgColor);

bool CmIRC::Pre_HUD_Init (void)
{
	HOOK_COMMAND_2("bmircdisplay", IRCToggle);
	HOOK_COMMAND_2("bmircinput", IRCInput);
	HOOK_COMMAND_2("bmircrawinput", IRCRawInput);
	HOOK_COMMAND_2("bmirctogame", IRCAnnounceIRCToGame);
	HOOK_COMMAND_2("bgametomirc", IRCAnnounceGameToIRC);
	HOOK_COMMAND_2("bmircglobalsay", IRCGlobalSay);
	HOOK_COMMAND_2("bmircnextchannel", IRCNextChannel);
	HOOK_COMMAND_2("bmircprevchannel", IRCPrevChannel);
	HOOK_COMMAND_2("bmircactivecolor", IRCactiveColor);
	HOOK_COMMAND_2("bmircinactivecolor", IRCinactiveColor);
	HOOK_COMMAND_2("bmircbgcolor", IRCbgColor);

	return true;
}

void CmIRC::Post_HUD_Init (void)
{
	// create the window
	WNDCLASS wc; 

    // Register a new window class  
    ZeroMemory(&wc, sizeof(WNDCLASS)); 
    wc.style			= CS_HREDRAW | CS_VREDRAW; 
    wc.lpfnWndProc		= (WNDPROC) WndProc; 
    wc.hInstance        = gDllHandle; 
    wc.hbrBackground    = (HBRUSH) GetStockObject(WHITE_BRUSH); 
    wc.lpszClassName    = "[sparky] IRC Window"; 
    RegisterClass(&wc); 

    // Create a new window of the specified class 
    hwndIrcInput = CreateWindow("[sparky] IRC Window",  
        "[sparky] IRC Window", WS_POPUP, 
        CW_USEDEFAULT, CW_USEDEFAULT, 240, 100,  
        NULL,//mWnd,  
        NULL,  
        gDllHandle,  
        NULL);

	msg = RegisterWindowMessage("IRC MESSAGE");

	if (!hwndIrcInput)
		MessageBox(NULL, "Unable to create message window. Game end detection won't work.", "IRC Launcher Error", MB_OK);

	// to make the dll get the new sparky process id
	sendCommand("/dll -u sparkyirc.dll");
}


CmIRC::CmIRC(void)
{
	bIRCDisplay = true;
	iNotification = 0;
	bIRCToGame = false;
	bGameToIRC = false;
	bGlobalSay = true;
	strcpy(sCurChannel, "");

	bgColor = Vector(0.8, 0.8, 0.8);
	activeColor = Vector(0.0, 0.0, 1.0);
	inactiveColor = Vector(.75, .75, 1.0);
	bgBrightness = 20;
}

CmIRC::~CmIRC(void)
{
/*	for (int i=0;i<sChatInfo.size();i++) {
		if (sChatInfo[i])
			delete sChatInfo[i];
	}
*/
}


bool CmIRC::Pre_HUD_Redraw (float flTime, int intermission)
{
	return true;
}


/*
	0 = (255,255,255) 
	1 = (0,0,0) 
	2 = (0,0,125) 
	3 = (0,125,0) 
	4 = (255,0,0) 
	5 = (125,0,0) 
	6 = (125,0,125) 
	7 = (255,125,0) 
	8 = (255,255,0) 
	9 = (0,255,0) 
	10 = (0,125,125) 
	11 = (0,255,255) 
	12 = (0,0,255) 
	13 = (255,0,255) 
	14 = (125,125,125) 
	15 = (200,200,200)
*/
//						0		1		2		3		4		5		6		7		8		9		10		11		12		13		14		15		
float ircColor_r[16] = {1.0,	0.0,	0.0,	0.0,	1.0,	0.5,	0.5,	1.0,	1.0,	0.0,	0.0,	0.0,	0.0,	1.0,	0.5,	.75};
float ircColor_g[16] = {1.0,	0.0,	0.0,	0.5,	0.0,	0.0,	0.0,	0.5,	1.0,	1.0,	0.5,	1.0,	0.0,	0.0,	0.5,	.75};
float ircColor_b[16] = {1.0,	0.0,	0.5,	0.0,	0.0,	0.0,	0.5,	0.0,	0.0,	0.0,	0.5,	1.0,	1.0,	1.0,	0.5,	.75};

void drawColorString(char *s, int x, int y) {
	char *str, *save;
	str = save = new char[strlen(s)+1];
	strcpy(str, s);

	char *temp;
	int color = 2;
	char colorstr[3] = "00";

	int w, h;
	do {
		temp = strchr(str, '\x03');
		if (temp)
			*temp = '\0';

		gEngfuncs.pfnDrawSetTextColor(ircColor_r[color], ircColor_g[color], ircColor_b[color]);
		gEngfuncs.pfnDrawConsoleString(x, y, str);
		gEngfuncs.pfnDrawConsoleStringLen(str, &w, &h);
		x += w;

		if (temp) {
			temp++;
			if (*temp>='0' && *temp<='9')
				colorstr[0] = *(temp++);
			else
				colorstr[0] = '\0';
			if ((colorstr[0]=='1' || colorstr[0]=='0') && *temp>='0' && *temp<='9')
				colorstr[1] = *(temp++);
			else
				colorstr[1] = '\0';
			colorstr[2] = '\0';
			color = atoi(colorstr);

			str = temp;
		} else {
			str = NULL;
		}
	} while (str);

	delete save;
}

void CmIRC::Post_HUD_Redraw (float flTime, int intermission)
{
	if (!bIRCDisplay)
		return;

	int maxmessages = 30;

	gEngfuncs.pfnFillRGBA(40, 40, gSuperHUD.m_scrinfo.iWidth-90, 70+maxmessages*15,
		bgColor.x, bgColor.y, bgColor.z, bgBrightness);
	
	//char buf[30];
	// display channel list
	int x = 50;
	int w, h;
	for (int c=0;c<sChannels.size();c++) {
		if (!strcmp(sChannels[c]->channelname, sCurChannel)) {
			gEngfuncs.pfnDrawSetTextColor(activeColor.x, activeColor.y, activeColor.z);
		} else {
			gEngfuncs.pfnDrawSetTextColor(inactiveColor.x, inactiveColor.y, inactiveColor.z);
		}
		//sprintf(buf, "%s-%d", sChannels[c]->channelname, sChannels[c]->messages.size());
		gEngfuncs.pfnDrawConsoleString(x, 25, sChannels[c]->channelname);

		gEngfuncs.pfnDrawConsoleStringLen(sChannels[c]->channelname, &w, &h);
		x += w + 15;
	}
	
	char *tmp;
	for (c=0;c<sChannels.size();c++) {
		if (!strcmp(sChannels[c]->channelname, sCurChannel)) {
			while (sChannels[c]->messages.size()>maxmessages) {
				tmp = sChannels[c]->messages[0];
				sChannels[c]->messages.erase(sChannels[c]->messages.begin());
				//if (tmp!=NULL) delete tmp;
			}

			for (int i=0;i<sChannels[c]->messages.size();i++) {
				//gEngfuncs.pfnDrawSetTextColor(gSuperHUD.HUDColor.x/255, gSuperHUD.HUDColor.y/255, gSuperHUD.HUDColor.z/255);
				//gEngfuncs.pfnDrawConsoleString(50, 50+i*15, sChannels[c]->messages[i]);
				drawColorString(sChannels[c]->messages[i], 50, 50+i*15);
			}
		}
	}
}

bool CmIRC::Pre_Hook_ConsoleCommand (char *pszString)
{
	if((pszString[0] == '.') && (strlen(pszString) > 1))
	{
		sendCommand(&pszString[1]);
		return false;
	}
	return true;
}

void CmIRC::Cmd_IRCInput(void)
{
	static char buf[1024];
	
	if(gEngfuncs.Cmd_Argc() > 1)
	{
		sprintf(buf, "/msg %s ", sCurChannel);
		for(int i = 1; i < gEngfuncs.Cmd_Argc(); i++)
		{
			if(strlen(buf) + strlen(gEngfuncs.Cmd_Argv(i)) < 1000)
			{
				if(i > 1) strcat(buf, " ");
				strcat(buf, gEngfuncs.Cmd_Argv(i));
			}
		}
		//strcat(buf, "\n");
		if (strlen(buf) <= 6+strlen(sCurChannel))
			return;
		
		sendCommand(buf);

		char *tmp = new char[strlen(buf)+1];
		strcpy(tmp, buf+6+strlen(sCurChannel));
		for (int c=0;c<gmIRC.sChannels.size();c++) {
			if (!strcmp(gmIRC.sChannels[c]->channelname, sCurChannel)) {
				gmIRC.sChannels[c]->messages.push_back(tmp);
			}
		}
	}
}
void CmIRC::Cmd_IRCRawInput(void)
{
	static char buf[1024];
	
	if(gEngfuncs.Cmd_Argc() > 1)
	{
		sprintf(buf, "", sCurChannel);
		for(int i = 1; i < gEngfuncs.Cmd_Argc(); i++)
		{
			if(strlen(buf) + strlen(gEngfuncs.Cmd_Argv(i)) < 1000)
			{
				if(i > 1) strcat(buf, " ");
				strcat(buf, gEngfuncs.Cmd_Argv(i));
			}
		}
		//strcat(buf, "\n");
		sendCommand(buf);
	}
}

void CmIRC::Cmd_IRCToggle(void)
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


void CmIRC::Cmd_IRCbgColor(void)
{
	if(gEngfuncs.Cmd_Argc() > 4)
	{
		bgColor.x = atoi(gEngfuncs.Cmd_Argv(1));
		if(bgColor.x > 255) bgColor.x = 255;
		bgColor.y = atoi(gEngfuncs.Cmd_Argv(2));
		if(bgColor.y > 255) bgColor.y = 255;
		bgColor.z = atoi(gEngfuncs.Cmd_Argv(3));
		if(bgColor.z > 255) bgColor.z = 255;
		bgBrightness = atoi(gEngfuncs.Cmd_Argv(4));
		if (bgBrightness > 255) bgBrightness = 255;
	}
}
void CmIRC::Cmd_IRCactiveColor(void)
{
	if(gEngfuncs.Cmd_Argc() > 3)
	{
		activeColor.x = atoi(gEngfuncs.Cmd_Argv(1));
		if(activeColor.x > 255) activeColor.x = 255;
		activeColor.y = atoi(gEngfuncs.Cmd_Argv(2));
		if(activeColor.y > 255) activeColor.y = 255;
		activeColor.z = atoi(gEngfuncs.Cmd_Argv(3));
		if(activeColor.z > 255) activeColor.z = 255;

		activeColor.x /= 255;
		activeColor.y /= 255;
		activeColor.z /= 255;
	}
}
void CmIRC::Cmd_IRCinactiveColor(void)
{
	if(gEngfuncs.Cmd_Argc() > 3)
	{
		inactiveColor.x = atoi(gEngfuncs.Cmd_Argv(1));
		if(inactiveColor.x > 255) inactiveColor.x = 255;
		inactiveColor.y = atoi(gEngfuncs.Cmd_Argv(2));
		if(inactiveColor.y > 255) inactiveColor.y = 255;
		inactiveColor.z = atoi(gEngfuncs.Cmd_Argv(3));
		if(inactiveColor.z > 255) inactiveColor.z = 255;
		
		inactiveColor.x /= 255;
		inactiveColor.y /= 255;
		inactiveColor.z /= 255;
	}
}

void CmIRC::Cmd_IRCAnnounceIRCToGame(void)
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

void CmIRC::Cmd_IRCAnnounceGameToIRC(void)
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

void CmIRC::Cmd_IRCGlobalSay(void)
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

void CmIRC::Cmd_IRCNextChannel(void)
{
	int found = -1;
	for (int c=0;c<sChannels.size();c++) {
		
		if (!strcmp(sChannels[c]->channelname, sCurChannel)) {
			found = c;
		}
	}
	if (found != -1) {
		strcpy(sCurChannel, sChannels[ (found+1)%sChannels.size() ]->channelname);
	} else if (sChannels.size()) {
		strcpy(sCurChannel, sChannels[0]->channelname);
	}

	char buf[255];
	sprintf(buf, "Changing channel to: %s", sCurChannel);
	gSuperHUD.WriteHUDChannel(HC_IRC_GLOBALSAY, 1, buf);
}

void CmIRC::Cmd_IRCPrevChannel(void)
{
	int found = -1;
	for (int c=0;c<sChannels.size();c++) {
		
		if (!strcmp(sChannels[c]->channelname, sCurChannel)) {
			found = c;
		}
	}
	if (found != -1) {
		strcpy(sCurChannel, sChannels[ (found+sChannels.size()-1)%sChannels.size() ]->channelname);
	} else if (sChannels.size()) {
		strcpy(sCurChannel, sChannels[0]->channelname);
	}

	char buf[255];
	sprintf(buf, "Changing channel to: %s", sCurChannel);
	gSuperHUD.WriteHUDChannel(HC_IRC_GLOBALSAY, 1, buf);
}

bool CmIRC::Pre_Hook_TextMsg (const char *pszString)
{
	return true;
}

void CmIRC::Post_Hook_TextMsg (const char *pszString)
{
}

bool CmIRC::Pre_Hook_SayText (const char *pszString)
{
	return true;
}

void CmIRC::Post_Hook_SayText (const char *pszString)
{
	char buf[8192];
	if(!bGameToIRC) return;
	if(strlen(pszString) > 12)
		if(strstr(pszString, "[sparkyIRC] ")) return;
	sprintf(buf, "/msg %s [sparkyIRC] %s", sCurChannel, pszString);
	sendCommand(buf);
}

bool CmIRC::Pre_Hook_DeathMsg (int killer, int victim, const char *pszWeapon)
{
	return true;
}

void CmIRC::Post_Hook_DeathMsg (int killer, int victim, const char *pszWeapon)
{
}

void sendCommand(char *str)
{
	static HWND mirchWnd = NULL;
	if (!mirchWnd)
		mirchWnd = FindWindow("mIRC", NULL);

	if (!mirchWnd)
		return;

	HANDLE	hFileMap;
	LPSTR	mData;
	hFileMap = CreateFileMapping(INVALID_HANDLE_VALUE,0,PAGE_READWRITE,0,4096,"mIRC");     
	mData = (LPSTR)MapViewOfFile(hFileMap,FILE_MAP_ALL_ACCESS,0,0,0);
	wsprintf(mData,str);
	UnmapViewOfFile(mData);
	CloseHandle(hFileMap);

	SendMessage(mirchWnd, WM_USER + 200,1,0);          //evaluate it
}



LRESULT CALLBACK WndProc(HWND hwnd, UINT uiMsg, WPARAM wParam, LPARAM lParam) 
{ 
    PAINTSTRUCT ps; 

	char buf[4096];

	if (uiMsg == msg) {
		
		HANDLE	hFileMap;
		LPSTR	mData;
		hFileMap = OpenFileMapping(FILE_MAP_ALL_ACCESS, true, "sparky-irc");
		mData = (LPSTR)MapViewOfFile(hFileMap,FILE_MAP_ALL_ACCESS,0,0,0);
		if (!mData) return 0;
		strcpy(buf, mData);       //copy “$alias(Hello mIRC!)” into the file mapping
		UnmapViewOfFile(mData);
		CloseHandle(hFileMap);

		char *chan = buf;
		char *message = buf;
		while (*message!=' ' && *message!='\0') message++;
		if (*message==' ') *message = '\0';
		message+=3;

		if (strlen(buf) && strlen(buf)<200) {
			//buf[size-1] = '\0';
			char *tmp = new char[strlen(message)];
			strcpy(tmp, message);
			//gSuperHUD.WriteHUDChannel(HC_IRC_TO_GAME, 1, buf);
			bool found = false;
			for (int c=0;c<gmIRC.sChannels.size();c++) {
				if (!strcmp(gmIRC.sChannels[c]->channelname, chan)) {
					gmIRC.sChannels[c]->messages.push_back(tmp);
					found = true;
				}
			}
			if (!found) {
				Channel_t *tempchan = new Channel_t;
				strcpy(tempchan->channelname, chan);
				tempchan->messages.push_back(tmp);
				gmIRC.sChannels.push_back(tempchan);
			}
		}
		return 0;
	} 

    switch (uiMsg)  
    {
		case WM_CREATE:
			sendCommand("/grabchans");
			break;
        case WM_PAINT: 
            BeginPaint(hwnd, &ps); 
            EndPaint(hwnd, &ps); 
            break;  

        default: 
            return DefWindowProc(hwnd, uiMsg, wParam, lParam); 
   } 


	return 0; 
} 