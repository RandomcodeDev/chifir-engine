root = ".."
includes(root)

set_project("Красний")
set_version("0.0.0", {build = "%Y%m%d%H%M"})

set_allowedplats(
	"gdk", "gdkx", "xbox360", "windows",
	"linux",
	"switch", "switchhb"
)
set_allowedarchs(
	"gdk|x64", "gdkx|x64", "xbox360|powerpc64", "windows|x86",
	"switch|arm64", "switchhb|arm64"
)

if is_toolchain("msvc", "clang-cl", "xbox360") then
	add_cxflags("-Zl") -- omit default library name in object file
	add_ldflags("-nodefaultlib") -- disable default libraries
else
	add_cxflags("-fshort-wchar", "-ffreestanding")
	add_ldflags("-nostdlib") -- disable linking to C runtime
	add_shflags("-nostdlib")
end

local directx12 = is_plat("gdk", "gdkx", "windows")
local directx9 = is_plat("gdk", "xbox360", "windows")
local vulkan = is_plat("gdk", "windows", "linux", "switch")

local discord = is_plat("gdk", "gdkx", "windows", "linux")

includes("../rendersystem")
includes("../inputsystem")
includes("../uisystem")
includes("../physicssystem")
includes("../animationsystem")
includes("../audiosystem")
includes("../game/shared")
includes("../game/client")
includes("../game/server")
