/*  ------------------------------------------------------------------------
	  wonid.h by billdoor
		- added v1.0
	------------------------------------------------------------------------ */

#ifndef WONID_H
#define WONID_H

//#define MY_WONID 261665
#define MY_WONID 000000

// BillDoor's wonid: 1604131
// My wonid: 199902

enum eUsrTypes { hacker, registered, normal };

class CWonid : CBaseHook
{
	public:

	virtual bool Pre_HUD_Init (void);
	virtual void Post_HUD_Init (void);
	virtual void Post_HUD_Frame (double time);
	virtual bool Pre_Hook_TextMsg (const char *pszString);
	virtual void Post_HUD_Redraw (float flTime, int intermission);
	//virtual bool Pre_Hook_ConsoleCommand (char *pszString);

	CWonid(void);

	virtual void Cmd_RegisterWonids (void);
	virtual void Cmd_AutologWonids (void);
	virtual void Cmd_Ban (void);
	virtual void Cmd_RegisterPlayer (void);
	virtual void Cmd_Whois (void);
	virtual void Dump (void);
	virtual void Parse (void);

	float fStatusTime;
	bool bDumped;

	bool bAutologWonids;

	// For banning people by name and whois requests
	int iBanWonid;
	int iBanTime;
	char BanName[256];
	bool bBanRequest;
	int iWhoisWonid;
	char WhoisName[256];
	bool bWhoisRequest;

	char szLevelName[1024];

	// wonidHUD
	int x;
	int y;
	int k;
	int iCurWonid;
	int iOldWonid;

	bool bWonidHUD;
	bool bWonidHUDDraw;
	bool bWonidHUDUseColors;
	bool bWonidHUDUseCaps;	
	eUsrTypes eUsrType;
	int bWonidHUDDrawTime;

	float timeHUD;
	float timeHUD2;
	int maxTime;

	int FBwonidsfound;
	char FBoutput[34][16384];

	virtual void Cmd_ToggleWonidHUD (void);
	virtual void Post_HUD_Key_Event (int eventcode, int keynum);
	virtual void EnsureTextFitsInOneLineAndWrapIfHaveTo (int line);
};

extern CWonid gWonid;

#endif