target("Base")
	set_kind("shared")

	add_headerfiles("../public/base/*.h", "*.h")
	add_files("*.cpp")
	if is_plat("gdk", "gdkx", "xbox360", "windows") then
		add_files("*_win32.cpp")
		remove_files("*_linux.cpp")
		add_packages("phnt_local", {public = true})
	end

	add_defines("IN_BASE")

	--add_packages("mimalloc_local")

	on_load(fix_target)
target_end()
