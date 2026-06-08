#include "session.h"

#include <print>

#include <asio.hpp>

#include "session_manager.h"
#include "utility/logger.h"

namespace fileserver::connection {

Session::Session(tcp::socket socket, asio::strand<asio::any_io_executor> strand,
                 http::Router *router, SessionManager *manager)
    : socket_{std::move(socket)},
      strand_{std::move(strand)},
      router_{router},
      manager_{manager},
      request_parser_{request_builder_} {
  std::error_code error_code;
  auto endpoint = socket_.remote_endpoint(error_code);
  if (!error_code) {
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
  std::error_code err_code;
  err_code = socket_.shutdown(tcp::socket::shutdown_both, err_code);
  err_code = socket_.close(err_code);

  if (manager_ != nullptr) {
    manager_->Stop(shared_from_this());
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
  auto result = request_parser_.Parse({read_buffer_.data(), bytes_amount});
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
