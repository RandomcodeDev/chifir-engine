target("Launcher")
	set_kind("binary")

	add_files("main.cpp")
	if is_plat("gdk", "gdkx", "xbox360", "windows") then
		add_files("main_win32.cpp")
		if is_toolchain("msvc", "clang-cl") then
			add_ldflags("-subsystem:windows")
		else
			add_ldflags("-mwindows")
		end
	elseif is_plat("linux") then
		add_files("main_linux.cpp")
	elseif is_plat("switch") then
		add_files("../private/launcher/main_switch.cpp")
	end

	add_deps("Platform")

	on_load(fix_name)
target_end()
