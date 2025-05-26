-- VideoSystem build script
-- Copyright 2025 Randomcode Developers

target("VideoSystem")
    set_kind("$(kind)")

    add_defines("IN_VIDEO")

    add_headerfiles("**.h", "../private/videosystem/**.h", "../public/videosystem/**.h")
    add_files(
        "interface.cpp",
        "video_null.cpp"
    )

    if is_plat("windows", "scarlett") then
        add_files("video_win32.cpp", "video_winrt.cpp")
	elseif is_plat("xbox") then
		add_files("video_xbox.cpp")
    elseif is_plat("nx") then
        add_files("../private/videosystem/video_nx.cpp")
    elseif is_plat("linux") then
        add_files("video_sdl.cpp")
        add_packages("libsdl3")
    end

    add_deps("Base", "CommonFiles", "DllSupport")
target_end()
