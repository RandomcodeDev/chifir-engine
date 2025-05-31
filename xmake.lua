-- Root build script
-- Copyright 2025 Randomcode Developers

add_rules(
	"plugin.vsxmake.autoupdate"
--"c.unity_build",
--"c++.unity_build"
)

set_project("chifir-engine")
set_version("0.0.0", { build = "%Y%m%d%H%M%S" })

-- platforms, architectures, and modes

set_allowedplats(
	"windows", "scarlett", "xbox",
	"linux",
	"nx",
	"orbis"
)

set_allowedarchs(
	"windows|x64", "scarlett|x64", "orbis|x64",
	"linux|x86_64",
	"windows|x86", "xbox|x86", "linux|x86",
	"windows|arm64", "linux|arm64", "linux|arm64-v8a", "nx|arm64"
)

set_allowedmodes(
	"debug",
	"release",
	"retail"
)

if is_plat("xbox", "nx", "orbis") then
	set_kind("static")
end

xdk = os.getenv("XDK")

if is_plat("xbox") then
	if os.exists(xdk) then
		print("using Xbox SDK in " .. xdk)
	elseif os.exists("external/xbox") then
		print("using Xbox source code in external/xbox")
	else
		print("ERROR: missing Xbox headers and libraries, check the instructions")
        die()
	end
end

-- main include directories

add_sysincludedirs(
	"external",
	"external/directx",
	"external/stb",
	"external/volk",
	"external/vulkan"
)

if is_plat("xbox") then
    add_sysincludedirs(
        "public/xbox"
    )
	if os.exists(xdk) then
		add_sysincludedirs(
			"$(env XDK)/xbox/include"
		)
	else
		add_sysincludedirs(
			"external/xbox/inc"
		)
	end
else
	add_sysincludedirs(
		"external/d3d8",
		"external/phnt"
	)
end

if is_plat("windows") and not is_host("windows") then
	add_sysincludedirs(
		"external/winsdk/include/clang",
		"external/winsdk/include/msvc",
		"external/winsdk/include/ucrt",
		"external/winsdk/include/um",
		"external/winsdk/include/shared",
		"external/winsdk/include/winrt"
	)
end

add_includedirs(
	"public",
	"$(builddir)/config"
)

if os.exists("private") then
	add_includedirs(
		"private/public",
		"private"
	)
end

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

includes("toolchains/windows-cross.lua")
includes("toolchains/xbox-cross.lua")

if os.exists("config.lua") then
	includes("config.lua")
else
	includes("config.default.lua")
end

-- preprocessor stuff

set_configvar("GAME_NAME", game_name)
set_configvar("GAME_DISPLAY_NAME", game_display_name)
set_configvar("REPO_NAME", repo_name)
set_configvar("NX_TITLE_ID", nx_title_id)

set_configdir("$(builddir)/config")
add_configfiles("public/config.h.in")

vulkan = is_plat("windows", "linux", "nx")
if vulkan then
	add_defines("CH_VULKAN")
end

directx = is_plat("windows", "scarlett")
if directx then
	add_defines("CH_DIRECTX", "CH_DIRECTX12")
end

directx8 = is_plat("windows", "xbox")
if directx8 then
	add_defines("CH_DIRECTX8")
end

directx9 = is_plat("windows")
if directx9 then
	add_defines("CH_DIRECTX9")
end

if is_kind("static") then
	add_defines("CH_STATIC")
end

if is_plat("windows") then
	add_defines("CH_WIN32", "CH_UWP")
	if is_arch("x64", "x86_64") then
		add_defines("CH_GDK")
	end
elseif is_plat("xbox") then
	add_defines("CH_XBOX", "CH_CONSOLE")
elseif is_plat("scarlett") then
	add_defines("CH_SCARLETT", "CH_GDK", "CH_WIN32", "CH_CONSOLE")
elseif is_plat("linux") then
	add_defines("CH_LINUX", "CH_UNIX")
	add_requires("libsdl3")
elseif is_plat("nx") then
	add_defines("CH_NX", "CH_CONSOLE")
elseif is_plat("orbis") then
	add_defines("CH_ORBIS", "CH_CONSOLE")
end

if is_arch("x64", "x86_64") then
	add_defines("CH_AMD64", "CH_X86", "CH_SIMD128", "CH_SIMD256", "CH_SIMD128_COMPARE", "_AMD64_", "CH_KNOWN_SIMD128", "CH_KNOWN_SIMD256")
elseif is_arch("x86") then
	add_defines("CH_IA32", "CH_X86", "CH_SIMD128", "_X86_")
	if is_plat("windows") then
		add_defines("CH_SIMD256", "CH_SIMD128_COMPARE")
    elseif is_plat("xbox") then
		add_defines("CH_KNOWN_SIMD128")
    end
elseif is_arch("arm64", "arm64-v8a") then
	add_defines("CH_ARM64", "CH_SIMD128", "CH_SIMD256", "CH_SIMD128_COMPARE", "_ARM64_", "CH_KNOWN_SIMD128", "CH_KNOWN_SIMD256")
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

if is_arch("x64", "x86", "arm64", "arm64-v8a") then
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

-- make msvcrt and glibc behave
add_defines("_GNU_SOURCE", "_CRT_SECURE_NO_WARNINGS")

set_languages("gnu17", "gnuxx20")
set_warnings("all", "error")

-- compiler/linker flags to make this ungodly nightmare work
if is_plat("windows", "scarlett", "xbox") then
	set_exceptions("none")

	add_cxflags(
		"/Zl",				 -- prevent C runtime from being linked
		"/GS-",				-- prevent buffer checks
		"/GR-",				-- no RTTI
		"/Zc:__cplusplus",
		"/Zc:threadSafeInit-", -- idk how to implement this, just gonna do it the old fashioned way

		"/wd4201",			 -- nameless struct
		"/wd4324",			 -- structure padded due to alignment specifier
		"/wd4505",			 -- unreferenced static function removed
		"/wd4005",			 -- macro redefinition
		"/wd4211",			 -- nonstandard extension used
		"/wd4127",			 -- conditional expression is constant
		"/wd4229",			 -- modifiers on data are ignored
		"/wd4319",			 -- zero extending 'x' to 'y' of greater type
		"/wd4163",			 -- 'x': not available as an intrinsic function
		"/wd4164",			 -- 'x': intrinsic function not declared
		"/wd4125",			 -- decimal digit terminates octal escape sequence
		"/wd4464",			 -- relative include path contains '..'
		"/wd5247",			 -- section 'x' is reserved for C++ dynamic initialization. Manually creating the section will interfere with C++ dynamic initialization and may lead to undefined behavior
		"/wd5248",			 -- section 'x' is reserved for C++ dynamic initialization. Variables manually put into the section may be optimized out and their order relative to compiler generated dynamic initializers is unspecified
		"/wd4820",			 -- 'x' bytes padding added after data member 'y'
		"/wd5045",			 -- Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified
		"/wd4273",			   -- 'x': inconsistent DLL linkage
	{force = true})

	if is_toolchain("clang", "clang-cl", "windows-cross", "xbox-cross") then
		add_cxflags(
			"-Wno-nonportable-include-path", -- None of this is on non-Windows code I control"
		{force = true})
	else
		add_cxflags(
			"/Zc:preprocessor",	-- for __VA_OPT__
		{force = true})
	end

	-- Modern Xbox is all AMD
	if is_plat("scarlett") then
		if is_toolchain("msvc") then
			add_cxflags(
				"/favor:AMD64",
			{force = true})
		end
	end

	if is_plat("xbox") then
		add_cxflags(
			"/arch:SSE",
			"/X",
			"/wd5040", -- dynamic exception specifications are valid only in C++14 and earlier; treating as noexcept(false)
		{force = true})

		add_ldflags(
			"/FORCE:MULTIPLE",
			"/MAP",
			"/SAFESEH:NO",
		{force = true})

		if os.exists(xdk) then
			add_linkdirs(
				"$(env XDK)/xbox/lib"
			)
		else
			add_linkdirs(
				"external/xbox/lib"
			)
		end
	else
		if is_arch("x64", "x86_64") then
			add_linkdirs("public/win32/x64")
			if is_toolchain("clang-cl") then
				add_cxflags(
					-- Tune AMD64 builds for newer CPUs
					"-march=x86-64-v3",
				{force = true})
			end
		elseif is_arch("x86") then
            -- Let x86 builds run on Pentiums
			add_cxflags(
				"/arch:IA32"
			)
			add_linkdirs("public/win32/x86")
		elseif is_arch("arm64") then
			add_linkdirs("public/win32/arm64")
		end
	end

	add_shflags(
		"/nodefaultlib", -- further prevent C runtime
		{force = true})
	add_ldflags(
		"/nodefaultlib", -- further prevent C runtime
		{force = true})

	-- windows cross compilation requires the real sdk, mingw's headers don't work with phnt and gcc is garbage
	if not is_host("windows") then
		if is_plat("windows", "scarlett") then
			add_defines("CH_WIN32_CROSS")
			if is_arch("x86_64", "x64") then
				add_linkdirs(
					"external/winsdk/lib/um/x64"
				)
			elseif is_arch("x86") then
				add_linkdirs(
					"external/winsdk/lib/um/x86"
				)
			end
		elseif is_plat("xbox") then
			add_defines("CH_XBOX_CROSS")
		end

		add_cxflags(
			"/X",
			"-clang:-ffreestanding",
			"-clang:-Wno-ignored-pragma-intrinsic"
		)
	end
elseif is_plat("linux", "nx", "orbis") then
	add_cxflags(
		"-fms-extensions",
		"-fno-threadsafe-statics",
		"-fno-rtti",					   -- no RTTI
		"-working-directory=$(scriptdir)", -- to fix __FILE__

		"-Wno-unknown-warning-option",
		"-Wno-#pragma-messages",
		"-Wno-nonportable-include-path", -- None of this is on non-Windows code I control
		"-Wno-macro-redefined",		  -- This is avoided in any scenario where it matters
		"-Wno-unused-function",		  -- I don't care, that's the linker's problem
		"-Wno-unknown-attributes",
		"-Wno-ignored-pragma-intrinsic",
		"-Wno-ignored-attributes",
		"-Wno-frame-address",
		{force = true})

	if is_arch("arm64", "arm64-v8a") then
		add_cxflags(
			"-mfloat-abi=hard",
			{force = true})
	end

	if is_plat("linux") then
		add_ldflags(
			"-fuse-ld=lld",
			{force = true})
		add_shflags(
			"-fuse-ld=lld",
			{force = true})

		if is_arch("x64", "x86_64") then
			add_cxflags(
				"-march=x86-64-v3",
				{force = true})
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
