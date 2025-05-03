/// \file Windows argument parsing
/// \copyright 2025 Randomcode Developers

#include "base.h"
#include "base/string.h"
#include "base/types.h"
#include "base/vector.h"
#include "platform_win32.h"

// this function is long and annoying
BASEAPI void Plat_GetArgs(CVector<CString>& args)
{
	// Based on ReactOS's CommandLineToArgvW

	static CVector<CString> s_args;

	if (!s_args.Size())
	{
		u32 argc;
		cstr s;
		dstr d;
		dstr cmdline;
		s32 quoteCount, backslashCount;

		CVector<cstr> s_argv;

		ANSI_STRING ansiCmdline;
		RtlUnicodeStringToAnsiString(&ansiCmdline, &NtCurrentPeb()->ProcessParameters->CommandLine, true);

		cmdline = Base_StrClone(ansiCmdline.Buffer, ansiCmdline.Length);

		/* --- First count the arguments */
		argc = 1;
		s = ansiCmdline.Buffer;
		/* The first argument, the executable path, follows special rules */
		if (*s == '"')
		{
			/* The executable path ends at the next quote, no matter what */
			s++;
			while (*s)
				if (*s++ == '"')
					break;
		}
		else
		{
			/* The executable path ends at the next space, no matter what */
			while (*s && !Base_IsWhitespace(*s))
				s++;
		}
		/* skip to the first argument, if any */
		while (Base_IsWhitespace(*s))
			s++;
		if (*s)
			argc++;

		/* Analyze the remaining arguments */
		quoteCount = backslashCount = 0;
		while (*s)
		{
			if (Base_IsWhitespace(*s) && quoteCount == 0)
			{
				/* skip to the next argument and count it if any */
				while (Base_IsWhitespace(*s))
					s++;
				if (*s)
					argc++;
				backslashCount = 0;
			}
			else if (*s == '\\')
			{
				/* '\', count them */
				backslashCount++;
				s++;
			}
			else if (*s == '"')
			{
				/* '"' */
				if ((backslashCount & 1) == 0)
					quoteCount++; /* unescaped '"' */
				s++;
				backslashCount = 0;
				/* consecutive quotes, see comment in copying code below */
				while (*s == '"')
				{
					quoteCount++;
					s++;
				}
				quoteCount = quoteCount % 3;
				if (quoteCount == 2)
					quoteCount = 0;
			}
			else
			{
				/* a regular character */
				backslashCount = 0;
				s++;
			}
		}

		// reserve space for arguments
		s_argv.Resize(argc);

		/* --- Then split and copy the arguments */
		d = cmdline;
		s_argv[0] = d;
		argc = 1;
		/* The first argument, the executable path, follows special rules */
		if (*d == '"')
		{
			/* The executable path ends at the next quote, no matter what */
			s = d + 1;
			while (*s)
			{
				if (*s == '"')
				{
					s++;
					break;
				}
				*d++ = *s++;
			}
		}
		else
		{
			/* The executable path ends at the next space, no matter what */
			while (*d && !Base_IsWhitespace(*d))
				d++;
			s = d;
			if (*s)
				s++;
		}
		/* close the executable path */
		*d++ = 0;

		/* skip to the first argument and initialize it if any */
		while (Base_IsWhitespace(*s))
			s++;
		if (!*s)
		{
			/* There are no parameters so we are all done */
			goto Cleanup;
		}

		/* Split and copy the remaining arguments */
		d = cmdline + (s - cmdline);
		s_argv[argc++] = d;
		quoteCount = backslashCount = 0;
		while (*s)
		{
			if (Base_IsWhitespace(*s) && quoteCount == 0)
			{
				/* close the argument */
				*d++ = 0;
				backslashCount = 0;

				/* skip to the next one and initialize it if any */
				do
				{
					s++;
				} while (Base_IsWhitespace(*s));
				if (*s)
					s_argv[argc++] = d;
			}
			else if (*s == '\\')
			{
				*d++ = *s++;
				backslashCount++;
			}
			else if (*s == '"')
			{
				if ((backslashCount & 1) == 0)
				{
					/* Preceded by an even number of '\', this is half that
					 * number of '\', plus a quote which we erase.
					 */
					d -= backslashCount / 2;
					quoteCount++;
				}
				else
				{
					/* Preceded by an odd number of '\', this is half that
					 * number of '\' followed by a '"'
					 */
					d = d - backslashCount / 2 - 1;
					*d++ = '"';
				}
				s++;
				backslashCount = 0;
				/* Now count the number of consecutive quotes. Note that qcount
				 * already takes into account the opening quote if any, as well as
				 * the quote that lead us here.
				 */
				while (*s == '"')
				{
					if (++quoteCount == 3)
					{
						*d++ = '"';
						quoteCount = 0;
					}
					s++;
				}
				if (quoteCount == 2)
					quoteCount = 0;
			}
			else
			{
				/* a regular character */
				*d++ = *s++;
				backslashCount = 0;
			}
		}
		*d = '\0';

	Cleanup:
		s_args.Resize(argc);
		for (ssize i = 0; i < s_argv.Size(); i++)
		{
			s_args[i] = s_argv[i];
		}

		Base_Free(cmdline);
		RtlFreeAnsiString(&ansiCmdline);
	}

	args.Empty();
	args.Add(s_args);
}
