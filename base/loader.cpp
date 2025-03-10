#include "base/loader.h"
#include "base/log.h"
#include "base/platform.h"
#include "base/string.h"

// Try the most likely path first, in retail bin is more likely, in a developer build it's probably flat
#ifdef CH_RETAIL
#define FIRST_PATH  "%s/bin/%s", Plat_GetEngineDir(), name
#define SECOND_PATH "%s/%s", Plat_GetEngineDir(), name
#else
#define FIRST_PATH  "%s/%s", Plat_GetEngineDir(), name
#define SECOND_PATH "%s/bin/%s", Plat_GetEngineDir(), name
#endif

static ILibrary* LoadEngineLibraryImpl(cstr name)
{
#ifdef CH_UNIX
	// the launcher restarts itself to add the engine's bin folder to LD_LIBRARY_PATH
	Log_Debug("Attempting to load engine library %s", name);
	return Base_LoadLibrary(name);
#else
	CString libName = CString::FormatStr(FIRST_PATH);
	Log_Debug("Attempting to load engine library %s from %s", name, libName.Data());
	ILibrary* lib = Base_LoadLibrary(libName.Data());
	if (!lib)
	{
		libName.Format(SECOND_PATH);
		Log_Debug("Attempting to load engine library %s from %s", name, libName.Data());
		lib = Base_LoadLibrary(libName.Data());
		if (!lib)
		{
			return nullptr;
		}
	}

	return lib;
#endif
}

BASEAPI ILibrary* Base_LoadEngineLibrary(cstr name)
{
	ILibrary* lib = LoadEngineLibraryImpl(name);
	if (!lib)
	{
		Log_Error("Failed to load engine library %s!", name);
		return nullptr;
	}

	return lib;
}

BASEAPI ILibrary* Base_LoadEngineOrSystemLibrary(cstr name)
{
	ILibrary* lib = Base_LoadEngineLibrary(name);
	if (!lib)
	{
		Log_Debug("Attempting to load system version of %s", name);
		lib = Base_LoadLibrary(name);
		if (!lib)
		{
			Log_Error("Failed to load library %s!", name);
			return nullptr;
		}
	}
	
	return lib;
}
