target("Platform")
	set_kind("shared")

	add_headerfiles("../public/platform/*.h", "*.h")
	add_files("*_common.cpp")
	if is_plat("gdk", "gdkx", "xbox360", "windows") then
		add_files("../public/dllmain.cpp", "*_win32.cpp")
		add_packages("phnt_local", {public = true})
	end

	if is_toolchain("msvc") then
		add_files("../public/platform/compiler_msvc.cpp")
	end

	add_defines("IN_PLATFORM")

	add_deps("Base")

	on_load(fix_name)
target_end()
