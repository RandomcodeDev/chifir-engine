-- DirectX 8 RHI implementation build script
-- Copyright 2025 Randomcode Developers

target("RhiDirectX8")
    set_kind("$(kind)")

    add_defines("IN_RHI", "IN_RHI_DIRECTX8", "RHI_DIRECTX8")

    add_includedirs("../dxcommon")

    add_headerfiles("**.h")
    add_files(
    	"../dxcommon/commandlist_dx.cpp",
    	"../dxcommon/util_dx.cpp",

        "device_dx8.cpp",
        "image_dx8.cpp",
        "instance_dx8.cpp",
        "swapchain_dx8.cpp"
    )

    add_deps("Base", "CommonFiles", "DllSupport")
target_end()
