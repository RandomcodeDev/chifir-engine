local xbox360root = os.getenv("XEDK")

toolchain("xbox360")
    set_kind("standalone")

    set_sdkdir(path.join(xbox360root))
    set_bindir(path.join(xbox360root, "bin"))

    set_toolset("cc", path.join(xbox360root, "bin", "win32", "cl.exe"))
    set_toolset("cxx",path.join(xbox360root, "bin", "win32", "cl.exe"))
    set_toolset("ld", path.join(xbox360root, "bin", "win32", "link.exe"))
    set_toolset("sh", path.join(xbox360root, "bin", "win32", "link.exe"))
    set_toolset("ar", path.join(xbox360root, "bin", "win32", "link.exe"))
    set_toolset("as", path.join(xbox360root, "bin", "win32", "ml.exe"))

    on_load(function (toolchain)
        toolchain:add("cflags", "-TC")
        toolchain:add("cxxflags", "-TP")
        toolchain:add("cxflags", "-X")
        toolchain:add("cxflags", "-Dalignof=__alignof")
        toolchain:add("cxflags", "-D_XBOX")
--		toolchain:add("cxflags", "-D_WIN64")
        toolchain:add("ldflags", "-nodefaultlib")
        toolchain:add("ldflags", "-xex:no")

        for _, includedir in ipairs({
            path.join(xbox360root, "include", "xbox"),
            path.join(xbox360root, "Source", "crt")
        }) do
            toolchain:add("includedirs", includedir)
        end

        for _, libdir in ipairs({
            path.join(xbox360root, "lib", "xbox")
        }) do
            toolchain:add("libdirs", libdir)
        end
    end)
toolchain_end()
