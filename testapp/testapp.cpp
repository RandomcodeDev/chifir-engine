#include "base/allocator.h"
#include "base/log.h"
#include "base/string.h"

#include "math/vec3.h"

#include "testapp.h"

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

	Vec3f a(1, 2, 3);
    Vec3f b(2, 4, 6);

    Log_Info("a = [%f %f %f], |a| = %f", a[0], a[1], a[2], a.Length());
    Log_Info("b = [%f %f %f], |b| = %f", b[0], b[1], b[2], b.Length());
    Vec3f a2 = a * 2;
    Log_Info("a * 2 = [%f %f %f]", a2[0], a2[1], a2[2]);
    Vec3f b2 = b / 2;
    Log_Info("b / 2 = [%f %f %f]", b2[0], b2[1], b2[2]);
    Log_Info("a * b = %f", a.Dot(b));
    Vec3f c = a.Cross(b);
    Log_Info("a x b = [%f %f %f]", c[0], c[1], c[2]);

	return 0;
}
