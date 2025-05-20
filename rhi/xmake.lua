-- RHI interface build script
-- Copyright 2025 Randomcode Developers

if directx8 then
    includes("dx8")
end

if directx9 then
    includes("dx9")
end

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
        if directx8 then
            add_deps("RhiDirectX8")
        end

        if directx9 then
            add_deps("RhiDirectX9")
        end

        if directx then
            add_deps("RhiDirectX12")
        end

        if vulkan then
            add_deps("RhiVulkan")
        end
    end
target_end()
