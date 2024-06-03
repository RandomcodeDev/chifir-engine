#include "framework/Log.h"

#include "spdlog/sinks/ansicolor_sink.h"
#include "spdlog/sinks/msvc_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"

FRAMEWORK_API void SetupLogging()
{
	GetLogger()->sinks().push_back(std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>(spdlog::color_mode::automatic));
#ifdef KR_PLATFORM_WINDOWS
	GetLogger()->sinks().push_back(std::make_shared<spdlog::sinks::msvc_sink_mt>(false));
#endif

#ifdef KR_DEBUG
	GetLogger()->set_level(spdlog::level::debug);
#else
	GetLogger()->set_level(spdlog::level::info);
#endif
}

FRAMEWORK_API std::shared_ptr<spdlog::logger> GetLogger()
{
	static std::shared_ptr<spdlog::logger> logger = std::make_shared<spdlog::logger>("main");
	return logger;
}

FRAMEWORK_API void AddLogFile(const std::string& path, size_t maxSize, size_t maxFiles, bool rotateOnOpen)
{
	GetLogger()->sinks().push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(path, maxSize, maxFiles, rotateOnOpen));
}
