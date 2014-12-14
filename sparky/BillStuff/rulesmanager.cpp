/*  ------------------------------------------------------------------
	  rulesmanager.cpp by FryGuy, FB, and ruiner
	    - note: look in triggers.cpp for 'timeleft' say trigger
				look in timers.cpp for the on-screen display
		- updated v1.39 - 03/08/03 - FB: added announcing options
	    - updated v1.33 - changed method from timeleft to timelimit
		- added v1.32
	------------------------------------------------------------------ */

#include "..\client.h"

#include "hook.h"
#include "superhud.h"
#include "rulesmanager.h"
#include "logging.h"
#include <time.h>
#include "util.h"
#include "help.h"


CRulesManager gRulesManager;

DECLARE_COMMAND_2(gRulesManager, Timeleft);

// Server rules requestor and parserm by isc (R.S.), 2001
static void RulesResponse (net_response_t *response)
{
	if ( response->error != NET_SUCCESS 
	|| response->type != NETAPI_REQUEST_RULES
	|| !response->response )
	{
		return;
	}

    // Send "update" message
    gRulesManager.Net_RulesUpdated((const char*) response->response);
}

CRulesManager::CRulesManager(void)
{
	iContextId = 30;
	fNextRulesUpdate = 0;
	bTimeleftOK = 0;

	b15MinDone = 0;
	b5MinDone = 0;
	b2MinDone = 0;
	b1MinDone = 0;
	b30SecDone = 0;
}

void CRulesManager::Net_RulesUpdated (const char * rules_string)
{
	const char * timelimit = gEngfuncs.pNetAPI->ValueForKey(rules_string, "mp_timelimit"); 

	//gLogging.add_log("[timelimit] timelimit: %s", timelimit);

	if ( timelimit && atoi(timelimit) ) 
		iTimelimit = atoi(timelimit) * 60; 

	if(!strcmp(timelimit, "0"))
		iTimelimit = 0;
}

void CRulesManager::UpdateRules()
{
	net_status_t status;
	
	gEngfuncs.pNetAPI->Status(&status);
	gEngfuncs.pNetAPI->SendRequest (iContextId++, NETAPI_REQUEST_RULES, 0, 5.0, &status.remote_address, ::RulesResponse);
	fNextRulesUpdate = gEngfuncs.GetClientTime() + MIN_TIME_BETWEEN_RULES_UPDATES;
}

bool CRulesManager::Pre_HUD_Init (void)
{
	gEngfuncs.pNetAPI->InitNetworking();
	
	HOOK_COMMAND_2("btimeleft", Timeleft);
	gHelp.addCommand("btimeleft", "Toggles display of the time left in the round\nUsage: btimeleft");

	return true;
}

bool CRulesManager::Pre_HUD_Frame (double time)
{
	return true;
}

void CRulesManager::Post_HUD_Frame (double time)
{
	if(bTimeleftOK && bTimeleftOn)
	{
		cur_time = gEngfuncs.GetClientTime();

		if ( cur_time > fNextRulesUpdate )
			UpdateRules();
	}

	if((fLastTime-10) > gEngfuncs.GetClientTime())
	{
		fLastTime = 0;
		fNextRulesUpdate = 20;
	}
	fLastTime = gEngfuncs.GetClientTime();

	//reset the announcing bools
	if(strcmp(szLevelName, gEngfuncs.pfnGetLevelName()))
	{
		sprintf(szLevelName, "%s", gEngfuncs.pfnGetLevelName());

		b15MinDone = 0;
		b5MinDone = 0;
		b2MinDone = 0;
		b1MinDone = 0;
		b30SecDone = 0;
	}
	
	if(bTimeleftAnnounce)
	{
		if(((int)((iTimelimit - gEngfuncs.GetClientTime())/60) == 15) && ((int)fmod(iTimelimit - gEngfuncs.GetClientTime(), 60) == 0))
		{
			if(!b15MinDone)
			{
				b15MinDone = 1;
				gEngfuncs.pfnClientCmd("btrigger_timeleft;");
			}
		}

		if(((int)((iTimelimit - gEngfuncs.GetClientTime())/60) == 5) && ((int)fmod(iTimelimit - gEngfuncs.GetClientTime(), 60) == 0))
		{
			if(!b5MinDone)
			{
				b5MinDone = 1;
				gEngfuncs.pfnClientCmd("btrigger_timeleft;");
			}
		}

		if(((int)((iTimelimit - gEngfuncs.GetClientTime())/60) == 2) && ((int)fmod(iTimelimit - gEngfuncs.GetClientTime(), 60) == 0))
		{
			if(!b2MinDone)
			{
				b2MinDone = 1;
				gEngfuncs.pfnClientCmd("btrigger_timeleft;");
			}
		}

		if(((int)((iTimelimit - gEngfuncs.GetClientTime())/60) == 1) && ((int)fmod(iTimelimit - gEngfuncs.GetClientTime(), 60) == 0))
		{
			if(!b1MinDone)
			{
				b1MinDone = 1;
				gEngfuncs.pfnClientCmd("btrigger_timeleft;");
			}
		}

		if(((int)fmod(iTimelimit - gEngfuncs.GetClientTime(), 60) == 30) && ((int)((iTimelimit - gEngfuncs.GetClientTime())/60) == 0))
		{
			if(!b30SecDone)
			{
				b30SecDone = 1;
				gEngfuncs.pfnClientCmd("btrigger_timeleft;");
			}
		}
	}
}

bool CRulesManager::Pre_Hook_ConsoleCommand (char *pszString)
{
	char szNewCommand[8192];
	sprintf(szNewCommand, "%s", pszString);
	cl_entity_s *pLocal = gEngfuncs.GetLocalPlayer();
	char map[256];
	strcpy(map, gEngfuncs.pfnGetLevelName());
	if(!pLocal || !strcmp(map, "")) return true; // player not spawned yet -- prevent crash
	if(!(strstr(szNewCommand, "say") || strstr(szNewCommand, "say_team") || strstr(szNewCommand, "tell"))) return true;
	if(strstr(szNewCommand, "bind") || strstr(szNewCommand, "alias")) return true; // don't convert tokens in binds or aliases
	if(bTimeleftOn)
	{
		if(strstr(szNewCommand, "%t")) // display current timeleft
		{
			fLastTriggerTime = gEngfuncs.GetClientTime();
			char timeleftbuf[256];
			if(iTimelimit)
				sprintf(timeleftbuf, "%02d:%02d", (int)((iTimelimit - gEngfuncs.GetClientTime())/60), (int)fmod(iTimelimit - gEngfuncs.GetClientTime(), 60));
			else
				sprintf(timeleftbuf, "00:00 (no limit)");
			sprintf(szNewCommand, "%s", StringSearchReplace("%t", timeleftbuf, szNewCommand));
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

void CRulesManager::Post_Hook_TextMsg (const char *pszString)
{
	if(!gEngfuncs.pDemoAPI->IsPlayingback()) //check to see if a demo is running
	{
		if(!strcmp(pszString, "#Game_joinedteam"))
		bTimeleftOK = 1;
	}
}

void CRulesManager::Cmd_Timeleft (void)
{
	if(gEngfuncs.Cmd_Argc() < 2)
	{
		bTimeleftOn = bTimeleftOn ? 0 : 1;

		if(bTimeleftOn) 
			gSuperHUD.WriteHUDChannel(HC_IN_GAME_COLOR, 2, "timeleft ON");
		else 
			gSuperHUD.WriteHUDChannel(HC_IN_GAME_COLOR, 2, "timeleft OFF");
	}

	if(!strcmp(gEngfuncs.Cmd_Argv(1), "announce"))
	{
		bTimeleftAnnounce = bTimeleftAnnounce ? 0 : 1;

		if(bTimeleftAnnounce)
			gSuperHUD.WriteHUDChannel(HC_IN_GAME_COLOR, 2, "timeleft announcing ON");
		else
			gSuperHUD.WriteHUDChannel(HC_IN_GAME_COLOR, 2, "timeleft announcing OFF");
	}
}