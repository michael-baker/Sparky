/*  ------------------------------------------------------------------------
	  laserstatus.h by ruiner@konspiracy.org
		- added v1.33 - 01/12/03
	------------------------------------------------------------------------ */

#ifndef LASERSTATUS_H
#define LASERSTATUS_H

struct laserscore
{
	int		points;
	int		team;
};

class CLaserStatus : CBaseHook
{
	public:
		void	ResetLasers(void);
		void 	Post_Hook_hudText (const char * msg);
		bool	Pre_HUD_Init (void);
		void	Post_HUD_Redraw (float flTime, int intermission);
		void	Post_HUD_Frame (double time);
		bool	Pre_Hook_ConsoleCommand (char *pszString);
		void	Post_Hook_HudText (const char * msg);
		void	Cmd_ToggleLaserStatus (void);
		void	hook_teamscore (char *tbuf, int points);
	private:
		float	fLastTime;

		char	blueLaser[20];
		char	redLaser[20];
		laserscore	teamscores[3];
		int		tLaserTime;
		char	szLevelName[1024];

		bool	bLaserStatusOn;
};

extern CLaserStatus gLaserStatus;

#endif
