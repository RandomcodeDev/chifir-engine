/// Macros

#pragma once

#define ASSERT_IMPL(cond, action)                                                                                                \
	if (!(cond))                                                                                                                 \
	{                                                                                                                            \
		action;                                                                                                                  \
	}

/// Quit if a condition isn't true, add a message
#define ASSERT_MSG(cond, ...) ASSERT_IMPL(cond, Base_Abort(ABORT_RELEVANT_ERROR, "Assertion " #cond " failed: " __VA_ARGS__))

/// Quit if a condition isn't true, add a message
#define ASSERT_MSG_SAFE(cond, msg) ASSERT_IMPL(cond, Base_AbortSafe(ABORT_RELEVANT_ERROR, "Assertion " #cond " failed: " msg))

/// Quit if a condition isn't true
#define ASSERT(cond) ASSERT_IMPL(cond, Base_AbortSafe(ABORT_RELEVANT_ERROR, "Assertion " #cond " failed"))

/// Stringize something
#define STRINGIZE(x) #x

/// Stringize something, and expand macros one level
#define STRINGIZE_EXPAND(x) STRINGIZE(x)

/// Get the address of a structure containing the given pointer
#define CONTAINING_STRUCTURE(type, member, ptr) (reinterpret_cast<type*>(reinterpret_cast<uptr>(ptr) - offsetof(type, member)))

/// Cast away const/function pointer
#define CONST_CAST(T, ptr) const_cast<T>(ptr)

/// Get the offset of a field in a struct with a stable layout
#define OFFSETOF(T, m) (reinterpret_cast<uptr>(&static_cast<T*>(nullptr)->m))
