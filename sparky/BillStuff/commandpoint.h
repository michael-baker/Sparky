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


#ifndef COMMANDPOINT_H
#define COMMANDPOINT_H

class CCommandPoint : CBaseHook
{
	public:
	CCommandPoint (void);

	virtual bool Pre_HUD_Init (void);
	virtual void Post_HUD_Frame (double time);
	virtual void Post_HUD_Redraw (float flTime, int intermission);
	virtual void Post_Hook_TextMsg (const char *buf);
	virtual void Post_Hook_HudText (const char *pszString);

	// begin: added by Mulchman
	virtual void Cmd_ToggleCommandPoint (void);
	void SetbTeamCapFix( bool b );
	bool GetbTeamCapFix( );
	bool GetbMasterCommandPointOn( );
	bool GetbCommandPointOn( );
	// end: added by Mulchman

	bool bCommandPointOn;
	bool bScrummage; // added by Mulchman
	bool bMasterCommandPointOn; // added by Mulchman
	bool bTeamCapFix; // added by Mulchman
	int cpteam[9];

	float fLastTime;
	int iCommandPoints;
};

extern CCommandPoint gCommandPoint;

#endif
