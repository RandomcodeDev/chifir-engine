// Linux platform stuff

#pragma once

#include "base/loader.h"
#include "base/types.h"

class CLinuxLibrary : public ILibrary
{
  public:
	CLinuxLibrary(cstr name, void* base);
	~CLinuxLibrary();

	void* GetSymbol(cstr name);

	cstr GetName()
	{
		return m_name;
	}

  private:
	dstr m_name;
	void* m_base;
};

#define STDIN_FD 0
#define STDOUT_FD 1
#define STDERR_FD 2

extern uptr g_errno;
 
// Make a system call
extern uptr Base_SysCall(uptr number, uptr arg1 = 0, uptr arg2 = 0, uptr arg3 = 0, uptr arg4 = 0, uptr arg5 = 0, uptr arg6 = 0);
