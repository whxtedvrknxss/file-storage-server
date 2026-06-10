#include "router.h"

namespace fileserver::http {

Router::Router(std::size_t max_allowed_payload)
    : max_payload_{max_allowed_payload} {}

RouteResult Router::AnalyzeHeaders(const Request& request) const noexcept {
  if (request.GetMethod() == Method::Post ||
      request.GetMethod() == Method::Put) {
    const Header* content_length = request.GetHeader("content-length");
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

void Router::AddRoute(Method method, std::string path,
                      HandlerCallback handler) {}

}  // namespace fileserver::http
