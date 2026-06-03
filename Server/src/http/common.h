#pragma once

#include <asio.hpp>
#include <string>

namespace fileserver::http {

enum class Version : std::uint8_t { Http1_0, Http1_1, Unsupported };

struct Header {
  std::string name;
  std::string value;
};

template <typename T>
concept AsyncReadStream = requires(T stream, asio::mutable_buffer buffer) {
  {
    stream.async_read_some(buffer, asio::use_awaitable)
  } -> std::same_as<asio::awaitable<std::size_t>>;
};

template <typename T>
concept AsyncWriteStream = requires(T stream, asio::const_buffer buffer) {
  {
    stream.async_write_some(buffer, asio::use_awaitable)
  } -> std::same_as<asio::awaitable<std::size_t>>;
};

}  // namespace fileserver::http
