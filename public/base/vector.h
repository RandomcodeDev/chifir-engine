// Dynamic array implementation around stb_ds

#pragma once

#include "base.h"
#include "container.h"
#include "types.h"

#include "stb/stb_ds.h"

template <typename T> class CVector : public IContainer<T, ssize>
{
  public:
	CVector() DEFAULT;

	CVector(const T* data, ssize size)
	{
		Resize(size);
		Base_MemCopy(m_buffer, data, m_size);
	}

	~CVector() DEFAULT;

	ssize Size() const
	{
		return stbds_arrlenu(m_buffer);
	}

	ssize Add(const T& object, ssize index = IContainer<T, ssize>::BAD_INDEX)
	{
		m_sorted = false;
		if (index == IContainer<T, ssize>::BAD_INDEX || index > Size())
		{
			stbds_arrput(m_buffer, object);
			return Size() - 1;
		}
		else
		{
			stbds_arrins(m_buffer, object, index);
			return index;
		}
	}

	void Delete(ssize index)
	{
		stbds_arrdel(m_buffer, index);
		m_sorted = false;
	}

	T& operator[](const ssize& index) const
	{
		ASSERT(index < Size() && index != BAD_INDEX);
		return m_buffer[index];
	}

	void operator+=(const T& object)
	{
		Add(object);
		m_sorted = false;
	}

	ssize Find(s32 (*Compare)(const T& a, const T& b)) const
	{
		(void)Compare;
		return BAD_INDEX;
	}

	void Sort(s32 (*Compare)(const T& a, const T& b))
	{
		(void)Compare;
		m_sorted = true;
	}

	// So other containers that are similar, like strings, can share functionality more easily
  protected:
	T* m_buffer;
	bool m_sorted;
};
