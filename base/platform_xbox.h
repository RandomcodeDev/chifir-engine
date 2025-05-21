/// \file Xbox support
/// \copyright 2025 Randomcode Developers

#pragma once

#ifndef CH_XBOX
#error "This header is Xbox/Xbox 360 only"
#endif

#include <stdio.h>

#include "base/async.h"
#include "base/base.h"
#include "base/loader.h"
#include "base/platform.h"
#include "base/types.h"

typedef struct _XBOX_KRNL_VERSION {
    USHORT Major;
    USHORT Minor;
    USHORT Build;
    USHORT Qfe;
} XBOX_KRNL_VERSION, *PXBOX_KRNL_VERSION;

extern "C" DLLIMPORT PXBOX_KRNL_VERSION XboxKrnlVersion;

typedef struct _XBOX_HARDWARE_INFO {
    ULONG Flags;
    UCHAR GpuRevision;
    UCHAR McpRevision;
    UCHAR reserved[2];
} XBOX_HARDWARE_INFO, *PXBOX_HARDWARE_INFO;

extern "C" DLLIMPORT PXBOX_HARDWARE_INFO XboxHardwareInfo;

#define XBOX_HW_FLAG_DEVKIT_KERNEL 0x0000002

class CXboxMutex: public IMutex
{
  public:
	CXboxMutex();
	virtual ~CXboxMutex();

	virtual void Lock();
	virtual bool TryLock(u32 timeout);
	virtual void Unlock();

	virtual u64 GetHandle() const
	{
		return reinterpret_cast<u64>(&m_handle);
	}

  private:
	HANDLE m_handle;
};

struct TlsData
{
	IThread* currentThread;
	bool isMainThread;
};

extern u32 g_tlsIndex;

/// Get the TLS data for this thread
extern TlsData* Plat_GetTlsData();

class CXboxThread: public IThread
{
  public:
	CXboxThread(ThreadStart_t start, void* userData, cstr name, ssize stackSize, ssize maxStackSize);
	virtual ~CXboxThread();

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
  	HANDLE m_handle;
	bool m_alive;
	u64 m_id;
	s32 m_result;
	dstr m_name;
	ThreadStart_t m_start;
	void* m_userData;

	static LONG WINAPI ThreadMain(CXboxThread* thread);
};

class CXboxLibrary: public ILibrary
{
  public:
	CXboxLibrary(cstr name, void* base);
	~CXboxLibrary();

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
