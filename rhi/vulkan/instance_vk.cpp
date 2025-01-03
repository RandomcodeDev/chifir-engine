#include "instance_vk.h"
#include "base/loader.h"
#include "base/log.h"

cstr REQUIRED_EXTENSIONS[] = {
	VK_KHR_SURFACE_EXTENSION_NAME,
#ifdef CH_WIN32
	VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#elif defined CH_SWITCH
	VK_NN_VI_SURFACE_EXTENSION_NAME,
#elif defined CH_LINUX
	VK_KHR_XCB_SURFACE_EXTENSION_NAME,
#endif
	VK_EXT_DEBUG_UTILS_EXTENSION_NAME};

cstr REQUIRED_LAYERS[] = {
#ifdef CH_SWITCH
	"VK_LAYER_NN_vi_swapchain",
#endif
#ifdef CH_DEBUG
	"VK_LAYER_KHRONOS_validation"
#endif
};

cstr LAYER_NAME = "VK_LAYER_KHRONOS_validation";

const VkBool32 SETTING_VALIDATE_CORE = VK_TRUE;
const VkBool32 SETTING_VALIDATE_SYNC = VK_TRUE;
const VkBool32 SETTING_THREAD_SAFETY = VK_TRUE;
const char* SETTING_DEBUG_ACTION[] = {"VK_DBG_LAYER_ACTION_BREAK"};
const char* SETTING_REPORT_FLAGS[] = {"info", "warn", "perf", "error", "debug"};
const VkBool32 SETTING_ENABLE_MESSAGE_LIMIT = VK_TRUE;
const int32_t SETTING_DUPLICATE_MESSAGE_LIMIT = 3;
const char* SETTING_ENABLES[] = {
	"VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT", "VALIDATION_CHECK_ENABLE_VENDOR_SPECIFIC_ALL"};
const VkBool32 SETTING_PRINTF_TO_STDOUT = VK_FALSE;

const VkLayerSettingEXT LAYER_SETTINGS[] = {
	{LAYER_NAME,           "validate_core", VK_LAYER_SETTING_TYPE_BOOL32_EXT,                                1,           &SETTING_VALIDATE_CORE},
	// TODO: fix the WRITE_AFTER_WRITE hazard that shows up when this is enabled
	//{LAYER_NAME,           "validate_sync", VK_LAYER_SETTING_TYPE_BOOL32_EXT,                                1,
	//&SETTING_VALIDATE_SYNC},
	{LAYER_NAME,           "thread_safety", VK_LAYER_SETTING_TYPE_BOOL32_EXT,                                1,           &SETTING_THREAD_SAFETY},
	{LAYER_NAME,            "debug_action", VK_LAYER_SETTING_TYPE_STRING_EXT,                                1,             SETTING_DEBUG_ACTION},
	{LAYER_NAME,            "report_flags", VK_LAYER_SETTING_TYPE_STRING_EXT, ARRAY_SIZE(SETTING_REPORT_FLAGS),             SETTING_REPORT_FLAGS},
	{LAYER_NAME,    "enable_message_limit", VK_LAYER_SETTING_TYPE_BOOL32_EXT,                                1,    &SETTING_ENABLE_MESSAGE_LIMIT},
	{LAYER_NAME, "duplicate_message_limit",  VK_LAYER_SETTING_TYPE_INT32_EXT,                                1, &SETTING_DUPLICATE_MESSAGE_LIMIT},
	//{LAYER_NAME,				 "enables", VK_LAYER_SETTING_TYPE_STRING_EXT,      ARRAY_SIZE(SETTING_ENABLES), SETTING_ENABLES},
	{LAYER_NAME,        "printf_to_stdout", VK_LAYER_SETTING_TYPE_BOOL32_EXT,                                1,        &SETTING_PRINTF_TO_STDOUT}
};

GLADapiproc GladLoadFunction(void* userData, cstr name)
{
	ILibrary* lib = static_cast<ILibrary*>(userData);
	return lib->GetSymbol<GLADapiproc>(name);
}

bool CVulkanRhiInstance::Initialize()
{
	m_vulkanLib = Base_LoadLibrary("vulkan-1");
	if (!m_vulkanLib)
	{
		Log_Error("Failed to load Vulkan runtime!");
		return false;
	}

	if (!gladLoadVulkanUserPtr(VK_NULL_HANDLE, GladLoadFunction, m_vulkanLib))
	{
		Log_Error("Failed to get Vulkan API!");
		Destroy();
		return false;
	}

	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	appInfo.pApplicationName = GAME_NAME;
	appInfo.applicationVersion = 0;
	appInfo.pEngineName = "Чифир Engine";
	appInfo.engineVersion = 0;
	appInfo.apiVersion = VK_API_VERSION_1_3;

	VkInstanceCreateInfo instanceCreateInfo = {};
	instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceCreateInfo.pApplicationInfo = &appInfo;

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
    layerSettingsCreateInfo.settingCount = ARRAY_SIZE(LAYER_SETTINGS);
    layerSettingsCreateInfo.pNext = &debugCreateInfo;

#ifdef CH_DEBUG
#ifdef CH_SWITCH
    instanceCreateInfo.pNext = &debugCreateInfo;
#else
    instanceCreateInfo.pNext = &layerSettingsCreateInfo;
#endif
#endif

	VkResult result = vkCreateInstance(&instanceCreateInfo, &g_vkAllocationCallbacks, &m_instance);
	if (result != VK_SUCCESS)
	{
		Log_Error("Failed to create VkInstance: %s", GetVkResultString(result));
		Destroy();
		return false;
	}

	return true;
}

void CVulkanRhiInstance::Destroy()
{
	if (m_instance)
	{
		vkDestroyInstance(m_instance, &g_vkAllocationCallbacks);
	}

	if (m_vulkanLib)
	{
		delete m_vulkanLib;
	}
}
