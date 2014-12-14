/*        _______________   
         |  ___________  |
		 | |   ________| | _____________menu.h________________
         | |  |______    |/ ________   ________   ___     ___ \
         | |   ______|   ||  ____  | |  ____  | |___|___|___| |
         | |  |________  || |    | | | |____| |  ___|___|___  |
         | |___________| ||_|    |_| |________| |___|   |___| |
         |_______________|\___________________________________/  */

#ifndef INCLUDE_MENU_H
#define INCLUDE_MENU_H
class CSparkyMenu : CBaseHook
{
	public:
	virtual bool Pre_HUD_Init (void);
	virtual void Post_HUD_Redraw (float flTime, int intermission);
	virtual bool Pre_HUD_Key_Event (int eventcode, int keynum, const char *pszCurrentBinding);
	virtual int  DrawLen(char *fmt);
	virtual void Cmd_SparkyMenu(void);
	virtual void Cmd_CloseMenu(void);
	virtual void Parse(char *menuName);

	CSparkyMenu (void);
	bool menu_active;
};

extern CSparkyMenu gSparkyMenu;
#endif