/*  ------------------------------------------------------------------
	  rulesmanager.h by FryGuy, FB, and ruiner
		- added v1.32
	------------------------------------------------------------------ */

#ifndef RULESMANAGER_H
#define RULESMANAGER_H

const float MIN_TIME_BETWEEN_RULES_UPDATES = 20;

class CRulesManager
{
public:
	CRulesManager();
	void Net_RulesUpdated (const char * rules_string);
	void UpdateRules();

	virtual bool Pre_HUD_Init (void);
	virtual bool Pre_HUD_Frame (double time);
	virtual void Post_HUD_Frame (double time);
	virtual bool Pre_Hook_ConsoleCommand (char *pszString);
	virtual void Post_Hook_TextMsg (const char *pszString);

	virtual void Cmd_Timeleft (void);
	bool bTimeleftOn;
	bool bTimeleftOK;

	float	cur_time;
	int		timeleft;
	int		iTimelimit;

	float	fLastTime;
	float	fLastTriggerTime;

	float	fNextRulesUpdate;
	int		iContextId;

	char	szLevelName[1024];

	bool	b15MinDone;
	bool	b5MinDone;
	bool	b2MinDone;
	bool	b1MinDone;
	bool	b30SecDone;
	bool	bTimeleftAnnounce;
};

extern CRulesManager gRulesManager;

#endif