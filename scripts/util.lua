function is_toolchain(...)
	local r = false
	args = table.pack(...)
	for i = 1, args.n do
		r = r or (get_config("toolchain") == args[i])
	end
	return r
end

function fix_name(target)
	if is_plat("gdk", "gdkx", "xbox360") then
		target:set("prefixname", "")
		if target:kind() == "binary" then
			target:set("extension", ".exe")
		elseif target:kind() == "static" then
			target:set("extension", ".lib")
		elseif target:kind() == "shared" then
			target:set("extension", ".dll")
		end
	elseif is_plat("switch") then
		if target:kind() == "binary" then
			target:set("prefixname", "")
			target:set("extension", ".nss")
		elseif target:kind() == "static" then
			target:set("prefixname", "lib")
			target:set("extension", ".a")
		elseif target:kind() == "shared" then
			target:set("prefixname", "lib")
			target:set("extension", ".nrs")
		end
	elseif is_plat("switchhb", "psp", "ps3") then
		if target:kind() == "binary" then
			target:set("prefixname", "")
			target:set("extension", ".elf")
		elseif target:kind() == "static" then
			target:set("prefixname", "lib")
			target:set("extension", ".a")
		elseif target:kind() == "shared" then
			target:set("prefixname", "lib")
			target:set("extension", ".elf")
		end
	end
end

function add_commonfiles(target)
	if target:kind() == "shared" and is_plat("gdk", "gdkx", "xbox360", "windows") then
		target:add("files", os.projectdir() .. "/public/base/dllmain.cpp")
	end

	if is_plat("gdk", "gdkx", "xbox360", "windows") then
		target:add("files", os.projectdir() .. "/public/base/compiler_msvc.cpp")
		if is_arch("x64", "amd64", "x86_64") then
			target:add("files", os.projectdir() .. "/public/base/compiler_msvc_amd64.asm")
		end
	end

	if is_toolchain("clang", "clang-cl") then
		target:add("files", os.projectdir() .. "/public/base/compiler_clang.cpp")
	end

	target:add("files", os.projectdir() .. "/public/base/memoverride.cpp")
end

function fix_target(target)
	local util = import("scripts.util", {rootdir = os.projectdir()})
	util.fix_name(target)
	util.add_commonfiles(target)
end
