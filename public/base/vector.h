// Dynamic array implementation around stb_ds

#pragma once

#include "base.h"
#include "container.h"
#include "types.h"

#include "stb/stb_ds.h"

template <typename T>
class CVector : public IContainer<T, usize>
{
	CVector() DEFAULT;
	CVector(const T* data, usize size)
	{
		Resize(size);
		Base_MemCpy(m_buffer, data, m_size);
	}
	~CVector() DEFAULT;

	usize Size()
	{
		return stbds_arrlenu(m_buffer);
	}

	I Insert(const T& object, usize index = BAD_INDEX)
	{
		stbds_arrins(m_buffer, object, index);
	}

	void Delete(I index);
	T& operator[](const I& index)
	{
	}
	void operator+(const T& object);
	I Find(s32 (*Compare)(const T& a, const T& b));
	void Sort(s32 (*Compare)(const T& a, const T& b));

	// So other containers that are similar, like strings, can share functionality more easily
  protected:
	T* m_buffer;
};
