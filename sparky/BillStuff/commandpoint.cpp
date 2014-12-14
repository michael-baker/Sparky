/*  ------------------------------------------------------------------------
	  commandpoint.cpp by fryguy, fb, and mulchman
	    - updated v1.38 - 03/01/03 - fb: cp detting support
		- added v1.31
	------------------------------------------------------------------------ */

#include "..\client.h"

#include "hook.h"
#include "superhud.h"
#include "commandpoint.h"
#include "..\hookmessages.h"
#include "logging.h"
#include "help.h"

/*Global*/
CCommandPoint gCommandPoint;

DECLARE_COMMAND_2(gCommandPoint, ToggleCommandPoint);

CCommandPoint::CCommandPoint()
{
	for (int i=0; i<=iCommandPoints; i++)
		cpteam[i] = 0;

	bCommandPointOn = 0;
	bMasterCommandPointOn = 0;
}

bool CCommandPoint::Pre_HUD_Init (void)
{
	HOOK_COMMAND_2("bcommandpoint", ToggleCommandPoint);
	gHelp.addCommand("bcommandpoint", "Toggles display of commandpoint status on territorial maps. i.e. pipezone or cz2.\nUsage: bcommandpoint");

	return true;
}

void CCommandPoint::Post_HUD_Frame (double time)
{
	char map[256];
	bScrummage = 0; // added by Mulchman
	strcpy(map, gEngfuncs.pfnGetLevelName());

	if((strstr(map, "warpath")) || (strstr(map, "tiger")) || (strstr(map, "cz2")))
	{ 
		bCommandPointOn = 1; 
		iCommandPoints = 5; 
	}

	if(strstr(map, "canalzon"))
	{ 
		bCommandPointOn = 1; 
		iCommandPoints = 8; 
	}

	if((strstr(map, "pipezone")) || (strstr(map, "flagrun"))) // flagrun added by Mulchman
	{ 
		bCommandPointOn = 1; 
		iCommandPoints = 4; 
	}

	// Beg: added by Mulchman	
	if(strstr(map, "scrummage"))
	{
		bCommandPointOn = 1;
		bScrummage = 1;
		iCommandPoints = 3;
	}
	// End: added by Mulchman

	//char buf[256];
	//sprintf(buf, "map: %s bCommandPointOn: %i iCommandPoints: %i", map, bCommandPointOn, iCommandPoints);
	//gSuperHUD.WriteHUDChannel(HC_IN_GAME_COLOR, 2, buf);

	if((fLastTime-10) > gEngfuncs.GetClientTime())
	{
		bCommandPointOn = 0;
		for (int i=0; i<=iCommandPoints; i++)
			cpteam[i] = 0;
	}
	fLastTime = gEngfuncs.GetClientTime();
}

void CCommandPoint::Post_HUD_Redraw (float flTime, int intermission)
{
	//return;
	char buf[80];

	if (bMasterCommandPointOn && (bCommandPointOn && (gSuperHUD.iSuperHUDLevel >= 1)))
	{
		// beg: Added by Mulchman
		if ( bScrummage )
		{
			switch ( cpteam[1] ) // blue arena
			{
			case 0: gSuperHUD.DrawHudString(384-60+15*1, 15, 255, 255, 255, "B"); break;
			case 1: gSuperHUD.DrawHudString(384-60+15*1, 15, 9, 102, 255, "B");   break;
			case 2: gSuperHUD.DrawHudString(384-60+15*1, 15, 255, 15, 9, "B");    break;
			}
			switch ( cpteam[2] ) // center arena
			{
			case 0: gSuperHUD.DrawHudString(384-60+15*2, 15, 255, 255, 255, "C"); break;
			case 1: gSuperHUD.DrawHudString(384-60+15*2, 15, 9, 102, 255, "C");   break;
			case 2: gSuperHUD.DrawHudString(384-60+15*2, 15, 255, 15, 9, "C");    break;
			}
			switch ( cpteam[3] ) // red arena
			{
			case 0: gSuperHUD.DrawHudString(384-60+15*3, 15, 255, 255, 255, "R"); break;
			case 1: gSuperHUD.DrawHudString(384-60+15*3, 15, 9, 102, 255, "R");   break;
			case 2: gSuperHUD.DrawHudString(384-60+15*3, 15, 255, 15, 9, "R");    break;
			}
		} // end: Added by Mulchman
		else
		{		
			for (int i=1; i<=iCommandPoints; i++) 
			{
				sprintf(buf, "%d", i);
				if (cpteam[i]==0)
					gSuperHUD.DrawHudString(384-60+15*i, 15, 255,255,255, buf);
				else if (cpteam[i]==1)
					gSuperHUD.DrawHudString(384-60+15*i, 15, 9, 102, 255, buf);
				else if (cpteam[i]==2)
					gSuperHUD.DrawHudString(384-60+15*i, 15, 255, 15, 9, buf);
			}
		}
	}
}

void CCommandPoint::Post_Hook_HudText (const char *pszString)
{
	if (!strcmp(pszString, "warpath_gates_open")
		|| !strcmp(pszString, "warpath_10_secs")
		|| !strcmp(pszString, "warpath_30_secs")
		|| !strcmp(pszString, "warpath_blue_wins") 
		|| !strcmp(pszString, "warpath_red_wins"))
		{ cpteam[1]=1; cpteam[2]=0; cpteam[3]=0; cpteam[4]=0; cpteam[5]=2; }
	
	if (!strcmp(pszString, "cz_cap_all_1") 
		|| !strcmp(pszString, "cz_cap_all_2") 
		|| !strcmp(pszString, "LUE GETS QUAD DAMAGE!")
		|| !strcmp(pszString, "ED GETS QUAD DAMAGE!\n"))
	{
		for (int i=0; i<=iCommandPoints; i++)
			cpteam[i] = 0;
	}

	//pipezone
	if (strstr(pszString, "LUE captures CP1"))
		{ cpteam[1]=1; bTeamCapFix = 1; }
	else if (strstr(pszString, "LUE captures CP2"))
		{ cpteam[2]=1; bTeamCapFix = 1; }
	else if (strstr(pszString, "LUE captures CP3"))
		{ cpteam[3]=1; bTeamCapFix = 1; }
	else if (strstr(pszString, "LUE captures CP4"))
		{ cpteam[4]=1; bTeamCapFix = 1; }
	else if (strstr(pszString, "ED captures CP1"))
		{ cpteam[1]=2; bTeamCapFix = 1; }
	else if (strstr(pszString, "ED captures CP2"))
		{ cpteam[2]=2; bTeamCapFix = 1; }
	else if (strstr(pszString, "ED captures CP3"))
		{ cpteam[3]=2; bTeamCapFix = 1; }
	else if (strstr(pszString, "ED captures CP4"))
		{ cpteam[4]=2; bTeamCapFix = 1; }

	// beg: flagrun by Mulchman
	cl_entity_t *pLocal = gEngfuncs.GetLocalPlayer();

	if (strstr(pszString, "ed team secured all 4 flags")
		|| strstr(pszString, "lue team secured all 4 flags"))
	{
		cpteam[4] = 1;	// reset command points
		cpteam[3] = 0;	// reset command points
		cpteam[2] = 0;	// reset command points
		cpteam[1] = 2;	// reset command points
	}
	else if (strstr(pszString, "war_usec_flag1") && (pLocal->curstate.team == 2))
		{ cpteam[1] = 2; }
	else if (strstr(pszString, "war_usec_flag1") && (pLocal->curstate.team == 1))
		{ cpteam[1] = 1; }
	else if (strstr(pszString, "war_usec_flag2") && (pLocal->curstate.team == 2))
		{ cpteam[2] = 2; }
	else if (strstr(pszString, "war_usec_flag2") && (pLocal->curstate.team == 1))
		{ cpteam[3] = 1; }
	else if (strstr(pszString, "war_usec_flag3") && (pLocal->curstate.team == 2))
		{ cpteam[3] = 2; }
	else if (strstr(pszString, "war_usec_flag3") && (pLocal->curstate.team == 1))
		{ cpteam[3] = 1; }
	else if (strstr(pszString, "war_usec_flag4") && (pLocal->curstate.team == 2))
		{ cpteam[4] = 2; }
	else if (strstr(pszString, "war_usec_flag4") && (pLocal->curstate.team == 1))
		{ cpteam[4] = 1; }
	else if (strstr(pszString, "war_esec_flag1") && (pLocal->curstate.team == 1))
		{ cpteam[1] = 2; }
	else if (strstr(pszString, "war_esec_flag1") && (pLocal->curstate.team == 2))
		{ cpteam[1] = 1; }
	else if (strstr(pszString, "war_esec_flag2") && (pLocal->curstate.team == 1))
		{ cpteam[2] = 2; }
	else if (strstr(pszString, "war_esec_flag2") && (pLocal->curstate.team == 2))
		{ cpteam[2] = 1; }
	else if (strstr(pszString, "war_esec_flag3") && (pLocal->curstate.team == 1))
		{ cpteam[3] = 2; }
	else if (strstr(pszString, "war_esec_flag3") && (pLocal->curstate.team == 2))
		{ cpteam[3] = 1; }
	else if (strstr(pszString, "war_esec_flag4") && (pLocal->curstate.team == 1))
		{ cpteam[4] = 2; }
	else if (strstr(pszString, "war_esec_flag4") && (pLocal->curstate.team == 2))
		{ cpteam[4] = 1; }	
	else if (strstr(pszString, "war_u_stole_1"))
		{ cpteam[1] = 0; }
	else if (strstr(pszString, "war_u_stole_2"))
		{ cpteam[2] = 0; }
	else if (strstr(pszString, "war_u_stole_3"))
		{ cpteam[3] = 0; }
	else if (strstr(pszString, "war_u_stole_4"))
		{ cpteam[4] = 0; }
	else if (strstr(pszString, "war_en_stole_1"))
		{ cpteam[1] = 0; }
	else if (strstr(pszString, "war_en_stole_2"))
		{ cpteam[2] = 0; }
	else if (strstr(pszString, "war_en_stole_3"))
		{ cpteam[3] = 0; }
	else if (strstr(pszString, "war_en_stole_4"))
		{ cpteam[4] = 0; }
	else if (strstr(pszString, "war_tm_stole_1"))
		{ cpteam[1] = 0; }
	else if (strstr(pszString, "war_tm_stole_2"))
		{ cpteam[2] = 0; }
	else if (strstr(pszString, "war_tm_stole_3"))
		{ cpteam[3] = 0; }
	else if (strstr(pszString, "war_tm_stole_4"))
		{ cpteam[4] = 0; }
	else if (strstr(pszString, "war_uret_flag1") && (pLocal->curstate.team == 2))
		{ cpteam[1] = 2; }
	else if (strstr(pszString, "war_uret_flag1") && (pLocal->curstate.team == 1))
		{ cpteam[1] = 1; }
	else if (strstr(pszString, "war_uret_flag2") && (pLocal->curstate.team == 2))
		{ cpteam[2] = 2; }
	else if (strstr(pszString, "war_uret_flag2") && (pLocal->curstate.team == 1))
		{ cpteam[2] = 1; }
	else if (strstr(pszString, "war_uret_flag3") && (pLocal->curstate.team == 2))
		{ cpteam[3] = 2; }
	else if (strstr(pszString, "war_uret_flag3") && (pLocal->curstate.team == 1))
		{ cpteam[3] = 1; }
	else if (strstr(pszString, "war_uret_flag4") && (pLocal->curstate.team == 2))
		{ cpteam[4] = 2; }
	else if (strstr(pszString, "war_uret_flag4") && (pLocal->curstate.team == 1))
		{ cpteam[4] = 1; }
	else if (strstr(pszString, "war_eret_flag1") && (pLocal->curstate.team == 1))
		{ cpteam[1] = 2; }
	else if (strstr(pszString, "war_eret_flag1") && (pLocal->curstate.team == 2))
		{ cpteam[1] = 1; }
	else if (strstr(pszString, "war_eret_flag2") && (pLocal->curstate.team == 1))
		{ cpteam[2] = 2; }
	else if (strstr(pszString, "war_eret_flag2") && (pLocal->curstate.team == 2))
		{ cpteam[2] = 1; }
	else if (strstr(pszString, "war_eret_flag3") && (pLocal->curstate.team == 1))
		{ cpteam[3] = 2; }
	else if (strstr(pszString, "war_eret_flag3") && (pLocal->curstate.team == 2))
		{ cpteam[3] = 1; }
	else if (strstr(pszString, "war_eret_flag4") && (pLocal->curstate.team == 1))
		{ cpteam[4] = 2; }
	else if (strstr(pszString, "war_eret_flag5") && (pLocal->curstate.team == 2))
		{ cpteam[4] = 1; }
	// end: flagrun by Mulchman
}

void CCommandPoint::Post_Hook_TextMsg (const char *buf) {
	//add_log("textmsg: %s", buf);
	//add_log("testmsg: %s", "%s [Red Team] captures Command Point 8");

	cl_entity_t *pLocal = gEngfuncs.GetLocalPlayer(); // added by Mulchman
	
	if (!strcmp(buf, "#cz_bcap1") ||
		!strcmp(buf, "%s [Blue Team] captures Command Point 1\n"))
		{ cpteam[1]=1; bTeamCapFix = 1; }
	else if (!strcmp(buf, "#cz_bcap2") ||
			!strcmp(buf, "%s [Blue Team] captures Command Point 2\n"))
		{ cpteam[2]=1; bTeamCapFix = 1; }
	else if (!strcmp(buf, "#cz_bcap3") ||
			!strcmp(buf, "%s [Blue Team] captures Command Point 3\n"))
		{ cpteam[3]=1; bTeamCapFix = 1; }
	else if (!strcmp(buf, "#cz_bcap4") ||
			!strcmp(buf, "%s [Blue Team] captures Command Point 4\n"))
		{ cpteam[4]=1; bTeamCapFix = 1; }
	else if (!strcmp(buf, "#cz_bcap5") ||
			!strcmp(buf, "%s [Blue Team] captures Command Point 5\n"))
		{ cpteam[5]=1; bTeamCapFix = 1; }
	else if (!strcmp(buf, "#cz_bcap6") ||
			!strcmp(buf, "%s [Blue Team] captures Command Point 6\n"))
		{ cpteam[6]=1; bTeamCapFix = 1; }
	else if (!strcmp(buf, "#cz_bcap7") ||
			!strcmp(buf, "%s [Blue Team] captures Command Point 7\n"))
		{ cpteam[7]=1; bTeamCapFix = 1; }
	else if (!strcmp(buf, "#cz_bcap8") ||
			!strcmp(buf, "%s [Blue Team] captures Command Point 8\n"))
		{ cpteam[8]=1; bTeamCapFix = 1; }
	else if (!strcmp(buf, "#cz_rcap1") ||
			!strcmp(buf, "%s [Red Team] captures Command Point 1\n"))
		{ cpteam[1]=2; bTeamCapFix = 1; }
	else if (!strcmp(buf, "#cz_rcap2") ||
			!strcmp(buf, "%s [Red Team] captures Command Point 2\n"))
		{ cpteam[2]=2; bTeamCapFix = 1; }
	else if (!strcmp(buf, "#cz_rcap3") ||
			!strcmp(buf, "%s [Red Team] captures Command Point 3\n"))
		{ cpteam[3]=2; bTeamCapFix = 1; }
	else if (!strcmp(buf, "#cz_rcap4") ||
			!strcmp(buf, "%s [Red Team] captures Command Point 4\n"))
		{ cpteam[4]=2; bTeamCapFix = 1; }
	else if (!strcmp(buf, "#cz_rcap5") ||
			!strcmp(buf, "%s [Red Team] captures Command Point 5\n"))
		{ cpteam[5]=2; bTeamCapFix = 1; }
	else if (!strcmp(buf, "#cz_rcap6") ||
			!strcmp(buf, "%s [Red Team] captures Command Point 6\n"))
		{ cpteam[6]=2; bTeamCapFix = 1; }
	else if (!strcmp(buf, "#cz_rcap7") ||
			!strcmp(buf, "%s [Red Team] captures Command Point 7\n"))
		{ cpteam[7]=2; bTeamCapFix = 1; }
	else if (!strcmp(buf, "#cz_rcap8") ||
			!strcmp(buf, "%s [Red Team] captures Command Point 8\n"))
		{ cpteam[8]=2; bTeamCapFix = 1; }
	else if (!strcmp(buf, "BLUE captures 1!\n")) //pipezone backups
		{ cpteam[1]=1; bTeamCapFix = 1; }
	else if (!strcmp(buf, "BLUE captures 2!\n"))
		{ cpteam[2]=1; bTeamCapFix = 1; }
	else if (!strcmp(buf, "BLUE captures 3!\n"))
		{ cpteam[3]=1; bTeamCapFix = 1; }
	else if (!strcmp(buf, "BLUE captures 4!\n"))
		{ cpteam[4]=1; bTeamCapFix = 1; }
	else if (!strcmp(buf, "RED captures 1!\n"))
		{ cpteam[1]=2; bTeamCapFix = 1; }
	else if (!strcmp(buf, "RED captures 2!\n"))
		{ cpteam[2]=2; bTeamCapFix = 1; }
	else if (!strcmp(buf, "RED captures 3!\n"))
		{ cpteam[3]=2; bTeamCapFix = 1; }
	else if (!strcmp(buf, "RED captures 4!\n"))
		{ cpteam[4]=2; bTeamCapFix = 1; }
	// beg: flagrun by Mulchman
	else if ((strstr(buf, "#war_tmsec_flag1") && (pLocal->curstate.team == 1)))
		{ cpteam[1] = 1; }
	else if ((strstr(buf, "#war_tmsec_flag1") && (pLocal->curstate.team == 2)))
		{ cpteam[1] = 2; }
	else if ((strstr(buf, "#war_tmsec_flag2") && (pLocal->curstate.team == 1)))
		{ cpteam[2] = 1; }
	else if ((strstr(buf, "#war_tmsec_flag2") && (pLocal->curstate.team == 2)))
		{ cpteam[2] = 2; }
	else if ((strstr(buf, "#war_tmsec_flag3") && (pLocal->curstate.team == 1)))
		{ cpteam[3] = 1; }
	else if ((strstr(buf, "#war_tmsec_flag3") && (pLocal->curstate.team == 2)))
		{ cpteam[3] = 2; }
	else if ((strstr(buf, "#war_tmsec_flag4") && (pLocal->curstate.team == 1)))
		{ cpteam[4] = 1; }
	else if ((strstr(buf, "#war_tmsec_flag4") && (pLocal->curstate.team == 2)))
		{ cpteam[4] = 2; }
	// end: flagrun by Mulchman
	// beg: scrummage by Mulchman
	else if (strstr(buf, "Red Team Captures the Red Arena!"))
		{ cpteam[3] = 2; }
	else if (strstr(buf, "Red Team Captures the Center Arena!"))
		{ cpteam[2] = 2; cpteam[3] = 2; }
	else if (strstr(buf, "Red Team Captures the Blue Arena!"))
		{ cpteam[1] = 2; cpteam[2] = 2; cpteam[3] = 2; }
	else if (strstr(buf, "Blue Team Captures the Blue Arena!"))
		{ cpteam[1] = 1; }
	else if (strstr(buf, "Blue Team Captures the Center Arena!"))
		{ cpteam[2] = 1; cpteam[1] = 1; }
	else if (strstr(buf, "Blue Team Captures the Red Arena!"))
		{ cpteam[3] = 1; cpteam[2] = 1; cpteam[1] = 1; }
	else if (strstr(buf, "SCRUMMED!"))
		{ cpteam[1] = 0; cpteam[2] = 0; cpteam[3] = 0; }
	// end: scrummage by Mulchman
	else if (strstr(buf, "#cz_b_cc_destroy_"))
	{
		for (int i = 1; i<=iCommandPoints; i++)
			if (cpteam[i] == 1)
				cpteam[i] = 0;
	}
	else if (strstr(buf, "#cz_r_cc_destroy_"))
	{
		for (int i = 1; i<=iCommandPoints; i++)
			if (cpteam[i] == 2)
				cpteam[i] = 0;
	}
}

void CCommandPoint::Cmd_ToggleCommandPoint( )
{
	bMasterCommandPointOn = bMasterCommandPointOn ? 0 : 1;

	if(bMasterCommandPointOn)
		gSuperHUD.WriteHUDChannel(HC_IN_GAME_COLOR, 2, "Command Point display ON");
	else
		gSuperHUD.WriteHUDChannel(HC_IN_GAME_COLOR, 2, "Command Point display OFF");
}

void CCommandPoint::SetbTeamCapFix( bool b ) // added by Mulchman
{
	bTeamCapFix = b;
}

bool CCommandPoint::GetbTeamCapFix( ) // added by Mulchman
{
	return bTeamCapFix;
}

bool CCommandPoint::GetbMasterCommandPointOn( ) // added by Mulchman
{
	return bMasterCommandPointOn;
}

bool CCommandPoint::GetbCommandPointOn( ) // added by Mulchman
{
	return bCommandPointOn;
}