#pragma once

#include <asio.hpp>
#include <memory>

#include "core/router.h"
#include "http/request/builder.h"
#include "http/request/parser.h"
#include "http/response_builder.h"
#include "utility/logger.h"

namespace fileserver::core {
class SessionManager;
}

namespace fileserver::connection {

class Session : public std::enable_shared_from_this<Session> {
  using tcp = asio::ip::tcp;

  using CloseHandler = std::function<void(std::shared_ptr<Session>)>;

  static constexpr std::size_t kMaxLength = 4096;
  using Buffer = std::array<char, kMaxLength>;

 public:
  explicit Session(tcp::socket socket, core::Router &router,
                   CloseHandler on_close);
  // : socket_{std::move(socket)},
  //   request_parser_{request_builder_},
  //   router_{&router} {
  //   std::error_code error_code;
  //   auto endpoint = socket_.remote_endpoint(error_code);
  //   if (!error_code) {
  //     endpoint_address = endpoint.address().to_string();
  //   }
  //   SERVER_LOG(Info) << "New connection created: "
  //                    << socket.remote_endpoint().address().to_string();
  // }

  ~Session();

  void Start();
  void Shutdown() noexcept;

 private:
  asio::awaitable<void> Read();
  asio::awaitable<void> Write(std::size_t bytes_amount);

  void OnRead(std::size_t bytes_amount);

 private:
  tcp::socket socket_;
  std::string endpoint_address;
  Buffer read_buffer_;
  Buffer write_buffer_;

  http::RequestBuilder request_builder_;
  http::RequestParser request_parser_;

  http::ResponseBuilder response_builder_;
  core::Router *router_;

  CloseHandler on_close_;
};

}  // namespace fileserver::connection
