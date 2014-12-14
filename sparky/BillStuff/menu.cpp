/*        _______________   
         |  ___________  |
		 | |   ________| | ____________menu.cpp_______________
         | |  |______    |/ ________   ________   ___     ___ \
         | |   ______|   ||  ____  | |  ____  | |___|___|___| |
         | |  |________  || |    | | | |____| |  ___|___|___  |
         | |___________| ||_|    |_| |________| |___|   |___| |
         |_______________|\___________________________________/  */

/*
	User-Configurable, Multi-Menu System.
	*Basic Menu Code + Draw code - equ1n0x
	*File Parser + Revamped Menu Code - 8BALL
*/
               
#include "..\client.h"

#include "hook.h"
#include "superhud.h"
#include "Menu.h"
#include "util.h"
#include <vector>
#include <string>
#include "help.h"

#pragma warning (disable:4244) // float to int loss of data

struct menuEntry 
{
	char title[128];
	char command[128];
	int r;
	int g;
	int b;
};

struct menu_t
{
	char title[128];
	menuEntry entrys[10];
	char layout[25];
	int title_r;
	int title_g;
	int title_b;
	int bg_r, bg_g, bg_b, bg_a;
	int w;

};

menu_t MenuData;

/*Global*/
CSparkyMenu gSparkyMenu;

DECLARE_COMMAND_2(gSparkyMenu, SparkyMenu);
DECLARE_COMMAND_2(gSparkyMenu, CloseMenu);

bool CSparkyMenu::Pre_HUD_Init (void)
{
	//Hook Commands Here
	gEngfuncs.pfnRegisterVariable("sparky_menu_style", "1", FCVAR_ARCHIVE); //menu style, 1 = borders + background, 2 = nothing
	gEngfuncs.pfnRegisterVariable("sparky_menu_autoclose", "1", FCVAR_ARCHIVE); //option to close menu after each command
	gEngfuncs.pfnRegisterVariable("sparky_menu_x", "200", FCVAR_ARCHIVE); //x coord
	gEngfuncs.pfnRegisterVariable("sparky_menu_y", "200", FCVAR_ARCHIVE); //y coord
	HOOK_COMMAND_2("bmenu", SparkyMenu);
	gHelp.addCommand("bmenu", "Displays a specified menu.\nUsage: bmenu <menu name>\nSee Also:\nsparky_menu_style\nsparky_menu_autoclose\nsparkymenu_x\nsparky_menu_y");
	gHelp.addCommand("sparky_menu_style", "Menu Style Cvar\nUsage: sparky_menu_style <value>\n1 - borders and background\n2 - nothing");
	gHelp.addCommand("sparky_menu_autoclose", "Menu Cvar\nOption to close menu after each command\nUsage: sparky_menu_autoclose 0/1");
	gHelp.addCommand("sparky_menu_x", "Menu \"x coordinate\" Cvar\nUsage: sparky_menu_x <value>\nDefault: 200");
	gHelp.addCommand("sparky_menu_y", "Menu \"y coordinate\" Cvar\nUsage: sparky_menu_y <value>\nDefault: 200");
	HOOK_COMMAND_2("bmenuclose", CloseMenu);
	gHelp.addCommand("bmenuclose", "Closes active menus.\nUsage: bmenuclose");
	return true;
}

int CSparkyMenu::DrawLen(char *fmt)
{
	int len=0;
	for ( char * p = fmt; *p; p++ ) len += gSuperHUD.m_scrinfo.charWidths[*p]; 
	return len;
}


void CSparkyMenu::Cmd_SparkyMenu(void)
{
	char buffer[128];


	if(menu_active)
	{
		menu_active = false;
		return;
	}
	else if(gEngfuncs.Cmd_Argc() < 2)
	{
		//gEngfuncs.pfnConsolePrint("[sparky] No menu file specified, using default: 'main'\n");
		Parse("main");
	}
	else
	{
		sprintf(buffer, "%s", gEngfuncs.Cmd_Argv(1));
		Parse(buffer);
	}
}
void CSparkyMenu::Cmd_CloseMenu(void) { menu_active = false; }

void CSparkyMenu::Parse(char *menuName)
{
	char buff[4096];
	char *line;
	int  w;
	int  entryNum = 0;
	int  layoutNum = 0;
	int  r = 255;
	int  g = 255;
	int  b = 255;
	int  temp=0;
	int  temp2=0;
	bool quoteOpen;
	bool badformat = false;
	FILE *fp;


	
	MenuData.bg_a = 100;
	MenuData.bg_r = 70;
	MenuData.bg_g = 70;
	MenuData.bg_b = 70;
	MenuData.layout[0] = '\0';

	sprintf(buff,"sparkyutils\\menus\\%s.mnu",menuName);

	if(fp=fopen(buff, "rt") )
	{

		bool gottitle = false;
		MenuData.title[0] = '\0';
		while(!gottitle)
		{
			if(line = fgets(buff,1024,fp))
			{
				sprintf(buff, "%s",line);
				if(!strncmp(buff,"#rgb",4))
				{
					temp = 0;
					while(!(strchr("1234567890",buff[temp])) )
						temp++;
					r = atoi(buff+temp);
					temp += 2;
					if(r>9)
						temp++;
					if(r>99)
						temp++;
					g = atoi(buff+temp);
					temp += 2;
					if(g>9)
						temp++;
					if(g>99)
						temp++;
					b = atoi(buff+temp);
				}
				else if(!strncmp(buff,"#bgcolor",8))
				{

					temp = 0;
					while(!(strchr("1234567890",buff[temp])) )
						temp++;
					MenuData.bg_r = atoi(buff+temp);
					temp += 2;
					if(MenuData.bg_r>9)
						temp++;
					if(MenuData.bg_r>99)
						temp++;
					MenuData.bg_g = atoi(buff+temp);
					temp += 2;
					if(MenuData.bg_g>9)
						temp++;
					if(MenuData.bg_g>99)
						temp++;
					MenuData.bg_b = atoi(buff+temp);
					temp += 2;
					if(MenuData.bg_b>9)
						temp++;
					if(MenuData.bg_b>99)
						temp++;
					MenuData.bg_a = atoi(buff+temp);
					
				}
				else if(strncmp(buff,"//",2) && strcmp(buff,"\n") && strcmp(buff," "))
				{
					strcpy(MenuData.title,buff);


					temp = strlen(MenuData.title);
					MenuData.title[temp-1] = '\0';
					
					MenuData.w = DrawLen(MenuData.title);

					MenuData.title_r = r;
					MenuData.title_g = g;
					MenuData.title_b = b;
					gottitle = true;
				}

			}
			else
			{
				gottitle = true;
				badformat = true;
			}
		}


		if(MenuData.title[0] == '\0')
			badformat = true;


		while((line = fgets(buff, 1024, fp)) && entryNum<10 && !badformat)
		{
			sprintf(buff,"%s",line);
			if(buff[0] == '#')
			{
				
				if(!strncmp(buff,"#rgb",4))
				{

					temp = 0;
					while(!(strchr("1234567890",buff[temp])) )
						temp++;
					r = atoi(buff+temp);
					temp += 2;
					if(r>9)
						temp++;
					if(r>99)
						temp++;
					g = atoi(buff+temp);
					temp += 2;
					if(g>9)
						temp++;
					if(g>99)
						temp++;
					b = atoi(buff+temp);
				}
				else if(!strncmp(buff,"#bgcolor",8))
				{
					temp = 0;
					while(!(strchr("1234567890",buff[temp])) )
						temp++;
					MenuData.bg_r = atoi(buff+temp);
					temp += 2;
					if(MenuData.bg_r>9)
						temp++;
					if(MenuData.bg_r>99)
						temp++;
					MenuData.bg_g = atoi(buff+temp);
					temp += 2;
					if(MenuData.bg_g>9)
						temp++;
					if(MenuData.bg_g>99)
						temp++;
					MenuData.bg_b = atoi(buff+temp);
					temp += 2;
					if(MenuData.bg_b>9)
						temp++;
					if(MenuData.bg_b>99)
						temp++;
					MenuData.bg_a = atoi(buff+temp);
				}
				else if(!strncmp(buff,"#space",6) )
				{
					MenuData.layout[layoutNum] = 's';
					layoutNum++;
					MenuData.layout[layoutNum] = '\0';
				}
				else if(!strncmp(buff,"#line",5) )
				{
					MenuData.layout[layoutNum] = 'l';
					layoutNum++;
					MenuData.layout[layoutNum] = '\0';
				}
			}//buff[0] == #
			if(buff[0] == '\"')
			{
				
				quoteOpen = true;
				for(temp = 0; temp<127 && quoteOpen; temp++)
				{
					if(buff[temp+1] == '\0')
					{
						quoteOpen = false;
						badformat = true;
					}
					else if(buff[temp+1] == '\"')
						quoteOpen = false;
					else
					{
						MenuData.entrys[entryNum].title[temp] = buff[temp+1];
						MenuData.entrys[entryNum].title[temp+1] = '\0';
					}
				}

				
				w = DrawLen(MenuData.entrys[entryNum].title);

				if(w>MenuData.w)
					MenuData.w = w;

				temp++;

				while(buff[temp] != '\"' && buff[temp] != '\0' && !badformat)
					temp++;
				
				
				if(buff[temp] == '\0' || badformat)
					badformat = true;
				else
				{
					temp++;
					temp2 = 0;
					quoteOpen = true;
					while(quoteOpen && temp2 < 127)
					{
						if(buff[temp] == '\0')
						{
							quoteOpen = false;
							badformat = true;
						}
						else if(buff[temp] == '\"')
						{
							quoteOpen = false;
						}
						else
						{
							MenuData.entrys[entryNum].command[temp2] = buff[temp];
							temp++;
							temp2++;
							MenuData.entrys[entryNum].command[temp2] = '\0';
						}
					}
					

					temp2++;
					MenuData.layout[layoutNum] = ' ';
					layoutNum++;
					MenuData.layout[layoutNum] = '\0';
					MenuData.entrys[entryNum].r = r;
					MenuData.entrys[entryNum].g = g;
					MenuData.entrys[entryNum].b = b;
				}
				entryNum++;
			}//buff[0] = '\"'
		}//while
		


		while(entryNum < 10)
		{
			sprintf(MenuData.entrys[entryNum].command, "bmenu %s", menuName);
			MenuData.entrys[entryNum].title[0]='\0';
			entryNum++;
		}

		fclose(fp);
	}//if
	else
	{
		gEngfuncs.pfnConsolePrint("[Sparky] ERROR: Menu file could not be opened!\n");
		return;
	}
	if(badformat)
		gEngfuncs.pfnConsolePrint("[Sparky] ERROR: Bad menu file format!\n");
	else
		menu_active = true;
}

void CSparkyMenu::Post_HUD_Redraw (float flTime, int intermission)
{
	/* Drawing menu - Title near bottom */
	if (menu_active)
	{
		int x = gEngfuncs.pfnGetCvarFloat("sparky_menu_x") + 10;
		int y = gEngfuncs.pfnGetCvarFloat("sparky_menu_y");
		int i = 0;
		int layoutNum = 0;
		int entryNum = 0;
		int w = MenuData.w;
		char buf[264];

		/* Items */
		while(MenuData.layout[layoutNum] != '\0')
		{
			if(MenuData.layout[layoutNum] == 's') y += 18;	// #space

			else if(MenuData.layout[layoutNum] == 'l')		// #line
			{
				y += 18;
				gEngfuncs.pfnFillRGBA(x, y+15, w, 1, MenuData.title_r, MenuData.title_g, MenuData.title_b, 255);
			}
			else
			{
				y += 18;
				if((entryNum) == 9)		sprintf(buf, "0. %s", MenuData.entrys[entryNum].title); //change to draw '0' instead of'10'
				else					sprintf(buf, "%d. %s", entryNum+1, MenuData.entrys[entryNum].title);
				
				gSuperHUD.DrawHudString(x, y, MenuData.entrys[entryNum].r, MenuData.entrys[entryNum].g, MenuData.entrys[entryNum].b, buf);
				entryNum++;
			}

			layoutNum++;
		}

		int x2 = gEngfuncs.pfnGetCvarFloat("sparky_menu_x");
		int x3 = MenuData.w+40;
		int y2 = gEngfuncs.pfnGetCvarFloat("sparky_menu_y");
		int y3 = (strlen(MenuData.layout)*18+18);
		int tx = x2+((MenuData.w-DrawLen(MenuData.title)+40)/2);

		/* Draw Background + Borders */
		if (gEngfuncs.pfnGetCvarFloat("sparky_menu_style") == 1)
		{
			//					  x			y			w	 h		r					g					b					a
			gEngfuncs.pfnFillRGBA(x2,		y2,			1,	 y3+14,	MenuData.title_r,	MenuData.title_g,	MenuData.title_b,	255); //left
			gEngfuncs.pfnFillRGBA(x2+x3,	y2,			1,	 y3+14,	MenuData.title_r,	MenuData.title_g,	MenuData.title_b,	255); //right
			gEngfuncs.pfnFillRGBA(x2,		y2,			x3,  1,		MenuData.title_r,	MenuData.title_g,	MenuData.title_b,	255); //top
			gEngfuncs.pfnFillRGBA(x2,		y2+14,		x3,  1,		MenuData.title_r,	MenuData.title_g,	MenuData.title_b,	255); //middle
			gEngfuncs.pfnFillRGBA(x2,		y2+y3+14,	x3+1,1,		MenuData.title_r,	MenuData.title_g,	MenuData.title_b,	255); //bottom
			gEngfuncs.pfnFillRGBA(x2,		y2,			x3,  y3+14,	MenuData.bg_r,		MenuData.bg_g,		MenuData.bg_g,		MenuData.bg_a); //background
			/* Draw Title */
			gSuperHUD.DrawHudString(tx, y2, MenuData.title_r, MenuData.title_g, MenuData.title_b, MenuData.title);
		}
		else
		{
			/* Draw Title */
			gSuperHUD.DrawHudString(x2, y2, MenuData.title_r, MenuData.title_g, MenuData.title_b, MenuData.title);
			gEngfuncs.pfnFillRGBA(x2, y2+15, DrawLen(MenuData.title), 1, MenuData.title_r, MenuData.title_g, MenuData.title_b, 255);
		}
	}
}

bool CSparkyMenu::Pre_HUD_Key_Event (int eventcode, int keynum, const char *pszCurrentBinding)
{

	//keynums 1=49; 2=50; ect....
	int keynum2 = keynum - 49;
	
	if(keynum2 == -1)
		keynum2 = 9;

	if (menu_active && eventcode == 1)
	{
		if(keynum2 >= 0 && keynum2 <= 9)
		{
			if (gEngfuncs.pfnGetCvarFloat("sparky_menu_autoclose") == 1) menu_active = false;
			gEngfuncs.pfnClientCmd(MenuData.entrys[keynum2].command);
			return false;
		}
		else if(keynum == 27)
		{
			menu_active = false;
			return false;
		}
	}
	
	return true;
}

CSparkyMenu::CSparkyMenu(void)
{
	menu_active = false;
}
