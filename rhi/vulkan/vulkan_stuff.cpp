/// \file Vulkan utility functions
/// \copyright Randomcode Developers

#include "base/async.h"
#include "base/log.h"
#include "vulkan.h"

static void VKAPI_CALL
VkAllocNotification(void* pUserData, usize size, VkInternalAllocationType allocationType, VkSystemAllocationScope allocationScope)
{
	UNUSED(pUserData);
	Log_Trace("Vulkan allocation: %zu bytes, type %u, scope %u", size, allocationType, allocationScope);
}

static void VKAPI_CALL
VkFreeNotification(void* pUserData, usize size, VkInternalAllocationType allocationType, VkSystemAllocationScope allocationScope)
{
	UNUSED(pUserData);
	Log_Trace("Vulkan free: %zu bytes, type %u, scope %u", size, allocationType, allocationScope);
}

static void* VKAPI_CALL
VkRealloc(void* pUserData, void* pOriginal, usize size, usize alignment, VkSystemAllocationScope allocationScope)
{
	UNUSED(pUserData);
	UNUSED(allocationScope);
	ASSERT(Base_GetAllocAlignment(pOriginal) == static_cast<ssize>(alignment));
	return Base_Realloc(pOriginal, size);
}

static void* VKAPI_CALL VkAlloc(void* pUserData, usize size, usize alignment, VkSystemAllocationScope allocationScope)
{
	UNUSED(pUserData);
	UNUSED(allocationScope);
	return Base_Alloc(size, alignment);
}

static void VKAPI_CALL VkFree(void* pUserData, void* pMemory)
{
	UNUSED(pUserData);
	// silly ahh drivers got free(nullptr) in them
	if (pMemory)
	{
		Base_Free(pMemory);
	}
}

const VkAllocationCallbacks* GetVkAllocationCallbacks()
{
//#ifdef CH_SWITCH
	// allocations are already overridden globally
	return nullptr;
//#else
//	static VkAllocationCallbacks s_vkAllocationCallbacks = {nullptr,           VkAlloc, VkRealloc, VkFree, VkAllocNotification,
//															VkFreeNotification};
//	return &s_vkAllocationCallbacks;
//#endif
}

VkBool32 VKAPI_CALL VkDebugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT types,
	const VkDebugUtilsMessengerCallbackDataEXT* callbackData, void* userData)
{
	char typeStr[64];
	LogLevel level;

	UNUSED(userData);

	switch (severity)
	{
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
		level = LogLevel::Trace;
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
		level = LogLevel::Debug;
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
		level = LogLevel::Info;
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		level = LogLevel::Warning;
		break;
	default:
		level = LogLevel::Debug;
		break;
	}

	Base_StrFormat(
		typeStr, ArraySize(typeStr), "%s%s%smessage", (types & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) ? "general " : "",
		(types & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) ? "performance " : "",
		(types & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) ? "validation " : "");
	typeStr[0] = Base_ToUpper(typeStr[0]);
	// TODO: The real function is 3 frames up in the loader, more in the validation layers (this is, however, consistent between
	// platforms)
	Log_Write(level, Plat_GetReturnAddress(1), true, "Vulkan", typeStr, "Main", UINT64_MAX, callbackData->pMessage);

	return true;
}

void NameVkObject(VkDevice device, u64 object, VkObjectType type, cstr name, va_list realArgs)
{
	VkDebugUtilsObjectNameInfoEXT nameInfo = {};
	nameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
	nameInfo.objectType = type;
	nameInfo.objectHandle = object;

	va_list args;
	va_copy(args, realArgs);
	nameInfo.pObjectName = Base_VStrFormat(name, args);
	va_end(args);

	Log_Trace("Setting name of type %u Vulkan object 0x%016X to %s", type, object, nameInfo.pObjectName);

	vkSetDebugUtilsObjectNameEXT(device, &nameInfo);

	Base_Free(const_cast<dstr>(nameInfo.pObjectName));
}
