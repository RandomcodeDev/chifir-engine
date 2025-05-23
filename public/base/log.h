/// \file Logging system
/// \copyright 2025 Randomcode Developers

#pragma once

#include "base/base.h"
#include "base/compiler.h"
#include "base/string.h"
#include "base/types.h"

/// Log message priority level
enum class LogLevel : s32
{
	Trace = 0,
	Debug,
	Info,
	Warning,
	Error,
	FatalError
};

/// Represents a log message
struct LogMessage
{
	LogLevel level;
	uptr location;
	bool isAddress; /// Whether location should be displayed in hex
	cstr file;
	cstr function;
	cstr threadName;
	u64 threadId;
	DateTime time;
	cstr message;
	cstr formatted;
	ssize formattedLength;
	cstr formattedColor;
	ssize formattedColorLength;
};

/// Log writer interface
class BASEAPI ILogWriter
{
  public:
	virtual ~ILogWriter() = default;

	/// Write a message
	virtual void Write(const LogMessage& message) = 0;

	/// Set the level that messages have to be at or above to be logged
	LogLevel SetLevel(LogLevel level)
	{
		LogLevel old = m_minLevel;
		m_minLevel = level;
		return old;
	}

  protected:
	friend void BASEAPI Log_Write(const LogMessage& message);
	friend void BASEAPI Log_Write(
		LogLevel level, uptr location, bool isAddress, cstr file, cstr function, cstr threadName, u64 threadId, cstr message,
		...);

	LogLevel m_minLevel =
#ifdef CH_DEBUG
		LogLevel::Trace;
#else
		LogLevel::Info;
#endif

	/// Indexable with any LogLevel_t value
	static constexpr cstr LEVEL_NAMES[] = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};
	static constexpr cstr LEVEL_COLORED_NAMES[] = {
		"\x1b[38;5;126mT\x1b[38;5;161mR\x1b[38;5;231mA\x1b[38;5;208mC\x1b[38;5;196mE\x1b[0m", // lesbian flag
		"\x1b[38;5;123mD\x1b[38;5;213mE\x1b[38;5;231mB\x1b[38;5;213mU\x1b[38;5;123mG\x1b[0m", // trans flag
		"\x1b[38;5;118mINFO\x1b[0m",                                                          // light lime green
		"\x1b[38;5;214mWARN\x1b[0m",                                                          // ochre?
		"\x1b[38;5;196mERROR\x1b[0m",                                                         // bright red
		"\x1b[30m\x1b[48;5;124mFATAL\x1b[0m"};                                                // bright red background
};

class IWritableFilesystem;

/// Filesystem log writer
class BASEAPI CFileLogWriter: public ILogWriter
{
  public:
	CFileLogWriter(IWritableFilesystem* filesystem, cstr logName, bool addDate = true);
	~CFileLogWriter();

	void Write(const LogMessage& message);

  protected:
	IWritableFilesystem* m_filesystem;
	dstr m_filename;
};

#ifdef CH_WIN32
/// DbgPrint log writer
class BASEAPI CDbgPrintLogWriter: public ILogWriter
{
  public:
	void Write(const LogMessage& message);
};
#endif

class BASEAPI CConsoleLogWriter: public ILogWriter
{
  public:
	void Write(const LogMessage& message);
};

/// Add a log writer
extern BASEAPI void Log_AddWriter(ILogWriter* writer);

/// Write a log message
extern BASEAPI void Log_Write(const LogMessage& message);

/// Write a log message, and format it
extern BASEAPI void Log_Write(
	LogLevel level, uptr location, bool isAddress, cstr file, cstr function, cstr threadName, u64 threadId, cstr message, ...);

#ifdef CH_RETAIL
#define Log_Message(level, ...)                                                                                                  \
	Log_Write(level, __LINE__, false, __FILE__, FUNCTION_NAME, nullptr, UINT64_MAX, __VA_ARGS__)
#else
#define Log_Message(level, ...)                                                                                                  \
	Log_Write(level, __LINE__, false, __FILE__, FUNCTION_SIGNATURE, nullptr, UINT64_MAX, __VA_ARGS__)
#endif
#define Log_Trace(...)      Log_Message(LogLevel::Trace, __VA_ARGS__)
#define Log_Debug(...)      Log_Message(LogLevel::Debug, __VA_ARGS__)
#define Log_Info(...)       Log_Message(LogLevel::Info, __VA_ARGS__)
#define Log_Warning(...)    Log_Message(LogLevel::Warning, __VA_ARGS__)
#define Log_Error(...)      Log_Message(LogLevel::Error, __VA_ARGS__)
#define Log_FatalError(...) Log_Message(LogLevel::FatalError, __VA_ARGS__)
