-- DirectX 12 RHI implementation build script
-- Copyright 2025 Randomcode Developers

target("RhiDirectX12")
    set_kind("$(kind)")

    add_defines("IN_RHI", "IN_RHI_DIRECTX12", "RHI_DIRECTX12")

    add_headerfiles("**.h")
    add_files(
        "device_dx12.cpp",
        "image_dx12.cpp",
        "instance_dx12.cpp",
        "swapchain_dx12.cpp"
    )

    after_build(function (target)
        if is_plat("windows") then
            agility_dir = "$(scriptdir)/../../external/d3d12_agility/build/native/bin/"
            if is_arch("x64") then
                agility_dir = path.join(agility_dir, "x64/")
            elseif is_arch("x86") then
                agility_dir = path.join(agility_dir, "win32/")
            elseif is_arch("arm64") then
                agility_dir = path.join(agility_dir, "arm64/")
            end

            outdir = target:targetdir() .. "/bin"
            os.mkdir(outdir)
            for _, dll in ipairs({"D3D12Core", "d3d12SDKLayers"}) do
                if not os.exists(path.join(outdir, dll .. ".dll")) then
                    if os.exists(path.join(agility_dir, dll .. ".dll")) then
                        os.cp(path.join(agility_dir, dll .. ".dll"), path.join(outdir, dll .. ".dll"))
                    end
                    if os.exists(path.join(agility_dir, dll .. ".pdb")) then
                        os.cp(path.join(agility_dir, dll .. ".pdb"), path.join(outdir, dll .. ".pdb"))
                    end
                end
            end
        end
    end)

    add_deps("Base", "CommonFiles", "DllSupport")
target_end()
