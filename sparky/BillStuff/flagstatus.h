/*  ------------------------------------------------------------------------
	  flagstatus.h by ruiner@konspiracy.org
		- added v1.33 - 01/12/03
	------------------------------------------------------------------------ */

#ifndef FLAGSTATUS_H
#define FLAGSTATUS_H

struct flagscore
{
	int		points;
	int		team;
};

class CFlagStatus : CBaseHook
{
	public:
		CFlagStatus(void);

		bool		Pre_HUD_Init (void);
		void		Post_HUD_Redraw (float flTime, int intermission);
		void		Post_HUD_Frame (double time);
		bool		Pre_Hook_ConsoleCommand (char *pszString);
		void		hook_teamscore (char *tbuf, int points);
		void		Post_Hook_HudText (const char *pszString);
		void		Cmd_ToggleFlagStatus (void);

		float		fLastTime;
		bool		bFlagStatusOn;
		char		blueFlagStatus[20];
		char		redFlagStatus[20];
		flagscore	teamscores[3];
};

extern CFlagStatus gFlagStatus;

#endif
