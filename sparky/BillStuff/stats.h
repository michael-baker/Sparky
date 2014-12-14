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
 * Stats addition by L0ki.
 *
 */

#ifndef STATS_H
#define STATS_H

class CStats : CBaseHook
{
	public:
	
	virtual bool Pre_HUD_Init (void);
	virtual void Post_HUD_Init(void);
	virtual bool Pre_HUD_Frame(double time);
	virtual void Post_HUD_Frame(double time);
	virtual bool Pre_HUD_Redraw (float flTime, int intermission);
	virtual void Post_HUD_Redraw (float flTime, int intermission);

	virtual void hook_scoreinfo(int cl, int frags, int deaths, int team); //added by loki

	virtual void Cmd_ToggleStats(void);

	CStats (void);

	bool bShowStatsOn;
	int iKills;
	int iDeaths;
	float fRatioKills;
	float fLastTime;
};

extern CStats gStats;

#endif