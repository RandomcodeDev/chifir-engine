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
