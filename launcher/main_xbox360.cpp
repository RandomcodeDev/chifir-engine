#include <xtl.h>

int main()
{
	HMODULE launcher = LoadLibraryA(GAME_NAME ".xex");
	if (!launcher)
	{
		// TODO: figure out how to display an error
		return HRESULT_FROM_WIN32(GetLastError());
	}

	static const UINT16 LAUNCHERMAIN_ORDINAL = 1;

	// By ordinal
	FARPROC launcherMainAddr = GetProcAddress(launcher, (LPCSTR)LAUNCHERMAIN_ORDINAL);
	if (!launcherMainAddr)
	{
		return HRESULT_FROM_WIN32(GetLastError());
	}

	int (*LauncherMain)() = reinterpret_cast<int (*)()>(launcherMainAddr);
	int result = LauncherMain();

	return result;
}
