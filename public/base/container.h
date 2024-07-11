// Common container stuff

#pragma once

#include "types.h"

// Shared interface in all containers
template <typename T, typename I>
class IContainer
{
  public:
	static const I BAD_INDEX = (I)-1;

	virtual IContainer(const T* data, usize size) = 0;
	~IContainer() DEFAULT;

	// Get the size
	virtual usize Size() = 0;

	// Insert an item at index, or append if index == BAD_INDEX
	virtual I Insert(const T& object, I index = BAD_INDEX) = 0;

	// Remove the specified index if it exists
	virtual void Delete(I index) = 0;

	// Get an item
	virtual T& operator[](const I& index) = 0;

	// Append an item
	virtual void operator+(const T& object) = 0;

	// Find a matching item
	virtual I Find(s32 (*Compare)(const T& a, const T& b)) = 0;

	// Sort
	virtual void Sort(s32 (*Compare)(const T& a, const T& b)) = 0;
};
