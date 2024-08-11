// Logging system

#pragma once

#include "base/compiler.h"
#include "base/string.h"
#include "base/types.h"
#include "utility/utility.h"

// Log message priority level
enum LogLevel_t
{
	LogLevelTrace,
	LogLevelDebug,
	LogLevelInfo,
	LogLevelWarning,
	LogLevelError,
	LogLevelFatalError
};

// Represents a log message
struct LogMessage_t
{
	LogLevel_t level;
	uptr location;
	bool isAddress; // Whether location should be displayed in hex
	cstr file;
	cstr function;
	cstr message;
};

// Log writer interface
class UTILAPI ILogWriter
{
  public:
	virtual ~ILogWriter() DEFAULT;

	// Write a message
	virtual void Write(const LogMessage_t& message) = 0;
};

#ifdef CH_WIN32
// DbgPrint log writer
class UTILAPI CDbgPrintLogWriter : public ILogWriter
{
  public:
	void Write(const LogMessage_t& message);
};
#endif

// Add a log writer
extern UTILAPI void Log_AddWriter(ILogWriter* writer);

// Write a log message
extern UTILAPI void Log_Write(const LogMessage_t& message);

// Write a log message, and format it
extern UTILAPI void Log_Write(LogLevel_t level, uptr location, bool isAddress, cstr file, cstr function, cstr message, ...);

#define Log_Message(level, ...) Log_Write(LogLevel##level, __LINE__, false, __FILE__, FUNCTION_NAME, __VA_ARGS__)
#define Log_Trace(...)          Log_Message(Trace, __VA_ARGS__)
#define Log_Debug(...)          Log_Message(Debug, __VA_ARGS__)
#define Log_Info(...)           Log_Message(Info, __VA_ARGS__)
#define Log_Warning(...)        Log_Message(Warning, __VA_ARGS__)
#define Log_Error(...)          Log_Message(Error, __VA_ARGS__)
#define Log_FatalError(...)     Log_Message(FatalError, __VA_ARGS__)
