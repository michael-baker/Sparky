/*  ------------------------------------------------------------------------
	  spammeter.cpp by ruiner based on speedos by mstr
		- added v1.41 - 04/09/03
	------------------------------------------------------------------------ */

#include "..\client.h"

#include "hook.h"
#include "superhud.h"
#include "spammeter.h"
#include "help.h"

#pragma warning (disable:4244) // float to int loss of data

/*Global*/
CSpamMeter gSpamMeter;

DECLARE_COMMAND_2(gSpamMeter, ToggleSpamMeter);
DECLARE_COMMAND_2(gSpamMeter, MoveSpamMeter)
DECLARE_COMMAND_2(gSpamMeter, SpamCount)

bool CSpamMeter::Pre_HUD_Init (void)
{
	HOOK_COMMAND_2("bspammeter", ToggleSpamMeter);
	gHelp.addCommand("bspammeter", "Toggles display of the spammeter\nUsage: bspammeter\nSee also: bspammetermove, bspamcount");
	HOOK_COMMAND_2("bspammetermove", MoveSpamMeter);
	gHelp.addCommand("bspammetermove", "Adjusts the location of the spammeter.\nUsage: bspammetermove u/d/l/r/def/x/y <#>\ndef - default position\nu - up\nd - down \nl - left\n r - right\nx - x coord\ny - y coord");
	HOOK_COMMAND_2("bspamcount", SpamCount);
	gHelp.addCommand("bspamcount", "Sets the maximum spam amount for the top of the scale to be reached.\nUsage: bspamcount <value>\nDefault is 12.");

	return true;
}

void CSpamMeter::Post_HUD_Init(void)
{
	iGraphicSpamMeterX = gSuperHUD.m_scrinfo.iWidth*0.4;
	iGraphicSpamMeterY = gSuperHUD.m_scrinfo.iCharHeight * 2 + 5;
	iMaxSpam = 12;
}

bool CSpamMeter::Pre_HUD_AddEntity (int type, struct cl_entity_s *ent, const char *modelname)
{
	if(strstr(modelname, "explode") || strstr(modelname,"spy_grenade") || strstr(modelname,"ngren"))
		iSpamCount++;

	if(strstr(modelname, "flame"))
		iFlameCount++;

	return true;
}

void CSpamMeter::Post_HUD_Redraw (float flTime, int intermission)
{
	int spdR, spdG, spdB = 0, x, y;

	//mstr's color code (for "speed color")
	int barWd = gSuperHUD.m_scrinfo.iWidth*0.20;

	spdB = 0;
	spdG = 50;
	spdR = iSpamCount * 255 / iMaxSpam + 50;

	if(spdR > 255) {
		spdR = 255;
		spdG = 0;
	}

	//hud color stuffs!
	int customR, customG, customB;
	if((gSuperHUD.HUDColor.x >= 0) && (gSuperHUD.HUDColor.y >= 0) && (gSuperHUD.HUDColor.z >= 0))
	{
		customR = gSuperHUD.HUDColor.x;
		customG = gSuperHUD.HUDColor.y;
		customB = gSuperHUD.HUDColor.z;	
	}
	else
	{
		customR = 240;	
		customG = 179;
		customB = 16;
	}

	/* ---------------------------------Nemesis's Speedometer-------------------------------
						 the nemesis graphical speedometer thingy by mstr.
	   ------------------------------------------------------------------------------------- */

	if(bSpamMeterGraphicOn && (gSuperHUD.iSuperHUDLevel >= 1))
	{
		x = iGraphicSpamMeterX;
		y = iGraphicSpamMeterY;
		int spam = (iSpamCount + iFlameCount / 15) * barWd / iMaxSpam;

		if(iFlameCount > 0)
			spam++;

		if (spam > barWd)
			spam = barWd;

		if(iLastSpam < spam) // smooth it a bit
			spam = iLastSpam + 1 + (spam - iLastSpam) / 5;

		if(iLastSpam > spam) // smooth it a bit
			spam = iLastSpam - 1 - (iLastSpam - spam) / 5;

		iLastSpam = spam;
		
		gEngfuncs.pfnFillRGBA (x+1, y+1, spam, (gSuperHUD.m_scrinfo.iCharHeight)-1, spdR, spdG, spdB, 0xFF);
		gEngfuncs.pfnFillRGBA (x, y, barWd, 1, customR, customG, customB, 0xFF);
		gEngfuncs.pfnFillRGBA (x, y+(gSuperHUD.m_scrinfo.iCharHeight), barWd, 1, customR, customG, customB, 0xFF);
		gEngfuncs.pfnFillRGBA (x, y+1, 1, (gSuperHUD.m_scrinfo.iCharHeight)-1, customR, customG, customB, 0xFF);
		gEngfuncs.pfnFillRGBA (x+barWd, y, 1, (gSuperHUD.m_scrinfo.iCharHeight)+1, customR, customG, customB, 0xFF);
	}
	iSpamCount = 0;
	iFlameCount = 0;
}

void CSpamMeter::Cmd_ToggleSpamMeter (void)
{
	bSpamMeterGraphicOn = bSpamMeterGraphicOn ? 0 : 1;

	if(bSpamMeterGraphicOn)
		gSuperHUD.WriteHUDChannel(HC_IN_GAME_COLOR, 2, "spammeter ON");
	else
		gSuperHUD.WriteHUDChannel(HC_IN_GAME_COLOR, 2, "spammeter OFF");
}

void CSpamMeter::Cmd_SpamCount(void)
{
	if(gEngfuncs.Cmd_Argc() < 1)
	{
		gEngfuncs.pfnConsolePrint("[sparky] please specify amount of spam.");
		return;
	}
	if(atoi(gEngfuncs.Cmd_Argv(1)))
	{
		iMaxSpam = atoi(gEngfuncs.Cmd_Argv(1));
	}
}

void CSpamMeter::Cmd_MoveSpamMeter(void)
{
	if(gEngfuncs.Cmd_Argc() < 2)
	{
		gEngfuncs.pfnConsolePrint("[sparky] valid bspammetermove commands: u/d/l/r/def/x/y <#>\n");
		return;
	}

	if(!bSpamMeterGraphicOn)
	{
		gEngfuncs.pfnConsolePrint("[sparky] ERROR: Speedo is not active\n");
		return;
	}

	int *activeSpeedoX, *activeSpeedoY;

	activeSpeedoX = &iGraphicSpamMeterX;
	activeSpeedoY = &iGraphicSpamMeterY;

	if(!strcmp(gEngfuncs.Cmd_Argv(1),"u"))
	{
		*activeSpeedoY -= 10;
	}

	if(!strcmp(gEngfuncs.Cmd_Argv(1),"d"))
	{
		*activeSpeedoY += 10;
	}
	
	if(!strcmp(gEngfuncs.Cmd_Argv(1),"l"))
	{
		*activeSpeedoX -= 10;
	}
	
	if(!strcmp(gEngfuncs.Cmd_Argv(1),"r"))
	{
		*activeSpeedoX += 10;
	}
	if(!strcmp(gEngfuncs.Cmd_Argv(1),"x"))
	{
		if(gEngfuncs.Cmd_Argc() < 3)
			gEngfuncs.Con_Printf("[sparky] Active spammeter x is %i\n",*activeSpeedoX);
		int iNewVal;
		if(atoi(gEngfuncs.Cmd_Argv(2)))
		{
			iNewVal = atoi(gEngfuncs.Cmd_Argv(2));
			*activeSpeedoX = iNewVal;
		}
	}
	if(!strcmp(gEngfuncs.Cmd_Argv(1),"y"))
	{
		if(gEngfuncs.Cmd_Argc() < 3)
			gEngfuncs.Con_Printf("[sparky] Active spammeter y is %i\n",*activeSpeedoY);
		int iNewVal;
		if(atoi(gEngfuncs.Cmd_Argv(2)))
		{
			iNewVal = atoi(gEngfuncs.Cmd_Argv(2));
			*activeSpeedoY = iNewVal;
		}
	}
	if(!strcmp(gEngfuncs.Cmd_Argv(1),"def"))
	{
		iGraphicSpamMeterX = gSuperHUD.m_scrinfo.iWidth*0.4;
		iGraphicSpamMeterY = gSuperHUD.m_scrinfo.iCharHeight * 2 + 5;
	}
}

int CSpamMeter::GetPixLen(char *text)
{
	int len = 0;
	for ( char * p = text; *p; p++ )
		len += gSuperHUD.m_scrinfo.charWidths[*p];
	return len;
}