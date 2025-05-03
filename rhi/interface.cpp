/// \file RHI library interface
/// \copyright 2025 Randomcode Developers

/// Honestly, some of the ifdefs in this file are really ugly

#include "base/basicstr.h"
#include "base/loader.h"
#include "base/log.h"

#include "rhi/irhiinstance.h"
#include "rhi/rhi.h"

RHIAPI cstr Rhi_GetBackendName(RhiBackendType backend)
{
	switch (backend)
	{
	case RhiBackendType::None:
	default:
		return "None";
	case RhiBackendType::Vulkan:
		return "Vulkan";
	case RhiBackendType::DirectX12:
		return "DirectX12";
	case RhiBackendType::DirectX9:
		return "DirectX9";
	case RhiBackendType::OpenGl:
		return "OpenGl";
	case RhiBackendType::Custom:
		return "Custom";
	}
}

RHIAPI RhiBackendType Rhi_GetBackendTypeByName(cstr name)
{
	// TODO: case insensitive, once it's done
	if (Base_StrCompare(name, "Vulkan") == 0 || Base_StrCompare(name, "vk") == 0)
	{
		return RhiBackendType::Vulkan;
	}
	else if (Base_StrCompare(name, "DirectX12") == 0 || Base_StrCompare(name, "dx12") == 0 || Base_StrCompare(name, "d3d12") == 0)
	{
		return RhiBackendType::DirectX12;
	}
	else if (Base_StrCompare(name, "DirectX9") == 0 || Base_StrCompare(name, "dx9") == 0 || Base_StrCompare(name, "d3d9") == 0)
	{
		return RhiBackendType::DirectX9;
	}
	else if (Base_StrCompare(name, "OpenGL") == 0 || Base_StrCompare(name, "gl") == 0)
	{
		return RhiBackendType::OpenGl;
	}
	else
	{
		return RhiBackendType::Custom;
	}
}

#ifdef CH_STATIC
#ifdef CH_VULKAN
extern IRhiInstance* CreateVulkanRhiInstance();
#endif
#ifdef CH_DIRECTX12
extern IRhiInstance* CreateDx12RhiInstance();
#endif
#ifdef CH_DIRECTX9
// extern IRhiInstance* CreateDx9Instance();
#endif
#ifdef CH_OPENGL
// extern IRhiInstance* CreateOpenGlInstance();
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

extern "C" RHIAPI IRhiInstance* Rhi_CreateInstance(cstr backendName)
{
	ssize endOffset = 0;
	RhiBackendType type = static_cast<RhiBackendType>(Base_ParseInt(backendName, &endOffset));
	if (endOffset < 1) // wasn't a number
	{
		type = Rhi_GetBackendTypeByName(backendName);
	}

#ifndef CH_STATIC
	// only pass the name if it's unrecognized
	return GetBackend(type == RhiBackendType::Custom ? backendName : Rhi_GetBackendName(type));
#else
	switch (type)
	{
	case RhiBackendType::Vulkan: {
#ifdef CH_VULKAN
		return CreateVulkanRhiInstance();
#else
		Log_Error("Vulkan support not available in this build!");
#endif
		return nullptr;
	}
	case RhiBackendType::DirectX12: {
#ifdef CH_DIRECTX12
		return CreateDx12RhiInstance();
#else
		Log_Error("DirectX 12 support not available in this build!");
#endif
	}
	case RhiBackendType::DirectX9: {
#ifdef CH_DIRECTX9
		return CreateDx9RhiInstance();
#else
		Log_Error("DirectX 9 support not available in this build!");
#endif
	}
	case RhiBackendType::OpenGl: {
#ifdef CH_OPENGL
		return CreateOpenGlRhiInstance();
#else
		Log_Error("OpenGL support not available in this build!");
#endif
	}
	case RhiBackendType::Custom:
	case RhiBackendType::None:
	default:
		return nullptr;
	}
#endif
}
