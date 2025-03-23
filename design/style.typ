= Engine coding style
The engine is written in C++17 with no STL and (almost) no C runtime. This comes with some advantages, but plenty of disadvantages
as well.
#table(
  columns: 2,
  [*Pros*], [*Cons*],
  [Full control over nearly everything], [Have to implement everything],
  [_Very_ portable, almost everything that can possibly run the engine has a C++14 compiler],
    [The Xbox 360 is a platform I'd really like to support, but it only has a C++03 TR1/C89 compiler],
  [Only the exact functionality needed is implemented], [Features not built on existing support code take longer],
)

Just try to copy the existing style as much as possible and you'll be fine. Don't reformat external code.

== Colourful language
You shouldn't be mean to anyone, but you can swear a little and be snarky. You can complain about something that gave you a headache
within reason.

== Documentation
You should try to document everything important. Any public declarations, macros, `constexpr` functions, any weird decisions,
folders with weird stuff in them, anything where someone might wonder what it is and how it works/why it's there. For implementations
of interfaces/abstract classes, you don't have to document inherited functions. TLDR, just document stuff in a header file and anything
complicated/weird.

== External code
Always document where external code is from, and avoid introducing it. Put it in `external` when possible, preferrably as
a Git submodule unless it's only a few files. Always include licenses, and add information to `scripts/licenses.toml`.

== File header
Put a very short comment followed by a copyright line (usually for Randomcode Developers). Keep track of where files outside
`external` come from, and add a license in `external` and `scripts/licenses.toml` for loose code (i.e. SDL/musl).

Example:
```c++
/// \file <file description>
/// \author Randomcode Developers
```

== Formatting
Just use `clang-format` aggressively. The only thing that I'm pretty sure it doesn't do is adding curly brackets to one-line
if statements and loops, which is part of the style. Also, be careful about putting a blank line between headers that shouldn't
be sorted alphabetically.

== `#include`s
Separate by folder, sort alphabetically when possible

== Types
`public/base/types.h` defines short type names largely based on Rust's type names. Sizes should use the signed `ssize` to make
calculation errors easier to see, and the fact that it's a size already gives the indication it can't be negative. Any other
type can be unsigned, and for certain things like `operator new()` where using `usize` is required, that's fine too.

== Comments
Comments should explain what code does. At the top of a file that implements something complex, or the main header for a whole
component, explain the overall design of the component, any important choices and the reasoning, and whatever limitations exist.
Additionally, when functions are complex, add comments explaining what's happening/why it's happening. The memory manager in
`base/memory.cpp` and the Windows loader in `base/loader_win32.cpp` are the best example of commenting things so far.

== Naming
Variables are camel case, prefixed with `m_` for private/protected members, `g_` for globals, and `s_` for static globals, and `f_` for (some)
function pointers. Types are Pascal case, prefixed with `C` for classes, `CBase` for abstract classes, and `I` for interfaces, while structs are
suffixed with `_t`. Functions are Pascal case, with the name of their component and an underscore as a prefix, like `Base_`. Common abbreviations
(like str, len, max, min, alloc, buf, src, dest, common acronyms) are allowed, but obscure ones should be avoided. Try to balance clearness with
succintness when naming variables, so they're easier to type but you can still easily recognize them.

== Classes vs structs
Classes do things, structs store data (you can have a destructor in them though). That's the distinction so far.

== Headers
Public headers (ones visible to any component) should include as few headers as possible, and forward declare types where needed.
In `.cpp` files, all headers for the things used should be included, not just ones that happen to include the right things. Private
headers are more free to include things, and have references to globals inside components, like `base/base.h`.

== Standard library replacement
Because the C runtime and STL aren't used, there are some replacements for the commonly used stuff, and there are also utility
functions commonly implemented on top of these, like automatically allocating a buffer for snprintf.

In terms of replacements for the CRT, `public/base/base.h` has `Base_Alloc`, `Base_MemSet`, `Base_MemCopy`, and `Base_MemCompare`, and
`public/base/basicstr.h` has `Base_StrFormat`, `Base_StrCopy`, `Base_StrClone`, and `Base_StrCompare`. They work basically just like
`malloc`, `memset`, `memcpy`/`memmove`, `memcmp`, `snprintf`, `strcopy`, `strdup`, and `strcmp`, but because this is still C++,
they're overloaded and have behaviour controlled by parameters, which makes them more convenient to use. `Base_MemSet`,
`Base_MemCopy`, and `Base_MemCompare` (and the string functions implemented on top of them) also make use of SIMD where possible.

There's not many fancy containers yet, but `CVector<T>` defined in `public/base/vector.h` is a working implementation of a dynamic array.
`public/base/string.h` defines `CString`, and it implements advanced features like splitting and multiplication. Additionally, there's
`CIntrusiveLinkedList<T>`, which is used for the free list in the memory allocator, and offers significant user control over the nodes for exactly
that reason.

== Assertions and error handling
Assertions are mainly for scenarios that shouldn't happen, and are disabled in retail builds because anything triggering them should be caught in
debug/release builds; don't use them for general error handling. For example, if a piece of memory _must_ be allocated successfully, like in `operator new()`
where the standard technically requires that it not return `nullptr` (even though the standard isn't as relevant for the engine), or an index is
outside the valid range, or a parameter is wrong in a way it shouldn't be, then you can use an assert. Normally, you can use the `ASSERT` macro.
If a condition isn't the most indicative of why something is wrong, `ASSERT_MSG` lets you add a message. For functions which just succeed or fail,
return `false`, `nullptr`, or some other reasonable/documented value when an error happens. When an unrecoverable error happens, use `Base_Quit` (or
`Base_Abort`/`Base_AbortSafe` in functions where logging/allocation isn't available) to kill the engine and show the user an error message.
