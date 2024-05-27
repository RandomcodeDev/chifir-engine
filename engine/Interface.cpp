#include "CEngineApplication.h"
#include "Engine.h"

extern "C" KR_EXPORT IApplication* CreateInterface()
{
	return new CEngineApplication();
}
