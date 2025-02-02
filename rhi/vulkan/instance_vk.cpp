#include "device_vk.h"
#include "instance_vk.h"
#include "base/loader.h"
#include "base/log.h"
#include "base/vector.h"
#include "rhi/irhidevice.h"
#include "videosystem/ivideosystem.h"

constexpr cstr REQUIRED_EXTENSIONS[] = {
	VK_KHR_SURFACE_EXTENSION_NAME,
#ifdef CH_WIN32
	VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#elif defined CH_SWITCH
	VK_NN_VI_SURFACE_EXTENSION_NAME,
#elif defined CH_LINUX
	VK_KHR_XCB_SURFACE_EXTENSION_NAME,
#endif
#ifdef VULKAN_DEBUG
	VK_EXT_DEBUG_UTILS_EXTENSION_NAME
#endif
};

constexpr cstr REQUIRED_LAYERS[] = {
#ifdef CH_SWITCH
	"VK_LAYER_NN_vi_swapchain",
#endif
#ifdef VULKAN_DEBUG
	"VK_LAYER_KHRONOS_validation",
#endif
	nullptr};

#ifdef VULKAN_DEBUG
constexpr cstr LAYER_NAME = "VK_LAYER_KHRONOS_validation";

constexpr VkBool32 SETTING_VALIDATE_CORE = VK_TRUE;
// constexpr VkBool32 SETTING_VALIDATE_SYNC = VK_TRUE;
constexpr VkBool32 SETTING_THREAD_SAFETY = VK_TRUE;
constexpr cstr SETTING_DEBUG_ACTION[] = {"VK_DBG_LAYER_ACTION_BREAK"};
constexpr cstr SETTING_REPORT_FLAGS[] = {"info", "warn", "perf", "error"};
constexpr VkBool32 SETTING_ENABLE_MESSAGE_LIMIT = VK_TRUE;
constexpr int32_t SETTING_DUPLICATE_MESSAGE_LIMIT = 3;
// constexpr cstr SETTING_ENABLES[] = {
//	"VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT", "VALIDATION_CHECK_ENABLE_VENDOR_SPECIFIC_ALL"};
constexpr VkBool32 SETTING_PRINTF_TO_STDOUT = VK_FALSE;

constexpr VkLayerSettingEXT LAYER_SETTINGS[] = {
	{LAYER_NAME,           "validate_core", VK_LAYER_SETTING_TYPE_BOOL32_EXT,                                    1,           &SETTING_VALIDATE_CORE},
	// TODO: fix the WRITE_AFTER_WRITE hazard that shows up when this is enabled
	//{LAYER_NAME,           "validate_sync", VK_LAYER_SETTING_TYPE_BOOL32_EXT,                                1,
	//&SETTING_VALIDATE_SYNC},
	{LAYER_NAME,           "thread_safety", VK_LAYER_SETTING_TYPE_BOOL32_EXT,                                    1,           &SETTING_THREAD_SAFETY},
	{LAYER_NAME,            "debug_action", VK_LAYER_SETTING_TYPE_STRING_EXT,                                    1,             SETTING_DEBUG_ACTION},
	{LAYER_NAME,            "report_flags", VK_LAYER_SETTING_TYPE_STRING_EXT, ArraySize<u32>(SETTING_REPORT_FLAGS),             SETTING_REPORT_FLAGS},
	{LAYER_NAME,    "enable_message_limit", VK_LAYER_SETTING_TYPE_BOOL32_EXT,                                    1,    &SETTING_ENABLE_MESSAGE_LIMIT},
	{LAYER_NAME, "duplicate_message_limit",  VK_LAYER_SETTING_TYPE_INT32_EXT,                                    1, &SETTING_DUPLICATE_MESSAGE_LIMIT},
	//{LAYER_NAME,				 "enables", VK_LAYER_SETTING_TYPE_STRING_EXT,      ArraySize(SETTING_ENABLES), SETTING_ENABLES},
	{LAYER_NAME,        "printf_to_stdout", VK_LAYER_SETTING_TYPE_BOOL32_EXT,                                    1,        &SETTING_PRINTF_TO_STDOUT}
};
#endif

PFN_vkVoidFunction VolkLoadFunction(void* userData, cstr name)
{
	ILibrary* lib = static_cast<ILibrary*>(userData);
	PFN_vkVoidFunction symbol = lib->GetSymbol<PFN_vkVoidFunction>(name);
	if (!symbol)
	{
		Log_Error("Failed to get Vulkan symbol %s!", name);
	}
	return symbol;
}

bool CVulkanRhiInstance::Initialize(IVideoSystem* videoSystem)
{
	Log_Debug("Initializing Vulkan instance");

	Log_Debug("Loading Vulkan runtime");
	m_vulkanLib = Base_LoadLibrary("vulkan-1");
	if (!m_vulkanLib)
	{
		Log_Error("Failed to load Vulkan runtime!");
		return false;
	}

	volkInitializeCustom(VolkLoadFunction, m_vulkanLib);

	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = GAME_NAME;
	appInfo.applicationVersion = 0;
	appInfo.pEngineName = "Чифир Engine";
	appInfo.engineVersion = 0;
	appInfo.apiVersion = VK_API_VERSION_1_3;

	VkInstanceCreateInfo instanceCreateInfo = {};
	instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceCreateInfo.pApplicationInfo = &appInfo;

	instanceCreateInfo.ppEnabledExtensionNames = REQUIRED_EXTENSIONS;
	instanceCreateInfo.enabledExtensionCount = ArraySize<u32>(REQUIRED_EXTENSIONS);
	Log_Debug("Required extensions:");
	for (u32 i = 0; i < instanceCreateInfo.enabledExtensionCount; i++)
	{
		Log_Debug("\t%s", instanceCreateInfo.ppEnabledExtensionNames[i]);
	}

	instanceCreateInfo.ppEnabledLayerNames = REQUIRED_LAYERS;
	instanceCreateInfo.enabledLayerCount = ArraySize<u32>(REQUIRED_LAYERS) - 1;
	Log_Debug("Required layers:");
	for (u32 i = 0; i < instanceCreateInfo.enabledLayerCount; i++)
	{
		Log_Debug("\t%s", instanceCreateInfo.ppEnabledLayerNames[i]);
	}

#ifdef VULKAN_DEBUG
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
	debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	debugCreateInfo.messageSeverity =
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
								  VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
	debugCreateInfo.pfnUserCallback = VkDebugCallback;

	VkLayerSettingsCreateInfoEXT layerSettingsCreateInfo = {};
	layerSettingsCreateInfo.sType = VK_STRUCTURE_TYPE_LAYER_SETTINGS_CREATE_INFO_EXT;
	layerSettingsCreateInfo.pSettings = LAYER_SETTINGS;
	layerSettingsCreateInfo.settingCount = ArraySize<u32>(LAYER_SETTINGS);
	debugCreateInfo.pNext = &layerSettingsCreateInfo;
	instanceCreateInfo.pNext = &debugCreateInfo;
#endif

	Log_Debug("Creating VkInstance");
	VkResult result = vkCreateInstance(&instanceCreateInfo, GetVkAllocationCallbacks(), &m_instance);
	if (result == VK_ERROR_LAYER_NOT_PRESENT)
	{
		Log_Debug("Validation layer(s) missing, retrying without them");
		instanceCreateInfo.enabledLayerCount = 0;
		result = vkCreateInstance(&instanceCreateInfo, GetVkAllocationCallbacks(), &m_instance);
	}

	if (result != VK_SUCCESS)
	{
		Log_Error("Failed to create VkInstance: %s", GetVkResultString(result));
		Destroy();
		return false;
	}

	Log_Debug("Loading additional Vulkan functions");
	volkLoadInstance(m_instance);

#ifdef VULKAN_DEBUG
	Log_Debug("Creating real debug messenger");
	vkCreateDebugUtilsMessengerEXT(m_instance, &debugCreateInfo, GetVkAllocationCallbacks(), &m_debugMessenger);
#endif

	Log_Debug("Creating surface");
	m_surface = videoSystem->CreateVulkanSurface(reinterpret_cast<u64>(m_instance), GetVkAllocationCallbacks());
	if (!m_surface)
	{
		// CreateVulkanSurface does its own logging
		Destroy();
		return false;
	}

	return true;
}

void CVulkanRhiInstance::Destroy()
{
	if (m_surface)
	{
		Log_Debug("Destroying VkSurfaceKHR 0x%016X", m_surface);
		vkDestroySurfaceKHR(m_instance, m_surface, GetVkAllocationCallbacks());
		m_surface = VK_NULL_HANDLE;
	}

	if (m_debugMessenger)
	{
		Log_Debug("Destroying VkDebugUtilsMessengerEXT 0x%016X", m_debugMessenger);
		vkDestroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, GetVkAllocationCallbacks());
		m_debugMessenger = VK_NULL_HANDLE;
	}

	if (m_instance)
	{
		Log_Debug("Destroying VkInstance 0x%016X", m_instance);
		vkDestroyInstance(m_instance, GetVkAllocationCallbacks());
		m_instance = VK_NULL_HANDLE;
	}

	if (m_vulkanLib)
	{
		Log_Debug("Unloading Vulkan library %s!0x%016X", m_vulkanLib->GetName(), m_vulkanLib->GetBase());
		delete m_vulkanLib;
		m_vulkanLib = nullptr;
	}
}

void CVulkanRhiInstance::GetDeviceInfo(CVector<RhiDeviceInfo_t>& info)
{
	Log_Debug("Getting physical devices");

	u32 deviceCount = 0;
	VkResult result = vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);
	if (result != VK_SUCCESS || deviceCount < 1)
	{
		Log_Error("Failed to enumerate physical devices (or none are available): %s", GetVkResultString(result));
		return;
	}

	CVector<VkPhysicalDevice> devices;
	devices.Resize(deviceCount);
	result = vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.Data());
	if (result != VK_SUCCESS)
	{
		Log_Error("Failed to get physical devices: %s", GetVkResultString(result));
		return;
	}

	info.Empty();
	info.Reserve(devices.Size());

	for (ssize i = 0; i < devices.Size(); i++)
	{
		RhiDeviceInfo_t rhiInfo;
		VulkanDeviceInfo_t vkInfo;
		if (CVulkanRhiDevice::GetDeviceInfo(rhiInfo, vkInfo, devices[i], m_surface, i))
		{
			// only add if the device is usable
			info.Add(rhiInfo);
		}

		// RHI info includes index into all devices
		m_devices.Add(vkInfo);
	}
}

RHIAPI IRhiInstance* Vulkan_CreateInstance()
{
	return new CVulkanRhiInstance();
}
