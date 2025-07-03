#pragma once

#include "iapplication.h"

#include "base/string.h"
#include "base/vector.h"

class CTestApp: public IApplication
{
  public:
	CTestApp() = default;
	~CTestApp() = default;

	virtual void Setup(const CVector<CString>& args);
	virtual void GetDependencies(CVector<SystemDependency>& systems, CVector<LibDependency_t>& libs);
	virtual s32 Run(const CVector<ISystem*>& systems);

  private:
    CVector<CString> m_args;

	static constexpr LibDependency_t LIBS[] = {
		{"RustTest", true}
    };
};
