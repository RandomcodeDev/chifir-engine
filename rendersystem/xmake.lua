target("RenderSystem")
    add_defines("IN_RENDER")

    add_headerfiles("**.h", "../public/rendersystem/**.h")
    add_files(
        "interface.cpp",
        "render.cpp"
    )

    add_deps("Base", "CommonFiles", "DllSupport", "Rhi")
target_end()
