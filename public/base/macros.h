/// \file Macros
/// \copyright Randomcode Developers

#pragma once

/// Stringize something
#define STRINGIZE(x) #x

/// Stringize something, and expand macros one level
#define STRINGIZE_EXPAND(x) STRINGIZE(x)

/// Make a string literal wchar_t's
#define WIDEN(x) L##x

/// Make a macro wchar_t's
#define WIDEN_EXPAND(x) WIDEN(x)

#ifdef CH_RETAIL
#define ASSERT_IMPL(cond, action)
#else
#define ASSERT_IMPL(cond, action)                                                                                                \
	if (!(cond))                                                                                                                 \
	{                                                                                                                            \
		action;                                                                                                                  \
	}
#endif

/// Quit if a condition isn't true, add a message
#define ASSERT_MSG(cond, ...)                                                                                                    \
	ASSERT_IMPL(                                                                                                                 \
		cond,                                                                                                                    \
		Base_Abort(                                                                                                              \
			ABORT_RELEVANT_ERROR, "Assertion " #cond " at " __FILE__ ":" STRINGIZE_EXPAND(__LINE__) " failed: " __VA_ARGS__))

/// Quit if a condition isn't true, add a message
#define ASSERT_MSG_SAFE(cond, msg) ASSERT_IMPL(cond, Base_AbortSafe(ABORT_RELEVANT_ERROR, "Assertion " #cond " failed: " msg))

/// Quit if a condition isn't true
#define ASSERT(cond)                                                                                                             \
	ASSERT_IMPL(                                                                                                                 \
		cond,                                                                                                                    \
		Base_AbortSafe(ABORT_RELEVANT_ERROR, "Assertion " #cond " at " __FILE__ ":" STRINGIZE_EXPAND(__LINE__) " failed!"))

/// Get the address of a structure containing the given pointer
#define CONTAINING_STRUCTURE(type, member, ptr) (reinterpret_cast<type*>(reinterpret_cast<uptr>(ptr) - offsetof(type, member)))

/// Cast away const/function pointer
#define CONST_CAST(T, ptr) const_cast<T>(ptr)

/// Get the offset of a field in a struct with a stable layout
#define OFFSETOF(T, m) (reinterpret_cast<uptr>(&static_cast<T*>(nullptr)->m))

/// Mark a variable as unused
#define UNUSED(x) ((void)(x))
