// Dynamic string implementation

#pragma once

#include "base.h"
#include "types.h"
#include "vector.h"

template <typename T>
class CBaseString : public CVector<T>
{
  public:
	CBaseString() DEFAULT;

	

  protected:
	T* m_buffer;
	usize m_size;
	usize m_capacity;

	virtual usize StrLen(const T* value) = 0;
};
