#include "server.h"

#include "utils/logger.h"

namespace fileserver::core {

Server::Server(ServerConfig &config)
    : acceptor_(io_context_, tcp::endpoint(tcp::v4(), config.port)),
      num_threads_{std::max(1UZ, config.num_threads)},
      thread_pool_{num_threads_},
      router_{kMaxPayloadSize},
      file_system_{config.root_dir},
      session_manager_(io_context_.get_executor()) {
  SERVER_LOG(Info) << "Server started.";
  SERVER_LOG(Info) << "Server host: " << config.host;
  SERVER_LOG(Info) << "Server port: " << config.port;
  SERVER_LOG(Info) << "Server root directory: " << config.root_dir;
  SERVER_LOG(Info) << "Server thread count: " << config.num_threads;
}

void Server::Run() {
  asio::co_spawn(io_context_, ListenForConnections(), asio::detached);

  auto guard = asio::make_work_guard(io_context_);
  for (size_t i{}; i < num_threads_; ++i) {
    asio::post(thread_pool_, [this]() {
      io_context_.run();
    });
  }

  thread_pool_.join();
}

asio::awaitable<void> Server::ListenForConnections() {
  for (;;) {
    auto session_strand = asio::make_strand(co_await asio::this_coro::executor);
    auto [err, socket] = co_await acceptor_.async_accept(
        session_strand, asio::as_tuple(asio::use_awaitable));

    if (err) [[unlikely]] {
      SERVER_LOG(Error) << "Acceptor network fault encountered: "
                        << err.message();

      if (err == std::errc::too_many_files_open) {
        constexpr auto timeout = std::chrono::milliseconds(100);
        asio::steady_timer timer(co_await asio::this_coro::executor, timeout);
        co_await timer.async_wait(asio::use_awaitable);
        continue;
      }

      co_return;
    }

    auto new_session = std::make_shared<connection::Session>(
        std::move(socket), &router_, &session_manager_);

    asio::co_spawn(
        session_strand,
        [this, new_session]() -> asio::awaitable<void> {
          session_manager_.Register(new_session);
          new_session->Start();
          co_return;
        },
        asio::detached);
  }
}

}  // namespace fileserver::core
