#include "framework/IPlatform.h"
#include "framework/Util.h"

#include "render/IRenderSystem.h"
#include "render/impl/CNVRHIRenderSystem.h"

extern "C" KR_EXPORT ISystem* CreateInterface()
{
	return new CNVRHIRenderSystem();
}

extern "C" KR_EXPORT u32 GetSystemVersion()
{
	return IRenderSystem::VERSION;
}
