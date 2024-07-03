target("alloctest")
	set_kind("binary")

	add_headerfiles("random.h")
	add_files("alloctest.cpp")

	add_deps("Base")

	if is_toolchain("msvc") then
		add_links("libcmt", "libcpmt")
	else
		add_links("libc", "libc++")
	end

	on_load(fix_target)
target_end()
