#include "base/log.h"
#include "vulkan.h"

static void VKAPI_CALL
VkAllocNotification(void* pUserData, usize size, VkInternalAllocationType allocationType, VkSystemAllocationScope allocationScope)
{
	(void)pUserData;
	Log_Trace("Vulkan allocation: %zu bytes, type %u, scope %u", size, allocationType, allocationScope);
}

static void VKAPI_CALL
VkFreeNotification(void* pUserData, usize size, VkInternalAllocationType allocationType, VkSystemAllocationScope allocationScope)
{
	(void)pUserData;
	Log_Trace("Vulkan free: %zu bytes, type %u, scope %u", size, allocationType, allocationScope);
}

static void* VKAPI_CALL
VkRealloc(void* pUserData, void* pOriginal, usize size, usize alignment, VkSystemAllocationScope allocationScope)
{
	(void)pUserData;
	(void)allocationScope;
	ASSERT(Base_GetAllocAlignment(pOriginal) == static_cast<ssize>(alignment));
	return Base_Realloc(pOriginal, size);
}

static void* VKAPI_CALL VkAlloc(void* pUserData, usize size, usize alignment, VkSystemAllocationScope allocationScope)
{
	(void)pUserData;
	(void)allocationScope;
	return Base_Alloc(size, alignment);
}

static void VKAPI_CALL VkFree(void* pUserData, void* pMemory)
{
	(void)pUserData;
	// silly ahh drivers got free(nullptr) in them
	if (pMemory)
	{
		Base_Free(pMemory);
	}
}

const VkAllocationCallbacks* GetVkAllocationCallbacks()
{
#ifdef CH_SWITCH
	// allocations are already overridden globally
	return nullptr;
#else
	static VkAllocationCallbacks s_vkAllocationCallbacks = {nullptr, VkAlloc, VkRealloc, VkFree, VkAllocNotification, VkFreeNotification};
	return &s_vkAllocationCallbacks;
#endif
}

VkBool32 VKAPI_CALL VkDebugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT types,
	const VkDebugUtilsMessengerCallbackDataEXT* callbackData, void* userData)
{
	char typeStr[64];
	LogLevel_t level;

	(void)userData;

	switch (severity)
	{
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
		level = LogLevel_t::Trace;
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
		level = LogLevel_t::Debug;
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
		level = LogLevel_t::Info;
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		level = LogLevel_t::Warning;
		break;
	default:
		level = LogLevel_t::Debug;
		break;
	}

	Base_StrFormat(
		typeStr, ArraySize(typeStr), "%s%s%smessage", (types & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) ? "general " : "",
		(types & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) ? "performance " : "",
		(types & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) ? "validation " : "");
	typeStr[0] = Base_ToUpper(typeStr[0]);
	// TODO: symbol name getting?
	Log_Write(level, Plat_GetReturnAddress(), true, "Vulkan", typeStr, callbackData->pMessage);

	return true;
}
