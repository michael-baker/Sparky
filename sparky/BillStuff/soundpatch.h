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


#ifndef SOUNDPATCH_H
#define SOUNDPATCH_H

class CSoundPatch : CBaseHook
{
	public:

	virtual bool Pre_HUD_Init (void);
	virtual void Post_HUD_Init (void);
	virtual bool Pre_HUD_Redraw (float flTime, int intermission);
	virtual void Post_HUD_Redraw (float flTime, int intermission);

	virtual void RegisterSound(const char *szSample, const float *Origin);

	CSoundPatch(void);

	virtual void Cmd_ToggleSoundDisplay(void);

	bool bSoundPatchDisplay;
	bool bInitialized;
	float fMaxRange;
	bool bTimerActive;
	float fTimerStart;
	HSPRITE spriteTimer;

};

extern CSoundPatch gSoundPatch;

#endif