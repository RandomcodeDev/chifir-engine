/// @file Log.h
/// @brief This file declares logging stuff
/// @copyright (c) 2024 Randomcode Developers. See LICENSE.

#pragma once

#include "Framework.h"

#define KR_LOG_TRACE(...) SPDLOG_LOGGER_CALL(GetLogger(), spdlog::level::trace, __VA_ARGS__)
#define KR_LOG_DEBUG(...) SPDLOG_LOGGER_CALL(GetLogger(), spdlog::level::debug, __VA_ARGS__)
#define KR_LOG_INFO(...)  SPDLOG_LOGGER_CALL(GetLogger(), spdlog::level::info, __VA_ARGS__)
#define KR_LOG_WARN(...)  SPDLOG_LOGGER_CALL(GetLogger(), spdlog::level::warn, __VA_ARGS__)
#define KR_LOG_ERROR(...) SPDLOG_LOGGER_CALL(GetLogger(), spdlog::level::err, __VA_ARGS__)
#define KR_LOG_FATAL(...) SPDLOG_LOGGER_CALL(GetLogger(), spdlog::level::critical, __VA_ARGS__)

/// @brief Set up logging
extern FRAMEWORK_API void SetupLogging();

/// @brief Get the logger
FRAMEWORK_API std::shared_ptr<spdlog::logger> GetLogger();

/// @brief Add a file
extern FRAMEWORK_API void AddLogFile(const std::string& path, size_t maxSize = 1024 * 1024, size_t maxFiles = 5, bool rotateOnOpen = true);
