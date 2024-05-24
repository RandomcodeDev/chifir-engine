#include "CCommandLine.h"

CCommandLine::CCommandLine(int argc, char* argv[]) : m_name(argv[0]), m_args(argc)
{
	uint32_t count = argc;
	for (uint32_t i = 1; i < count; i++)
	{
		m_args[i] = argv[i];
	}
}

CCommandLine::CCommandLine(const std::string& cmdLine)
{
	// This logic is based on ReactOS's CommandLineToArgvW

	// executable path ends at next space, or next quote if it starts with one
	auto source = cmdLine.begin();
	auto end = source;
	if (*end == '"')
	{
		source = end + 1;
		while (source <= cmdLine.end())
		{
			if (*source == '"')
			{
				source++;
				break;
			}
			m_name += *source++;
		}
	}
	else
	{
		while (end <= cmdLine.end() && !std::isblank(*end))
		{
			end++;
		}
		source = end;
		if (source <= cmdLine.end())
		{
			source++;
		}
	}

	while (std::isblank(*source) && source <= cmdLine.end())
	{
		source++;
	}
	if (source >= cmdLine.end())
	{
		return;
	}

	size_t escapeCount = 0;
	size_t quoteCount = 0;
	size_t argCount = 0;
	std::string arg;
	while (source <= cmdLine.end())
	{
		if (std::isblank(*source) && quoteCount == 0)
		{
			m_args.push_back(arg);
			arg.clear();
			escapeCount = 0;
			do
			{
				source++;
			} while (std::isblank(*source) && source < cmdLine.end());
		}
		else if (*source == '\\')
		{
			arg += *source++;
			escapeCount++;
		}
		else if (*source == '"')
		{
			if ((escapeCount & 1) == 0)
			{
				arg.erase(arg.end() - escapeCount / 2, arg.end());
				quoteCount++;
			}
			else
			{
				arg.erase(arg.end() - escapeCount / 2, arg.end());
				arg += '"';
			}

			source++;
			escapeCount = 0;

			while (*source == '"')
			{
				if (++quoteCount == 3)
				{
					arg += '"';
					quoteCount = 0;
				}
				source++;
			}

			if (quoteCount == 2)
			{
				quoteCount = 0;
			}
		}
		else
		{
			arg += *source++;
			escapeCount = 0;
		}
	}

	m_args.push_back(arg);
}

CCommandLine::CCommandLine(const std::vector<std::string>& args)
{
	m_args.resize(args.size());
	std::copy(args.begin(), args.end(), m_args.begin());
}
