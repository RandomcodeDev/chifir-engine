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
	ssize m_size;
	ssize m_capacity;

	virtual ssize StrLen(const T* value) = 0;
};
