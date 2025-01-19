target("Engine")
    add_defines("IN_ENGINE")

    add_headerfiles("**.h")
    add_files("interface.cpp", "engine.cpp")

    if is_plat("windows", "gdkx") then
        add_files("../public/base/dllmain.cpp")
    end

    add_deps("Base", "CommonFiles")
target_end()