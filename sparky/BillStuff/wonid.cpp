/*  ------------------------------------------------------------------------
	  wonid.cpp by billdoor
	    - updated v1.38 - 02/17/03 (buggy wonid hud by FB)
	    - updated v1.34 - 01/13/03 (bwhois thanks to 8*B@LL)
	    - updated v1.33 - 01/12/03 (bregisterplayer thanks to 8*B@LL)
		- added v1.0
	------------------------------------------------------------------------ */

#include "..\client.h"

#include "hook.h"
#include "superhud.h"
#include "wonid.h"
#include "logging.h"

#define MAX_CHARS_PER_LINE	240
#define MAX_LINE_WIDTH  (gSuperHUD.m_scrinfo.iWidth - 40)
#define LINE_START  10
#define MAX_LINES	33

/*Global*/
CWonid gWonid;

DECLARE_COMMAND_2(gWonid, RegisterWonids);
DECLARE_COMMAND_2(gWonid, Ban);
DECLARE_COMMAND_2(gWonid, AutologWonids);
DECLARE_COMMAND_2(gWonid, RegisterPlayer);
DECLARE_COMMAND_2(gWonid, Whois);
DECLARE_COMMAND_2(gWonid, ToggleWonidHUD);

bool CWonid::Pre_HUD_Init (void)
{
	HOOK_COMMAND_2("bregisterwonids", RegisterWonids);
	HOOK_COMMAND_2("bban", Ban);
	HOOK_COMMAND_2("bautologwonids", AutologWonids);
	HOOK_COMMAND_2("bregisterplayer", RegisterPlayer); // bregisterplayer
	HOOK_COMMAND_2("bwhois", Whois);
	HOOK_COMMAND_2("bwonidhud", ToggleWonidHUD); // leet wonid hud

	return true;
}

void CWonid::Post_HUD_Init (void)
{
	gEngfuncs.pfnClientCmd("alias \"+wonidhud\" \"bwonidhud +\"; alias \"-wonidhud\" \"bwonidhud -\";");
	bWonidHUDUseColors = 1;
	bWonidHUDDrawTime = 0;
}

CWonid::CWonid(void)
{
	fStatusTime = 0;
	bDumped = 0;
	iBanWonid = 0;
	sprintf(BanName, "\0");
	iBanTime = 0;
	bBanRequest = 0;
	bAutologWonids = 0;
	iWhoisWonid = 0;
	sprintf(WhoisName, "\0");
}


void CWonid::Cmd_RegisterWonids(void)
{
	if(strlen(szLevelName))
	{
		char buf[1024];
		fStatusTime = gEngfuncs.GetClientTime();
		gEngfuncs.pfnClientCmd("status;");
		bDumped = 0;
		sprintf(buf, "%s\\condump000.txt", gEngfuncs.pfnGetGameDirectory());
		DeleteFile(buf);
	}
	else
		gEngfuncs.pfnConsolePrint("[sparky] sorry, you must be connected to wave this wand\n");
}

void CWonid::Cmd_AutologWonids(void)
{
	bAutologWonids = bAutologWonids ? 0 : 1;

	if(bAutologWonids)
		gSuperHUD.WriteHUDChannel(HC_IN_GAME_COLOR, 2, "wonid autolog - on");
	else
		gSuperHUD.WriteHUDChannel(HC_IN_GAME_COLOR, 2, "wonid autolog - off");
}

void CWonid::Cmd_Ban (void)
{
	char NameString[1024];
	int i;

	if(gEngfuncs.Cmd_Argc() < 3)
	{
		gEngfuncs.pfnConsolePrint("[sparky] bban command usage:\n");
		gEngfuncs.pfnConsolePrint("[sparky] bban <minutes> <partial-name>\n");
		return;
	}

	bBanRequest = 1;
	sprintf(NameString, "%s", gEngfuncs.Cmd_Argv(2));
	if(gEngfuncs.Cmd_Argc() > 2)
	{
		for(i = 3; i < gEngfuncs.Cmd_Argc(); i++)
		{
			strcat(NameString, gEngfuncs.Cmd_Argv(i));
		}
	}
	sprintf(BanName, "%s", NameString);
	iBanTime = atoi(gEngfuncs.Cmd_Argv(1));
	gEngfuncs.pfnClientCmd("bregisterwonids");
}


/* -----------------------------------bregisterplayer-----------------------------------
      Allows clients to locally "register" wonid/name combos, all thanks to 8*B@LL!
   ------------------------------------------------------------------------------------- */

void CWonid::Cmd_RegisterPlayer(void)
{
	char name[1024];
	int wonid, i;
	FILE *fp;
	

	if(gEngfuncs.Cmd_Argc() < 3)
	{
		gEngfuncs.pfnConsolePrint("[sparky] bregisterplayer command usage:\n");
		gEngfuncs.pfnConsolePrint("[sparky] bregisterplayer <wonid> <name>\n");
		return;
	}
	else
	{
		wonid = atoi(gEngfuncs.Cmd_Argv(1));
		sprintf(name, "%s", gEngfuncs.Cmd_Argv(2));
		if(gEngfuncs.Cmd_Argc() > 3)
		{
			for(i=3; i< gEngfuncs.Cmd_Argc(); i++)
				strcat(name, gEngfuncs.Cmd_Argv(i));
		}
	
		if(fp=fopen("sparkyutils\\registeredplayers.txt", "at"))
		{
			fprintf(fp, "%d %s\n", wonid, name);
			fclose(fp);
		}
		else
			gEngfuncs.pfnConsolePrint("[sparky] Couldn't open registeredplayers.txt for writing. Fekin shite.\n");
	}
	return;

}

/* ---------------------------------------bwhois----------------------------------------
					  Locally lookup a single player, coded by 8*B@LL.
   ------------------------------------------------------------------------------------- */

void CWonid::Cmd_Whois(void)
{
	
	int i;

	if(gEngfuncs.Cmd_Argc() < 2)
	{
		gEngfuncs.pfnConsolePrint("[sparky] bwhois command usage:\n");
		gEngfuncs.pfnConsolePrint("[sparky] bwhois <name/# [wonid]>\n");
		gEngfuncs.pfnConsolePrint("[sparky] NOTE: for people with spaces in their name, use a unique portion containing no spaces\n");
		gEngfuncs.pfnConsolePrint("[sparky] i.e. if the name was \"John Johomonzoquimpe\", you could search for \"quimpe\".\n");
	}
	else
	{
		sprintf(WhoisName, "%s", gEngfuncs.Cmd_Argv(1));
		if(!strcmp(WhoisName, "#"))
		{
			iWhoisWonid = atoi(gEngfuncs.Cmd_Argv(2));
			WhoisName[0] = '\0';
		}
		else
		{
			for(i=2; i<gEngfuncs.Cmd_Argc(); i++)
			{
				strcat(WhoisName, gEngfuncs.Cmd_Argv(i));
			}
		}

		bWhoisRequest = true;
		gEngfuncs.pfnClientCmd("bregisterwonids");
	}
}

void CWonid::Post_HUD_Frame (double time)
{
	//added for connected fix
	if(strcmp(szLevelName, gEngfuncs.pfnGetLevelName()))
		sprintf(szLevelName, "%s", gEngfuncs.pfnGetLevelName());

	if(fStatusTime && (gEngfuncs.GetClientTime() - fStatusTime > 1) && !bDumped)
	{
		Dump();
		bDumped = 1;
	}

	if(fStatusTime && (gEngfuncs.GetClientTime() - fStatusTime > 1.1) && bDumped)
	{
		Parse();
		fStatusTime = 0;
	}
}

void CWonid::Dump (void)
{
	gEngfuncs.pfnClientCmd("condump;");
}

void CWonid::Parse (void)
{
	char buf[4096];
	char localname[1024];
	char *line;
	bool foundheader = 0;
	bool endofstatus = 0;
	bool playerfound = false;//for whois search
	int wonidsfound = 0;
	int iWhoisPosition = -1;
	char output[34][16384];
	struct
	{
		int wonid;
		char name[256];
		bool duplicate;
		int aliases;
		bool registered;
	} entry[32];
	FILE *fp;

	hud_player_info_t pinfo;
	cl_entity_t *pLocal = gEngfuncs.GetLocalPlayer();
	gEngfuncs.pfnGetPlayerInfo(pLocal->index, &pinfo);
	sprintf(localname, "%s", pinfo.name);
	//sprintf(buf, "[local] [%s]\n", localname);
	//gEngfuncs.pfnConsolePrint(buf);


	sprintf(buf, "%s\\condump000.txt", gEngfuncs.pfnGetGameDirectory());
	
	if(fp = fopen(buf, "r"))
	{
		// ConsolePrint("[sparky] Opened condump.txt for reading.\n");
		// Read the condump line by line
		while((line = fgets(buf, 1024, fp)) && !endofstatus)
		{
			sprintf(buf, "%s", line);

			if(!foundheader)
			{
				// Check if this line is the header of the status info
				// truncate the line string to end with name since that's the best way to compare
				buf[11] = '\0';
				if(!strcmp(buf, "#      name"))
				{
					foundheader = 1;
				}
			}
			else
			{
				// Check to see that this line starts with a # character
				if(buf[0]=='#')
				{
					// Now extract the player name and wonid
					// Scan the string for the first quote character (start of player name)
					bool foundopeningquote = 0;
					bool foundclosingquote = 0;
					bool foundspace = 0;
					bool alldone = 0;
					char wonidstring[128];
					wonidstring[0] = '\0';
					entry[wonidsfound].name[0] = '\0';
					for(int i = 0; (i < (int)strlen(buf)) && !alldone; i++)
					{
						if(foundopeningquote && foundclosingquote && foundspace)
						{
							// Read the wonid
							if(buf[i] == ' ')
							{
								entry[wonidsfound].wonid = atoi(wonidstring);
								wonidsfound++;
								alldone = 1;
							}
							else
							{
								char tmp[2];
								tmp[0] = buf[i];
								tmp[1] = '\0';
								strcat(wonidstring, tmp);
							}
						}

						if(foundopeningquote && foundclosingquote && !foundspace)
						{
							if(buf[i] == ' ')
								foundspace = 1;
						}

						if(foundopeningquote && !foundclosingquote)
						{
							if(buf[i] == '\"')
							{
								foundclosingquote = 1;
								i++;
							}
							else
							{
								char tmp[2];
								tmp[0] = buf[i];
								tmp[1] = '\0';
								strcat(entry[wonidsfound].name, tmp);
							}
														
						}

						if(!foundopeningquote)
						{
							if(buf[i] == '\"')
								foundopeningquote = 1;
						}
					}
				
				}
				else
				{
					// This is the end of the block of status information
					endofstatus = 1;
				}
						
			}
						
		}
		fclose(fp);
	}
	else
	{
		gEngfuncs.pfnConsolePrint("[sparky] Couldn't open condump.txt for parsing.\n");
	}
	
	// Now it's time to scan the wonid data file and record these ids, unless the same id/name is already registered
	// Scan first, for duplicate ids
	int i;
	for(i = 0; i < wonidsfound; i++)
	{
		entry[i].duplicate = 0;
		entry[i].aliases = 0;
		entry[i].registered = false;

		// Just process the names if this is a request from bban
		if(bBanRequest)
		{
			int iPatternLength = strlen(BanName);
			char MatchString[256];
			sprintf(MatchString, "%s", entry[i].name);
			MatchString[iPatternLength] = '\0';
			if(!strcmp(MatchString, BanName))
			{
				iBanWonid = entry[i].wonid;
				if(iBanTime > 0)
				{
					sprintf(buf, "[sparky] Banning [%s] with wonid [%d] for [%d] minutes.\n", entry[i].name, entry[i].wonid, iBanTime);
					gEngfuncs.pfnConsolePrint(buf);
					sprintf(buf, "rcon banid %d %d kick;", iBanTime, entry[i].wonid);
					gEngfuncs.pfnClientCmd(buf);
				}
				else
				{
					sprintf(buf, "[sparky] Banning [%s] with wonid [%d] permanently.\n", entry[i].name, entry[i].wonid);
					gEngfuncs.pfnConsolePrint(buf);
					sprintf(buf, "rcon banid 0 %d kick; rcon writeid;", entry[i].wonid);
					gEngfuncs.pfnClientCmd(buf);
				}
			}
		}
		else if(bWhoisRequest && !iWhoisWonid)
		{
			if(strstr(entry[i].name, WhoisName))
			{
				if(!playerfound)
				{
					sprintf(buf, "[sparky] Searching for player \"%s\"...\n[sparky] Player match for name \"%s\" found: \"%s\".\n", WhoisName, WhoisName, entry[i].name);
					gEngfuncs.pfnConsolePrint(buf);
					playerfound = true;
					iWhoisPosition = i;
				}
				else
				{
					sprintf(buf, "[sparky] Error: Multiple matches found for \"%s\", be more spefic!", WhoisName);
					bWhoisRequest = 0;
					return;
				}
			}
		}
		else if(bWhoisRequest)//they must have searched based on wonid
		{
			if(iWhoisWonid == entry[i].wonid)
			{
				sprintf(buf, "[sparky] Searching for player with wonid %d on server...\n[sparky] Player match: \"%s\".\n", iWhoisWonid, entry[i].name);
				gEngfuncs.pfnConsolePrint(buf);
				iWhoisPosition = i;
			}
		}
	}

	if(bWhoisRequest)
	{
		if(!iWhoisWonid && (iWhoisPosition == -1))
		{
			sprintf(buf, "[sparky] Could not find a player in the server while searching for \"%s\".\n[sparky] With no WONID, the search cannot take place.\n", WhoisName);
			bWhoisRequest = 0;
			return;
		}
		if(WhoisName[0] == '\0' && (iWhoisPosition == -1))
		{
			sprintf(buf, "[sparky] Player with wonid %d not on the server, searching anyway.\n", iWhoisWonid);
			gEngfuncs.pfnConsolePrint(buf);
			entry[0].wonid = iWhoisWonid;
			_itoa(iWhoisWonid, entry[0].name, 10);
			entry[0].duplicate = true;
		}
		else
		{
			entry[0] = entry[iWhoisPosition];
		}
		wonidsfound = 1;
	}
	// k so, since the new method searches by line in file first, we need to store the results
	// in an array so they can still be displayed in name order
	// so initialise the array to 0
	for(i = 0; i < wonidsfound; i++)
	{
		sprintf(output[i], "\0");
	}

	//start code for 'registered' players
	if(fp=fopen("sparkyutils\\registeredplayers.txt", "rt"))
	{
		while(line = fgets(buf, 1024, fp))
		{
			char wonidstring[128];
			char name[1024];
			int wonid, j;
			sprintf(buf, "%s", line);
			for(j=0; buf[j] != ' '; j++)
			{
				wonidstring[j]=buf[j];
				wonidstring[j+1]='\0';
			}
			wonid = atoi(wonidstring);
			// ConsolePrint(wonidstring);
			sprintf(name, "%s", &buf[j+1]);
			name[strlen(name)-1] = '\0';
			// ConsolePrint(name);

			for(j=0; j<wonidsfound; j++)
			{
				if(wonid == entry[j].wonid)
				{
					// ignore bots, single player, and hltv
					if(strcmp(name, entry[j].name) && strcmp(localname, entry[j].name) && (entry[j].wonid!=-1) && (entry[j].wonid!=0)/* && (entry[j].wonid!=1448) && (entry[j].wonid!=199902)*/)
					{
						sprintf(buf, "[%s (%d)] Registered to be: %s", entry[j].name, entry[j].wonid, name);
						strcat(output[j], buf);
					}
					entry[j].registered = true;
					entry[j].duplicate = 1; //we dont want to record this wonid/name, its reg'd
				}
			}//for
		}//while
		fclose(fp);
	}//if file opened
	else
	{
		gEngfuncs.pfnConsolePrint("[sparky] Couldn't open registeredplayers.txt for reading.\n");
	}
	//end code for 'registered' players

	if(fp=fopen("sparkyutils\\playerwonids.txt", "rt"))
	{
		while(line = fgets(buf, 1024, fp))
		{
			char wonidstring[128];
			char name[1024], * szPad = 0;
			int wonid, j, iPad, i;
			sprintf(buf, "%s", line);
			for(j = 0; buf[j] != ' '; j++)
			{
				wonidstring[j] = buf[j];
				wonidstring[j+1] = '\0';
			}
			wonid = atoi(wonidstring);
			// ConsolePrint(wonidstring);
			sprintf(name, "%s", &buf[j+1]);
			name[strlen(name)-1] = '\0';
			// ConsolePrint(name);

			for(j = 0; j < wonidsfound; j++)
			{
				//if wonid's match and current entry is not registered in registeredplayers.txt
				if((wonid == entry[j].wonid) && !entry[j].registered)
				{
					// ignore bots, single player, hltv and.... me! Actually, me!
					if(strcmp(name, entry[j].name) && strcmp(localname, entry[j].name) 	&& (entry[j].wonid!=-1) && (entry[j].wonid!=0)/* && (entry[j].wonid!=1448) && (entry[j].wonid!=199902)*/)
					{
						if (entry[j].aliases == 0) 
						{
							szPad = 0;
							/*   */if (( entry[j].wonid / 10000000000 ) > 0 ) /* 11 digit wonid */ { iPad = 1; }
							else { if (( entry[j].wonid / 1000000000 ) > 0 ) /* 10 digit wonid */ { iPad = 2; }
							else { if (( entry[j].wonid / 100000000 ) > 0 ) /* 9 digit wonid */ { iPad = 3; }
							else { if (( entry[j].wonid / 10000000 ) > 0 ) /* 8 digit wonid */ { iPad = 4; }
							else { if (( entry[j].wonid / 1000000 ) > 0 ) /* 7 digit wonid */ { iPad = 5; }
							else { if (( entry[j].wonid / 100000 ) > 0 ) /* 6 digit wonid */ { iPad = 6; }
							else { if (( entry[j].wonid / 10000 ) > 0 ) /* 5 digit wonid */ { iPad = 7; }
							else { if (( entry[j].wonid / 1000 ) > 0 ) /* 4 digit wonid */ { iPad = 8; }
							else { if (( entry[j].wonid / 100 ) > 0 ) /* 3 digit wonid */ { iPad = 9; }
							} /*4*/ } /*5*/ } /*6*/ } /*7*/ } /*8*/ } /*9*/ } /*10*/ } /*11*/

							szPad = new char[iPad];
							for ( i = 0; i < iPad; i++ )
								szPad[i] = ' ';

							sprintf(buf, "[(%d)%s%s] %s | ", entry[j].wonid, szPad, entry[j].name, name);
						} 
						else 
						{
							sprintf(buf, "%s | ", name);
						}
						entry[j].aliases += 1;
						if(strlen(output[j]) < (16384 - strlen(buf)))
							strcat(output[j], buf);
					}
					else
						entry[j].duplicate = 1;
				}
			}
		}
		fclose(fp);
		for(i = 0; i < wonidsfound; i++)
		{
			//gEngfuncs.pfnConsolePrint(output[i]);
			if (strlen(output[i])) 
			{
				gEngfuncs.pfnConsolePrint(output[i]);
				gEngfuncs.pfnConsolePrint("\n");
			}
		}
	}
	else
	{
		gEngfuncs.pfnConsolePrint("[sparky] Couldn't open playerwonids.txt for reading.\n");
	}
	// Now open the file for writing all non-duplicate wonids
	if(fp=fopen("sparkyutils\\playerwonids.txt", "at"))
	{
		for(i = 0; i < wonidsfound; i++)
		{
			if((!entry[i].duplicate) && strcmp(localname,entry[i].name))
				fprintf(fp, "%d %s\n", entry[i].wonid, entry[i].name);
		}
		fclose(fp);
		// START wonidHUD
		FBwonidsfound = wonidsfound;
		bWonidHUD = 1;
		bWonidHUDDraw = 0;
		timeHUD = gEngfuncs.GetClientTime();
		for (k = 0; k < 34; k++)				// NULL ALL LINES in case we have to "wrap" later
				sprintf(FBoutput[k], "\0");
		for(i = 0; i < wonidsfound; i++)
		{
			//gEngfuncs.pfnConsolePrint(output[i]);
			if (strlen(output[i])) 
			{
				strcpy(FBoutput[i], output[i]);
				//gEngfuncs.pfnConsolePrint(output[i]);
				//gEngfuncs.pfnConsolePrint("\n");
			}
		}
		// END wonidHUD
	}
	else
	{
		gEngfuncs.pfnConsolePrint("[sparky] Couldn't open playerwonids.txt for writing. ARGGGGGGH!\n");
	}

	// Clear banrequest flag
	bBanRequest = 0;
	bWhoisRequest = 0;
}

bool CWonid::Pre_Hook_TextMsg (const char *pszString)
{
	if(bAutologWonids)
	{
		if(!strcmp(pszString, "#Game_playerjoin"))
		{
			gEngfuncs.pfnClientCmd("bregisterwonids");
			gSuperHUD.WriteHUDChannel(HC_IN_GAME_COLOR, 1, "updating wonids");
		}
	}

	return true;
}

void CWonid::Post_HUD_Redraw (float flTime, int intermission)
{
	//float time = gEngfuncs.GetClientTime();

	if(bWonidHUD)
	{
		int x = int(gSuperHUD.m_scrinfo.iWidth*0.02);
		int y = int(gSuperHUD.m_scrinfo.iHeight*0.02);
		int w = int(gSuperHUD.m_scrinfo.iWidth*0.96);
		int h = 0;
		int WHbarR, WHbarG, WHbarB, WHborderR, WHborderG, WHborderB;
		int opacity = 128;

		if(bWonidHUDUseColors)
		{
			if(gSuperHUD.HUDColor.x) 
			{
				WHbarR = int(gSuperHUD.HUDColor.x);
				WHborderR = int(gSuperHUD.HUDColor.x) - 40;
			}
			if(gSuperHUD.HUDColor.y)
			{
				WHbarG = int(gSuperHUD.HUDColor.y);
				WHborderG = int(gSuperHUD.HUDColor.y) - 40;
			}
			if(gSuperHUD.HUDColor.z)
			{
				WHbarB = int(gSuperHUD.HUDColor.z);
				WHborderB = int(gSuperHUD.HUDColor.z) - 40;
			}

			if(WHborderR < 0)
				WHborderR = 0;
			if(WHborderG < 0)
				WHborderG = 0;
			if(WHborderB < 0)
				WHborderB = 0;
		}
		else
		{
			//WHbarR = 60; WHbarG = 60; WHbarB = 60; //should be grey
			//WHbarR = 240;	WHbarG = 179;	WHbarB = 16; //orange
			//WHbarR = 10;	WHbarG = 71;	WHbarB = 96; //light blueish
			WHbarR = 10;	WHbarG = 10;	WHbarB = 30; //dark blue
			WHborderR = WHbarR + 40;
			WHborderG = WHbarG + 40;
			WHborderB = WHbarB + 40;
		}

		/*float time = gEngfuncs.GetClientTime();
		timeHUD2 = time - timeHUD;
		maxTime = bWonidHUDDrawTime;
		if(maxTime == -1)
			maxTime = int(timeHUD2) + 1;*/

		int line_width = 0; int line_height = 0;
		//h = FBwonidsfound * gSuperHUD.m_scrinfo.iCharHeight;
		
		//char buf[128];
		//sprintf(buf, "timeHUD: %.0f - timeHUD2:	%.0f - time: %.0f", timeHUD, timeHUD2, time);
		//gSuperHUD.WriteHUDChannel(HC_IN_GAME_COLOR, 2, buf);

		/*if (timeHUD2 > maxTime) 
		{ 
			bWonidHUD = 0; 
			//bDrawBorders = 0;
			for (k = 0; k <= FBwonidsfound; k++)
				sprintf(FBoutput[k], "\0");			
		}*/

		// initialize shit
		iOldWonid = -2;
		iCurWonid = -1;
		char strWonidChecker[20];

		if (/*(timeHUD2 < maxTime) || */bWonidHUDDraw && (gSuperHUD.iSuperHUDLevel >= 1)) 
		{
			if(FBwonidsfound - 1) //mulch's thingy to stop drawing of borders when no results
				gEngfuncs.pfnFillRGBA (x, y+gSuperHUD.m_scrinfo.iCharHeight+2, w, 1, WHborderR, WHborderG, WHborderB, 0xFF); //top border

			for(k = 0; k < FBwonidsfound; k++)
			{
				if (strlen(FBoutput[k])) 
				{
					// using a temp var cause there might be a limit on how many
					// chars atoi can process, plus wonid should be in the first
					// 15 or so chars of FBoutput[k] (if it's not then we'll get
					// zero and keep our current "painting" settings)
					strcpy( strWonidChecker, "\0" );
					for ( int i = 0; i < 20; i++ )
					{
						strWonidChecker[i] = FBoutput[k][i + 2];

						if ( strWonidChecker[i] == ')' )
							strWonidChecker[i] = ' ';
					}
					strWonidChecker[19] = '\0';

					// get the wonid from the string
					iCurWonid = atoi( strWonidChecker );

					if ( iCurWonid == 0 )	// text was wrapped and we dont want a new paint scheme yet
						iCurWonid = iOldWonid;

					if ( iCurWonid != iOldWonid )
					{
						// figure out if we got a hacker, registered, or normal user
						if ( strstr( FBoutput[k], "<hacker" ) )
							eUsrType = hacker;
						else if ( strstr( FBoutput[k], "Registered to be:" ) )
							eUsrType = registered;
						else
							eUsrType = normal;
					}

					EnsureTextFitsInOneLineAndWrapIfHaveTo(k);

					//be aware, players with this listing are on a blacklist of some sort or another, but are
					//not necessarily cheaters. if in doubt, come in #sparky and FB might be able to help you
					//find out where the listing came from and why it was there in the first place...
					switch ( eUsrType )
					{
						case hacker:
						{
							h = gSuperHUD.m_scrinfo.iCharHeight;
							y = y + 2 + gSuperHUD.m_scrinfo.iCharHeight;
							gEngfuncs.pfnFillRGBA (x, y, w-1, 1, WHbarR, WHbarG, WHbarB, opacity-30); //divider
							gEngfuncs.pfnFillRGBA (x+1, y+1, w-1, h+1, 255, 15, 9, 180); //fill (red)
							gEngfuncs.pfnFillRGBA (x, y, 1, h+2, WHborderR, WHborderG, WHborderB, 0xFF); //left border
							gEngfuncs.pfnFillRGBA (x+w, y, 1, h+2, WHborderR, WHborderG, WHborderB, 0xFF); //right border
							gEngfuncs.pfnDrawSetTextColor(255, 255, 30);
							gEngfuncs.pfnDrawConsoleString(x+5, y+3, FBoutput[k]);
						}
						break;

						case registered:
						{
							h = gSuperHUD.m_scrinfo.iCharHeight;
							y = y + 2 + gSuperHUD.m_scrinfo.iCharHeight;
							gEngfuncs.pfnFillRGBA (x, y, w-1, 1, WHbarR, WHbarG, WHbarB, opacity-30); //divider
							gEngfuncs.pfnFillRGBA (x+1, y+1, w-1, h+1, 255, 173, 219, 200); //fill (pink)
							gEngfuncs.pfnFillRGBA (x, y, 1, h+2, WHborderR, WHborderG, WHborderB, 0xFF); //left border
							gEngfuncs.pfnFillRGBA (x+w, y, 1, h+2, WHborderR, WHborderG, WHborderB, 0xFF); //right border
							gEngfuncs.pfnDrawSetTextColor(255, 255, 30);
							gEngfuncs.pfnDrawConsoleString(x+5, y+3, FBoutput[k]);
						}
						break;

						default:
						case normal:
						{
							h = gSuperHUD.m_scrinfo.iCharHeight;
							y = y + 2 + gSuperHUD.m_scrinfo.iCharHeight;
							opacity = opacity + 2;
							gEngfuncs.pfnFillRGBA (x, y, w-1, 1, WHbarR, WHbarG, WHbarB, opacity-30); //divider
							gEngfuncs.pfnFillRGBA (x+1, y+1, w-1, h+1, WHbarR, WHbarG, WHbarB, opacity); //fill
							gEngfuncs.pfnFillRGBA (x, y, 1, h+2, WHborderR, WHborderG, WHborderB, 0xFF); //left border
							gEngfuncs.pfnFillRGBA (x+w, y, 1, h+2, WHborderR, WHborderG, WHborderB, 0xFF); //right border
							gEngfuncs.pfnDrawSetTextColor(255, 255, 30);
							gEngfuncs.pfnDrawConsoleString(x+5, y+3, FBoutput[k]);
						}
						break;
					}

					iOldWonid = iCurWonid;
				}				
			}
			if(FBwonidsfound - 1) //mulch's thingy to stop drawing of borders when no results
				gEngfuncs.pfnFillRGBA (x, y+h+2, w, 1, WHborderR, WHborderG, WHborderB, 0xFF); //bottom border
		}
	}
}

/*bool CWonid::Pre_Hook_ConsoleCommand (char *pszString)
{
	if((!strcmp(pszString, "reconnect")) || (!strcmp(pszString, "retry")))
	{
		timeHUD2 = 1;
		maxTime = 0;
	}

	return true;
}*/

void CWonid::Cmd_ToggleWonidHUD (void)
{
	if(gEngfuncs.Cmd_Argc() < 2)
	{
		char buf[256];
		
		//sprintf(buf, "[sparky] vaild bwonidhud commands: hudcolor, drawtime <%d>, capslock\n", bWonidHUDDrawTime);
		sprintf(buf, "[sparky] vaild bwonidhud commands: hudcolor, capslock\n");
		gEngfuncs.pfnConsolePrint(buf);
	}

	if(!strcmp(gEngfuncs.Cmd_Argv(1), "hudcolor"))
	{
		if(gEngfuncs.Cmd_Argc() == 2)
		{
			bWonidHUDUseColors = bWonidHUDUseColors ? 0 : 1;
			if(bWonidHUDUseColors)
				gEngfuncs.pfnConsolePrint("[sparky] bwonidhud: hudcolor ON\n");
			else
				gEngfuncs.pfnConsolePrint("[sparky] bwonidhud: hudcolor OFF\n");
		}
	}

	/*if(!strcmp(gEngfuncs.Cmd_Argv(1), "drawtime"))
	{
		char buf[256];
		if(gEngfuncs.Cmd_Argc() < 3)
		{
			sprintf(buf, "[sparky] bwonidhud: draw-time = %d)\n", bWonidHUDDrawTime);
			gEngfuncs.pfnConsolePrint(buf);
			return;
		}

		bWonidHUDDrawTime = atoi(gEngfuncs.Cmd_Argv(2));
		sprintf(buf, "[sparky] bwonidhud: draw-time = %d)\n", bWonidHUDDrawTime);
		gEngfuncs.pfnConsolePrint(buf);
	}*/

	if(!strcmp(gEngfuncs.Cmd_Argv(1), "+"))
		bWonidHUDDraw = 1;

	if(!strcmp(gEngfuncs.Cmd_Argv(1), "-"))
		bWonidHUDDraw = 0;

	if(!strcmp(gEngfuncs.Cmd_Argv(1), "capslock"))
		bWonidHUDUseCaps = bWonidHUDUseCaps ? 0 : 1;
}

void CWonid::Post_HUD_Key_Event (int eventcode, int keynum)
{
	if(bWonidHUDUseCaps)
	{
		if(keynum == 175)
		{
			if(eventcode == 1)
				bWonidHUDDraw = 1;//gEngfuncs.pfnClientCmd("+wonidhud");
			else
				bWonidHUDDraw = 0;//gEngfuncs.pfnClientCmd("-wonidhud");
		}
	}
}

void CWonid::EnsureTextFitsInOneLineAndWrapIfHaveTo (int line)
{
	int line_width = 0; int line_height = 0;
	gEngfuncs.pfnDrawConsoleStringLen(FBoutput[line], &line_width, &line_height);

	if ( (line_width + LINE_START) > MAX_LINE_WIDTH )
	{ // string is too long to fit on line
		// scan the string until we find what word is too long,  and wrap the end of the sentence after the word
		int length = LINE_START;
		int tmp_len = 0;
		char *last_break = NULL;
		for ( char *x = FBoutput[line]; *x != 0; x++ )
		{
			char buf[2];
			buf[1] = 0;

			if ( *x == ' ' && x != FBoutput[line] )  // store each line break,  except for the very first character
				last_break = x;

			buf[0] = *x;  // get the length of the current character
			gEngfuncs.pfnDrawConsoleStringLen( buf, &tmp_len, &line_height );
			length += tmp_len;

			if ( length > MAX_LINE_WIDTH )
			{  // needs to be broken up
				if ( !last_break )
					last_break = x-1;

				x = last_break;

				// need to count how many lines are in FBoutput
				int iLines = 0;
				bool bFoundIt = false;

				while (( iLines < 34 ) && ( !bFoundIt ))
				{
					if ( strcmp( FBoutput[iLines], "\0" ) == 0 )
						bFoundIt = true;

					if ( !bFoundIt )
						iLines++;
				}

				for ( int i = 0; i < (iLines - line); i++ )	// clear a space heh
				{
					strcpy( FBoutput[iLines - i], FBoutput[iLines - i - 1] );
				}

				FBwonidsfound += 1; //make sure that every line gets drawn

				int j = line + 1;
				strcpy( FBoutput[j], "" );

				// copy remaining string into next buffer,  making sure it starts with a space character
				if ( (char)*last_break == (char)' ' )
				{
					int linelen = strlen(FBoutput[j]);
					int remaininglen = strlen(last_break);

					if ( (linelen - remaininglen) <= MAX_CHARS_PER_LINE )
						strcat( FBoutput[j], last_break );
				}
				else
				{
					if ( (strlen(FBoutput[line]) - strlen(last_break) - 2) < MAX_CHARS_PER_LINE )
					{
						strcat( FBoutput[j], " " );
						strcat( FBoutput[j], last_break );
					}
				}

				*last_break = 0; // cut off the last string

				EnsureTextFitsInOneLineAndWrapIfHaveTo(j);
				break;
			}
		}
	}
}