/// @file CWindowsSharedLibrary.h
/// @brief This file defines the Windows implementation of ISharedLibrary
/// @copyright (c) 2024 Randomcode Developers. See LICENSE.

#pragma once

#include "framework/Framework.h"
#include "framework/ISharedLibrary.h"

class CWindowsSharedLibrary : public ISharedLibrary
{
  public:
	CWindowsSharedLibrary(const std::string& name, HMODULE module);
	~CWindowsSharedLibrary();
	void* GetSymbol(const std::string& name) const;

	const std::string& GetName() const
	{
		return m_name;
	}

	u64 GetHandle() const
	{
		return reinterpret_cast<u64>(m_handle);
	}

  private:
	std::string m_name;
	HMODULE m_handle;
};
