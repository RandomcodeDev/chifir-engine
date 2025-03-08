/// Null platform stuff

#pragma once

// TODO: error for wrong platform
//#ifndef CH_<FIXME>
//#error "This header is <FIXME> only"
//#endif

#include "base/base.h"
#include "base/loader.h"
#include "base/types.h"

class CNullLibrary: public ILibrary
{
  public:
	CNullLibrary(cstr name, void* base);
	~CNullLibrary();

	void* GetSymbol(cstr name);

	cstr GetName()
	{
		return m_name;
	}

	uptr GetBase()
	{
		return reinterpret_cast<uptr>(m_base);
	}

  private:
	dstr m_name;
	void* m_base;
};
