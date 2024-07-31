#include "utility/log.h"
#include "base/base.h"
#include "base/basicstr.h"
#ifdef CH_WIN32
#include "base/platform.h"
#endif
#include "base/vector.h"
#include "utility/utility.h"

static CVector<ILogWriter*> s_writers;

#ifdef CH_WIN32
extern "C" DLLIMPORT bool DbgPrint_Available();

UTILAPI void CDbgPrintLogWriter::Write(const LogMessage_t& message)
{
	static const cstr LEVEL_NAMES[] = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};

	if (DbgPrint_Available())
	{
		if (message.isAddress)
		{
			DbgPrint(
				"[%s] [0x%llX@%s %s] %s\n", LEVEL_NAMES[message.level], message.location, message.file, message.function,
				message.message);
		}
		else
		{
			DbgPrint(
				"[%s] [%s:%d %s] %s\n", LEVEL_NAMES[message.level], message.file, message.location, message.function,
				message.message);
		}
	}
}
#endif

UTILAPI void Log_AddWriter(ILogWriter* writer)
{
	if (writer)
	{
		s_writers.Add(writer);
	}
}

UTILAPI void Log_Write(const LogMessage_t& message)
{
	for (ssize i = 0; i < s_writers.Size(); i++)
	{
		s_writers[i]->Write(message);
	}
}

UTILAPI void Log_Write(LogLevel_t level, uptr location, bool isAddress, cstr file, cstr function, cstr message, ...)
{
	va_list args;

	va_start(args, message);
	dstr formatted = Base_VStrFormat(message, args);
	va_end(args);
	if (!formatted)
	{
		return;
	}

	// TODO: trim chifir-engine/ and anything before it from file
	// TODO: keep level within valid range
	LogMessage_t messageData = {level, location, isAddress, file, function, formatted};
	Log_Write(messageData);

	Base_Free(formatted);
}
