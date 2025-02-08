target("Base")
    add_defines("IN_BASE")

    add_headerfiles("**.h", "../public/base/**.h")
    add_files(
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

        "../public/base/memoverride.cpp",
        "../public/base/libc.cpp"
    )

    if is_toolchain("clang", "nx-clang") then
        add_files("../public/base/compiler_clang.cpp")
    end

    if is_plat("windows", "gdkx") then
        add_files(
            "filesystem_win32.cpp",
            "loader_win32.cpp",
            "platform_win32.cpp",
            "../public/base/compiler_msvc.cpp",
            "../public/base/dllmain.cpp"
        )

        if is_arch("x64") then
			add_headerfiles("../public/base/compiler_msvc_amd64.asm", "stubs_win32_amd64.asm")
            add_files("../public/base/compiler_msvc_amd64.asm", "stubs_win32_amd64.asm")
        elseif is_arch("x86") then
			add_headerfiles("../public/base/compiler_msvc_x86.asm", "stubs_win32_x86.asm")
            add_files("../public/base/compiler_msvc_x86.asm", "stubs_win32_x86.asm")
        end
    elseif is_plat("linux") then
        add_files(
            "filesystem_unix.cpp",
            "platform_unix.cpp"
        )
    elseif is_plat("switch") then
        add_files(
            "../private/base/filesystem_switch.cpp",
            "../private/base/platform_switch.cpp"
        )
    end
target_end()

target("CommonFiles")
    set_kind("static")

    add_files(
        "../public/base/memoverride.cpp",
        "../public/base/libc.cpp"
    )

    if is_toolchain("clang", "nx-clang") then
        add_files("../public/base/compiler_clang.cpp")
    end

    if is_plat("windows", "gdkx") then
        add_files("../public/base/compiler_msvc.cpp")

        if is_arch("x64") then
            add_files("../public/base/compiler_msvc_amd64.asm")
        elseif is_arch("x86") then
            add_files("../public/base/compiler_msvc_x86.asm")
        end
    end
target_end()

target("DllSupport")
    set_kind("static")

    if is_plat("windows", "gdkx") then
        add_files("../public/base/dllmain.cpp")
    else
        add_files("../public/dummy.cpp")
    end
target_end()
