#include "request_validator.h"

namespace fileserver::http1 {

RequestValidationStatus RequestValidator::Validate(
    const Request& request) const noexcept {
  using enum RequestValidationStatus;

  const auto method = request.GetMethod();

  if (method != Method::Post && method != Method::Put) {
    return Ok;
  }

  const auto content_length_opt = request.GetHeaders().content_length;

  if (content_length_opt) {
    if (content_length_opt > max_payload_) {
      return PayloadTooLarge;
    }

    if (request.IsChunked()) {
      return BothTransferEncodingAndContentLengthProvided;
    }

    return Ok;
  }

  if (!request.IsChunked()) {
    return ContentLengthRequired;
  }

  return Ok;
}

}  // namespace fileserver::http1