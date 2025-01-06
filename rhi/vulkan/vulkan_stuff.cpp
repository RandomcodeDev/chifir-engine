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
    // silly ahh drivers got free(nullptr)
	if (pMemory)
	{
		Base_Free(pMemory);
	}
}

VkAllocationCallbacks g_vkAllocationCallbacks = {nullptr, VkAlloc, VkRealloc, VkFree, VkAllocNotification, VkFreeNotification};

cstr GetVkResultString(VkResult result)
{
#define X(result)                                                                                                                \
	case result:                                                                                                                 \
		return STRINGIZE(result);

	switch (result)
	{
		X(VK_SUCCESS)
		X(VK_NOT_READY)
		X(VK_TIMEOUT)
		X(VK_EVENT_SET)
		X(VK_EVENT_RESET)
		X(VK_INCOMPLETE)
		X(VK_ERROR_OUT_OF_HOST_MEMORY)
		X(VK_ERROR_OUT_OF_DEVICE_MEMORY)
		X(VK_ERROR_INITIALIZATION_FAILED)
		X(VK_ERROR_DEVICE_LOST)
		X(VK_ERROR_MEMORY_MAP_FAILED)
		X(VK_ERROR_LAYER_NOT_PRESENT)
		X(VK_ERROR_EXTENSION_NOT_PRESENT)
		X(VK_ERROR_FEATURE_NOT_PRESENT)
		X(VK_ERROR_INCOMPATIBLE_DRIVER)
		X(VK_ERROR_TOO_MANY_OBJECTS)
		X(VK_ERROR_FORMAT_NOT_SUPPORTED)
		X(VK_ERROR_FRAGMENTED_POOL)
		X(VK_ERROR_UNKNOWN)
		X(VK_ERROR_OUT_OF_POOL_MEMORY)
		X(VK_ERROR_INVALID_EXTERNAL_HANDLE)
		X(VK_ERROR_FRAGMENTATION)
		X(VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS)
		X(VK_ERROR_SURFACE_LOST_KHR)
		X(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR)
		X(VK_SUBOPTIMAL_KHR)
		X(VK_ERROR_OUT_OF_DATE_KHR)
		X(VK_ERROR_INCOMPATIBLE_DISPLAY_KHR)
		X(VK_ERROR_VALIDATION_FAILED_EXT)
		X(VK_ERROR_INVALID_SHADER_NV)
		X(VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT)
		X(VK_ERROR_NOT_PERMITTED_EXT)
		X(VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT)
		X(VK_THREAD_IDLE_KHR)
		X(VK_THREAD_DONE_KHR)
		X(VK_OPERATION_DEFERRED_KHR)
		X(VK_OPERATION_NOT_DEFERRED_KHR)
		X(VK_PIPELINE_COMPILE_REQUIRED_EXT)
#ifndef CH_SWITCH
		X(VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR)
		X(VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR)
		X(VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR)
		X(VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR)
		X(VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR)
		X(VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR)
#endif
	default:
		return "VK_UNKNOWN";
	}

#undef X
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
		typeStr, ARRAY_SIZE(typeStr), "%s%s%smessage", (types & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) ? "general " : "",
		(types & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) ? "performance " : "",
		(types & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) ? "validation " : "");
	typeStr[0] = Base_ToUpper(typeStr[0]);
	// TODO: symbol name getting?
	Log_Write(level, Plat_GetReturnAddress(), true, "Vulkan", typeStr, callbackData->pMessage);

	return true;
}
