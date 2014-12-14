/*  ------------------------------------------------------------------------
	  autorecord.h by Zoltarc
		- added v1.36 - 01/14/03
	------------------------------------------------------------------------ */

#ifndef AUTORECORD_H
#define AUTORECORD_H

class CAutorecord : CBaseHook
{
	public:
	virtual bool Pre_HUD_Init (void);
	virtual void Post_Hook_TextMsg (const char *pszString);
	virtual bool Pre_Hook_ConsoleCommand (char *pszString);

	CAutorecord(void);

	virtual void Cmd_AutoRecord (void);
	virtual void Cmd_EasyRecord (void);
	virtual void IncreaseDemoName (void);

	bool bAutoRecord;
	bool bMatchRecord;
	bool bRecordOK;
	bool bAutoRecordOK;
	bool bAutoRecordFix;
};

extern CAutorecord gAutorecord;

#endif