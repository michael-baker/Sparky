/*  ------------------------------------------------------------------------
	  laserstatus.cpp by ruiner@konspiracy.org
	    - updated v1.35 - 01/14/03 (typo fixes)
		- updated v1.34 - 01/13/03 (mulch_map fixes)
		- added v1.33 - 01/12/03
	------------------------------------------------------------------------ */

#include <time.h>
#include "..\client.h"
#include "hook.h"
#include "superhud.h"
#include "util.h"
#include "flagstatus.h"
#include "laserstatus.h"
#include "help.h"

#pragma warning (disable:4244) // float to int loss of data

/*Global*/
CLaserStatus gLaserStatus;

DECLARE_COMMAND_2(gLaserStatus, ToggleLaserStatus);

time_t		tLaserStatus[2];

bool CLaserStatus::Pre_HUD_Init (void)
{
	HOOK_COMMAND_2("blaserstatus", ToggleLaserStatus);
	gHelp.addCommand("blaserstatus", "Toggles display of the flag status (up/down).\nUsage: blaserstatus");

	tLaserStatus[1] = 0;
	tLaserStatus[2] = 0;
	teamscores[1].team = 1;
	teamscores[2].team = 2;
	teamscores[1].points = 0;
	teamscores[2].points = 0;
	ResetLasers();
	return true;
}

void CLaserStatus::Cmd_ToggleLaserStatus (void)
{
	bLaserStatusOn = bLaserStatusOn ? 0 : 1;

	if(bLaserStatusOn)
		gSuperHUD.WriteHUDChannel(HC_IN_GAME_COLOR, 2, "laser status ON");
	else
		gSuperHUD.WriteHUDChannel(HC_IN_GAME_COLOR, 2, "laser status OFF");
}

void CLaserStatus::ResetLasers(void)
{
	tLaserTime = 60; // default time

	if( (strstr(szLevelName, "mulch_faf.bsp"))
		)
		tLaserTime = 45;

	if( (strstr(szLevelName, "hollow.bsp"))
		|| (strstr(szLevelName, "shutdown.bsp"))
		|| (strstr(szLevelName, "shutdown_.bsp"))
		)
		tLaserTime = 60;

	if((strstr(szLevelName, "openfire"))
		|| (strstr(szLevelName, "fry_complex.bsp"))
		|| (strstr(szLevelName, "evenflow.bsp"))
		)
		tLaserTime = 30;

	if((strstr(szLevelName, "shutdown2.bsp"))
		)
		tLaserTime = 75;

	// maps where the security/lasers are not timed
	if((strstr(szLevelName, "security_r.bsp"))
		)
		tLaserTime = 32000;
}

void CLaserStatus::Post_HUD_Frame (double time)
{
	if((strcmp(szLevelName, gEngfuncs.pfnGetLevelName())) || ((fLastTime-10) > gEngfuncs.GetClientTime()))
	{
		sprintf(szLevelName, "%s", gEngfuncs.pfnGetLevelName());
		ResetLasers();
	}
	if((fLastTime-10) > gEngfuncs.GetClientTime()) {
		tLaserStatus[1] = 0;
		tLaserStatus[2] = 0;
		teamscores[1].team = 1;
		teamscores[2].team = 2;
		teamscores[1].points = 0;
		teamscores[2].points = 0;
	}
	fLastTime = gEngfuncs.GetClientTime();
}

void CLaserStatus::Post_HUD_Redraw (float flTime, int intermission)
{
	if((tLaserStatus[1]) && ( int(time(NULL) - tLaserStatus[1]) > tLaserTime) && (!strcmp(blueLaser, "down"))) {
		tLaserStatus[1] = time(NULL);
		sprintf(blueLaser,"up");
	}
	if((tLaserStatus[2]) && ( int(time(NULL) - tLaserStatus[2]) > tLaserTime) && (!strcmp(redLaser, "down"))) {
		tLaserStatus[2] = time(NULL);
		sprintf(redLaser,"up");
	}

	if((gSuperHUD.iSuperHUDLevel >= 1) && (bLaserStatusOn))
	{
		int ixRight;
		int iyBlue, iyRed;
		int iBlueR	= 9;	int iBlueG	= 102;	int iBlueB	= 255;
		int iRedR	= 255;	int iRedG	= 15;	int iRedB	= 9;
		int iyLaser = int(gSuperHUD.m_scrinfo.iHeight*0.18); // score display base y

		// there's a bug here... dunno what it is damnit
		
		ixRight = 86;
		if(gFlagStatus.bFlagStatusOn)
				ixRight = ixRight + gSuperHUD.m_scrinfo.charWidths['d'] * 7;

		if(teamscores[1].team == 1) {
			iyBlue	= iyLaser;
			iyRed	= iyLaser + gSuperHUD.m_scrinfo.iCharHeight - 3;
		} else {
			iyRed	= iyLaser;	
			iyBlue	= iyLaser + gSuperHUD.m_scrinfo.iCharHeight - 3;
		}

		if(tLaserStatus[1])
			gSuperHUD.DrawHudString(ixRight, iyBlue, iBlueR, iBlueG, iBlueB, "%s", blueLaser);
		if(tLaserStatus[2])
			gSuperHUD.DrawHudString(ixRight, iyRed, iRedR, iRedG, iRedB, "%s", redLaser);
	}
}

bool CLaserStatus::Pre_Hook_ConsoleCommand (char *pszString)
{
/*	commented this because there is currently no way i know to detect if the lasers are back up...
	would look stupid if it said "down" for the whole round now wouldn't it?

	if (strstr(pszString, "vox/red security is deactivated")) // security_r
	{ 
		tLaserStatus[2] = time(NULL);
		sprintf(redLaser, "down");
	}
	if (strstr(pszString, "vox/blue security is deactivated")) // security_r
	{ 
		tLaserStatus[1] = time(NULL);
		sprintf(blueLaser, "down");
	} */
	return true;
}

void CLaserStatus::Post_Hook_HudText (const char * msg)
{
	cl_entity_t *pLocal = gEngfuncs.GetLocalPlayer();

	if ( ( !strcmp(msg, "OUR SECURITY IS DOWN!\n") )
		) {
		if(pLocal->curstate.team == 1){
			sprintf(blueLaser, "down");
			tLaserStatus[1] = time(NULL);
		} 
		else {
			sprintf(redLaser, "down");
			tLaserStatus[2] = time(NULL);
		}
	}

	if ( ( !strcmp(msg, "OUR SECURITY HAS\n\nBEEN RESTORED!\n") )
//		|| ( !strcmp(msg, "OUR lasers ACTIVATED!\n") ) // commented since you can't track if your lasers go down.. gg mulchman
		){
		if(pLocal->curstate.team == 1)
		{
			sprintf(blueLaser, "up");
			tLaserStatus[1] = time(NULL);
		} else {
			sprintf(redLaser, "up");
			tLaserStatus[2] = time(NULL);
		}
	}
	
	if ( ( !strcmp (msg, "ED SECURITY IS\n\n DOWN FOR 30 SECONDS\n") )
		|| ( !strcmp (msg, "ED SECURITY IS DOWN\n\n ACCESS TO RED FLAG IS AVAILABLE!\n") )
		|| ( !strcmp (msg, "ed security down for 60 seconds!") )
		)
	{
		sprintf(redLaser, "down");
		tLaserStatus[2] = time(NULL);
	}

	if ( ( !strcmp (msg, "ED FLAG ACCESS:\n\n DENIED!\n") )
		|| ( !strcmp (msg, "ED SECURITY IS\n\n UP!\n") )
		|| ( !strcmp (msg, "ed security is operational!") )
		|| ( !strcmp (msg, "ED SECURITY IS\n\nOPERATING\n") )
		) {
		sprintf(redLaser, "up");
		tLaserStatus[2] = time(NULL);
	}

	if ( ( !strcmp (msg, "LUE SECURITY IS\n\n DOWN FOR 30 SECONDS\n") )
		|| ( !strcmp (msg, "LUE SECURITY IS DOWN\n\n ACCESS TO BLUE FLAG IS AVAILABLE!\n") )
		|| ( !strcmp (msg, "LUE security down for 60 seconds!") )
		)
	{
		sprintf(blueLaser, "down");
		tLaserStatus[1] = time(NULL);
	}

	if ( ( !strcmp (msg, "LUE FLAG ACCESS:\n\n DENIED!\n") )
		|| ( !strcmp (msg, "LUE SECURITY IS\n\n UP!\n") )
		|| ( !strcmp (msg, "lue security is operational!") )
		|| ( !strcmp (msg, "LUE SECURITY IS\n\nOPERATING\n") )
		) {
		sprintf(blueLaser, "up");
		tLaserStatus[1] = time(NULL);
	}

	if ( ( !strcmp(msg, "NEMY laser field DEACTIVATED!\n") ) 
	) {
		if(pLocal->curstate.team == 1)
		{
			sprintf(redLaser, "down");
			tLaserStatus[2] = time(NULL);
		} else {
			sprintf(blueLaser, "down");
			tLaserStatus[1] = time(NULL);
		}
	}

	if ( ( !strcmp(msg, "NEMY lasers ACTIVATED!\n") ) 
	) {
		if(pLocal->curstate.team == 1)
		{
			sprintf(redLaser, "up");
			tLaserStatus[2] = time(NULL);
		} else {
			sprintf(blueLaser, "up");
			tLaserStatus[1] = time(NULL);
		}
	}

} 

void CLaserStatus::hook_teamscore(char *tbuf, int points)
// had to add this to sort scores to match mulchman's score display sorter thing
{
	if((!strcmp(tbuf, "Blue"))
		|| (strcmp(tbuf, "#Hunted_team1")==0)
		|| (strcmp(tbuf, "#Dustbowl_team1")==0)
		|| (strcmp(tbuf, "Targets")==0)
		|| (strcmp(tbuf, "Goto Clan")==0)
	) {
		if(teamscores[1].team == 1)
			teamscores[1].points = points; 
		else
			teamscores[2].points = points; 
	}

	if((!strcmp(tbuf, "Red")) 
		|| (strcmp(tbuf, "#Hunted_team2")==0)
		|| (strcmp(tbuf, "#Dustbowl_team2")==0)
		|| (strcmp(tbuf, "Xterminators")==0)
		|| (strcmp(tbuf, "Ii Clan")==0)
		|| (strcmp(tbuf, "Snipers")==0)
	) { 
		if(teamscores[2].team == 2)
		{
			teamscores[2].points = points;
		} else {
			teamscores[1].points = points;
		}
	} 

	if(teamscores[2].points > teamscores[1].points)
	{
		teamscores[3] = teamscores[1];
		teamscores[1] = teamscores[2];
		teamscores[2] = teamscores[3];
	}
}

