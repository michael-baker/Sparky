/*  ------------------------------------------------------------------------
	  flagstatus.cpp by ruiner@konspiracy.org
		- added v1.33 - 01/12/03
	------------------------------------------------------------------------ */

#include "..\client.h"
#include "util.h"
#include "hook.h"
#include "superhud.h"
#include "flagstatus.h"
#include "help.h"

/*Global*/
CFlagStatus gFlagStatus;

DECLARE_COMMAND_2(gFlagStatus, ToggleFlagStatus);

int tFlagStatus[2];

bool CFlagStatus::Pre_HUD_Init (void)
{
	HOOK_COMMAND_2("bflagstatus", ToggleFlagStatus);
	gHelp.addCommand("bflagstatus", "Toggles display of the flag status (dropped/base/carried).\nUsage: bflagstatus");
	return true;
}

void CFlagStatus::Cmd_ToggleFlagStatus (void)
{
	bFlagStatusOn = bFlagStatusOn ? 0 : 1;

	if(bFlagStatusOn)
		gSuperHUD.WriteHUDChannel(HC_IN_GAME_COLOR, 2, "flag status ON");
	else
		gSuperHUD.WriteHUDChannel(HC_IN_GAME_COLOR, 2, "flag status OFF");
}

void CFlagStatus::Post_HUD_Frame (double time)
{
	if((fLastTime-10) > gEngfuncs.GetClientTime())
	{
		tFlagStatus[1] = 0;
		tFlagStatus[2] = 0;
		teamscores[1].team = 1;
		teamscores[2].team = 2;
		teamscores[1].points = 0;
		teamscores[2].points = 0;
	}
	fLastTime = gEngfuncs.GetClientTime();
}

bool CFlagStatus::Pre_Hook_ConsoleCommand (char *pszString)
{

	// You/Your team got the flag
	if (strstr(pszString,"spk")) {
		cl_entity_t *pLocal = gEngfuncs.GetLocalPlayer();

		if ((strstr(pszString, "ctf_yougotflag"))
			|| (strstr(pszString, "ctf_gotflag"))
			|| (strstr(pszString, "rk_gotkey"))
			|| (strstr(pszString, "rk_takekey"))
			|| (strstr(pszString, "youhave")) // mulch_trench
			|| (strstr(pszString, "teamhas")) // mulch_trench
			) 
			if(pLocal->curstate.team == 1) {
				tFlagStatus[2] = 1;
				sprintf(redFlagStatus, "carried");
			} else {
				tFlagStatus[1] = 1;
				sprintf(blueFlagStatus, "carried");
			}

		// enemy flag returns / is caped
		if ((strstr(pszString, "ctf_youcap"))
			|| (strstr(pszString, "ctf_teamcap"))
			|| (strstr(pszString, "rk_gotkey"))
			|| (strstr(pszString, "ctf_eflagback"))
			|| ((strstr(pszString, "rk_bscore") && pLocal->curstate.team == 1))
			|| ((strstr(pszString, "rk_rscore") && pLocal->curstate.team == 2))
			|| ((strstr(pszString, "fball_bscore") && pLocal->curstate.team == 1))
			|| ((strstr(pszString, "fball_rscore") && pLocal->curstate.team == 2))
			|| (strstr(pszString, "rk_ekeyback"))
			|| (strstr(pszString, "enemyret")) // mulch_trench
			|| (strstr(pszString, "teamcap")) // mulch_trench
			|| (strstr(pszString, "youcap")) // mulch_trench
			) 
			if(pLocal->curstate.team == 1) {
				tFlagStatus[2] = 1;
				sprintf(redFlagStatus, "base");
			} else {
				tFlagStatus[1] = 1;
				sprintf(blueFlagStatus, "base");
			}

		// Enemy got your flag
		if ((strstr(pszString, "ctf_lostflag"))
			|| (strstr(pszString, "rk_lostkey"))
			|| (strstr(pszString, "enemyhas")) // mulch_trench
			) 
			if(pLocal->curstate.team == 1) {
				tFlagStatus[1] = 1;
				sprintf(blueFlagStatus, "carried");
			} else {
				tFlagStatus[2] = 1;
				sprintf(redFlagStatus, "carried");
			}

		// Our flag returns / is caped
		if ((strstr(pszString, "ctf_theycap"))
			|| (strstr(pszString, "rk_keyback"))
			|| (strstr(pszString, "ctf_flagback"))
			|| ((strstr(pszString, "rk_bscore") && pLocal->curstate.team == 2))
			|| ((strstr(pszString, "rk_rscore") && pLocal->curstate.team == 1))
			|| ((strstr(pszString, "fball_bscore") && pLocal->curstate.team == 2))
			|| ((strstr(pszString, "fball_rscore") && pLocal->curstate.team == 1))
			|| (strstr(pszString, "enemycap")) // mulch_trench
			|| (strstr(pszString, "yourret")) // mulch_trench
			) 
			if(pLocal->curstate.team == 1) {
				tFlagStatus[1] = 1;
				sprintf(blueFlagStatus, "home");
			} else {
				tFlagStatus[2] = 1;
				sprintf(redFlagStatus, "home");
			}
	}
	return true;
}

void CFlagStatus::Post_Hook_HudText (const char *pszString)
{
	cl_entity_t *pLocal = gEngfuncs.GetLocalPlayer();

	// Our flag is dropped
	if(	   ( !strcmp (pszString, "2fort_enemy_lost_flag") )
		|| ( !strcmp (pszString, "rock_enemy_dropped_key") ))
	{
		if(pLocal->curstate.team == 1) {
			tFlagStatus[1] = 1;
			sprintf(blueFlagStatus, "dropped");
		} else {
			tFlagStatus[2] = 1;
			sprintf(redFlagStatus, "dropped");
		}
	}

	// Enemy flag is dropped
	if(    ( !strcmp (pszString, "2fort_lost_enemy_flag") )
        || ( !strcmp (pszString, "rock_team_dropped_key") )
		)
	{
		if(pLocal->curstate.team == 2) {
			tFlagStatus[1] = 1;
			sprintf(blueFlagStatus, "dropped");
		} else {
			tFlagStatus[2] = 1;
			sprintf(redFlagStatus, "dropped");
		}
	}
}

void CFlagStatus::hook_teamscore(char *tbuf, int points)
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

void CFlagStatus::Post_HUD_Redraw (float flTime, int intermission)
{
	if((gSuperHUD.iSuperHUDLevel >= 1) && (bFlagStatusOn))
	{
		int ixRight = 86; // display it next to score
		int iyBlue, iyRed;
		int iBlueR	= 9;	int iBlueG	= 102;	int iBlueB	= 255;
		int iRedR	= 255;	int iRedG	= 15;	int iRedB	= 9;
		int iyLaser = int(gSuperHUD.m_scrinfo.iHeight*0.18); // score display base y

		if(teamscores[1].team == 1) {
			iyBlue	= iyLaser;
			iyRed	= iyLaser + gSuperHUD.m_scrinfo.iCharHeight - 3;
		} else {
			iyRed	= iyLaser;	
			iyBlue	= iyLaser + gSuperHUD.m_scrinfo.iCharHeight - 3;
		}

		if(tFlagStatus[1])
			gSuperHUD.DrawHudString(ixRight, iyBlue, iBlueR, iBlueG, iBlueB, "%s", blueFlagStatus);
		if(tFlagStatus[2])
			gSuperHUD.DrawHudString(ixRight, iyRed, iRedR, iRedG, iRedB, "%s", redFlagStatus);
	}
}

CFlagStatus::CFlagStatus(void)
{
	tFlagStatus[1] = 0;
	tFlagStatus[2] = 0;
	teamscores[1].team = 1;
	teamscores[2].team = 2;
	teamscores[1].points = 0;
	teamscores[2].points = 0;
}
