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

Next, run `xmake config` with these flags (you have to pass all of them or it will seem like it works and then not build):

- `-p` sets the platform (required):
  - `windows` Windows/PC GDK
  - `linux` Linux
  - `gdkx` Xbox One/Series X|S (private)
  - `switch` Nintendo Switch (private)
  - `orbis` PlayStation 4 (private)
- `-a` sets the architecture (required):
  - `x64` AMD64, supported on `windows`, `linux`, `gdkx`, `orbis`
  - `x86` IA-32, supported on `windows` and `linux`
  - `arm64` ARM64, supported on `linux` and `switch`
- `-m` sets the build mode (required):
  - `debug` lots of logs, symbols, no optimization, debugging features
  - `release` reasonable amount of logs, symbols, full optimization, debugging features
  - `retail` reasonable amount of logs, no symbols, full optimization
- `-k` sets whether to build static or dynamic libraries (required):
  - `static` everything is statically linked, supported on `windows`, `linux`, `gdkx`, `switch`, `orbis`
  - `shared` everything is dynamically linked (preferred), supported on `windows`, `linux`, `gdkx`
- `--toolchain` sets what compiler/linker to use (required)
  - `msvc` MSVC is the most functional toolchain for `windows`/`gdkx`
  - `clang-cl` Clang usually works for `windows`/`gdkx`, but it breaks sometimes
  - `clang`/`llvm` Clang is the only supported compiler on `linux`
  - `nx-clang` Nintendo Switch SDK version of Clang and LLVM (use this for `switch`)
  - `orbis-clang` PS4 Clang (use this for `orbis`)
- `--vs` Sets the Visual Studio version to generate wrapper projects for (optional)

For the Rust stuff, build the engine, then do `cargo build`.

## Supported platforms

- Windows
  - Windows is the most actively developed platform
  - Windows XP and later are supported (although the renderer doesn't support DirectX 9 yet)
  - UWP support is in progress
  - Nothing is statically imported from system libraries, it's all done at runtime
  - Most features are implemented with native functions in `ntdll.dll` that have been around since Windows XP, rather than using `kernel32.dll`
- Linux
  - Linux is usually mostly up to date with Windows
  - glibc is the only libc I've tested with
  - I haven't decided how to deal with X11/Wayland yet
  - Only Clang is supported, GCC is a weird compiler and I don't like it
- Nintendo Switch
  - Mostly works, crashes in Vulkan init code
  - I might work on a version using libnx and deko3d eventually
- PlayStation 4
  - Not done implementing the platform abstraction code yet
  - I might make a homebrew version eventually
  - My PS4 is on 11.02, so I'm stuck with shadPS4 for now
- Xbox One/Series X|S
  - Once UWP support is done, it'll work in the retail dev mode
  - I don't have a devkit or an OS version supported by the Collateral Damage exploit
- Xbox 360
  - I did write some code for it, but eventually I decided that C++03 sucks
  - If I find a way to make a modern compiler target it, I'll add support again
  - Will require DirectX 9, similar to Windows XP

## External dependencies
See [`scripts/licenses.toml`](scripts/licenses.toml)
