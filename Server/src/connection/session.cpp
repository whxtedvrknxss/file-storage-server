#include "session.h"

#include <asio.hpp>

#include "session_manager.h"
#include "utils/logger.h"
#include "http/parser.h"
#include "http/builder.h"

namespace fileserver::connection {

Session::Session(tcp::socket socket, http::Router *router,
                 SessionManager *manager)
    : socket_{std::move(socket)},
      strand_{socket.get_executor()},
      router_{router},
      manager_{manager},
      id_{next_id_++} {
  std::error_code err;
  auto endpoint = socket_.remote_endpoint(err);
  if (!err) {
    endpoint_address = endpoint.address().to_string();
  }
  SERVER_LOG(Info) << "New connection created: " << endpoint_address;
}

Session::~Session() {
  SERVER_LOG(Info) << "Client disconnected gracefully: "
                   << this->endpoint_address;
}

void Session::Start() {
  asio::co_spawn(strand_, Read(), asio::detached);
}

void Session::Shutdown() noexcept {
  auto self = shared_from_this();
  std::error_code err;

  // NOLINTNEXTLINE(bugprone-unused-return-value)
  socket_.shutdown(tcp::socket::shutdown_both, err);
  if (err) [[unlikely]] {
    SERVER_LOG(Warn) << "Error shutting down socket: " << err.message();
  }

  // NOLINTNEXTLINE(bugprone-unused-return-value)
  socket_.close(err);
  if (err) [[unlikely]] {
    SERVER_LOG(Error) << "Close error: " << err.message();
  }

  if (manager_ != nullptr) {
    manager_->Unregister(self);
  }
}

asio::awaitable<void> Session::Read() {
  auto self = shared_from_this();

  for (;;) {
    auto [err, length] = co_await socket_.async_read_some(
        asio::buffer(read_buffer_), asio::as_tuple(asio::use_awaitable));

    if (err) {
      if (err == asio::error::eof || err == asio::error::connection_reset) {
        Shutdown();
      } else {
        SERVER_LOG(Error) << "Socket error: " << err.message();
      }
      co_return;
    }

    std::size_t response_bytes = self->ProcessIncomingData(length);
    if (response_bytes > 0) {
      co_await Write(response_bytes);
    }
  }
}

asio::awaitable<void> Session::Write(std::size_t bytes_to_write) {
  auto self = shared_from_this();

  // TODO: allow chunked writing for big messages(finish the method)
  auto [err, bytes_written] = co_await socket_.async_write_some(
      asio::buffer(self->write_buffer_, bytes_to_write),
      asio::as_tuple(asio::use_awaitable));

  if (err) {
    SERVER_LOG(Error) << "Write failed: " << err.message();
    co_return;
  }
}

std::size_t Session::ProcessIncomingData(std::size_t bytes_amount) {
  http::RequestBuilder builder;
  http::RequestParser parser{builder};
  auto result = parser.Parse({read_buffer_.data(), bytes_amount});
  if (result.error == http::ParseError::None) {
    std::string response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Length: 14\r\n"
        "\r\n"
        "Hello, world!\n";
    std::ranges::copy(response, write_buffer_.begin());
    return response.size();
  }
  return 0;
}

}  // namespace fileserver::connection
