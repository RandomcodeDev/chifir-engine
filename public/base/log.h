/// \file Logging system
/// \copyright Randomcode Developers

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
	uptr location;
	bool isAddress; /// Whether location should be displayed in hex
	cstr file;
	cstr function;
	cstr message;
	DateTime_t time;
};

/// Log writer interface
class BASEAPI ILogWriter
{
  public:
	virtual ~ILogWriter() DEFAULT;

	/// Write a message
	virtual void Write(const LogMessage_t& message) = 0;

	/// Set the level that messages have to be at or above to be logged
	LogLevel_t SetLevel(LogLevel_t level)
	{
		LogLevel_t old = m_minLevel;
		m_minLevel = level;
		return old;
	}

  protected:
	friend void BASEAPI Log_Write(const LogMessage_t& message);

	LogLevel_t m_minLevel =
#ifdef CH_DEBUG
		LogLevel_t::Trace;
#else
		LogLevel_t::Info;
#endif

	/// Indexable with any LogLevel_t value
	static constexpr cstr LEVEL_NAMES[] = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};
	static constexpr cstr LEVEL_COLORED_NAMES[] = {
		"\x1b[38;5;126mT\x1b[38;5;161mR\x1b[38;5;231mA\x1b[38;5;208mC\x1b[38;5;196mE\x1b[0m", // lesbian flag
		"\x1b[38;5;123mD\x1b[38;5;213mE\x1b[38;5;231mB\x1b[38;5;213mU\x1b[38;5;123mG\x1b[0m", // trans flag
		"\x1b[38;5;118mINFO\x1b[0m", // light lime green
		"\x1b[38;5;214mWARN\x1b[0m", // ochre?
		"\x1b[38;5;196mERROR\x1b[0m", // bright red
		"\x1b[30m\x1b[48;5;124mFATAL\x1b[0m"}; // bright red background
};

class IWritableFilesystem;

/// Filesystem log writer
class BASEAPI CFileLogWriter: public ILogWriter
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
class BASEAPI CDbgPrintLogWriter: public ILogWriter
{
  public:
	void Write(const LogMessage_t& message);
};
#endif

class BASEAPI CConsoleLogWriter: public ILogWriter
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

#ifdef CH_RETAIL
#define Log_Message(level, ...) Log_Write(LogLevel_t::level, __LINE__, false, __FILE__, FUNCTION_NAME, __VA_ARGS__)
#else
#define Log_Message(level, ...) Log_Write(LogLevel_t::level, __LINE__, false, __FILE__, FUNCTION_SIGNATURE, __VA_ARGS__)
#endif
#define Log_Trace(...)          Log_Message(Trace, __VA_ARGS__)
#define Log_Debug(...)          Log_Message(Debug, __VA_ARGS__)
#define Log_Info(...)           Log_Message(Info, __VA_ARGS__)
#define Log_Warning(...)        Log_Message(Warning, __VA_ARGS__)
#define Log_Error(...)          Log_Message(Error, __VA_ARGS__)
#define Log_FatalError(...)     Log_Message(FatalError, __VA_ARGS__)

/// Produces printf arguments for the given log message. `color` decides whether ANSI escapes are used.
#define LOG_FORMAT(color, msg)                                                                                                   \
	(msg).isAddress ? "[%s] [%s] [%s!0x%llX %s] %s\r\n" : "[%s] [%s] [%s:%u %s] %s\r\n",                                         \
		(color) ? LEVEL_COLORED_NAMES[s32((msg).level)] : LEVEL_NAMES[s32((msg).level)], Base_FormatDateTime((msg).time).Data(), \
		(msg).file, (msg).location, (msg).function, (msg).message
