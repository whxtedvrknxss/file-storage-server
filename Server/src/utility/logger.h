#pragma once

#include <spdlog/formatter.h>
#include <spdlog/spdlog.h>

namespace fileserver::logging {

enum class Severity : std::uint8_t {
  Info = spdlog::level::info,
  Warning = spdlog::level::warn,
  Error = spdlog::level::err,
  Fatal = spdlog::level::critical
};

class Logger {
 public:
  static void Initialize();
};

class LogMessage {
 public:
#ifdef _DEBUG
  explicit LogMessage(Severity severity, const char* file, int line)
      : severity_{severity} {
    std::string_view file_view(file);
    auto last_slash = file_view.find_last_of("\\/");
    if (last_slash != std::string_view::npos) {
      file_view.remove_prefix(last_slash + 1);
    }

    fmt::format_to(std::back_inserter(buffer_), "[{}:{}] ", file_view, line);
  }
#else
  explicit LogMessage(Severity severity) : severity_{severity} {}
#endif
  ~LogMessage() noexcept;

  template <typename T>
  LogMessage& operator<<(const T& value) {
    fmt::format_to(std::back_inserter(buffer_), "{}", value);
    return *this;
  }

  LogMessage& operator<<(std::string_view value) {
    fmt::format_to(std::back_inserter(buffer_), "{}", value);
    return *this;
  }

 private:
  Severity severity_;
  fmt::memory_buffer buffer_;
};

#ifdef _DEBUG
#define SERVER_LOG(severity)                                               \
  fileserver::logging::LogMessage(fileserver::logging::Severity::severity, \
                                  __FILE__, __LINE__)

#else
#define SERVER_LOG(severity) \
  fileserver::logging::LogMessage(fileserver::logging::Severity::severity)
#endif

// #define SERVER_LOG_INFO(...) ::fileserver::logger::(__VA_ARGS__)
// #define SERVER_LOG_ERROR(...) ::fileserver::logger::Error(__VA_ARGS__)

// #ifdef _DEBUG
// #define SERVER_LOG_DEBUG(...) ::fileserver::logger::Debug(__VA_ARGS__)
// #else
// #define FSS_LOG_DEBUG(...) ((void)0)
// #endif  // _DEBUG

}  // namespace fileserver::logging
