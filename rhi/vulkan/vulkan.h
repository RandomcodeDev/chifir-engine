#pragma once

// Unfortunately, the SDK I have doesn't like the debugging stuff
#if defined CH_DEBUG && !defined CH_SWITCH
#define VULKAN_DEBUG
#endif

#ifdef CH_WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#elif defined CH_SWITCH
#define VK_USE_PLATFORM_VI_NN
#else
// man i hate xlib _and_ wayland, they both suck ass
#define VK_USE_PLATFORM_XCB_KHR
#define VK_USE_PLATFORM_WAYLAND_KHR
#endif

#define VK_USE_64_BIT_PTR_DEFINES 0
#include "volk/volk.h"

extern const VkAllocationCallbacks* GetVkAllocationCallbacks();

extern cstr GetVkResultString(VkResult result);

extern VkBool32 VKAPI_CALL VkDebugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT types,
	const VkDebugUtilsMessengerCallbackDataEXT* callbackData, void* userData);
