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
#include "soundpatch.h"
#include "util.h"

/*Global*/
CSoundPatch gSoundPatch;



DECLARE_COMMAND_2(gSoundPatch, ToggleSoundDisplay);

bool CSoundPatch::Pre_HUD_Init (void)
{
	HOOK_COMMAND_2("bsounddisplay", ToggleSoundDisplay);

	return true;
}

void CSoundPatch::Post_HUD_Init (void)
{
	bInitialized = true;
	fMaxRange = 1536;
	//spriteTimer = gEngfuncs.pfnSPR_Load("sprites/timer.spr");
}

CSoundPatch::CSoundPatch(void)
{
	bSoundPatchDisplay = 0;
	bInitialized = false;
	bTimerActive = false;
	fTimerStart = 0;
}

void CSoundPatch::Cmd_ToggleSoundDisplay(void)
{
	if(bSoundPatchDisplay == 1)
	{
		bSoundPatchDisplay = 0;
		gSuperHUD.WriteHUDChannel(HC_SOUNDPATCH, 1, "sound display - off");
	}
	else if(bSoundPatchDisplay == 0)
	{
		bSoundPatchDisplay = 1;
		gSuperHUD.WriteHUDChannel(HC_SOUNDPATCH, 1, "sound display - on");
	}
}


void CSoundPatch::RegisterSound(const char *szSample, const float *Origin)
{
	/*
	const char IgnoreSounds[16][32] = { "ric1", "ric2", "ric3", "ric4", "ric5", "", "", "", "", "", "", "", "", "", "", ""};
	cl_entity_t *pLocal;
	int i;
	if(bSoundPatchDisplay)
	{
		pLocal = gEngfuncs.GetLocalPlayer();
		if(pLocal)
		{
			Vector newOrigin(Origin[0], Origin[1], Origin[2]);
			if((pLocal->curstate.origin - newOrigin).Length() < fMaxRange)
			{
				char buf1[1024];
				char buf2[1024];
				sprintf(buf1, "%s", NameFromModel(szSample));
				for(i = 0; i < 16; i++)
				{
					if(!strcmp(buf1, IgnoreSounds[i])) return;
				}
				sprintf(buf2, "-[%s]", buf1);
				gSuperHUD.FloatingText3d.AddFloatingText(buf2, newOrigin, Vector(255,64,0), gEngfuncs.GetClientTime() + 2, true);
			}
		}
		else
		{
				Vector newOrigin(Origin[0], Origin[1], Origin[2]);
				char buf[1024];
				sprintf(buf, "-[%s]", NameFromModel(szSample));
				gSuperHUD.FloatingText3d.AddFloatingText(buf, newOrigin, Vector(255,64,0), gEngfuncs.GetClientTime() + 2, true);
		}
	}
	*/
	if(!strcmp(szSample, "weapons/ax1.wav"))
	{
		bTimerActive = true;
		fTimerStart = gEngfuncs.GetClientTime();
	}
}

bool CSoundPatch::Pre_HUD_Redraw (float flTime, int intermission)
{
	return true;
}

void CSoundPatch::Post_HUD_Redraw (float flTime, int intermission)
{
	if(bTimerActive && ((fTimerStart > gEngfuncs.GetClientTime()) || (gEngfuncs.GetClientTime() - fTimerStart > 4)))
	{
		bTimerActive = false;
	}

	if(bTimerActive)
	{
		/*
		gEngfuncs.pfnFillRGBA(gSuperHUD.m_scrinfo.iWidth/2 +1, gSuperHUD.m_scrinfo.iHeight/2 +1, (gEngfuncs.GetClientTime() - fTimerStart)/8 +1, 0.5 +1, 255, 255, 255, 128);
		*/
	}
}
