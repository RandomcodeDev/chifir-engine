-- DirectX 9 RHI implementation build script
-- Copyright 2025 Randomcode Developers

target("RhiDirectX9")
    set_kind("$(kind)")

    add_defines("IN_RHI", "IN_RHI_DIRECTX9", "RHI_DIRECTX9")

    add_includedirs("../dxcommon")

    add_headerfiles("**.h", "../dxcommon/**.h")
    add_files(
    	"../dxcommon/commandlist_dx.cpp",
    	"../dxcommon/util_dx.cpp",

        "device_dx9.cpp",
        "image_dx9.cpp",
        "instance_dx9.cpp",
        "swapchain_dx9.cpp"
    )

    add_deps("Base", "CommonFiles", "DllSupport")
target_end()
