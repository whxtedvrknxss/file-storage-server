#include "session.h"

#include <asio.hpp>
#include <print>

#include "session_manager.h"

namespace fileserver::connection {

Session::Session(tcp::socket socket, core::Router &router,
                 CloseHandler on_close)
    : socket_{std::move(socket)},
      request_parser_{request_builder_},
      router_{&router},
      on_close_{std::move(on_close)} {
  std::error_code error_code;
  auto endpoint = socket_.remote_endpoint(error_code);
  if (!error_code) {
    endpoint_address = endpoint.address().to_string();
  }
  SERVER_LOG(Info) << "New connection created: " << endpoint_address;
}

Session::~Session() {
  SERVER_LOG(Info) << "Successfully closed the connection " << endpoint_address;
}

void Session::Start() {
  asio::co_spawn(socket_.get_executor(), Read(), asio::detached);
}

void Session::Shutdown() {
  std::error_code err_code;
  err_code = socket_.shutdown(tcp::socket::shutdown_both, err_code);
  err_code = socket_.close(err_code);

  if (on_close_) {
    on_close_(shared_from_this());
  }
}

asio::awaitable<void> Session::Read() {
  auto self = shared_from_this();

  for (;;) {
    std::error_code err;
    std::size_t length = co_await socket_.async_read_some(
        asio::buffer(read_buffer_),
        asio::redirect_error(asio::use_awaitable, err));

    if (err) {
      if (err == asio::error::eof || err == asio::error::connection_reset) {
        SERVER_LOG(Info) << "Client disconnected gracefully: "
                         << self->endpoint_address;
      } else {
        SERVER_LOG(Error) << "Socket error: " << err.message();
      }
      co_return;
    }

    self->OnRead(length);
  }
}

asio::awaitable<void> Session::Write(std::size_t bytes_amount) {
  auto self = shared_from_this();

  std::error_code err;
  co_await asio::async_write(self->socket_,
                             asio::buffer(self->write_buffer_, bytes_amount),
                             asio::redirect_error(asio::use_awaitable, err));
  if (err) {
    SERVER_LOG(Error) << "Write failed: " << err.message();
    co_return;
  }
}

void Session::OnRead(std::size_t bytes_amount) {
  auto result = request_parser_.Parse({read_buffer_.data(), bytes_amount});
}

}  // namespace fileserver::connection
