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

	CString string = "asdf";
	Log_Info("%s %zu %zu", string.Data(), string.Size(), string.Length());

	string += "asdf";
	string.Add("asdf", 2);
	string.Delete(2);
	string = "asdf";
	CString other = string * 5;

	return 0;
}