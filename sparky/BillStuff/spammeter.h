/*  ------------------------------------------------------------------------
	  spammeter.h by ruiner based on mstr's speed meter
		- added v1.41 - 04/09/03
	------------------------------------------------------------------------ */

#ifndef SPAMMETER_H
#define SPAMMETER_H

class CSpamMeter : CBaseHook
{
	public:

	virtual bool Pre_HUD_Init (void);
	virtual void Post_HUD_Init (void);
	virtual void Post_HUD_Redraw (float flTime, int intermission);
	virtual bool Pre_HUD_AddEntity (int type, struct cl_entity_s *ent, const char *modelname);

	virtual void Cmd_ToggleSpamMeter (void);
	virtual void Cmd_MoveSpamMeter (void);
	virtual void Cmd_SpamCount (void);

	virtual int GetPixLen(char *text);

	bool bSpamMeterGraphicOn;

	int iGraphicSpamMeterX;
	int iGraphicSpamMeterY;
	int iSpamCount;
	int iFlameCount;
	int iMaxSpam;
	int iLastSpam;

	float fLastTime;
};

extern CSpamMeter gSpamMeter;

#endif