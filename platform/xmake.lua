target("Platform")
	set_kind("shared")

	add_headerfiles("../public/platform/*.h", "*.h")
	add_files("*_common.cpp")
	if is_plat("gdk", "gdkx", "xbox360", "windows") then
		add_files("../public/dllmain.cpp", "*_win32.cpp")
	end

	add_packages("mimalloc_local")
	add_packages("phnt_local", {public = true})

	on_load(fix_name)
target_end()
