#include "framework/Log.h"

#include "spdlog/sinks/ansicolor_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/msvc_sink.h"

FRAMEWORK_API void SetupLogging()
{
	std::vector<spdlog::sink_ptr>& sinks = spdlog::default_logger()->sinks();
#ifdef KR_PLATFORM_WINDOWS
	sinks.push_back(std::make_shared<spdlog::sinks::msvc_sink_mt>(false));
#endif
	sinks.push_back(std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>());
}

FRAMEWORK_API void AddFile(const std::string& path, size_t maxSize, size_t maxFiles, bool rotateOnOpen)
{
	std::vector<spdlog::sink_ptr>& sinks = spdlog::default_logger()->sinks();
	sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(path, maxSize, maxFiles, rotateOnOpen));
}
