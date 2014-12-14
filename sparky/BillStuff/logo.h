/*  ------------------------------------------------------------------------
	  logo.h by ruiner
		- added v1.42 - 04/11/03
	------------------------------------------------------------------------ */

#ifndef LOGO_H
#define LOGO_H

class CLogo : CBaseHook
{
	public:

	virtual bool Pre_HUD_Init (void);
	virtual void Cmd_ToggleLogo (void);
	virtual void Post_HUD_Redraw (float flTime, int intermission);

	bool	bDisplayLogo;
	bool	bDisplayScroll;
	char	strVersion[20];
	float	fScrollerX;
	float	fFade;
	float	fRot;
	float	fChange;
};

extern CLogo gLogo;

#endif
