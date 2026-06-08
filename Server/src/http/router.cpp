#include "router.h"

namespace fileserver::http {

RouteResult Router::AnalyzeHeaders(const Request& request) const noexcept {
  if (request.GetMethod() == Method::Post ||
      request.GetMethod() == Method::Put) {
    const Header* content_length = request.GetHeader("Content-Length");
    if (!content_length && !request.IsChunked()) {
      return RouteResult{.status = AnalysisStatus::LengthRequired,
                         .error_message = "Missing length for write operation"};
    }

    if (content_length) {
      if (std::size_t length = std::stoull(content_length->value);
          length > max_payload_) {
        return RouteResult{
            .status = AnalysisStatus::PayloadTooLarge,
            .error_message = "Upload target size exceeds the limit"};
      }
    }
  }

  return RouteResult{.status = AnalysisStatus::Valid,
                     .error_message = std::nullopt};
}

}  // namespace fileserver::http
