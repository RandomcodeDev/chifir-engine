// DLL interface for engine

#include "engine.h"
#include "iapplication.h"
#include "isystem.h"

#ifdef CH_STATIC
IApplication* CreateEngine()
#else
extern "C" DLLEXPORT IApplication* CreateInterface()
#endif
{
	return reinterpret_cast<IApplication*>(new CEngine());
}
