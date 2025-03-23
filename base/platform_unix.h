/// Unix support
/// Copyright 2025 Randomcode Developers

#pragma once

#ifndef CH_UNIX
#error "This header is Unix only"
#endif

#include "base/base.h"
#include "base/loader.h"
#include "base/types.h"

class CUnixLibrary: public ILibrary
{
  public:
	CUnixLibrary(cstr name, void* base);
	~CUnixLibrary();

	void* GetSymbol(cstr name);

	cstr GetName()
	{
		return m_name;
	}

	uptr GetBase()
	{
		return reinterpret_cast<uptr>(m_base);
	}

  private:
	dstr m_name;
	void* m_base;
};

#define STDIN_FD  0
#define STDOUT_FD 1
#define STDERR_FD 2

extern uptr g_errno;

/// Make a system call
template <class T1 = uptr, class T2 = uptr, class T3 = uptr, class T4 = uptr, class T5 = uptr, class T6 = uptr>
ATTRIBUTE(naked) uptr Base_SysCall(uptr number, T1 arg1 = 0, T2 arg2 = 0, T3 arg3 = 0, T4 arg4 = 0, T5 arg5 = 0, T6 arg6 = 0);

extern s64 g_timeZoneOffset;

/// Convert a Unix timestamp in milliseconds to a DateTime
extern bool UnixTimestampToDateTime(s64 seconds, DateTime_t& time);
