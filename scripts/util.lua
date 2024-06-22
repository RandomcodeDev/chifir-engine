function is_toolchain(...)
	local r = false
	for _, v in ipairs(...) do
		r = r or get_config("toolchain") == v
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
