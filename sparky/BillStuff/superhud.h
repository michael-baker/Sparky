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


#ifndef SUPERHUD_H
#define SUPERHUD_H

#define FLOATING_TEXT_SLOTS 256

#define HC_HUDLEVEL 0
#define HC_SOUNDPATCH 17
#define HC_SPEEDO_SPEED 18
#define HC_SPEEDO_MAX 19
#define HC_SPEEDO_PERCENT 20
//#define HC_BHOP_PRED 20
#define HC_IN_GAME_COLOR 21
#define HC_STATS_OPTION 25
#define HC_STATS_STATS 26
//#define HC_STATS_RATIO 27
#define HC_WINAMP_CURSONG 28
#define HC_WINAMP_CURSONG_STATUS 29

#define HC_USER_TIMER 48
#define HC_CLOCK 49
#define HC_ROUND_TIMER 50
#define HC_IRC_DISPLAY 51
#define HC_IRC_CHANNEL 52
#define HC_IRC_TO_GAME 53
#define HC_GAME_TO_IRC 54
#define HC_IRC_GLOBALSAY 55

class CFloatingText
{
	public:
	void AddFloatingText(char *szText, int ix, int iy, Vector Color, float fEndTime, bool bConsole);
	void UpdateFloatingText(void);

	CFloatingText(void);

	typedef struct floattext_s
	{
		bool bFree;
		char szText[1024];
		int ix;
		int iy;
		Vector Color;
		float fEndTime;
		bool bConsole;
	} floattext_t;

	floattext_t FloatText[FLOATING_TEXT_SLOTS];
};

class CFloatingText3d // like normal floating text but coordinates are in 3d space and text doesnt' move with the player
{
	public:
	void AddFloatingText(char *szText, Vector Origin, Vector Color, float fEndTime, bool bConsole);
	void UpdateFloatingText(void);

	CFloatingText3d(void);

	typedef struct floattext3d_s
	{
		bool bFree;
		char szText[1024];
		Vector Origin;
		Vector Color;
		float fEndTime;
		bool bConsole;
	} floattext3d_t;

	floattext3d_t FloatText[FLOATING_TEXT_SLOTS];
};

class CHUDChannel
{
	public:

	virtual void WriteHUDChannel (float fPeriod, char *WriteText);
	virtual void DisplayHUDChannel (int iSlot);

	CHUDChannel (void);
	
	float fStartTime;
	float fEndTime;
	char Text[512];
};

class CSuperHUD : CBaseHook
{
	public:

	virtual bool Pre_HUD_Init (void);
	virtual void Post_HUD_Init (void);
	virtual bool Pre_HUD_Redraw (float flTime, int intermission);
	virtual void Post_HUD_Redraw (float flTime, int intermission);
	virtual bool Pre_HUD_AddEntity (int type, struct cl_entity_s *ent, const char *modelname);
	virtual void Post_HUD_AddEntity (int type, struct cl_entity_s *ent, const char *modelname);
	virtual bool Pre_HUD_PlayerMove (struct playermove_s *ppmove, int server);
	virtual void Post_HUD_PlayerMove (struct playermove_s *ppmove, int server);
	virtual bool Pre_HUD_PlayerMoveInit (struct playermove_s *ppmove);
	virtual void Post_HUD_PlayerMoveInit (struct playermove_s *ppmove);
	/*
	virtual bool Pre_HUD_PlayerMoveTexture (char *name);
	virtual void Post_HUD_PlayerMoveTexture (char *name);
	*/


	CSuperHUD(void);
	~CSuperHUD(void);

	virtual void Cmd_SuperHUDLevel(void);
	virtual void Cmd_Gamma(void);
	virtual void Cmd_PredictHop(void);
	virtual void Cmd_SetHUDColor(void);
	virtual void Cmd_ToggleColors(void);
	virtual void WriteHUDChannel(int iChannel, float fPeriod, char *WriteText);
	virtual void DrawHUDLabels (void);

	virtual int DrawHudString (int x, int y, int r, int g, int b, const char *fmt, ... );

	int iSuperHUDLevel;
	bool bUsedGamma;
	bool bPredictHop;
	CHUDChannel Channel[64];
	bool bRendered[1024];
	BYTE *bHopOffset;
	bool bHopOffsetFound;
	SCREENINFO m_scrinfo;
	Vector HUDColor;
	float fHUDBrightness;
	bool bHUDColorInGame;

	CFloatingText FloatingText;
	CFloatingText3d FloatingText3d;
};

extern CSuperHUD gSuperHUD;


#endif