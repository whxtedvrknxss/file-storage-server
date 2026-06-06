#pragma once

#include <cstddef>

#include "builder.h"

namespace fileserver::http {

enum class AnalysisStatus : std::uint8_t {
  Valid,
  PayloadTooLarge,
  LengthRequired,
  BadRequest
};

struct AnalysisResult {
  AnalysisStatus status = AnalysisStatus::Valid;
  std::optional<std::string> error_message;
};

class RequestAnalyzer {
 public:
  explicit RequestAnalyzer(std::size_t max_allowed_payload)
      : max_payload_(max_allowed_payload) {}

  [[nodiscard]] AnalysisResult AnalyzeHeaders(const Request& request) const;

 private:
  std::size_t max_payload_;
};

}  // namespace fileserver::http
