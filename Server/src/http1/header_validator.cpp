#include "header_validator.h"

namespace fileserver::http1 {

HeaderValidationStatus HeaderValidator::Validate(
    const Request& request) const noexcept {
  const auto method = request.GetMethod();

  if (method != Method::Post && method != Method::Put) {
    return HeaderValidationStatus::Ok;
  }

  const auto content_length_opt = request.GetHeaders().content_length;

  if (content_length_opt) {
    if (content_length_opt > max_payload_) {
      return HeaderValidationStatus::PayloadTooLarge;
    }

    if (request.IsChunked()) {
      return HeaderValidationStatus::
          BothTransferEncodingAndContentLengthProvided;
    }

    return HeaderValidationStatus::Ok;
  }

  if (!request.IsChunked()) {
    return HeaderValidationStatus::ContentLengthRequired;
  }

  return HeaderValidationStatus::Ok;
}

}  // namespace fileserver::http1