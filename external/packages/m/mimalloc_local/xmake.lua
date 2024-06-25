package("mimalloc_local")
	set_homepage("https://github.com/microsoft/mimalloc")
	set_description("mimalloc (pronounced 'me-malloc') is a general purpose allocator with excellent performance characteristics.")
	set_license("MIT")

	add_includedirs("include")
	add_links("mimalloc_local")

	set_urls("https://github.com/microsoft/mimalloc/archive/v$(version).zip")
	add_versions("2.1.7", "fa61cf01e3dd869b35275bfd8be95bfde77f0b65dfa7e34012c09a66e1ea463f")

	if is_plat("android") then
		add_syslinks("atomic")
	end

	on_install("macosx", "gdk", "gdkx", "windows", "linux", "android", "mingw", function (package)
		os.cp(path.join(package:scriptdir(), "port", "xmake.lua"), "xmake.lua")
		import("package.tools.xmake").install(package, configs)
	end)
package_end()
