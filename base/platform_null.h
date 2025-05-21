/// \file Xbox support
/// \copyright 2025 Randomcode Developers

#pragma once

#ifndef CH_XBOX
#error "This header is Xbox/Xbox 360 only"
#endif

#include "base/async.h"
#include "base/base.h"
#include "base/loader.h"
#include "base/platform.h"
#include "base/types.h"

class CNullMutex: public IMutex
{
  public:
	CNullMutex();
	virtual ~CNullMutex();

	virtual void Lock();
	virtual bool TryLock(u32 timeout);
	virtual void Unlock();

	virtual u64 GetHandle() const
	{
		return reinterpret_cast<u64>(&m_handle);
	}

  private:
	void* m_handle;
};

class CNullThread: public IThread
{
  public:
	CNullThread(ThreadStart_t start, void* userData, cstr name, ssize stackSize, ssize maxStackSize);
	virtual ~CNullThread();

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

	virtual u64 GetHandle() const
	{
		return reinterpret_cast<u64>(m_handle);
	}

	virtual cstr GetName() const
	{
		return m_name;
	}

  private:
  	void* m_handle;
	bool m_alive;
	u64 m_id;
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

	virtual void* GetSymbol(cstr name, bool optional);

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
