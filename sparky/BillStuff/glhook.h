/*  ------------------------------------------------------------------------
	  glhook.h by ruiner based on mstr code
		- added v1.41 - 04/09/03
	------------------------------------------------------------------------ */

#ifndef GLHOOK_H
#define GLHOOK_H

class CGLHook : CBaseHook
{
	public:


	virtual bool Pre_HUD_Init (void);
	virtual void Cmd_ToggleWallHack (void);
	virtual void Cmd_ToggleFogHack (void);
	virtual bool Pre_HUD_Redraw (float flTime, int intermission);
	virtual bool Pre_HUD_AddEntity (int type, struct cl_entity_s *ent, const char *modelname);

	bool	bFogHack;		// fog hack enabled?
	bool	bWallHack;		// wall hack eneabled?
	bool	bSpectator;		// is the user a spectator?
	int		bPlayingDemo;	// is the user playing a demo?
};

extern CGLHook gGLHook;

extern void( APIENTRY *pglEnable )(int arg1);
extern void APIENTRY hook_glEnable(int arg1);

extern void( APIENTRY *pglBegin )(int arg1);
extern void APIENTRY hook_glBegin(int arg1);
	
#endif
