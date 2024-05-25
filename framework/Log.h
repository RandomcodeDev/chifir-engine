/// @file Log.h
/// @brief This file declares logging stuff
/// @copyright (c) 2024 Randomcode Developers. See LICENSE.

#pragma once

#include "Framework.h"

#ifdef KR_DEBUG
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#elif defined KR_MINSIZEREL
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_WARN
#else
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_INFO
#endif
#include "spdlog/spdlog-inl.h"

#define KR_LOG_TRACE(...) SPDLOG_TRACE(__VA_ARGS__)
#define KR_LOG_DEBUG(...) SPDLOG_DEBUG(__VA_ARGS__)
#define KR_LOG_INFO(...) SPDLOG_INFO(__VA_ARGS__)
#define KR_LOG_WARN(...) SPDLOG_WARN(__VA_ARGS__)
#define KR_LOG_ERROR(...) SPDLOG_ERROR(__VA_ARGS__)
#define KR_LOG_FATAL(...) SPDLOG_CRITICAL(__VA_ARGS__)

/// @brief Set up logging
extern FRAMEWORK_API void SetupLogging();
