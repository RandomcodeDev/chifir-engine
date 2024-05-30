#include "framework/Log.h"

#include "spdlog/sinks/ansicolor_sink.h"
#include "spdlog/sinks/msvc_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"

FRAMEWORK_API void SetupLogging()
{
#ifdef KR_PLATFORM_WINDOWS
	spdlog::default_logger()->sinks().push_back(std::make_shared<spdlog::sinks::msvc_sink_mt>(false));
#endif
}

FRAMEWORK_API void AddLogFile(const std::string& path, size_t maxSize, size_t maxFiles, bool rotateOnOpen)
{
	spdlog::default_logger()->sinks().push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(path, maxSize, maxFiles, rotateOnOpen));
}
