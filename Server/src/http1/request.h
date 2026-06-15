#pragma once

#include <string>
#include <expected>

#include "common.h"

namespace fileserver::http1 {

enum class Method : std::uint8_t {
  Get,
  Head,
  Post,
  Put,
  Delete,
  Patch,
  Invalid,
};

//     Supported headers:
// Connection
// Host
// Range
// Content-Length
// Transfer-Encoding
//

class Request {
 public:
  using Headers = KnownHeaders;

  Request() = default;
  Request(Method method, Version version, Headers headers, std::string uri)
      : method_{method},
        version_{version},
        uri_{std::move(uri)},
        headers_{headers},
        is_chunked_{false},
        keep_connection_{false} {}

 public:
  void SetChunked(bool chunked) noexcept {
    is_chunked_ = chunked;
  }

  [[nodiscard]] bool IsChunked() const {
    return is_chunked_;
  }

  [[nodiscard]] bool KeepConnection() const {
    return keep_connection_;
  }

 public:
  [[nodiscard]] Method GetMethod() const {
    return method_;
  }

  [[nodiscard]] Version GetVersion() const {
    return version_;
  }

  [[nodiscard]] const std::string& GetUri() const {
    return uri_;
  }
  [[nodiscard]] const Headers& GetHeaders() const {
    return headers_;
  }

  friend class RequestBuilder;

 private:
  Method method_;
  Version version_;
  std::string uri_;
  Headers headers_;
  bool is_chunked_;
  bool keep_connection_;
};

}  // namespace fileserver::http1
