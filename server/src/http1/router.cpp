#include "router.h"

namespace fileserver::http1 {

Router::Router(std::size_t max_allowed_payload)
    : max_payload_{max_allowed_payload} {}

void Router::RegisterRoute(Method method, std::string_view prefix,
                           RouteHandler handler) {}

}  // namespace fileserver::http1
