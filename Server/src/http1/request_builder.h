#pragma once

#include <span>

#include "common.h"
#include "request.h"

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
  static bool TryParseStoull(const std::string& s, unsigned long long& out);

 private:
  Request request_;
};

}  // namespace fileserver::http1
