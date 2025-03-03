target("RhiDirectX12")
    add_defines("IN_RHI", "IN_RHI_DIRECTX12", "RHI_DIRECTX12")

    add_headerfiles("**.h")
    add_files(
        "device_dx12.cpp",
        "instance_dx12.cpp"
    )

    add_deps("Base", "CommonFiles", "DllSupport")

    add_links("dxgi.lib")
    if is_plat("gdkx") then
        add_links("d3d12_xs.lib")
    else
        add_links("d3d12.lib")
    end
target_end()
