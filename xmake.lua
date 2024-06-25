includes("scripts/util.lua")
includes("scripts/xbox360.lua")

add_rules(
	"mode.debug",
	"mode.release",
	"plugin.vsxmake.autoupdate"
)

set_policy("check.auto_ignore_flags", false)

--set_warnings("everything")

set_exceptions("no-cxx")

-- Microsoft no longer supports anything below C++14, so Clang can't compile the system headers in that mode
if is_plat("gdk", "gdkx", "xbox360", "windows") then
	set_languages("gnu89", "cxx14")
else
	set_languages("gnu89", "cxx03")
end

add_defines("_CRT_SECURE_NO_WARNINGS", "_POSIX_C_SOURCE=200809")

if is_toolchain("msvc", "clang-cl") then
	add_cxflags("-Zl") -- omit default library name in object file
	add_cxxflags("-GR-") -- Disable RTTI
	add_ldflags("-nodefaultlib") -- disable default libraries
else
	add_cxflags("-fshort-wchar", "-ffreestanding")
	add_cxxflags("-fno-rtti") -- Disable RTTI
	add_ldflags("-nostdlib") -- disable linking to C runtime
	add_shflags("-nostdlib")
end

if is_plat("windows") then
	add_defines("KR_WIN32")
elseif is_plat("gdk") then
	add_defines("KR_WIN32", "KR_GDK")
elseif is_plat("gdkx") then
	add_defines("KR_WIN32", "KR_GDK", "KR_GDKX")
elseif is_plat("linux") then
	add_defines("KR_LINUX", "KR_UNIX")
elseif is_plat("freebsd") then
	add_defines("KR_FREEBSD", "KR_UNIX")
elseif is_plat("switch", "switchhb") then
	add_defines("KR_SWITCH", "KR_UNIX")
	if is_plat("switch") then
		set_toolchain("switch")
	else
		set_toolchain("devkitA64")
	end
elseif is_plat("psp", "ps3") then
    add_defines("KR_PLAYSTATION", "KR_UNIX")
    if is_plat("psp") then
        add_defines("KR_PSP")
    elseif is_plat("ps3") then
        add_defines("KR_PS3")
    end
elseif is_plat("xbox360") then
    add_defines("KR_WIN32", "KR_XBOX360")
	set_toolchain("xbox360")
end

if is_arch("x86") then
	add_defines("KR_X86")
elseif is_arch("x64", "amd64", "x86_64") then
	add_defines("KR_X86", "KR_AMD64")
elseif is_arch("arm64", "aarch64") then
	add_defines("KR_ARM", "KR_ARM64")
elseif is_arch("powerpc64") then
	add_defines("KR_PPC64")
end

if is_mode("debug") then
	add_defines("KR_DEBUG")
elseif is_mode("release") then
	add_defines("KR_RELEASE")
end

add_repositories("local-repo external")
add_requires("mimalloc_local", {configs = {pic = not is_plat("gdk", "gdkx")}})
add_requires("phnt_local", {configs = {pic = not is_plat("gdk", "gdkx")}})
add_requires("rtm_local", {configs = {pic = not is_plat("gdk", "gdkx")}})

add_includedirs(
	"external",
	"public"
)

includes("base")
includes("math")
includes("utility")
includes("texture")
includes("mesh")
includes("pack")
includes("launcher")

local tools = false
option("tools")
	set_default(false)
	after_check(function (option)
		tools = option:get()
	end)
option_end()

if tools then
	includes("tools")
else
	includes("engine")
end

