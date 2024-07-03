includes(path.join(root, "scripts/util.lua"))
includes(path.join(root, "scripts/xbox360.lua"))

add_rules(
	"mode.debug",
	"mode.release",
	"plugin.vsxmake.autoupdate"
)

set_policy("check.auto_ignore_flags", false)

--set_warnings("everything")

set_exceptions("no-cxx")

-- Microsoft no longer supports anything below C++14, so Clang can't compile the system headers in that mode
if is_plat("gdk", "gdkx", "windows") then
	set_languages("gnu89", "cxx14")
else
	set_languages("gnu89", "cxx03")
end

add_defines("_CRT_SECURE_NO_WARNINGS", "_POSIX_C_SOURCE=200809")

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
elseif is_plat("psp", "ps3") then
    add_defines("KR_PLAYSTATION", "KR_UNIX")
    if is_plat("psp") then
        add_defines("KR_PSP")
    elseif is_plat("ps3") then
        add_defines("KR_PS3")
    end
elseif is_plat("xbox360") then
    add_defines("KR_WIN32", "KR_XBOX360")
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

add_repositories("local-repo " .. path.join(root, "external"))
add_requires("phnt_local")

add_includedirs(
	path.join(root, "external"),
	path.join(root, "public")
)

if is_plat("xbox360") then
	add_includedirs(
		path.join(root, "external/xbox")
	)
end

includes(path.join(root, "base/xmake.lua"))
includes(path.join(root, "math/xmake.lua"))
includes(path.join(root, "utility/xmake.lua"))
includes(path.join(root, "texture/xmake.lua"))
includes(path.join(root, "mesh/xmake.lua"))
includes(path.join(root, "pack/xmake.lua"))
includes(path.join(root, "launcher/xmake.lua"))

