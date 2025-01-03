#pragma once

#ifdef CH_WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#elif defined CH_SWITCH
#define VK_USE_PLATFORM_NN_VI
#else
// man i hate xlib _and_ wayland, they both suck ass
#define VK_USE_PLATFORM_XCB_KHR
#define VK_USE_PLATFORM_WAYLAND_KHR
#endif

#define VK_USE_64_BIT_PTR_DEFINES 0
#include "glad/vulkan.h"

extern VkAllocationCallbacks g_vkAllocationCallbacks;

extern cstr GetVkResultString(VkResult result);

extern VkBool32 VKAPI_CALL VkDebugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT types,
	const VkDebugUtilsMessengerCallbackDataEXT* callbackData, void* userData);
