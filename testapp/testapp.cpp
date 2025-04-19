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
	UNUSED(systems);
	
	libs.Add(LIBS, ArraySize(LIBS));
}

s32 ThreadFunc(void* data)
{
	Log_Info("Thread running!");
	*reinterpret_cast<u32*>(data) = 69;
	return 420;
}

s32 CTestApp::Run(const CVector<ISystem*>& systems)
{
	UNUSED(systems);

	Log_Info("Running test app");

	u32 test = 0;
	IThread* thread = Async_CreateThread(ThreadFunc, &test, "Test");
	Log_Info("Starting thread");
	thread->Run();
	Log_Info("Waiting for thread %u", thread->GetId());
	thread->Wait();
	Log_Info("Thread returned %d, data = %u", thread->GetResult(), test);

	return 0;
}
