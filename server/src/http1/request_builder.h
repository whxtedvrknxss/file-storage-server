#pragma once

#include <span>

#include "common.h"
#include "request.h"
#include "utils/defines.hpp"

namespace fileserver::http1 {

class RequestBuilder {
 public:
  RequestBuilder() = default;

  void OnMethod(Method method);
  void OnURI(std::string_view uri);
  void OnHeader(Header header);
  void OnVersion(uint8_t major, uint8_t minor);
  void OnBody(std::span<const char> chunk);

  void OnComplete();

  [[nodiscard]] Request Release() {
    return request_;
  }

  void Reset();

 private:
  static TransferEncoding OnTransferEncoding(std::string_view value);
  static ConnectionType OnConnectionType(std::string_view value);

  static utils::ErrcOr<std::uint64_t> ParseStoull(std::string_view str);

 private:
  Request request_;
};

}  // namespace fileserver::http1
