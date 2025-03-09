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


BASEAPI void Plat_Init()
{
}

BASEAPI void Plat_Shutdown()
{
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
	static LinkedNode_t<SystemAllocation_t> memoryNodes[64];
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

void* CNullLibrary::GetSymbol(cstr name)
{
}

BASEAPI bool Plat_ConsoleHasColor()
{
}

BASEAPI void Plat_WriteConsole(cstr message)
{
}

BASEAPI cstr Plat_GetSaveLocation()
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

BASEAPI void Plat_GetDateTime(DateTime_t& time, bool utc)
{
}
