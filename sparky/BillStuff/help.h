/*  ------------------------------------------------------------------------
	  help.h by Fr33
		- april 2003
	------------------------------------------------------------------------ */

#ifndef HELP_H
#define HELP_H

#include <vector>

using namespace std;

struct sHelp
{ 
	char *command; 
	char *text; 
};

class CSparkyHelp : CBaseHook
{
	public:

	virtual bool Pre_HUD_Init (void);
	virtual void Post_HUD_Init (void);

	virtual void Cmd_SparkyHelp (void);

	virtual void addCommand (char *command, char *text);

	char *lookup(char *command);

	CSparkyHelp (void);

	vector<sHelp> cmdList;
};

extern CSparkyHelp gHelp;

#endif
