includes("vulkan")

target("Rhi")
    add_defines("IN_RHI")

    add_headerfiles("*.h", "../public/rhi/**.h")
    add_files(
        "interface.cpp"
    )

    add_deps("Base", "CommonFiles", "DllSupport")
target_end()
