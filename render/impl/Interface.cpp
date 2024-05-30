#include "framework/Util.h"

#include "render/IRenderSystem.h"
#if defined KR_RENDER_DX12
#include "render/impl/dx12/CDirectX12RenderSystem.h"
#endif

extern "C" KR_EXPORT ISystem* CreateInterface()
{
#ifdef KR_RENDER_DX12
	return new CDirectX12RenderSystem();
#else
	KR_QUIT("No render system implementation!");
#endif
}

extern "C" KR_EXPORT u32 GetSystemVersion()
{
	return IRenderSystem::VERSION;
}
