/*  ------------------------------------------------------------------------
	  logo.cpp by ruiner
		- added v1.42 - 04/09/03
	------------------------------------------------------------------------ */

#include "..\client.h"
#include "hook.h"
#include "superhud.h"
#include "logo.h"
#include "help.h"
#include <math.h>
#include <gl\gl.h>

#pragma comment (lib, "opengl32.lib") 
#pragma warning (disable:4244) // float to int loss of data
#pragma warning (disable:4018) // float to int loss of data

/*Global*/
CLogo gLogo;

DECLARE_COMMAND_2(gLogo, ToggleLogo);

bool CLogo::Pre_HUD_Init (void)
{
	HOOK_COMMAND_2("blogo", ToggleLogo);
	gHelp.addCommand("blogo", "Toggles display of ingame sparky logo.\nUsage: blogo");
	bDisplayLogo = true;
	bDisplayScroll = true;
	fScrollerX = gSuperHUD.m_scrinfo.iWidth + 50;
	fFade = 0.4f;
	fChange = 0.2f;

	return true;
}

void CLogo::Cmd_ToggleLogo (void)
{
	bDisplayLogo = bDisplayLogo ? 0 : 1;

	if(bDisplayLogo) {
		fFade = 0.0f;
		fScrollerX = gSuperHUD.m_scrinfo.iWidth;
		gSuperHUD.WriteHUDChannel(HC_IN_GAME_COLOR, 2, "logo ON");
	}
	else
		gSuperHUD.WriteHUDChannel(HC_IN_GAME_COLOR, 2, "logo OFF");
}

void glDrawRectangle(float x1, float y1, float x2, float y2, float r, float g, float b, float a)
{
		glColor4f(r,g,b,a);
		glBegin(GL_QUADS); 
		glVertex2f(x1, y1);
		glVertex2f(x2, y1);
		glVertex2f(x2, y2);
		glVertex2f(x1, y2);
		glEnd();
}

void glDrawRectangle3d(float x1, float y1, float x2, float y2, float z, float r, float g, float b, float a)
{
		float color[] = { r,g,b,a };
		GLfloat no_mat[] = { r / 3, g / 3, b / 3,1.0F };
		GLfloat mat_diffuse[] = { r,g,b,1.0F };
		GLfloat mat_specular[] = { 1.0F,1.0F,1.0F,1.0F };
		GLfloat high_shininess[] = { 100.0F };

		glMaterialfv(GL_FRONT,GL_AMBIENT,no_mat);
		glMaterialfv(GL_FRONT,GL_DIFFUSE,mat_diffuse);
		glMaterialfv(GL_FRONT,GL_SPECULAR,mat_specular);
		glMaterialfv(GL_FRONT,GL_SHININESS,high_shininess);
		glMaterialfv(GL_FRONT,GL_EMISSION,no_mat);
		glBegin(GL_QUADS); 

		glVertex3f(x1, y1, 0);
		glVertex3f(x2, y1, 0);
		glVertex3f(x2, y2, 0);
		glVertex3f(x1, y2, 0);

		glVertex3f(x1, y1, z);
		glVertex3f(x2, y1, z);
		glVertex3f(x2, y2, z);
		glVertex3f(x1, y2, z);

		glVertex3f(x1, y1, z);
		glVertex3f(x1, y1, 0);
		glVertex3f(x1, y2, 0);
		glVertex3f(x1, y2, z);

		glVertex3f(x2, y1, 0);
		glVertex3f(x2, y1, z);
		glVertex3f(x2, y2, z);
		glVertex3f(x2, y2, 0);

		glVertex3f(x1, y1, z);
		glVertex3f(x2, y1, z);
		glVertex3f(x2, y1, 0);
		glVertex3f(x1, y1, 0);

		glVertex3f(x1, y2, 0);
		glVertex3f(x2, y2, 0);
		glVertex3f(x2, y2, z);
		glVertex3f(x1, y2, z);

		glEnd();
}

void glDrawLine(float x1, float y1, float x2, float y2, float r, float g, float b, float a)
{
		glColor4f(r,g,b,a);
		glBegin(GL_LINES); 
		glVertex2f(x1, y1);
		glVertex2f(x2, y2);
		glEnd();
}

void CLogo::Post_HUD_Redraw (float flTime, int intermission)
{
	if((bDisplayScroll) && (gSuperHUD.iSuperHUDLevel >= 1)) {
		glPushMatrix();
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_TEXTURE_2D);
	
		glDrawRectangle(0, gSuperHUD.m_scrinfo.iHeight - 16, 
						gSuperHUD.m_scrinfo.iWidth, gSuperHUD.m_scrinfo.iHeight, 
						0.0f,0.0f,0.0f,fFade);

		glDrawLine(0, gSuperHUD.m_scrinfo.iHeight - 16, 
						gSuperHUD.m_scrinfo.iWidth, gSuperHUD.m_scrinfo.iHeight - 16, 
						0.0f,0.0f,0.0f,fFade - 0.1f);
		glDrawLine(0, gSuperHUD.m_scrinfo.iHeight - 17, 
						gSuperHUD.m_scrinfo.iWidth, gSuperHUD.m_scrinfo.iHeight - 17, 
						0.0f,0.0f,0.0f,fFade - 0.2f);

		glDrawLine(0, gSuperHUD.m_scrinfo.iHeight - 18, 
						gSuperHUD.m_scrinfo.iWidth, gSuperHUD.m_scrinfo.iHeight - 18, 
					0.0f,0.0f,0.0f,fFade - 0.3f);
		glEnable(GL_TEXTURE_2D);
		glDisable(GL_BLEND); 
		glPopMatrix();

		char buf[255];
		sprintf(buf,"sparky v%s - non-cheat hook - based on goldfinder's hook - source available under gpl - visit us at http://sparky.konspiracy.org", strVersion);
		
		gEngfuncs.pfnDrawSetTextColor(255, 255, 255);
		gEngfuncs.pfnDrawConsoleString(fScrollerX, gSuperHUD.m_scrinfo.iHeight - 17, buf);

		if(fScrollerX < 0 - gSuperHUD.m_scrinfo.charWidths['0'] * 145) {
			fFade = fFade - 0.01f;
		} else {
			fScrollerX = fScrollerX - 1.0f;
			if(fFade < 0.4f)
				fFade = fFade + 0.005f;
		}
		if(fFade <= 0)
			bDisplayScroll = false;
	}

	if((bDisplayLogo) && (gSuperHUD.iSuperHUDLevel >= 1)) {
		
		float red = 0.25f;
		float green = 0.25f;
		float blue = 0.25f;
		float rad = fRot * 3.141592653589 / 180;

		if(gEngfuncs.GetLocalPlayer()->curstate.team == 1)
			blue = 0.80f;
		if((gEngfuncs.GetLocalPlayer()->curstate.team == 2) || (gEngfuncs.GetLocalPlayer()->curstate.team == 3))
			red = 0.80f;
		if((gEngfuncs.GetLocalPlayer()->curstate.team == 4) || (gEngfuncs.GetLocalPlayer()->curstate.team == 3))
			green = 0.80f;

		fRot = fRot + fChange;
		if (fRot > 360)
			fChange = -0.2f;
		if (fRot < 0)
			fChange = 0.2f;

		glPushMatrix();
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_LIGHT0);
		glEnable(GL_LIGHTING);
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glTranslatef(gSuperHUD.m_scrinfo.iWidth - 55, gSuperHUD.m_scrinfo.iHeight - 125, 0.0f);
		glRotatef(fRot * sin(rad), 1.0f, 0.0f, 0.0f);
		glRotatef(fRot * cos(rad), 0.0f, 1.0f, 0.0f);
		glTranslatef(-25, -25, 0.0f);
		glDrawRectangle3d( 0,  0, 50, 10, 12, red, green, blue, 1.0f);
		glDrawRectangle3d(-5, 10, 10, 20, 12, red, green, blue, 1.0f);
		glDrawRectangle3d( 0, 20, 50, 30, 12, red, green, blue, 1.0f);
		glDrawRectangle3d(45, 30, 55, 40, 12, red, green, blue, 1.0f);
		glDrawRectangle3d( 0, 40, 50, 50, 12, red, green, blue, 1.0f);
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
		glEnable(GL_TEXTURE_2D);
		char buf[255];
		sprintf(buf,"sparky v%s", strVersion);
		gSuperHUD.DrawHudString(1, 45, red * 255, green * 255, blue * 255, buf);
		glPopMatrix();

	}
}
