/*  ------------------------------------------------------------------------
	  kselookup.h by ruiner
	    - updated v1.36 - 01/14/03
		- added v1.35 - 01/14/03
	------------------------------------------------------------------------ */

#ifndef KSELOOKUP_H
#define KSELOOKUP_H

class CKSELookup : CBaseHook
{
	public:
		void	Post_HUD_Init(void);
		void	Cmd_KSELookup(void);
};

extern CKSELookup gKSELookup;

#endif