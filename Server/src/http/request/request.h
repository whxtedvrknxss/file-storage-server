#pragma once

#include <string>
#include <vector>

#include "http/common.h"

namespace fileserver::http {

enum class Method : std::uint8_t {
  Get,
  Head,
  Post,
  Put,
  Delete,
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
  using Headers = std::vector<Header>;

  Request() = default;
  Request(Method method, Version version, std::vector<Header> headers,
          std::string uri)
      : method_{method},
        version_{version},
        headers_{std::move(headers)},
        uri_{std::move(uri)} {}

  bool IsChunked() const { return bool{}; }

 public:
  // NOLINTNEXTLINE(modernize-use-nodiscard)
  Method GetMethod() const { return method_; }

  // NOLINTNEXTLINE(modernize-use-nodiscard)
  Version GetVersion() const { return version_; }

  [[nodiscard]] const Header* GetHeader(std::string_view) const;

  [[nodiscard]] const std::string& GetUri() const { return uri_; }
  [[nodiscard]] const Headers& GetHeaders() const { return headers_; }

 public:
  // void SetMethod(Method method) { method_ = method; }
  // void SetVersion(Version version) { version_ = version; }
  // void AddHeader(Header header) { headers_.push_back(std::move(header)); }
  // void SetUri(std::string uri) { uri_ = std::move(uri); }

  friend class RequestBuilder;

 private:
  Method method_;
  Version version_;
  std::string uri_;
  Headers headers_;
};

}  // namespace fileserver::http
