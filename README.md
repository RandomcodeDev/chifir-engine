# Чифир Engine
This is my new game engine. The main reason for this is that the renderer in the last one wasn't designed well from the
start, and other elements such as the build system became cumbersome. In this new engine, I'm hoping to avoid this.

## Information
I'm doing my best to document how I'm designing things in [`design/`](design/engine.typ). The renderer is going to be
designed around a render hardware interface, with more complex render passes on top of that, instead of implementing
a fixed function pipeline in each graphics API like before. This engine also uses [FASTBuild](https://fastbuild.org),
which is much more verbose than some build systems, but I've found it to be amazing to work with. Another notable feature
of this iteration is that it uses DLLs for each system, and the reason is so that the systems of the engine could be used
by tools that I might make, and also because it makes it easier to separate functionality that shouldn't get tangled up.

The engine doesn't use the CRT or STL at all, which does improve portability. Some performance-critical
functions are written to use SIMD if the current CPU supports it (on x86, most of that isn't baked in at compile time, and
`cpuid` is used). I also wrote my own allocator that works pretty well, but I think there are still issues with its bookkeeping.

## Build instructions
First, you should copy `fastbuild/Config.Default.bff` to `fastbuild/Config.bff`, and change any paths/version numbers. I
might automate this in the future, but it's probably not necessary. You need FASTBuild, download a binary from the website
or use the Windows binary in `tools/`. After that, you can run `fbuild`. Another useful command is `fbuild solution` to
generate a Visual Studio solution.

## Supported platforms
So far, 64-bit and 32-bit Windows with MSVC or Clang, Xbox 360 with MSVC, and Nintendo Switch with Clang are supported. I've
tested the 32-bit binaries as far back as Windows XP in Qemu (I have a Compaq Deskpro 2000 with a Pentium and some Medion laptop
with a Celeron M from 2004, and some other old PC with a Pentium something that has an MMX sticker on the front, and I do intend
to test on them eventually) and they run fine, but because I'm focusing on modern graphics APIs and systems first, that's going
to change soon.
