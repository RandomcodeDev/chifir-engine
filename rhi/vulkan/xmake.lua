target("RhiVulkan")
    add_defines("IN_RHI", "IN_RHI_VULKAN", "RHI_VULKAN")

    add_headerfiles("**.h")
    add_files(
        "device_vk.cpp",
        "instance_vk.cpp",
        "vulkan_stuff.cpp",

        "../../external/volk/volk.cpp"
    )

    add_deps("Base", "CommonFiles", "DllSupport")
target_end()
