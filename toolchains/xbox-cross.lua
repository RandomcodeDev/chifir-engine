toolchain("xbox-cross")
    set_kind("standalone")
    set_homepage("https://llvm.org/")
    set_description("A collection of modular and reusable compiler and toolchain technologies")
    set_runtimes("c++_static", "c++_shared", "stdc++_static", "stdc++_shared")

    set_toolset("cc",     "clang-cl")
    set_toolset("cxx",    "clang-cl")
    set_toolset("cpp",    "clang-cl -E")
	set_toolset("as",	  "clang")
    set_toolset("ld",     "lld-link")
    set_toolset("sh",     "lld-link")
    set_toolset("ar",     "llvm-ar")
    set_toolset("strip",  "llvm-strip")
    set_toolset("ranlib", "llvm-ranlib")
    set_toolset("objcopy","llvm-objcopy")
    set_toolset("mrc",    "llvm-rc")

	on_check(function (toolchain) return true end)

    on_load(function (toolchain)

        -- add march flags
        local march
        if toolchain:is_plat("xbox") and not is_host("windows") then
            -- cross-compilation for windows
            if toolchain:is_arch("i386", "x86") then
                march = "-target i386-pc-windows-msvc"
                toolchain:add("ldflags", "/safeseh:no")
                toolchain:add("shflags", "/safeseh:no")
            else
                march = "-target x86-pc-windows-msvc"
            end
        elseif toolchain:is_arch("i386", "x86") then
            march = "-m32"
        end

		if march then
			toolchain:add("cxflags", march)
			toolchain:add("asflags", march)
		end

		toolchain:add("shflags", "/subsystem:XBOX,1.00")
    end)
toolchain_end()
