#ifndef ENGINEHOOK_H
#define ENGINEHOOK_H

#include "engine.h"

extern cl_enginefunc_t gEngfuncs;
extern cl_enginefunc_t gHookEngfuncs;

extern efx_api_t gHookEfxAPI;

void InitializeEngineHook(void);
#endif