#include "request_builder.h"

#include <charconv>

namespace fileserver::http1 {

void RequestBuilder::OnMethod(Method method) {
  request_.method_ = method;
}

void RequestBuilder::OnURI(std::string_view uri) {
  request_.uri_.append(uri);
}

void RequestBuilder::OnVersion(uint8_t major, uint8_t minor) {
  if (major == 1 && (minor == 1 || minor == 0)) {
    request_.version_ = (minor == 0) ? Version::Http1_0 : Version::Http1_1;
  } else {
    request_.version_ = Version::Unsupported;
  }
}

// NOLINTNEXTLINE(performance-unnecessary-value-param)
void RequestBuilder::OnHeader(Header header) {
  auto& [name, value] = header;
  auto& headers = request_.headers_;
  if (name == "transfer-encoding") {
    if (value == "chunked") {
      headers.transfer_encoding = http1::TransferEncoding::Chunked;
      request_.SetChunked(true);
    } else if (value == "identity") {
      headers.transfer_encoding = http1::TransferEncoding::Identity;
    } else {
      headers.transfer_encoding = http1::TransferEncoding::Unsupported;
    }
  }

  if (name == "connection") {
    if (value == "keep-alive") {
      headers.connection = http1::ConnectionType::KeepAlive;
    } else if (value == "close") {
      headers.connection = http1::ConnectionType::Close;
    }
  }

  if (name == "content-length") {
    std::uint64_t content_length;
    if (TryParseStoull(value, content_length)) {
      headers.content_length = content_length;
    }
  }

  if (name == "x-upload-offset") {
    headers.x_upload_offset = std::stoull(value);
  }
}

void RequestBuilder::OnBody(std::span<const char> chunk) {
  // request_.body.append_range(chunk);
}

void RequestBuilder::OnComplete() {}

void RequestBuilder::Reset() {}

bool RequestBuilder::TryParseStoull(const std::string& s,
                                    unsigned long long& out) {
  const auto* first = s.data();
  const auto* last = s.data() + s.size();
  auto result = std::from_chars(first, last, out);
  return result.ec == std::errc() && result.ptr == last;
}

}  // namespace fileserver::http1
