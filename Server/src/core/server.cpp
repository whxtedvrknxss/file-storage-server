#include "server.h"

#include "utility/logger.h"

namespace fileserver::core {

Server::Server(std::uint16_t port)
    : acceptor_(io_context_, tcp::endpoint(tcp::v4(), port)),
      thread_pool_{std::thread::hardware_concurrency()},
      // TODO: change it for a constant
      router_{kMaxPayloadSize} {
  SERVER_LOG(Info) << "Server started running on port: " << port;
}

void Server::Run() {
  asio::co_spawn(io_context_, ListenForConnections(),
                 [](const std::exception_ptr& eptr) {
                   if (eptr) {
                     std::rethrow_exception(eptr);
                   }
                 });

  auto thread_amount{std::max(1U, std::thread::hardware_concurrency())};
  for (size_t i{}; i < thread_amount; ++i) {
    asio::post(thread_pool_, [this]() {
      io_context_.run();
    });
  }

  thread_pool_.join();
}

asio::awaitable<void> Server::ListenForConnections() {
  while (true) {
    auto session_strand = asio::make_strand(co_await asio::this_coro::executor);
    auto [err, socket] =
        co_await acceptor_.async_accept(asio::as_tuple(asio::use_awaitable));

    if (err) [[unlikely]] {
      SERVER_LOG(Error) << "Acceptor network fault encountered: "
                        << err.message();

      if (err == std::errc::too_many_files_open) {
        asio::steady_timer timer(co_await asio::this_coro::executor,
                                 std::chrono::milliseconds(100));
        co_await timer.async_wait(asio::use_awaitable);
        continue;
      }

      co_return;
    }

    SERVER_LOG(Info) << "Handover to session strand completed smoothly.";

    auto new_session = std::make_shared<connection::Session>(
        std::move(socket), std::move(session_strand), &router_,
        &session_manager_);

    session_manager_.Start(new_session);
  }
}

}  // namespace fileserver::core
