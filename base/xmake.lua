-- Base build script
-- Copyright 2025 Randomcode Developers

target("Base")
    set_kind("$(kind)")

    add_defines("IN_BASE")

    add_includedirs(".")

    add_headerfiles("**.h", "../private/base/**.h", "../public/base/**.h", "**.inc")
    add_files(
        "async.cpp",
        "base.cpp",
        "basicstr.cpp",
        "filesystem.cpp",
        "filesystem_raw.cpp",
        "loader.cpp",
        "log.cpp",
        "memory.cpp",
        "platform.cpp",
        "stb.cpp",
        "string.cpp",

        "../public/base/dllmain.cpp",
        "../public/base/memoverride.cpp",
        "../public/base/libc.cpp"
    )

    if is_toolchain("clang", "llvm", "nx-clang", "orbis-clang", "clang-cl") then
        add_files("../public/base/compiler_clang.cpp")
    end

    if is_plat("windows", "scarlett") then
        add_files(
            "args_win32.cpp",
			"async_win32.cpp",
            "filesystem_win32.cpp",
            "loader_win32.cpp",
            "platform_win32.cpp",
            "../public/base/compiler_msvc.cpp"
        )

        if is_plat("windows") then
            add_files("platform_winrt.cpp")
        end

		if is_host("windows") then
	        if is_arch("x64", "x86_64") then
				add_headerfiles("../public/base/compiler_msvc_amd64.asm", "stubs_win32_amd64.asm", "stubs_win32_amd64.inc")
			    add_files("../public/base/compiler_msvc_amd64.asm", "stubs_win32_amd64.asm")
	        elseif is_arch("x86") then
				add_headerfiles("../public/base/compiler_msvc_x86.asm", "stubs_win32_x86.asm", "stubs_win32_x86.inc")
	            add_files("../public/base/compiler_msvc_x86.asm", "stubs_win32_x86.asm")
	        elseif is_arch("arm64") then
				add_headerfiles("../public/base/compiler_msvc_arm64.asm", "stubs_win32_arm64.asm")
	            add_files("../public/base/compiler_msvc_arm64.asm", "stubs_win32_arm64.asm")
		    end
		else
			if is_arch("x64", "x86_64") then
				add_headerfiles("../public/base/compiler_msvc_amd64.S", "stubs_win32_amd64.S", "stubs_win32_amd64.inc")
			    add_files("../public/base/compiler_msvc_amd64.S", "stubs_win32_amd64.S")
	        elseif is_arch("x86") then
				add_headerfiles("../public/base/compiler_msvc_x86.S", "stubs_win32_x86.S", "stubs_win32_x86.inc")
	            add_files("../public/base/compiler_msvc_x86.S", "stubs_win32_x86.S")
	        elseif is_arch("arm64") then
				add_headerfiles("../public/base/compiler_msvc_arm64.S", "stubs_win32_arm64.S")
	            add_files("../public/base/compiler_msvc_arm64.S", "stubs_win32_arm64.S")
		    end
		end
	elseif is_plat("xbox") then
		add_files(
			"async_xbox.cpp",
			"filesystem_xbox.cpp",
			"platform_xbox.cpp"
		)
    elseif is_plat("linux") then
        add_files(
            "async_unix.cpp",
            "filesystem_unix.cpp",
            "platform_unix.cpp"
        )
    elseif is_plat("nx") then
        add_files(
            "../private/base/filesystem_nx.cpp",
            "../private/base/platform_nx.cpp"
        )
    elseif is_plat("orbis") then
        add_files(
            "../private/base/filesystem_orbis.cpp",
            "../private/base/platform_orbis.cpp"
        )
    end
target_end()

target("CommonFiles")
    set_kind("static")

    add_includedirs(".")

    add_files(
        "../public/base/memoverride.cpp",
        "../public/base/libc.cpp"
    )

    if is_toolchain("clang", "llvm", "nx-clang", "orbis-clang", "clang-cl") then
        add_files("../public/base/compiler_clang.cpp")
    end

    if is_plat("windows", "scarlett") then
        add_files("../public/base/compiler_msvc.cpp")

		if is_host("windows") then
			if is_arch("x64", "x86_64") then
		        add_files("../public/base/compiler_msvc_amd64.asm")
	        elseif is_arch("x86") then
			    add_files("../public/base/compiler_msvc_x86.asm")
		    end
		else
			if is_arch("x64", "x86_64") then
		        add_files("../public/base/compiler_msvc_amd64.S")
	        elseif is_arch("x86") then
			    add_files("../public/base/compiler_msvc_x86.S")
		    end
		end
    end
target_end()

target("DllSupport")
    set_kind("static")

    add_files("../public/base/dllmain.cpp")
target_end()
