target("Engine")
    set_kind("$(kind)")

    add_defines("IN_ENGINE")

    add_headerfiles("**.h", "../public/engine/**.h")
    add_files("interface.cpp", "engine.cpp")

    add_deps("Base", "CommonFiles", "DllSupport")
target_end()
