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
 
// Make a system call
extern u64 Base_SysCall(u64 number, u64 arg1 = 0, u64 arg2 = 0, u64 arg3 = 0, u64 arg4 = 0, u64 arg5 = 0, u64 arg6 = 0);
