#include "request_builder.h"

#include <charconv>
#include <system_error>

namespace fileserver::http1 {

void RequestBuilder::OnMethod(Method method) {
  request_.method_ = method;
}

void RequestBuilder::OnURI(std::string_view uri) {
  request_.uri_.append(uri);
}

void RequestBuilder::OnVersion(uint8_t major, uint8_t minor) {
  using enum Version;

  if (major == 1 && (minor == 1 || minor == 0)) {
    request_.version_ = (minor == 0) ? Http1_0 : Http1_1;
  } else {
    request_.version_ = Unsupported;
  }
}

// NOLINTNEXTLINE(performance-unnecessary-value-param)
void RequestBuilder::OnHeader(Header header) {
  auto &[name, value] = header;
  auto &headers = request_.headers_;

  if (name == "transfer-encoding") {
    headers.transfer_encoding = OnTransferEncoding(value);
  }

  if (name == "connection") {
    headers.connection = OnConnectionType(value);
  }

  if (name == "content-length") {
    auto result = ParseStoull(value);
    if (result) {
      headers.content_length = result.value();
    }
  }

  if (name == "x-upload-offset") {
    auto result = ParseStoull(value);
    if (result) {
      headers.x_upload_offset = result.value();
    }
  }
}

void RequestBuilder::OnBody(std::span<const char> chunk) {
  // request_.body.append_range(chunk);
}

void RequestBuilder::OnComplete() {
}

void RequestBuilder::Reset() {
  request_ = {};
}

TransferEncoding RequestBuilder::OnTransferEncoding(std::string_view value) {
  using enum TransferEncoding;

  if (value == "chunked") {
    return Chunked;
  }

  if (value == "identity") {
    return Identity;
  }

  return Unsupported;
}

ConnectionType RequestBuilder::OnConnectionType(std::string_view value) {
  using enum ConnectionType;

  if (value == "keep-alive") {
    return KeepAlive;
  }

  if (value == "close") {
    return Close;
  }

  return Unsupported;
}

utils::ErrcOr<std::uint64_t> RequestBuilder::ParseStoull(std::string_view str) {
  std::uint64_t out;
  const auto *first = str.data();
  const auto *last = first + str.size();
  auto result = std::from_chars(first, last, out);
  if (result.ec != std::errc() || result.ptr != last) {
    return std::unexpected(std::make_error_code(result.ec));
  }
  return out;
}

}  // namespace fileserver::http1
