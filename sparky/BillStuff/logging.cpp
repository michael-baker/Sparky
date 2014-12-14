/*  
 * Half-Life client-side generic hook engine.
 * Copyright (c) Vasily Pupkin (clientbot@narod.ru) 2000-2001
 * Copyright (c) Tim Yunaev (tim@lviv.gu.net) 2000-2001
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * NOTE:
 * GNU license doesn't cover Engine directory.
 * Content of Engine directory is copyrighted (c) 1999, 2000, by Valve LLC
 * and it is licensed under Valve's proprietary license (see original HL SDK).
 *
 * $Id: menu.h,v 1.2 2001/07/16 11:23:10 vasily Exp $
 */

#include "..\client.h"

#include "hook.h"
#include "superhud.h"
#include "logging.h"
#include "util.h"
#include "help.h"
#include <time.h>

const int LOGMSG_BUF = 512;

/*Global*/
CLogging gLogging;

DECLARE_COMMAND_2(gLogging, LogLevel);

bool CLogging::Pre_HUD_Init (void)
{
	HOOK_COMMAND_2("bloglevel", LogLevel);
	gHelp.addCommand("bloglevel", "Enables logging to a filename (the server name) in sparkyutils/logs.\nUsage: bloglevel 0/1/2/3\n0 - no logging (default)\n1 - log chat only\n2 - log chat and killn3 - log chat, kills, and debug info");
	return true;
}

void CLogging::Post_HUD_Init (void)
{
}

bool CLogging::Pre_HUD_Frame (double time)
{
	return true;
}

void CLogging::Post_HUD_Frame (double time)
{
	// open new log file for the new server
	if(bChangedServer && iLogLevel)
	{
		char FileName[1024];
		char buf[1024];
		if(strlen(ServerName) > 1023) ServerName[1023] = '\0';
		sprintf(buf, "%s", ServerName);
		RemoveIllegalsFromFileName(buf);
		sprintf(FileName, "sparkyutils\\logs\\%s.log", buf);

		// close old file
		if(fpLog) fclose(fpLog);

		if(fpLog = fopen(FileName, "at"))
		{
			bLogging = 1;
			char tbuf[64];
			char dbuf[64];
			_strtime(tbuf);
			_strdate(dbuf);
			if(iLogLevel) fprintf(fpLog, "\n[%s] [%s] [logging begins]\n", tbuf, dbuf);
		}
		else
		{
			bLogging = 0;
			gEngfuncs.pfnConsolePrint("[sparky] could not open log file for writing\n");
		}
	}

	// check for new map
	if(((fLastTime-10) > gEngfuncs.GetClientTime()) || bChangedServer)
	{
		fLastTime = gEngfuncs.GetClientTime();
		if(bLogging && iLogLevel)
		{
			char tbuf[64];
			char dbuf[64];
			_strtime(tbuf);
			_strdate(dbuf);
			fprintf(fpLog, "[%s] [%s] [map: %s]\n", tbuf, dbuf, gEngfuncs.pfnGetLevelName());
		}
	}
	else
	{
		fLastTime = gEngfuncs.GetClientTime();
	}

	bChangedServer = 0;
}

bool CLogging::Pre_Hook_TextMsg (const char *pszString)
{
	return true;
}

void CLogging::Post_Hook_TextMsg (const char *pszString)
{
	add_log("[Post_Hook_TextMsg] %s", pszString);
}

bool CLogging::Pre_Hook_SayText (const char *pszString)
{
	return true;
}

void CLogging::Post_Hook_SayText (const char *pszString)
{
	if(bLogging && iLogLevel && fpLog)
	{
		char tbuf[64];
		_strtime(tbuf);
		fprintf(fpLog, "[%s] %s", tbuf, pszString);
	}
}

//added by FB
bool CLogging::Pre_Hook_HudText (const char *pszString)
{
	return true;
}

//added by FB
void CLogging::Post_Hook_HudText (const char *pszString)
{
	add_log("[Post_Hook_HudText] %s", pszString);
}

bool CLogging::Pre_Hook_DeathMsg (int killer, int victim, const char *pszWeapon)
{
	return true;
}

void CLogging::Post_Hook_DeathMsg (int killer, int victim, const char *pszWeapon)
{
	if(bLogging && (iLogLevel > 1) && fpLog)
	{
		hud_player_info_t pvictiminfo;
		hud_player_info_t pkillerinfo;
		char tbuf[64];

		_strtime(tbuf);

		if(!killer || (killer == victim))
		{
			gEngfuncs.pfnGetPlayerInfo(victim, &pvictiminfo);
			fprintf(fpLog, "[%s] %s killed self with %s\n", tbuf, pvictiminfo.name, pszWeapon);
		}
		else
		{
			gEngfuncs.pfnGetPlayerInfo(victim, &pvictiminfo);
			gEngfuncs.pfnGetPlayerInfo(killer, &pkillerinfo);
			fprintf(fpLog, "[%s] %s killed %s with %s\n", tbuf, pkillerinfo.name, pvictiminfo.name, pszWeapon);
		}
	}
}

void CLogging::Cmd_LogLevel(void)
{
	char buf[4096];
	int newlevel;
	if(gEngfuncs.Cmd_Argc() < 2)
	{
		sprintf(buf, "[sparky] bloglevel is %d\n", iLogLevel);
		gEngfuncs.pfnConsolePrint(buf);
	}
	else
	{
		newlevel = atoi(gEngfuncs.Cmd_Argv(1));
		if(strcmp(ServerName, "not connected") && !iLogLevel)
		{
			bChangedServer = true;
		}
		if(newlevel == 0) iLogLevel = 0;
		if(newlevel == 1) iLogLevel = 1;
		if(newlevel == 2) iLogLevel = 2;
		if(newlevel == 3) iLogLevel = 3;
	}
}

void CLogging::add_log (const char *fmt, ...)
{
	va_list va_alist;
	static char szLastLogMessage[LOGMSG_BUF];
	static int iRepeatCounter = 0;
	char logbuf[LOGMSG_BUF];
    //FILE * fp;

	va_start (va_alist, fmt);
	_vsnprintf (logbuf, sizeof(logbuf), fmt, va_alist);
	va_end (va_alist);

	if ( !strcmp(logbuf, szLastLogMessage) )
	{
		iRepeatCounter++;
		return;
	}
	
   	time_t ticktime = time (NULL);
	struct tm * timestruct = localtime (&ticktime);

	if(fpLog && iLogLevel > 2)
	{
		if ( iRepeatCounter )
			fprintf ( fpLog, "[%02d:%02d:%02d] Last message has been repeated %d times\n", 
					timestruct->tm_hour, 
					timestruct->tm_min, 
					timestruct->tm_sec, 
					iRepeatCounter );

		iRepeatCounter = 0;
		strcpy(szLastLogMessage, logbuf);
		
		fprintf ( fpLog, "[%02d:%02d:%02d] %s\n", 
					timestruct->tm_hour, 
					timestruct->tm_min, 
					timestruct->tm_sec, 
					logbuf );

		//fclose (fp);
	}
	else
		printf("%s\n", logbuf);
}
	
CLogging::CLogging(void)
{
	sprintf(ServerName, "not connected");
	fLastTime = 0;
	bChangedServer = 0;
	bLogging = 0;
	iLogLevel = 0;
}

CLogging::~CLogging(void)
{
	if(fpLog && iLogLevel)
	{
		char tbuf[64];
		char dbuf[64];
		_strtime(tbuf);
		_strdate(dbuf);
		fprintf(fpLog, "[%s] [%s] [quit]\n", tbuf, dbuf);
		fclose(fpLog);
	}
}