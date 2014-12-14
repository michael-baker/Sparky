/*  ------------------------------------------------------------------------
	  RTFM.cpp
		- by Mulchman
	------------------------------------------------------------------------ */

#include "..\client.h"

#include "hook.h"
#include "superhud.h"
#include <string>
#include "help.h"

using namespace std;

#include "RTFM.h"

/*Global*/
CRTFM gRTFM;

DECLARE_COMMAND_2(gRTFM, ToggleRTFM);

bool CRTFM::Pre_HUD_Init (void)
{
	HOOK_COMMAND_2("breadmanual", ToggleRTFM);
	gHelp.addCommand("breadmanual", "Removes the \"I need to read the manual to get rid of this\" spam all over your screen.\nUsage: breadmanual");
	bSentPage = false;
	return true;
}

bool CRTFM::Pre_HUD_Redraw (float flTime, int intermission)
{
	return true;
}

void CRTFM::Post_HUD_Redraw (float flTime, int intermission)
{
	if (!bReadManual)
	{
		if(!bSentPage) {
			bSentPage = true;
			ShellExecute(NULL, NULL, "http://sparky.konspiracy.org/docs.php", NULL, NULL, SW_SHOW);
		}
		for ( int i = 0; i < gSuperHUD.m_scrinfo.iHeight; i += 12 )
		{
			int r = rand( ) % 255;
			int g = rand( ) % 255;
			int b = rand( ) % 255;

			gSuperHUD.DrawHudString( 0, i, r, g, b, "%s", strMessage.c_str( ) );
		}
	}
}

CRTFM::CRTFM (void)
{
	string strTemp = "Read the manual to get rid of this! http://sparky.konspiracy.org/docs.php ";

	// should be enought to cover the newbs screen
	// and it's one hell of a frame killer too!!
	strMessage += strTemp;
	strMessage += strTemp;
	strMessage += strTemp;
	strMessage += strTemp;
	strMessage += strTemp;
	strMessage += strTemp;
	strMessage += strTemp;
	strMessage += strTemp;

	bReadManual = 0;
	bSentPage = 0;
}

void CRTFM::Cmd_ToggleRTFM (void)
{
	bReadManual = bReadManual ? 0 : 1;

	if (bReadManual) {
		gEngfuncs.pfnConsolePrint("[sparky] User has read the manual\n");
		bSentPage = true;
	}
}