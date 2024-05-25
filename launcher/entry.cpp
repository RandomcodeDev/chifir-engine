#include "framework/Framework.h"
#include "framework/CCommandLine.h"
#include "framework/IPlatform.h"
#include "framework/Log.h"

#if defined KR_PLATFORM_WINDOWS && !defined KR_DEBUG
extern "C" int WinMain(HINSTANCE instance, HINSTANCE prevInstance, char* cmdline, int show)
{
	CCommandLine cmdLine(GetCommandLineA());
#else
extern "C" int main(int argc, char* argv[])
{
	CCommandLine cmdLine(argc, argv);
#endif

	GetPlatform()->Initialize();

	KR_LOG_INFO("Running on {}", GetPlatform()->DescribeOs());

	GetPlatform()->Shutdown();

	return 0;
}
