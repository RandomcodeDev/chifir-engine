// Logging system

#pragma once

#include "base.h"
#include "compiler.h"
#include "string.h"
#include "types.h"

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
	cstr message;
};

// Log writer interface
class ILogWriter
{
  public:
	virtual ~ILogWriter() DEFAULT;

	// Write a message
	virtual void Write(const LogMessage_t& message) = 0;
};

// Initialize logging
extern BASEAPI bool Log_Init();
