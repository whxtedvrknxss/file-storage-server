#pragma once

#include <asio.hpp>
#include <vector>

#include "http/common.h"

// class FileHandler {
//  private:
//   asio::stream_file file_;
// };

namespace fileserver::storage {

constexpr static std::size_t kKilobyte = 1024;
constexpr static std::size_t kBytesAmount = 64 * kKilobyte;

template <http::AsyncReadStream Source, http::AsyncWriteStream Destination>
asio::awaitable<std::size_t> AsyncPipeStream(Source &source,
                                             Destination &destination) {
  std::vector<char> buffer(kBytesAmount);
  std::size_t total_bytes{};

  for (;;) {
    std::size_t bytes_read = co_await source.async_read_some(
        asio::buffer(buffer), asio::use_awaitable);

    if (bytes_read == 0) [[unlikely]] {
      break;
    }

    co_await asio::async_write(destination, asio::buffer(buffer, bytes_read),
                               asio::use_awaitable);

    total_bytes += bytes_read;
  }

  co_return total_bytes;
}

}  // namespace fileserver::storage
