// Miscellaneous utility functions

#include "isystem.h"
#include "base/loader.h"
#include "utility/utility.h"

UTILAPI ISystem* GetSystem(ILibrary* library, u32 minVersion)
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
