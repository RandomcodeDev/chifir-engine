/// @file CConfigVar.h
/// @brief This file declares a class for configuration variables
/// @copyright (c) 2024 Randomcode Developers. See LICENSE.

#pragma once

#include "Framework.h"

class FRAMEWORK_API CConfigVar
{
  public:
	CConfigVar(const std::string& name, const std::string& defaultValue);
	CConfigVar(const std::string& name, s64 defaultValue, s64 min = INT64_MIN, s64 max = INT64_MAX);
	CConfigVar(const std::string& name, double defaultValue, double min = DBL_MIN, double max = DBL_MAX);
	CConfigVar(const std::string& name, bool defaultValue);

	void Set(const std::string& value);
	void Set(s64 value);
	void Set(double value);
	void Set(bool value);

	void Reset()
	{
		m_value = m_default;
	}

	const std::string& GetString() const
	{
		return m_value;
	}

	s64 GetInt() const
	{
		return std::stoll(m_value);
	}

	double GetDouble() const
	{
		return std::stod(m_value);
	}

	bool GetBool() const
	{
		return std::stoi(m_value) != 0;
	}

	const std::string& GetName() const
	{
		return m_name;
	}

  private:
	std::string m_name;
	std::string m_value;
	std::string m_default;
	double m_min;
	double m_max;
};
