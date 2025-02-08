target("LauncherMain")
    set_kind("binary")
    set_basename(game_name)

    add_private_launcher_settings()

    if is_plat("windows", "gdkx") then
        add_files("main_win32.cpp")
        add_links("ntdll.lib", "ntdll_libc.lib")
    elseif is_plat("linux") then
        add_files("main_unix.cpp")
    elseif is_plat("switch") then
        add_files("../private/launcher/main_switch.cpp")
    end

    if static_build then
        add_deps("Launcher", "Engine", "RenderSystem", "VideoSystem")
    end
target_end()

target("Launcher")
    add_headerfiles("**.h", "../public/launcher/**.h")
    add_files("main.cpp")

    add_deps("Base", "CommonFiles", "DllSupport")
target_end()
