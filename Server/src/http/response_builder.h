#pragma once

#include <vector>

#include "common.h"

namespace fileserver::http {

enum class StatusCode : uint16_t {
  Ok = 200,
  Created = 201,
  BadRequest = 400,
  NotFound = 404,
  ContentTooLarge = 413,
  URITooLong = 414,
  IAmATeapot = 418,
  InternalServerError = 500,
  NotImplemented = 501,
  HTTPVersionNotSupported = 505,
};

struct Response {
  Version version;
  std::vector<Header> headers;
  std::vector<char> body;

  StatusCode status;
};

class ResponseBuilder {
 public:
  ResponseBuilder() = default;

  void Build(StatusCode status);
  [[nodiscard]] std::vector<char> GetResponseBytes() {
    return std::move(response_bytes_);
  }

  void Reset();

 private:
  ResponseBuilder &AddStatusCode(StatusCode status);
  ResponseBuilder &AddVersion(Version version);
  ResponseBuilder &AddHeader(Header Header);

 private:
  static std::string StatusToString(StatusCode status);
  static std::string VersionToString(Version version);

 private:
  Response response_;
  std::vector<char> response_bytes_;
};

}  // namespace fileserver::http
