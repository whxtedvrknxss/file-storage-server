#pragma once

#include <cstddef>
#include <functional>
#include <expected>
#include <system_error>

#include "request.h"

namespace fileserver::connection {
class Session;
}

namespace fileserver::http1 {

// GET
// - /storage/list/{directory}
// - /storage/download/{filename}.{ext}

// HEAD
// - /storage/download/{filename}.{ext}

// PUT
// - /storage/upload/{filename}.{ext}

// POST
// - /storage/append/{filename}.{ext}

// DELETE
// - /storage/remove/{filename}.{ext}

using RouteHandler =
    std::function<asio::awaitable<std::expected<void, std::error_code>>(
        std::shared_ptr<connection::Session>, std::string_view)>;

struct RouteResult {
  RouteHandler handler;
  std::string_view target;
};

class Router {
 public:
  explicit Router(std::size_t max_allowed_payload);

  [[nodiscard]] std::optional<RouteResult> Resolve(Method method,
                                                   std::string_view url) const;
  void RegisterRoute(Method method, std::string_view prefix,
                     RouteHandler handler);

 private:
  struct RouteEntry {
    std::string_view prefix;
    RouteHandler handler;
  };

  std::size_t max_payload_;
  std::unordered_map<Method, std::vector<RouteEntry>> routes_;
};

}  // namespace fileserver::http1
