/// Logging
/// Copyright 2025 Randomcode Developers

#include "base/log.h"
#include "base/base.h"
#include "base/basicstr.h"
#include "base/filesystem.h"
#include "base/platform.h"
#include "base/types.h"
#include "base/vector.h"

static CVector<ILogWriter*> s_writers;

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

BASEAPI void CFileLogWriter::Write(const LogMessage_t& message)
{
	if (m_filesystem->IsWriteSafe())
	{
		dstr formatted = nullptr;
		formatted = Base_StrFormat(LOG_FORMAT(false, message));
		m_filesystem->Write(m_filename, formatted, Base_StrLength(formatted));
		Base_Free(formatted);
	}
}

BASEAPI void CConsoleLogWriter::Write(const LogMessage_t& message)
{
	dstr fullMessage = Base_StrFormat(LOG_FORMAT(Plat_ConsoleHasColor(), message));
	Plat_WriteConsole(fullMessage);
	Base_Free(fullMessage);
}

BASEAPI void Log_AddWriter(ILogWriter* writer)
{
	if (writer)
	{
		s_writers.Add(writer);
	}
}

BASEAPI void Log_Write(const LogMessage_t& message)
{
	for (ssize i = 0; i < s_writers.Size(); i++)
	{
		if (message.level >= s_writers[i]->m_minLevel)
		{
			s_writers[i]->Write(message);
		}
	}
}

BASEAPI void Log_Write(LogLevel_t level, uptr location, bool isAddress, cstr file, cstr function, cstr message, ...)
{
	va_list args;
	va_start(args, message);
	dstr formatted = Base_VStrFormat(message, args);
	va_end(args);
	if (!formatted)
	{
		return;
	}

	// strip repo path from log messages, to make them shorter
	DateTime_t time;
	LogMessage_t messageData = {
		Clamp(level, LogLevel_t::Trace, LogLevel_t::FatalError), location, isAddress, file, function, formatted, time};
	ssize pos = Base_StrFind(file, REPO_NAME, true);
	if (pos >= 0)
	{
		// chifir-engine[\/] is 14 characters, and there's always gonna be a slash
		messageData.file = file + pos + 14;
	}

	Log_Write(messageData);

	Base_Free(formatted);
}
