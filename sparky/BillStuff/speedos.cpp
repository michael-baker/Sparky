/*  ------------------------------------------------------------------------
	  speedos.cpp by fb, billdoor, loki, and mstr
		- updated v1.41 - 04/09/03 - ruiner: fixed a tiny annoyance in pixels drawing
		- updated v1.38 - 02/28/03 - loki: rewrote dial speedo
	    - updated v1.37 - 02/02/03 - loki: position stuff
		- added v1.33 - 01/12/03
	------------------------------------------------------------------------ */

#include "..\client.h"

#include "hook.h"
#include "superhud.h"
#include "speedos.h"
#include "playerinfo.h"
#include "help.h"

#ifndef M_PI
#define M_PI		3.14159265358979323846	// matches value in gcc v2 math.h
#endif

#pragma warning (disable:4244) // float to int loss of data

/*Global*/
CSpeedos gSpeedos;

/* global vars for dial speedo */
float sinv[360], cosv[360];		//sin and cos values for all angles in a circle
int czR[90],czG[90],czB[90];		//colored zone rgb values
int rad;				//radius of dial speedo

DECLARE_COMMAND_2(gSpeedos, ToggleSpeedo);
DECLARE_COMMAND_2(gSpeedos, MoveSpeedo)

bool CSpeedos::Pre_HUD_Init (void)
{
	HOOK_COMMAND_2("bspeedo", ToggleSpeedo);
	gHelp.addCommand("bspeedo", "Toggles display of the speedo.\nUsage: bspeedo text/graphic/dial");
	HOOK_COMMAND_2("bspeedomove", MoveSpeedo);
	gHelp.addCommand("bspeedomove", "Adjusts the location of the speedo.\nUsage: bspeedomove u/d/l/r/def/x/y <#>\ndef - default position\nu - up\nd - down \nl - left\n r - right\nx - x coord\ny - y coord");

	return true;
}

void CSpeedos::Post_HUD_Init(void)
{
	rad = (gSuperHUD.m_scrinfo.iWidth / 16);  //  1/16 of screen width
	if(rad<40)						//minimum size of 40
		rad=40;

	iDialSpeedoX = rad + GetPixLen("999");	//left of screen
	iDialSpeedoY = gSuperHUD.m_scrinfo.iHeight * 0.6;	//in middle

	iGraphicSpeedoX = gSuperHUD.m_scrinfo.iWidth*0.4;
	iGraphicSpeedoY = gSuperHUD.m_scrinfo.iCharHeight;

	iTextSpeedoX = 16;
	iTextSpeedoY = 416; //default text speedo y
	//iTextSpeedoY = gSuperHUD.m_scrinfo.iHeight / 2;

	for(int i=90; i<=180; i++)
	{
		czR[i-90] = (270 - i) * 255 / 90 / 1.7;
		if (czR[i-90] > 255)
			czR[i-90] = 255;
		czG[i-90] = 255 - czR[i-90];
		czB[i-90]=0;

		sinv[i] = sin(i * M_PI / 180);
		cosv[i] = cos(i * M_PI / 180);
	}
	czR[0] = 255;
	czG[0] = 0;
	czB[0] = 0;
	for(i=181; i<360; i++)  //colored zone
	{
		//pre calculate all needed sin and cos values
		sinv[i] = sin(i * M_PI / 180);
		cosv[i] = cos(i * M_PI / 180);
	}
}
void CSpeedos::Post_HUD_Redraw (float flTime, int intermission)
{
	//variables used by 2 or more speedos
	char buf[1024];
	int spdR, spdG, spdB = 0, x, y;
	int speed = (int)Vector(gPlayerinfo.ppmove_cache.velocity.x, gPlayerinfo.ppmove_cache.velocity.y, (float)0).Length();
	int maxSpeed = (int)gPlayerinfo.ppmove_cache.maxspeed;

	//mstr's color code (for "speed color")
	int barWd = gSuperHUD.m_scrinfo.iWidth*0.20;
	int spd = (int)((float)speed / ((float)maxSpeed * 2.0) * (barWd - 1) );
	if (spd > (barWd - 1))
		spd = (barWd - 1);
		
	if (speed <= maxSpeed) {
		 spdR = 0;
		spdG = 255;
	} else {
		spdR = speed / (maxSpeed * 1.7) * 255;
		if (spdR > 255)
			spdR = 255;
		spdG = 255 - spdR;
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


	/* ----------------------------------Sparky's Speedometer-------------------------------
									the sparky text speedometer
	   ------------------------------------------------------------------------------------- */

	if(gPlayerinfo.bCached && bSpeedoTextOn && (gSuperHUD.iSuperHUDLevel >= 1))
	{
		sprintf(buf, "[speed - %d]", (int)Vector(gPlayerinfo.ppmove_cache.velocity.x, gPlayerinfo.ppmove_cache.velocity.y, (float)0).Length());
		gSuperHUD.DrawHudString(iTextSpeedoX,iTextSpeedoY,gSuperHUD.HUDColor.x,gSuperHUD.HUDColor.y,gSuperHUD.HUDColor.z,buf);
		sprintf(buf, "[max speed - %d]", (int)gPlayerinfo.ppmove_cache.maxspeed);
		gSuperHUD.DrawHudString(iTextSpeedoX,iTextSpeedoY+16,gSuperHUD.HUDColor.x,gSuperHUD.HUDColor.y,gSuperHUD.HUDColor.z,buf);
		if(gPlayerinfo.ppmove_cache.maxspeed == 0)
			sprintf(buf, "[percent - infinity]");
		else
			sprintf(buf, "[percent - %d]", (int)(100*((float)Vector(gPlayerinfo.ppmove_cache.velocity.x, gPlayerinfo.ppmove_cache.velocity.y, (float)0).Length()/(float)gPlayerinfo.ppmove_cache.maxspeed)));
		gSuperHUD.DrawHudString(iTextSpeedoX,iTextSpeedoY+32,gSuperHUD.HUDColor.x,gSuperHUD.HUDColor.y,gSuperHUD.HUDColor.z,buf);
	}



	/* ---------------------------------Nemesis's Speedometer-------------------------------
						 the nemesis graphical speedometer thingy by mstr.
	   ------------------------------------------------------------------------------------- */

	if(gPlayerinfo.bCached && bSpeedoGraphicOn && (gSuperHUD.iSuperHUDLevel >= 1))
	{
		x = iGraphicSpeedoX;
		y = iGraphicSpeedoY;

		gEngfuncs.pfnFillRGBA (x+1, y+1, spd, (gSuperHUD.m_scrinfo.iCharHeight)-1, spdR, spdG, spdB, 0xFF);

		gEngfuncs.pfnFillRGBA (x, y, barWd, 1, customR, customG, customB, 0xFF);
		gEngfuncs.pfnFillRGBA (x, y+(gSuperHUD.m_scrinfo.iCharHeight), barWd, 1, customR, customG, customB, 0xFF);

		//					   x  y  w  h	 r	  g	 b     a
		gEngfuncs.pfnFillRGBA (x, y+1, 1, (gSuperHUD.m_scrinfo.iCharHeight)-1, customR, customG, customB, 0xFF);
		gEngfuncs.pfnFillRGBA (x+barWd, y, 1, (gSuperHUD.m_scrinfo.iCharHeight)+1, customR, customG, customB, 0xFF);
		gEngfuncs.pfnFillRGBA (x+barWd/2, y+1, 1, (gSuperHUD.m_scrinfo.iCharHeight)-1, customR, customG, customB, 0xFF);
		gEngfuncs.pfnFillRGBA (x+barWd/1.1764705, y+1, 1, (gSuperHUD.m_scrinfo.iCharHeight)-1, customR, customG, customB, 0xFF);  
	}



	/* --------------------------------Loki's Dial Speedometer------------------------------
									       written by l0ki
	   ------------------------------------------------------------------------------------- */

	if(gPlayerinfo.bCached && bSpeedoDialOn && (gSuperHUD.iSuperHUDLevel >= 1))
	{
		rad = (gSuperHUD.m_scrinfo.iWidth / 16);  //  1/16 of screen width
		if(rad<40)						//minimum size of 40
			rad=40;
		float spdAng = 0;
		int cx = iDialSpeedoX, cy = iDialSpeedoY;
	
		/* Colored zone */
		int nexti = 90;
		for(int i=90; i<180; i++)
		{
			//draw the border point
			x = rad * sinv[i];
			y = rad * cosv[i];
			gEngfuncs.pfnFillRGBA(cx+x, cy+y, 2, 1, czR[i-90], czG[i-90], czB[i-90], 255);
			
			//draw the number and hash mark if needed
			if(i == nexti)
			{
				x = (rad + 5) * sinv[i];
				y = (rad + 5) * cosv[i];
			
				sprintf(buf,"%i",(200 - (i/18*20-100)));
				gSuperHUD.DrawHudString(cx+x, cy+y-gSuperHUD.m_scrinfo.iCharHeight/2,czR[i-90],czG[i-90],czB[i-90],buf);
				//draw hash mark
				for(int j=(rad-5);j<=(rad+5);j++)
				{
					x = j * sinv[i];
					y = j * cosv[i];
					gEngfuncs.pfnFillRGBA(cx+x, cy+y, 2, 1, czR[i-90], czG[i-90], czB[i-90], 255);
				}
				nexti += 18;
			}
		}
	
		/* Normal zone */
		nexti = 180;
		for(i=180; i<=270; i++)
		{
			//draw the border point
			x = rad * sinv[i];
			y = rad * cosv[i];
			gEngfuncs.pfnFillRGBA(cx+x, cy+y, 2, 1, customR, customG, customB, 255);
			
			//draw the number and hash mark if needed
			if(i == nexti)
			{
				x = (rad + 5) * sinv[i];
				y = (rad + 5) * cosv[i];
		
				sprintf(buf,"%i",(200 - (i/18*20-100)));
				if(i > 180)
					gSuperHUD.DrawHudString(cx+x-GetPixLen(buf),cy+y-gSuperHUD.m_scrinfo.iCharHeight/2,customR, customG, customB,buf);
				else
					gSuperHUD.DrawHudString(cx-GetPixLen(buf)/2,cy-rad-gSuperHUD.m_scrinfo.iCharHeight,customR, customG, customB,buf);
				
				//draw hash mark
				for(int j=(rad-5);j<=(rad+5);j++)
				{
					x = j * sinv[i];
					y = j * cosv[i];
					gEngfuncs.pfnFillRGBA(cx+x, cy+y, 2, 1, customR, customG, customB, 255);
				}
				nexti += 18;
			}
		}
		//draw bottom line - draw in hud color
		gEngfuncs.pfnFillRGBA(cx-rad, cy, (2 * rad), 1, customR, customG, customB, 255);

		//draw needle in appropriate color
		for (i=0; i<=rad;i++)
		{
			spdAng = 270 - (90 / (float)maxSpeed * (float)speed);
			if(spdAng < 90)
				spdAng = 90;
	
			x = i * sinv[(int)spdAng];
			y = i * cosv[(int)spdAng];
			gEngfuncs.pfnFillRGBA(cx+x, cy+y, 2, 2, spdR, spdG, spdB, 255);
		}

		sprintf(buf, "%d", speed);
		gSuperHUD.DrawHudString(cx-GetPixLen(buf)/2,cy+3,customR,customG,customB,buf);
	}
}

CSpeedos::CSpeedos(void)
{
}

void CSpeedos::Cmd_ToggleSpeedo (void)
{
	if(gEngfuncs.Cmd_Argc() < 2)
	{
		gEngfuncs.pfnConsolePrint("[sparky] valid bspeedo commands: text graphic dial\n");
		return;
	}

	if(!strcmp(gEngfuncs.Cmd_Argv(1), "text"))
	{
		bSpeedoTextOn = bSpeedoTextOn ? 0 : 1;
		bSpeedoDialOn = 0;
		bSpeedoGraphicOn = 0;

		if(bSpeedoTextOn)
			gSuperHUD.WriteHUDChannel(HC_IN_GAME_COLOR, 2, "speedo text ON");
		else
			gSuperHUD.WriteHUDChannel(HC_IN_GAME_COLOR, 2, "speedo text OFF");
	}

	if(!strcmp(gEngfuncs.Cmd_Argv(1), "graphic"))
	{
		bSpeedoGraphicOn = bSpeedoGraphicOn ? 0 : 1;
		bSpeedoTextOn = 0;
		bSpeedoDialOn = 0;

		if(bSpeedoGraphicOn)
			gSuperHUD.WriteHUDChannel(HC_IN_GAME_COLOR, 2, "speedo graphic ON");
		else
			gSuperHUD.WriteHUDChannel(HC_IN_GAME_COLOR, 2, "speedo graphic OFF");
	}

	if(!strcmp(gEngfuncs.Cmd_Argv(1), "dial"))
	{
		bSpeedoDialOn = bSpeedoDialOn ? 0 : 1;
		bSpeedoTextOn = 0;
		bSpeedoGraphicOn = 0;

		if(bSpeedoDialOn)
			gSuperHUD.WriteHUDChannel(HC_IN_GAME_COLOR, 2, "speedo dial ON");
		else
			gSuperHUD.WriteHUDChannel(HC_IN_GAME_COLOR, 2, "speedo dial OFF");
	}	
}

void CSpeedos::Cmd_MoveSpeedo(void)
{
	if(gEngfuncs.Cmd_Argc() < 2)
	{
		gEngfuncs.pfnConsolePrint("[sparky] valid bspeedomove commands: u/d/l/r/def/x/y <#>\n");
		return;
	}

	if(!bSpeedoGraphicOn && !bSpeedoTextOn & !bSpeedoDialOn)
	{
		gEngfuncs.pfnConsolePrint("[sparky] ERROR: Speedo is not active\n");
		return;
	}

	int *activeSpeedoX, *activeSpeedoY;

	if(bSpeedoTextOn)
	{
		activeSpeedoX = &iTextSpeedoX;
		activeSpeedoY = &iTextSpeedoY;
	}
	if(bSpeedoDialOn)
	{
		activeSpeedoX = &iDialSpeedoX;
		activeSpeedoY = &iDialSpeedoY;
	}
	if(bSpeedoGraphicOn)
	{
		activeSpeedoX = &iGraphicSpeedoX;
		activeSpeedoY = &iGraphicSpeedoY;
	}

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
			gEngfuncs.Con_Printf("[sparky] Active speedo x is %i\n",*activeSpeedoX);
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
			gEngfuncs.Con_Printf("[sparky] Active speedo y is %i\n",*activeSpeedoY);
		int iNewVal;
		if(atoi(gEngfuncs.Cmd_Argv(2)))
		{
			iNewVal = atoi(gEngfuncs.Cmd_Argv(2));
			*activeSpeedoY = iNewVal;
		}
	}
	if(!strcmp(gEngfuncs.Cmd_Argv(1),"def"))
	{
		if(activeSpeedoX == &iDialSpeedoX)
		{
			int rad = (gSuperHUD.m_scrinfo.iWidth / 16);  //  1/16 of screen width
			if(rad<40)						//minimum size of 40
				rad=40;
			iDialSpeedoX = rad + GetPixLen("999");	//left of screen
			iDialSpeedoY = gSuperHUD.m_scrinfo.iHeight * 0.6;	//in middle
		}
		if(activeSpeedoX == &iGraphicSpeedoX)
		{
			iGraphicSpeedoX = gSuperHUD.m_scrinfo.iWidth*0.4;
			iGraphicSpeedoY = gSuperHUD.m_scrinfo.iCharHeight;
		}
		if(activeSpeedoX == &iTextSpeedoX)
		{
			iTextSpeedoX = 16;
			iTextSpeedoY = gSuperHUD.m_scrinfo.iHeight / 2;
		}
	}
}

int CSpeedos::GetPixLen(char *text)
{
	int len = 0;
	for ( char * p = text; *p; p++ )
		len += gSuperHUD.m_scrinfo.charWidths[*p];
	return len;
}