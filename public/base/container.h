/// \file Common container stuff
/// \copyright 2025 Randomcode Developers

#pragma once

#include "types.h"

/// Shared interface in all containers
template <typename T, typename I> class IContainer
{
  public:
	static constexpr I BAD_INDEX = (I)-1;

	virtual ~IContainer() = default;

	/// Get the size
	virtual ssize Size() const = 0;

	/// Whether the container is empty
	virtual bool IsEmpty() const
	{
		return Size() == 0;
	}

	/// Insert an item at index, or append if index == BAD_INDEX
	virtual I Add(const T& object, I index = BAD_INDEX) = 0;

	/// Insert count objects at index, or at the end
	virtual I Add(const T* objects, ssize count, ssize index = IContainer<T, ssize>::BAD_INDEX) = 0;

	/// Remove the specified index if it exists
	virtual void Delete(I index) = 0;

	/// Get an item
	virtual T& operator[](const I& index) const = 0;

	/// Append an item
	virtual void operator+=(const T& object) = 0;

	/// Find a matching item
	virtual I Find(s32 (*Compare)(const T& a, const T& b)) const = 0;

	/// Sort
	virtual void Sort(s32 (*Compare)(const T& a, const T& b)) = 0;
};
