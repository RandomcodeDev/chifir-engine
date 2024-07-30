#include <xtl.h>

int main(void)
{
	HMODULE launcher = LoadLibraryA("Launcher.xex");
	if (!launcher)
	{
		// TODO: figure out how to display an error
		abort();
	}

	static const UINT16 LAUNCHERMAIN_ORDINAL = 1;

	// By ordinal
	void* launcherMainAddr = GetProcAddress(launcher, (LPCSTR)LAUNCHERMAIN_ORDINAL);
	if (!launcherMainAddr)
	{
		abort();
	}

	int (*LauncherMain)() = reinterpret_cast<int(*)()>(launcherMainAddr);
	int result = LauncherMain();

	return result;
}

