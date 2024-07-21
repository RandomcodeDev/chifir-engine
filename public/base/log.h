// Logging system

#pragma once

#include "compiler.h"
#include "string.h"
#include "types.h"
#include "vector.h"

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
	s32 line;
	cstr file;
	cstr function;

};

// Log writer interface
class ILogWriter
{
  public:
	virtual ~ILogWriter() DEFAULT;

	// Check if it's currently safe to use this writer (say, if allocation shouldn't be used)
	virtual bool Safe() = 0;

	// Write a message
	virtual void Write(const LogMessage_t& message) = 0;
};
