/// \file Windows support
/// \copyright Randomcode Developers

#pragma once

#ifndef CH_WIN32
#error "This header is Windows only"
#endif

#include "base/loader.h"
#include "base/platform.h"
#include "base/types.h"

/// Export a function but also define it so it can be used like it was imported from NTDLL within Base.dll
/// The way the forwarder is written is kinda iffy, but the code generated should be (and has been thus far) valid
/// Additionally, a function that tells whether the function pointer is null is exported, which is useful with functions like
/// GetDpiForWindow, which only Windows 10 and up support
#define STUB_AVAILABLE_2(x) x##_Available
#define STUB_AVAILABLE(x)   STUB_AVAILABLE_2(x)
#define STUB_NAME_2(x)      STUB_##x
#define STUB_NAME(x)        STUB_NAME_2(x)
/// x86 is the only name mangled architecture
#ifdef CH_IA32
#define MAKE_STUB(x, callingConv, ...)                                                                                           \
	extern "C" uptr (*STUB_NAME(x))(...);                                                                                        \
	extern "C" BASEAPI bool STUB_AVAILABLE(x)();                                                                                 \
	EXPORT_RAW("_" STRINGIZE(x) "_Available")                                                                                  \
	EXPORT_AS_RAW(STRINGIZE(x) "_Forwarder", "_" STRINGIZE(x) #__VA_ARGS__)
#elif defined CH_XBOX360
#define MAKE_STUB(x, ...)                                                                                                        \
	extern "C" BASEAPI bool STUB_AVAILABLE(x)()                                                                                  \
	EXPORT_RAW(STRINGIZE(x) "_Available")
#else
#define MAKE_STUB(x, ...)                                                                                                        \
	extern "C" uptr (*STUB_NAME(x))(...);                                                                                        \
	extern "C" BASEAPI bool STUB_AVAILABLE(x)();                                                                                 \
	EXPORT_RAW(STRINGIZE(x) "_Available")                                                                                        \
	EXPORT_AS(x##_Forwarder, x)
#endif

/// Get the address of NTDLL, find LdrGetProcedureAddress manually, find LdrLoadDll properly, load any other system DLLs/functions
/// using those
extern bool Base_InitLoader();
extern bool Base_CheckWoW64();
extern cstr Base_IsWine();

extern bool Base_InitWinRt();
extern void Base_ShutdownWinRt();
extern int Base_RunMainWinRt(int (*main)());

extern bool g_loaderInitialized;

extern bool g_uwp;

class CWindowsLibrary: public ILibrary
{
  public:
	CWindowsLibrary(cstr name, void* base);
	~CWindowsLibrary();

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
