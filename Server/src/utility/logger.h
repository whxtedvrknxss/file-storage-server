#pragma once

#include <spdlog/spdlog.h>

namespace fileserver::logger {

void Init();

template <typename... Args>
void Info(fmt::format_string<Args...> format, Args &&...args) {
  spdlog::info(format, std::forward<Args>(args)...);
}

template <typename... Args>
void Warn(fmt::format_string<Args...> format, Args &&...args) {
  spdlog::warn(format, std::forward<Args>(args)...);
}

template <typename... Args>
void Error(fmt::format_string<Args...> format, Args &&...args) {
  spdlog::error(format, std::forward<Args>(args)...);
}

template <typename... Args>
void Debug(fmt::format_string<Args...> format, Args &&...args) {
  spdlog::debug(format, std::forward<Args>(args)...);
}

#define FSS_LOG_INFO(...) ::fileserver::logger::Info(__VA_ARGS__)
#define FSS_LOG_ERROR(...) ::fileserver::logger::Error(__VA_ARGS__)

#ifdef _DEBUG
#define FSS_LOG_DEBUG(...) ::fileserver::logger::Debug(__VA_ARGS__)
#else
#define FSS_LOG_DEBUG(...) ((void)0)
#endif  // _DEBUG

}  // namespace fileserver::logger
