#include "iapplication.h"
#include "isystem.h"
#include "testapp.h"

#ifdef CH_STATIC
IApplication* CreateTestApp()
#else
extern "C" DLLEXPORT IApplication* CreateInterface()
#endif
{
	return reinterpret_cast<IApplication*>(new CTestApp());
}
