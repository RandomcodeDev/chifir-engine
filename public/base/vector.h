// Dynamic array implementation

#pragma once

#include <new>

#include "base.h"
#include "container.h"
#include "types.h"

template <typename T> class CVector : public IContainer<T, ssize>
{
  public:
	CVector() : m_buffer(nullptr), m_size(0), m_capacity(0), m_sorted(false)
	{
	}

	// Create a vector from the given data
	CVector(const T* data, ssize size)
	{
		Resize(size);
		// Copying objects is fine because their vtables stay the same
		Base_MemCopy(m_buffer, data, m_size);
	}

	// Avoid this
	CVector(const CVector<T>& other)
	{
		Add(other);
	}

	~CVector()
	{
		if (m_buffer)
		{
			for (ssize i = 0; i < Size(); ++i)
			{
				m_buffer[i].~T();
			}
			Base_Free(m_buffer);
		}
	}

	// Get the underlying buffer of this vector
	T* Data() const
	{
		return m_buffer;
	}

	ssize Size() const
	{
		return m_size;
	}

	// Resize the vector to the given size
	void Resize(ssize newSize)
	{
		ssize oldSize = m_size;
		if (m_capacity < newSize)
		{
			Reserve(Max(m_capacity * 2, newSize));
		}
		m_size = newSize;

		if (oldSize < newSize)
		{
			for (ssize i = oldSize; i < newSize; i++)
			{
				// Construct all the objects
				new (&m_buffer[i]) T;
			}
			m_sorted = false;
		}
		else
		{
			for (ssize i = newSize; i < oldSize; i++)
			{
				// Destroy the objects
				m_buffer[i].~T();
			}
		}
	}

	// Reserve space for future additions
	void Reserve(ssize size)
	{
		if (size > m_capacity)
		{
			m_capacity = size;
			m_buffer = static_cast<T*>(Base_Realloc(m_buffer, m_capacity * sizeof(T)));
			ASSERT(m_buffer != nullptr);
		}
	}

	// Add an object
	ssize Add(const T& object, ssize index = IContainer<T, ssize>::BAD_INDEX)
	{
		m_sorted = false;
		m_size++;
		if (m_size > m_capacity)
		{
			Reserve(Max<ssize>(1, m_capacity * 2));
		}

		// Append by default
		if (index == IContainer<T, ssize>::BAD_INDEX || index > m_size)
		{
			m_buffer[m_size - 1] = object;
			return m_size - 1;
		}
		else
		{
			Base_MemCopy(&m_buffer[index + 1], &m_buffer[index], m_size - index - 1);
			m_buffer[index] = object;
			return index;
		}
	}

	// Add many objects
	ssize Add(const T* objects, ssize count, ssize startIndex = IContainer<T, ssize>::BAD_INDEX)
	{
		Reserve(count);

		ssize index = 0;
		for (ssize i = 0; i < count; i++)
		{
			index = Add(objects[i], startIndex + i);
		}

		return index - count - 1;
	}

	// Add the contents of another vector
	ssize Add(const CVector<T>& other, ssize startIndex = IContainer<T, ssize>::BAD_INDEX)
	{
		return Add(other.Data(), other.Size(), startIndex);
	}

	// Remove the given element
	void Delete(ssize index = IContainer<T, ssize>::BAD_INDEX)
	{
		// Delete the last element by default
		if (index == IContainer<T, ssize>::BAD_INDEX || index > m_size)
		{
			index = m_size - 1;
		}
		m_buffer[index].~T();
		Base_MemCopy(&m_buffer[index], &m_buffer[index + 1], m_size - index - 1);
		m_size--;
		m_sorted = false;
	}

	// Empty the vector
	void Empty()
	{
		Resize(0);
	}

	// Get the given element
	T& operator[](const ssize& index) const
	{
		ASSERT(index < m_size && index >= 0);
		return m_buffer[index];
	}

	// Append an object
	void operator+=(const T& object)
	{
		Add(object);
	}

	// Append the contents of another vector
	void operator+=(const CVector<T>& other)
	{
		Add(other);
	}

	// Not implemented yet
	ssize Find(s32 (*Compare)(const T& a, const T& b)) const
	{
		(void)Compare;
		return IContainer<T, ssize>::BAD_INDEX;
	}

	// Not implemented yet
	void Sort(s32 (*Compare)(const T& a, const T& b))
	{
		(void)Compare;
		m_sorted = false;
	}

	// So other containers that are similar, like strings, can share functionality more easily
  protected:
	T* m_buffer;
	ssize m_size;
	ssize m_capacity;
	bool m_sorted;
};
