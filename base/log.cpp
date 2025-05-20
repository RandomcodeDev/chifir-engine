/// \file Logging
/// \copyright 2025 Randomcode Developers

#include "base/log.h"
#include "base/async.h"
#include "base/base.h"
#include "base/basicstr.h"
#include "base/filesystem.h"
#include "base/platform.h"
#include "base/types.h"
#include "base/vector.h"

static IMutex* s_logMutex;
static CVector<ILogWriter*> s_writers;

void Log_Init()
{
	if (!s_logMutex)
	{
		s_logMutex = Async_CreateMutex();
	}
}

void Log_Shutdown()
{
	if (s_logMutex)
	{
		delete s_logMutex;
		s_logMutex = nullptr;
	}
}

BASEAPI CFileLogWriter::CFileLogWriter(IWritableFilesystem* filesystem, cstr logName, bool addDate) : m_filesystem(filesystem)
{
	if (addDate)
	{
		// TODO
	}
	else {}
	m_filename = Base_StrClone(logName);
}

BASEAPI CFileLogWriter::~CFileLogWriter()
{
	Base_Free(m_filename);
}

BASEAPI void CFileLogWriter::Write(const LogMessage& message)
{
	if (m_filesystem->IsWriteSafe())
	{
		m_filesystem->Write(m_filename, message.formatted, message.formattedLength);
	}
}

BASEAPI void CConsoleLogWriter::Write(const LogMessage& message)
{
	Plat_WriteConsole(Plat_ConsoleHasColor() ? message.formattedColor : message.formatted);
}

BASEAPI void Log_AddWriter(ILogWriter* writer)
{
	if (writer)
	{
		ASSERT_MSG(s_logMutex != nullptr, "Call Base_Init first!");
		s_logMutex->Lock();
		s_writers.Add(writer);
		s_logMutex->Unlock();
	}
}

BASEAPI void Log_Write(const LogMessage& message)
{
	ASSERT_MSG(s_logMutex != nullptr, "Call Base_Init first!");

	// give up after 50ms, probably doesn't matter
	if (s_logMutex->TryLock(50))
	{
		for (ssize i = 0; i < s_writers.Size(); i++)
		{
			if (message.level >= s_writers[i]->m_minLevel)
			{
				s_writers[i]->Write(message);
			}
		}
		s_logMutex->Unlock();
	}
}

BASEAPI void Log_Write(
	LogLevel level, uptr location, bool isAddress, cstr file, cstr function, cstr threadName, u64 threadId, cstr message, ...)
{
	va_list args;
	va_start(args, message);
	dstr formatted = Base_VStrFormat(message, args);
	va_end(args);
	if (!formatted)
	{
		return;
	}

	if (!threadName)
	{
		threadName = Async_GetCurrentThreadName();
		if (!threadName)
		{
			threadName = "<unnamed thread>";
		}
	}

	if (threadId == UINT64_MAX)
	{
		threadId = Async_GetCurrentThreadId();
	}

	// strip repo path from log messages, to make them shorter
	DateTime time;
	LogMessage messageData = {Clamp(level, LogLevel::Trace, LogLevel::FatalError),
							  location,
							  isAddress,
							  file,
							  function,
							  threadName,
							  threadId,
							  time,
							  formatted};
	ssize pos = Base_StrFind(file, REPO_NAME, true);
	if (pos >= 0)
	{
		// chifir-engine[\/] is 14 characters, and there's always gonna be a slash
		messageData.file = file + pos + 14;
	}

	// preformat for every writer to save memory etc
	dstr fullFormatted = Base_StrFormat(
		messageData.isAddress ? "[%s]\t[%s] [%s %llu] [%s!0x%zX %s] %s\r\n" : "[%s]\t[%s] [%s %llu] [%s:%zu %s] %s\r\n",
		ILogWriter::LEVEL_NAMES[s32(messageData.level)], Base_FormatDateTime(messageData.time).Data(), messageData.threadName,
		messageData.threadId, messageData.file, messageData.location, messageData.function, messageData.message);
	dstr fullFormattedColor = Base_StrFormat(
		messageData.isAddress ? "[%s]\t[%s] [%s %llu] [%s!0x%zX %s] %s\r\n" : "[%s]\t[%s] [%s %llu] [%s:%zu %s] %s\r\n",
		ILogWriter::LEVEL_COLORED_NAMES[s32(messageData.level)], Base_FormatDateTime(messageData.time).Data(),
		messageData.threadName, messageData.threadId, messageData.file, messageData.location, messageData.function,
		messageData.message);

	messageData.formatted = fullFormatted;
	messageData.formattedLength = Base_StrLength(fullFormatted);
	messageData.formattedColor = fullFormattedColor;
	messageData.formattedColorLength = Base_StrLength(fullFormattedColor);

	Log_Write(messageData);

	Base_Free(fullFormattedColor);
	Base_Free(fullFormatted);
	Base_Free(formatted);
}
