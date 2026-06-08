#pragma once

#include <cstddef>

#include "request.h"

namespace fileserver::http {

enum class AnalysisStatus : std::uint8_t {
  Valid,
  PayloadTooLarge,
  LengthRequired,
  BadRequest,
  MethodNotAllowed,
  InvalidPath
};

struct RouteResult {
  AnalysisStatus status = AnalysisStatus::Valid;
  std::optional<std::string> error_message;
};

class Router {
 public:
  explicit Router(std::size_t max_allowed_payload)
      : max_payload_{max_allowed_payload} {}

  [[nodiscard]] RouteResult AnalyzeHeaders(
      const Request& request) const noexcept;

 private:
  std::size_t max_payload_;
};

}  // namespace fileserver::http
