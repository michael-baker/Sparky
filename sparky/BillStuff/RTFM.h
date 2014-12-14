/*  ------------------------------------------------------------------------
	  RTFM.h
		- by Mulchman
	------------------------------------------------------------------------ */

#ifndef RTFM_H
#define RTFM_H

#include <string>
using namespace std;

class CRTFM : CBaseHook
{
	public:

	virtual bool Pre_HUD_Init (void);
	virtual bool Pre_HUD_Redraw (float flTime, int intermission);
	virtual void Post_HUD_Redraw (float flTime, int intermission);

	CRTFM (void);

	virtual void Cmd_ToggleRTFM (void);

	bool bReadManual;
	bool bSentPage;
	string strMessage;
};

extern CRTFM gRTFM;

#endif