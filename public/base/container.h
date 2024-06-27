// Common container stuff

#pragma once

template <typename T, typename I>
class IContainer
{
  public:
	typedef T ItemType_t;
	typedef I IndexType_t;

	~IContainer() = default;

	virtual void Insert(const T& object) = 0;
	virtual void Insert(const T& object, IndexType_t index) = 0;
	virtual void Delete(IndexType_t index) = 0;

	virtual T&
};
