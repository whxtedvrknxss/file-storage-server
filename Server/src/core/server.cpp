#include "server.h"

#include "utility/logger.h"

namespace fileserver::core {

Server::Server(std::uint16_t port)
    : acceptor_(io_context_, tcp::endpoint(tcp::v4(), port)),
      thread_pool_{std::thread::hardware_concurrency()} {
  SERVER_LOG(Info) << "Server started running on port: " << port;
}

void Server::Run() {
  asio::co_spawn(io_context_, ListenForConnections(), [](std::exception_ptr e) {
    if (e) {
      try {
        std::rethrow_exception(e);
      } catch (const std::exception& ex) {
        SERVER_LOG(Fatal) << "Fatal error in accept loop" << ex.what();
        std::terminate();
      }
    }
  });

  auto thread_amount{std::max(1U, std::thread::hardware_concurrency())};
  for (size_t i{}; i < thread_amount; ++i) {
    asio::post(thread_pool_, [this] { io_context_.run(); });
  }

  thread_pool_.join();
}

asio::awaitable<void> Server::ListenForConnections() {
  auto executor = co_await asio::this_coro::executor;

  while (true) {
    auto session_strand = asio::make_strand(io_context_);
    auto [error_code, socket] = co_await acceptor_.async_accept(
        session_strand, asio::as_tuple(asio::use_awaitable));

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
      co_return;
    }

    // auto native_fd = socket.release(error_code);
    // if (error_code) [[unlikely]] {
    //   SERVER_LOG(Error) << "Failed to release socket handle: "
    //                     << error_code.message();
    //   continue;
    // }

    // if (error_code) [[unlikely]] {
    //   SERVER_LOG(Fatal) << "Error assigning socket.";
    //   co_return;
    // }

    SERVER_LOG(Info) << "Handover to session strand completed smoothly.";
    session_manager_.Start(
        std::make_shared<connection::Session>(std::move(socket), router_));
  }
}

}  // namespace fileserver::core
