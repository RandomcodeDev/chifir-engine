// Miscellaneous utility functions

#include "utility/utility.h"
#include "base/loader.h"
#include "isystem.h"
#include "utility/log.h"

UTILAPI NORETURN void Util_Fatal(cstr message, ...)
{
	va_list args;

	va_start(args, message);
	cstr formatted = Base_VStrFormat(message, args);
	va_end(args);
	if (!formatted)
	{
		formatted = message;
	}

	Log_FatalError("%s", formatted);
	Base_QuitSafe(1, formatted);
}

UTILAPI ISystem* Util_GetSystem(ILibrary* library, u32 minVersion)
{
	CreateSystemInterface_t CreateInterface = library->GetSymbol<CreateSystemInterface_t>("CreateInterface");
	if (!CreateInterface)
	{
		return nullptr;
	}

	ISystem* system = CreateInterface();
	if (!system)
	{
		return nullptr;
	}

	if (system->GetVersion() < minVersion)
	{
		delete system;
		return nullptr;
	}

	return system;
}
