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
 * $Id: hookmessages.cpp,v 1.3 2001/10/04 10:31:24 vasily Exp $
 */

#include "client.h"
#include "hookmessages.h"
#include "BillStuff\hook.h"
#include "BillStuff\logging.h"
#include "BillStuff\triggers.h"
#include "BillStuff\fb.h" //added
#include "BillStuff\stats.h" //added
#include "BillStuff\laserstatus.h" //added
#include "BillStuff\flagstatus.h" //added
#include <time.h>


typedef struct
{
	const char 		* 	name;
	pfnUserMsgHook		hooked_func;
	pfnUserMsgHook		orig_func;
	const char 		* 	msgdump;

} hook_entry_t;


static hook_entry_t * hook_lookup (const char * name);
static void dump_message (const char * dumpstr, const char * name, void * pbuf, int size);
static int univ_dump_hook (const char *pszName, int iSize, void *pbuf);


static hook_entry_t hook_list[] =
{
	{	"SayText",		hook_saytext,		NULL,	"BS"	},
	{	"ShowMenu",		univ_dump_hook,		NULL,	"SCBS"	},
	{	"DeathMsg",		hook_deathmsg,		NULL,	"BBBS"	},
	{	"TextMsg",		hook_textmsg,		NULL,	"BSSSSS"},
	{	"HudText",		hook_hudtext,		NULL,	"SSS"	}, //added
	{	"ResetHUD",		hook_resethud,		NULL,	NULL	},
	{	"SetFOV",		hook_setfov,		NULL,	"B"		},
	{	"ServerName",	hook_servername,	NULL,	"S"		},
	{	"TeamScore",	hook_teamscore,		NULL,	"SH"	}, //added
	{	"StatusIcon",	hook_statusicon,	NULL,	"BS"	}, //added
	{	"ScoreInfo",	hook_scoreinfo,		NULL,	"BHHH"	}, //added by loki
	{	"AmmoX",		hook_ammox,			NULL,	"BB"	}, //added by Mulchman
	{	"Feign",		hook_feign,			NULL,	"B"		}, //added by Mulchman
	{	"Detpack",		hook_detpack,		NULL,	"B"		},
	{	NULL,			NULL,				NULL,	NULL	}
};

// Added by Mulchman
int hook_detpack (const char *pszName, int iSize, void *pbuf)
{
	hook_entry_t * hook = hook_lookup(pszName);
	pfnUserMsgHook pOrigFun = hook ? hook->orig_func : 0;
	
	int retval = 0;

	BEGIN_READ (pbuf, iSize);

	int iIsSettingDetpack = READ_BYTE();

	// Run Pre hook, abort if necessary
	if(!gMainHook.Pre_Hook_Detpack(iIsSettingDetpack))
		return 0;

	if(pOrigFun)
		retval = (*pOrigFun)(pszName, iSize, pbuf);

	// Run post hook
	gMainHook.Post_Hook_Detpack(iIsSettingDetpack);

	return retval;
}

// Added by Mulchman
int hook_feign (const char *pszName, int iSize, void *pbuf)
{
	hook_entry_t * hook = hook_lookup(pszName);
	pfnUserMsgHook pOrigFun = hook ? hook->orig_func : 0;
	
	int retval = 0;

	BEGIN_READ (pbuf, iSize);

	int iIsFeigning = READ_BYTE();

	// Run Pre hook, abort if necessary
	if(!gMainHook.Pre_Hook_Feign(iIsFeigning))
		return 0;

	if(pOrigFun)
		retval = (*pOrigFun)(pszName, iSize, pbuf);

	// Run post hook
	gMainHook.Post_Hook_Feign(iIsFeigning);

	return retval;
}	

// Added by Mulchman
int hook_ammox (const char *pszName, int iSize, void *pbuf)
{
	hook_entry_t * hook = hook_lookup(pszName);
	pfnUserMsgHook pOrigFun = hook ? hook->orig_func : 0;
	
	int retval = 0;

	BEGIN_READ (pbuf, iSize);

	int iIndex = READ_BYTE();
	int iCount = READ_BYTE();

	// Run Pre hook, abort if necessary
	if(!gMainHook.Pre_Hook_AmmoX(iIndex, iCount))
		return 0;

	if(pOrigFun)
		retval = (*pOrigFun)(pszName, iSize, pbuf);

	// Run post hook
	gMainHook.Post_Hook_AmmoX(iIndex, iCount);

	return retval;
}

//added by L0ki
int hook_scoreinfo (const char *pszName, int iSize, void *pbuf)
{
	int retval = 0;
	hook_entry_t * hook = hook_lookup(pszName);
	pfnUserMsgHook pOrigFun = hook ? hook->orig_func : 0;
	if ( pOrigFun )
		retval = (*pOrigFun)(pszName, iSize, pbuf);
	BEGIN_READ( pbuf, iSize );
	int cl = READ_BYTE();
	int frags = READ_SHORT();
	int deaths = READ_SHORT();
	int teamnumber = READ_SHORT();
	gStats.hook_scoreinfo(cl,frags,deaths,teamnumber);
	/*
	char buf[2048];
	sprintf(buf, "%s", READ_STRING());
	int points = READ_SHORT();
	gSuperHUD.hook_teamscore(buf, points);*/
	return retval;
}

//added by FB; FryGuy's nade timer stuff
static int hook_statusicon (const char *pszName, int iSize, void *pbuf) {
	int retval = 0;
	
	hook_entry_t * hook = hook_lookup(pszName);
	pfnUserMsgHook pOrigFun = hook ? hook->orig_func : 0;

	if ( pOrigFun )
		retval = (*pOrigFun)(pszName, iSize, pbuf);

	char buf[2048];
	BEGIN_READ( pbuf, iSize );
	int status = READ_BYTE();
	sprintf (buf, "%s", READ_STRING());

	gLogging.add_log("[statusicon] %s %d", buf, status);

	//add_log("statusicon: %s %d (%f)", buf, status, gEngfuncs.GetClientTime());
	
	gFB.hook_statusicon(buf, status);
	gTriggers.hook_statusicon(buf, status);

	return retval;
}

//added by FB as per equ1n0x's instructions who got it from fryguy... whew.
int hook_teamscore (const char *pszName, int iSize, void *pbuf)
{
	int retval = 0;
	hook_entry_t * hook = hook_lookup(pszName);
	pfnUserMsgHook pOrigFun = hook ? hook->orig_func : 0;
	if ( pOrigFun )
		retval = (*pOrigFun)(pszName, iSize, pbuf);
	BEGIN_READ( pbuf, iSize );
	char buf[2048];
	sprintf(buf, "%s", READ_STRING());
	int points = READ_SHORT();

	gFB.hook_teamscore(buf, points);
	gLaserStatus.hook_teamscore(buf, points);
	gFlagStatus.hook_teamscore(buf, points);

	gLogging.add_log("[teamscore] %s points: %d",buf, points);

	return retval;
}

//added by FB
int hook_hudtext (const char *pszName, int iSize, void *pbuf)
{
	hook_entry_t * hook = hook_lookup(pszName);
	pfnUserMsgHook pOrigFun = hook ? hook->orig_func : 0;
	int retval = 0;


	BEGIN_READ (pbuf, iSize);
        const char * text = READ_STRING() + 1;	// skip color byte
	
	dump_message (hook ? hook->msgdump : NULL, pszName, pbuf, iSize);
		
	// Run Pre hook, abort if necessary
	if(!gMainHook.Pre_Hook_HudText(text))
		return 0;

	if(pOrigFun)
		retval = (*pOrigFun)(pszName, iSize, pbuf);

	// Run post hook
	gMainHook.Post_Hook_HudText(text);

	return retval;
}

int hook_saytext (const char *pszName, int iSize, void *pbuf)
{
	hook_entry_t * hook = hook_lookup(pszName);
	pfnUserMsgHook pOrigFun = hook ? hook->orig_func : 0;
	int retval = 0;


	BEGIN_READ (pbuf, iSize);
    int entity = READ_BYTE();
    const char * text = READ_STRING() + 1;	// skip color byte

	// Run Pre hook, abort if necessary
	if(!gMainHook.Pre_Hook_SayText(text))
		return 0;

	if(pOrigFun)
		retval = (*pOrigFun)(pszName, iSize, pbuf);

	// Run post hook
	gMainHook.Post_Hook_SayText(text);

	return retval;
}


int hook_setfov (const char *pszName, int iSize, void *pbuf)
{
	static pfnUserMsgHook pOrigFun = NULL;
	int retval = 0;

	if ( !pOrigFun )
	{
		hook_entry_t * hook = hook_lookup("SetFOV");
		pOrigFun = hook ? hook->orig_func : 0;
	}

	if ( pOrigFun )
		retval = (*pOrigFun)(pszName, iSize, pbuf);

	return retval;
}


static int hook_resethud (const char *pszName, int iSize, void *pbuf)
{
	int retval = 0;
	hook_entry_t * hook = hook_lookup(pszName);
	pfnUserMsgHook pOrigFun = hook ? hook->orig_func : 0;
	
	// set spawn trigger
	gTriggers.bJustSpawned = 1;

	if ( pOrigFun )
		retval = (*pOrigFun)(pszName, iSize, pbuf);

	return retval;
}


static int hook_deathmsg (const char *pszName, int iSize, void *pbuf)
{
	int retval = 0;
	int killer, victim;//, headshot;
	const char *weapon;
	hook_entry_t * hook = hook_lookup(pszName);
	pfnUserMsgHook pOrigFun = hook ? hook->orig_func : 0;
	BEGIN_READ(pbuf, iSize);
	killer = READ_BYTE();
	victim = READ_BYTE();
	//headshot = READ_BYTE();
	weapon = READ_STRING();

	// Run Pre hook, abort if necessary
	if(!gMainHook.Pre_Hook_DeathMsg(killer, victim, weapon))
		return 0;

	if ( pOrigFun )
		retval = (*pOrigFun)(pszName, iSize, pbuf);

	// Run post hook
	gMainHook.Post_Hook_DeathMsg(killer, victim, weapon);

	return retval;
}


static int hook_textmsg (const char *pszName, int iSize, void *pbuf)
{
	hook_entry_t * hook = hook_lookup(pszName);
	pfnUserMsgHook pOrigFun = hook ? hook->orig_func : 0;

	BEGIN_READ (pbuf, iSize);
    int id = READ_BYTE();
	int retval = 0;
    const char * str1 = READ_STRING();

	// Run Pre hook, abort if necessary
	if(!gMainHook.Pre_Hook_TextMsg(str1))
		return 0;


	if ( pOrigFun )
		retval = (*pOrigFun)(pszName, iSize, pbuf);

	// Run post hook
	gMainHook.Post_Hook_TextMsg(str1);

	return retval;
}

static int hook_servername(const char *pszName, int iSize, void *pbuf)
{
	int retval = 0;
	hook_entry_t * hook = hook_lookup(pszName);
	pfnUserMsgHook pOrigFun = hook ? hook->orig_func : 0;

	BEGIN_READ(pbuf, iSize);
	const char *str1 = READ_STRING();

	sprintf(gLogging.ServerName, "%s", str1);
	gLogging.bChangedServer = 1;
	gTriggers.bNewServer = 1;

	if ( pOrigFun )
		retval = (*pOrigFun)(pszName, iSize, pbuf);
	return retval;
}


static hook_entry_t * hook_lookup (const char * name)
{
	for ( hook_entry_t * hook = hook_list; hook->name; hook++ )
		if ( !strcmp(hook->name, name) )
			return hook;

	return NULL;
}


pfnUserMsgHook GetHookOrigPtr (char *cmd_name)
{
	hook_entry_t * hook = hook_lookup (cmd_name);

	if ( hook )
		return hook->orig_func;

	return NULL;
}


pfnUserMsgHook CheckAddHook (char *cmd_name, pfnUserMsgHook function)
{
	hook_entry_t * hook = hook_lookup (cmd_name);

	if ( hook )
	{
		hook->orig_func = function;
		return hook->hooked_func;
	}

	return NULL;
}


static int univ_dump_hook (const char *pszName, int iSize, void *pbuf)
{
	int retval = 0;

	hook_entry_t * hook = hook_lookup(pszName);
	pfnUserMsgHook pOrigFun = hook ? hook->orig_func : 0;

	dump_message (hook ? hook->msgdump : NULL, pszName, pbuf, iSize);

	if ( pOrigFun )
		retval = (*pOrigFun)(pszName, iSize, pbuf);

	/* do something... */
	return retval;
}


static void dump_message (const char * dumpstr, const char * name, void * pbuf, int size)
{
	char buf[2048], tmpbuf[256];

    buf[0] = '\0';
	BEGIN_READ (pbuf, size);

	for ( ; *dumpstr; dumpstr++ )
	{
		switch (*dumpstr)
		{
		default:
			sprintf (tmpbuf, "wrong type: %c", *dumpstr);
			break;
			
		case 'C':
			sprintf (tmpbuf, "char %d", READ_CHAR());
			break;

		case 'B':
			sprintf (tmpbuf, "byte %d", READ_BYTE());
			break;

		case 'H':
			sprintf (tmpbuf, "short %i", READ_SHORT() );
			break;

		case 'L':
			sprintf (tmpbuf, "long %i", READ_LONG() );
			break;

		case 'F':
			sprintf (tmpbuf, "float %f", READ_FLOAT() );
			break;

		case '3':
			sprintf (tmpbuf, "coords %5.2f/%5.2f/%5.2f",
					READ_COORD(), READ_COORD(), READ_COORD() );
			break;

		case 'S':
			sprintf (tmpbuf, "string '%s'", READ_STRING());
			break;
        }

        sprintf(buf+strlen(buf), "%s ", tmpbuf);
	}
}