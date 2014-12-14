/*  ------------------------------------------------------------------------
	  triggers.cpp by billdoor; additions by ruiner, FB, and Mulchman
		- updated v1.42 - 04/11/03 - ruiner: btrigger_humiliate stolen from fr33
	    - updated v1.40 - 03/31/03 - Mulchman: bunches...
	    - updated v1.38 - 02/28/03 - Mulchman: sucide/killsg/youkillflagcarrier/%en/%ec/youtakeflag
		- added v1.0
	------------------------------------------------------------------------ */

#include "..\client.h"

#include "hook.h"
#include "superhud.h"
#include "triggers.h"
#include "util.h"
#include <time.h>
#include "logging.h"
#include "rulesmanager.h"
#include "commandpoint.h" // added by Mulchman
#include "fb.h" // added by Mulchman
#include "location.h"
#include "help.h"

/*Global*/
CTriggers gTriggers;

bool CTriggers::Pre_HUD_Init (void)
{
	gHelp.addCommand("btrigger_endofround", "Triggered when round ends.\nExample:\nalias btrigger_endofround \"say gg!\"");
	gHelp.addCommand("btrigger_spawn", "Triggered when you spawn.\nExample:\nalias btrigger_spawn \"tf_weapon_supershotgun\"");
	gHelp.addCommand("btrigger_kill", "Triggered when you kill someone - annoying spam.\nExample:\nalias btrigger_kill \"say_team Killed %en [%ec]\"");
	gHelp.addCommand("btrigger_die", "Triggered when you die - annoying spam.\nExample:\nalias btrigger_die \"say_team I'm dead\"");
	gHelp.addCommand("btrigger_newserver", "Triggered with a new server.\n");
	gHelp.addCommand("btrigger_newmap", "Triggered when a new map is loaded.\nExample:\nalias btrigger_newmap \"say_team new map time\"");
	gHelp.addCommand("btrigger_enemycap", "Triggered when the enemy caps - annoying spam.\nExample:\nalias btrigger_enemycap \"say_team omg, the enemy capped\"");
	gHelp.addCommand("btrigger_teamcap", "Triggered when your team caps - annoying spam.\nExample:\nalias btrigger_teamcap \"say_team yay, we capped\"");
	gHelp.addCommand("btrigger_youdropflag", "Spamless flag dropping notification.\nExample:\nalias btrigger_youdropflag \"say_team Flag dropped %l\"");
	gHelp.addCommand("btrigger_timeleft", "Triggered when a teammate says timeleft.\nExample:\nalias btrigger_timeleft \"say_team %t\"");
	gHelp.addCommand("btrigger_killsg", "Triggered when you kill a sentry gun.\nExample:\nalias btrigger_killsg \"say_team I just killed a sg\"");
	gHelp.addCommand("btrigger_suicide", "Triggered when you suicide.\nExample:\nalias btrigger_suicide \"say_team [ suicide ]\"");
	gHelp.addCommand("btrigger_youkillflagcarrier", "Triggered when you kill the flag carrier - innacurate.\nExample:\nalias btrigger_youkillflagcarrier \"say_team Flag carrier - dead\"");
	gHelp.addCommand("btrigger_yougotflag", "Triggered when you get the enemy flag.\nExample:\nalias btrigger_yougotflag \"say_team I have the flag\"");
	gHelp.addCommand("btrigger_enemycap", "Triggered when a sentry gun kills you.\nExample:\nalias btrigger_diesg \"say_team %kn's SG UP\"");
	gHelp.addCommand("btrigger_humiliate", "Triggered when you kill someone with an embarassing weapon.\nExample:\nalias btrigger_humiliate \"say owned!; snapshot\"");
	gHelp.addCommand("btrigger_irc_connect", "Triggered when you connect to a server with the IRC plugin.");
	gHelp.addCommand("btrigger_feigning", "Triggered when you feign (must be spy...).\nExample:\nalias btrigger_feigning \"say_team feigning @ %l\"");
	gHelp.addCommand("btrigger_settingdetpack", "Triggered when you set a detpack.\nExample:\nalias btrigger_settingdetpack \"say_team setting detpack @ %l\"");
	
	return true;
}

void CTriggers::Post_HUD_Init (void)
{
}

bool CTriggers::Pre_HUD_Frame (double time)
{
	return true;
}

void CTriggers::Post_HUD_Frame (double time)
{
	if(bEndOfRound)
	{
		bEndOfRound = 0;
		gEngfuncs.pfnClientCmd("btrigger_endofround;");
	}
	if(bJustSpawned)
	{
		bJustSpawned = 0;
		gEngfuncs.pfnClientCmd("btrigger_spawn;");
	}
	if(bJustKilled)
	{
		bJustKilled = 0;
		gEngfuncs.pfnClientCmd("btrigger_kill;");
	}
	if(bJustDied)
	{
		bJustDied = 0;
		gEngfuncs.pfnClientCmd("btrigger_die;");
	}
	if(bNewServer)
	{
		bNewServer = 0;
		gEngfuncs.pfnClientCmd("btrigger_newserver;");
	}
	if(bNewMap)
	{
		bNewMap = 0;
		gEngfuncs.pfnClientCmd("btrigger_newmap;");
	}
	if(bEnemyCap)
	{
		bEnemyCap = 0;
		gEngfuncs.pfnClientCmd("btrigger_enemycap;");
	}
	if(bTeamCap)
	{
		bTeamCap = 0;
		gEngfuncs.pfnClientCmd("btrigger_teamcap;");
	}
	if(bYouDropFlag)
	{
		bYouDropFlag = 0;

		if (gCommandPoint.GetbMasterCommandPointOn( ) || gCommandPoint.GetbCommandPointOn( ))
		{
			if ((!bTeamCapFix) && (!gCommandPoint.GetbTeamCapFix()))
				gEngfuncs.pfnClientCmd("btrigger_youdropflag;");
			bTeamCapFix = 0;
			gCommandPoint.SetbTeamCapFix( 0 );
		}
		else
		{
			if (!bTeamCapFix)
				gEngfuncs.pfnClientCmd("btrigger_youdropflag;");					
			bTeamCapFix = 0;
		}
	}
	if(bTimeleft)
	{
		bTimeleft = 0;
		gEngfuncs.pfnClientCmd("btrigger_timeleft;");
	}

	if((fLastTime-10) > gEngfuncs.GetClientTime())
	{
		bNewMap = 1;
	}
	fLastTime = gEngfuncs.GetClientTime();

	//mulch addition, v1.38
	if(bJustKilledSG)
	{
		bJustKilledSG = 0;
		gEngfuncs.pfnClientCmd("btrigger_killsg;");
	}
	if(bJustSuicided)
	{
		bJustSuicided = 0;
		gEngfuncs.pfnClientCmd("btrigger_suicide;");
	}
	if(bJustKilledFlagCarrier)
	{
		if(bEnemyLostFlag)
			gEngfuncs.pfnClientCmd("btrigger_youkillflagcarrier;");
		bJustKilledFlagCarrier = 0;
		bEnemyLostFlag = 0; 
	}
	if(bYouGotFlag)
	{
		bYouGotFlag = 0;
		gEngfuncs.pfnClientCmd("btrigger_yougotflag;");
	}
	if(bJustDiedSG)
	{
		bJustDiedSG = 0;
		gEngfuncs.pfnClientCmd("btrigger_diesg;");
	}
	//end mulch addition, v1.38

	// beg: Fr33 addition v1.42
	if(bHumiliate)
	{
		bHumiliate = 0;
		gEngfuncs.pfnClientCmd("btrigger_humiliate;");
	}
	// end: Fr33 addition v1.42

	// Mulch additions 1.43
	if ( bFeigning )
	{
		bFeigning = false;
		gEngfuncs.pfnClientCmd( "btrigger_feigning;" );
	}
	if ( bSettingDetpack )
	{
		bSettingDetpack = false;
		gEngfuncs.pfnClientCmd( "btrigger_settingdetpack;" );
	}
}

bool CTriggers::Pre_Hook_TextMsg (const char *pszString)
{
	return true;
}

void CTriggers::Post_Hook_TextMsg (const char *pszString)
{
	if(!strcmp(pszString, "#Match_results"))
	{
		bEndOfRound = 1;
		sprintf(victclass, "NULL");
		sprintf(victname, "NULL");
		sprintf(killclass, "NULL");
		sprintf(killname, "NULL");
	}	
}

bool CTriggers::Pre_Hook_SayText (const char *pszString)
{
	return true;
}

void CTriggers::Post_Hook_SayText (const char *pszString)
{
	const char *p;
	if(gRulesManager.fLastTriggerTime > gEngfuncs.GetClientTime())
		gRulesManager.fLastTriggerTime = gEngfuncs.GetClientTime();
	if((gEngfuncs.GetClientTime() - gRulesManager.fLastTriggerTime) > 21)
	{
		if(strlen(pszString) > 8)
		{
			p = pszString;// + strlen(pszString) - 7;
			if(strstr(p, "(TEAM)") && strstr(p, "timeleft\n"))
			{
				bTimeleft = 1;
			}
		}
	}
}

bool CTriggers::Pre_Hook_DeathMsg (int killer, int victim, const char *pszWeapon)
{
	return true;
}

void CTriggers::Post_Hook_DeathMsg (int killer, int victim, const char *pszWeapon)
{
	cl_entity_t *pLocal = gEngfuncs.GetLocalPlayer();
	hud_player_info_t victiminfo;
	hud_player_info_t killerinfo;

	if (( victim == killer ) && ( victim == pLocal->index ))
	{
		bJustSuicided = 1;
	}
	else
	{
		if ( victim == pLocal->index )
		{
			bJustDied = 1;

			cl_entity_t *kEnt = gEngfuncs.GetEntityByIndex( killer );
			if (( kEnt != NULL ) && ( kEnt->player ))
			{
				gEngfuncs.pfnGetPlayerInfo( killer, &killerinfo );
				sprintf( killname, "%s", killerinfo.name );
				sprintf( killclass, "%s", killerinfo.model );
			}			

			if ( !strcmp( pszWeapon, "sentrygun" ) )
			{
				//sprintf( killname, "N/A" );
				bJustDied = 0;
				bJustDiedSG = 1;
				sprintf( killclass, "sentrygun" );
			}
		}

		if ( killer == pLocal->index )
		{
			cl_entity_t *vEnt = gEngfuncs.GetEntityByIndex( victim );
			if (( vEnt != NULL ) && ( vEnt->player ))
			{
				gEngfuncs.pfnGetPlayerInfo( victim, &victiminfo );
				bJustKilled = 1;
				bJustKilledFlagCarrier = 1; // maybe... we're guessing
				sprintf( victname, "%s", victiminfo.name );
				sprintf( victclass, "%s", victiminfo.model );
			}

			if (( vEnt != NULL ) && ( !vEnt->player ) && ( !strcmp( pszWeapon, "sentrygun" ) ))
			{
				bJustKilledSG = 1;
				bJustKilled = 0;
				bJustKilledFlagCarrier = 0;
				//sprintf( victname, "N/A" );
			}

			if (( vEnt != NULL ) & ( vEnt->player ) && (
				( !strcmp( pszWeapon, "axe" ) ) ||
				( !strcmp( pszWeapon, "spanner" ) ) ||
				( !strcmp( pszWeapon, "medikit" ) ) ||
				( !strcmp( pszWeapon, "caltrop" ) )	||
				( !strcmp( pszWeapon, "railgun" ) )))
			{
				bHumiliate = 1;
			}
		}
	}
}

void CTriggers::Post_Hook_HudText (const char *pszString)
{
	cl_entity_t *pLocal = gEngfuncs.GetLocalPlayer();

	//your team caps enemy flag
	if(	( !strcmp (pszString, "2fort_you_capped_flag") )
		|| ( !strcmp (pszString, "2fort_your_team_capped") )
		|| ( !strcmp (pszString, "well_capture") ) 
		|| (  strstr (pszString, "italy_you_secure_") ) 
		|| ( !strcmp (pszString, "dblmint_you_capped_flag") ) 
		|| (  strstr (pszString, "ou captured the") ) 
		|| (  strstr (pszString, "our team captured the") ) 
		|| ( !strcmp (pszString, "well_red_scores") && pLocal->curstate.team == 2 ) 
		|| ( !strcmp (pszString, "well_blue_scores") && pLocal->curstate.team == 1 ) 
		|| ( !strcmp (pszString, "rock_red_scores") && pLocal->curstate.team == 2 ) 
		|| ( !strcmp (pszString, "rock_blue_scores") && pLocal->curstate.team == 1 ) 
		|| ( !strcmp (pszString, "dustbowl_you_secure_one") )
		|| ( !strcmp (pszString, "dustbowl_you_secure_two") )
		|| ( !strcmp (pszString, "dustbowl_you_secure_three") )
		|| (  strstr (pszString, "war_usec_") )
		)
	{
		bTeamCapFix = 1;
		bTeamCap = 1;
	}

	//enemy caps your flag
	if(	   ( !strcmp (pszString, "2fort_your_flag_capped") )
		|| (  strstr (pszString, "our flag was captured") ) 
		|| ( !strcmp (pszString, "well_red_scores_2") && pLocal->curstate.team == 1 ) 
		|| ( !strcmp (pszString, "well_blue_scores_2") && pLocal->curstate.team == 2 ) 
		|| ( !strcmp (pszString, "rock_red_scores_2") && pLocal->curstate.team == 1 ) 
		|| ( !strcmp (pszString, "rock_blue_scores_2") && pLocal->curstate.team == 2 ) 
		)
			bEnemyCap = 1;

	//enemy dropped flag, mulch addition, v1.38
	if(	( !strcmp (pszString, "2fort_enemy_lost_flag") )
		|| ( !strcmp (pszString, "rock_team_dropped_key") )
		)
			bEnemyLostFlag = 1;
}

void CTriggers::hook_statusicon(char *buf, int status)
{
	if ((!strcmp("flag_blue",buf)) || (!strcmp("flag_red",buf)) || (!strcmp("flag_yellow",buf)) || (!strcmp("flag_green",buf)))
	{
		if (!status)
			bYouDropFlag = 1;
	}
}

//mulch addition, v1.38
bool CTriggers::Pre_Hook_ConsoleCommand (char *pszString)
{
	// You have flag
	if ((strstr(pszString, "ctf_yougotflag"))
		|| (strstr(pszString, "rk_takekey"))
		|| (strstr(pszString, "youhave")) // mulch_trench
		)
	{
		bYouGotFlag = 1;
	}

	char tmpbuf[256];
	char szNewCommand[8192];
	sprintf(szNewCommand, "%s", pszString);
	cl_entity_s *pLocal = gEngfuncs.GetLocalPlayer();
	if(!pLocal || !strcmp(gLocation.szLevelName, "")) return true; // player not spawned yet -- prevent crash
	if(!(strstr(szNewCommand, "say") || strstr(szNewCommand, "say_team") || strstr(szNewCommand, "tell"))) return true;
	if(strstr(szNewCommand, "bind") || strstr(szNewCommand, "alias")) return true; // don't convert tokens in binds or aliases
	if((pLocal->curstate.playerclass >= 0) && (pLocal->curstate.playerclass <= 11))
	{
		if(strstr(szNewCommand, "%sgp"))
		{
			sprintf(szNewCommand, "%s", StringSearchReplace("%sgp", gLocation.strSGLocation, szNewCommand));
		}
		if(strstr(szNewCommand, "%ec")) // display enemy class
		{
			sprintf(szNewCommand, "%s", StringSearchReplace("%ec", victclass, szNewCommand));
		}
		if(strstr(szNewCommand, "%en")) // display enemy name
		{
			sprintf(szNewCommand, "%s", StringSearchReplace("%en", victname, szNewCommand));
		}
		if(strstr(szNewCommand, "%kc")) // display killers class (dude who killed you)
		{
			sprintf(szNewCommand, "%s", StringSearchReplace("%kc", killclass, szNewCommand));
		}
		if(strstr(szNewCommand, "%kn")) // display killers name (dude who killed you)
		{
			sprintf(szNewCommand, "%s", StringSearchReplace("%kn", killname, szNewCommand));
		}
		if(strstr(szNewCommand, "%scb"))
		{
			if(gFB.bScoreBlue)
			{
				sprintf(tmpbuf, "%d", gFB.teamscores[gFB.FindTeamIndex('b')].points);
				sprintf(szNewCommand, "%s", StringSearchReplace("%scb", tmpbuf, szNewCommand));
			}
			else
				sprintf(szNewCommand, "%s", StringSearchReplace("%scb", "null", szNewCommand));
		}
		if(strstr(szNewCommand, "%scr"))
		{
			if(gFB.bScoreRed)
			{
				sprintf(tmpbuf, "%d", gFB.teamscores[gFB.FindTeamIndex('r')].points);
				sprintf(szNewCommand, "%s", StringSearchReplace("%scr", tmpbuf, szNewCommand));				
			}
			else
				sprintf(szNewCommand, "%s", StringSearchReplace("%scr", "null", szNewCommand));
		}
		if(strstr(szNewCommand, "%scy"))
		{
			if(gFB.bScoreYellow)
			{
				sprintf(tmpbuf, "%d", gFB.teamscores[gFB.FindTeamIndex('y')].points);
				sprintf(szNewCommand, "%s", StringSearchReplace("%scy", tmpbuf, szNewCommand));
			}
			else
				sprintf(szNewCommand, "%s", StringSearchReplace("%scy", "null", szNewCommand));
		}
		if(strstr(szNewCommand, "%scg"))
		{
			if(gFB.bScoreGreen)
			{
				sprintf(tmpbuf, "%d", gFB.teamscores[gFB.FindTeamIndex('g')].points);
				sprintf(szNewCommand, "%s", StringSearchReplace("%scg", tmpbuf, szNewCommand));
			}
			else
				sprintf(szNewCommand, "%s", StringSearchReplace("%scg", "null", szNewCommand));
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
//end mulch addition, v1.38

//mulch addition 1.43
void CTriggers::Post_Hook_Feign (int iIsFeigning)
{
	if ( iIsFeigning )
		bFeigning = true;
}

void CTriggers::Post_Hook_Detpack (int iIsSettingDetpack)
{
	if ( iIsSettingDetpack == 1 )
		bSettingDetpack = true;
}

CTriggers::CTriggers(void)
{
	bJustSpawned = 0;
	bJustDied = 0;
	bJustKilled = 0;
	bNewMap = 0;
	bNewServer = 0;
	bEnemyCap = 0;
	bTeamCap = 0;
	bTeamCapFix = 0;
	bYouDropFlag = 0;
	bTimeleft = 0;
	fLastTime = 0;
	bEndOfRound = 0;
	//mulch addition, v1.38
	bJustKilledFlagCarrier = 0;
	bJustSuicided = 0;
	bEnemyLostFlag = 0;
	sprintf(victclass, "NULL");
	sprintf(victname, "NULL");
	sprintf(killclass, "NULL");
	sprintf(killname, "NULL");
	bYouGotFlag = 0;
	bJustDiedSG = 0;
	//end mulch addition, v1.38
	// mulch addition 1.43
	bFeigning = false;
	bSettingDetpack = false;
}
