#include "logger.h"

#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace fileserver::utils {

void Logger::Initialize() {
  constexpr std::size_t kQueueSize = 8192;
  spdlog::init_thread_pool(kQueueSize, 1);

  std::vector<spdlog::sink_ptr> sinks;
  sinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
  sinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("Logs.txt", true));
  sinks[0]->set_pattern("[%H:%M:%S] [thread %t] [%^%l%$] %v");

  auto logger = std::make_shared<spdlog::async_logger>(
      "Server", sinks.begin(), sinks.end(), spdlog::thread_pool(),
      spdlog::async_overflow_policy::block
  );

  spdlog::set_default_logger(logger);
}

void Logger::Destroy() {
  spdlog::shutdown();
}

#ifdef _DEBUG
LogMessage::LogMessage(Severity severity, const char* file, int line) : severity_{severity} {
  std::string_view file_view(file);
  auto last_slash = file_view.find_last_of("\\/");
  if (last_slash != std::string_view::npos) {
    file_view.remove_prefix(last_slash + 1);
  }

  fmt::format_to(std::back_inserter(buffer_), "[{}:{}] ", file_view, line);
}
#endif

LogMessage::~LogMessage() noexcept {
  auto level = static_cast<spdlog::level::level_enum>(severity_);
  spdlog::log(level, fmt::to_string(buffer_));

  if (severity_ == Severity::Fatal) {
    spdlog::shutdown();
    std::abort();
  }
}

}  // namespace fileserver::utils
