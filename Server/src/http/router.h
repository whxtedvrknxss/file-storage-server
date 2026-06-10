#pragma once

#include <cstddef>
#include <functional>
#include <expected>
#include <system_error>

#include "request.h"

namespace fileserver::connection {
class Session;
}

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

using HandlerCallback =
    std::function<asio::awaitable<std::expected<void, std::error_code>>(
        std::shared_ptr<connection::Session>, std::string_view)>;

class Router {
 public:
  explicit Router(std::size_t max_allowed_payload);

  [[nodiscard]] RouteResult AnalyzeHeaders(
      const Request& request) const noexcept;

  void AddRoute(Method method, std::string path, HandlerCallback handler);

 private:
  std::size_t max_payload_;
  std::unordered_map<Method, HandlerCallback> routes_;
};

}  // namespace fileserver::http
