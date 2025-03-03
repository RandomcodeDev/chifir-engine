// Exports Rhi_CreateInstance

#include "base/loader.h"
#include "base/log.h"
#include "rhi/irhiinstance.h"
#include "rhi/rhi.h"

#ifdef CH_STATIC
#ifdef CH_VULKAN
extern IRhiInstance* CreateVulkanRhiInstance();
#endif
#ifdef CH_DIRECTX12
extern IRhiInstance* CreateDx12RhiInstance();
#endif
#ifdef CH_DIRECTX9
//extern IRhiInstance* CreateDx9Instance();
#endif
#ifdef CH_OPENGL
//extern IRhiInstance* CreateOpenGlInstance();
#endif
#else
static IRhiInstance* GetBackend(cstr name)
{
	CString fullName = CString::FormatStr("Rhi%s", name);

	Log_Info("Loading %s RHI backend", name);
	ILibrary* backend = Base_LoadEngineLibrary(fullName.Data());
	if (!backend)
	{
		Log_Error("Failed to load %s RHI backend!", name);
		return nullptr;
	}

	CreateRhiInstance_t CreateInstance = backend->GetSymbol<CreateRhiInstance_t>("CreateInstance");
	if (!CreateInstance)
	{
		Log_Error("Failed to get CreateInstance in %s backend!", fullName.Data());
		return nullptr;
	}

	return CreateInstance();
}
#endif

extern "C" RHIAPI IRhiInstance* Rhi_CreateInstance(RhiBackendType_t type)
{
	switch (type)
	{
	case RhiBackendType_t::Vulkan: {
#ifdef CH_STATIC
		return CreateVulkanRhiInstance();
#else
		return GetBackend("Vulkan");
#endif
	}
	case RhiBackendType_t::DirectX12: {
#ifdef CH_STATIC
		return CreateDx12RhiInstance();
#else
		return GetBackend("DirectX12");
#endif
	}
	case RhiBackendType_t::DirectX9: {
#ifdef CH_STATIC
		//return CreateDx9RhiInstance();
#else
		return GetBackend("DirectX9");
#endif
	}
	case RhiBackendType_t::OpenGl: {
#ifdef CH_STATIC
		//return CreateOpenGlRhiInstance();
#else
		return GetBackend("OpenGl");
#endif
	}
	case RhiBackendType_t::Unknown: {
#ifndef CH_STATIC
		return GetBackend("Custom");
#endif
	}
	case RhiBackendType_t::None:
	default:
		return nullptr;
	}
}
