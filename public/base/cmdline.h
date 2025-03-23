/// Command line parsing (WIP)
/// Copyright 2025 Randomcode Developers

#pragma once

#include "base/string.h"
#include "base/types.h"
#include "base/vector.h"

class CCommandLine
{
  public:
	CCommandLine();
	CCommandLine(const CString& cmdline);
	CCommandLine(s32 argc, cstr* argv);
	CCommandLine(const CVector<CString>& args);
};
