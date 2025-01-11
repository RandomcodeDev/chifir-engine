// DLL interface for render system

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
