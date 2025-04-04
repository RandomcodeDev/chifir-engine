# Чифир Engine

<img src="chifir.png" width="200" />

This is my new game engine. The main reason for this is that the renderer in the last one wasn't designed well from the
start, and other elements such as the build system became cumbersome. In this new engine, I'm hoping to avoid this.

## Information

I'm doing my best to document how I'm designing things in [`design/`](design/engine.typ). The renderer is going to be
designed around a render hardware interface, with more complex render passes on top of that, instead of implementing
a fixed function pipeline in each graphics API like before. Another notable feature of this iteration is that it uses
DLLs for each system, and the reason is so that the systems of the engine could be reused by tools that I might make,
and also because it makes it easier to separate functionality that shouldn't get tangled up.

The engine doesn't use the CRT or STL at all (where possible, meaning not at all on Windows and barely on Linux/Switch),
which does improve portability, especially between Windows versions. Some performance-critical functions are written to use
SIMD if the current CPU supports it (on x86, it's all dynamically enabled based on `cpuid` instead of being done at compile
time). I also wrote my own allocator that works pretty well, but I'm sure I'll have to optimize it eventually. I have my own
containers too.

## Build requirements

For every platform, you need [xmake](https://xmake.io).

- For Windows, you need Visual Studio 2022 (or any version with C++14 support). You can also build with Clang, but you still
  need VS. You also (will, in the future) need the GDK to build the x64 version.
- For Xbox 360, you need the SDK, it's on Internet Archive.
- For Switch, you need version 15.3.2 of the SDK, the private repo, and my private fork of xmake.
- For PS4, you need version 8.008 of the SDK, the private repo, and my private fork of xmake.
- For Linux, you need Clang and glibc. GCC isn't supported and isn't a priority (maybe I'll add support for MinGW so I can
  cross compile for Windows).

There are Rust bindings in progress, you need the `nightly` toolchain for them.

You can get a semi-accurate line count with this command if you have tokei installed:
```sh
tokei -t="C,C++,C Header,Python,Lua,Assembly" -e external -e public/xbox
```

## Build instructions

You'll need xmake. First, you should copy `config.default.lua` to `config.lua`, and change any important stuff.
Then, just run `xmake config -m debug`, followed by `xmake build`.

For the Rust stuff, build the engine, then do `cargo build`.

## Supported platforms

|       | Windows AMD64 | Windows x86 | Windows ARM64 | Xbox 360 | Nintendo Switch | PlayStation 4 | Linux |
|-------|---------------|-------------|---------------|----------|-----------------|---------------|-------|
| Clang | yes           | yes         | no            | no       | yes             | yes           | yes   |
| MSVC  | yes           | yes         | yes           | no       | no              |               | no    |

## External dependencies
See [`scripts/licenses.toml`](scripts/licenses.toml)
