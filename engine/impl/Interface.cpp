#include "CEngineApplication.h"
#include "engine/Engine.h"

extern "C" KR_EXPORT IApplication* CreateInterface()
{
	return new CEngineApplication();
}
