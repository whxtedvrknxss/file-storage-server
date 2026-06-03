#pragma once

#include <span>
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

struct Request {
  Version version;
  std::vector<Header> headers;
  // std::vector<char> body;

  Method method;
  std::string uri;
};

class RequestBuilder {
 public:
  RequestBuilder() = default;

  void OnMethod(std::string_view method);
  void OnURI(std::string_view uri);
  void OnHeader(Header &&header);
  void OnVersion(uint8_t major, uint8_t minor);
  void OnBody(std::span<const char> chunk);

  void OnComplete();

  [[nodiscard]] Request Release() { return request_; }

  void Reset();

 private:
  Request request_;
  std::string method_buffer_;
};

}  // namespace fileserver::http
