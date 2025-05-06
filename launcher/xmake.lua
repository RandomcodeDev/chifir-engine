-- Launcher build script
-- Copyright 2025 Randomcode Developers

target("LauncherMain")
    set_kind("binary")
    set_basename(game_name)

    add_private_launcher_settings()

    if is_plat("windows", "scarlett") then
        add_files("main_win32.cpp")
        if not is_kind("static") then
			-- see public/win32/readme.txt
            add_links("ntdll.lib", "ntdll_libc.lib")
        end
    elseif is_plat("xbox") then
        add_files("main_xbox.cpp")
        add_links("xboxkrnl.lib")
        if is_mode("debug") then
            add_links("libcmtd.lib", "xapilibd.lib", "xbdm.lib")
        else
            add_links("libcmt.lib", "xapilib.lib")
        end
    elseif is_plat("linux") then
        add_files("main_unix.cpp")
    elseif is_plat("nx") then
        add_files("../private/launcher/main_switch.cpp")
	elseif is_plat("orbis") then
		add_files("../private/launcher/main_orbis.cpp")
    end

    if is_kind("static") then
        add_deps("Launcher", "Engine", "RenderSystem", "VideoSystem")
    end
target_end()

target("Launcher")
    set_kind("$(kind)")

    add_headerfiles("**.h", "../public/launcher/**.h")
    add_files("main.cpp")

    add_deps("Base", "CommonFiles", "DllSupport")
target_end()
