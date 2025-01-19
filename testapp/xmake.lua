target("TestApp")
    add_defines("IN_TESTAPP")

    add_headerfiles("**.h")
    add_files("interface.cpp", "testapp.cpp")

    if is_plat("gdk", "gdkx", "windows") then
        add_files("../public/base/dllmain.cpp")
    end

    add_deps("Base", "CommonFiles")
target_end()