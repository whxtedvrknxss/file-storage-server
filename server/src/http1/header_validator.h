#pragma once

#include "request.h"

namespace fileserver::http1 {

enum class HeaderValidationStatus : std::uint8_t {
  Ok,
  PayloadTooLarge,
  ContentLengthRequired,
  BothTransferEncodingAndContentLengthProvided,
};

class HeaderValidator {
 public:
  HeaderValidator(std::size_t max_payload) : max_payload_{max_payload} {}

  [[nodiscard]] HeaderValidationStatus Validate(
      const Request& request) const noexcept;

 private:
  std::size_t max_payload_;
};

}  // namespace fileserver::http1
