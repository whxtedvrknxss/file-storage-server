#include "logger.h"

#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace fileserver::logger {

void Init() {
  auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  auto file_sink =
      std::make_shared<spdlog::sinks::basic_file_sink_mt>("Logs.txt", true);

  std::vector<spdlog::sink_ptr> sinks{console_sink, file_sink};

  auto logger = std::make_shared<spdlog::async_logger>(
      "MultiSink", sinks.begin(), sinks.end(), spdlog::thread_pool(),
      spdlog::async_overflow_policy::block);

  spdlog::set_default_logger(logger);
  spdlog::set_level(spdlog::level::info);
}

}  // namespace fileserver::logger
