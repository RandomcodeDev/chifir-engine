target("Launcher")
	set_kind("binary")

	add_files("main.cpp")
	if is_plat("gdk", "gdkx", "xbox360", "windows") then
		add_files("main_win32.cpp")
	elseif is_plat("linux") then
		add_files("main_linux.cpp")
	elseif is_plat("switch") then
		add_files("../private/launcher/main_switch.cpp")
	end
target_end()
