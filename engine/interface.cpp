/// DLL interface for engine
/// Copyright 2025 Randomcode Developers

#include "engine.h"
#include "iapplication.h"
#include "isystem.h"

#ifdef CH_STATIC
IApplication* CreateEngine()
#else
extern "C" DLLEXPORT IApplication* CreateInterface()
#endif
{
	return static_cast<IApplication*>(new CEngine());
}
