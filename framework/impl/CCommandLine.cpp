#include "framework/CCommandLine.h"
#include "framework/IPlatform.h"
#include "framework/Util.h"

CCommandLine::CCommandLine(int argc, char* argv[]) : m_name(argv[0])
{
	std::vector<std::string> args(argc);

	u32 count = argc;
	for (u32 i = 1; i < count; i++)
	{
		args[i] = argv[i];
	}

	Parse(args);
}

CCommandLine::CCommandLine(const std::string& cmdLine)
{
	std::vector<std::string> args;

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

	usize escapeCount = 0;
	usize quoteCount = 0;
	std::string arg;
	while (source <= cmdLine.end())
	{
		if (std::isblank(*source) && quoteCount == 0)
		{
			args.push_back(arg);
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

	args.push_back(arg);

	Parse(args);
}

CCommandLine::CCommandLine(const std::vector<std::string>& args)
{
	Parse(args);
}

void CCommandLine::Parse(const std::vector<std::string>& args)
{
	for (usize i = 0; i < args.size() - 1; i++)
	{
		if (std::strchr(OPTION_PREFIXES, args[i][0]) && !std::strchr(OPTION_PREFIXES, args[i + 1][0]))
		{
			std::string option = args[i];
			option.erase(option.begin());
			m_args[option] = args[i + 1];
			i++;
		}
	}
}

const std::string& CCommandLine::GetArg(const std::string& name) const
{
	if (HasArg(name))
	{
		return m_args.at(name);
	}
	else
	{
		KR_QUIT("Call HasArg to check for arguments!");
	}
}
