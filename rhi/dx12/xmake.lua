target("RhiDirectX12")
    add_defines("IN_RHI", "IN_RHI_DIRECTX12", "RHI_DIRECTX12")

    add_headerfiles("**.h")
    add_files(
        "device_dx12.cpp",
        "instance_dx12.cpp"
    )

    add_deps("Base", "CommonFiles", "DllSupport")
target_end()
