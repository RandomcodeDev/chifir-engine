target("RhiVulkan")
    set_kind("$(kind)")

    add_defines("IN_RHI", "IN_RHI_VULKAN", "RHI_VULKAN")

    add_headerfiles("**.h")
    add_files(
        "device_vk.cpp",
        "instance_vk.cpp",
        "swapchain_vk.cpp",
        "vulkan_stuff.cpp",

        "../../external/volk/volk.cpp"
    )

    if is_plat("windows") then
        add_defines("VK_USE_PLATFORM_WIN32_KHR")
    elseif is_plat("switch") then
        add_defines("VK_USE_PLATFORM_VI_NN")
    else
        add_defines("VK_USE_PLATFORM_WAYLAND_KHR", "VK_USE_PLATFORM_XCB_KHR")
    end

    add_deps("Base", "CommonFiles", "DllSupport")
target_end()
