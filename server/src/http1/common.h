#pragma once

#include <string>
#include <optional>

#include <asio.hpp>

namespace fileserver::http1 {

enum class Version : std::uint8_t {
  Http1_0,
  Http1_1,
  Unsupported
};

struct Header {
  std::string name;
  std::string value;
};

enum class TransferEncoding : std::uint8_t {
  Chunked,
  Identity,
  Unsupported
};

enum class ConnectionType : std::uint8_t {
  KeepAlive,
  Close,
  Unsupported
};

struct KnownHeaders {
  std::optional<TransferEncoding> transfer_encoding;
  std::optional<ConnectionType> connection;
  std::optional<std::uint64_t> content_length;
  std::optional<std::uint64_t> x_upload_offset;

  bool operator==(const KnownHeaders& other) const = default;
};

}  // namespace fileserver::http1
