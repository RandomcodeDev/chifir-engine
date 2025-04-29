#include "testapp.h"
#include "base/async.h"
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

void CTestApp::GetDependencies(CVector<SystemDependency>& systems, CVector<LibDependency_t>& libs)
{
	UNUSED(libs);
	UNUSED(systems);

	//libs.Add(LIBS, ArraySize(LIBS));
}

s32 CTestApp::Run(const CVector<ISystem*>& systems)
{
	UNUSED(systems);

	Log_Info("Running test app");

	u32 values[] = {1, 2, 3, 5, 8, 10, 23, 45, 232};
	auto cmpInt = [](const u32& a, const u32& b, void* userData) -> s32 {
		Log_Debug("a=%u b=%u", a, b);
		return a == b ? 0 : (a < b ? -1 : 1);
	};
	ssize index = Base_Search<u32>(23, values, ArraySize(values), cmpInt);
	Log_Info("Index of 23 is %zd", index);

	return 0;
}
