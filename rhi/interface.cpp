// Exports Rhi_CreateInstance

#include "base/loader.h"
#include "base/log.h"
#include "rhi/irhiinstance.h"
#include "rhi/rhi.h"

#ifdef CH_STATIC
#ifdef RHI_VULKAN
extern IRhiInstance* CreateVulkanRhiInstance();
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
	case RhiBackendType_t::None:
	case RhiBackendType_t::Unknown:
	default:
		return nullptr;
	}
}
