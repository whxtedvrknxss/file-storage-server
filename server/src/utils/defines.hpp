#pragma once

#include <expected>
#include <system_error>

namespace fileserver::utils {

template <typename T>
using ExpectedErrc = std::expected<T, std::error_code>;

}
