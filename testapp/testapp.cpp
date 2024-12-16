#include "testapp.h"
#include "base/log.h"
#include "base/string.h"

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
