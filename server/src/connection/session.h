#pragma once

#include <memory>

#include <asio.hpp>

#include "http1/request_builder.h"
#include "http1/parser.h"
#include "http1/router.h"

namespace fileserver::connection {

class SessionManager;

class Session : public std::enable_shared_from_this<Session> {
  using tcp = asio::ip::tcp;

  static constexpr std::size_t kMaxLength = 4096;
  using Buffer = std::array<char, kMaxLength>;

 public:
  explicit Session(tcp::socket socket, http1::Router *router,
                   SessionManager *manager);
  Session(const Session &other) = delete;
  Session(Session &&other) = delete;
  Session &operator=(const Session &other) = delete;
  Session &operator=(Session &&other) = delete;
  ~Session();

 public:
  uint64_t GetId() const {
    return id_;
  }

  void Start();
  void Shutdown() noexcept;

 private:
  asio::awaitable<void> Read();
  asio::awaitable<void> Write(std::size_t bytes_to_write);
  asio::awaitable<void> WriteChunked(std::size_t bytes_to_write);

  std::size_t ProcessIncomingData(std::size_t bytes_amount);

 private:
  tcp::socket socket_;
  asio::strand<asio::any_io_executor> strand_;

  std::string endpoint_address;

  Buffer read_buffer_{};
  Buffer write_buffer_{};

  http1::RequestBuilder req_builder_;
  http1::RequestParser req_parser_;
  http1::Router *router_;
  SessionManager *manager_;

  uint64_t id_;
  inline static std::atomic_ullong next_id_{0};
};

}  // namespace fileserver::connection
