// HttpServer.h
#pragma once

#include <asio.hpp>

#include "connection/session_manager.h"
#include "router.h"

namespace fileserver::core {

class Server {
  using tcp = asio::ip::tcp;

 public:
  Server(std::uint16_t port);

  Server(const Server&) = delete;
  Server& operator=(const Server&) = delete;

  void Run();

 private:
  asio::awaitable<void> ListenForConnections();

 private:
  asio::io_context io_context_;
  tcp::acceptor acceptor_;

  connection::SessionManager session_manager_;

  Router router_;

  asio::thread_pool thread_pool_;
};

}  // namespace fileserver::core
