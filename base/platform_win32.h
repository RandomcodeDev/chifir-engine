/// \file Windows support
/// \copyright 2025 Randomcode Developers

#pragma once

#ifndef CH_WIN32
#error "This header is Windows only"
#endif

#include "base/async.h"
#include "base/loader.h"
#include "base/platform.h"
#include "base/types.h"

class CWindowsMutex: public IMutex
{
  public:
	CWindowsMutex();
	virtual ~CWindowsMutex();

	virtual void Lock();
	virtual bool TryLock(u32 timeout);
	virtual void Unlock();

	virtual uptr GetHandle() const
	{
		return reinterpret_cast<uptr>(m_handle);
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

class CWindowsThread: public IThread
{
  public:
	CWindowsThread(ThreadStart_t start, void* userData, cstr name, ssize stackSize, ssize maxStackSize);
	virtual ~CWindowsThread();

	virtual void Run();
	virtual bool Wait(u32 timeout);

	virtual bool IsAlive() const
	{
		return m_handle != nullptr;
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
		// win32 threads don't have names
		return m_name;
	}

  private:
  	HANDLE m_handle;
	void* m_stackAlloc;
	usize m_stackSize;
	u64 m_id;
	s32 m_result;
	dstr m_name;
	ThreadStart_t m_start;
	void* m_userData;

	void CreateVistaThread(ssize stackSize, ssize maxStackSize);
	void CreateStack(ssize size, ssize maxSize, PINITIAL_TEB initialTeb);
	void InitContext(PCONTEXT context, ThreadStart_t start, void* param, void* stack);
	void CreateXpThread(ssize stackSize, ssize maxStackSize);
	static NTSTATUS NTAPI ThreadMain(CWindowsThread* thread);
};

extern SYSTEM_BASIC_INFORMATION g_systemInfo;
extern SYSTEM_PERFORMANCE_INFORMATION g_systemPerfInfo;
extern SYSTEM_BUILD_VERSION_INFORMATION g_buildVerInfo;

extern bool Base_InitWinRt();
extern void Base_ShutdownWinRt();
extern int Base_RunMainWinRt(int (*main)());
extern cstr Base_GetWinRtAppData();

extern bool g_uwp;

/// Export a function but also define it so it can be used like it was imported from NTDLL within Base.dll
/// The way the forwarder is written is kinda iffy, but the code generated should be (and has been thus far) valid
/// Additionally, a function that tells whether the function pointer is null is exported, which is useful with functions like
/// GetDpiForWindow, which only Windows 10 and up support
#define STUB_AVAILABLE_2(x) x##_Available
#define STUB_AVAILABLE(x)   STUB_AVAILABLE_2(x)
#define STUB_NAME_2(x)      STUB_##x
#define STUB_NAME(x)        STUB_NAME_2(x)
#ifdef CH_WIN32_CROSSCOMPILE
#define FORWARDER_PREFIX "_"
#define EXPORT_PREFIX "_"
#else
#define FORWARDER_PREFIX ""
#define EXPORT_PREFIX "_"
#endif
/// x86 is the only name mangled architecture
#ifdef CH_IA32
#define MAKE_STUB(x, callingConv, ...)                                                                                           \
	extern "C" uptr (*STUB_NAME(x))(...);                                                                                        \
	extern "C" BASEAPI bool STUB_AVAILABLE(x)();                                                                                 \
	EXPORT_RAW("_" STRINGIZE(x) "_Available")                                                                                    \
	EXPORT_RAW(EXPORT_PREFIX STRINGIZE(x) #__VA_ARGS__)
#elif defined CH_XENON
#define MAKE_STUB(x, ...) extern "C" BASEAPI bool STUB_AVAILABLE(x)() EXPORT_RAW(STRINGIZE(x) "_Available")
#else
#define MAKE_STUB(x, ...)                                                                                                        \
	extern "C" uptr (*STUB_NAME(x))(...);                                                                                        \
	extern "C" BASEAPI bool STUB_AVAILABLE(x)();                                                                                 \
	EXPORT_RAW(STRINGIZE(x) "_Available")                                                                                        \
	EXPORT_AS(x##_Forwarder, x)
#endif

// Somehow the Rtl function for this isn't inline in phnt, I guess it does that thing with the last section or whatever
#define RVA_TO_VA(base, rva) (reinterpret_cast<const u8*>(base) + (rva))

/// Get the address of NTDLL, find LdrGetProcedureAddress manually, find LdrLoadDll properly, load any other system DLLs/functions
/// using those
extern bool Base_InitLoader();
extern bool Base_CheckWoW64();
extern cstr Base_GetWineVersion();

extern bool g_loaderInitialized;

class CWindowsLibrary: public ILibrary
{
  public:
	CWindowsLibrary(cstr name, void* base);
	~CWindowsLibrary();

	virtual void Unload();

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

extern SYSTEM_BASIC_INFORMATION g_systemInfo;
