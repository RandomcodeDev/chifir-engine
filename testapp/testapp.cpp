#include "testapp.h"
#include "base/log.h"
#include "base/string.h"

void CTestApp::Setup(const CVector<CString>& args)
{
	Log_Info("Got %zd arguments:", args.Size());
	for (ssize i = 0; i < args.Size(); i++)
	{
		Log_Info("\t%s\n", args[i].Data());
	}
}

void CTestApp::GetRequiredSystems(CVector<SystemDependency_t>& dependencies)
{
	(void)dependencies;
}

s32 CTestApp::Run(const CVector<ISystem*>& systems)
{
	(void)systems;

	Log_Info("Running test app");

	CString string = "a s d f";
	CVector<CString> parts = string.Split();

	return 0;
}
