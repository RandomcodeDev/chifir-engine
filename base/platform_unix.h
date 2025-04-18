/// \file Unix support
/// \copyright Randomcode Developers

#pragma once

#ifndef CH_UNIX
#error "This header is Unix only"
#endif

#include <pthread.h>

#include "base/async.h"
#include "base/base.h"
#include "base/loader.h"
#include "base/types.h"

class CUnixMutex: public IMutex
{
  public:
	CUnixMutex();
	virtual ~CUnixMutex();
	
	virtual void Lock();
	virtual bool TryLock(u32 timeout);
	virtual void Unlock();

	virtual uptr GetHandle() const
	{
		return reinterpret_cast<uptr>(&m_handle);
	}

  private:
	pthread_mutex_t m_handle;
};

extern thread_local IThread* g_currentThread;
extern thread_local bool g_isMainThread;

class CUnixThread: public IThread
{
  public:
	CUnixThread(ThreadStart_t start, void* userData, cstr name, ssize stackSize, ssize maxStackSize);
	virtual ~CUnixThread();
	
	virtual void Run();
	virtual bool Wait(u32 timeout);
	
	virtual bool IsAlive() const
	{
		return m_alive;
	}

	virtual s32 GetResult() const
	{
		return m_result;
	}

	virtual u64 GetId() const
	{
		return m_id;
	}

	virtual uptr GetHandle() const
	{
		return reinterpret_cast<uptr>(m_handle);
	}

	virtual cstr GetName() const
	{
		return m_name;
	}

  private:
  	pthread_t m_handle;
	bool m_alive;
	pid_t m_id;
	s32 m_result;
	dstr m_name;
	ThreadStart_t m_start;
	void* m_userData;

	static void* ThreadMain(CUnixThread* thread);
};

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
extern bool UnixTimestampToDateTime(s64 seconds, DateTime& time);
