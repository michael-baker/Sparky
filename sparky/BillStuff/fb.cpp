/*  ------------------------------------------------------------------------
	  fb.cpp by umm... fb
		- soon to be extinct
		- updated v1.41 - added bshowcommands
	------------------------------------------------------------------------ */

#include "..\client.h"

#include "hook.h"
#include "superhud.h"
#include "fb.h"
#include <time.h>
#include "playerinfo.h"
#include "logging.h"
#include "help.h"

#pragma warning (disable:4244) // float to int loss of data

/*Global*/
CFB gFB;

DECLARE_COMMAND_2(gFB, ToggleScoreDisplay);
DECLARE_COMMAND_2(gFB, ToggleNadeTimer);
DECLARE_COMMAND_2(gFB, Sumo);
DECLARE_COMMAND_2(gFB, ToggleShowCommands);

bool CFB::Pre_HUD_Init (void)
{
	HOOK_COMMAND_2("bscores", ToggleScoreDisplay);
	gHelp.addCommand("bscores", "Toggles display of the team-scores.\nUsage: bscores");
	HOOK_COMMAND_2("bnadetimer", ToggleNadeTimer);
	gHelp.addCommand("bnametimer", "Toggles display of the visual nadetimer. Only visible when a nade primed.\nUsage: bnadetimer text/x/y/h/w\nPositioning: <axis> value");
	HOOK_COMMAND_2("bsumo", Sumo);
	gHelp.addCommand("bsumo", "Toggles neat entity effects.\nUsage: bsumo");
	HOOK_COMMAND_2("bshowcommands", ToggleShowCommands);
	gHelp.addCommand("bshowcommands", "Toggles displaying of executed client side commands\nUsage: bshowcommands");

	return true;
}
	
void CFB::Cmd_ToggleShowCommands (void)
{
	bShowCommands = bShowCommands ? 0 : 1;

	if(bShowCommands)
		gSuperHUD.WriteHUDChannel(HC_IN_GAME_COLOR, 2, "console commands echo ON");
	else
		gSuperHUD.WriteHUDChannel(HC_IN_GAME_COLOR, 2, "console commands echo OFF");
}

void CFB::Post_HUD_Frame (double time)
{
	//reset the timers!
	if((fLastTime-10) > gEngfuncs.GetClientTime())
	{
		fNadeTimer = 0;
		bScoreBlue = 0;
		bScoreRed = 0;
		bScoreYellow = 0;
		bScoreGreen = 0;

		// begin: added by Mulchman
		teamscores[0].team = 'b';
		teamscores[1].team = 'r';
		teamscores[2].team = 'y';
		teamscores[3].team = 'g';
		// end: added by Mulchman
	}
	fLastTime = gEngfuncs.GetClientTime();
}

bool CFB::Pre_Hook_TextMsg (const char *pszString)
{

	//fb's bonus class configs
	/*
	cl_entity_s *pLocal = gEngfuncs.GetLocalPlayer();

	if(pLocal->curstate.spectator)
	{
		//gEngfuncs.pfnClientCmd("exec spectator.cfg");
		gEngfuncs.pfnConsolePrint("[sparky] Execing spectator.cfg, omgz! (Method #2)\n");
	}

	if(pLocal->curstate.playerclass == 0)
	{
		//gEngfuncs.pfnClientCmd("exec spectator.cfg");
		gEngfuncs.pfnConsolePrint("[sparky] Execing spectator.cfg, omgz! (Method #3)\n");
	}
	*/
	/*if( (!strcmp(pszString, "#Spec_Mode3")) || 
		(!strcmp(pszString, "#Spec_Mode2")) ||
		(!strcmp(pszString, "#Spec_Mode1"))	)
	{
		gEngfuncs.pfnClientCmd("exec spectator.cfg");
		gEngfuncs.pfnConsolePrint("[sparky] Execing spectator.cfg, omgz! (Method #1)\n");
	}*/
	/*
	if(pLocal->curstate.playerclass == 11)
	{
		//gEngfuncs.pfnClientCmd("exec civilian.cfg");
		gEngfuncs.pfnConsolePrint("[sparky] Execing civilian.cfg, omgz! (Method #2)\n");
	}
	*/
	/*if( (!strcmp(pszString, "#Civilian")) ||
		(!strcmp(pszString, "hunted_target_welcome")) )
	{
		gEngfuncs.pfnClientCmd("exec civilian.cfg");
		gEngfuncs.pfnConsolePrint("[sparky] Execing civilian.cfg, omgz! (Method #1)\n");
	}*/

	return true;
}

void CFB::Post_Hook_TextMsg (const char *pszString)
{
	if(!strcmp(pszString, "#Match_results"))
	{
		fLastTime = 0;
		fNadeTimer = 0;
		bScoreBlue = 0;
		bScoreRed = 0;
		bScoreYellow = 0;
		bScoreGreen = 0;

		// begin: added by Mulchman
		teamscores[0].team = 'b';
		teamscores[1].team = 'r';
		teamscores[2].team = 'y';
		teamscores[3].team = 'g';
		// end: added by Mulchman
	}
}

void CFB::Post_Hook_HudText (const char *pszString)
{
	// FB's civilian config exec
	if(!strcmp(pszString, "hunted_target_welcome"))
	{
		gEngfuncs.pfnClientCmd("exec civilian.cfg");
		gEngfuncs.pfnConsolePrint("[sparky] Execing civilian.cfg, omgz! (Method #3)\n");
	}
}

bool CFB::Pre_HUD_Redraw (float flTime, int intermission)
{
	return true;
}

void CFB::Post_HUD_Redraw (float flTime, int intermission)
{
	/* -------------------------------------Score Display-----------------------------------
					 thanks to fryguy, equinox, and mulchman for this section
	   ------------------------------------------------------------------------------------- */

	if(bScoreDisplayOn && (gSuperHUD.iSuperHUDLevel >= 1))
	{
		cl_entity_t *pLocal = gEngfuncs.GetLocalPlayer();
		hud_player_info_t localinfo;
		gEngfuncs.pfnGetPlayerInfo(gEngfuncs.GetLocalPlayer()->index, &localinfo);

		iySlot1   = int(gSuperHUD.m_scrinfo.iHeight*0.18); 
		iySlot2   = iySlot1 + gSuperHUD.m_scrinfo.iCharHeight - 3; 
		iySlot3   = iySlot2 + gSuperHUD.m_scrinfo.iCharHeight - 3; 
		iySlot4   = iySlot3 + gSuperHUD.m_scrinfo.iCharHeight - 3; 
		char tbuf[1024];

		if(pLocal->curstate.team == 3)
			bScoreYellow = 1;
		if(pLocal->curstate.team == 4)
			bScoreGreen = 1;	

		strcpy(bluename,	" B ");
		strcpy(redname,		" R ");
		strcpy(yellowname,	" Y ");
		strcpy(greenname,	" G ");

		switch (pLocal->curstate.team)
		{
		case 1: strcpy(bluename,	"-B-"); break;
		case 2: strcpy(redname,		"-R-"); break;
		case 3: strcpy(yellowname,	"-Y-"); break;
		case 4: strcpy(greenname,	"-G-"); break;
		}		

		// display
		sprintf(tbuf, "%d", teamscores[0].points);
		if (teamscores[0].team == 'b')
			Draw_Blue_Scores(iySlot1, tbuf);
		else if (teamscores[0].team == 'r')
			Draw_Red_Scores(iySlot1, tbuf);
		else if ((teamscores[0].team == 'y') && (bScoreYellow))
			Draw_Yellow_Scores(iySlot1, tbuf);
		else if ((teamscores[0].team == 'g') && (bScoreGreen))
			Draw_Green_Scores(iySlot1, tbuf);

		sprintf(tbuf, "%d", teamscores[1].points);
		if (teamscores[1].team == 'b')
			Draw_Blue_Scores(iySlot2, tbuf);
		else if (teamscores[1].team == 'r')
			Draw_Red_Scores(iySlot2, tbuf);
		else if ((teamscores[1].team == 'y') && (bScoreYellow))
			Draw_Yellow_Scores(iySlot2, tbuf);
		else if ((teamscores[1].team == 'g') && (bScoreGreen))
			Draw_Green_Scores(iySlot2, tbuf);

		sprintf(tbuf, "%d", teamscores[2].points);
		if (teamscores[2].team == 'b')
			Draw_Blue_Scores(iySlot3, tbuf);
		else if (teamscores[2].team == 'r')
			Draw_Red_Scores(iySlot3, tbuf);
		else if ((teamscores[2].team == 'y') && (bScoreYellow))
			Draw_Yellow_Scores(iySlot3, tbuf);
		else if ((teamscores[2].team == 'g') && (bScoreGreen))
			Draw_Green_Scores(iySlot3, tbuf);

		sprintf(tbuf, "%d", teamscores[3].points);
		if (teamscores[3].team == 'b')
			Draw_Blue_Scores(iySlot4, tbuf);
		else if (teamscores[3].team == 'r')
			Draw_Red_Scores(iySlot4, tbuf);
		else if ((teamscores[3].team == 'y') && (bScoreYellow))
			Draw_Yellow_Scores(iySlot4, tbuf);
		else if ((teamscores[3].team == 'g') && (bScoreGreen))
			Draw_Green_Scores(iySlot4, tbuf);
	}



	/* ----------------------------------Visual Nade Timer----------------------------------
			    visual nade-timer, loosely based on the Nemesis graphical speedo.
	   ------------------------------------------------------------------------------------- */

	if(bNadeTimerOn && (gSuperHUD.iSuperHUDLevel >= 1) && fNadeTimer)
	{
		float time = gEngfuncs.GetClientTime();

		//int barWd, nade/*, barR, barG, barB*/ = 0;
		int barWd, nade, barR, barG, barB, x, y, h = 0;

		if((gEngfuncs.GetLocalPlayer()->curstate.playerclass == 1) && bNadeGren1)
		{
			fNadeTimer = 0;
			bNadeGren1 = 0;
		}

		if(bNadeCustomXOn)
			x = iNadeCustomX;
		else
		{
			x = gSuperHUD.m_scrinfo.iWidth*0.4;
			iNadeCustomX = x;
		}

		if(bNadeCustomYOn)
			y = iNadeCustomY;
		else
		{
			y = gSuperHUD.m_scrinfo.iHeight*0.54;
			iNadeCustomY = y;
		}

		if(bNadeCustomHOn)
			h = iNadeCustomH;
		else
		{
			h = gSuperHUD.m_scrinfo.iCharHeight*0.5;
			iNadeCustomH = h;
		}

		if(bNadeCustomWOn)
			barWd = iNadeCustomW;
		else
		{
			barWd = gSuperHUD.m_scrinfo.iWidth*0.20;
			iNadeCustomH = barWd;
		}

		//barWd = gSuperHUD.m_scrinfo.iWidth*0.20;
		//int x = gSuperHUD.m_scrinfo.iWidth*0.4;
		//int y = gSuperHUD.m_scrinfo.iHeight*0.54;
		//int h = gSuperHUD.m_scrinfo.iCharHeight*0.5;

		float nadetime = (time - (float)fNadeTimer);
		int maxTime = 4;

		if (nadetime > maxTime)
		{
			fNadeTimer = 0;
			primed = 0;
		}

		nade = (int)((float)nadetime / ((float)maxTime) * (barWd - 1) );//(int)((float)speed * (barWd - 1));
		if (nade > (barWd - 1))
			nade = (barWd - 1);

		/*if (speed <= maxSpeed) {
			barR = 0;
			barG = 255;
		} else {
			barR = speed / (maxSpeed * 1.7) * 255;
			if (barR > 255)
				barR = 255;
			barG = 255 - barR;
		}*/

		if((gSuperHUD.HUDColor.x >= 0) && (gSuperHUD.HUDColor.y >= 0) && (gSuperHUD.HUDColor.z >= 0))
		{
			barR = gSuperHUD.HUDColor.x;
			barG = gSuperHUD.HUDColor.y;
			barB = gSuperHUD.HUDColor.z;
			
		}
		else
		{
			barR = 240;	
			barG = 179;
			barB = 16;
		}
	
		int borderR = barR - 100;
		int	borderG = barG - 100;
		int borderB = barB - 100;

		if (borderR < 0)
			borderR = 0;
		if (borderG < 0)
			borderG = 0;
		if (borderB < 0)
			borderB = 0;
		
		gEngfuncs.pfnFillRGBA (x+1, y+1, nade, h-1, barR, barG, barB, 0xFF);

		gEngfuncs.pfnFillRGBA (x, y, barWd, 1, borderR, borderG, borderB, 0xFF);
		gEngfuncs.pfnFillRGBA (x, y+h, barWd, 1, borderR, borderG, borderB, 0xFF);

		//					   x  y  w  h  r   g	b     a
		gEngfuncs.pfnFillRGBA (x, y, 1, h, borderR, borderG, borderB, 0xFF);
		gEngfuncs.pfnFillRGBA (x+barWd, y, 1, h+1, borderR, borderG, borderB, 0xFF);
		gEngfuncs.pfnFillRGBA (x+barWd*0.25, y, 1, h, borderR, borderG, borderB, 0xFF);
		gEngfuncs.pfnFillRGBA (x+barWd*0.5, y, 1, h, borderR, borderG, borderB, 0xFF);
		gEngfuncs.pfnFillRGBA (x+barWd*0.75, y, 1, h, borderR, borderG, borderB, 0xFF);

		if (bNadeTextOn) 
		{
			//textual timer
			int iy1 = 128 + 14*16;
			int ixRight = gSuperHUD.m_scrinfo.iWidth*0.48;
			char buf[1024];
			sprintf(buf, "[ %.2f ]", time - fNadeTimer);
			gSuperHUD.DrawHudString(ixRight, iy1, barR, barG, barB, buf);
		}
		if (bNadeDebugOn) 
		{
			//textual debuggage
			int iy1 = gSuperHUD.m_scrinfo.iHeight*0.58;
			int ixRight = gSuperHUD.m_scrinfo.iWidth*0.31;
			char buf[1024];
			sprintf(buf, "[ %.2f ] [ %d %d %d ] [ %d %d %d ] [ x:%d y:%d h:%d w:%d ]", time - fNadeTimer, barR, barG, barB, borderR, borderG, borderB, x, y, h, barWd);
			gSuperHUD.DrawHudString(ixRight, iy1, 0, 66, 164, buf);
		}
	}
}

bool CFB::Pre_Hook_ConsoleCommand (char *pszString)
{
	if (bShowCommands) {
		const char IgnoreCommands[55][32] = { 
			"+attack", "-attack", 
			"+attack2","-attack2", 
			"+moveleft", "-moveleft", 
			"+moveright", "-moveright", 
			"+forward", "-forward", 
			"+back", "-back", 
			"+use", "-use", 
			"+reload", "-reload", 
			"throwgren", 
			"+left", "-left", 
			"+right", "-right", 
			"+jump", "-jump", 
			"+moveup", "-moveup", 
			"+movedown", "-movedown", 
			"+duck", "-duck", 
			"+showscores", "-showscores", 
			"+commandmenu", "-commandmenu", 
			"btrigger", 
			"toggleconsole", 
			"spk", "speak", "play", 
			"+speed", "-speed", 
			"slot", "room", "setinfo", 
			"+gren1", "-gren1", 
			"+gren2", "-gren2", 
			"primeone", "primetwo", 
			"discard","wait",
			"fullserverinfo",
			"special", "_special",
			"m_pitch"}; 
		bool bIgnored = false; 
		char buf[1024]; 
		for(int i = 0; i < 51; i++) 
		{ 
			if(IgnoreCommands[i][0] && strstr(pszString, IgnoreCommands[i])) 
			{ 
				bIgnored = true; 
				break; 
			} 
		} 
	
		if(!bIgnored) 
		{ 
			sprintf(buf, "%c%c[console command] %s\n", 0xff, 0x02, pszString); 
			hook_saytext("SayText", strlen(buf), buf); 
		} 
	}

	if((strstr(pszString, "+gren1")) || (strstr(pszString, "+gren2")))
	{
		if(strstr(pszString, "+gren1"))
			bNadeGren1 = 1;

		fNadeTimer = gEngfuncs.GetClientTime();
		
		if(gLogging.bLogging && gLogging.iLogLevel > 2 && gLogging.fpLog)
		{
			char tbuf2[64];
			_strtime(tbuf2);
			char tbuf[256];
			sprintf(tbuf, "[%s] cmdparser: %s\n", tbuf2, pszString);
			fprintf(gLogging.fpLog, "%s", tbuf);
		}
		return true;
	}
	
	if((strstr(pszString, "primeone")) || (strstr(pszString, "primetwo")))
	{
		primed = primed ? 0 : 1;
		if(primed)
		{
			fNadeTimer = gEngfuncs.GetClientTime();
		}
	}
	return true;
}

void CFB::Post_HUD_AddEntity (int type, struct cl_entity_s *ent, const char *modelname)
{
	if (gSuperHUD.iSuperHUDLevel >= 1)
	{
		if (bSumo 
			&& (!strcmp(modelname,"models/backpack.mdl") 
			|| !strcmp(modelname,"models/g_armor.mdl") 
			|| !strcmp(modelname,"models/r_armor.mdl") 
			|| !strcmp(modelname,"models/y_armor.mdl") 
			|| !strcmp(modelname,"models/y_armor.mdl") 
			|| !strcmp(modelname,"models/w_medkit.mdl"))) 
		{
			ent->curstate.renderfx = kRenderFxHologram; //make it jittery, skip
			ent->curstate.rendermode = kRenderTransAdd; //make it translucent
			ent->curstate.renderamt = 1; //yep on the hologram-age 
		}				
	}
}

void CFB::hook_teamscore(char *tbuf, int points)
{
	if((strcmp(tbuf, "Blue")==0)
		|| (strcmp(tbuf, "#Hunted_team1")==0)
		|| (strcmp(tbuf, "#Dustbowl_team1")==0)
		|| (strcmp(tbuf, "Targets")==0)
		|| (strcmp(tbuf, "Goto Clan")==0))
	{
		if (teamscores[0].team == 'b')
			teamscores[0].points = points;
		else if (teamscores[1].team == 'b')
			teamscores[1].points = points;
		else if (teamscores[2].team == 'b')
			teamscores[2].points = points;
		else if (teamscores[3].team == 'b')
			teamscores[3].points = points;
		bScoreBlue = 1; 
	}

	if((strcmp(tbuf, "Red")==0) 
		|| (strcmp(tbuf, "#Hunted_team2")==0)
		|| (strcmp(tbuf, "#Dustbowl_team2")==0)
		|| (strcmp(tbuf, "Xterminators")==0)
		|| (strcmp(tbuf, "Ii Clan")==0)
		|| (strcmp(tbuf, "Snipers")==0))
	{ 
		if (teamscores[0].team == 'r')
			teamscores[0].points = points;
		else if (teamscores[1].team == 'r')
			teamscores[1].points = points;
		else if (teamscores[2].team == 'r')
			teamscores[2].points = points;
		else if (teamscores[3].team == 'r')
			teamscores[3].points = points;
		bScoreRed = 1; 
	}

	if((strcmp(tbuf, "Yellow")==0)
		|| (strcmp(tbuf, "#Hunted_team3")==0)
		|| (strcmp(tbuf, "Environmentalist")==0))
	{ 
		if (teamscores[0].team == 'y')
			teamscores[0].points = points;
		else if (teamscores[1].team == 'y')
			teamscores[1].points = points;
		else if (teamscores[2].team == 'y')
			teamscores[2].points = points;
		else if (teamscores[3].team == 'y')
			teamscores[3].points = points;
		bScoreYellow = 1; 
	}

	if(strcmp(tbuf, "Green")==0) 
	{ 
		if (teamscores[0].team == 'g')
			teamscores[0].points = points;
		else if (teamscores[1].team == 'g')
			teamscores[1].points = points;
		else if (teamscores[2].team == 'g')
			teamscores[2].points = points;
		else if (teamscores[3].team == 'g')
			teamscores[3].points = points;
		bScoreGreen = 1; 
	}

	// sort
	int InSlot, iInt, iTmp, SelectedSlot;
	char cTmp;

	for ( InSlot = 0; InSlot < 4; InSlot++ )
	{
		SelectedSlot = InSlot;

		iTmp = teamscores[InSlot].points;
		cTmp = teamscores[InSlot].team;

		for ( iInt = InSlot + 1; iInt < 4; iInt++ )
		{
			if ( teamscores[iInt].points > teamscores[SelectedSlot].points )
			{
				SelectedSlot = iInt;
			}
		}

		teamscores[InSlot].points = teamscores[SelectedSlot].points;
		teamscores[InSlot].team   = teamscores[SelectedSlot].team;
		teamscores[SelectedSlot].points = iTmp;
		teamscores[SelectedSlot].team	= cTmp;
	}
}

void CFB::hook_statusicon(char *buf, int status)
{
	/*if ((!strcmp("d_normalgrenade",buf)) || (!strcmp("d_concussiongrenade",buf)) || (!strcmp("d_mirvgrenade",buf)) || (!strcmp("d_empgrenade",buf)) || (!strcmp("d_nailgrenade",buf)) || (!strcmp("d_napalmgrenade",buf)))
	{
		if (status)
			fNadeTimer = gEngfuncs.GetClientTime();
		else
			fNadeTimer = 0;
	}*/
}

CFB::CFB(void)
{
	fLastTime = 0;
	fNadeTimer = 0;
	bScoreBlue = 0;
	bScoreRed = 0;
	bScoreYellow = 0;
	bScoreGreen = 0;
	bNadeTimerOn = 0;
	bNadeTextOn = 0;
	bNadeDebugOn = 0;
	bNadeCustomXOn = 0;
	bNadeCustomYOn = 0;
	bNadeCustomHOn = 0;
	bNadeCustomWOn = 0;
	bNadeGren1 = 0;
	primed = 0;
	bSumo = 0; //bsumo!

	// begin: added by Mulchman
	teamscores[0].team = 'b';
	teamscores[1].team = 'r';
	teamscores[2].team = 'y';
	teamscores[3].team = 'g';
	// end: added by Mulchman
}

void CFB::Cmd_ToggleScoreDisplay (void)
{
	bScoreDisplayOn = bScoreDisplayOn ? 0 : 1;

	if(bScoreDisplayOn)
		gSuperHUD.WriteHUDChannel(HC_IN_GAME_COLOR, 2, "score display ON");
	else
		gSuperHUD.WriteHUDChannel(HC_IN_GAME_COLOR, 2, "score display OFF");
}

void CFB::Cmd_ToggleNadeTimer (void)
{
	if(gEngfuncs.Cmd_Argc() < 2)
	{
		bNadeTimerOn = bNadeTimerOn ? 0 : 1;
	
		gEngfuncs.pfnConsolePrint("[sparky] valid bnadetimer commands: text, x <#>, y <#>, h <#>, w <#>\n");

		if(bNadeTimerOn)
			gSuperHUD.WriteHUDChannel(HC_IN_GAME_COLOR, 2, "visual nade timer ON");
		else
			gSuperHUD.WriteHUDChannel(HC_IN_GAME_COLOR, 2, "visual nade timer OFF");
	}

	if(!strcmp(gEngfuncs.Cmd_Argv(1), "text"))
	{
		bNadeTextOn = bNadeTextOn ? 0 : 1;
		bNadeDebugOn = 0;

		if(bNadeTextOn)
			gSuperHUD.WriteHUDChannel(HC_IN_GAME_COLOR, 2, "visual nade timer (text) ON");
		else
			gSuperHUD.WriteHUDChannel(HC_IN_GAME_COLOR, 2, "visual nade timer (text) OFF");
	}

	if(!strcmp(gEngfuncs.Cmd_Argv(1), "debug"))
	{
		bNadeDebugOn = bNadeDebugOn ? 0 : 1;
		bNadeTextOn = 0;

		if(bNadeDebugOn)
			gSuperHUD.WriteHUDChannel(HC_IN_GAME_COLOR, 2, "visual nade timer (debug) ON");
		else
			gSuperHUD.WriteHUDChannel(HC_IN_GAME_COLOR, 2, "visual nade timer (debug) OFF");
	}

	if(!strcmp(gEngfuncs.Cmd_Argv(1), "x"))
	{
		if(gEngfuncs.Cmd_Argc() < 3)
		{
			char buf[256];
			sprintf(buf, "[sparky] visual nade timer (custom x = %d)\n", iNadeCustomX);
			gEngfuncs.pfnConsolePrint(buf);
			return;
		}

		if(gEngfuncs.Cmd_Argc() == 3)
		{
			if(gEngfuncs.Cmd_Argv(2) == 0)
				bNadeCustomXOn = 0;
			if(gEngfuncs.Cmd_Argv(2) > 0)
				bNadeCustomXOn = 1;
		}

		if(bNadeCustomXOn)
		{
			char buf[256];
			iNadeCustomX = atoi(gEngfuncs.Cmd_Argv(2));
			sprintf(buf, "[sparky] visual nade timer (custom x = %d)\n", iNadeCustomX);
			gEngfuncs.pfnConsolePrint(buf);
		}
	}

	if(!strcmp(gEngfuncs.Cmd_Argv(1), "y"))
	{
		if(gEngfuncs.Cmd_Argc() < 3)
		{
			char buf[256];
			sprintf(buf, "[sparky] visual nade timer (custom y = %d)\n", iNadeCustomY);
			gEngfuncs.pfnConsolePrint(buf);
			return;
		}

		if(gEngfuncs.Cmd_Argc() == 3)
		{
			if(gEngfuncs.Cmd_Argv(2) == 0)
				bNadeCustomYOn = 0;
			if(gEngfuncs.Cmd_Argv(2) > 0)
				bNadeCustomYOn = 1;
		}

		if(bNadeCustomYOn)
		{
			char buf[256];
			iNadeCustomY = atoi(gEngfuncs.Cmd_Argv(2));
			sprintf(buf, "[sparky] visual nade timer (custom y = %d)\n", iNadeCustomY);
			gEngfuncs.pfnConsolePrint(buf);
		}
	}

	if(!strcmp(gEngfuncs.Cmd_Argv(1), "h"))
	{
		if(gEngfuncs.Cmd_Argc() < 3)
		{
			char buf[256];
			sprintf(buf, "[sparky] visual nade timer (custom h = %d)\n", iNadeCustomH);
			gEngfuncs.pfnConsolePrint(buf);
			return;
		}

		if(gEngfuncs.Cmd_Argc() == 3)
		{
			if(gEngfuncs.Cmd_Argv(2) == 0)
				bNadeCustomHOn = 0;
			if(gEngfuncs.Cmd_Argv(2) > 0)
				bNadeCustomHOn = 1;
		}

		if(bNadeCustomHOn)
		{
			char buf[256];
			iNadeCustomH = atoi(gEngfuncs.Cmd_Argv(2));
			sprintf(buf, "[sparky] visual nade timer (custom h = %d)\n", iNadeCustomH);
			gEngfuncs.pfnConsolePrint(buf);
		}
	}

	if(!strcmp(gEngfuncs.Cmd_Argv(1), "w"))
	{
		if(gEngfuncs.Cmd_Argc() < 3)
		{
			char buf[256];
			sprintf(buf, "[sparky] visual nade timer (custom w = %d)\n", iNadeCustomW);
			gEngfuncs.pfnConsolePrint(buf);
			return;
		}

		if(gEngfuncs.Cmd_Argc() == 3)
		{
			if(gEngfuncs.Cmd_Argv(2) == 0)
				bNadeCustomWOn = 0;
			if(gEngfuncs.Cmd_Argv(2) > 0)
				bNadeCustomWOn = 1;
		}

		if(bNadeCustomWOn)
		{
			char buf[256];
			iNadeCustomW = atoi(gEngfuncs.Cmd_Argv(2));
			sprintf(buf, "[sparky] visual nade timer (custom w = %d)\n", iNadeCustomW);
			gEngfuncs.pfnConsolePrint(buf);
		}
	}
}

void CFB::Cmd_Sumo (void)
{
	bSumo = bSumo ? 0 : 1;
	if(bSumo)
		gEngfuncs.pfnConsolePrint("[sparky] bsumo ON");
	else
		gEngfuncs.pfnConsolePrint("[sparky] bsumo OFF");
}

void CFB::Draw_Blue_Scores (int slot, char *buf)
{
	gSuperHUD.DrawHudString(16, slot, 9, 102, 255, bluename);
	gSuperHUD.DrawHudString(50, slot, 9, 102, 255, buf);
}

void CFB::Draw_Red_Scores (int slot, char *buf)
{
	gSuperHUD.DrawHudString(16, slot, 255, 15, 9, redname);
	gSuperHUD.DrawHudString(50, slot, 255, 15, 9, buf);
}

void CFB::Draw_Yellow_Scores (int slot, char *buf)
{
	gSuperHUD.DrawHudString(16, slot, 255, 255, 9, yellowname);
	gSuperHUD.DrawHudString(50, slot, 255, 255, 9, buf);
}

void CFB::Draw_Green_Scores (int slot, char *buf)
{
	gSuperHUD.DrawHudString(16, slot, 32, 255, 9, greenname);
	gSuperHUD.DrawHudString(50, slot, 32, 255, 9, buf);
}

int CFB::FindTeamIndex(const char c)
{
	if (teamscores[0].team == c)
		return 0;
	else if (teamscores[1].team == c)
		return 1;
	else if (teamscores[2].team == c)
		return 2;
	else if (teamscores[3].team == c)
		return 3;
	else
		return 0;
}
