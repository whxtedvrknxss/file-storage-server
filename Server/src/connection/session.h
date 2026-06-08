#pragma once

#include <asio.hpp>
#include <memory>

#include "http/router.h"
#include "http/builder.h"
#include "http/parser.h"
#include "http/response_builder.h"

namespace fileserver::connection {

class SessionManager;

class Session : public std::enable_shared_from_this<Session> {
  using tcp = asio::ip::tcp;

  static constexpr std::size_t kMaxLength = 4096;
  using Buffer = std::array<char, kMaxLength>;

 public:
  explicit Session(tcp::socket socket,
                   asio::strand<asio::any_io_executor> strand,
                   http::Router *router, SessionManager *manager);
  ~Session();

  void Start();
  void Shutdown() noexcept;

 private:
  asio::awaitable<void> Read();
  asio::awaitable<void> Write(std::size_t bytes_to_write);

  std::size_t ProcessIncomingData(std::size_t bytes_amount);

 private:
  tcp::socket socket_;
  asio::strand<asio::any_io_executor> strand_;

  std::string endpoint_address;

  Buffer read_buffer_;
  Buffer write_buffer_;

  http::RequestBuilder request_builder_;
  http::RequestParser request_parser_;
  http::ResponseBuilder response_builder_;

  http::Router *router_;
  SessionManager *manager_;
};

}  // namespace fileserver::connection
