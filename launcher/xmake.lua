target("Launcher")
    set_kind("binary")
    set_basename(game_name)

    add_headerfiles("**.h", "../public/launcher/**.h")
    add_files("main.cpp")

    add_private_launcher_settings()

    if is_plat("windows", "gdkx") then
        add_files("main_win32.cpp")
    elseif is_plat("linux") then
        add_files("main_linux.cpp")
    elseif is_plat("switch") then
        add_files("../private/launcher/main_switch.cpp")
    end

    add_deps("Base", "CommonFiles")
    if static_build then
        add_deps("Engine", "RenderSystem", "VideoSystem")
    end
target_end()