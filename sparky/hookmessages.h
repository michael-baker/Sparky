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
 * $Id: hookmessages.h,v 1.2 2001/10/04 10:31:24 vasily Exp $
 */

#ifndef HOOKUSERMSG_H
#define HOOKUSERMSG_H


/*
 * typedef int (*pfnUserMsgHook)(const char *pszName, int iSize, void *pbuf);
 * defined in cl_dll.h:
 */

typedef int (*pfnUserMsgHook)(const char *pszName, int iSize, void *pbuf);

pfnUserMsgHook CheckAddHook (char *cmd_name, pfnUserMsgHook function);
pfnUserMsgHook GetHookOrigPtr (char *cmd_name);

/* hooks */
int hook_deathmsg 	(const char *pszName, int iSize, void *pbuf);
int hook_resethud 	(const char *pszName, int iSize, void *pbuf);
int hook_textmsg 	(const char *pszName, int iSize, void *pbuf);
int hook_setfov (const char *pszName, int iSize, void *pbuf);
int hook_saytext(const char *pszName, int iSize, void *pbuf);
int hook_servername(const char *pszName, int iSize, void *pbuf);
int hook_hudtext (const char *pszName, int iSize, void *pbuf); //added
int hook_teamscore (const char *pszName, int iSize, void *pbuf); //added
int hook_statusicon (const char *pszName, int iSize, void *pbuf); //added
int hook_scoreinfo (const char *pszName, int iSize, void *pbuf); //added by loki
int hook_ammox (const char *pszName, int iSize, void *pbuf); //added by Mulchman
int hook_feign (const char *pszName, int iSize, void *pbuf); //added by Mulchman
int hook_detpack (const char *pszName, int iSize, void *pbuf); //added by Mulchman

#endif
