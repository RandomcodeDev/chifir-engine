// Common container stuff

#pragma once

// Shared interface in all containers
template <typename T, typename I>
class IContainer
{
  public:
	typedef T ItemType_t;
	typedef I IndexType_t;

	static const IndexType_t BAD_INDEX = (IndexType_t)-1;

	~IContainer() = default;

	// Insert an item at index, or append if index == BAD_INDEX
	virtual void Insert(const ItemType_t& object, IndexType_t index = BAD_INDEX) = 0;

	// Remove the specified index if it exists
	virtual void Delete(IndexType_t index) = 0;

	// Get an item
	virtual ItemType_t& operator[](const IndexType_t& index) = 0;

	// Append an item
	virtual void operator+(const ItemType_t& object) = 0;

	// Find a matching item
	virtual IndexType_t Find(s32 (*Match)(const ItemType_t& item)) = 0;
};
