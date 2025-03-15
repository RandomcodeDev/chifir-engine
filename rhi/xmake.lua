if directx then
    includes("dx12")
end

if vulkan then
    includes("vulkan")
end

target("Rhi")
    set_kind("$(kind)")

    add_defines("IN_RHI")

    add_headerfiles("*.h", "../public/rhi/**.h")
    add_files(
        "interface.cpp"
    )

    add_deps("Base", "CommonFiles", "DllSupport")
    if is_kind("static") then
        if directx then
            add_deps("RhiDirectX12")
        end

        if vulkan then
            add_deps("RhiVulkan")
        end
    end
target_end()
