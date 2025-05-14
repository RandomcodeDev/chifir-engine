#include "testapp.h"
#include "base/allocator.h"
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

	CArenaAllocator allocator(32);
	int* ints = (int*)allocator.Alloc(4 * sizeof(int));
	allocator.Free(ints, 4 * sizeof(int));

	return 0;
}
