#pragma once

#include "iapplication.h"

class CTestApp: public IApplication
{
  public:
	CTestApp() DEFAULT;
	~CTestApp() DEFAULT;

	void Setup(const CVector<CString>& args);
	void GetRequiredSystems(CVector<SystemDependency_t>& dependencies);
	s32 Run(const CVector<ISystem*>& systems /* TODO: , CCommandLine cmdLine */);
};
