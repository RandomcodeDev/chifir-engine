/// @file Util.h
/// @brief This file declares miscellaneous useful functions
/// @copyright (c) 2024 Randomcode Developers. See LICENSE.

#pragma once

#include "Framework.h"

/// @brief Get the size of an array
template <class T, usize N> constexpr usize KR_ARRAY_SIZE(T (&)[N])
{
	return N;
}

/// @brief Quit the program
#define KR_QUIT(...) GetPlatform()->Quit(fmt::format(__VA_ARGS__), true, __FILE__, __FUNCTION__, __LINE__)

static inline void Replace(std::string& string, const std::string& oldStr, const std::string& newStr)
{
	size_t offset = 0;
	while ((offset = string.find(oldStr, offset)) != SIZE_MAX)
	{
		string.replace(offset, oldStr.length(), newStr);
	}
}

#ifdef KR_PLATFORM_WINDOWS
static inline std::string GetHresultString(HRESULT result)
{
	_com_error error(result);
	return fmt::format("{} (HRESULT 0x{:08X})", error.ErrorMessage(), result);
}
#endif

static inline std::string ToLower(const std::string& str)
{
	std::string lower;
	std::transform(str.begin(), str.end(), lower.begin(), [](u8 c) { return std::tolower(c); });
	return lower;
}

static inline std::string ToUpper(const std::string& str)
{
	std::string upper;
	std::transform(str.begin(), str.end(), upper.begin(), [](u8 c) -> u8 { return (u8)std::toupper(c); });
	return upper;
}

// TODO: make less bad if not only used for ConfigVars
static inline bool ParseBool(const std::string& str)
{
	std::string lower = ToLower(str);
	if (lower == "true" || lower == "yes" || lower == "on" || std::stoi(lower) != 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}
