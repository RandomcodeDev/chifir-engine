#include "framework/CConfigVar.h"

CConfigVar::CConfigVar(const std::string& name, const std::string& defaultValue) : m_name(name), m_default(defaultValue), m_value(m_default)
{
	m_min = DBL_MIN;
	m_max = DBL_MAX;
}

CConfigVar::CConfigVar(const std::string& name, s64 defaultValue, s64 min, s64 max) : CConfigVar(name, std::to_string(defaultValue))
{
	m_min = min;
	m_max = max;
}

CConfigVar::CConfigVar(const std::string& name, double defaultValue, double min, double max) : CConfigVar(name, std::to_string(defaultValue))
{
	m_min = min;
	m_max = max;
}

CConfigVar::CConfigVar(const std::string& name, bool defaultValue) : CConfigVar(name, std::to_string(defaultValue))
{
}

void CConfigVar::Set(const std::string& value)
{
	m_value = value;
}

void CConfigVar::Set(s64 value)
{
	Set(std::to_string(value));
}

void CConfigVar::Set(double value)
{
	Set(std::to_string(value));
}

void CConfigVar::Set(bool value)
{
	Set(std::to_string(value));
}
