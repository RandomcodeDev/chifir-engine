target("mimalloc_local")
	set_kind("static")

	add_headerfiles(
		path.join("include", "mimalloc.h"),
		path.join("include", "mimalloc", "atomic.h"),
		path.join("include", "mimalloc", "internal.h"),
		path.join("include", "mimalloc", "prim.h"),
		path.join("include", "mimalloc", "track.h"),
		path.join("include", "mimalloc", "types.h")
	)
	add_files(
		path.join("src", "alloc.c"),
		path.join("src", "alloc-aligned.c"),
		path.join("src", "alloc-posix.c"),
		path.join("src", "arena.c"),
		path.join("src", "bitmap.c"),
		path.join("src", "heap.c"),
		path.join("src", "init.c"),
		path.join("src", "libc.c"),
		path.join("src", "options.c"),
		path.join("src", "os.c"),
		path.join("src", "page.c"),
		path.join("src", "random.c"),
		path.join("src", "segment.c"),
		path.join("src", "segment-map.c"),
		path.join("src", "stats.c"),
		path.join("src", "prim", "prim.c")
	)

	if is_plat("gdk", "gdkx") then
		set_prefixname("")
		set_extension(".lib")
	end

	add_includedirs("include")

	if is_mode("debug") then
		add_defines("MI_DEBUG=3", "MI_SECURE=4")
	end

	set_warnings("none")
target_end()
