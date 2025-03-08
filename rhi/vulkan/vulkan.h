#pragma once

// Unfortunately, the SDK I have doesn't like the debugging stuff
#if defined CH_DEBUG && !defined CH_SWITCH
#define VULKAN_DEBUG
#endif

#define VOLK_VULKAN_H_PATH "vulkan/vulkan.h"

#define VK_USE_64_BIT_PTR_DEFINES 0
#include "volk/volk.h"

#include "base/macros.h"
#include "base/types.h"

extern const VkAllocationCallbacks* GetVkAllocationCallbacks();

constexpr cstr GetVkResultString(VkResult result)
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

extern VkBool32 VKAPI_CALL VkDebugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT types,
	const VkDebugUtilsMessengerCallbackDataEXT* callbackData, void* userData);

template <typename T>
void NameVkObject(VkDevice device, T object, VkObjectType type, cstr name, ...)
{
	extern void NameVkObject(VkDevice device, u64 object, VkObjectType type, cstr name, va_list args);
	va_list args;
	va_start(args, name);
	NameVkObject(device, reinterpret_cast<u64>(object), type, name, args);
	va_end(args);
}
