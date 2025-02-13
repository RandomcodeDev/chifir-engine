#include "base/loader.h"
#include "base/log.h"
#include "base/platform.h"
#include "base/string.h"

// Try the most likely path first, in retail bin is more likely, in a developer build it's probably flat
#ifdef CH_RETAIL
#define FIRST_PATH "%s/bin/%s", Plat_GetEngineDir(), name
#define SECOND_PATH "%s/%s", Plat_GetEngineDir(), name
#else
#define FIRST_PATH "%s/%s", Plat_GetEngineDir(), name
#define SECOND_PATH "%s/bin/%s", Plat_GetEngineDir(), name
#endif

BASEAPI ILibrary* Base_LoadEngineLibrary(cstr name)
{
#ifdef CH_UNIX
    Log_Debug("Attempting to load engine library %s", name);
	return Base_LoadLibrary(name);
#else
    CString libName = CString::FormatStr(FIRST_PATH);
    Log_Debug("Attempting to load engine library %s from %s", name, libName.Data());
    ILibrary* lib = Base_LoadLibrary(libName.Data());
	if (!lib)
	{
		Log_Warning("Failed to load from %s, retrying", libName.Data());
		libName.Format(SECOND_PATH);
		Log_Debug("Attempting to load engine library %s from %s", name, libName.Data());
		lib = Base_LoadLibrary(libName.Data());
		if (!lib)
		{
			Log_Error("Failed to load engine library %s!", name);
            return nullptr;
		}
	}

    return lib;
#endif
}
