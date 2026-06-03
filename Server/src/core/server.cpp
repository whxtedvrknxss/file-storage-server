#include "server.h"

#include "utility/logger.h"

namespace fileserver::core {

Server::Server(std::uint16_t port)
    : acceptor_(io_context_, tcp::endpoint(tcp::v4(), port)),
      thread_pool_(std::thread::hardware_concurrency()) {
  logger::Init();

  // FSS_LOG_INFO("The server starts working on port {}", port);
}

void Server::Run() {
  for (size_t i{}; i < std::thread::hardware_concurrency(); ++i) {
    asio::post(thread_pool_, [this] { io_context_.run(); });
  }

  thread_pool_.join();
}

asio::awaitable<void> Server::ListenForConnections() {
  auto executor = co_await asio::this_coro::executor;

  while (true) {
    auto [error_code, socket] =
        co_await acceptor_.async_accept(asio::as_tuple(asio::use_awaitable));

    if (error_code) [[unlikely]] {
      // FSS_LOG_ERROR("Acceptor network fault encountered: {}",
      //               error_code.message());

      // if (error_code == std::errc::too_many_files_open) {
      //   asio::steady_timer timer(co_await asio::this_coro::executor,
      //                            std::chrono::milliseconds(100));
      //   co_await timer.async_wait(asio::use_awaitable);
      // } else {
      //   co_return;
      // }
    }

    auto session_strand = asio::make_strand(io_context_);
    auto native_fd = socket.release(error_code);

    tcp::socket bound_socket(session_strand);
    if (std::error_code error_code =
            bound_socket.assign(tcp::v4(), native_fd, error_code);
        error_code) {
      co_return;
    }
  }
}

}  // namespace fileserver::core
