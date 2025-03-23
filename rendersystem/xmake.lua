-- RenderSystem build script
-- Copyright 2025 Randomcode Developers

target("RenderSystem")
    set_kind("$(kind)")

    add_defines("IN_RENDER")

    add_headerfiles("**.h", "../public/rendersystem/**.h")
    add_files(
        "interface.cpp",
        "render.cpp"
    )

    add_deps("Base", "CommonFiles", "DllSupport", "Rhi")
target_end()
