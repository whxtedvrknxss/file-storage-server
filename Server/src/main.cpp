#include <print>

#include "core/server.h"
#include "utility/logger.h"

int main(int argc, char *argv[]) {
  fileserver::logging::Logger::Initialize();

  fileserver::core::ServerConfig config{.root_dir = argv[0]};
  for (int i = 1; i < argc; i++) {
    std::string_view arg{argv[i]};

    if ((arg == "-p") || (arg == "--port") && i + 1 < argc) {
      std::string_view val{argv[++i]};
      std::from_chars(val.data(), val.data() + val.size(), config.port);

    } else if ((arg == "-r") || (arg == "--root") && i + 1 < argc) {
      config.root_dir = argv[++i];

    } else if ((arg == "-t") || (arg == "--threads") && i + 1 < argc) {
      std::string_view val{argv[++i]};
      std::from_chars(val.data(), val.data() + val.size(), config.num_threads);

    } else if ((arg == "-a") || (arg == "--host") && i + 1 < argc) {
      config.host = argv[++i];

    } else {
      std::println(stderr, "Unknown or malformed argument: {}", arg);
      std::println(stderr,
                   "Usage: {} [-p port] [-r root_dir] [-t threads] [-a host]",
                   argv[0]);
      return 1;
    }
  }

  if (config.root_dir == argv[0]) {
    std::size_t pos = config.root_dir.find_last_of('\\');
    if (pos != std::string::npos) {
      config.root_dir = config.root_dir.substr(0, pos);
    }
  }

  fileserver::core::Server server(std::move(config));
  server.Run();

  return EXIT_SUCCESS;
}
