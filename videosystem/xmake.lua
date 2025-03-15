target("VideoSystem")
    set_kind("$(kind)")

    add_defines("IN_VIDEO")

    add_headerfiles("**.h", "../private/videosystem/**.h", "../public/videosystem/**.h")
    add_files(
        "interface.cpp",
        "video_null.cpp"
    )

    if is_plat("windows", "gdkx") then
        add_files("video_win32.cpp")
    elseif is_plat("switch") then
        add_files("../private/videosystem/video_switch.cpp")
    end

    add_deps("Base", "CommonFiles", "DllSupport")
target_end()
