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
#include "location.h"
#include "util.h"
#include "foxbotarea.h"
#include "help.h"

/*Global*/
CLocation gLocation;

bool CLocation::Pre_HUD_Init (void)
{
	gHelp.addCommand("btrigger_sgup", "Triggered when you build an sg.\nExample:\nalias btrigger_sgup \"say_team my sg is up and ready!\"");
	gHelp.addCommand("btrigger_sgdown", "Triggered when your sg is destroyed or dismantled.\nExample:\nalias btrigger_sgdown \"say_team my sg was destroyed!\"");

	return true;
}

void CLocation::Post_HUD_Init (void)
{
}

bool CLocation::Pre_HUD_Frame (double time)
{
	return true;
}

void CLocation::Post_HUD_Frame (double time)
{
	if(strcmp(szLevelName, gEngfuncs.pfnGetLevelName()))
	{
		sprintf(szLevelName, "%s", gEngfuncs.pfnGetLevelName());
		LoadBeacons();
	}
	// Beg: Fr33 addition 1.42
	if(bSentryUp)
	{
		bSentryUp = 0;
		gEngfuncs.pfnClientCmd("btrigger_sgup;");
	}
	if(bSentryDown)
	{
		bSentryDown = 0;
		gEngfuncs.pfnClientCmd("btrigger_sgdown;");
	}
	// End: Fr33 addition 1.42
}

bool CLocation::Pre_Hook_TextMsg (const char *pszString)
{
	return true;
}

void CLocation::Post_Hook_TextMsg (const char *pszString)
{
	if(strstr(pszString, "#Sentry_finish"))
	{
		bSentryUp = 1;
		
		// Get Players Location for SG placement		
		int index = AreaInsideClosest(gEngfuncs.GetLocalPlayer());

		if( index == -1 )
			strcpy(strSGLocation, "unknown location");
		else
			strcpy(strSGLocation, NeoTFGetAreaByTeam(gEngfuncs.GetLocalPlayer()->curstate.team, index));
	}
	if(strstr(pszString, "#Sentry_destroyed") || strstr(pszString, "#Sentry_dismantle"))
	{
		bSentryDown = 1;

		sprintf(strSGLocation, "destroyed / not built");
	}
}

bool CLocation::Pre_Hook_ConsoleCommand (char *pszString)
{
	char szNewCommand[8192];
	sprintf(szNewCommand, "%s", pszString);
	cl_entity_s *pLocal = gEngfuncs.GetLocalPlayer();
	if(!pLocal || !strcmp(szLevelName, "")) return true; // player not spawned yet -- prevent crash
	if(!(strstr(szNewCommand, "say") || strstr(szNewCommand, "say_team") || strstr(szNewCommand, "tell"))) return true;
	if(strstr(szNewCommand, "bind") || strstr(szNewCommand, "alias")) return true; // don't convert tokens in binds or aliases
	if((pLocal->curstate.playerclass >= 0) && (pLocal->curstate.playerclass <= 11))
	{
		if(strstr(szNewCommand, "%c")) // display own class
		{
			char classes[16][16] = {"none", "scout", "sniper", "soldier", "demoman", "medic", "hwguy", "pyro", "spy", "engineer", "random", "civilian"};
			sprintf(szNewCommand, "%s", StringSearchReplace("%c", classes[pLocal->curstate.playerclass], szNewCommand));
		}
		if(strstr(szNewCommand, "%l")) // display own location
		{
			char location[512];
			/*
			// Find the nearest beacon the player has LOS to
			int ClosestBeacon = -1;
			int ClosestRange = 8192; // So we don't have to keep calculating it
			int counter;
			pmtrace_t tr;
			for(counter = 0; counter<BeaconCount; counter++)
			{
				// Perform range check first to cut down on the number of necessary tracelines
				if((pLocal->curstate.origin - Beacon[counter].origin).Length() < ClosestRange)
				{
					tr = *(gEngfuncs.PM_TraceLine( (float *)&Beacon[counter].origin, (float *)&pLocal->curstate.origin, PM_TRACELINE_PHYSENTSONLY, 2, -1 ));
					if((tr.endpos - pLocal->curstate.origin).Length() < 8)
					{
						ClosestBeacon = counter;
						ClosestRange = (pLocal->curstate.origin - Beacon[counter].origin).Length();
					}
				}
			}
			if(ClosestBeacon == -1)
				sprintf(location, "unknown location");
			else
				sprintf(location, "%s", Beacon[ClosestBeacon].name);
			*/

			int index;
			index = AreaInsideClosest(gEngfuncs.GetLocalPlayer());

			if( index == -1 )
				strcpy(location, "unknown location");
			else
				strcpy(location, NeoTFGetAreaByTeam(gEngfuncs.GetLocalPlayer()->curstate.team, index));

			sprintf(szNewCommand, "%s", StringSearchReplace("%l", location, szNewCommand));
		}
		if(strstr(szNewCommand, "%p")) // display location being looked at
		{
			pmtrace_t tr;
			cl_entity_t Dummy;
			cl_entity_t *pDummy;
			pDummy = &Dummy;
			float vof[3];
			float ang[3];
			Vector Forward, Right, Up;
			Vector Target;
			char location[512];
			static char *New_Args;
			gEngfuncs.GetViewAngles(ang);
			if(ang[0] > 180) ang[0] = ang[0] - 360;
			if(ang[1] > 180) ang[1] = ang[1] - 360;
			gEngfuncs.pfnAngleVectors(ang, Forward, Right, Up);
			gEngfuncs.pEventAPI->EV_LocalPlayerViewheight(vof);
			Vector view_ofs = Vector(vof[0], vof[1], vof[2]);
			tr = *(gEngfuncs.PM_TraceLine( (float *)&(pLocal->curstate.origin + view_ofs), (float *)&((pLocal->curstate.origin + view_ofs) + (Forward * 8192)), PM_TRACELINE_PHYSENTSONLY, 2, -1 ));
			Target = tr.endpos;
			/*
			for(counter = 0; counter<BeaconCount; counter++)
			{
				// Perform range check first to cut down on the number of necessary tracelines
				if((Target - Beacon[counter].origin).Length() < ClosestRange)
				{
					tr = *(gEngfuncs.PM_TraceLine( (float *)&Beacon[counter].origin, (float *)&Target, PM_TRACELINE_PHYSENTSONLY, 2, -1 ));
					if((tr.endpos - Target).Length() < 8)
					{
						ClosestBeacon = counter;
						ClosestRange = (Target - Beacon[counter].origin).Length();
					}
				}
			}
			if(ClosestBeacon == -1)
				sprintf(location, "unknown location");
			else
				sprintf(location, "%s", Beacon[ClosestBeacon].name);
			*/
			pDummy->curstate.origin = Target;

			int index;
			index = AreaInsideClosest(pDummy);

			if( index == -1 )
				strcpy(location, "unknown location");
			else
				strcpy(location, NeoTFGetAreaByTeam(gEngfuncs.GetLocalPlayer()->curstate.team, index));

			sprintf(szNewCommand, "%s", StringSearchReplace("%p", location, szNewCommand));
		}
	}
	if(strcmp(szNewCommand, pszString))
	{
		strcat(szNewCommand, ";");
		gEngfuncs.pfnClientCmd(szNewCommand);
		return false;
	}
	return true;
}

void CLocation::LoadBeacons(void)
{
	/*
	FILE *fp;
	char filename[256];
	char location[512];
	char word[256];
	char buf[1024];
	int CurrentBeacon = 0;
	int tx, ty, tz;

	BeaconCount = 0;

	sprintf(buf, "%s", gEngfuncs.pfnGetLevelName());
	sprintf(filename, "sparkyutils\\%s.nli", NameFromFileName(buf));
	gEngfuncs.Con_Printf("[sparky] location filename: %s\n", filename);

	if(fp = fopen(filename, "rt"))
	{
		while(fscanf(fp, "%s", word) != EOF)
		{
			if(!strcmp(word, "["))
			{
				sprintf(Beacon[CurrentBeacon].name, "\0");
				fscanf(fp, "%d %d %d", &tx, &ty, &tz);
				Beacon[CurrentBeacon].origin.x = tx;
				Beacon[CurrentBeacon].origin.y = ty;
				Beacon[CurrentBeacon].origin.z = tz;
				do
				{
					fscanf(fp, "%s", word);
					if(strcmp(word, "]"))
					{
						sprintf(location, "%s", Beacon[CurrentBeacon].name);
						// Choose whether to start or append to the string
						if(Beacon[CurrentBeacon].name[0]!='\0')
							sprintf(Beacon[CurrentBeacon].name, "%s %s", location, word);
						else
							sprintf(Beacon[CurrentBeacon].name, "%s", word);
					}
				} while(strcmp(word, "]"));
				CurrentBeacon++;
				BeaconCount++;
			}
		}
		fclose(fp);
		gEngfuncs.pfnConsolePrint("[sparky] loaded beacons\n");
	}
	else
	{
		gEngfuncs.pfnConsolePrint("[sparky] no location information available on this map\n");
	}
	*/

	AreaDefInit();
	AreaDefLoad(gEngfuncs.GetLocalPlayer());
}
	
CLocation::CLocation(void)
{
	BeaconCount = 0;
	fLastTime = 9999;
	sprintf(szLevelName, "");
	sprintf(strSGLocation, "destroyed / not built");
}
