#include "CWindowsSharedLibrary.h"

CWindowsSharedLibrary::CWindowsSharedLibrary(const std::string& name, HMODULE handle) : m_name(name), m_handle(handle)
{
}

CWindowsSharedLibrary::~CWindowsSharedLibrary()
{
	FreeLibrary(m_handle);
}

void* CWindowsSharedLibrary::GetSymbol(const std::string& name) const
{
	return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(GetProcAddress(m_handle, name.c_str())));
}
