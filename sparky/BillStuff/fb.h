/*  ------------------------------------------------------------------------
	  fb.h
		- soon to be extinct
	------------------------------------------------------------------------ */

#ifndef FB_H
#define FB_H

// begin: added by Mulchman
struct teamscore
{
	int		points;
	char	team;
};
// end: added by Mulchman

class CFB : CBaseHook
{
	public:

	virtual bool Pre_HUD_Init (void);
	virtual void Post_HUD_Frame (double time);
	virtual bool Pre_HUD_Redraw (float flTime, int intermission);
	virtual void Post_HUD_Redraw (float flTime, int intermission);
	virtual bool Pre_Hook_TextMsg (const char *pszString);
	virtual void Post_Hook_TextMsg (const char *pszString);
	virtual void Post_Hook_HudText (const char *pszString);
	virtual bool Pre_Hook_ConsoleCommand (char *pszString);
	virtual void Post_HUD_AddEntity (int type, struct cl_entity_s *ent, const char *modelname);

	virtual void hook_teamscore (char *tbuf, int points); //added
	virtual void hook_statusicon (char *buf, int status); //added

	CFB (void);

	virtual void Cmd_ToggleScoreDisplay (void);
	virtual void Cmd_ToggleNadeTimer (void);
	virtual void Cmd_Sumo (void);
	virtual void Cmd_ToggleShowCommands (void);

	bool bScoreDisplayOn;
	bool bScoreBlue;
	bool bScoreRed;
	bool bScoreYellow;
	bool bScoreGreen;
	bool bNadeTimerOn;
	bool bNadeTextOn;
	bool bNadeDebugOn;
	bool bNadeCustomXOn;
	bool bNadeCustomYOn;
	bool bNadeCustomHOn;
	bool bNadeCustomWOn;
	bool bNadeGren1;
	bool primed;
	bool bSumo; //bsumo!
	bool bShowCommands;

	// begin: added by mulchman
	void Draw_Blue_Scores(int slot, char *buf);
	void Draw_Red_Scores(int slot, char *buf);
	void Draw_Yellow_Scores(int slot, char *buf);
	void Draw_Green_Scores(int slot, char *buf);

	int FindTeamIndex(const char c);
	// end: added by mulchman

	float fLastTime;
	float fNadeTimer;

	int iySlot1;
	int iySlot2;
	int iySlot3;
	int iySlot4;

	int iNadeCustomX;
	int iNadeCustomY;
	int iNadeCustomH;
	int iNadeCustomW;

	// beg: added by Mulchman
	char redname[20];
	char bluename[20];
	char yellowname[20];
	char greenname[20];
	teamscore teamscores[4];
	// end: added by Mulchman
};

extern CFB gFB;

#endif