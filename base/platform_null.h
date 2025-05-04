/// \file <PLATFORM NAME> support
/// \copyright 2025 Randomcode Developers

#pragma once

// TODO: error for wrong platform
//#ifndef CH_<FIXME>
//#error "This header is <FIXME> only"
//#endif

#include "base/async.h"
#include "base/base.h"
#include "base/loader.h"
#include "base/types.h"

class CNullMutex: public IMutex
{
  public:
	CNullMutex();
	virtual ~CNullMutex();

	virtual void Lock();
	virtual bool TryLock(u32 timeout);
	virtual void Unlock();

	virtual uptr GetHandle() const
	{
		return reinterpret_cast<uptr>(&m_handle);
	}

  private:
	void* m_handle;
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
  	void* m_handle;
	bool m_alive;
	pid_t m_id;
	s32 m_result;
	dstr m_name;
	ThreadStart_t m_start;
	void* m_userData;

	static void* ThreadMain(CNullThread* thread);
};

class CNullLibrary: public ILibrary
{
  public:
	CNullLibrary(cstr name, void* base);
	~CNullLibrary();

	virtual void Unload();

	virtual void* GetSymbol(cstr name);

	virtual cstr GetName()
	{
		return m_name;
	}

	virtual uptr GetBase()
	{
		return reinterpret_cast<uptr>(m_base);
	}

  private:
	dstr m_name;
	void* m_base;
};
