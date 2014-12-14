/*  ------------------------------------------------------------------------
	  triggers.h by billdoor
		- added v1.0
	------------------------------------------------------------------------ */

#ifndef TRIGGERS_H
#define TRIGGERS_H

class CTriggers : CBaseHook
{
	public:

	virtual bool Pre_HUD_Init (void);
	virtual void Post_HUD_Init (void);
	virtual bool Pre_HUD_Frame (double time);
	virtual void Post_HUD_Frame (double time);
	virtual bool Pre_Hook_TextMsg (const char *pszString);
	virtual void Post_Hook_TextMsg (const char *pszString);
	virtual bool Pre_Hook_SayText (const char *pszString);
	virtual void Post_Hook_SayText (const char *pszString);
	virtual bool Pre_Hook_DeathMsg (int killer, int victim, const char *pszWeapon);
	virtual void Post_Hook_DeathMsg (int killer, int victim, const char *pszWeapon);
	virtual void Post_Hook_HudText (const char *pszString);

	virtual void hook_statusicon (char *buf, int status); //added

	virtual bool Pre_Hook_ConsoleCommand (char *pszString); //mulch, v1.38
	
	virtual void Post_Hook_Feign (int iIsFeigning);			// mulch v1.43
	virtual void Post_Hook_Detpack (int iIsSettingDetpack);	// mulch v1.43

	CTriggers (void);

	bool bEndOfRound;
	bool bJustSpawned;
	bool bJustDied;
	bool bJustKilled;
	bool bNewMap;
	bool bNewServer;
	bool bEnemyCap;
	bool bTeamCap;
	bool bTeamCapFix;
	bool bYouDropFlag;
	bool bTimeleft;
	float fLastTime;

	//mulch addition, v1.38
	char victclass[20]; 
	char killclass[20];
	char victname[256];
	char killname[256];
	bool bJustKilledSG; 
	bool bJustKilledFlagCarrier; 
	bool bJustKilledFlagCarrierMaybe; 
	bool bEnemyLostFlag; 
	bool bJustSuicided;
	bool bYouGotFlag;
	bool bJustDiedSG;
	//end mulch addition, v1.38

	//mulch addition 1.43
	bool bFeigning;
	bool bSettingDetpack;

	// beg: Fr33 addition 1.42
	bool bHumiliate;
	// end: fr33 addition, 1.42
};

extern CTriggers gTriggers;

#endif