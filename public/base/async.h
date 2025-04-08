/// \file Asynchronous support (threads, sync objects, atomics, etc)
/// \copyright Randomcode Developers

#pragma once

#include "compiler.h"
#include "platform.h"
#include "types.h"

/// A mutex
class IMutex
{
  public:
	virtual ~IMutex() = default;
	
	/// Wait indefinitely and lock the mutex
	virtual void Lock() = 0;

	/// Wait up to the given number of milliseconds to acquire the mutex
	virtual bool TryLock(u64 timeout = 0) = 0;

	/// Release the mutex
	virtual void Release() = 0;
};

/// A running thread
class IThread
{
  public:
	virtual ~IThread() = default;

	/// Start the thread and let it run independently
	virtual void Run() = 0;

	/// Wait for the thread to finish, with an optional millisecond timeout
	virtual bool Wait(u64 timeout = UINT64_MAX) = 0;
	
	/// Get whether the thread is alive
	virtual bool IsAlive() const = 0;

	/// Get the result of the thread, if it's finished
	virtual s32 GetResult() const = 0;

	/// Get the ID of the thread
	virtual u64 GetId() const = 0;

	/// Get the system handle for the thread
	virtual uptr GetHandle() const = 0;

	/// Get the name of the thread
	virtual cstr GetName() const = 0;
};

/// The required signature for a thread function
typedef s32 (*ThreadStart_t)(void* userData);

/// Create a thread
extern BASEAPI IThread* Async_CreateThread(ThreadStart_t start, void* userData, cstr name = nullptr, ssize stackSize = 0, ssize maxStackSize = 0);

/// Get the current thread (on the main thread and external threads, returns nullptr)
extern BASEAPI IThread* Async_GetCurrentThread();

#define IS_VALID_ATOMIC(T) (sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8)

#define ATOMIC_DISPATCH(name, type, params, dispname, args)                                                                      \
	template <typename T> type Async_Atomic##name params                                                                         \
	{                                                                                                                            \
		static_assert(IS_VALID_ATOMIC(T));                                                                                       \
                                                                                                                                 \
		if constexpr (sizeof(T) == 1)                                                                                            \
		{                                                                                                                        \
			return Async_Atomic##dispname##8 args;                                                                               \
		}                                                                                                                        \
		else if constexpr (sizeof(T) == 2)                                                                                       \
		{                                                                                                                        \
			return Async_Atomic##dispname##16 args;                                                                              \
		}                                                                                                                        \
		else if constexpr (sizeof(T) == 4)                                                                                       \
		{                                                                                                                        \
			return Async_Atomic##dispname##32 args;                                                                              \
		}                                                                                                                        \
		else if constexpr (sizeof(T) == 8)                                                                                       \
		{                                                                                                                        \
			return Async_AtomicAsync_Atomic##dispname##64 args;                                                                  \
		}                                                                                                                        \
	}

ATOMIC_DISPATCH(Xchg, T, (T & dest, T value), Xchg, (dest, value))
ATOMIC_DISPATCH(CmpXchg, T, (T & dest, T value, T compare), CmpXchg, (dest, value, compare))
ATOMIC_DISPATCH(And, T, (T & dest, T value), And, (dest, value))
ATOMIC_DISPATCH(Or, T, (T & dest, T value), Or, (dest, value))
ATOMIC_DISPATCH(Xor, T, (T & dest, T value), Xor, (dest, value))
ATOMIC_DISPATCH(XchgAdd, T, (T & dest, T value), XchgAdd, (dest, value))
ATOMIC_DISPATCH(XchgSub, T, (T & dest, T value), XchgSub, (dest, value))

#undef ATOMIC_DISPATCH

/// Atomic wrapper
template <typename T> struct Atomic
{
	static_assert(IS_VALID_ATOMIC(T));

	Atomic(T value) : m_value(value)
	{
	}

	T Exchange(T value)
	{
		return Async_AtomicXchg(m_value, value);
	}

	T Load() const
	{
		return m_value;
	}

	operator T() const
	{
		return Load();
	}

	void Store(T value)
	{
		Exchange(value);
	}

	void operator=(T value)
	{
		Store(value);
	}

	T CompareExchange(T value, T compare)
	{
		return Async_AtomicCmpXchg(m_value, value, compare);
	}

	T ExchangeAdd(T value)
	{
		return Async_AtomicXchgAdd(m_value, value);
	}

	void operator+=(T value)
	{
		ExchangeAdd(value);
	}

	// prefix
	T operator++()
	{
		return ExchangeAdd(1) + 1;
	}

	// suffix
	T operator++(int value)
	{
		return ExchangeAdd(1);
	}

	T ExchangeSubtract(T value)
	{
		return Async_AtomicXchgSub(m_value, value);
	}

	void operator-=(T value)
	{
		ExchangeSubtract(value);
	}

	// prefix
	T operator--()
	{
		return ExchangeSubtract(1) - 1;
	}

	// suffix
	T operator--(int value)
	{
		return ExchangeSubtract(1);
	}

	T And(T value)
	{
		return Async_AtomicAnd(m_value, value);
	}

	void operator&=(T value)
	{
		And(value);
	}

	T Or(T value)
	{
		return Async_AtomicOr(m_value, value);
	}

	void operator|=(T value)
	{
		Or(value);
	}

	T Xor(T value)
	{
		return Async_AtomicXor(m_value, value);
	}

	void operator^=(T value)
	{
		Xor(value);
	}

  private:
	T m_value;
};
