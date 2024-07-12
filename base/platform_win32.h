// Implementation details of Windows DLL shenanigans

#pragma once

#include "base/loader.h"
#include "base/types.h"

// Export a function but also define it so it can be used like it was imported from NTDLL within Base.dll
// The way the forwarder is written is kinda iffy, but the code generated should be (and has been thus far) valid
#ifdef CH_I386
#define STUB_NAME(x) STUB_##x
#define MAKE_STUB(x, callingConv, paramSize)                                                                                     \
	extern "C" uptr(callingConv* STUB_##x)(...);                                                                                 \
	EXPORT_AS_RAW(#x "_Forwarder", "_" #x "@" #paramSize);
#else
#ifdef CH_PPC
#define STUB_NAME(x) STUB_##x
#define MAKE_STUB(x, ...)                                                                                     \
	extern "C" uptr(callingConv* STUB_NAME(x))(...);                                                                             \
	extern "C" uptr callingConv x##_Forwarder(...);                                                                              \
	EXPORT_AS(x##_Forwarder, x);
#else
#define STUB_NAME(x) __imp_##x
#define MAKE_STUB(x, callingConv, paramSize)                                                                                     \
	extern "C"                                                                                                                   \
	{                                                                                                                            \
		uptr(callingConv* STUB_NAME(x))(...);                                                                                    \
                                                                                                                                 \
		uptr callingConv x##_Forwarder(...)                                                                                      \
		{                                                                                                                        \
			return STUB_NAME(x)();                                                                                               \
		}                                                                                                                        \
	}                                                                                                                            \
	EXPORT_AS(x##_Forwarder, x);
#endif
#endif

// Get the address of NTDLL, find LdrGetProcedureAddress manually, find LdrLoadDll properly, load any other system DLLs/functions
// using those
extern bool Base_InitLoader();

class CWin32Library : public ILibrary
{
  public:
	CWin32Library(void* base);
	~CWin32Library();

	void* GetSymbol(cstr name);

  private:
	void* m_base;
};

extern SYSTEM_BASIC_INFORMATION g_systemInfo;
