-- Root build script
-- Copyright 2025 Randomcode Developers

add_rules(
	"plugin.vsxmake.autoupdate"
--"c.unity_build",
--"c++.unity_build"
)

set_project("chifir-engine")
set_version("0.0.0", { build = "%Y%m%d%H%M%S" })

set_allowedplats(
	"windows", "gdkx",
	"linux",
	"switch",
	"orbis"
)

set_allowedarchs(
	"x64", "x86", "arm64"
)

set_allowedmodes(
	"debug",
	"release",
	"retail"
)

if is_plat("switch", "orbis") then
	set_kind("static")
end

add_sysincludedirs(
	"external",
	"external/directx",
	"external/phnt",
	"external/stb",
	"external/volk",
	"external/vulkan",
	"public/xbox"
)

add_includedirs(
	"public",
	"private/public",
	"private"
)

function get_toolchain()
	return get_config("toolchain")
end

function is_toolchain(...)
	local arg = { ... }
	for i, v in ipairs(arg) do
		if get_toolchain() == v then
			return true
		end
	end

	return false
end

if os.exists("config.lua") then
	includes("config.lua")
else
	includes("config.default.lua")
end
add_defines("GAME_NAME=\"" .. game_name .. "\"")
add_defines("GAME_DISPLAY_NAME=\"" .. game_display_name .. "\"")
add_defines("REPO_NAME=\"" .. repo_name .. "\"")

vulkan = is_plat("windows", "linux", "switch")
if vulkan then
	add_defines("CH_VULKAN")
end

directx = is_plat("windows", "gdkx")
if directx then
	add_defines("CH_DIRECTX", "CH_DIRECTX12")
end

if is_kind("static") then
	add_defines("CH_STATIC")
end

if is_plat("windows") then
	add_defines("CH_WIN32")
	if is_arch("x64") then
		add_defines("CH_GDK")
	end
elseif is_plat("gdkx") then
	add_defines("CH_GDKX", "CH_GDK", "CH_WIN32", "CH_CONSOLE", "CH_XBOX")
elseif is_plat("linux") then
	add_defines("CH_LINUX", "CH_UNIX")
elseif is_plat("switch") then
	add_defines("CH_SWITCH", "CH_CONSOLE")
elseif is_plat("orbis") then
	add_defines("CH_ORBIS", "CH_CONSOLE")
end

if is_arch("x64") then
	add_defines("CH_AMD64", "CH_X86", "CH_SIMD128", "CH_SIMD256")
elseif is_arch("x86") then
	add_defines("CH_IA32", "CH_X86", "CH_SIMD128", "CH_SIMD256")
elseif is_arch("arm64") then
	add_defines("CH_ARM64", "CH_SIMD128", "CH_SIMD256")
end

if is_mode("debug") then
	add_defines("CH_DEBUG")
	set_symbols("debug")
	set_optimize("none")
elseif is_mode("release") then
	add_defines("CH_RELEASE")
	set_symbols("debug")
	set_optimize("fastest")
	set_strip("all")
elseif is_mode("retail") then
	add_defines("CH_RETAIL", "CH_RELEASE")
	set_symbols("none")
	set_optimize("fastest")
	set_strip("all")
end

if is_arch("x64", "x86", "arm64") then
	add_defines("CH_LITTLE_ENDIAN")
else
	add_defines("CH_BIG_ENDIAN")
end

if os.exists("private/xmake.lua") then
	includes("private")
else
	function add_private_settings() end

	function add_private_launcher_settings() end
end

add_private_settings()

add_defines("_GNU_SOURCE", "_CRT_SECURE_NO_WARNINGS")

set_languages("gnu17", "gnuxx17")
set_warnings("all", "error")

if is_plat("windows", "gdkx") then
	set_exceptions("none")

	add_cxflags(
		"/Zl",                 -- prevent C runtime from being linked
		"/GS-",                -- prevent buffer checks
		"/GR-",                -- no RTTI
		"/Zc:__cplusplus",
		"/Zc:threadSafeInit-", -- idk how to implement this, just gonna do it the old fashioned way

		"/wd4201",             -- nameless struct
		"/wd4324",             -- structure padded due to alignment specifier
		"/wd4505",             -- unreferenced static function removed
		"/wd4005",             -- macro redefinition
		"/wd4211",             -- nonstandard extension used
		"/wd4127",             -- conditional expression is constant
		"/wd4229",             -- modifiers on data are ignored
		"/wd4319",             -- zero extending 'x' to 'y' of greater type
		"/wd4163",             -- 'x': not available as an intrinsic function
		"/wd4164",             -- 'x': intrinsic function not declared
		"/wd4125",             -- decimal digit terminates octal escape sequence
		"/wd4464",             -- relative include path contains '..'
		"/wd5247",             -- section 'x' is reserved for C++ dynamic initialization. Manually creating the section will interfere with C++ dynamic initialization and may lead to undefined behavior
		"/wd5248",             -- section 'x' is reserved for C++ dynamic initialization. Variables manually put into the section may be optimized out and their order relative to compiler generated dynamic initializers is unspecified
		"/wd4820",             -- 'x' bytes padding added after data member 'y'
		"/wd5045",             -- Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified
		{ force = true })

	-- Modern Xbox is all AMD
	if is_plat("gdkx") then
		add_cxflags(
			"/favor:AMD64",
			{ force = true })
	end

	-- /arch:SSE2 is the default for x86, SSE2 is the baseline for AMD64, but for x86, it needs
	-- to be turned down to IA32 (I'm insane/stupid enough to eventually try to make this run on
	-- a Pentium, which I do have, or something dumb like that)
	if is_arch("x64") then
		add_linkdirs("public/win32/x64")
		if is_toolchain("clang-cl") then
			add_cxflags(
				"-march=x86-64-v3",
				{ force = true })
		end
	elseif is_arch("x86") then
		add_cxflags(
			"/arch:IA32"
		)
		add_linkdirs("public/win32/x86")
	elseif is_arch("arm64") then
		add_linkdirs("public/win32/arm64")
	end

	add_shflags(
		"/nodefaultlib", -- further prevent C runtime
		{ force = true })
	add_ldflags(
		"/nodefaultlib", -- further prevent C runtime
		{ force = true })
elseif is_plat("linux", "switch", "orbis") then
	add_cxflags(
		"-fms-extensions",
		"-fno-threadsafe-statics",
		"-fno-rtti",                       -- no RTTI
		"-working-directory=$(scriptdir)", -- to fix __FILE__

		"-Wno-unknown-warning-option",
		"-Wno-#pragma-messages",
		"-Wno-nonportable-include-path", -- None of this is on non-Windows code I control
		"-Wno-macro-redefined",          -- This is avoided in any scenario where it matters
		"-Wno-unused-function",          -- I don't care, that's the linker's problem
		"-Wno-unknown-attributes",
		"-Wno-ignored-pragma-intrinsic",
		"-Wno-ignored-attributes",
		"-Wno-frame-address",
		{ force = true })

	if is_arch("arm64") then
		add_cxflags(
			"-mfloat-abi=hard",
			{ force = true })
	end

	if is_plat("linux") then
		add_ldflags(
			"-fuse-ld=lld",
			{ force = true })
		add_shflags(
			"-fuse-ld=lld",
			{ force = true })

		if is_arch("x64") then
			add_cxflags(
				"-march=x86-64-v3",
				{ force = true })
		end
	end
end

includes("base")
includes("engine")
includes("testapp")
includes("rendersystem")
includes("rhi")
includes("videosystem")
includes("launcher")

target("public")
set_kind("headeronly")

add_headerfiles("external/**", "public/**", "private/public/**")
