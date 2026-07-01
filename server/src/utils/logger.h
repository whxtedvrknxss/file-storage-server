#pragma once

#include <spdlog/formatter.h>
#include <spdlog/spdlog.h>

namespace fileserver::utils {

enum class Severity : std::uint8_t {
  Info = spdlog::level::info,
  Warn = spdlog::level::warn,
  Error = spdlog::level::err,
  Fatal = spdlog::level::critical
};

class Logger {
 public:
  static void Initialize();
  static void Destroy();
};

class LogMessage {
 public:
#ifdef _DEBUG
  explicit LogMessage(Severity severity, const char* file, int line);
#else
  explicit LogMessage(Severity severity) : severity_{severity} {
  }
#endif
  LogMessage(const LogMessage& other) = delete;
  LogMessage(LogMessage&& other) = delete;
  LogMessage& operator=(const LogMessage& other) = delete;
  LogMessage& operator=(LogMessage&& other) = delete;

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
  std::mutex mutex_;
};

#ifdef _DEBUG

#define SERVER_LOG(severity) \
  fileserver::utils::LogMessage(fileserver::utils::Severity::severity, __FILE__, __LINE__)

#else

#define SERVER_LOG(severity) fileserver::utils::LogMessage(fileserver::utils::Severity::severity)

#endif

}  // namespace fileserver::utils
