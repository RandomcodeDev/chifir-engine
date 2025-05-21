/// \file <PLATFORM NAME> support
/// \copyright 2025 Randomcode Developers

#include "platform_null.h"
#include "base.h"
#include "base/base.h"
#include "base/basicstr.h"
#include "base/compiler.h"
#include "base/log.h"
#include "base/platform.h"
#include "base/string.h"
#include "base/types.h"
#include "base/vector.h"

static dstr s_systemDescription;
static dstr s_hardwareDescription;

BASEAPI void Plat_Init()
{
	if (!g_platInitialized)
	{
		g_allocUsable = true;
		g_platInitialized = true;
	}
}

BASEAPI void Plat_Shutdown()
{
	if (g_platInitialized)
	{
		g_platInitialized = false;
	}
}

BASEAPI void Plat_GetArgs(CVector<CString>& args)
{
}

BASEAPI cstr Plat_GetSystemDescription()
{
	// TODO: system description
	return "<FIXME>";
}

BASEAPI cstr Plat_GetHardwareDescription()
{
	// TODO: hardware description
	return "unknown hardware";
}

BASEAPI NORETURN void Base_AbortSafe(s32 error, cstr msg)
{
}

bool Base_GetSystemMemory(ssize size)
{
	// Linked list nodes, can contain any size of allocation, but there's a limit to the number of OS allocations
	static IntrusiveLinkedNode<SystemAllocation> memoryNodes[64];
}

BASEAPI ILibrary* Base_LoadLibrary(cstr name)
{
}

CNullLibrary::CNullLibrary(cstr name, void* base) : m_name(Base_StrClone(name)), m_base(base)
{
}

CNullLibrary::~CNullLibrary()
{
}

void CNullLibrary::Unload()
{
}

void* CNullLibrary::GetSymbol(cstr name, bool optional)
{
}

BASEAPI bool Plat_ConsoleHasColor()
{
}

BASEAPI void Plat_WriteConsole(cstr message)
{
}

BASEAPI cstr Plat_GetDataLocation()
{
}

BASEAPI cstr Plat_GetEngineDir()
{
}

BASEAPI cstr Plat_GetEnvironment(cstr name)
{
}

BASEAPI u64 Plat_GetMilliseconds()
{
}

BASEAPI void Plat_GetDateTime(DateTime& time, bool utc)
{
}
