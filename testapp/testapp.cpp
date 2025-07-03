#include "base/allocator.h"
#include "base/log.h"
#include "base/string.h"

#include "math/math.h"

#include "testapp.h"

void CTestApp::Setup(const CVector<CString>& args)
{
    m_args = args;

	Log_Info("Got %zd arguments:", m_args.Size());
	for (ssize i = 0; i < m_args.Size(); i++)
	{
		Log_Info("\t%s\n", m_args[i].Data());
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

    f32 x = 0.0f;
    for (ssize i = 0; i < m_args.Size(); i++)
    {
        char c = m_args[i][0];
        if (c == '-' && m_args[i].Length() > 1)
        {
            c = m_args[i][1];
        }

        if (Base_IsDecDigit(c))
        {
            x = (f32)m_args[i].ParseFloat();
            break;
        }
    }

	Log_Info("x = %f", x);
    Log_Info("sqrt(x) = %f", Sqrt(x));
    Log_Info("sin(x) = %f", Sin(x));
    Log_Info("cos(x) = %f", Cos(x));
    Log_Info("tan(x) = %f", Tan(x));
    Log_Info("asin(x) = %f", Asin(x));
    Log_Info("acos(x) = %f", Acos(x));
    Log_Info("atan(x) = %f", Atan(x));

	return 0;
}
