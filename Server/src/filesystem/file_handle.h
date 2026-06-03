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

asio::awaitable<void> tcp_echo_server(asio::ip::tcp::socket socket) {
  constexpr static std::size_t kSize = 1024;
  std::array<char, kSize> data;
  while (true) {
    std::size_t length = co_await socket.async_read_some(asio::buffer(data));
    co_await asio::async_write(socket, asio::buffer(data, length));
  }
}

// template <typename DataProcessor>
// asio::awaitable<void> StreamFileChunks(asio::stream_file &file,
//                                        DataProcessor &process_chunk) {
//   std::vector<char> file_buffer(kBytesAmount);

//   for (;;) {
//     std::size_t bytes_read = co_await AsyncPipeStream(file, file_buffer);

//     if (bytes_read == 0) {
//       break;
//     }

//     co_await ProcessChunk(asio::buffer(file_buffer, bytes_read));
//   }
// }

}  // namespace fileserver::storage
