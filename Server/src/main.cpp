#include "core/server.h"

int main(int argc, char *argv[]) {
  const std::uint16_t port = 8080;
  fileserver::core::Server server(port);
  server.Run();

  return EXIT_SUCCESS;
}
