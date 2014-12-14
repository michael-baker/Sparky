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
 * $Id: util.h,v 1.3 2001/10/04 10:31:24 vasily Exp $
 */

#ifndef UTIL_H
#define UTIL_H

const char * dump (void * buf, int size);
HSPRITE		LoadSprite(const char *pszName);
bool 		str_prefix (const char * substr, const char * str);
const char * get_argument (const char *string, char *buf, size_t bufsize);

bool CalcScreen(float *origin,float *vecScreen);

char *NameFromModel(const char *szModel);
char *NameFromFileName(const char *szFileName);
char *NameFromClass(int playerclass);
Vector ColorFromTeam(int team);
void RemoveIllegalsFromFileName(char *szFileName);
void RemoveIllegalsFromDemoName(char *szFileName); // Added by Zoltarc
void PathFromFileName(char szFileName);
char *StringSearchReplace(const char *Search, const char *Replace, const char *Input);

#endif
