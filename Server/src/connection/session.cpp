#include "session.h"

#include <print>

namespace fileserver::connection {

void Session::Start() { Read(); }

void Session::Read() {
  auto self = shared_from_this();
  auto func = [self](std::error_code error_code, size_t length) {
    self->OnRead(length);
  };
  auto bind_executor = asio::bind_executor(Socket.get_executor(), func);
  Socket.async_read_some(asio::buffer(ReadBuffer), bind_executor);
}

void Session::Write(std::size_t bytes_amount) {
  auto self = shared_from_this();
  auto func = [this, self](asio::error_code error_code, size_t) {
    if (!error_code) {
      Read();
    }
  };
  auto bind_executor = asio::bind_executor(Socket.get_executor(), func);
  asio::async_write(Socket, asio::buffer(WriteBuffer, bytes_amount),
                    bind_executor);
}

void Session::OnRead(std::size_t bytes_amount) {
  auto result = RequestParser.Parse({ReadBuffer.data(), bytes_amount});
}

}  // namespace fileserver::connection
