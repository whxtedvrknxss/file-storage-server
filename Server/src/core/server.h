// HttpServer.h
#pragma once

#include <asio.hpp>

#include "connection/session_manager.h"
#include "filesystem/filesystem.h"
#include "http/router.h"
#include "filesystem/sizes.h"

namespace fileserver::core {

using namespace storage::units;
constexpr std::size_t kMaxPayloadSize = 10_GiB;

struct ServerConfig {
  std::string host = "0.0.0.0";
  // NOLINTNEXTLINE(readability-magic-numbers)
  std::uint16_t port = 8080;
  std::string root_dir;
  std::size_t num_threads = 4;
};

class Server {
  using tcp = asio::ip::tcp;

 public:
  Server(ServerConfig& config);

  void Run();

 private:
  asio::awaitable<void> ListenForConnections();

 private:
  asio::io_context io_context_;
  tcp::acceptor acceptor_;

  connection::SessionManager session_manager_;

  std::size_t num_threads_;
  asio::thread_pool thread_pool_;

  http::Router router_;
  storage::FileSystem file_system_;
};

}  // namespace fileserver::core
