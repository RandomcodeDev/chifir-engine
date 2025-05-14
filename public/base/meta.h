/// \file Metaprogramming stuff
/// \copyright 2025 Randomcode Developers

#pragma once

#include "compiler.h"

// encodes a constant value in a type for meta nonsense
template <typename T, T v> struct ValueType_t
{
	static constexpr T value = v;
};

using True_t = ValueType_t<bool, true>;
using False_t = ValueType_t<bool, false>;

namespace impl
{
template <typename Base> struct IsBaseOf
{
	// if it converts to Base, then return True_t. otherwise, fallthrough to the varargs one that returns False_t.
	static True_t Test(Base*);
	static False_t Test(...);
};
} // namespace impl

template <typename Base, typename T> constexpr bool IsBaseOf()
{
	return decltype(impl::IsBaseOf<Base>::Test(static_cast<T*>(nullptr)))::value;
}
