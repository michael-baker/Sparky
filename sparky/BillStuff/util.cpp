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
 * $Id: util.cpp,v 1.4 2001/10/04 10:31:24 vasily Exp $
 */

#include <stdio.h>
#include "..\client.h"


const char * dump (void * buf, int size)
{
	static char textbuf[256];

    sprintf(textbuf, "%d bytes: ", size);

	for ( int i = 0; i < size; i++ )
	{
		unsigned char byte = ((unsigned char *)buf)[i];
		sprintf(textbuf+strlen(textbuf), "0x%02X(%c) ", 
				(unsigned int) byte, 
				(byte >= ' ' && byte < 128) ? byte : '?' );

		if ( strlen(textbuf) > sizeof(textbuf) - 20 )
			break;
	}

	return textbuf;
}

/*
HSPRITE LoadSprite(const char *pszName)
{
	int i;
	char sz[256]; 

	if ( gHud.m_scrinfo.iWidth < 640 )
		i = 320;
	else
		i = 640;

	sprintf (sz, pszName, i);

	return SPR_Load(sz);
}
*/

bool str_prefix (const char * substr, const char * str)
{
	if ( !*substr )
		return true;

	for ( ; *substr && *str; substr++, str++ )
		if ( *substr != *str )
			return true;

	return false;
}


const char * get_argument (const char *string, char *buf, size_t bufsize)
{
    while ( isspace(*string) )
        string++;

    while ( *string != '\0' && bufsize > 1 )
    {
        if ( isspace(*string) )
        {
            string++;
            break;
        }

        *buf++ = *string++;
        bufsize--;
    }

    *buf = '\0';

    while ( isspace(*string) )
        string++;

    return string;
}

bool CalcScreen(float *origin,float *vecScreen)
{
	SCREENINFO m_scrinfo;
	m_scrinfo.iSize = sizeof(m_scrinfo);
	gEngfuncs.pfnGetScreenInfo (&m_scrinfo);
	int result = gEngfuncs.pTriAPI->WorldToScreen(origin,vecScreen);
	if(vecScreen[0] < 1 && vecScreen[1] < 1 && vecScreen[0] > -1 && vecScreen[1] > -1 && !result)
	{
	vecScreen[0] = vecScreen[0] * m_scrinfo.iWidth/2 + m_scrinfo.iWidth/2;
	vecScreen[1] = -vecScreen[1] * m_scrinfo.iHeight/2 + m_scrinfo.iHeight/2;
	return true;
	}
	return false;
}

char *NameFromModel(const char *szModel)
{
	static char buf[512], temp[512];
	sprintf(buf, "%s", szModel);
	int i, startpos = 0, endpos = strlen(buf);
	for(i = 0; i < int(strlen(buf)); i++)
	{
		if(buf[i] == '/') startpos = i+1;
		if(buf[i] == '.') {buf[i] = '\0'; break;}
	}
	sprintf(temp, "%s", &buf[startpos]);
	
	return temp;
}

char *NameFromFileName(const char *szFileName)
{
	static char buf[512], temp[512];
	sprintf(buf, "%s", szFileName);
	int i, startpos = 0, endpos = strlen(buf);
	for(i = 0; i < int(strlen(buf)); i++)
	{
		if(buf[i] == '/') startpos = i+1;
		if(buf[i] == '.') {buf[i] = '\0'; break;}
	}
	sprintf(temp, "%s", &buf[startpos]);
	
	return temp;
}


char *NameFromClass(int playerclass)
{
	if(playerclass == 0) return "spectator";
	if(playerclass == 1) return "scout";
	if(playerclass == 2) return "sniper";
	if(playerclass == 3) return "soldier";
	if(playerclass == 4) return "demoman";
	if(playerclass == 5) return "medic";
	if(playerclass == 6) return "hwguy";
	if(playerclass == 7) return "pyro";
	if(playerclass == 8) return "spy";
	if(playerclass == 9) return "engineer";
	if(playerclass == 10) return "random";
	if(playerclass == 11) return "civilian";
	return "unknown";
}

Vector ColorFromTeam(int team)
{
	if(team == 0) return Vector(255, 255, 255);
	if(team == 1) return Vector(64, 64, 255);
	if(team == 2) return Vector(255, 64, 64);
	if(team == 3) return Vector(64, 255, 64);
	if(team == 4) return Vector(64, 255, 255);
	return Vector(255,255,255);
}

void RemoveIllegalsFromFileName(char *szFileName)
{
	char *illegal = ".,/:;[]{}\\\"\'*?~`@|\t\n<>";
	if(strlen(szFileName) > 255) szFileName[255] = '\0';
	for (int i = 0; i < int(strlen(szFileName)); i++)
	{
		for(int j = 0; j < int(strlen(illegal)); j++)
		{
			if((szFileName[i] == illegal[j]) || (szFileName[i] == '\0'))
				szFileName[i] = '-';
		}
	}
}

void RemoveIllegalsFromDemoName(char *szFileName) // Added by Zoltarc
{
	char *illegal = ".,/:;\\\"\'*?~`@|\t\n<>";
	if(strlen(szFileName) > 255) szFileName[255] = '\0';
	for (int i = 0; i < int(strlen(szFileName)); i++)
	{
		for(int j = 0; j < int(strlen(illegal)); j++)
		{
			if((szFileName[i] == illegal[j]) || (szFileName[i] == '\0'))
				szFileName[i] = '_';
		}
	}
}

void PathFromFileName(char *szFileName)
{
}

char *StringSearchReplace(const char *Search, const char *Replace, const char *Input)
{
	static char Output[8192], Temp[8192];
	char *ptr;
	sprintf(Output, "%s", Input);
	while(ptr = strstr(Output, Search))
	{
		*ptr = '\0';
		sprintf(Temp, "%s%s%s", Output, Replace, ptr + strlen(Search));
		sprintf(Output, "%s", Temp);
	}
	return Output;
}
