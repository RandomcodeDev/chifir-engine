/// \file Asynchronous support (threads, sync objects, atomics, etc)
/// \copyright Randomcode Developers

#pragma once

#include "compiler.h"
#include "platform.h"
#include "types.h"

/// Mutex
class IMutex
{
};

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

	void operator++(int value)
	{
		ExchangeAdd(value);
	}

	T ExchangeSubtract(T value)
	{
		return Async_AtomicXchgSub(m_value, value);
	}

	void operator-=(T value)
	{
		ExchangeSubtract(value);
	}

	void operator--(int value)
	{
		ExchangeSubtract(value);
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
