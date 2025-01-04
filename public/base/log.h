/// Logging system

#pragma once

#include "base/base.h"
#include "base/compiler.h"
#include "base/string.h"
#include "base/types.h"

/// Log message priority level
enum class LogLevel_t : s32
{
	Trace = 0,
	Debug,
	Info,
	Warning,
	Error,
	FatalError
};

/// Represents a log message
struct LogMessage_t
{
	LogLevel_t level;
	DateTime_t time;
	uptr location;
	bool isAddress; /// Whether location should be displayed in hex
	cstr file;
	cstr function;
	cstr message;
};

/// Log writer interface
class BASEAPI ILogWriter
{
  public:
	virtual ~ILogWriter() DEFAULT;

	/// Write a message
	virtual void Write(const LogMessage_t& message) = 0;

  protected:
	/// Indexable with any LogLevel_t value
	static const cstr LEVEL_NAMES[];
	static const cstr LEVEL_COLORED_NAMES[];
};

class IWritableFilesystem;

/// Filesystem log writer
class BASEAPI CFileLogWriter : public ILogWriter
{
  public:
	CFileLogWriter(IWritableFilesystem* filesystem, cstr logName, bool addDate = true);
	~CFileLogWriter();

	void Write(const LogMessage_t& message);

  protected:
	IWritableFilesystem* m_filesystem;
	dstr m_filename;
};

#ifdef CH_WIN32
/// DbgPrint log writer
class BASEAPI CDbgPrintLogWriter : public ILogWriter
{
  public:
	void Write(const LogMessage_t& message);
};
#endif

class BASEAPI CConsoleLogWriter : public ILogWriter
{
  public:
	void Write(const LogMessage_t& message);
};

/// Add a log writer
extern BASEAPI void Log_AddWriter(ILogWriter* writer);

/// Write a log message
extern BASEAPI void Log_Write(const LogMessage_t& message);

/// Write a log message, and format it
extern BASEAPI void Log_Write(LogLevel_t level, uptr location, bool isAddress, cstr file, cstr function, cstr message, ...);

#define Log_Message(level, ...) Log_Write(LogLevel_t::level, __LINE__, false, __FILE__, FUNCTION_NAME, __VA_ARGS__)
#define Log_Trace(...)          Log_Message(Trace, __VA_ARGS__)
#define Log_Debug(...)          Log_Message(Debug, __VA_ARGS__)
#define Log_Info(...)           Log_Message(Info, __VA_ARGS__)
#define Log_Warning(...)        Log_Message(Warning, __VA_ARGS__)
#define Log_Error(...)          Log_Message(Error, __VA_ARGS__)
#define Log_FatalError(...)     Log_Message(FatalError, __VA_ARGS__)

#define LOG_FORMAT(color, msg)                                                                                                   \
	(msg).isAddress ? "[%s] [%s] [%s!0x%llX %s] %s\n" : "[%s] [%s] [%s:%u %s] %s\n",                                             \
		(color) ? LEVEL_COLORED_NAMES[s32((msg).level)] : LEVEL_NAMES[s32((msg).level)], Base_FormatDateTime((msg).time).Data(),           \
		(msg).file, (msg).location, (msg).function, (msg).message
