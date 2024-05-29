#include "engine/Engine.h"

#include "CEngineApplication.h"

extern "C" KR_EXPORT IApplication* CreateInterface()
{
	return new CEngineApplication();
}
