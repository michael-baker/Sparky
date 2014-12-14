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


#ifndef MIRC_H
#define MIRC_H

#include <vector>

struct Channel_t {
	char channelname[255];
	std::vector<char *>	messages;
};


class CmIRC : CBaseHook
{
	public:

	virtual bool Pre_HUD_Init (void);
	virtual void Post_HUD_Init (void);
	virtual bool Pre_HUD_Redraw (float flTime, int intermission);
	virtual void Post_HUD_Redraw (float flTime, int intermission);
	virtual bool Pre_Hook_ConsoleCommand (char *pszString);
	virtual bool Pre_Hook_TextMsg (const char *pszString);
	virtual void Post_Hook_TextMsg (const char *pszString);
	virtual bool Pre_Hook_SayText (const char *pszString);
	virtual void Post_Hook_SayText (const char *pszString);
	virtual bool Pre_Hook_DeathMsg (int killer, int victim, const char *pszWeapon);
	virtual void Post_Hook_DeathMsg (int killer, int victim, const char *pszWeapon);

	CmIRC(void);
	~CmIRC(void);

	void Cmd_IRCInput(void);
	void Cmd_IRCRawInput(void);
	void Cmd_IRCToggle(void);
	void Cmd_IRCAnnounceIRCToGame(void);
	void Cmd_IRCAnnounceGameToIRC(void);
	void Cmd_IRCGlobalSay(void);
	void Cmd_IRCNextChannel(void);
	void Cmd_IRCPrevChannel(void);
	void Cmd_IRCactiveColor(void);
	void Cmd_IRCinactiveColor(void);
	void Cmd_IRCbgColor(void);
	void Cmd_IRCbgBrightness(void);
	

	bool bIRCDisplay;
	int iNotification;
	bool bIRCToGame;
	bool bGameToIRC;
	bool bGlobalSay;
	char sCurChannel[255];

	Vector bgColor;
	float bgBrightness;
	Vector activeColor;
	Vector inactiveColor;

	std::vector<Channel_t *> sChannels;
};

extern CmIRC gmIRC;

void sendCommand(char *str);

#endif