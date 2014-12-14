/*  ------------------------------------------------------------------------
	  speedos.h by fb, billdoor, loki, and mstr
	    - updated v1.37 - 02/02/03 - added loki's position stuff
		- added v1.33 - 01/12/03
	------------------------------------------------------------------------ */

#ifndef SPEEDOS_H
#define SPEEDOS_H

class CSpeedos : CBaseHook
{
	public:

	virtual bool Pre_HUD_Init (void);
	virtual void Post_HUD_Init (void);
	virtual void Post_HUD_Redraw (float flTime, int intermission);

	CSpeedos (void);

	virtual void Cmd_ToggleSpeedo (void);
	virtual void Cmd_MoveSpeedo (void);

	virtual int GetPixLen(char *text);

	bool bSpeedoGraphicOn;
	bool bSpeedoTextOn;
	bool bSpeedoDialOn;

	int iDialSpeedoX;
	int iDialSpeedoY;
	int iGraphicSpeedoX;
	int iGraphicSpeedoY;
	int iTextSpeedoX;
	int iTextSpeedoY;

	float fLastTime;
};

extern CSpeedos gSpeedos;

#endif