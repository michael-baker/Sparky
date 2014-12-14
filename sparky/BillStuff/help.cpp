/*  ------------------------------------------------------------------------
	  help.cpp by Fr33
		- april 2003
		- Updates: 
			* better wildcard handling
			* returns appropriate command if only one wildcard match found

	------------------------------------------------------------------------ */

#include "..\client.h"
#include "hook.h"
#include "help.h"
#include <vector>
#include <string.h>
#include "util.h"

/* Wildcard Function */
int wildcmp(char *wild, char *string);

using namespace std;

/*Global*/
CSparkyHelp gHelp;

DECLARE_COMMAND_2(gHelp, SparkyHelp);


bool CSparkyHelp::Pre_HUD_Init (void)
{
	HOOK_COMMAND_2("bhelp", SparkyHelp);

	return true;
}

void CSparkyHelp::Post_HUD_Init (void)
{
	// Add any other commands here

	// Messagemode replacement vars
	addCommand("%c", "%c is replaced with your current class.\nUsage: say_team I am a %c");
	addCommand("%l", "%l is replaced with your current location.\nUsage: say_team Dead at %l");
	addCommand("%p", "%p is replaced with the location at which you are looking.\nUsage: say_team Flag thrown - %p");
	addCommand("%t", "%t is replaced with the timeleft in the game, provided btimers timeleft is on.\nUsage: say %t");
	addCommand("%en", "%en is replaced with the name of the enemy you last killed.\nUsage: say_team I killed %en");
	addCommand("%ec", "%ec is replaced with the class of the enemy you last killed.\nUsage: say_team I killed %en [%ec]");
	addCommand("%kn", "%kn is replaced with the name of the enemy who last killed you.\nUsage: say_team %kn killed me");
	addCommand("%kc", "%kc is replaced with the class of the enemy who last killed you.\nUsage: say_team %kn [%kc] killed me");
	addCommand("%scb", "%scb is replaced with blue's current score\nUsage: bircinput \\say #chan Blue - %scb");
	addCommand("%scr", "%scr is replaced with red's current score\nUsage: bircinput \\say #chan Red - %scr");
	addCommand("%scg", "%scg is replaced with green's current score\nUsage: bircinput \\say #chan Green - %scg");
	addCommand("%scy", "%scy is replaced with yellow's current score\nUsage: bircinput \\say #chan Yellow - %scy");
	//End messagemode replacement vars
}	

void CSparkyHelp::addCommand(char *command, char *text)
{
	sHelp temp;
	temp.command = command;
	temp.text = text;
	cmdList.push_back(temp);
}

void CSparkyHelp::Cmd_SparkyHelp (void)
{
	if (gEngfuncs.Cmd_Argc() < 2) 
	{
		gEngfuncs.pfnConsolePrint("bhelp usage: bhelp <command> (* - wildcard)");	
		return;
	}

	if(strstr(gEngfuncs.Cmd_Argv(1), "*") || !strcmp(gEngfuncs.Cmd_Argv(1), "*")) 
	{
		char wildcard[1024];

		sprintf(wildcard, "%s", gEngfuncs.Cmd_Argv(1));
		
		char cbuf[60][256];

		int a = 0; // counter
		for(int z=0; z<cmdList.size(); z++)
		{
			if(wildcmp(wildcard, cmdList[z].command)) // Get a wildcard comparison
			{
				a++;
				sprintf(cbuf[a-1], "%s", cmdList[z].command);
			}
		}
		// Handle returned results
		if(a==0)
		{
			gEngfuncs.Con_Printf("%s\n", "====================");
			gEngfuncs.Con_Printf("No help availble for '%s'\n", gEngfuncs.Cmd_Argv(1)); 
			gEngfuncs.Con_Printf("%s\n", "====================");
		}
		else
		{

			if(a==1) // Only one result .'. Get that command
			{
				for(int z=0; z<cmdList.size(); z++)
				{	
					if(!strcmp(cbuf[0], cmdList[z].command)) // Get a standard comparison
					{
						gEngfuncs.Con_Printf("%s\n", "====================");
						gEngfuncs.Con_Printf("%s\n", cmdList[z].command);
						gEngfuncs.Con_Printf("%s\n", "====================");
						gEngfuncs.Con_Printf("%s\n", cmdList[z].text);
						gEngfuncs.Con_Printf("%s\n", "====================");
					}
				}
			}
			else 
			{
				gEngfuncs.Con_Printf("%s\n", "====================");
				gEngfuncs.Con_Printf("%s\n", gEngfuncs.Cmd_Argv(1));
				gEngfuncs.Con_Printf("%s\n", "====================");
				//if(a < 11){
				for(int x = 0; x<a; x++){
					gEngfuncs.Con_Printf("%s\n", cbuf[x]);
				}
				/*} else {
					int x = 0;

					int ibuf;

				

					if(gEngfuncs.Cmd_Argv(2)) 
					{
						ibuf = atoi(gEngfuncs.Cmd_Argv(2));
						if((ibuf+10) < a){
							x = ibuf; // Set the start
						}
					}
					//Fix this up, results > 10
					int c = a;
					if((x+10) < a) { // Still more
						c = x+11;
					}
					else {
						c = a % 10;
					}
	
					for(x; x<c; x++){
						gEngfuncs.Con_Printf("%s\n", cbuf[x]);
					}	
					gEngfuncs.Con_Printf("%s\n", "Not all results could be displayed");
				}*/
				gEngfuncs.Con_Printf("%s\n", "====================");
				gEngfuncs.Con_Printf("%i %s\n", a, "results matching");
				gEngfuncs.Con_Printf("%s\n", "====================");
				}
			}
		}
	else 
	{
		char *text = lookup(gEngfuncs.Cmd_Argv(1)); 
		if (text)
		{
			gEngfuncs.Con_Printf("%s\n", "====================");
			gEngfuncs.Con_Printf("%s\n", gEngfuncs.Cmd_Argv(1));
			gEngfuncs.Con_Printf("%s\n", "====================");
			gEngfuncs.Con_Printf("%s\n", text);
			gEngfuncs.Con_Printf("%s\n", "====================");

		}
	
		else
		{
			gEngfuncs.Con_Printf("%s\n", "====================");
			gEngfuncs.Con_Printf("No help availble for '%s'\n", gEngfuncs.Cmd_Argv(1)); 
			gEngfuncs.Con_Printf("%s\n", "====================");
		}
	}
}

char *CSparkyHelp::lookup(char *command) 
{
		for (int i=0; i<cmdList.size(); i++)
		{ 
			if (!strcmp(cmdList[i].command, command))
				return cmdList[i].text; 
		} 
	return NULL; 
}

int wildcmp(char *wild, char *string) {
	char *cp, *mp;
	
	while ((*string) && (*wild != '*')) {
		if ((*wild != *string) && (*wild != '?')) {
			return 0;
		}
		wild++;
		string++;
	}
		
	while (*string) {
		if (*wild == '*') {
			if (!*++wild) {
				return 1;
			}
			mp = wild;
			cp = string+1;
		} else if ((*wild == *string) || (*wild == '?')) {
			wild++;
			string++;
		} else {
			wild = mp;
			string = cp++;
		}
	}
		
	while (*wild == '*') {
		wild++;
	}
	return !*wild;
}

CSparkyHelp::CSparkyHelp (void)
{
}

