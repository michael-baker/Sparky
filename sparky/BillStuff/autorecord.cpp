/*  ------------------------------------------------------------------------
	  autorecord.cpp by Zoltarc
		- updated v1.4 - 03/22/03 - billdoor: fixed corrupt demos on map-change
	    - updated v1.38 - 03/04/03 - mulch: fixed recording of multiples
	    - updated v1.38 - 02/14/03 - fucking hell, more temp fixes.
	    - updated v1.37 - 02/02/03 - temp fix for autorecord when not conn'd.
		- added v1.36 - 01/14/03
	------------------------------------------------------------------------ */

#include "..\client.h"

#include "hook.h"
#include "superhud.h"
#include "autorecord.h"
#include "util.h"
#include <time.h>

#include <stdio.h>
#include <string.h>
#include "help.h"

#pragma warning (disable:4800) // forcing value to bool

/*Global*/
CAutorecord gAutorecord;

DECLARE_COMMAND_2(gAutorecord, AutoRecord);
DECLARE_COMMAND_2(gAutorecord, EasyRecord);

static char DemoName [2048];
static char DemoRecord [2048];
static char PlayerName [256];

bool CAutorecord::Pre_HUD_Init (void)
{
	HOOK_COMMAND_2("autorecord", AutoRecord);
	gHelp.addCommand("autorecord", "Starts recording using the easyrecord format and halts when the round ends. The command \"autorecord 0\" will stop the demo and auto recording.\nUsage: autorecord");
	HOOK_COMMAND_2("easyrecord", EasyRecord);
	gHelp.addCommand("easyrecord", "Starts recording a demo with the name in the format dd-mm-yyyy-hh-mm-ss-map.\nUsage: easyrecord");
	return true;
}

void CAutorecord::Post_Hook_TextMsg (const char *pszString)
{
	/* Listen for end of game and stop recording */
	if((!strcmp(pszString, "#Match_results") && bAutoRecord))
	{
		bRecordOK = 0;
		gEngfuncs.pfnClientCmd("stop");
		bAutoRecordFix = false;
	}

	/* Listen for end of clan war and stop recording */
	if((!strcmp(pszString, "#Match_results") && bMatchRecord))
	{
		bRecordOK = 0;
		gEngfuncs.pfnClientCmd("stop");
	}

	/* Listen for start of clan war and start recording */
	if((!strcmp(pszString, "#Game_thirtysecleft") && bMatchRecord))
	{
		bRecordOK = 1;
		Cmd_EasyRecord();
	}

	/* FB: Listen for #Game_joinedteam and start recording, a bad hack */
	if(!strcmp(pszString, "#Game_joinedteam"))
	{
		bAutoRecordOK = 1;

		if ( !bAutoRecordFix )
		{
			if(bAutoRecord)
			{
				Cmd_EasyRecord();
				bAutoRecordFix = true;
			}
		}
	}

}

bool CAutorecord::Pre_Hook_ConsoleCommand (char *pszString)
{
	if((!strcmp(pszString, "reconnect")) || (!strcmp(pszString, "retry")) || (!strcmp(pszString, "quit")))
	{
		gEngfuncs.pfnClientCmd("stop");
	}

	//the above stuff looks through the incoming commands and stops the demo on map-
	//change or quit, both things that could be catostrophic. credit goes to billdoor.

	return true;
}


void CAutorecord::Cmd_EasyRecord(void)
{
	/* Setup the time */
	SYSTEMTIME st;
	GetLocalTime ( &st);

	if(bRecordOK || bAutoRecordOK) // FB's really (really!) sloppy hack to check whether we can record
	{
		char MapName	[2048];			// Holds the map name

		/* Setup the MapName and PlayerName */
		strcpy(MapName, NameFromFileName(gEngfuncs.pfnGetLevelName()));		
		strcpy(PlayerName, gHookEngfuncs.pfnGetCvarString("name"));
		RemoveIllegalsFromDemoName(PlayerName);

		/* Adds the date string and map name onto the record command */
		sprintf(DemoRecord, "record \"%s-%d-%d-%d-%s\"", PlayerName, st.wDay, st.wMonth, st.wYear, MapName);
		sprintf(DemoName, "%s-%d-%d-%d-%s", PlayerName, st.wDay, st.wMonth, st.wYear, MapName);

		IncreaseDemoName();

		char buf[2064];
		sprintf(buf, "[sparky] autorecord: initiating recoring (%s)\n", DemoName);
		gEngfuncs.pfnConsolePrint(buf);
	}
	else
		gEngfuncs.pfnConsolePrint("[sparky] delaying the recording until connected ;D\n");

}

void CAutorecord::IncreaseDemoName(void)
{
	/* Based off the demo code found in FuhQuake
	 * Got buffer? */
	char name2 [2048];
	char name3 [2048];
	char NameHack [2048];
	char NewRecord [2048];
	FILE * file;
	int i;

	sprintf(name2, "%s\\%s.dem", gEngfuncs.pfnGetGameDirectory(), DemoName);

	file = fopen (name2, "rb");
	if (file) 
	{
		i = 0;
		do {
			fclose (file);
			/* Copy the current demo name, and the extention string to a buffer */
			sprintf(name3, "%s-%d.dem", DemoName, i);

			/* This is needed to stop zero byte files outside of the tfc directory */
			sprintf(NameHack, "%s\\%s-%d.dem", gEngfuncs.pfnGetGameDirectory(), DemoName, i);
			file = fopen (NameHack, "rb");
			i++;			
		} while (file);
	}
	fopen (NameHack, "wb");
	if (!NameHack) 
	{
		gEngfuncs.pfnConsolePrint("ERROR: Couldn't open.\n");
		return;
	}
	
	sprintf(NewRecord, "record \"%s\"", name3);

	file = fopen (name2, "rb");
	if (file)
	{
		fclose (file);
		gEngfuncs.pfnClientCmd(NewRecord); // Record increased demo
	}
	else
	{
		gEngfuncs.pfnClientCmd(DemoRecord); // Record first demo
	}
}

void CAutorecord::Cmd_AutoRecord(void)
{
	int setting;
	if(gEngfuncs.Cmd_Argc() > 1)
	{
		setting = atoi(gEngfuncs.Cmd_Argv(1));
		if((setting >= 0) && (setting <= 1))
		{
			bAutoRecord = (bool)setting;
			if(setting == 0) gSuperHUD.WriteHUDChannel(HC_IN_GAME_COLOR, 3, "autorecord  OFF");
			if(setting == 1) gSuperHUD.WriteHUDChannel(HC_IN_GAME_COLOR, 3, "autorecord  ON"); 
		}
	}
	if (bAutoRecord)
	{
		Cmd_EasyRecord();
	}

	if (!bAutoRecord)
	{
		gEngfuncs.pfnClientCmd("stop");
	}

	if(!strcmp(gEngfuncs.Cmd_Argv(1), "match"))
	{
		bMatchRecord = bMatchRecord ? 0 : 1;
		if(bMatchRecord)
		{
			gSuperHUD.WriteHUDChannel(HC_IN_GAME_COLOR, 2, "matchrecord ON");
		}
		else
		{
			gSuperHUD.WriteHUDChannel(HC_IN_GAME_COLOR, 2, "matchrecord OFF");
			gEngfuncs.pfnClientCmd("stop");
		}
	}	
}

CAutorecord::CAutorecord(void)
{
	bAutoRecord = 0;
	bMatchRecord = 0;
	bRecordOK = 0;
	bAutoRecordFix = false;
}