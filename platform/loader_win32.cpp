#include "platform/loader.h"
#include "loader_win32.h"

static void* ntDll;

// Export a function but also define it so it can be used like it was imported from NTDLL within Platform.dll.

static void FindNtDll()
{

}

static void FindLdrGetProcedureAddress()
{

}

bool Plat_InitLoader()
{
	return true;
}
