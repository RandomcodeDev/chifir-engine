/// DLL interface for render system
/// Copyright 2025 Randomcode Developers

#include "isystem.h"
#include "render.h"

#ifdef CH_STATIC
ISystem* CreateRenderSystem()
#else
extern "C" DLLEXPORT ISystem* CreateInterface()
#endif
{
	return new CRenderSystem();
}
