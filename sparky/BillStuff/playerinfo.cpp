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
#include "playerinfo.h"

/*Global*/
CPlayerinfo gPlayerinfo;

bool CPlayerinfo::Pre_HUD_PlayerMove (struct playermove_s *ppmove, int server)
{
	/*
	// Using memcpy to cache the whole structure every frame cut FPS in half :(
	// So this just copies the entire structure once and then updates individual elements as needed
	if(!bCached)
	{
		memcpy(&ppmove_cache, ppmove, sizeof(playermove_t));
		bCached = 1;
	}
	else
	{
	int playerclass = gEngfuncs.GetLocalPlayer()->curstate.playerclass;
	ppmove_cache.velocity = ppmove->velocity;
	ppmove_cache.flags = ppmove->flags;
	ppmove_cache.waterlevel = ppmove->waterlevel;
	ppmove_cache.movetype = ppmove->movetype;
	ppmove_cache.maxspeed = ppmove->maxspeed;
	ppmove_cache.origin = ppmove->origin;
	}
	*/
	return true;
}

void CPlayerinfo::Post_HUD_PlayerMove (struct playermove_s *ppmove, int server)
{
	// Using memcpy to cache the whole structure every frame cut FPS in half :(
	// So this just copies the entire structure once and then updates individual elements as needed
	if(!bCached)
	{
		memcpy(&ppmove_cache, ppmove, sizeof(playermove_t));
		bCached = 1;
	}
	else
	{
		ppmove_cache.velocity = ppmove->velocity;
		ppmove_cache.flags = ppmove->flags;
		ppmove_cache.waterlevel = ppmove->waterlevel;
		ppmove_cache.movetype = ppmove->movetype;
		ppmove_cache.maxspeed = ppmove->maxspeed;
		ppmove_cache.origin = ppmove->origin;
	}
}


CPlayerinfo::CPlayerinfo(void)
{
	bCached = 0;
}