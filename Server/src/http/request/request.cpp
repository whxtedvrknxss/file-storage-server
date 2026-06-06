#include "request.h"

namespace fileserver::http {

const Header* Request::GetHeader(std::string_view name) const {
  auto it = std::ranges::find_if(
      headers_, [&name](const Header& header) { return header.name == name; });

  if (it == headers_.end()) {
    return nullptr;
  }

  return &(*it);
}

}  // namespace fileserver::http
