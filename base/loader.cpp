#include "base/loader.h"
#include "base/log.h"
#include "base/platform.h"
#include "base/string.h"

BASEAPI ILibrary* Base_LoadEngineLibrary(cstr name)
{
    CString libName = CString::FormatStr("%s/bin/%s", Plat_GetEngineDir(), name);
    Log_Debug("Attempting to load engine library %s from %s", name, libName.Data());
    ILibrary* lib = Base_LoadLibrary(libName.Data());
	if (!lib)
	{
		Log_Warning("Failed to load from %s, retrying", libName.Data());
		libName.Format("%s/%s", Plat_GetEngineDir(), name);
		Log_Debug("Attempting to load engine library %s from %s", name, libName.Data());
		lib = Base_LoadLibrary(libName.Data());
		if (!lib)
		{
			Log_Error("Failed to load engine library %s!", name);
            return nullptr;
		}
	}

    return lib;
}
